#pragma once

#include <D3D11.h>
#include "DirectX.h"

class CDXText : public CDXBase
{
public:
	enum class Alignment
	{
		Left = 0,
		Center = 1,
		Right = 2,
		Justify = 3,
		JustifyAlways = 4,
	};

	CDXText();
	~CDXText();

	void SetColours(int colour);
	void SetColours(int colour1, int colour2);
	void SetColours(int colour1, int colour2, int colour3, int colour4);

	virtual void Render(float x, float y);

	void SetText(char* text, Alignment alignment = Alignment::Left);
	void SetText(char* text, Rect rect, Alignment alignment = Alignment::Left);
	void SetText(LPCWSTR text, Alignment alignment = Alignment::Left);
	void SetText(LPCWSTR text, Rect rect, Alignment alignment = Alignment::Left);

	float PixelWidth(char* text);
	int Lines() { return _lines; }

	float Width();
	void Width(float w);
	float Height();

	class CWordList
	{
	public:
		CWordList();
		~CWordList();

		void Add(char* text, int chars, float pixels);
		CWordList* Next();
		float Pixels();
		char* Text();
		int Chars();

		void SetNext(CWordList* pNext) { _next = pNext; }

	protected:
		CWordList(char* text, int chars, float pixels);

		char* _text;
		int _chars;
		float _pixels;

		CWordList* _next;
		CWordList* _last;
	};

protected:
	ID3D11Buffer* _vertexBuffer;
	int _printableCharacters;
	int _lines;
	float _width;

	int _colour1;
	int _colour2;
	int _colour3;
	int _colour4;
};
