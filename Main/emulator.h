#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include "Display/Screen.h"
#include "Emulator/SpectrumScreen.h"

using namespace Display;

extern Screen DebugScreen;
extern SpectrumScreen MainScreen;

void initializeVideo();
void startVideo();

#endif /* __EMULATOR_H__ */
