#include <string.h>

#include "zxScreen.h"

volatile uint8_t zx::VideoMemoryPixels[BITMAP_SIZE];
volatile uint16_t zx::VideoMemoryColors[COLORS_SIZE];
volatile uint8_t zx::BorderColor;

uint8_t zx::BackupMemoryPixels[BITMAP_SIZE];
uint16_t zx::BackupMemoryColors[COLORS_SIZE];
uint8_t zx::BackupBorderColor;

void zx::save_screen()
{
    memcpy(BackupMemoryPixels, (const void*)VideoMemoryPixels, BITMAP_SIZE);
    memcpy(BackupMemoryColors, (const void*)VideoMemoryColors, COLORS_SIZE * sizeof(uint16_t));
    BackupBorderColor = BorderColor;
}

void zx::restore_screen()
{
    memcpy((void*)VideoMemoryPixels, BackupMemoryPixels, BITMAP_SIZE);
    memcpy((void*)VideoMemoryColors, BackupMemoryColors, COLORS_SIZE * sizeof(uint16_t));
    BorderColor = BackupBorderColor;
}

void zx::showSinclairScreenshot(const char *screenshot)
{
    memcpy((void*)VideoMemoryPixels, screenshot, BITMAP_SIZE);
    for (uint32_t i = 0; i < COLORS_SIZE; i++)
    {
        zx::VideoMemoryColors[i] = zx::ConvertSinclairColor(screenshot[BITMAP_SIZE + i]);
    }
}

volatile uint8_t* zx::GetBitmapAddress(uint8_t vline)
{
    // ZX Sinclair addressing
    // 00-00-00-Y7-Y6-Y2-Y1-Y0 Y5-Y4-Y3-x4-x3-x2-x1-x0
    //          12 11 10  9  8  7  6  5  4  3  2  1  0 

    uint32_t y012 = ((vline & 0B00000111) << 8);
    uint32_t y345 = ((vline & 0B00111000) << 2);
    uint32_t y67 =  ((vline & 0B11000000) << 5);
    return &zx::VideoMemoryPixels[y012 | y345 | y67];
}

volatile uint8_t* zx::GetBitmapAddress(uint8_t vline, uint8_t character)
{
    character &= 0B00011111;
    return zx::GetBitmapAddress(vline) + character;
}

uint16_t zx::ConvertSinclairColor(uint8_t sinclairColor)
{
    // Sinclair: Flash-Bright-PaperG-PaperR-PaperB-InkG-InkR-InkB
    //               7      6      5      4      3    2    1    0
    // Our colors: 00-PaperB01-PaperG01-PaperR01 : 00-InkB01-InkG01-InkR01
    //                      54       32       10 :        54     32     10 

    bool bright = ((sinclairColor & 0B01000000) != 0);

    uint16_t ink = ((sinclairColor & 0B00000100) << 8); // InkG
    ink |= ((sinclairColor & 0B00000010) << 7);         // InkR
    ink |= ((sinclairColor & 0B00000001) << 12);        // InkB
    if (bright)
    {
        ink |= (ink << 1);
    }
    
    uint16_t paper = ((sinclairColor & 0B00100000) >> 3); // PaperG
    paper |= ((sinclairColor & 0B00010000) >> 4);         // PaperR
    paper |= ((sinclairColor & 0B00001000) << 1);         // PaperB
    if (bright)
    {
        paper |= (paper << 1);
    }

	uint16_t result = ink | paper;

    if (bright)
    {
        // This is only needed to correctly read back "bright black" color
		result |= 0x4040;
    }

	if ((sinclairColor & 0B10000000) != 0)
	{
		// Blink
		result |= 0x8080;
	}

	return result;
}

uint8_t zx::ConvertToSinclairColor(uint16_t color)
{
    // Our colors: 00-PaperB01-PaperG01-PaperR01 : 00-InkB01-InkG01-InkR01
    //                      54       32       10 :        54     32     10 
    // Sinclair: Flash-Bright-PaperG-PaperR-PaperB-InkG-InkR-InkB
    //               7      6      5      4      3    2    1    0

    uint8_t result = 0;
    if ((color & 0x4040) != 0)
    {
        // Bright
        result |= 0B01000000;
    }

	if ((color & 0x8080) != 0)
	{
		// Blink
		result |= 0B10000000;
	}

	result |= ((color & 0B00010000) >> 1); // PaperB
	result |= ((color & 0B00000001) << 4); // PaperR
	result |= ((color & 0B00000100) << 3); // PaperG
	
    color >>= 8;
	result |= ((color & 0B00010000) >> 4); // InkB
	result |= ((color & 0B00000001) << 1); // InkR
	result |= (color & 0B00000100);        // InkG

    return result;
}

void zx::clear_screen(uint16_t color)
{
    memset((void *)zx::VideoMemoryPixels, 0, BITMAP_SIZE);
    for (int i = 0; i < COLORS_SIZE; i++)
    {
        zx::VideoMemoryColors[i] = color;
    }
    zx::BorderColor = (uint8_t)color;
}

void zx::Draw4(uint8_t *bitmap, uint16_t *colors, uint8_t *dest)
{
    __asm__ volatile(
        "  ldr r1, [%[pix]], #4 \n\t" // pixels for characters 0..3
        "  ldr r3, [%[col]], #4 \n\t" // colors for characters 0..1

        // character #0
        "  ror r1, r1, #4    \n\t" // pixels >> 4
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".rept 7 \n\t"
        "  ror r1, r1, #31   \n\t" // pixels << 1
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".endr   \n\t"

        // character #1
        "  ror r3, r3, #16   \n\t" // colors
        "  ror r1, r1, #15   \n\t" // pixels >> 15
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".rept 7 \n\t"
        "  ror r1, r1, #31   \n\t" // pixels << 1
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".endr   \n\t"

        "  ldr r3, [%[col]], #4 \n\t" // colors for next 2 characters

        // character #2
        "  ror r1, r1, #15   \n\t" // pixels >> 15
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".rept 7 \n\t"
        "  ror r1, r1, #31   \n\t" // pixels << 1
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".endr   \n\t"

        // character #3
        "  ror r3, r3, #16   \n\t" // colors
        "  ror r1, r1, #15   \n\t" // pixels >> 15
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".rept 7 \n\t"
        "  ror r1, r1, #31   \n\t" // pixels << 1
        "  and r0, r1, #8    \n\t"
        "  lsr r0, r3, r0    \n\t"
        "  strb r0, [%[odr]], #1 \n\t"
        ".endr   \n\t"

        :
        : [pix] "r"(bitmap),
          [col] "r"(colors),
          [odr] "r"(dest)
        : "r0", "r1", "r2", "r3");
}
