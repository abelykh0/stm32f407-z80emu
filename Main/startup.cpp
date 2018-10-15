#include <string.h>
#include <stdint.h>

#include "startup.h"
#include "Emulator.h"
#include "Emulator/z80main.h"
#include "Keyboard/ps2Keyboard.h"

extern "C" void initialize()
{
	initializeVideo();
}

extern "C" void setup()
{
	startVideo();

	// Initialize PS2 Keyboard
	Ps2_Initialize();

	// Initialize Spectrum emulator
	zx_setup(&MainScreen);

	//loadSnapshot("circles.z80");
}

extern "C" void loop()
{
	zx_loop();
}
