#pragma once

#include "AnimBase.h"

class CH2O : public CAnimBase
{
public:
	CH2O(int factor = 1);
	~CH2O();

	virtual BOOL Init(LPBYTE pData, int length);
	virtual BOOL HasVideo() { return (_videoFramePointer != NULL); }

protected:
	int _factor;

	virtual BOOL DecodeFrame();

	virtual int DecodeH2OAudio(LPBYTE source, LPBYTE destination, int chunkLength);

	BOOL ProcessFrame(int& offset, BOOL video);

	int _channels;
	int _depth;
	int _remainingLength;
	BOOL _audioCompressed;

	LPBYTE* _ppAudioOutputBuffers;
	int _audioOutputBufferIndex;

	int _minimumBitCount;
	int* _pDecodingTable;
	LPBYTE _pDecodingBuffer;
	int _decodedSize;

	void Unpack(int offset, int size);

	void SkipOrFill(int val);
	void PatternFill(int val);
	int GetPattern(int function, int input);
	void Write(int x, int y, int value);
	void PatternCopy(int val);
	void NewLine();

	int _inputOffset;
	int _x;
	int _y;
	int _remainingX;
	int _remainingY;
	int _qw;

	int _startAudioOnFrame;
};
