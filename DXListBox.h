#pragma once
#include "DXControl.h"
#include <list>
#include <vector>
#include "Texture.h"

class CDXListBox : public CDXControl
{
public:
	CDXListBox();
	virtual ~CDXListBox();

	static void Init();
	static void Dispose();

	void Init(std::vector<ListBoxItem> items, float btnX);

	virtual void Render();

	int HitTestLB(float x, float y);

	static void SetGreyColours(int colour1, int colour2, int colour3, int colour4) { _greyColour1 = colour1; _greyColour2 = colour2; _greyColour3 = colour3; _greyColour4 = colour4; }
	static void SetBlackColours(int colour1, int colour2, int colour3, int colour4) { _blackColour1 = colour1; _blackColour2 = colour2; _blackColour3 = colour3; _blackColour4 = colour4; }

protected:
	void Clear();

	static CTexture _texBackground;
	std::vector<int> _listBoxVerticeInfo;
	std::vector<ListBoxItem> _items;

	int _topIndex;
	int _visibleItemsCount;
	int _highlightIndex;

	static int _greyColour1;
	static int _greyColour2;
	static int _greyColour3;
	static int _greyColour4;
	static int _blackColour1;
	static int _blackColour2;
	static int _blackColour3;
	static int _blackColour4;
};
