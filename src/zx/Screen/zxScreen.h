#ifndef _ZXSCREEN_H_
#define _ZXSCREEN_H_

#include <stdint.h>

// Horizontal resolution
#define HSIZE_CHARS 32
#define HSIZE_PIXELS (HSIZE_CHARS * 8)

// Vertical resolution
#define VSIZE_CHARS 24
#define VSIZE_PIXELS (VSIZE_CHARS * 8)

#define BITMAP_SIZE (HSIZE_CHARS * VSIZE_PIXELS)
#define COLORS_SIZE (HSIZE_CHARS * VSIZE_CHARS)

namespace zx
{
extern volatile uint8_t VideoMemoryPixels[];
extern volatile uint16_t VideoMemoryColors[];
extern volatile uint8_t BorderColor;

extern uint8_t BackupMemoryPixels[];
extern uint16_t BackupMemoryColors[];
extern uint8_t BackupBorderColor;

volatile uint8_t *GetBitmapAddress(uint8_t vline);
volatile uint8_t *GetBitmapAddress(uint8_t vline, uint8_t character);
uint16_t ConvertSinclairColor(uint8_t sinclairColor);
uint8_t ConvertToSinclairColor(uint16_t color);

void clear_screen(uint16_t color);
void save_screen();
void restore_screen();
void showSinclairScreenshot(const char *screenshot);

void Draw4(uint8_t *bitmap, uint16_t *colors, uint8_t *dest);

} // namespace zx

#endif