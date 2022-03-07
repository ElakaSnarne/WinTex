#pragma once

#include <Windows.h>
#include "Mutex.h"

class CSimpleAnimPlayer
{
public:
	CSimpleAnimPlayer();
	virtual ~CSimpleAnimPlayer();

	void Init(LPBYTE pData);
	void Merge(LPBYTE pData);

	BOOL DecodeFrame(LPBYTE pScreen, int x, int y, int w);

protected:
	int _currentFrame;
	BOOL _firstFrameFull;
	int _animationFrames;
	int _animationWidth;
	int _animationHeight;
	LPBYTE _animationPointer;
	LPBYTE _dataPointer;
	CMutex _lock;
};
