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

protected:
	void Clear();

	static CTexture _texBackground;
	std::vector<int> _listBoxVerticeInfo;
	std::vector<ListBoxItem> _items;

	int _topIndex;
	int _visibleItemsCount;
	int _highlightIndex;
};
