#pragma once

#include <Windows.h>

class CMutex
{
public:
	CMutex();
	~CMutex();

	BOOL Lock(int timeout = 1000000);
	void Release();

protected:
	HANDLE _hMutex;

	DWORD _currentThreadId;
	int _lockCount;
};
