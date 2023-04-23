#pragma once

#include "PTF.h"
#include "Utilities.h"
#include "MediaIdentifiers.h"

BOOL CPTF::Init(LPBYTE pData, int length)
{
	BOOL ret = CAnimBase::Init(pData, length);

	// Validate that the file is a PTF
	if (ret && GetInt(pData, 4, 4) == PTF)
	{
		_width = GetInt(pData, 12, 2);
		_height = GetInt(pData, 14, 2);
		_rate = GetInt(pData, 18, 2);
		_frameTime = (100 * _rate) / 6;

		CreateBuffers(_width, _height, _factor);
		_texture.Init(_width, _height);

		// Find video and audio pointers
		_framePointer = 0x40;
		int framePtr = 0x40;
		while (framePtr < _inputBufferLength && (_videoFramePointer == 0 || _audioFramePointer == 0))
		{
			int frameSize = GetInt(_pInputBuffer, framePtr, 4);
			if (frameSize < 0) frameSize = 0x300 - frameSize;
			int frameType = GetInt(_pInputBuffer, framePtr + 4, 2);
			if (frameType == 0x0b1c || frameType == 0x5756) frameSize += 6;
			if ((frameType == 0xf1fa || frameType == 0x0b1c) && _videoFramePointer == 0)
			{
				_videoFramePointer = framePtr;
			}
			else if (frameType == 0x5657 && _audioFramePointer == 0)
			{
				_audioFramePointer = framePtr;
			}
			framePtr += frameSize;
		}

		if (_audioFramePointer != NULL && _videoFramePointer == NULL)
		{
			// Prepare for audio playback
			_framePointer = _inputBufferLength;
		}
	}

	return ret;
}

BOOL CPTF::ProcessFLCFrame(int inPtr, int chunkSize)
{
	int outPtr = 0;
	int currentRow = 0;

	int subChunks = *(short*)(_pInputBuffer + inPtr);
	inPtr += 10;// Skip subChunks count and reserved data
	while (subChunks > 0)
	{
		int cmdSize = *(int*)(_pInputBuffer + inPtr);		// Command size
		int cmdType = *(short*)(_pInputBuffer + inPtr + 4);	// Command type
		if (cmdType <= 0x10)
		{
			inPtr += 6;

			if (cmdType == 0x07)
			{
				// Delta frame
				currentRow = outPtr;

				int rowsToUpdate = *(short*)(_pInputBuffer + inPtr);
				inPtr += 2;
				if (rowsToUpdate <= _height)
				{
					for (int row = 0; row < rowsToUpdate; )
					{
						int changeCount = *(short*)(_pInputBuffer + inPtr);
						inPtr += 2;
						if ((changeCount & 0x8000) != 0)
						{
							// Signed
							if ((changeCount & 0x4000) != 0)
							{
								changeCount = -changeCount;
								if (changeCount >= _height) break;
								currentRow += changeCount & 0x3fff;
								outPtr = currentRow * _width;
							}
						}
						else if (changeCount > _width)
						{
							// Exit
							break;
						}
						else
						{
							// Plus or zero
							for (int change = 0; change < changeCount; change++)
							{
								outPtr += *(_pInputBuffer + inPtr++);
								int dataCount = (*(_pInputBuffer + inPtr++)) & 0xff;
								if ((dataCount & 0x80) != 0) dataCount |= 0xff00;
								dataCount += dataCount;
								if ((dataCount & 0x8000) != 0)
								{
									// Signed
									dataCount = (-dataCount) & 0xffff;
									int col1 = (*(_pInputBuffer + inPtr++)) & 0xff;
									int col2 = (*(_pInputBuffer + inPtr++)) & 0xff;

									try
									{
										for (int c = 0; c < dataCount / 4; c++)
										{
											_pVideoOutputBuffer[outPtr++] = col1;
											_pVideoOutputBuffer[outPtr++] = col2;
											_pVideoOutputBuffer[outPtr++] = col1;
											_pVideoOutputBuffer[outPtr++] = col2;
										}

										for (int c = 0; c < (dataCount & 3); c++)
										{
											_pVideoOutputBuffer[outPtr++] = col1;
										}
									}
									catch (...)
									{
										int debug = 0;
									}
								}
								else
								{
									// Copy dataCount next bytes from inPtr to outPtr
									for (int c = 0; c < dataCount; c++)
									{
										_pVideoOutputBuffer[outPtr++] = *(_pInputBuffer + inPtr++);
									}
								}
							}

							currentRow++;
							outPtr = _width * currentRow;
							row++;
						}
					}
				}
			}
			else if (cmdType == 0x0b)
			{
				// Palette
				int numberOfEntries = *(short*)(_pInputBuffer + inPtr);
				inPtr += 2;
				int currentIndex = 0;
				for (int i = 0; i < numberOfEntries; i++)
				{
					currentIndex += *(_pInputBuffer + inPtr++);
					int colourCount = *(_pInputBuffer + inPtr++);
					if (colourCount == 0)
					{
						colourCount = 0x100;
					}

					for (int j = 0; j < colourCount; j++)
					{
						int r = _colourTranslationTable[_pInputBuffer[inPtr++]];
						int g = _colourTranslationTable[_pInputBuffer[inPtr++]];
						int b = _colourTranslationTable[_pInputBuffer[inPtr++]];

						if (videoMode == VideoMode::Embedded && j < 32)
						{
							r = g = b = 0;
						}

						int c = 0xff000000 | b | (g << 8) | (r << 16);
						_pPalette[currentIndex++] = c;
					}
				}
			}
			else if (cmdType == 0xd)
			{
				// Clear screen
				ZeroMemory(_pVideoOutputBuffer, _width * _height);
				inPtr += 6;
			}
			else if (cmdType == 0x0f)
			{
				// Full frame
				for (int y = 0; y < _height; y++)
				{
					inPtr++;	// Skip packet count
					int bytesLeft = _width;
					while (bytesLeft > 0)
					{
						int count = *(_pInputBuffer + inPtr++);
						if ((count & 0x80) != 0)
						{
							count = min((-count) & 0xff, bytesLeft);
							for (int x = 0; x < count; x++)
							{
								_pVideoOutputBuffer[outPtr++] = *(_pInputBuffer + inPtr++);
							}
						}
						else
						{
							byte cp = *(_pInputBuffer + inPtr++);
							count = min(count, bytesLeft);
							for (int x = 0; x < count; x++)
							{
								_pVideoOutputBuffer[outPtr++] = cp;
							}
						}

						bytesLeft -= count;
					}
				}
			}
			else
			{
				inPtr += cmdSize - 6;
			}
		}
		else
		{
			break;
		}

		subChunks--;
	}

	return TRUE;
}

BOOL CPTF::DecodeFrame()
{
	// Video is usually before audio
	BOOL ret = FALSE;

	// If this is an audio-only PTF, should extract all audio bytes to prevent clicking
	if (_videoFramePointer == NULL)
	{
		//Trace(L"PTF Audio-Only Frame ");
		//TraceLine(_frame);

		ret = TRUE;

		if (_audioFramePointer > 0)
		{
			int inPtr = _audioFramePointer;
			while (inPtr < _inputBufferLength)
			{
				int chunkSize = GetInt(_pInputBuffer, inPtr, 4);
				inPtr += 6;
				int audioBytes = chunkSize;
				int audioPtr = inPtr;
				if (GetInt(_pInputBuffer, inPtr, 4) == RIFF)
				{
					// This is a new audio buffer
					_remainingAudioLength = GetInt(_pInputBuffer, inPtr + 0x28, 4);

					if (_sourceVoice == NULL)
					{
						char formatBuff[64];
						WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
						pwfx->wFormatTag = GetInt(_pInputBuffer, inPtr + 0x14, 2);// WAVE_FORMAT_PCM;
						pwfx->nChannels = GetInt(_pInputBuffer, inPtr + 0x16, 2);
						pwfx->nSamplesPerSec = GetInt(_pInputBuffer, inPtr + 0x18, 4);
						pwfx->nAvgBytesPerSec = GetInt(_pInputBuffer, inPtr + 0x1c, 4);
						pwfx->nBlockAlign = 2;
						pwfx->wBitsPerSample = GetInt(_pInputBuffer, inPtr + 0x22, 2);
						pwfx->cbSize = 0;
						_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
					}
					_sourceVoice->Start(0, 0);

					audioPtr += 0x2c;
					audioBytes -= 0x2c;
				}

				Buffer ab;
				audioBytes = min(audioBytes, _remainingAudioLength);
				audioBytes = min(audioBytes, _inputBufferLength - audioPtr);
				ab.Size = audioBytes;
				ab.pData = _pInputBuffer + audioPtr;
				_remainingAudioLength -= audioBytes;
				_audioBuffers.push_back(ab);

				inPtr += chunkSize;
			}

			if (_sourceVoice != NULL)
			{
				// Enqueue a couple of buffers	XAUDIO2_MAX_QUEUED_BUFFERS=64
				auto buffers = _audioBuffers.size();
				for (int i = 0; i < buffers && i < 10; i++)
				{
					Buffer ab = _audioBuffers.front();
					_audioBuffers.pop_front();

					XAUDIO2_BUFFER buf = { 0 };
					buf.AudioBytes = ab.Size;
					buf.pAudioData = ab.pData;
					//if (_remainingAudioLength == 0) buf.Flags = XAUDIO2_END_OF_STREAM;
					_sourceVoice->SubmitSourceBuffer(&buf);

					_audioFramesQueued++;
				}
			}

			_audioFramePointer = 0;

			ret = TRUE;
		}
	}
	else if (_pInputBuffer != NULL && _framePointer >= 0 && _framePointer < _inputBufferLength)
	{
		int inPtr = _framePointer;
		int chunkSize = GetInt(_pInputBuffer, inPtr, 4);
		int frameType = GetInt(_pInputBuffer, inPtr + 4, 2);
		BOOL embeddedPalette = FALSE;
		inPtr += 6;

#pragma region Wave
		if (frameType == 0x5657)
		{
			int audioBytes = chunkSize;
			chunkSize += 6;
			inPtr = _framePointer + 6;

			if (GetInt(_pInputBuffer, inPtr, 4) == RIFF)
			{
				// This is a new audio buffer
				_remainingAudioLength = GetInt(_pInputBuffer, inPtr + 0x28, 4);

				if (_sourceVoice == NULL)
				{
					char formatBuff[64];
					WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
					pwfx->wFormatTag = GetInt(_pInputBuffer, inPtr + 0x14, 2);// WAVE_FORMAT_PCM;
					pwfx->nChannels = GetInt(_pInputBuffer, inPtr + 0x16, 2);
					pwfx->nSamplesPerSec = GetInt(_pInputBuffer, inPtr + 0x18, 4);
					pwfx->nAvgBytesPerSec = GetInt(_pInputBuffer, inPtr + 0x1c, 4);
					pwfx->nBlockAlign = 2;
					pwfx->wBitsPerSample = GetInt(_pInputBuffer, inPtr + 0x22, 2);
					pwfx->cbSize = 0;
					_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
				}
				_sourceVoice->Start(0, 0);

				inPtr += 0x2c;
				audioBytes -= 0x2c;
			}

			if (_sourceVoice != NULL)
			{
				XAUDIO2_BUFFER buf = { 0 };
				audioBytes = min(audioBytes, _remainingAudioLength);
				audioBytes = min(audioBytes, _inputBufferLength - inPtr);
				buf.AudioBytes = audioBytes;
				buf.pAudioData = _pInputBuffer + inPtr;
				_remainingAudioLength -= audioBytes;
				//if (_remainingAudioLength == 0) buf.Flags = XAUDIO2_END_OF_STREAM;
				_sourceVoice->SubmitSourceBuffer(&buf);

				_audioFramesQueued++;
			}

			_framePointer += chunkSize;

			ret = TRUE;

			inPtr = _framePointer;
			if (inPtr < _inputBufferLength)
			{
				chunkSize = GetInt(_pInputBuffer, inPtr, 4);
				frameType = GetInt(_pInputBuffer, inPtr + 4, 2);
				inPtr += 6;
			}
		}
#pragma endregion

#pragma region Embedded palette
		if (chunkSize < 0)
		{
			// Embedded palette
			embeddedPalette = TRUE;

			chunkSize = -chunkSize;

			for (int c = 0; c < 256; c++)
			{
				int r = _colourTranslationTable[_pInputBuffer[inPtr++]];
				int g = _colourTranslationTable[_pInputBuffer[inPtr++]];
				int b = _colourTranslationTable[_pInputBuffer[inPtr++]];

				if (videoMode == VideoMode::Embedded && c < 32)
				{
					r = g = b = 0;
				}

				int col = 0xff000000 | b | (g << 8) | (r << 16);
				_pPalette[c] = col;
			}
		}
#pragma endregion

#pragma region FLC
		if (frameType == 0xf1fa)
		{
			// FLC
			ProcessFLCFrame(inPtr, chunkSize);
			ret = TRUE;
			_videoFramesProcessed++;
			_framePointer += chunkSize;
			if (embeddedPalette) _framePointer += 0x300;
		}
#pragma endregion
#pragma region BIC
		else if (frameType == 0x0b1c)
		{
			// BIC frame
			ProcessBICFrame(inPtr, chunkSize);
			chunkSize += 6;
			ret = TRUE;
			_videoFramesProcessed++;
			_framePointer += chunkSize;
			if (embeddedPalette) _framePointer += 0x300;
		}
#pragma endregion
	}

	return ret;
}
