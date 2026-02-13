#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include "Map.h"

class CUAKMMap : public CMap
{
public:
	virtual BOOL Init();
};
