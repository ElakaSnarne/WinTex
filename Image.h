#pragma once

#include "AnimBase.h"
#include "DoubleData.h"

class CImage : public CAnimBase
{
public:
	CImage(DoubleData dd, int width, int height, int factor = 1);
	CImage(LPBYTE palette, BinaryData bd, int width, int height, int factor = 1);
	virtual ~CImage();

	virtual BOOL Update();

protected:
	virtual BOOL DecodeFrame() { return (_frame == 0); }
	virtual void Init(LPBYTE palette, BinaryData bd, int width, int height, int factor);
};
