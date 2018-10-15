#include <string.h>
#include <stdio.h>

#include "z80snapshot.h"
#include "Emulator/z80main.h"
#include "Emulator/z80emu/z80emu.h"
#include "Emulator/SpectrumScreen.h"

//#include "debug/debugBand.h"

extern Z80_STATE _zxCpu;
extern uint8_t RamBuffer[];

/*
 Offset  Length  Description
 ---------------------------
 0       1       A register
 1       1       F register
 2       2       BC register pair (LSB, i.e. C, first)
 4       2       HL register pair
 6       2       0 to signal a version 2 or 3
 8       2       Stack pointer
 10      1       Interrupt register
 11      1       Refresh register (Bit 7 is not significant!)
 12      1       Bit 0  : Bit 7 of the R-register
 Bit 1-3: Border color
 Bit 4-7: No meaning
 13      2       DE register pair
 15      2       BC' register pair
 17      2       DE' register pair
 19      2       HL' register pair
 21      1       A' register
 22      1       F' register
 23      2       IY register (Again LSB first)
 25      2       IX register
 27      1       Interrupt flipflop, 0=DI, otherwise EI
 28      1       IFF2
 29      1       Bit 0-1: Interrupt mode (0, 1 or 2)
 Bit 2  : 1=Issue 2 emulation
 Bit 3  : 1=Double interrupt frequency
 Bit 4-5: unused
 Bit 6-7: 0=Cursor/Protek/AGF joystick
 1=Kempston joystick
 2=Sinclair 2 Left joystick (or user
 defined, for version 3 .z80 files)
 3=Sinclair 2 Right joystick
 30 0x1E 2       Length of additional header block (see below)
 ===========================
 32 0x20 2       Program counter
 34 0x22 1       0 for ZX Spectrum 48K
 ...

 Hereafter a number of memory blocks follow, each containing the compressed data of a 16K block.
 The compression method is very simple: it replaces repetitions of at least five equal bytes by
 a four-byte code ED ED xx yy, which stands for "byte yy repeated xx times".
 Only sequences of length at least 5 are coded. The exception is sequences consisting of ED's;
 if they are encountered, even two ED's are encoded into ED ED 02 ED.
 Finally, every byte directly following a single ED is not taken into a block, for example ED 6*00
 is not encoded into ED ED ED 06 00 but into ED 00 ED ED 05 00.

 The structure of a memory block is:
 Byte    Length  Description
 ---------------------------
 0       2       Length of compressed data (without this 3-byte header)
 If length=0xffff, data is 16384 bytes long and not compressed
 2       1       Page number, for ZX Spectrum 48K:
 8: 4000-7fff
 4: 8000-bfff
 5: c000-ffff
 3       [0]     Data

 */

struct FileHeader
{
	uint8_t A;
	uint8_t F;
	uint16_t BC;
	uint16_t HL;
	uint16_t Version;
	uint16_t SP;
	uint8_t InterruptRegister;
	uint8_t RefreshRegister;
	uint8_t Flags1;
	uint16_t DE;
	uint16_t BC_Dash;
	uint16_t DE_Dash;
	uint16_t HL_Dash;
	uint8_t A_Dash;
	uint8_t F_Dash;
	uint16_t IY;
	uint16_t IX;
	uint8_t InterruptFlipFlop;
	uint8_t IFF2;
	uint8_t Flags2;
	uint16_t AdditionalBlockLength;
	uint16_t PC;
}__attribute__((packed));

void DecompressPage(uint8_t *page, uint16_t pageLength, uint8_t** destMemory,
		bool isCompressed);
void ReadState(FileHeader* header);

void zx::LoadZ80Snapshot(uint8_t* z80Snapshot, uint8_t buffer1[0x4000])
{
	// Note: this requires little-endian processor
	FileHeader* header = (FileHeader*) z80Snapshot;

	ReadState(header);

	uint8_t* buffer = &z80Snapshot[0x20 + header->AdditionalBlockLength];
	uint16_t pageSize = *buffer;
	buffer++;
	pageSize |= *buffer << 8;
	buffer++;
	uint8_t pageNumber = *buffer;
	buffer++;

	uint8_t* memory = RamBuffer;
	while (pageSize > 0)
	{
		switch (pageNumber)
		{
		case 8:
			memory = buffer1;
			break;
		case 4:
			memory = &RamBuffer[0x8000 - 0x5B00];
			break;
		case 5:
			memory = &RamBuffer[0xC000 - 0x5B00];
			break;
		}

		if (pageSize == 0xFFFF)
		{
			DecompressPage(buffer, 0x4000, &memory, false);
		}
		else
		{
			DecompressPage(buffer, pageSize, &memory, true);
		}

		if (pageNumber == 8)
		{
			// 0x4000..0x5AFF
			_spectrumScreen->ShowScreenshot(buffer1);

			// 0x5B00..0x7FFF
			memcpy(RamBuffer, &buffer1[0x1B00], 0x2500);
		}

		buffer += pageSize;
		pageSize = *buffer;
		buffer++;
		pageSize |= *buffer << 8;
		buffer++;
		pageNumber = *buffer;
		buffer++;
	}
}

bool zx::LoadZ80Snapshot(FIL* file, uint8_t buffer1[0x4000],
		uint8_t buffer2[0x4000])
{
	UINT bytesRead;

	FRESULT readResult = f_read(file, buffer1, sizeof(FileHeader), &bytesRead);
	if (readResult != FR_OK || bytesRead != sizeof(FileHeader))
	{
		return false;
	}

	// Note: this requires little-endian processor
	FileHeader* header = (FileHeader*) buffer1;
	ReadState(header);

	UINT bytesToRead = header->AdditionalBlockLength - 2 + 3;
	readResult = f_read(file, buffer1, bytesToRead, &bytesRead);
	if (readResult != FR_OK || bytesRead != bytesToRead)
	{
		return false;
	}

	uint8_t* buffer = &buffer1[bytesToRead - 3];

	// Get pageSize and pageNumber
	uint16_t pageSize = *buffer;
	buffer++;
	pageSize |= *buffer << 8;
	buffer++;
	uint8_t pageNumber = *buffer;

	do
	{
		bool isCompressed = (pageSize != 0xFFFF);
		if (!isCompressed)
		{
			pageSize = 0x4000;
		}

		uint8_t* memory;
		switch (pageNumber)
		{
		case 8:
			memory = buffer2;
			break;
		case 4:
			memory = &RamBuffer[0x8000 - 0x5B00];
			break;
		case 5:
			memory = &RamBuffer[0xC000 - 0x5B00];
			break;
		default:
			continue;
		}

		// Read page into tempBuffer
		uint8_t* buffer = buffer1;
		int remainingBytesInPage = pageSize;
		do
		{
			UINT bytesToRead =
					remainingBytesInPage < _MIN_SS ?
							remainingBytesInPage : _MIN_SS;
			readResult = f_read(file, buffer, bytesToRead, &bytesRead);
			remainingBytesInPage -= bytesRead;
			buffer += bytesRead;
		} while (readResult == FR_OK && remainingBytesInPage > 0);

		DecompressPage(buffer1, pageSize, &memory, isCompressed);

		if (pageNumber == 8)
		{
			// 0x4000..0x5AFF
			_spectrumScreen->ShowScreenshot(memory);

			// 0x5B00..0x7FFF
			memcpy(RamBuffer, &memory[0x1B00], 0x2500);
		}

		readResult = f_read(file, buffer1, 3, &bytesRead);
		buffer = buffer1;
		if (bytesRead == 3)
		{
			pageSize = *buffer;
			buffer++;
			pageSize |= *buffer << 8;
			buffer++;
			pageNumber = *buffer;
		}
		else
		{
			pageSize = 0;
		}

	} while (pageSize > 0);

	return true;
}

bool zx::LoadScreenFromZ80Snapshot(FIL* file, uint8_t buffer1[0x4000])
{
	UINT bytesRead;

	FRESULT readResult = f_read(file, buffer1, sizeof(FileHeader), &bytesRead);
	if (readResult != FR_OK || bytesRead != sizeof(FileHeader))
	{
		return false;
	}

	// Note: this requires little-endian processor
	FileHeader* header = (FileHeader*) buffer1;
	ReadState(header);

	UINT bytesToRead = header->AdditionalBlockLength - 2 + 3;
	readResult = f_read(file, buffer1, bytesToRead, &bytesRead);
	if (readResult != FR_OK || bytesRead != bytesToRead)
	{
		return false;
	}

	uint8_t* buffer = &buffer1[bytesToRead - 3];

	// Get pageSize and pageNumber
	uint16_t pageSize = *buffer;
	buffer++;
	pageSize |= *buffer << 8;
	buffer++;
	uint8_t pageNumber = *buffer;

	do
	{
		bool isCompressed = (pageSize != 0xFFFF);
		if (!isCompressed)
		{
			pageSize = 0x4000;
		}

		// Read page into buffer1
		uint8_t* buffer = buffer1;
		int remainingBytesInPage = pageSize;
		do
		{
			UINT bytesToRead =
					remainingBytesInPage < _MIN_SS ?
							remainingBytesInPage : _MIN_SS;
			readResult = f_read(file, buffer, bytesToRead, &bytesRead);
			remainingBytesInPage -= bytesRead;
			buffer += bytesRead;
		} while (readResult == FR_OK && remainingBytesInPage > 0);

		if (pageNumber == 8)
		{
			// This page contains screenshoot
			uint8_t* buffer2 = &buffer1[0x2000];
			if (pageSize > 0x2000)
			{
				pageSize = 0x2000;
			}
			DecompressPage(buffer1, pageSize, &buffer2, isCompressed);
			_spectrumScreen->ShowScreenshot(buffer2);
		}

		readResult = f_read(file, buffer1, 3, &bytesRead);
		buffer = buffer1;
		if (bytesRead == 3)
		{
			pageSize = *buffer;
			buffer++;
			pageSize |= *buffer << 8;
			buffer++;
			pageNumber = *buffer;
		}
		else
		{
			pageSize = 0;
		}

	} while (pageSize > 0);

	return true;
}

bool zx::LoadScreenshot(FIL* file, uint8_t buffer1[0x4000])
{
	FRESULT readResult;
	UINT bytesRead;
	int remainingBytes = 6912;
	uint8_t* buffer = buffer1;

	do
	{
		UINT bytesToRead = remainingBytes < _MIN_SS ? remainingBytes : _MIN_SS;
		readResult = f_read(file, buffer, bytesToRead, &bytesRead);
		if (readResult != FR_OK)
		{
			return false;
		}

		remainingBytes -= bytesRead;
		buffer += bytesRead;
	} while (remainingBytes > 0);

	_spectrumScreen->ShowScreenshot(buffer1);
	return true;
}

void DecompressPage(uint8_t* page, uint16_t pageLength, uint8_t** destMemory,
		bool isCompressed)
{
	uint8_t* memory = *destMemory;
	for (int i = 0; i < pageLength; i++)
	{
		if (i < pageLength - 4)
		{
			if (page[i] == 0x00 && page[i + 1] == 0xED && page[i + 2] == 0xED
					&& page[i + 3] == 0x00)
			{
				break;
			}

			if (isCompressed && page[i] == 0xED && page[i + 1] == 0xED)
			{
				i += 2;
				int repeat = page[i++];
				uint8_t value = page[i];
				for (int j = 0; j < repeat; j++)
				{
					*memory = value;
					memory++;
				}

				continue;
			}
		}

		*memory = page[i];
		memory++;
	}
}

void ReadState(FileHeader* header)
{
	_zxCpu.registers.byte[Z80_A] = header->A;
	_zxCpu.registers.byte[Z80_F] = header->F;
	_zxCpu.registers.word[Z80_BC] = header->BC;
	_zxCpu.registers.word[Z80_HL] = header->HL;
	_zxCpu.registers.word[Z80_SP] = header->SP;
	_zxCpu.i = header->InterruptRegister;
	_zxCpu.r = (header->RefreshRegister & 0x7F)
			| ((header->Flags1 & 0x01) << 7);
	_zxCpu.im = header->Flags2 & 0x3;
	_zxCpu.registers.word[Z80_DE] = header->DE;
	_zxCpu.alternates[Z80_BC] = header->BC_Dash;
	_zxCpu.alternates[Z80_DE] = header->DE_Dash;
	_zxCpu.alternates[Z80_HL] = header->HL_Dash;
	_zxCpu.alternates[Z80_AF] = header->F_Dash | (header->A_Dash << 8);
	_zxCpu.registers.word[Z80_IY] = header->IY;
	_zxCpu.registers.word[Z80_IX] = header->IX;
	_zxCpu.iff1 = header->InterruptFlipFlop;
	_zxCpu.iff2 = header->IFF2;
	_zxCpu.pc = header->PC;

	uint8_t borderColor = (header->Flags1 & 0x0E) > 1;
	*_spectrumScreen->Settings.BorderColor = _spectrumScreen->FromSpectrumColor(
			borderColor);
}
