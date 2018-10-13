#ifndef _SPECTRUMSCREEN_H
#define _SPECTRUMSCREEN_H

#include <Display/VideoSettings.h>
#include "Display/Screen.h"

namespace Display
{

class SpectrumScreen: public Screen
{
protected:
	uint8_t* GetPixelPointer(uint16_t line) override;
	uint8_t* GetPixelPointer(uint16_t line, uint8_t character) override;

public:
	SpectrumScreen(VideoSettings settings);
	SpectrumScreen(VideoSettings settings, uint16_t startLine, uint16_t height);

	void ShowScreenshot(const char *screenshot);
	uint16_t FromSpectrumColor(uint8_t sinclairColor);
	uint8_t ToSpectrumColor(uint16_t color);
};

}

#endif
