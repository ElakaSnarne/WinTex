#include "H2O.h"
#include "Utilities.h"
#include "MediaIdentifiers.h"

CH2O::CH2O()
{
	_channels = 0;
	_depth = 0;
	_remainingLength = 0;
	_audioCompressed = FALSE;

	for (int i = 0; i < H2O_MAX_AUDIO_BUFFERS; i++)
	{
		_audioOutputBuffer[i] = new BYTE[H2O_AUDIO_BUFFER_SIZE];
		ZeroMemory(_audioOutputBuffer[i], H2O_AUDIO_BUFFER_SIZE);
	}
	_audioOutputBufferIndex = 0;

	_minimumBitCount = 0;
	_pDecodingTable = new int[65536];
	ZeroMemory(_pDecodingTable, sizeof(_pDecodingTable));
	_pDecodingBuffer = new BYTE[1048576];
	ZeroMemory(_pDecodingBuffer, sizeof(_pDecodingBuffer));
	_decodedSize = 0;

	_startAudioOnFrame = -1;
}

CH2O::~CH2O()
{
	for (int i = 0; i < H2O_MAX_AUDIO_BUFFERS; i++)
	{
		delete[] _audioOutputBuffer[i];
	}

	if (_pDecodingTable != NULL)
	{
		delete[] _pDecodingTable;
		_pDecodingTable = NULL;
	}

	if (_pDecodingBuffer != NULL)
	{
		delete[] _pDecodingBuffer;
		_pDecodingBuffer = NULL;
	}
}

BOOL CH2O::Init(LPBYTE pData, int length)
{
	BOOL ret = CAnimBase::Init(pData, length);

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

		_audioCompressed = (GetInt(_pInputBuffer, 0x2c, 4) != 0);
	}

	return ret;
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
	::Write(destination, outputOffset, channelPreviousSamples[0] << audioAmplifier, bytesPerSample);

	if (channelCount == 2)
	{
		// Read second channel data
		channelBitsPerSample[1] = 1 + (ReadBits(source, 4, readBitsCount, commandFlag) & 15);
		channelPreviousSamples[1] = ReadBits(source, bitsPerSample, readBitsCount, commandFlag);
		::Write(destination, outputOffset, channelPreviousSamples[1] << audioAmplifier, bytesPerSample);
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
			::Write(destination, outputOffset, sampleValue << audioAmplifier, bytesPerSample);
			channel++;
			if (channel >= channelCount)
			{
				channel = 0;
			}
		}
	}

	return outputOffset - outputOffsetStart;
}

BOOL CH2O::ProcessFrame(int& offset, BOOL video)
{
	if (offset > 0 && offset < _inputBufferLength)
	{
		BOOL initialAudioBuffer = FALSE;

		int frameSize = GetInt(_pInputBuffer, offset, 3);
		int frameFlags = _pInputBuffer[offset + 3];
		int chunkOffset = offset + 4;
		if ((frameFlags & 0x80) != 0)
		{
			// Unknown
			int chunkSize = GetInt(_pInputBuffer, chunkOffset, 4);
			chunkOffset += chunkSize + 4;
		}
		if ((frameFlags & 0x40) != 0)
		{
			// Bit-unpacking table
			int chunkSize = GetInt(_pInputBuffer, chunkOffset, 4);
			if (video)
			{
				int tag40Offset = chunkOffset + 4;

				int t40v1 = _pInputBuffer[tag40Offset++] & 0xff;
				int t40v2 = _pInputBuffer[tag40Offset++] & 0xff;
				_minimumBitCount = t40v1;

				int t40Counter = 0;

				while (t40v1 <= t40v2)
				{
					_pDecodingTable[t40v1 * 3] = t40Counter;
					int blockCount = GetInt(_pInputBuffer, tag40Offset, 2) & 0xffff;
					t40Counter += blockCount;
					_pDecodingTable[t40v1 * 3 + 1] = t40Counter - 1;
					_pDecodingTable[t40v1 * 3 + 2] = tag40Offset + 2;

					t40v1++;

					tag40Offset += blockCount * 2 + 2;

					t40Counter <<= 1;

					if (t40v1 > 100)
					{
						int disaster = 0;
					}
				}
			}
			chunkOffset += chunkSize + 4;
		}
		if ((frameFlags & 0x01) != 0)
		{
			// Palette
			int chunkSize = GetInt(_pInputBuffer, chunkOffset, 4);
			if (video)
			{
				// Palette, extract
				int paletteOffset = chunkOffset + 4;

				int blockCount = GetInt(_pInputBuffer, paletteOffset, 2);
				paletteOffset += 2;
				int currentIndex = 0;
				for (int b = 0; b < blockCount; b++)
				{
					currentIndex += _pInputBuffer[paletteOffset++];
					int colourCount = _pInputBuffer[paletteOffset++];
					if (colourCount == 0)
					{
						colourCount = 256;
					}
					for (int c = 0; c < colourCount; c++)
					{
						int r = _colourTranslationTable[_pInputBuffer[paletteOffset++]];
						int g = _colourTranslationTable[_pInputBuffer[paletteOffset++]];
						int b = _colourTranslationTable[_pInputBuffer[paletteOffset++]];

						//if (videoMode == VideoMode::Embedded && j < 32)
						//{
						//	r = g = b = 0;
						//}

						int col = 0xff000000 | b | (g << 8) | (r << 16);
						_pPalette[currentIndex++] = col;
					}
				}
			}

			chunkOffset += chunkSize + 4;
		}
		if ((frameFlags & 0x02) != 0)
		{
			// Video
			int chunkSize = GetInt(_pInputBuffer, chunkOffset, 4);
			if (video)
			{
				int videoOffset = chunkOffset + 4;

				// Unpack video
				Unpack(videoOffset, chunkSize);

				// Decode video
				_qw = _width / 4;
				int qh = _height / 4;
				_inputOffset = 0;
				_x = 0;
				_y = 0;
				_remainingX = _qw;
				_remainingY = qh;
				while (_inputOffset < _decodedSize && _remainingY > 0)
				{
					int val = (GetInt(_pDecodingBuffer, _inputOffset, 2)) & 0xffff;
					_inputOffset += 2;

					if (val < 0x4000)
					{
						SkipOrFill(val);
					}
					else if (val < 0x5000)
					{
						PatternFill(val);
					}
					else if (val < 0x6000)
					{
						PatternCopy(val);
					}
					else
					{
						int bug = 0;
						break;
					}
				}
			}
			chunkOffset += chunkSize + 4;
		}

		if ((frameFlags & 0x04) != 0)
		{
			// Audio
			if (!video)
			{
				int chunkSize = GetInt(_pInputBuffer, chunkOffset, 4);
				int dataOffset = chunkOffset + 4;
				int outputOffset = 0;
				int riffOffset = 0;
				if (GetInt(_pInputBuffer, dataOffset, 4) == RIFF)
				{
					// RIFF header, extract required properties
					riffOffset = dataOffset;
					_channels = GetInt(_pInputBuffer, dataOffset + 0x16, 2);
					_depth = GetInt(_pInputBuffer, dataOffset + 0x22, 2);
					_remainingLength = GetInt(_pInputBuffer, dataOffset + 0x28, 4);

					dataOffset += 0x2c;
					chunkSize -= 0x2c;

					initialAudioBuffer = TRUE;

					_startAudioOnFrame = (_width > 0 && _height > 0) ? _frame + 4 : _frame;
				}

				if (_audioCompressed)
				{
					outputOffset += DecodeH2OAudio(_pInputBuffer + dataOffset, _audioOutputBuffer[_audioOutputBufferIndex] + outputOffset, chunkSize);
				}
				else
				{
					CopyMemory(_audioOutputBuffer[_audioOutputBufferIndex], _pInputBuffer + dataOffset, chunkSize);
					outputOffset += chunkSize;
				}

				Buffer ab;
				ab.Frame = _frame;
				ab.Size = outputOffset;
				ab.pData = _audioOutputBuffer[_audioOutputBufferIndex];
				if (_audioBuffers.size() >= H2O_MAX_AUDIO_BUFFERS)
				{
					Trace(L"WARNING! Too many buffers in use: ");
					TraceLine((int)_audioBuffers.size());
				}
				_audioBuffers.push_back(ab);

				_remainingAudioLength -= outputOffset;

				if (_sourceVoice == NULL)
				{
					char formatBuff[64];
					WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
					pwfx->wFormatTag = GetInt(_pInputBuffer, riffOffset + 0x14, 2);
					pwfx->nChannels = _channels;
					pwfx->nSamplesPerSec = GetInt(_pInputBuffer, riffOffset + 0x18, 4);
					pwfx->nAvgBytesPerSec = GetInt(_pInputBuffer, riffOffset + 0x1c, 4);
					pwfx->nBlockAlign = 2;
					pwfx->wBitsPerSample = _depth;
					pwfx->cbSize = 0;
					_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
				}

				_audioOutputBufferIndex++;
				if (_audioOutputBufferIndex >= H2O_MAX_AUDIO_BUFFERS)
				{
					_audioOutputBufferIndex = 0;
				}
			}
		}

		if (_sourceVoice != NULL && _startAudioOnFrame > 0 && _frame == _startAudioOnFrame)
		{
			_sourceVoice->Start(0, 0);
			_startAudioOnFrame = -1;
		}

		offset += frameSize + 4;

		// This is the initial audio frame, should enqueue a few to avoid clicking
		if (!video && initialAudioBuffer && _sourceVoice != NULL)
		{
			Trace(L"Initial audio at frame ");
			TraceLine(_frame);

			for (int i = 0; i < H2O_MAX_AUDIO_BUFFERS / 2; i++)
			{
				if (!ProcessFrame(offset, video))
				{
					break;
				}
			}

			auto buffers = _audioBuffers.size();
			for (std::size_t i = 0; i < buffers && i < H2O_MAX_AUDIO_BUFFERS; i++)
			{
				Buffer ab = _audioBuffers.front();
				_audioBuffers.pop_front();

				XAUDIO2_BUFFER buf = { 0 };
				buf.AudioBytes = ab.Size;
				buf.pAudioData = ab.pData;
				//if (_remainingAudioLength == 0) buf.Flags = XAUDIO2_END_OF_STREAM;

				Trace(L"Enqueing buffer ");
				Trace((int)i);
				Trace(L" at address ");
				Trace((long)ab.pData, 16);
				TraceLine(L"");

				_sourceVoice->SubmitSourceBuffer(&buf);

				_audioFramesQueued++;
			}
		}

		return ((video && (frameFlags & 0xc3) != 0) || (!video && (frameFlags & 0x4) != 0));
	}

	return FALSE;
}

BOOL CH2O::DecodeFrame()
{
	BOOL ret = ProcessFrame(_videoFramePointer, TRUE) | ProcessFrame(_audioFramePointer, FALSE);
	if (!ret)
	{
		_framePointer = _inputBufferLength;
	}

	return ret;
}

void CH2O::Unpack(int offset, int size)
{
	int bitPattern = GetInt(_pInputBuffer, offset, 4); // Bit pattern
	offset += 4;
	int availableBitCount = 32;   // Number of bits left

	int videoEnd = offset + size;
	_decodedSize = 0;
	while (offset <= videoEnd)
	{
		int baseIndex = _minimumBitCount;
		int bits = 0;
		int requiredBitCount = _minimumBitCount;
		if (availableBitCount <= requiredBitCount)
		{
			// Need more bits
			bits = (int)((bitPattern >> (32 - availableBitCount)) & (0xffffffff >> (32 - availableBitCount)));
			requiredBitCount -= availableBitCount;

			bitPattern = GetInt(_pInputBuffer, offset, 4);
			offset += 4;
			availableBitCount = 32;
		}

		if (requiredBitCount > 0)
		{
			bits <<= requiredBitCount;
			bits |= (int)((bitPattern >> (32 - requiredBitCount)) & (0xffffffff >> (32 - requiredBitCount)));
			bitPattern <<= requiredBitCount;
			availableBitCount -= requiredBitCount;
		}

		// Get correct table offset
		while (bits > _pDecodingTable[baseIndex * 3 + 1])
		{
			bits <<= 1;
			// Add one more bit

			bits |= ((bitPattern >> 31) & 1);
			bitPattern <<= 1;
			availableBitCount--;
			if (availableBitCount == 0)
			{
				bitPattern = GetInt(_pInputBuffer, offset, 4);
				offset += 4;
				availableBitCount = 32;
			}

			baseIndex++;
		}

		// Lookup in tag40 table
		int ix = bits - _pDecodingTable[baseIndex * 3];
		int tagOffset = _pDecodingTable[baseIndex * 3 + 2];
		if (ix >= 0 && tagOffset > 0)
		{
			// Write to output buffer
			_pDecodingBuffer[_decodedSize++] = _pInputBuffer[tagOffset + ix * 2];
			_pDecodingBuffer[_decodedSize++] = _pInputBuffer[tagOffset + ix * 2 + 1];
		}
		else
		{
			int bug = 0;
		}
	}
}

void CH2O::SkipOrFill(int val)
{
	int count = (val >> 8) & 0xff;
	if (count >= 59)
	{
		count = 128 << (count - 59);
	}
	else
	{
		count++;
	}

	if ((val & 0xff) == 0)
	{
		while (count > 0)
		{
			if (count < _remainingX)
			{
				_remainingX -= count;
				_x += count * 4;
				break;
			}
			else // count >= remainingX
			{
				count -= _remainingX;

				NewLine();
			}
		}
	}
	else
	{
		byte set = (byte)(val & 0xff);
		while (count > 0 && _remainingY > 0)
		{
			int blockCount = min(count, _remainingX);

			// Fill blockCount 4x4 blocks with value

			for (int b = 0; b < blockCount; b++)
			{
				for (int y = 0; y < 4; y++)
				{
					for (int x = 0; x < 4; x++)
					{
						_pVideoOutputBuffer[(_y + y) * _width + _x + b * 4 + x] = set;
					}
				}
			}

			_x += blockCount * 4;
			_remainingX -= blockCount;
			if (_remainingX <= 0)
			{
				NewLine();
			}

			count -= blockCount;
		}
	}
}

void CH2O::NewLine()
{
	_y += 4;
	_x = 0;
	_remainingX = _qw;
	_remainingY--;
}

void CH2O::PatternFill(int val)
{
	int lineCountOrByteCount = (val & 0xfff) + 1;
	while (lineCountOrByteCount > 0 && _remainingY > 0)//loop1
	{
		int count = min(lineCountOrByteCount, _remainingX);

		_remainingX -= count;
		lineCountOrByteCount -= count;

		while (count > 0)//loop2
		{
			int functions = GetInt(_pDecodingBuffer, _inputOffset, 2) & 0xffff;
			int pattern = GetInt(_pDecodingBuffer, _inputOffset + 2, 2) & 0xffff;
			_inputOffset += 4;
			Write(_x, _y, GetPattern(functions & 0xf, pattern));
			Write(_x, _y + 1, GetPattern((functions >> 4) & 0xf, pattern));
			Write(_x, _y + 2, GetPattern((functions >> 8) & 0xf, pattern));
			Write(_x, _y + 3, GetPattern((functions >> 12) & 0xf, pattern));
			count--;
			_x += 4;
		}

		if (_remainingX <= 0)
		{
			NewLine();
		}
	}
}

int CH2O::GetPattern(int function, int input)
{
	int pattern[2] = { input & 0xff, (input >> 8) & 0xff };

	int ret = 0;
	for (int i = 0; i < 4; i++)
	{
		ret <<= 8;
		ret |= pattern[(function >> (3 - i)) & 1];
	}

	return ret;
}

void CH2O::Write(int x, int y, int value)
{
	_pVideoOutputBuffer[y * _width + x + 0] = (byte)(value & 0xff);
	_pVideoOutputBuffer[y * _width + x + 1] = (byte)((value >> 8) & 0xff);
	_pVideoOutputBuffer[y * _width + x + 2] = (byte)((value >> 16) & 0xff);
	_pVideoOutputBuffer[y * _width + x + 3] = (byte)((value >> 24) & 0xff);
}

void CH2O::PatternCopy(int val)
{
	int lineCountOrByteCount = (val & 0xfff) + 1;
	while (lineCountOrByteCount > 0 && _remainingY > 0)//loop1
	{
		int count = min(lineCountOrByteCount, _remainingX);
		_remainingX -= count;
		lineCountOrByteCount -= count;

		while (count > 0 && _remainingY > 0)//loop2
		{
			int functions = GetInt(_pDecodingBuffer, _inputOffset, 2) & 0xffff;
			_inputOffset += 2;

			for (int i = 0; i < 4; i++)
			{
				int function = (functions >> (i * 4)) & 0xf;
				switch (function)
				{
				case 0:
				{
					// No function
					break;
				}
				case 1:
				{
					// Load and store single byte
					_pVideoOutputBuffer[(_y + i) * _width + _x] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 2:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 3:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 4:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 5:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 6:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 7:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 0] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 8:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 9:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 10:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 11:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 0] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 12:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 13:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 0] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 14:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				case 15:
				{
					_pVideoOutputBuffer[(_y + i) * _width + _x + 0] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 1] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 2] = _pDecodingBuffer[_inputOffset++];
					_pVideoOutputBuffer[(_y + i) * _width + _x + 3] = _pDecodingBuffer[_inputOffset++];
					break;
				}
				}
			}

			_x += 4;

			if ((_inputOffset & 1) != 0)
			{
				_inputOffset++;
			}

			count--;
		}

		if (_remainingX <= 0)
		{
			NewLine();
		}
	}
}
