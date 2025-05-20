#pragma once

#include <Windows.h>
#include <unordered_map>
#include "BinaryData.h"

class CTexFont
{
public:
	CTexFont();
	~CTexFont();

	void Init(BinaryData& data);
	void Render(LPBYTE screen, int screenWidth, int screenHeight, int x, int y, BYTE character, int colourBase, BOOL center = FALSE);

protected:
	LPBYTE _pFontData;
	int _fontDataSize;
	std::unordered_map<char, LPBYTE> _fontMap;

	int _fontHeight;
	int _bitsPerPixel;
};
