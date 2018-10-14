#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include "Display/Screen.h"
#include "Emulator/SpectrumScreen.h"
#include "fatfs.h"

using namespace Display;

extern Screen DebugScreen;
extern SpectrumScreen MainScreen;

void initializeVideo();
void startVideo();
bool loadScreenshot(const TCHAR* fileName);
bool loadSnapshot(const TCHAR* fileName);

#endif /* __EMULATOR_H__ */
