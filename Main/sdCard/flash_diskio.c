/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "sd_diskio.h"

#define START_ADDRESS (FLASH_BASE+0x80000)
#define SIZE_IN_SECTORS 1024
#define SECTOR_SIZE 512

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

DSTATUS FlashInitialize(BYTE);
DSTATUS FlashStatus(BYTE);
DRESULT FlashRead(BYTE, BYTE*, DWORD, UINT);

const Diskio_drvTypeDef FlashDriver =
{ FlashInitialize, FlashStatus, FlashRead };

/**
 * @brief  Initializes a Drive
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS FlashInitialize(BYTE lun)
{
	Stat = ~STA_NOINIT;
	return Stat;
}

/**
 * @brief  Gets Disk Status
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS FlashStatus(BYTE lun)
{
	return Stat;
}

/**
 * @brief  Reads Sector(s)
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 */
DRESULT FlashRead(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
	if (sector + count > SIZE_IN_SECTORS)
	{
		return RES_ERROR;
	}

	memcpy(buff, (const void *)(START_ADDRESS + sector * SECTOR_SIZE), count * SECTOR_SIZE);

	return RES_OK;
}
