#pragma once

#include "DXText.h"
#include "D3D11-NoWarn.h"
#include "DirectX.h"

class CDXMultiColouredText : public CDXText
{
public:
	CDXMultiColouredText();
	~CDXMultiColouredText();

	void SetColours(int colour1, int colour2, int colour3, int colour4 = -1);
	virtual void Render(float x, float y);
	void SetText(LPCWSTR text, Rect rect);
	void SetTextPD(LPCWSTR text, Rect rect);
	void SetTextPD2(LPCWSTR text, Rect rect);

	class CMCWordList : public CWordList
	{
	public:
		CMCWordList() { Colour = 0; };
		CMCWordList(char* text, int chars, float pixels, int col);
		~CMCWordList() { };

		void Add(char* text, int chars, float pixels, int col);

		int Colour;

		float Red() { return ((float)((Colour >> 16) & 0xff)) / 255.0f; }
		float Green() { return ((float)((Colour >> 8) & 0xff)) / 255.0f; }
		float Blue() { return ((float)(Colour & 0xff)) / 255.0f; }
	};

	class CMCChar
	{
	public:
		char Char;
		int Colour;
		float Pixels;

		float Red() { return ((float)((Colour >> 16) & 0xff)) / 255.0f; }
		float Green() { return ((float)((Colour >> 8) & 0xff)) / 255.0f; }
		float Blue() { return ((float)(Colour & 0xff)) / 255.0f; }
	};

	class CMCWord
	{
	public:
		std::list<CMCChar> Characters;
		int Pixels;
	};
};
