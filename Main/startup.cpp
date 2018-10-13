#include "startup.h"

#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "m4vgalib/vga.h"
#include "m4vgalib/timing.h"

#include "etl/stm32f4xx/gpio.h"
#include <Display/Screen.h>
#include "fatfs.h"

#include "Emulator/z80main.h"
#include <Emulator/SpectrumScreen.h>
#include <Keyboard/ps2Keyboard.h>
#include <resources/keyboard.h>

using namespace etl::stm32f4xx;
using namespace Display;

// Debug band 50 x 5 characters
#define TEXT_COLUMNS 50
#define TEXT_ROWS 8
uint8_t _debugPixels[52 * 8 * TEXT_ROWS];
uint16_t _debugAttributes[52 * TEXT_ROWS];
uint8_t _debugBorderColor;
uint16_t _debugBandHeight = TEXT_ROWS * 8 * 2;
VideoSettings _videoSettings {
		&vga::timing_800x600_56hz, // Timing
		2,  // Scale
		TEXT_COLUMNS, TEXT_ROWS, _debugPixels, _debugAttributes, &_debugBorderColor };
uint16_t _spectrumBandHeight = _videoSettings.Timing->video_end_line - _videoSettings.Timing->video_start_line - _debugBandHeight;
Screen _debugScreen(_videoSettings, _spectrumBandHeight, _debugBandHeight);
vga::Band _debugBand { &_debugScreen, _debugBandHeight, nullptr };

// Spectrum screen band
uint8_t _pixels[32 * 8 * 24];
uint16_t _attributes[32 * 24];
uint8_t _borderColor;
VideoSettings _spectrumVideoSettings {
		&vga::timing_800x600_56hz, // Timing
		2,  // Scale
		32, 24, _pixels, _attributes, &_borderColor };
SpectrumScreen _spectrumScreen(_spectrumVideoSettings, 0, _spectrumBandHeight);
vga::Band _band { &_spectrumScreen, _spectrumBandHeight, &_debugBand };

extern "C" void initialize() {
	vga::init();

	// This changes the CPU clock speed
	vga::configure_timing(*_videoSettings.Timing);

	// Inform HAL that the CPU clock speed changed
	SystemCoreClockUpdate();
}

extern "C" void setup() {
	_debugScreen.Clear();
	vga::configure_band_list(&_band);
	vga::video_on();

//	int row = 1;
//	FRESULT mountResult = f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);
//	if (mountResult == FR_OK) {
//		FRESULT fr; /* Return value */
//		DIR dj; /* Directory search object */
//		FILINFO fno; /* File information */
//
//		fr = f_findfirst(&dj, &fno, (TCHAR const*) "/", (TCHAR const*) "*.z80");
//
//		TCHAR* fileName = fno.fname;
//		_debugScreen.PrintAt(1, row, fileName);
//		row++;
//
//		while (fr == FR_OK && fno.fname[0]) { /* Repeat while an item is found */
//			fr = f_findnext(&dj, &fno);
//
//			fileName = fno.fname;
//			_debugScreen.PrintAt(1, row, fileName);
//
//			row++;
//		}
//
//		f_mount(&SDFatFS, (TCHAR const*) SDPath, 0);
//	}
//
//	_spectrumScreen.ShowScreenshot(spectrumKeyboard);

	// Initialize PS2 Keyboard
	Ps2_Initialize();

	zx_setup(&_spectrumScreen);

	// Initialize GPIOA
	//rcc.enable_clock(AhbPeripheral::gpioa);
	//gpioa.set_mode(Gpio::p6, Gpio::Mode::gpio);
}

extern "C" void loop() {
	zx_loop();

	// Blink LED on PA6
	//gpioa.toggle(Gpio::p6);

	// Delay 1000 ms
	//HAL_Delay(1000);

//    int32_t scanCode = Ps2_GetScancode();
//    if (scanCode > 0)
//    {
//    	char buf[30];
//        if ((scanCode & 0xFF00) == 0xF000)
//        {
//            // key up
//            scanCode = ((scanCode & 0xFF0000) >> 8 | (scanCode & 0xFF));
//
//            sprintf(buf, "kup %04lx", scanCode);
//            _screen.PrintAt(1, 5, buf);
//            //SetRowAttributes(1, 0x103F); // blue on white
//
//            _band.rasterizer = &_spectrumScreen;
//            _spectrumScreen.ShowScreenshot(spectrumKeyboard);
//        }
//        else
//        {
//            // key down
//            sprintf(buf, "kdn %04lx", scanCode);
//            _screen.PrintAt(1, 6, buf);
//        }
//    }

}
