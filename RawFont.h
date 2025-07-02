#pragma once

#include "Texture.h"
#include <unordered_map>
#include "BinaryData.h"

class CRawFont : public CTexture
{
public:
	CRawFont();
	CRawFont(int resource);
	~CRawFont();

	void Init(int resource);
	void Init(BinaryData& data);
	void Render(LPBYTE screen, int screenWidth, int screenHeight, int x, int y, BYTE character, int colourBase, BOOL center = FALSE);
	RECT Render(LPBYTE screen, int screenWidth, int screenHeight, int x, int y, char* text, std::unordered_map<int, int> colourMap, int horizontalAdjustment = 0, int verticalAdjustment = 0, BOOL ignoreReturn = FALSE);

	int Measure(char* text, int horizontalAdjustment = 0);

	int GetHeight() { return _fontHeight; }
	int GetWidth(char character) { return _fontMap[character][0]; }

protected:
	LPBYTE _pFontData;
	int _fontDataSize;
	int _bitsPerPixel;
	int _fontHeight;

	std::unordered_map<char, LPBYTE> _fontMap;

	void MapAndCreateTexture(LPBYTE pFontData, int fontDataSize, BOOL createTexture = FALSE);
};
