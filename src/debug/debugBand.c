#include <string.h>

#include "debugBand.h"

const uint8_t *debug_font = NULL;
uint16_t debug_color;
uint8_t debug_cursor_x;
uint8_t debug_cursor_y;
volatile uint8_t debug_Pixels[DEBUG_BITMAP_SIZE];
volatile uint16_t debug_Colors[DEBUG_COLORS_SIZE];

void setCursorPosition(uint8_t x, uint8_t y);
void print_str(const char *str, uint16_t color);
void print_char(char c, uint16_t color);
void cursorNext();
void bitmap(uint16_t x, uint16_t y, const unsigned char *bmp,
            uint16_t i, uint8_t width, uint8_t lines);
void draw_char(const uint8_t *f, uint16_t x, uint16_t y, uint8_t c);
void print_char_at(uint8_t x, uint8_t y, unsigned char c, uint16_t color);

void debug_print_right(uint8_t width, uint8_t y, const char *str)
{
    uint8_t x = width - strlen(str);
    debug_print(x, y, str);
}

void debug_print(uint8_t x, uint8_t y, const char *str)
{
    if (debug_font == NULL)
    {
        return;
    }

    setCursorPosition(x, y);
    print_str(str, debug_color);
}

void debug_setFont(const uint8_t *font)
{
    debug_font = font;
}

void debug_setColor(uint16_t color)
{
    debug_color = color;
}

void debug_clear()
{
    memset((void*)debug_Pixels, 0, DEBUG_BITMAP_SIZE);
    for (int i = 0; i < DEBUG_COLORS_SIZE; i++)
    {
        debug_Colors[i] = debug_color;
    }
}

void setCursorPosition(uint8_t x, uint8_t y)
{
    if (debug_cursor_x == x && debug_cursor_y == y)
    {
        return;
    }

    debug_cursor_x = x;
    debug_cursor_y = y;
}

volatile uint8_t *debug_GetBitmapAddress(uint8_t vline)
{
    return &debug_Pixels[vline * DEBUG_HSIZE_CHARS];
}

void cursorNext()
{
    uint8_t x = debug_cursor_x;
    uint8_t y = debug_cursor_y;
    if (x < DEBUG_HSIZE_CHARS - 1)
    {
        x++;
    }
    else
    {
        if (y < DEBUG_VSIZE_CHARS - 1)
        {
            x = 0;
            y++;
        }
    }
    setCursorPosition(x, y);
}

void bitmap(uint16_t x, uint16_t y, const unsigned char *bmp,
            uint16_t i, uint8_t width, uint8_t lines)
{

    uint8_t temp, lshift, rshift, save, xtra;
    volatile uint8_t *si;

    rshift = x & 7;
    lshift = 8 - rshift;
    if (width == 0)
    {
        width = *(const uint8_t *)((uint32_t)(bmp) + i);
        i++;
    }
    if (lines == 0)
    {
        lines = *(const uint8_t *)((uint32_t)(bmp) + i);
        i++;
    }

    if (width & 7)
    {
        xtra = width & 7;
        width = width / 8;
        width++;
    }
    else
    {
        xtra = 8;
        width = width / 8;
    }

    for (uint8_t l = 0; l < lines; l++)
    {
        si = debug_GetBitmapAddress(y + l) + x / 8;
        if (width == 1)
        {
            temp = 0xff >> (rshift + xtra);
        }
        else
        {
            temp = 0;
        }

        save = *si;
        *si &= ((0xff << lshift) | temp);
        temp = *(const uint8_t *)((uint32_t)(bmp) + i++);
        *si |= temp >> rshift;
        si++;
        for (uint16_t b = i + width - 1; i < b; i++)
        {
            save = *si;
            *si = temp << lshift;
            temp = *(const uint8_t *)((uint32_t)(bmp) + i);
            *si |= temp >> rshift;
            si++;
        }

        if (rshift + xtra < 8)
        {
            *(si - 1) |= (save & (0xff >> (rshift + xtra))); //test me!!!
        }

        if (rshift + xtra - 8 > 0)
        {
            *si &= (0xff >> (rshift + xtra - 8));
        }

        *si |= temp << lshift;
    }
}

void draw_char(const uint8_t *f, uint16_t x, uint16_t y, uint8_t c)
{
    c -= *(f + 2);
    bitmap(x, y, f, (c * *(f + 1)) + 3, *f, *(f + 1));
}

void print_char_at(uint8_t x, uint8_t y, unsigned char c, uint16_t color)
{
    draw_char(debug_font, x * 8, y * 8, c);
    debug_Colors[y * DEBUG_HSIZE_CHARS + x] = color;
}

void print_char(char c, uint16_t color)
{
    switch (c)
    {
    case '\0': //null
        break;
    case '\n': //line feed
        if (debug_cursor_y < DEBUG_VSIZE_CHARS - 1)
        {
            setCursorPosition(0, debug_cursor_y + 1);
        }
        break;
    case 8: //backspace
        if (debug_cursor_x > 0)
        {
            print_char_at(debug_cursor_x - 1, debug_cursor_y, ' ', color);
            setCursorPosition(debug_cursor_x - 1, debug_cursor_y);
        }
        break;
    case 13: //carriage return !?!?!?!VT!?!??!?!
        debug_cursor_x = 0;
        break;
    case 14: //form feed new page(clear screen)
        //clear_screen();
        break;
    default:
    {
        uint8_t x = debug_cursor_x;
        uint8_t y = debug_cursor_y;
        cursorNext();
        print_char_at(x, y, c, color);
    }
    break;
    }
}

void print_str(const char *str, uint16_t color)
{
    while (*str)
    {
        print_char(*str++, color);
    }
}