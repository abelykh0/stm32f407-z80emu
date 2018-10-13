#include <string.h>
#include <stdint.h>

#include "startup.h"
#include "Emulator.h"
#include "Emulator/z80main.h"
#include "Keyboard/ps2Keyboard.h"

extern "C" void initialize() {
	initializeVideo();
}

extern "C" void setup() {
	startVideo();

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

	zx_setup(&MainScreen);

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
