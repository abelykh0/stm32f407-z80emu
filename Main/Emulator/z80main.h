#ifndef __ZXMAIN_INCLUDED__
#define __ZXMAIN_INCLUDED__

#include "z80user.h"
#include "main_ROM.h"
#include "SpectrumScreen.h"

using namespace Display;

extern SpectrumScreen* _spectrumScreen;

void zx_setup(SpectrumScreen* spectrumScreen);
int32_t zx_loop();
void zx_reset();

#endif
