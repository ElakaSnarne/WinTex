#pragma once
#include "AnimBase.h"

class CWave : public CAnimBase
{
public:
	CWave();
	~CWave();

	virtual BOOL Init(LPBYTE pData, int length);
	virtual BOOL IsWave() { return TRUE; }
	virtual BOOL HasVideo() { return FALSE; }

protected:
	virtual BOOL DecodeFrame();

	ULONGLONG _timeOfStart;
};
