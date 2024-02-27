#include "DXTabControl.h"

CDXTabControl::CDXTabControl(float w, float h)
{
	_selectedItem = NULL;
	_w = w;
	_h = h;
}

CDXTabControl::~CDXTabControl()
{
}

void CDXTabControl::Render()
{
	auto elementCount = _childElements.size();
	float elementWidth = _w / elementCount;
	float x = _x;
	float y = _y + 5 + TexFont.Height() * pConfig->FontScale;

	for (auto child : _childElements)
	{
		CDXTabItem* tabItem = (CDXTabItem*)child;
		tabItem->Render(_x, _y, x, y, (_selectedItem == tabItem));
		x += elementWidth;
	}
}

void CDXTabControl::MouseButtonDown()
{
	int debug = 0;
}

CDXControl* CDXTabControl::HitTest(float x, float y)
{
	auto elementCount = _childElements.size();
	float elementWidth = _w / elementCount;
	float cx = _x;
	float scaledFontHeight = TexFont.Height() * pConfig->FontScale;
	float cy = _y + 5 + scaledFontHeight;

	for (auto child : _childElements)
	{
		float nx = cx + elementWidth;

		// Hit test header
		if (x >= cx && x <= nx && y >= cy && y <= (cy + scaledFontHeight + 8))
		{
			return child;
		}

		CDXTabItem* tabItem = (CDXTabItem*)child;
		if (tabItem == _selectedItem)
		{
			CDXControl* pHit = tabItem->HitTest(x, y);
			if (pHit != NULL)
			{
				return pHit;
			}
		}

		cx = nx;
	}

	return NULL;
}

void CDXTabControl::AddChild(CDXControl* pCtrl, float x, float y)
{
	pCtrl->SetPosition(_x + x, _y + y);
	_childElements.push_back(pCtrl);

	if (_selectedItem == NULL)
	{
		_selectedItem = (CDXTabItem*)pCtrl;
	}
}

void CDXTabControl::Select(CDXTabItem* pItem)
{
	_selectedItem = pItem;
}
