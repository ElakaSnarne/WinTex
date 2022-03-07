#pragma once
#include "BIC.h"
#include "Utilities.h"
#include "MediaIdentifiers.h"

BOOL CBIC::Init(LPBYTE pData, int length)
{
	_embeddedAudioSize = 0;
	_firstAudioFrame = 0;
	_chunkTest = 0;

	CAnimBase::Init(pData, length);

	// Validate that the file is a BIC
	if (GetInt(pData, 0, 4) == BIC)
	{
		_width = GetInt(pData, 10, 2);
		_height = GetInt(pData, 12, 2);
		//_bpp = GetInt(pData, 14, 2);
		_rate = 6;
		_frameTime = 100;

		// Fix for some incorrect dimensions
		if (videoMode == VideoMode::Embedded && _width == 640)
		{
			_width = 432;
			_height = 324;
		}

		CreateBuffers(_width, _height, _factor);
		_texture.Init(_width, _height);

		// Copy palette
		for (int c = 0; c < 256; c++)
		{
			double r = pData[0x40 + c * 3 + 0];
			double g = pData[0x40 + c * 3 + 1];
			double b = pData[0x40 + c * 3 + 2];
			int ri = (byte)((r * 255.0) / 63.0);
			int gi = (byte)((g * 255.0) / 63.0);
			int bi = (byte)((b * 255.0) / 63.0);
			int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
			_pPalette[c] = col;
		}

		// Find video and audio pointers
		_videoFramePointer = 0x340;
		_audioFramePointer = 0x340;	// Initial main header is in first 0x40 bytes

		_chunkTest = 10 - GetInt(pData, 0x12, 2);
		_remainingAudioLength = GetInt(pData, 0x3c, 4);
		_embeddedAudioSize = GetInt(pData, 0x10, 2);
		_firstAudioFrame = GetInt(pData, 0x12, 2);

		if (_chunkTest > 0)
		{
			// We have preloaded wave data
			int preload = _chunkTest * _embeddedAudioSize;
			_videoFramePointer += preload;
		}

		if (_audioFramePointer != NULL)
		{
			// This is a new audio buffer
			_remainingAudioLength = GetInt(_pInputBuffer, 0x3c, 4);

			char formatBuff[64];
			WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
			pwfx->wFormatTag = GetInt(_pInputBuffer, 0x28, 2);// WAVE_FORMAT_PCM;
			pwfx->nChannels = GetInt(_pInputBuffer, 0x2a, 2);
			pwfx->nSamplesPerSec = GetInt(_pInputBuffer, 0x2c, 4);
			pwfx->nAvgBytesPerSec = GetInt(_pInputBuffer, 0x30, 4);
			pwfx->nBlockAlign = 2;
			pwfx->wBitsPerSample = GetInt(_pInputBuffer, 0x36, 2);
			pwfx->cbSize = 0;

			_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
			if (_sourceVoice != NULL)
			{
				if (_chunkTest > 0)
				{
					XAUDIO2_BUFFER buf = { 0 };
					int audioBytes = _videoFramePointer - 0x340;
					buf.AudioBytes = audioBytes;
					buf.pAudioData = _pInputBuffer + 0x340;
					_remainingAudioLength -= audioBytes;
					_sourceVoice->SubmitSourceBuffer(&buf);

					_audioFramesQueued++;
				}
			}
		}

		_framePointer = _videoFramePointer;

		return TRUE;
	}

	return FALSE;
}

int CBIC::ProcessBICFrame(int inPtr, int chunkSize)
{
	int outPtr = 0;
	int end = inPtr + chunkSize;
	int currentRow = 0;
	BOOL good = TRUE;
	while (inPtr < end && good)
	{
		int type = *(_pInputBuffer + inPtr++);
		if ((type & 1) != 0)
		{
			outPtr = currentRow * _width;

			BOOL readOffset = FALSE;
			int chunks = *(short*)(_pInputBuffer + inPtr);
			inPtr += 2;
			if (chunks < 0)
			{
				chunks = -chunks;
				readOffset = TRUE;
			}

			if (chunks >= 160)
			{
				good = FALSE;
				break;
			}

			while (chunks > 0)
			{
				if (readOffset)
				{
					outPtr += *(_pInputBuffer + inPtr++) * 4;
					chunks--;
				}

				int count = *(_pInputBuffer + inPtr++);
				for (int i = 0; i < count; i++)
				{
					byte b = *(_pInputBuffer + inPtr++);
					int copyOut = outPtr;
					for (int y = 0; y < 4; y++)
					{
						for (int x = 0; x < 4; x++)
						{
							_pVideoOutputBuffer[copyOut + x] = b;
						}

						copyOut += _width;
					}

					outPtr += 4;
				}

				chunks--;
				readOffset = TRUE;
			}
		}

		if (good && (type & 2) != 0)
		{
			outPtr = currentRow * _width;

			BOOL readOffset = FALSE;
			int chunks = *(short*)(_pInputBuffer + inPtr);
			inPtr += 2;
			if (chunks < 0)
			{
				chunks = -chunks;
				readOffset = TRUE;
			}

			if (chunks >= 160)
			{
				good = FALSE;
				break;
			}

			while (chunks > 0)
			{
				if (readOffset)
				{
					outPtr += *(_pInputBuffer + inPtr++) * 4;
					chunks--;
				}

				int count = *(_pInputBuffer + inPtr++);
				for (int i = 0; i < count; i++)
				{
					int c1 = *(_pInputBuffer + inPtr++);
					int c2 = *(_pInputBuffer + inPtr++);
					int pattern = *(short*)(_pInputBuffer + inPtr);
					inPtr += 2;
					int copyOut = outPtr;
					for (int y = 0; y < 4; y++)
					{
						_pVideoOutputBuffer[copyOut + 0] = (byte)(((pattern & 1) != 0) ? c2 : c1);
						_pVideoOutputBuffer[copyOut + 1] = (byte)(((pattern & 2) != 0) ? c2 : c1);
						_pVideoOutputBuffer[copyOut + 2] = (byte)(((pattern & 4) != 0) ? c2 : c1);
						_pVideoOutputBuffer[copyOut + 3] = (byte)(((pattern & 8) != 0) ? c2 : c1);

						copyOut += _width;

						pattern >>= 4;
					}

					outPtr += 4;
				}

				chunks--;
				readOffset = TRUE;
			}
		}

		if (good && (type & 4) != 0)
		{
			outPtr = currentRow * _width;

			BOOL readOffset = FALSE;
			int chunks = *(short*)(_pInputBuffer + inPtr);
			inPtr += 2;
			if (chunks < 0)
			{
				chunks = -chunks;
				readOffset = TRUE;
			}

			if (chunks >= 160)
			{
				good = FALSE;
				break;
			}

			while (chunks > 0)
			{
				if (readOffset)
				{
					outPtr += *(_pInputBuffer + inPtr++) * 4;
					chunks--;
				}

				int count = *(_pInputBuffer + inPtr++);
				for (int i = 0; i < count; i++)
				{
					int copyOut = outPtr;
					for (int y = 0; y < 4; y++)
					{
						for (int x = 0; x < 4; x++)
						{
							_pVideoOutputBuffer[copyOut + x] = *(_pInputBuffer + inPtr++);
						}

						copyOut += _width;
					}

					outPtr += 4;
				}

				chunks--;
				readOffset = TRUE;
			}
		}

		if (good && (type & 8) != 0)
		{
			outPtr = currentRow * _width;

			BOOL readOffset = FALSE;
			int chunks = *(short*)(_pInputBuffer + inPtr);
			inPtr += 2;
			if (chunks < 0)
			{
				chunks = -chunks;
				readOffset = TRUE;
			}

			if (chunks >= 160)
			{
				good = FALSE;
				break;
			}

			while (chunks > 0)
			{
				if (readOffset)
				{
					outPtr += *(_pInputBuffer + inPtr++) * 4;
					chunks--;
				}

				int count = *(_pInputBuffer + inPtr++);
				for (int i = 0; i < count; i++)
				{
					int c = *(_pInputBuffer + inPtr++);
					int c2 = (c >> 4) & 0xf;
					int c1 = c & 0xf;
					int pattern = *(short*)(_pInputBuffer + inPtr);
					inPtr += 2;

					int copyOut = outPtr;
					for (int y = 0; y < 4; y++)
					{
						_pVideoOutputBuffer[copyOut + 0] = (byte)(((pattern & 1) != 0) ? c2 : c1);
						_pVideoOutputBuffer[copyOut + 1] = (byte)(((pattern & 2) != 0) ? c2 : c1);
						_pVideoOutputBuffer[copyOut + 2] = (byte)(((pattern & 4) != 0) ? c2 : c1);
						_pVideoOutputBuffer[copyOut + 3] = (byte)(((pattern & 8) != 0) ? c2 : c1);

						copyOut += _width;

						pattern >>= 4;
					}

					outPtr += 4;
				}

				chunks--;
				readOffset = TRUE;
			}
		}

		currentRow += 4;
	}

	return end;
}

BOOL CBIC::DecodeFrame()
{
	BOOL ret = FALSE;

	if (_pInputBuffer != NULL && _framePointer >= 0 && _framePointer < _inputBufferLength)
	{
		int chunkSize = *(int*)(_pInputBuffer + _framePointer);
		int end = ProcessBICFrame(_framePointer + 4, chunkSize);

		if (end > _framePointer)
		{
			_framePointer += chunkSize + 4;

			// Play embedded audio...
			_chunkTest++;
			if (_chunkTest >= 1 && _remainingAudioLength > 0)
			{
				int remainingBufferLength = _inputBufferLength - _framePointer;
				int waveChunkSize = (remainingBufferLength == _remainingAudioLength) ? _remainingAudioLength : min(_embeddedAudioSize, _remainingAudioLength);
				_remainingAudioLength -= waveChunkSize;

				XAUDIO2_BUFFER buf = { 0 };
				buf.AudioBytes = waveChunkSize;
				buf.pAudioData = _pInputBuffer + _framePointer;
				if (_remainingAudioLength == 0) buf.Flags = XAUDIO2_END_OF_STREAM;
				_sourceVoice->SubmitSourceBuffer(&buf);

				_framePointer += waveChunkSize;

				_audioFramesQueued++;
			}

			if (_frame == _firstAudioFrame) _sourceVoice->Start(0, 0);

			ret = TRUE;
		}
	}

	return ret;
}
