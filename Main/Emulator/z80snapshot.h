#ifndef __Z80SNAPSHOT_INCLUDED__
#define __Z80SNAPSHOT_INCLUDED__

#include <stdint.h>
#include "fatfs.h"

namespace zx
{

bool LoadZ80Snapshot(FIL* file, uint8_t buffer1[0x4000], uint8_t buffer2[0x4000]);
bool LoadScreenFromZ80Snapshot(FIL* file, uint8_t buffer1[0x4000]);
bool LoadScreenshot(FIL* file, uint8_t buffer1[0x4000]);
bool SaveZ80Snapshot(FIL* file, uint8_t buffer1[0x4000], uint8_t buffer2[0x4000]);

}

#endif
