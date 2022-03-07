#include "Hint.h"

CHint::CHint(int hintIndex, std::wstring text)
{
	_hintIndex = hintIndex;
	_text = text;
	_state = -1;
}

CHint::~CHint()
{
}

void CHint::Prepare(int state, int col1, int col2, int col3, int col4, Rect hintRect)
{
	if ((state & 1) == 0)
	{
		_mcText.SetColours(col1, col2, col3);
	}
	else
	{
		_mcText.SetColours(col4, col4, col4);
	}

	_mcText.SetText(_text.c_str(), hintRect);
}

void CHint::Render(float x, float y)
{
	_mcText.Render(x, y);
}
