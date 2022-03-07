#pragma once

#include <windows.h>

class CColourTranslationTable
{
public:
	CColourTranslationTable()
	{
		for (int i = 0; i < 64; i++)
		{
			_colourTranslationTable[i] = (BYTE)(4.04762 * i);
		}
	}

	BYTE operator[] (int index) const
	{
		return _colourTranslationTable[index];
	}

protected:
	BYTE _colourTranslationTable[64];
};
