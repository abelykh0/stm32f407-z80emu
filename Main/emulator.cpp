#include "Emulator.h"

#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "m4vgalib/vga.h"
#include "m4vgalib/timing.h"
#include "etl/stm32f4xx/gpio.h"
#include "Keyboard/ps2Keyboard.h"
#include "Emulator/z80main.h"
#include "Emulator/z80snapshot.h"
#include "Emulator/z80emu/z80emu.h"
#include "resources/keyboard.h"

using namespace etl::stm32f4xx;

#define DEBUG_BAND_HEIGHT (DEBUG_ROWS * 8 * 2)

extern RTC_HandleTypeDef hrtc;

uint8_t _buffer16K_1[0x4000];
uint8_t _buffer16K_2[0x4000];

// Debug screen video RAM
// DEBUG_COLUMNS x DEBUG_ROWS characters
static uint8_t  _debugPixels[52 * 8 * DEBUG_ROWS]; // number of text columns must be divisible by 4
static uint16_t _debugAttributes[52 * DEBUG_ROWS]; // number of text columns must be divisible by 4
static uint8_t  _debugBorderColor;

// Spectrum video RAM + border color
// 256x192 pixels (or 32x24 characters)
static SpectrumScreenData _spectrumScreenData;

// Used in saveState / restoreState
static SpectrumScreenData* _savedScreenData = (SpectrumScreenData*)&_buffer16K_2[0x4000 - sizeof(SpectrumScreenData)];

// Debug band
static VideoSettings _videoSettings {
	&vga::timing_800x600_56hz, // Timing
	2,  // Scale
	DEBUG_COLUMNS, DEBUG_ROWS, _debugPixels, _debugAttributes,
	&_debugBorderColor
};
static uint16_t _spectrumBandHeight = _videoSettings.Timing->video_end_line
		- _videoSettings.Timing->video_start_line - DEBUG_BAND_HEIGHT;
Screen DebugScreen(_videoSettings, _spectrumBandHeight, DEBUG_BAND_HEIGHT);
static vga::Band _debugBand {
	&DebugScreen, DEBUG_BAND_HEIGHT, nullptr
};

// Spectrum screen band
static VideoSettings _spectrumVideoSettings {
	&vga::timing_800x600_56hz, // Timing
	2,  // Scale
	32, 24, _spectrumScreenData.Pixels, _spectrumScreenData.Attributes, &_spectrumScreenData.BorderColor
};
SpectrumScreen MainScreen(_spectrumVideoSettings, 0, _spectrumBandHeight);
static vga::Band _band {
	&MainScreen, _spectrumBandHeight, &_debugBand
};

static bool _showingKeyboard;
static bool _settingDateTime;
static uint32_t _frames;
static char* _newDateTime = (char*)_buffer16K_2;
static bool _helpShown;

void initializeVideo()
{
	vga::init();

	// This changes the CPU clock speed
	vga::configure_timing(*_videoSettings.Timing);

	// Inform HAL that the CPU clock speed changed
	SystemCoreClockUpdate();
}

void startVideo()
{
	DebugScreen.Clear();
	MainScreen.Clear();
	vga::configure_band_list(&_band);
	vga::video_on();
}

void showErrorMessage(const char* errorMessage)
{
	DebugScreen.SetAttribute(0x0310); // red on blue
	DebugScreen.PrintAlignCenter(2, errorMessage);
	DebugScreen.SetAttribute(0x3F10); // white on blue
}

void showTitle(const char* title)
{
	DebugScreen.SetAttribute(0x3F00); // white on black
	DebugScreen.PrintAlignCenter(0, title);
	DebugScreen.SetAttribute(0x3F10); // white on blue
}

void showKeyboardSetup()
{
	saveState();
	_showingKeyboard = true;

	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();
	DebugScreen.PrintAlignCenter(2, "Press any key to return");

	MainScreen.ShowScreenshot(spectrumKeyboard);
	_spectrumScreenData.BorderColor = 0; // Black
}

bool showKeyboardLoop()
{
	if (!_showingKeyboard)
	{
		return false;
	}

	int32_t scanCode = Ps2_GetScancode();
	if (scanCode == 0 || (scanCode & 0xFF00) != 0xF000)
	{
		return true;
	}

	_showingKeyboard = false;
	restoreState(true);
	return false;
}

void toggleHelp()
{
	if (_helpShown)
	{
		clearHelp();
	}
	else
	{
		showHelp();
	}
}

void clearHelp()
{
	DebugScreen.HideCursor();
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();

	_helpShown = false;
}

void showHelp()
{
	DebugScreen.HideCursor();
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();

#ifdef BOARD2
	DebugScreen.PrintAt(0, 0, "F1  - show / hide help");
	DebugScreen.PrintAt(0, 1, "F3  - load snapshot from flash");
	DebugScreen.PrintAt(0, 2, "F5  - reset");
	DebugScreen.PrintAt(0, 3, "F10 - show keyboard layout");
	DebugScreen.PrintAt(0, 4, "F12 - show registers");
#else
	DebugScreen.PrintAt(0, 0, "F1  - show / hide help");
	DebugScreen.PrintAt(0, 1, "F2  - save snapshot to SD card");
	DebugScreen.PrintAt(0, 2, "F3  - load snapshot from SD card");
	DebugScreen.PrintAt(0, 3, "F5  - reset");
	DebugScreen.PrintAt(0, 4, "F6  - set date and time");
	DebugScreen.PrintAt(0, 5, "F10 - show keyboard layout");
	DebugScreen.PrintAt(0, 6, "F12 - show registers");
#endif

	_helpShown = true;
}

void restoreHelp()
{
	if (_helpShown)
	{
		showHelp();
	}
	else
	{
		clearHelp();
	}
}

void setDateTimeSetup()
{
	_settingDateTime = true;

	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();
	showTitle("Set Date and Time. ENTER, ESC, BS");

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	char* formattedDateTime = (char*)_buffer16K_1;
	sprintf(formattedDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
			date.Year + 2000, date.Month, date.Date,
			time.Hours, time.Minutes, time.Seconds);
	DebugScreen.PrintAlignCenter(2, formattedDateTime);

	_frames = DebugScreen._frames + 5;

	DebugScreen.PrintAt(0, 4, "Enter new date and time (yyyy-mm-dd hh:mm:ss):");
	DebugScreen.SetCursorPosition(0, 5);
	DebugScreen.ShowCursor();
	memset(_newDateTime, 0, 20);
}

bool setDateTimeLoop()
{
	if (!_settingDateTime)
	{
		return false;
	}

	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	uint8_t x = DebugScreen._cursor_x;

	if (DebugScreen._frames > _frames)
	{
		HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
		char* formattedDateTime = (char*)_buffer16K_1;
		sprintf(formattedDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
				date.Year + 2000, date.Month, date.Date,
				time.Hours, time.Minutes, time.Seconds);
		DebugScreen.PrintAlignCenter(2, formattedDateTime);
		DebugScreen.SetCursorPosition(x, 5);
		DebugScreen.ShowCursor();
		_frames = DebugScreen._frames + 5;
	}

	int32_t scanCode = Ps2_GetScancode();
	if (scanCode == 0 || (scanCode & 0xFF00) == 0xF000)
	{
		return true;
	}

	int year, month, day;
	int hour, minute, second;

	scanCode = ((scanCode & 0xFF0000) >> 8 | (scanCode & 0xFF));
	switch (scanCode)
	{
	case KEY_BACKSPACE:
		if (DebugScreen._cursor_x > 0)
		{
			DebugScreen.PrintAt(DebugScreen._cursor_x - 1, DebugScreen._cursor_y, " ");
			DebugScreen.SetCursorPosition(DebugScreen._cursor_x - 1, DebugScreen._cursor_y);
			_newDateTime[DebugScreen._cursor_x] = '\0';
		}
		break;

	case KEY_ENTER:
	case KEY_KP_ENTER:
		DebugScreen.HideCursor();
		if (sscanf(_newDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
				&year, &month, &day,
				&hour, &minute, &second) == 6)
		{
			HAL_RTC_Init(&hrtc);

			memset(&time, 0, sizeof(time));
			time.Hours = hour;
			time.Minutes = minute;
			time.Seconds = second;
			HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);

			memset(&date, 0, sizeof(date));
			date.Year = year - 2000;
			date.Month = month;
			date.Date = day;
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);

			_settingDateTime = false;
			restoreState(false);
			return false;
		}
		else
		{
			DebugScreen.SetAttribute(0x0310); // red on blue
			DebugScreen.PrintAt(0, 7, "Invalid date and time");
			DebugScreen.SetAttribute(0x3F10); // white on blue
			DebugScreen.SetCursorPosition(x, 5);
			DebugScreen.ShowCursor();
		}
		break;

	case KEY_ESC:
		_settingDateTime = false;
		restoreState(false);
		return false;

	default:
		char character = Ps2_ConvertScancode(scanCode);
		if (DebugScreen._cursor_x < 20 && character != '\0')
		{
			char* text = (char*)_buffer16K_1;
			text[0] = character;
			_newDateTime[DebugScreen._cursor_x] = character;
			text[1] = '\0';
			DebugScreen.Print(text);
		}
		break;
	}

	return true;
}

void saveState()
{
	*_savedScreenData = _spectrumScreenData;
}

void restoreState(bool restoreScreen)
{
	if (restoreScreen)
	{
		_spectrumScreenData = *_savedScreenData;
	}

	restoreHelp();
}

void showRegisters()
{
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();
	showTitle("Registers. ESC - clear");

    char* buf = (char*)_buffer16K_1;

    sprintf(buf, "PC %04x  AF %04x  AF' %04x  I %02x",
        _zxCpu.pc, _zxCpu.registers.word[Z80_AF],
        _zxCpu.alternates[Z80_AF], _zxCpu.i);
    DebugScreen.PrintAlignCenter(2, buf);
    sprintf(buf, "SP %04x  BC %04x  BC' %04x  R %02x",
        _zxCpu.registers.word[Z80_SP], _zxCpu.registers.word[Z80_BC],
        _zxCpu.alternates[Z80_BC], _zxCpu.r);
    DebugScreen.PrintAlignCenter(3, buf);
    sprintf(buf, "IX %04x  DE %04x  DE' %04x  IM %x",
        _zxCpu.registers.word[Z80_IX], _zxCpu.registers.word[Z80_DE],
        _zxCpu.alternates[Z80_DE], _zxCpu.im);
    DebugScreen.PrintAlignCenter(4, buf);
    sprintf(buf, "IY %04x  HL %04x  HL' %04x      ",
        _zxCpu.registers.word[Z80_IY], _zxCpu.registers.word[Z80_HL],
        _zxCpu.alternates[Z80_HL]);
    DebugScreen.PrintAlignCenter(5, buf);
}
