#pragma once

#include "AnimBase.h"

class CH2O : public CAnimBase
{
public:
	CH2O();
	~CH2O();

	virtual BOOL Init(LPBYTE pData, int length);
	virtual BOOL HasVideo() { return (_videoFramePointer != NULL); }

protected:
	virtual BOOL DecodeFrame();

	virtual int DecodeH2OAudio(LPBYTE source, LPBYTE destination, int chunkLength);
};
