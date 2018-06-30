
/*
	Pinout for VGA
	==============

	PB6 - HSync
	PB7 - VSync

	PA0, PA1 - Red
	PA2, PA3 - Green
	PA4, PA5 - Blue

	Resistors
	=========
	3 * 680 Ohm on PA0, PA2, PA4 (0.33 V)
	3 * 470 Ohm on PA1, PA2, PA3

	Pinout for PS/2 Keyboard
	========================

	PB14 - CLK
	PB15 - DATA

	Resistors
	=========
	2 * 2K2 and 2 * 3K3 to build a primitive
    logic level converter from 5V to 3.3V

*/

#include <string.h>
#include "startup.h"
#include "m4vgalib/vga.h"
#include "m4vgalib/timing.h"
#include "etl/stm32f4xx/rcc.h"

#include "zx/z80main.h"
#include "zx/Screen/zxScreen.h"
#include "zx/Screen/zxRasterizer.h"
#include "zx/Keyboard/ps2Keyboard.h"
#include "zx/Keyboard/z80Input.h"
#include "zx/z80snapshot.h"
#include "zxResources/keyboard.h"

// Sinclair snapshots
#include "zxResources/testprogram.h"
#include "zxResources/hydrofool.h"
#include "zxResources/circles.h"
#include "zxResources/bubblebobble.h"

#include "debug/debugRasterizer.h"
#include "debug/debugBand.h"
extern Z80_STATE _zxCpu;

using namespace etl::stm32f4xx;

//vga::Timing _timing = vga::timing_vesa_640x480_60hz;
vga::Timing _timing = vga::timing_vesa_800x600_60hz;
//vga::Timing _timing = vga::timing_800x600_56hz;

#define DEBUG_BAND 

// ZX Screen and debug bands
#ifdef DEBUG_BAND
uint16_t debugWidthInCharacters = _timing.video_pixels / 16;
uint16_t zxHeight = _timing.video_end_line - _timing.video_start_line - (DEBUG_VSIZE_CHARS * 16);
debug::DebugRasterizer debugRasterizer(_timing.video_pixels, zxHeight);
vga::Band debugBand { &debugRasterizer, DEBUG_VSIZE_CHARS * 16, nullptr };
zx::ZXRasterizer z80rasterizer(2, _timing.video_pixels, zxHeight);
vga::Band band { &z80rasterizer, zxHeight, &debugBand };
#else
uint16_t zxHeight = _timing.video_end_line - _timing.video_start_line;
zx::ZXRasterizer z80rasterizer(2, _timing.video_pixels, zxHeight);
vga::Band band { &z80rasterizer, zxHeight, nullptr };
#endif

bool keyboardShown = false;

void process_special_key(int32_t scanCode);

void setup()
{
    FpuInit();
    //RtcInit();

    vga::init();
    vga::configure_timing(_timing);

    vga::configure_band_list(&band);
    vga::video_on();

#ifdef DEBUG_BAND
    char buf[40];
    sprintf(buf, "%dx%d", _timing.video_pixels, _timing.video_end_line - _timing.video_start_line);
    debug_print_right(debugWidthInCharacters, 2, buf);
    sprintf(buf, "CPU @ %d Mhz", (int)(HAL_RCC_GetSysClockFreq() / 1000000));
    debug_print_right(debugWidthInCharacters, 3, buf);
#endif

    zx_setup();
}

void loop()
{
    int32_t specialKey = zx_loop();

    if (specialKey > -1)
    {
        process_special_key(specialKey);
    }
}

void process_special_key(int32_t scanCode)
{ 
    char buf[40];

    switch (scanCode)
    {
        case KEY_ESC:
            // Ensure no keys pressed
            memset(indata, 0xFF, 128);
            break;
        case KEY_F1:
            // Toggle keyboard
            if (keyboardShown)
            {
                zx::restore_screen();
            }
            else
            {
                zx::save_screen();
                zx::showSinclairScreenshot(zxKeyboard);
            }
            keyboardShown = !keyboardShown;
            break;
        case KEY_F2:
            // Save screen
            zx::save_screen();
            break;
        case KEY_F3:
            // Restore saved screen
            zx::restore_screen();
            break;
        case KEY_F4:
            zx_reset();
            break;
        case KEY_F5:
            zx::ReadZ80Format((uint8_t*)testProgram);
            break;
        case KEY_F6:
            zx::ReadZ80Format((uint8_t*)hydrofool);
            break;
        case KEY_F7:
            zx::ReadZ80Format((uint8_t*)circles);
            break;
        case KEY_F10:
            zx::ReadZ80Format((uint8_t*)BubbleBobble);
            break;
        case KEY_F11:
            sprintf(buf, "%04x", indata[0]); 
            debug_print_right(debugWidthInCharacters, 0, buf);
            break;
        case KEY_F12:
            // display state
            sprintf(buf, "PC %04x  AF %04x  AF' %04x  I %02x", 
                _zxCpu.pc, _zxCpu.registers.word[Z80_AF], 
                _zxCpu.alternates[Z80_AF], _zxCpu.i);
            debug_print_right(debugWidthInCharacters, 0, buf);
            sprintf(buf, "SP %04x  BC %04x  BC' %04x  R %02x", 
                _zxCpu.registers.word[Z80_SP], _zxCpu.registers.word[Z80_BC], 
                _zxCpu.alternates[Z80_BC], _zxCpu.r);
            debug_print_right(debugWidthInCharacters, 1, buf);
            sprintf(buf, "IX %04x  DE %04x  DE' %04x  IM %x", 
                _zxCpu.registers.word[Z80_IX], _zxCpu.registers.word[Z80_DE], 
                _zxCpu.alternates[Z80_DE], _zxCpu.im);
            debug_print_right(debugWidthInCharacters, 2, buf);
            sprintf(buf, "IY %04x  HL %04x  HL' %04x      ", 
                _zxCpu.registers.word[Z80_IY], _zxCpu.registers.word[Z80_HL], 
                _zxCpu.alternates[Z80_HL]);
            debug_print_right(debugWidthInCharacters, 3, buf);

            break;
    }
}
