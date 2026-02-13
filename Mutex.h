#pragma once

#include "Platform.h"
#if defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
#include <Windows.h>
#else
#include "Win32Compat.h"
#include <mutex>
#endif

class CMutex
{
public:
	CMutex();
	~CMutex();

	BOOL Lock(int timeout = 1000000);
	void Release();

protected:
#if defined(PLATFORM_WINDOWS) && !defined(PLATFORM_LINUX)
	HANDLE _hMutex;
#else
    std::recursive_timed_mutex _mutex;
#endif

	DWORD _currentThreadId;
	int _lockCount;
};
