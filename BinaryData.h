#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

struct BinaryData
{
public:
	LPBYTE Data;
	int Length;
};
