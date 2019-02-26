#ifndef VGA_VGACONFIG_H
#define VGA_VGACONFIG_H

#include <string.h>
#include "stm32f4xx_hal.h"

#ifdef BOARD2

// PD0 	Red 1
// PD1	Red 2
// PD2 	Green 1
// PD3 	Green 2
// PD4 	Blue 1
// PD5 	Blue 2
// PB6 	HSync   // TIM4CH1
// PB7 	VSync
#define VIDEO_GPIO gpiod
#define VIDEO_GPIO_MASK 0x003F
#define VIDEO_GPIO_ODR (GPIOD_BASE+20)
#define SYNC_GPIO gpiob
#define HSYNC_PIN Gpio::p6
#define VSYNC_PIN Gpio::p7

#else

// PE8 		Red 1
// PE9 		Red 2
// PE10 	Green 1
// PE11 	Green 2
// PE12 	Blue 1
// PE13 	Blue 2
// PD15 	HSync   // TIM4CH4
// PD14 	VSync
#define VIDEO_GPIO gpioe
#define VIDEO_GPIO_MASK 0x3F00
#define VIDEO_GPIO_ODR (GPIOE_BASE+21)
#define SYNC_GPIO gpiod
#define HSYNC_PIN Gpio::p15
#define VSYNC_PIN Gpio::p14

#endif

#define IRQ extern "C"
#define etl_stm32f4xx_tim3_handler TIM3_IRQHandler
#define etl_stm32f4xx_tim4_handler TIM4_IRQHandler
#define etl_armv7m_pend_sv_handler PendSV_Handler
#define arena_reset()

#endif  // VGA_VGA_H
