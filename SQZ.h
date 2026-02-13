#pragma once
#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include "Utilities.h"

class CSQZ
{
public:
	CSQZ();
	~CSQZ();

	static BinaryData Decompress(PBYTE input, int length);
};
