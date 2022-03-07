#include "H2O.h"
#include "Utilities.h"
#include "MediaIdentifiers.h"

CH2O::CH2O()
{
}

CH2O::~CH2O()
{
}

BOOL CH2O::Init(LPBYTE pData, int length)
{
	BOOL ret = CAnimBase::Init(pData, length);

	// TODO: Prepare pointers to video and audio buffers

	// Validate that the file is a H2O
	if (ret && GetInt(pData, 0, 4) == H2O)
	{
		_width = GetInt(pData, 4, 2);
		_height = GetInt(pData, 8, 2);
		_rate = GetInt(pData, 16, 2);
		_frameTime = _rate;

		CreateBuffers(_width, _height);
		_texture.Init(_width, _height);

		// Find video and audio pointers
		_framePointer = 0x40;
		if (_width > 0 && _height > 0)
		{
			_videoFramePointer = 0x40;
		}

		// Can there be videos without audio?
		_audioFramePointer = 0x40;

		/*
			int framePtr = 0x40;
			// Extract audio frames
			int channels = 0, depth = 0, remainingLength = 0;
			byte[] outputBuffer = null;
			int outputOffset = 0;
			int frameOffset = 0x40;
			bool audioCompressed = (_data.GetInt(0x2c, 4) != 0);
			while (frameOffset < _data.Length)
			{
				int frameSize = _data.GetInt(frameOffset, 3);
				int frameFlags = _data[frameOffset + 3];

				int chunkOffset = frameOffset + 4;
				if ((frameFlags & 0x40) != 0)
				{
					chunkOffset += 4 + _data.GetInt(chunkOffset, 4);
				}
				if ((frameFlags & 0x80) != 0)
				{
					chunkOffset += 4 + _data.GetInt(chunkOffset, 4);
				}
				if ((frameFlags & 0x01) != 0)
				{
					chunkOffset += 4 + _data.GetInt(chunkOffset, 4);
				}
				if ((frameFlags & 0x02) != 0)
				{
					chunkOffset += 4 + _data.GetInt(chunkOffset, 4);
				}
				if ((frameFlags & 0x04) != 0)
				{
					// Audio frame

					int chunkSize = _data.GetInt(chunkOffset, 4);

					int dataOffset = chunkOffset + 4;
					if (_data.GetInt(dataOffset, 4) == 0x46464952)
					{
						// RIFF header, extract required properties
						channels = _data.GetInt(dataOffset + 0x16, 2);
						depth = _data.GetInt(dataOffset + 0x22, 2);
						remainingLength = _data.GetInt(dataOffset + 0x28, 4);

						outputBuffer = new byte[0x2c + remainingLength];
						Array.Copy(_data, dataOffset, outputBuffer, 0, 0x2c);
						outputOffset = 0x2c;
						dataOffset += 0x2c;

						chunkSize -= 0x2c;

						Debug.WriteLine($"{channels} Channel(s), {depth} bits per sample");
					}

					if (outputBuffer != null)
					{
						if (audioCompressed)
						{
							outputOffset += DecodeAudioFrame(_data, dataOffset, chunkOffset + chunkSize, outputBuffer, outputOffset);
						}
						else
						{
							Array.Copy(_data, dataOffset, outputBuffer, outputOffset, chunkSize);
							outputOffset += chunkSize;
						}
					}
				}

				frameOffset += frameSize + 4;
			}

			result.Add(new KeyValuePair<int, byte[]>(0, outputBuffer));

			return result;
		*/

		/*
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

		if (_audioFramePointer != NULL)
		{
			// Prepare for audio playback
		}
		*/
	}

	return ret;
}

BOOL CH2O::DecodeFrame()
{
	BOOL ret = FALSE;

	_frame++;

	// TODO: If this is an audio-only H2O, should extract all audio bytes to prevent clicking
	if (_videoFramePointer == NULL)
	{
		//Trace(L"H2O Audio-Only Frame ");
		//TraceLine(_frame);

		ret = TRUE;

		if (_audioFramePointer > 0)
		{
			int channels = 0, depth = 0, remainingLength = 0;
			int frameOffset = _audioFramePointer;
			int outputOffset = 0;
			BOOL audioCompressed = (GetInt(_pInputBuffer, 0x2c, 4) != 0);
			// Extract audio frames
			LPBYTE outputBuffer = NULL;
			while (frameOffset < _inputBufferLength)
			{
				int frameSize = GetInt(_pInputBuffer, frameOffset, 3);
				int frameFlags = _pInputBuffer[frameOffset + 3];

				int chunkOffset = frameOffset + 4;
				if ((frameFlags & 0x40) != 0)
				{
					chunkOffset += 4 + GetInt(_pInputBuffer, chunkOffset, 4);
				}
				if ((frameFlags & 0x80) != 0)
				{
					chunkOffset += 4 + GetInt(_pInputBuffer, chunkOffset, 4);
				}
				if ((frameFlags & 0x01) != 0)
				{
					chunkOffset += 4 + GetInt(_pInputBuffer, chunkOffset, 4);
				}
				if ((frameFlags & 0x02) != 0)
				{
					chunkOffset += 4 + GetInt(_pInputBuffer, chunkOffset, 4);
				}
				if ((frameFlags & 0x04) != 0)
				{
					// Audio frame

					int chunkSize = GetInt(_pInputBuffer, chunkOffset, 4);

					int dataOffset = chunkOffset + 4;
					if (GetInt(_pInputBuffer, dataOffset, 4) == RIFF)
					{
						// RIFF header, extract required properties
						channels = GetInt(_pInputBuffer, dataOffset + 0x16, 2);
						depth = GetInt(_pInputBuffer, dataOffset + 0x22, 2);
						remainingLength = GetInt(_pInputBuffer, dataOffset + 0x28, 4);

						outputBuffer = new BYTE[0x1002c + remainingLength];
						ZeroMemory(outputBuffer, 0x1002c + remainingLength);
						// Copy RIFF header
						CopyMemory(outputBuffer, _pInputBuffer + dataOffset, 0x2c);
						outputOffset = 0x2c;
						dataOffset += 0x2c;

						chunkSize -= 0x2c;
					}

					if (outputBuffer != NULL)
					{
						if (audioCompressed)
						{
							outputOffset += DecodeH2OAudio(_pInputBuffer + dataOffset, outputBuffer + outputOffset, chunkSize);
						}
						else
						{
							CopyMemory(outputBuffer + outputOffset, _pInputBuffer + dataOffset, chunkSize);
							outputOffset += chunkSize;
						}
					}
				}

				frameOffset += frameSize + 4;
			}

			if (_sourceVoice == NULL)
			{
				char formatBuff[64];
				WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
				pwfx->wFormatTag = GetInt(outputBuffer, 0x14, 2);// WAVE_FORMAT_PCM;
				pwfx->nChannels = GetInt(outputBuffer, 0x16, 2);
				pwfx->nSamplesPerSec = GetInt(outputBuffer, 0x18, 4);
				pwfx->nAvgBytesPerSec = GetInt(outputBuffer, 0x1c, 4);
				pwfx->nBlockAlign = 2;
				pwfx->wBitsPerSample = GetInt(outputBuffer, 0x22, 2);
				pwfx->cbSize = 0;
				_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
			}
			_sourceVoice->Start(0, 0);

			Buffer ab;
			ab.Size = remainingLength - 0x2c;
			ab.pData = outputBuffer + 0x2c;
			_remainingAudioLength = 0;
			_audioBuffers.push_back(ab);

			/*
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
			*/
		}

		if (_sourceVoice != NULL)
		{
			// Enqueue a couple of buffers	XAUDIO2_MAX_QUEUED_BUFFERS=64
			int buffers = _audioBuffers.size();
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

	return FALSE;
}

int ReadBits(LPBYTE data, int bitsToRead, int& bitOffset, bool& commandFlag)
{
	int byteOffset = bitOffset >> 3;
	int bitsUsed = bitOffset & 7;
	int value = GetInt(data, byteOffset, 3) >> bitsUsed;
	int mask = (1 << bitsToRead) - 1;
	int signMask = 1 << (bitsToRead - 1);
	int signExtension = -1 << bitsToRead;

	int maskedValue = (value & mask);
	commandFlag = (maskedValue == signMask);// ((maskedValue & signMask) != 0 && (maskedValue & ~signMask) == 0);

	int ret = maskedValue;
	if ((value & signMask) != 0)
	{
		ret |= signExtension;
	}

	bitOffset += bitsToRead;

	return ret;
}

void Write(LPBYTE output, int& outputOffset, int value, int size)
{
	//if (outputOffset < output.Length)
	{
		output[outputOffset] = (byte)value;
		if (size == 2)// && (outputOffset + 1) < output.Length)
		{
			output[outputOffset + 1] = (byte)(value >> 8);
		}
	}

	outputOffset += size;
}

int CH2O::DecodeH2OAudio(LPBYTE source, LPBYTE destination, int chunkLength)
{
	//	public int DecodeAudioFrame(byte[] input, int inputOffset, int inputEndOffset, byte[] output, int outputOffset)
	int outputOffsetStart = 0;
	int outputOffset = 0;
	int inputOffset = 0;

	int readBitsCount = inputOffset * 8;
	int endOffset = chunkLength;
	bool commandFlag = false;
	int flags = ReadBits(source, 4, readBitsCount, commandFlag);
	int channelCount = ((flags & 2) != 0) ? 2 : 1;
	int bytesPerSample = ((flags & 4) != 0) ? 1 : 2;
	int bitsPerSample = bytesPerSample * 8;

	int audioAmplifier = ReadBits(source, 4, readBitsCount, commandFlag) & 15;

	int channelBitsPerSample[2];
	int channelPreviousSamples[2];

	channelBitsPerSample[0] = 1 + (ReadBits(source, 4, readBitsCount, commandFlag) & 15);
	channelPreviousSamples[0] = ReadBits(source, bitsPerSample, readBitsCount, commandFlag);
	Write(destination, outputOffset, channelPreviousSamples[0] << audioAmplifier, bytesPerSample);

	if (channelCount == 2)
	{
		// Read second channel data
		channelBitsPerSample[1] = 1 + (ReadBits(source, 4, readBitsCount, commandFlag) & 15);
		channelPreviousSamples[1] = ReadBits(source, bitsPerSample, readBitsCount, commandFlag);
		Write(destination, outputOffset, channelPreviousSamples[1] << audioAmplifier, bytesPerSample);
	}

	int channelRepeatCounters[2];
	channelRepeatCounters[0] = channelRepeatCounters[1] = 0;
	int channel = 0;
	// outerloop
	while (true && (readBitsCount >> 3) < endOffset)
	{
		bool fullBreak = false;

		int sampleValue = 0;
		if (channelRepeatCounters[channel] == 0)
		{
			// middleloop
			while (true)
			{
				// Read new sample
				sampleValue = ReadBits(source, channelBitsPerSample[channel], readBitsCount, commandFlag);
				if (channelBitsPerSample[channel] == bitsPerSample)
				{
					if (bytesPerSample == 1)
					{
						sampleValue &= 0xff;
					}
				}
				else
				{
					sampleValue += channelPreviousSamples[channel];
				}

				// innerloop
				if (commandFlag)
				{
					flags = ReadBits(source, 2, readBitsCount, commandFlag) & 3;
					if (flags == 0)
					{
						// continue13
						int innertemp = ReadBits(source, 4, readBitsCount, commandFlag) & 15;

						if (innertemp == 0)
						{
							// exit
							return outputOffset - outputOffsetStart;
						}
						else if (innertemp < 2)
						{
							channelRepeatCounters[channel] = ReadBits(source, 8, readBitsCount, commandFlag) & 0xff;
							fullBreak = true;
							break;
						}
						else
						{
							channelBitsPerSample[channel] = innertemp + 1;
						}
					}
					else if (flags == 1)
					{
						// continue11
						// New channel bit size
						channelBitsPerSample[channel] = min(16, channelBitsPerSample[channel] + 1);
					}
					else if (flags == 2)
					{
						// continue12
						// New channel bit size
						channelBitsPerSample[channel] = max(2, channelBitsPerSample[channel] - 1);
					}
					else
					{
						// continue8
						break;
					}
				}
				else
				{
					// continue8
					break;
				}
			}
		}
		else
		{
			// Re-use previous sample value
			channelRepeatCounters[channel]--;
			sampleValue = channelPreviousSamples[channel];
		}

		if (!fullBreak)
		{
			// continue8
			channelPreviousSamples[channel] = sampleValue;

			// Write amplified sample value
			Write(destination, outputOffset, sampleValue << audioAmplifier, bytesPerSample);
			channel++;
			if (channel >= channelCount)
			{
				channel = 0;
			}
		}
	}

	return outputOffset - outputOffsetStart;
}

