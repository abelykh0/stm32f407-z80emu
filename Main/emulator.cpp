#include "Emulator.h"

#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "m4vgalib/vga.h"
#include "m4vgalib/timing.h"
#include "etl/stm32f4xx/gpio.h"
#include "Keyboard/ps2Keyboard.h"
#include "Emulator/z80snapshot.h"
#include "resources/keyboard.h"

using namespace etl::stm32f4xx;

uint8_t _buffer16K_1[0x4000];
uint8_t _buffer16K_2[0x4000];

uint8_t  _debugPixels[52 * 8 * DEBUG_ROWS]; // number of text columns must be divisible by 4
uint16_t _debugAttributes[52 * DEBUG_ROWS]; // number of text columns must be divisible by 4
uint8_t  _debugBorderColor;

// 256x192 (or 32x24 characters)
uint8_t  _pixels[32 * 8 * 24];
uint16_t _attributes[32 * 24];
uint8_t  _borderColor;

// Debug band
uint16_t _debugBandHeight = DEBUG_ROWS * 8 * 2;
VideoSettings _videoSettings {
	&vga::timing_800x600_56hz, // Timing
	2,  // Scale
	DEBUG_COLUMNS, DEBUG_ROWS, _debugPixels, _debugAttributes,
	&_debugBorderColor
};
uint16_t _spectrumBandHeight = _videoSettings.Timing->video_end_line
		- _videoSettings.Timing->video_start_line - _debugBandHeight;
Screen DebugScreen(_videoSettings, _spectrumBandHeight, _debugBandHeight);
vga::Band _debugBand {
	&DebugScreen, _debugBandHeight, nullptr
};

// Spectrum screen band
VideoSettings _spectrumVideoSettings {
	&vga::timing_800x600_56hz, // Timing
	2,  // Scale
	32, 24, _pixels, _attributes, &_borderColor
};
SpectrumScreen MainScreen(_spectrumVideoSettings, 0, _spectrumBandHeight);
vga::Band _band {
	&MainScreen, _spectrumBandHeight, &_debugBand
};

uint8_t readBuffer[_MIN_SS];
bool _showingKeyboard;

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
	_showingKeyboard = true;
	MainScreen.ShowScreenshot(spectrumKeyboard);
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
		_showingKeyboard = false;
		return false;
	}

	return true;
}

void showHelp()
{
	DebugScreen.SetAttribute(0x3F10); // white on blue
	DebugScreen.Clear();

	DebugScreen.PrintAt(0, 1, "F1  - show this screen");
	DebugScreen.PrintAt(0, 1, "F2  - load snapshot from SD card");
	DebugScreen.PrintAt(0, 1, "F10 - show keyboard layout");
}
