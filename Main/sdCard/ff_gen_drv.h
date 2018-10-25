/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FF_GEN_DRV_H
#define __FF_GEN_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "fatFS/diskio.h"
#include "fatFS/ff.h"

#define _USE_WRITE	1	/* 1: Enable disk_write function */
#define _USE_IOCTL	1	/* 1: Enable disk_ioctl function */

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  Disk IO Driver structure definition
 */
typedef struct
{
	DSTATUS (*disk_initialize)(BYTE); /*!< Initialize Disk Drive                     */
	DSTATUS (*disk_status)(BYTE); /*!< Get Disk Status                           */
	DRESULT (*disk_read)(BYTE, BYTE*, DWORD, UINT); /*!< Read Sector(s)                            */
#if _USE_WRITE == 1
	DRESULT (*disk_write)(BYTE, const BYTE*, DWORD, UINT); /*!< Write Sector(s) when _USE_WRITE = 0       */
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
	DRESULT (*disk_ioctl)(BYTE, BYTE, void*); /*!< I/O control operation when _USE_IOCTL = 1 */
#endif /* _USE_IOCTL == 1 */

} Diskio_drvTypeDef;

/**
 * @brief  Global Disk IO Drivers structure definition
 */
typedef struct
{
	uint8_t is_initialized[FF_VOLUMES];
	const Diskio_drvTypeDef *drv[FF_VOLUMES];
	uint8_t lun[FF_VOLUMES];
	volatile uint8_t nbr;
} Disk_drvTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t FATFS_LinkDriver(const Diskio_drvTypeDef *drv, char *path);
uint8_t FATFS_UnLinkDriver(char *path);
uint8_t FATFS_LinkDriverEx(const Diskio_drvTypeDef *drv, char *path, BYTE lun);
uint8_t FATFS_UnLinkDriverEx(char *path, BYTE lun);
uint8_t FATFS_GetAttachedDriversNbr(void);

#ifdef __cplusplus
}
#endif

#endif /* __FF_GEN_DRV_H */

