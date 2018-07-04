#ifndef VGA_VGACONFIG_H
#define VGA_VGACONFIG_H

#include "startup.h"

#define VIDEO_GPIO gpioa
#define VIDEO_GPIO_MASK 0x003F
#define VIDEO_GPIO_ODR ((Word)&GPIOA->ODR)

#define IRQ extern "C"
#define etl_stm32f4xx_tim3_handler TIM3_IRQHandler
#define etl_stm32f4xx_tim4_handler TIM4_IRQHandler
#define etl_armv7m_pend_sv_handler PendSV_Handler
#define arena_reset()

#endif  // VGA_VGA_H
