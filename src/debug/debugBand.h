#ifndef __DEBUGBAND_H
#define __DEBUGBAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// used only 50 @ 800x600 and 40 @ 640x480
#define DEBUG_HSIZE_CHARS 52

#define DEBUG_VSIZE_CHARS 4
#define DEBUG_BITMAP_SIZE (DEBUG_HSIZE_CHARS * DEBUG_VSIZE_CHARS * 8)
#define DEBUG_COLORS_SIZE (DEBUG_HSIZE_CHARS * DEBUG_VSIZE_CHARS)

void debug_setFont(const uint8_t* font);
void debug_setColor(uint16_t color);

void debug_clear();
void debug_print(uint8_t x, uint8_t y, const char* str);
void debug_print_right(uint8_t width, uint8_t y, const char *str);

extern volatile uint16_t debug_Colors[DEBUG_COLORS_SIZE];
volatile uint8_t* debug_GetBitmapAddress(uint8_t vline);

#ifdef __cplusplus
}
#endif

#endif
