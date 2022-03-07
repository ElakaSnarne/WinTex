#pragma once
#include <Windows.h>
#include "Utilities.h"

class CSQZ
{
public:
	CSQZ();
	~CSQZ();

	static BinaryData Decompress(PBYTE input, int length);
};
