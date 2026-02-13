#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include "BinaryData.h"

class CLZ
{
public:
	static BinaryData Decompress(LPBYTE pInput, int length);
	static BinaryData Decompress(LPBYTE pInput, int offset, int length);
	static BinaryData Decompress(LPWSTR pFileName);

	static BOOL IsCompressed(LPBYTE pInput, int length);
	static BOOL IsCompressed(LPBYTE pInput, int offset, int length);
};
