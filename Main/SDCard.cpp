#include <string.h>

#include "SDCard.h"
#include "Emulator.h"
#include "Keyboard/ps2Keyboard.h"
#include "Emulator/z80snapshot.h"

using namespace zx;

#define FILE_COLUMNS 3

uint8_t _buffer16K_1[0x4000];
uint8_t _buffer16K_2[0x4000];

uint8_t _listColumns[FILE_COLUMNS];
uint8_t _selectedFile = 0;
uint8_t _fileCount;

typedef TCHAR FileName[_MAX_LFN + 1];
FileName* _fileNames = (FileName*)_buffer16K_2;

void GetFileCoord(uint8_t fileIndex, uint8_t* x, uint8_t* y)
{
	*x= fileIndex / (DEBUG_ROWS - 1);
	*y= fileIndex % (DEBUG_ROWS - 1);
}

TCHAR* TruncateFileName(TCHAR* fileName)
{
	int maxLength = DEBUG_COLUMNS / FILE_COLUMNS - 1;
	TCHAR* result = (TCHAR*)_buffer16K_1;
	strncpy(result, fileName, maxLength);

	result[maxLength - 1] = '\0';
	TCHAR* extension = strrchr(result, '.');
	if (extension != nullptr)
	{
		*extension = '\0';
	}

	return result;
}

void SetSelection(uint8_t selectedFile)
{
	if (_fileCount == 0)
	{
		return;
	}

	_selectedFile = selectedFile;

	uint8_t x, y;
	GetFileCoord(selectedFile, &x, &y);
	DebugScreen.PrintAt(x, y, "\x10"); // ►

	// Show screenshot for the selected file
	TCHAR* fileName = _fileNames[selectedFile];

	FRESULT fr = f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);
	if (fr == FR_OK)
	{
		FIL file;
		fr = f_open(&file, fileName, FA_READ | FA_OPEN_EXISTING);
		if (fr == FR_OK)
		{
			LoadScreenFromZ80Snapshot(&file, _buffer16K_1);
		}

		f_mount(&SDFatFS, (TCHAR const*) SDPath, 0);
	}
}

bool loadSnapshotSetup()
{
	showTitle("Load snapshot. ENTER, ESC, \x18, \x19, \x1A, \x1B"); // ↑, ↓, →, ←

	FRESULT fr = f_mount(&SDFatFS, (TCHAR const*) SDPath, 1);
	if (fr != FR_OK)
	{
		return false;
	}

	DIR folder;
	FILINFO fileInfo;
	uint8_t maxFileCount = (DEBUG_ROWS - 1) * FILE_COLUMNS;
	_fileCount = 0;
	bool result = true;

	fr = f_findfirst(&folder, &fileInfo, (TCHAR const*) "/",
			(TCHAR const*) "*.z80");

	if (fr == FR_OK)
	{
		for (int fileIndex = 0; fileIndex < maxFileCount && fileInfo.fname[0]; fileIndex++)
		{
			strncpy(_fileNames[fileIndex], fileInfo.fname, _MAX_LFN);
			_fileCount++;

			fr = f_findnext(&folder, &fileInfo);
			if (fr != FR_OK)
			{
				result = false;
				break;
			}
		}
	}
	else
	{
		result = false;
	}

	for (int y = 1; y < DEBUG_ROWS; y++)
	{
		DebugScreen.PrintAt(_listColumns[1] - 1, y, "\xB3"); // │
		DebugScreen.PrintAt(_listColumns[2] - 1, y, "\xB3"); // │
	}

	uint8_t x, y;
	for (int fileIndex = 0; fileIndex < _fileCount; fileIndex++)
	{
		GetFileCoord(fileIndex, &x, &y);
		DebugScreen.PrintAt(x + 1, y, TruncateFileName(_fileNames[fileIndex]));
	}

	SetSelection(_selectedFile);

	// Unmount file system
	f_mount(&SDFatFS, (TCHAR const*) SDPath, 0);

	return result;
}

void loadSnapshotLoop()
{
    int32_t scanCode = Ps2_GetScancode();
    if (scanCode == 0
    	|| (scanCode & 0xFF00) != 0xF000)
    {
    	return;
    }

    uint8_t previousSelection = _selectedFile;

	scanCode = ((scanCode & 0xFF0000) >> 8 | (scanCode & 0xFF));
	switch (scanCode)
	{
	case KEY_UPARROW:
		if (_selectedFile > 0)
		{
			_selectedFile--;
		}
		break;
	case KEY_DOWNARROW:
		if (_selectedFile < _fileCount)
		{
			_selectedFile++;
		}
		break;
	}

	if (previousSelection == _selectedFile)
	{
		return;
	}

	uint8_t x, y;
	GetFileCoord(previousSelection, &x, &y);
	DebugScreen.PrintAt(x, y, " ");

	SetSelection(_selectedFile);
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
