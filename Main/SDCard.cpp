#include <string>

#include "SDCard.h"
#include "Emulator.h"

#define FILE_COLUMNS 3

uint8_t _listColumns[FILE_COLUMNS];

bool loadSnapshotSetup()
{




	DebugScreen.PrintAlignCenter(0, "Load snapshot. ENTER, ESC, \x18, \x19, \x1A, \x1B"); // ↑, ↓, →, ←

	for (int y = 1; y < DEBUG_ROWS; y++)
	{
		DebugScreen.PrintAt(_listColumns[1] - 1, y, "\xB3"); // │
		DebugScreen.PrintAt(_listColumns[2] - 1, y, "\xB3"); // │
	}





	return true;
}

void loadSnapshotLoop()
{
	// ►
}

/*

bool loadSnapshot(const TCHAR* fileName)
{
	FRESULT mountResult = f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);
	if (mountResult == FR_OK)
	{
		FIL fp;

		f_open(&fp, fileName, FA_READ | FA_OPEN_EXISTING);

		zx::LoadZ80Snapshot(&fp);

		f_close(&fp);

		f_mount(&SDFatFS, (TCHAR const*) SDPath, 0);
	}

	return true;
}

bool loadScreenshot(const TCHAR* fileName)
{
	FRESULT mountResult = f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);
	if (mountResult == FR_OK)
	{
		FIL fp;

		f_open(&fp, fileName, FA_READ | FA_OPEN_EXISTING);

		UINT bytesRead;
		FRESULT readResult;

		uint8_t* buffer = _pixels;
		UINT totalBytesRead = 0;
		do
		{
			readResult = f_read(&fp, buffer, _MIN_SS, &bytesRead);
			totalBytesRead += bytesRead;
			buffer += bytesRead;
		} while (readResult == FR_OK && totalBytesRead < 32 * 8 * 24);

		totalBytesRead = 0;
		do
		{
			readResult = f_read(&fp, readBuffer, _MIN_SS, &bytesRead);
			for (uint32_t i = 0; i < bytesRead; i++)
			{
				_attributes[totalBytesRead + i] = MainScreen.FromSpectrumColor(
						readBuffer[i]);
			}

			totalBytesRead += bytesRead;
		} while (readResult == FR_OK && totalBytesRead < 32 * 24);

		f_close(&fp);

		f_mount(&SDFatFS, (TCHAR const*) SDPath, 0);
	}

	return true;
}

*/
