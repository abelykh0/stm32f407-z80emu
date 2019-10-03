#include "midimessage.h"
#include "usbd_midi_if.h"

static uint8_t _buffer[64];
static uint8_t _bufferIndex = 0;

void midiMessage(uint8_t message, uint8_t channel, uint8_t data1, uint8_t data2)
{
	if (_bufferIndex >= 63 - 4)
	{
		// Discard unsent messages
		_bufferIndex = 0;
	}

	_buffer[_bufferIndex] = message >> 4;
	_bufferIndex++;
	_buffer[_bufferIndex] = message | (channel & 0x0F);
	_bufferIndex++;
	_buffer[_bufferIndex] = data1 & 0x7F;
	_bufferIndex++;
	_buffer[_bufferIndex] = data2 & 0x7F;
	_bufferIndex++;

	if (MIDI_Transmit_FS(_buffer, _bufferIndex) == USBD_OK)
	{
		_bufferIndex = 0;
	}
}
