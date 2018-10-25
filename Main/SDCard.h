#ifndef __SDCARD_H__
#define __SDCARD_H__

#include "fatfs.h"

bool loadSnapshotSetup();
bool loadSnapshotLoop();

bool saveSnapshotSetup();
bool saveSnapshotLoop();

#endif /* __SDCARD_H__ */
