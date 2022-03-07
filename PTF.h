#pragma once

#include "BIC.h"
#include <Windows.h>

class CPTF : public CBIC
{
public:
	CPTF(int factor = 1) : CBIC(factor) { }
	virtual BOOL Init(LPBYTE pData, int length);
	virtual BOOL HasVideo() { return (_videoFramePointer != NULL); }

protected:
	virtual BOOL ProcessFLCFrame(int inPtr, int chunkSize);
	virtual BOOL DecodeFrame();
};
