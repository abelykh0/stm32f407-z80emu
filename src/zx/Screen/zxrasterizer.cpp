#include <string.h>
#include "m4vgalib/vga.h"

#include "zxrasterizer.h"
#include "zxscreen.h"

namespace zx
{

ZXRasterizer::ZXRasterizer(
    uint8_t pixelSize, 
    uint16_t horizontalResolution,
    uint16_t verticalResolution)
{
    if (pixelSize < 1)
    {
        pixelSize = 1;
    }
    else if (pixelSize > 3)
    {
        pixelSize = 2;
    }

    if (horizontalResolution < 256 * pixelSize)
    {
        pixelSize = 2;
    }

    this->_pixelSize = pixelSize;
    this->_vResolution = verticalResolution / pixelSize;
    this->_hResolution = horizontalResolution / pixelSize;
    this->_leftBorder = ((this->_hResolution - HSIZE_PIXELS) / 2);
    this->_rightBorder = this->_leftBorder;
    this->_topBorder = ((this->_vResolution - VSIZE_PIXELS) / 2);
    this->_bottomBorder = this->_topBorder;
}

__attribute__((section(".ramcode")))
Rasterizer::RasterInfo ZXRasterizer::rasterize(
    unsigned cycles_per_pixel,
    unsigned line_number,
    Pixel *target)
{
    uint8_t borderColor = zx::BorderColor;
    if (line_number / this->_pixelSize < this->_topBorder)
    {
        memset(&target[0], borderColor, this->_hResolution);

        return {
            .offset = 0,
            .length = this->_hResolution,
            .cycles_per_pixel = cycles_per_pixel * this->_pixelSize,
            .repeat_lines = (unsigned)(this->_topBorder * this->_pixelSize - 1)
        };
    }
    else if (line_number / this->_pixelSize >= (unsigned)(this->_vResolution - this->_bottomBorder))
    {
        memset(&target[0], borderColor, this->_hResolution);

        return {
            .offset = 0,
            .length = this->_hResolution,
            .cycles_per_pixel = cycles_per_pixel * this->_pixelSize,
            .repeat_lines = (unsigned)(this->_topBorder * this->_pixelSize - 1)
        };
    }
    else
    {
        // Border to the left
        memset(&target[0], borderColor, this->_leftBorder);

        uint8_t vline = line_number / this->_pixelSize - this->_topBorder;
        uint8_t* bitmap = (uint8_t*)zx::GetBitmapAddress(vline);
        uint16_t* colors = (uint16_t*)&zx::VideoMemoryColors[vline / 8 * HSIZE_CHARS];
        uint8_t* dest = &target[this->_leftBorder];
        for (int i = 0; i < 8; i++)
        {
            zx::Draw4(bitmap, colors, dest);
            bitmap += 4; // characters
            colors += 4; // colors aka attributes
            dest += 32;  // pixels
        }

        // Border to the right
        memset(&target[this->_hResolution - this->_rightBorder], borderColor, this->_rightBorder);

        return {
            .offset = 0,
            .length = this->_hResolution,
            .cycles_per_pixel = cycles_per_pixel * this->_pixelSize,
            .repeat_lines = (unsigned)(this->_pixelSize - 1)
        };
    }
}

} // namespace z80
