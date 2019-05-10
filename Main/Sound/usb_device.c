#include "usbd_midi.h"
#include "usbd_midi_if.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"

USBD_HandleTypeDef hUsbDeviceFS;

/**
 * Init USB device Library, add supported class and start the library
 * @retval None
 */
void MX_USB_DEVICE_Init(void)
{
	if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK)
	{
		Error_Handler();
	}

	if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MIDI) != USBD_OK)
	{
		Error_Handler();
	}

	if (USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) != USBD_OK)
	{
		Error_Handler();
	}

	if (USBD_Start(&hUsbDeviceFS) != USBD_OK)
	{
		Error_Handler();
	}
}
