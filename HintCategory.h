#pragma once

#include <string>
#include <list>
#include "Hint.h"
#include "DXMultiColouredText.h"

class CHintCategory
{
public:
	CHintCategory(int categoryIndex, std::wstring title);
	~CHintCategory();

	void AddHint(int hintIndex, std::wstring text);

	void Prepare(int colBlack, int colBlue, int colCategory, int colOrange, int colGreen, int colScore, int colHighlight, int colShade, Rect directoryRect, Rect categoryRect, Rect hintRect);
	void Render(float x, float y, bool directory);

	std::list<CHint*> Hints;

	float Width() { return _categoryText.Width(); }
	void Reset();

private:
	int _hintCategoryIndex;
	std::wstring _title;

	CDXMultiColouredText _categoryText;
	CDXText _categoryTextBlack;
};
