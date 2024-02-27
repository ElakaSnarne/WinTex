#pragma once
#include "AnimBase.h"
#include <Windows.h>
#include "BIC.h"

class CSilentBIC : public CBIC
{
public:
	CSilentBIC(int factor = 1) : CBIC(factor) { }
	virtual BOOL Init(LPBYTE pData, int length);

protected:
	virtual BOOL DecodeFrame();
};
