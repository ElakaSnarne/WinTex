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
	void RemoveChild(CDXControl* pCtrl);
	virtual CDXControl* HitTest(float x, float y);

	virtual CDXBitmap* AddBitmap(PWCHAR fileName, Alignment alignment = Alignment::Default);
	virtual CDXBitmap* AddBitmap(PBYTE pImg, DWORD size, Alignment alignment = Alignment::Default);
	virtual CDXButton* AddButton(char* text, float x, float y, float w, float h, void(*onClick)(LPVOID data));

	void ShowModal(CDXControl* pControl);
	CDXControl* GetModal();
	void PopModal();

	CDXControl* GetChild(int index)
	{
		if (_childElements.size() >= index)
		{
			std::list<CDXControl*>::iterator it = _childElements.begin();
			for (int i = 0; i < index; i++)
			{
				it++;
			}

			return *it;
		}

		return NULL;
	}

	virtual void SetColours(int colour1, int colour2, int colour3, int colour4);

protected:
	std::list<CDXControl*> _childElements;
	std::list<CDXControl*> _modalElements;
};
