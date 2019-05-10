#ifndef __USBD_MIDI_IF_H__
#define __USBD_MIDI_IF_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "usbd_midi.h"

extern USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS;

uint8_t MIDI_Transmit_FS(uint8_t* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_MIDI_IF_H__ */
