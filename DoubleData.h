#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include "BinaryData.h"

struct DoubleData
{
public:
	BinaryData File1;
	BinaryData File2;
};
