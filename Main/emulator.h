#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include "Display/Screen.h"
#include "Emulator/SpectrumScreen.h"
#include "fatfs.h"

using namespace Display;

#define DEBUG_COLUMNS 50
#define DEBUG_ROWS 8

extern Screen DebugScreen;
extern SpectrumScreen MainScreen;
extern uint8_t _buffer16K_1[0x4000];
extern uint8_t _buffer16K_2[0x4000];

void showKeyboardSetup();
bool showKeyboardLoop();

void initializeVideo();
void startVideo();
void showErrorMessage(const char* errorMessage);
void showTitle(const char* title);

#endif /* __EMULATOR_H__ */
