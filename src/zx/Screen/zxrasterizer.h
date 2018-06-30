#ifndef ZX_RASTERIZER_H
#define ZX_RASTERIZER_H

#include "m4vgalib/rasterizer.h"

using namespace vga;

namespace zx
{

class ZXRasterizer : public Rasterizer
{
  private:
    uint8_t _pixelSize;
    uint16_t _hResolution;
    uint16_t _vResolution;
    uint16_t _leftBorder;
    uint16_t _rightBorder;
    uint8_t _bottomBorder;
    uint8_t _topBorder;

  public:
    ZXRasterizer(uint8_t pixelSize, uint16_t horizontalResolution, uint16_t verticalResolution);
    RasterInfo rasterize(unsigned cycles_per_pixel, unsigned line_number, Pixel *target) override;
};

} // namespace zx

#endif // VGA_RAST_SOLID_COLOR_H
