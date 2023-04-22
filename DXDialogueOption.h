#pragma once

#include "DXButton.h"
#include <string>

class CDXDialogueOption :
	public CDXButton
{
public:
	CDXDialogueOption();
	~CDXDialogueOption();

	static void Init();
	static void Dispose();

	void SetClick(void(*Clicked)(LPVOID data));
	void SetText(char* text, Size sz, float x);

	virtual void Render();

	void Clear();

	void Resize(int index, int width, int height);

	void SetValue(int value) { _value = value; }
	int GetValue() { return _value; }

protected:
	static CTexture _texBubble;
	std::string _text;
	Size _sz;
	int _value;
};
