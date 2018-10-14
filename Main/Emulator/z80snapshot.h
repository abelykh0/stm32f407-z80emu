#ifndef __Z80SNAPSHOT_INCLUDED__
#define __Z80SNAPSHOT_INCLUDED__

#include <stdint.h>
#include "fatfs.h"

namespace zx
{

void LoadZ80Snapshot(uint8_t* z80Snapshot);
bool LoadZ80Snapshot(FIL* file);


}

#endif
