#include "midimessage.h"
#include "usbd_midi.h"

void midiMessage(uint8_t message, uint8_t channel, uint8_t data1, uint8_t data2)
{
	uint8_t buffer[4];
	buffer[0] = message >> 4;
	buffer[1] = message | (channel & 0x0F);
	buffer[2] = data1 & 0x7F;
	buffer[3] = data2 & 0x7F;
	MIDI_Transmit_FS(buffer, 4);
}
