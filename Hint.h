#pragma once

#include <string>
#include "DXMultiColouredText.h"

class CHint
{
public:
	CHint(int hintIndex, std::wstring text);
	~CHint();

	int GetHintIndex() { return _hintIndex; }

	int GetState() { return _state; }
	void SetState(int state) { _state = state; }

	void Render(float x, float y);

	void Prepare(int state, int col1, int col2, int col3, int col4, Rect hintRect);
	int Height() { return _mcText.Height(); }

protected:
	int _hintIndex;
	std::wstring _text;
	int _state;
	CDXMultiColouredText _mcText;
};
