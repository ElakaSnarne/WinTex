#pragma once
#include "AnimBase.h"

class CStaticImage : public CAnimBase
{
public:
	CStaticImage(int factor = 1);
	virtual ~CStaticImage();

	virtual BOOL Init(LPBYTE pData, int length);

protected:
	virtual BOOL DecodeFrame() { return FALSE; }
};
