#pragma once

#include "BIC.h"
#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

class CPTF : public CBIC
{
public:
	CPTF(int factor = 1) : CBIC(factor) { }
	virtual BOOL Init(LPBYTE pData, int length);
	virtual BOOL HasVideo() { return (_videoFramePointer != 0); }

protected:
	virtual BOOL ProcessFLCFrame(int inPtr, int chunkSize);
	virtual BOOL DecodeFrame();
};
