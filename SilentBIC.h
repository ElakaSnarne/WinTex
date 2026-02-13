#pragma once
#include "AnimBase.h"
#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include "BIC.h"

class CSilentBIC : public CBIC
{
public:
	CSilentBIC(int factor = 1) : CBIC(factor) { }
	virtual BOOL Init(LPBYTE pData, int length);

protected:
	virtual BOOL DecodeFrame();
};
