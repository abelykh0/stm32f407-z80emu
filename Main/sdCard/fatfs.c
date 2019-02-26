#include "fatfs.h"

uint8_t retSD; /* Return value for SD */
char SDPath[4]; /* SD logical drive path */
FATFS SDFatFS; /* File system object for SD logical drive */
FIL SDFile; /* File object for SD */
extern RTC_HandleTypeDef hrtc;

void MX_FATFS_Init(void)
{
#ifdef BOARD2
	retSD = FATFS_LinkDriver(&FlashDriver, SDPath);
#else
	/*## FatFS: Link the SD driver ###########################*/
	retSD = FATFS_LinkDriver(&SD_Driver, SDPath);
#endif
}

/**
 * @brief  Gets Time from RTC
 * @param  None
 * @retval Time in DWORD
 */
DWORD get_fattime(void)
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    return  ((DWORD)(date.Year + 20) << 25)
            | ((DWORD)date.Month << 21)
            | ((DWORD)date.Date << 16)
            | ((DWORD)time.Hours << 11)
            | ((DWORD)time.Minutes << 5)
            | ((DWORD)time.Seconds >> 1);
    }
