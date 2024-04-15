#pragma once

#include "Texture.h"

class CDXFont : public CTexture
{
public:
	CDXFont();
	CDXFont(PBYTE pFont, DWORD size);
	~CDXFont();

	void Init(PBYTE pFont, DWORD size);

	float* Widths() { return _widths; };
	float Y1() { return _y1; }
	float Y2() { return _y2; }
	float Height();

	float PixelWidth(char* text);
	float PixelWidth(char ch);
	Size GetSize(char* text, float maxw);

	std::list<PointUV> Create(LPCWSTR text, BOOL texFont);

	static void SelectRedFont();
	static void SelectGreenFont();
	static void SelectBlueFont();
	static void SelectYellowFont();
	static void SelectWhiteFont();
	static void SelectGreyFont();
	static void SelectBlackFont();
	static void SelectFontColour(int red, int green, int blue, float alpha = 1.0f);
	static void SelectFontColour(int red1, int green1, int blue1, int red2, int green2, int blue2, float alpha = 1.0f);
	static void SelectFontColour(float red, float green, float blue, float alpha = 1.0f);
	static void SelectFontColour(float red1, float green1, float blue1, float red2, float green2, float blue2, float alpha = 1.0f);
	static void SelectFontColour(int colour);
	static void SelectFontColour(int colour1, int colour2);
	static void SelectFontColour(int colour1, int colour2, int colour3, int colour4);

protected:
	float _widths[224];

	float _y1;
	float _y2;
	float _height;
};
