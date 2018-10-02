#ifndef VGA_VGACONFIG_H
#define VGA_VGACONFIG_H

#include <string.h>
#include "stm32f4xx_hal.h"

#define VIDEO_GPIO gpioe
#define VIDEO_GPIO_MASK 0x3F00
#define VIDEO_GPIO_ODR 0x40021015

// HSYNC_PIN change requires other changes in code
#define HSYNC_PIN Gpio::p15

#define VSYNC_PIN Gpio::p14

#define IRQ extern "C"
#define etl_stm32f4xx_tim3_handler TIM3_IRQHandler
#define etl_stm32f4xx_tim4_handler TIM4_IRQHandler
#define etl_armv7m_pend_sv_handler PendSV_Handler
#define arena_reset()

#endif  // VGA_VGA_H
