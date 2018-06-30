#include "debugrasterizer.h"
#include "debugband.h"
#include "zx/main_ROM.h"
#include "zx/Screen/zxscreen.h"

using namespace vga;

#define PIXEL_SIZE 2

namespace debug
{

DebugRasterizer::DebugRasterizer(
    uint16_t hResulution,
    uint16_t startLine)
{
    this->_startLine = startLine;
    this->_hResulution = hResulution / PIXEL_SIZE;

    // using the 8x8 font from the Sinclair ROM
    debug_setFont(&ROM[0x3CFD]);

    debug_setColor(0x3F10);
    debug_clear();
}

__attribute__((section(".ramcode")))
Rasterizer::RasterInfo DebugRasterizer::rasterize(
    unsigned cycles_per_pixel,
    unsigned line_number,
    Pixel *target)
{
    uint8_t vline = (line_number - this->_startLine) / PIXEL_SIZE;
    uint8_t *bitmap = (uint8_t *)debug_GetBitmapAddress(vline);
    uint16_t *colors = (uint16_t *)&debug_Colors[vline / 8 * DEBUG_HSIZE_CHARS];
    uint8_t *dest = target;
    for (int i = 0; i < ((this->_hResulution + 16) / 32); i++)
    {
        zx::Draw4(bitmap, colors, dest);
        bitmap += 4; // characters
        colors += 4; // colors aka attributes
        dest += 32;  // pixels
    }

    return {
        .offset = 0,
        .length = this->_hResulution,
        .cycles_per_pixel = cycles_per_pixel * PIXEL_SIZE,
        .repeat_lines = PIXEL_SIZE - 1};
}

} // namespace debug