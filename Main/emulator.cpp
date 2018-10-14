#include "Emulator.h"

#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "m4vgalib/vga.h"
#include "m4vgalib/timing.h"
#include "etl/stm32f4xx/gpio.h"
#include "Keyboard/ps2Keyboard.h"

using namespace etl::stm32f4xx;

// Debug band
#define TEXT_COLUMNS 50
#define TEXT_ROWS 8
uint8_t _debugPixels[52 * 8 * TEXT_ROWS];
uint16_t _debugAttributes[52 * TEXT_ROWS];
uint8_t _debugBorderColor;
uint16_t _debugBandHeight = TEXT_ROWS * 8 * 2;
VideoSettings _videoSettings {
		&vga::timing_800x600_56hz, // Timing
		2,  // Scale
		TEXT_COLUMNS, TEXT_ROWS, _debugPixels, _debugAttributes,
		&_debugBorderColor };
uint16_t _spectrumBandHeight = _videoSettings.Timing->video_end_line
		- _videoSettings.Timing->video_start_line - _debugBandHeight;
Screen DebugScreen(_videoSettings, _spectrumBandHeight, _debugBandHeight);
vga::Band _debugBand { &DebugScreen, _debugBandHeight, nullptr };

// Spectrum screen band
uint8_t _pixels[32 * 8 * 24];
uint16_t _attributes[32 * 24];
uint8_t _borderColor;
VideoSettings _spectrumVideoSettings { &vga::timing_800x600_56hz, // Timing
		2,  // Scale
		32, 24, _pixels, _attributes, &_borderColor };
SpectrumScreen MainScreen(_spectrumVideoSettings, 0, _spectrumBandHeight);
vga::Band _band { &MainScreen, _spectrumBandHeight, &_debugBand };

uint8_t readBuffer[_MIN_SS];

void initializeVideo() {
	vga::init();

	// This changes the CPU clock speed
	vga::configure_timing(*_videoSettings.Timing);

	// Inform HAL that the CPU clock speed changed
	SystemCoreClockUpdate();
}

void startVideo() {
	DebugScreen.Clear();
	MainScreen.Clear();
	vga::configure_band_list(&_band);
	vga::video_on();
}

bool loadScreenshot(const TCHAR* fileName) {
	FRESULT mountResult = f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);
	if (mountResult == FR_OK) {
		FIL fp;

		f_open(&fp, fileName, FA_READ | FA_OPEN_EXISTING);

		UINT bytesRead;
		FRESULT readResult;

		uint8_t* buffer = _pixels;
		UINT totalBytesRead = 0;
		do {
			readResult = f_read(&fp, buffer, _MIN_SS, &bytesRead);
			totalBytesRead += bytesRead;
			buffer += bytesRead;
		} while (readResult == FR_OK && totalBytesRead < 32 * 8 * 24);

		totalBytesRead = 0;
		do {
			readResult = f_read(&fp, readBuffer, _MIN_SS, &bytesRead);
			for (uint32_t i = 0; i < bytesRead; i++)
			{
				_attributes[totalBytesRead + i] = MainScreen.FromSpectrumColor(readBuffer[i]);
			}

			totalBytesRead += bytesRead;
		} while (readResult == FR_OK && totalBytesRead < 32 * 24);

		f_close(&fp);

		f_mount(&SDFatFS, (TCHAR const*) SDPath, 0);
	}

	return true;
}

