#ifndef __DEBUGRASTERIZER_H
#define __DEBUGRASTERIZER_H

#include <stdint.h>
#include "m4vgalib/rasterizer.h"

using namespace vga;

namespace debug
{

class DebugRasterizer : public Rasterizer
{
  private:
    uint16_t _startLine;
    uint16_t _hResulution;

  public:
    DebugRasterizer(uint16_t hResulution, uint16_t startLine);

    RasterInfo rasterize(unsigned cycles_per_pixel, unsigned line_number, Pixel *target) override;
};

} // namespace debug

#endif