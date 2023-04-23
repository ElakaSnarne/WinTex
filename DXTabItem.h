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

	virtual void SetColours(int colour1, int colour2, int colour3, int colour4);

private:
	static CTexture _texBackgroundTabItem;

	CDXTabControl* _pOwner;

	int _colour1;
	int _colour2;
	int _colour3;
	int _colour4;
};
