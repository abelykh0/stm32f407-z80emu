#include "startup.h"

uint32_t SystemCoreClock = 16000000;
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

RTC_HandleTypeDef hRtc;

void SystemClock_Config_Default();

int main()
{
    HAL_Init();
    SystemClock_Config_Default();

    setup();

    while (1)
    {
        loop();
    }
}

void RtcInit()
{
    /**Initialize RTC Only 
    */
    hRtc.Instance = RTC;
    if (HAL_RTCEx_BKUPRead(&hRtc, RTC_BKP_DR0) != 0x32F2)
    {
        hRtc.Init.HourFormat = RTC_HOURFORMAT_24;
        hRtc.Init.AsynchPrediv = 127;
        hRtc.Init.SynchPrediv = 255;
        hRtc.Init.OutPut = RTC_OUTPUT_DISABLE;
        hRtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        hRtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
        HAL_RTC_Init(&hRtc);

        /**Initialize RTC and set the Time and Date 
        */
        RTC_TimeTypeDef sTime;
        sTime.Hours = 0x0;
        sTime.Minutes = 0x0;
        sTime.Seconds = 0x0;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;
        HAL_RTC_SetTime(&hRtc, &sTime, RTC_FORMAT_BCD);

        RTC_DateTypeDef sDate;
        sDate.WeekDay = RTC_WEEKDAY_MONDAY;
        sDate.Month = RTC_MONTH_JANUARY;
        sDate.Date = 0x1;
        sDate.Year = 0x0;
        HAL_RTC_SetDate(&hRtc, &sDate, RTC_FORMAT_BCD);

        HAL_RTCEx_BKUPWrite(&hRtc, RTC_BKP_DR0, 0x32F2);
    }
}

void FpuInit()
{
    // Enable floating point automatic/lazy state preservation.
    // The CONTROL bit governing FP will be set automatically when first used.
    FPU->FPCCR |= FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk;

    // Enable access to the floating point coprocessor.
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */

    // Instruction Synchronization Barrier
    asm volatile("isb");
}

void SystemClock_Config(
    RCC_OscInitTypeDef *rccOsc,
    RCC_ClkInitTypeDef *rccClk,
    uint32_t flashLatency)
{
    /**Configure the main internal regulator output voltage 
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
    HAL_StatusTypeDef m = HAL_RCC_OscConfig(rccOsc);
    if (m == HAL_ERROR)
    {
        __HAL_RCC_PWR_CLK_ENABLE();
    }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
    HAL_RCC_ClockConfig(rccClk, flashLatency);

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_HSE_DIV8;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /**Configure the Systick interrupt time 
    */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

    /**Configure the Systick 
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void SystemClock_Config_Default()
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    SystemClock_Config(&RCC_OscInitStruct, &RCC_ClkInitStruct, FLASH_LATENCY_5);
}

__weak void SysTick_Handler(void)
{
    HAL_IncTick();
}

__weak void NMI_Handler(void)
{
}

__weak void HardFault_Handler(void)
{
    while (1)
    {
    }
}

__weak void MemManage_Handler(void)
{
    while (1)
    {
    }
}

__weak void BusFault_Handler(void)
{
    while (1)
    {
    }
}

__weak void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

__weak void SVC_Handler(void)
{
}

__weak void DebugMon_Handler(void)
{
}

__weak void PendSV_Handler(void)
{
}

__weak void _Error_Handler(char *file, int line)
{
    while (1)
    {
    }
}
