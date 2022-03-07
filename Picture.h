#pragma once

#include "AnimBase.h"

class CPicture : public CAnimBase
{
public:
	CPicture();
	virtual ~CPicture();

	virtual BOOL Init(LPBYTE pData, int length);

protected:
	virtual BOOL DecodeFrame();
};
