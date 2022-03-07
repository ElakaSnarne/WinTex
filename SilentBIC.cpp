#include "SilentBIC.h"
#include "Utilities.h"

BOOL CSilentBIC::Init(LPBYTE pData, int length)
{
	_embeddedAudioSize = 0;
	_firstAudioFrame = 0;
	_chunkTest = 0;

	CAnimBase::Init(pData, length);

	_width = GetInt(pData, 6, 2);
	_height = GetInt(pData, 8, 2);
	//_bpp = GetInt(pData, 10, 2);
	_rate = 6;
	_frameTime = 100;

	CreateBuffers(_width, _height, _factor);
	_texture.Init(_width, _height);

	// Copy palette
	for (int c = 0; c < 256; c++)
	{
		double r = pData[12 + c * 3 + 0];
		double g = pData[12 + c * 3 + 1];
		double b = pData[12 + c * 3 + 2];
		int ri = (byte)((r * 255.0) / 63.0);
		int gi = (byte)((g * 255.0) / 63.0);
		int bi = (byte)((b * 255.0) / 63.0);
		int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
		_pPalette[c] = col;
	}

	_videoFramePointer = 0x30c;

	_framePointer = _videoFramePointer;

	return TRUE;
}

BOOL CSilentBIC::DecodeFrame()
{
	BOOL ret = FALSE;

	if (_pInputBuffer != NULL && _framePointer >= 0 && _framePointer < _inputBufferLength)
	{
		int chunkSize = *(int*)(_pInputBuffer + _framePointer);
		int end = ProcessBICFrame(_framePointer + 4, chunkSize);

		/*wchar_t buffer[10];
		OutputDebugString(L"Playing Silent BIC frame ");
		OutputDebugString(_itow(_frame, buffer, 10));
		OutputDebugString(L" @ ");
		OutputDebugString(_itow(_framePointer, buffer, 16));
		OutputDebugString(L", size = ");
		OutputDebugString(_itow(chunkSize, buffer, 16));
		OutputDebugString(L"\r\n");*/

		if (end > _framePointer)
		{
			_framePointer += chunkSize + 4;

			ret = TRUE;
		}
	}

	return ret;
}
