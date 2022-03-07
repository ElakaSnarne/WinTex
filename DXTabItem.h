#pragma once
#include "DXFrame.h"
#include <string>

class CDXTabControl;

class CDXTabItem : public CDXFrame
{
public:
	CDXTabItem(CDXTabControl* pOwner, LPSTR title, float w, float h);
	~CDXTabItem();

	virtual void Render(float x, float y, float hx, float hy, bool selected);
	virtual void MouseButtonDown();
	virtual CDXControl* HitTest(float x, float y);

	static void Init();
	static void Dispose();

	void Select();

private:
	static CTexture _texBackgroundTabItem;

	CDXTabControl* _pOwner;
};
