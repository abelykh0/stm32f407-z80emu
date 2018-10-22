#include <Display/Screen.h>
#include <string.h>
#include "m4vgalib/vga.h"

namespace Display
{

Screen::Screen(VideoSettings settings)
	: Screen(settings, 0,
			settings.Timing->video_end_line - settings.Timing->video_start_line)
{
}

Screen::Screen(VideoSettings settings, uint16_t startLine, uint16_t height)
{
	this->Settings = settings;
	this->_startLine = startLine;
	this->_isCursorVisible = false;

	this->_hResolutionNoBorder = this->Settings.TextColumns * 8;
	this->_hResolution = this->Settings.Timing->video_pixels / this->Settings.Scale;
	this->_vResolution = height / this->Settings.Scale;

	this->_horizontalBorder = (this->_hResolution - this->_hResolutionNoBorder) / 2;
    this->_verticalBorder = (this->_vResolution - this->Settings.TextRows * 8) / 2;

	this->_attributeCount = this->Settings.TextColumns * this->Settings.TextRows;
	this->_pixelCount = this->_attributeCount * 8;
}

void Screen::Clear()
{
	memset(this->Settings.Pixels, 0, this->_pixelCount);
	for (int i = 0; i < this->_attributeCount; i++)
	{
		this->Settings.Attributes[i] = this->_attribute;
	}
	*this->Settings.BorderColor = (uint8_t) this->_attribute;
}

uint8_t* Screen::GetPixelPointer(uint16_t line)
{
    return &this->Settings.Pixels[line * this->Settings.TextColumns];
}

uint8_t* Screen::GetPixelPointer(uint16_t line, uint8_t character)
{
	return this->GetPixelPointer(line) + character;
}

void Screen::SetFont(const uint8_t* font)
{
	this->_font = (uint8_t*) font;
}

void Screen::SetAttribute(uint16_t attribute)
{
	this->_attribute = attribute;
}

void Screen::SetCursorPosition(uint8_t x, uint8_t y)
{
	if (this->_cursor_x == x && this->_cursor_y == y)
	{
		return;
	}

	if (x >= this->Settings.TextColumns)
	{
		x = this->Settings.TextColumns - 1;
	}

	if (y >= this->Settings.TextRows)
	{
		y = this->Settings.TextRows - 1;
	}

    if (this->_isCursorVisible)
    {
    	this->InvertColor();
    }

	this->_cursor_x = x;
	this->_cursor_y = y;

    if (this->_isCursorVisible)
    {
    	this->InvertColor();
    }
}

void Screen::ShowCursor()
{
    if (!this->_isCursorVisible)
    {
    	this->_isCursorVisible = true;
    	this->InvertColor();
    }
}

void Screen::HideCursor()
{
    if (this->_isCursorVisible)
    {
    	this->_isCursorVisible = false;
    	this->InvertColor();
    }
}
void Screen::Print(const char* str)
{
    if (this->_font == nullptr)
    {
        return;
    }

    while (*str)
    {
        this->PrintChar(*str++, this->_attribute);
    }
}

void Screen::PrintAt(uint8_t x, uint8_t y, const char* str)
{
    this->SetCursorPosition(x, y);
    this->Print(str);
}

void Screen::PrintAlignRight(uint8_t y, const char *str)
{
    uint8_t leftX = this->Settings.TextColumns - strlen(str);
    this->PrintAt(leftX, y, str);
}

void Screen::PrintAlignCenter(uint8_t y, const char *str)
{
    uint8_t leftX = (this->Settings.TextColumns - strlen(str)) / 2;
    this->PrintAt(leftX, y, str);
}

__attribute__((section(".ramcode")))
Rasterizer::RasterInfo Screen::rasterize(
		unsigned cycles_per_pixel, unsigned line_number, Pixel *target)
{
    uint8_t borderColor = *this->Settings.BorderColor;

    unsigned scaledLine = (line_number - this->_startLine) / this->Settings.Scale;

    if (scaledLine < this->_verticalBorder
    	|| scaledLine >= (unsigned)(this->_vResolution - this->_verticalBorder))
    {
        memset(&target[0], borderColor, this->_hResolution);
    }
    else
    {
        // Border to the left
        memset(&target[0], borderColor, this->_horizontalBorder);

        uint16_t vline = scaledLine - this->_verticalBorder;
        uint8_t *bitmap = (uint8_t *)this->GetPixelPointer(vline);
        uint16_t *colors = (uint16_t *)&this->Settings.Attributes[vline / 8 * this->Settings.TextColumns];
        uint8_t *dest = &target[this->_horizontalBorder];

        for (int i = 0; i < ((this->_hResolutionNoBorder + 16) / 32); i++)
        {
            this->Draw4(bitmap, colors, dest);
            bitmap += 4; // characters
            colors += 4; // attributes
            dest += 32;  // pixels
        }

        // Border to the right
        memset(&target[this->_hResolution - this->_horizontalBorder], borderColor, this->_horizontalBorder);
    }

	Rasterizer::RasterInfo result;
	result.offset = 0;
	result.length = this->_hResolution;
	result.cycles_per_pixel = cycles_per_pixel * this->Settings.Scale;
	result.repeat_lines = (unsigned) (this->Settings.Scale - 1);
	return result;
}

void Screen::PrintChar(char c, uint16_t color)
{
	switch (c)
	{
	case '\0': //null
		break;
	case '\n': //line feed
		if (this->_cursor_y < this->Settings.TextRows - 1)
		{
			this->SetCursorPosition(0, this->_cursor_y + 1);
		}
		break;
	case '\b': //backspace
		if (this->_cursor_x > 0)
		{
			this->PrintCharAt(this->_cursor_x - 1, this->_cursor_y, ' ', color);
			this->SetCursorPosition(this->_cursor_x - 1, this->_cursor_y);
		}
		break;
	case 13: //carriage return
		this->_cursor_x = 0;
		break;
	default:
	{
		uint8_t x = this->_cursor_x;
		uint8_t y = this->_cursor_y;
		this->CursorNext();
		this->PrintCharAt(x, y, c, color);
	}
		break;
	}
}

void Screen::Bitmap(uint16_t x, uint16_t y, const unsigned char *bmp,
		uint16_t i, uint8_t width, uint8_t lines)
{

	uint8_t temp, lshift, rshift, save, xtra;
	volatile uint8_t *si;

	rshift = x & 7;
	lshift = 8 - rshift;
	if (width == 0)
	{
		width = *(const uint8_t *) ((uint32_t) (bmp) + i);
		i++;
	}
	if (lines == 0)
	{
		lines = *(const uint8_t *) ((uint32_t) (bmp) + i);
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
		si = this->GetPixelPointer(y + l) + x / 8;
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
		temp = *(const uint8_t *) ((uint32_t) (bmp) + i++);
		*si |= temp >> rshift;
		si++;
		for (uint16_t b = i + width - 1; i < b; i++)
		{
			save = *si;
			*si = temp << lshift;
			temp = *(const uint8_t *) ((uint32_t) (bmp) + i);
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

void Screen::DrawChar(const uint8_t *f, uint16_t x, uint16_t y, uint8_t c)
{
	c -= *(f + 2);
	this->Bitmap(x, y, f, (c * *(f + 1)) + 3, *f, *(f + 1));
}

void Screen::PrintCharAt(uint8_t x, uint8_t y, unsigned char c, uint16_t color)
{
	this->DrawChar(this->_font, x * 8, y * 8, c);
	this->Settings.Attributes[y * this->Settings.TextColumns + x] = color;
}

void Screen::CursorNext()
{
	uint8_t x = this->_cursor_x;
	uint8_t y = this->_cursor_y;
	if (x < this->Settings.TextColumns - 1)
	{
		x++;
	}
	else
	{
		if (y < this->Settings.TextRows - 1)
		{
			x = 0;
			y++;
		}
	}
	this->SetCursorPosition(x, y);
}

void Screen::InvertColor()
{
    uint16_t originalColor = this->Settings.Attributes[this->_cursor_y * this->Settings.TextColumns + this->_cursor_x];
    uint16_t newColor = __builtin_bswap16(originalColor);
    this->Settings.Attributes[this->_cursor_y * this->Settings.TextColumns + this->_cursor_x] = newColor;
}

void Screen::Draw4(uint8_t *bitmap, uint16_t *colors, uint8_t *dest)
{
	__asm__ volatile(
			"  ldr r1, [%[pix]], #4 \n\t" // pixels for characters 0..3
			"  ldr r3, [%[col]], #4 \n\t"// colors for characters 0..1

			// character #0
			"  ror r1, r1, #4    \n\t"// pixels >> 4
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".rept 7 \n\t"
			"  ror r1, r1, #31   \n\t"// pixels << 1
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".endr   \n\t"

			// character #1
			"  ror r3, r3, #16   \n\t"// colors
			"  ror r1, r1, #15   \n\t"// pixels >> 15
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".rept 7 \n\t"
			"  ror r1, r1, #31   \n\t"// pixels << 1
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".endr   \n\t"

			"  ldr r3, [%[col]], #4 \n\t"// colors for for characters 2..3

			// character #2
			"  ror r1, r1, #15   \n\t"// pixels >> 15
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".rept 7 \n\t"
			"  ror r1, r1, #31   \n\t"// pixels << 1
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".endr   \n\t"

			// character #3
			"  ror r3, r3, #16   \n\t"// colors
			"  ror r1, r1, #15   \n\t"// pixels >> 15
			"  and r0, r1, #8    \n\t"
			"  lsr r0, r3, r0    \n\t"
			"  strb r0, [%[odr]], #1 \n\t"
			".rept 7 \n\t"
			"  ror r1, r1, #31   \n\t"// pixels << 1
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

}
