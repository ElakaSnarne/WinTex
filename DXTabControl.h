#pragma once
#include "DXContainer.h"
#include "DXTabItem.h"

class CDXTabControl : public CDXContainer
{
public:
	CDXTabControl(float w, float h);
	~CDXTabControl();

	virtual void AddChild(CDXControl* pCtrl, float x, float y);

	virtual void Render();
	virtual void MouseButtonDown();
	virtual CDXControl* HitTest(float x, float y);

	void Select(CDXTabItem* pItem);

private:
	CDXTabItem* _selectedItem;
};
