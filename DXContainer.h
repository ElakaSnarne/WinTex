#pragma once

#include "DXControl.h"
#include <list>
#include "DXControls.h"
#include "Globals.h"

class CDXContainer : public CDXControl
{
	friend class CDXControl;

public:
	CDXContainer();
	virtual ~CDXContainer();

	virtual void Render();

	BOOL IsModal();

	CDXControl* GetCurrentMouseOver();

	void AddChild(CDXControl* pCtrl, float x, float y);
	virtual CDXControl* HitTest(float x, float y);

	virtual CDXBitmap* AddBitmap(PWCHAR fileName, Alignment alignment = Alignment::Default);
	virtual CDXBitmap* AddBitmap(PBYTE pImg, DWORD size, Alignment alignment = Alignment::Default);
	virtual CDXButton* AddButton(char* text, float x, float y, float w, float h, void(*onClick)(LPVOID data));

	void ShowModal(CDXControl* pControl);
	CDXControl* GetModal();
	void PopModal();

protected:
	std::list<CDXControl*> _childElements;
	std::list<CDXControl*> _modalElements;
};
