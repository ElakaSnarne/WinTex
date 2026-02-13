#pragma once

#include "AnimBase.h"
#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

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
