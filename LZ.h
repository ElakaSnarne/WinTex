#pragma once

#include <Windows.h>
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
