#ifndef __STARTUP_H
#define __STARTUP_H

#include <string.h>
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C"
{
#endif

void setup();
void loop();

extern void SystemClock_Config(
    RCC_OscInitTypeDef* rccOsc,
    RCC_ClkInitTypeDef* rccClk,
    uint32_t flashLatency);

extern RTC_HandleTypeDef hRtc;
void RtcInit();

void FpuInit();

#ifdef __cplusplus
}
#endif

#endif