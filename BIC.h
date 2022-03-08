#pragma once

#include "AnimBase.h"
#include <Windows.h>
#include "DXAnimation.h"

class CBIC : public CAnimBase
{
public:
	CBIC(int factor = 1) : CAnimBase() { _factor = factor; };
	virtual BOOL Init(LPBYTE pData, int length);

protected:
	virtual BOOL DecodeFrame();

	int _embeddedAudioSize{};
	int _firstAudioFrame{};
	int _chunkTest{};

	int ProcessBICFrame(int offset, int chunkSize);

	int _factor;
};
