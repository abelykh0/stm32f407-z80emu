#ifndef __ZXMAIN_INCLUDED__
#define __ZXMAIN_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#include "z80user.h"
#include "main_ROM.h"

void zx_setup();

int32_t zx_loop();

void zx_reset();

#ifdef __cplusplus
}
#endif

#endif
