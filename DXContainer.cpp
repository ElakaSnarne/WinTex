#include "DXContainer.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXBitmap.h"
#include <algorithm>

CDXContainer::CDXContainer()
{
}

CDXContainer::~CDXContainer()
{
	// Loop though list of child elements and delete
	std::list<CDXControl*>::iterator it = _childElements.begin();
	std::list<CDXControl*>::iterator end = _childElements.end();
	while (it != end)
	{
		delete* (it++);
	}
	_childElements.clear();
}

CDXBitmap* CDXContainer::AddBitmap(PWCHAR fileName, Alignment alignment)
{
	CDXBitmap* pBmp = new CDXBitmap(fileName, alignment);
	AddChild(pBmp, 0.0f, 0.0f);
	return pBmp;
}

CDXBitmap* CDXContainer::AddBitmap(PBYTE pImg, DWORD size, Alignment alignment)
{
	CDXBitmap* pBmp = new CDXBitmap(pImg, size, alignment);
	AddChild(pBmp, 0.0f, 0.0f);
	return pBmp;
}

CDXButton* CDXContainer::AddButton(char* text, float x, float y, float w, float h, void(*onClick)(LPVOID data))
{
	CDXButton* pBtn = new CDXButton(text, w, h, onClick);
	AddChild(pBtn, x, y);
	return pBtn;
}

void CDXContainer::AddChild(CDXControl* pCtrl, float x, float y)
{
	// Position is relative to parent...
	pCtrl->SetPosition(_x + x, _y + y);
	_childElements.push_back(pCtrl);
}

void CDXContainer::RemoveChild(CDXControl* pCtrl)
{
	_childElements.remove(pCtrl);
}

void CDXContainer::Render()
{
	dx.DisableZBuffer();
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	std::list<CDXControl*>::iterator it = _childElements.begin();
	std::list<CDXControl*>::iterator end = _childElements.end();
	while (it != end)
	{
		if ((*it)->GetVisible())
		{
			(*it)->Render();
		}
		it++;
	}

	dx.EnableZBuffer();
}

CDXControl* CDXContainer::HitTest(float x, float y)
{
	if (!_visible || !_enabled)
	{
		return NULL;
	}

	if (x >= _x && y >= _y && x < (_x + _w) && y < (_y + _h))
	{
		if (_modalElements.size() > 0)
		{
			CDXControl* pModal = _modalElements.front();
			if (pModal != NULL)
			{
				CDXControl* pCtrl = pModal->HitTest(x, y);
				if (pCtrl != NULL)
				{
					CDXControl::ControlType type = pCtrl->GetType();
					if (type == ControlType::Button || type == ControlType::ImageButton || type == ControlType::CheckBox || type == ControlType::SaveGameControl || type == ControlType::TabItem || type == ControlType::Control || type == ControlType::Slider)
					{
						return pCtrl;
					}
				}
			}
		}
		else
		{
			std::list<CDXControl*>::reverse_iterator it = _childElements.rbegin();
			std::list<CDXControl*>::reverse_iterator end = _childElements.rend();

			while (it != end)
			{
				CDXControl* pCtrl = (*it)->HitTest(x, y);
				if (pCtrl != NULL)
				{
					CDXControl::ControlType type = pCtrl->GetType();
					if (type == ControlType::Button || type == ControlType::ImageButton || type == ControlType::CheckBox || type == ControlType::SaveGameControl || type == ControlType::TabItem || type == ControlType::Control || type == ControlType::Slider)
					{
						return pCtrl;
					}
				}

				it++;
			}

			return this;
		}
	}

	return NULL;
}

void CDXContainer::ShowModal(CDXControl* pControl)
{
	_modalElements.push_front(pControl);
	pControl->SetVisible(TRUE);
}

void CDXContainer::PopModal()
{
	_modalElements.front()->SetVisible(FALSE);
	_modalElements.pop_front();
}

CDXControl* CDXContainer::GetCurrentMouseOver()
{
	if (_modalElements.size() > 0)
	{
		CDXControl* pModal = _modalElements.front();
		if (pModal != NULL)
		{
			CDXControl* pCurrent = pModal->GetCurrentMouseOver();
			if (pCurrent != NULL)
			{
				return pCurrent;
			}
		}

		return NULL;
	}

	std::list<CDXControl*>::iterator it = _childElements.begin();
	std::list<CDXControl*>::iterator end = _childElements.end();
	while (it != end)
	{
		CDXControl* pCurrent = (*it)->GetCurrentMouseOver();
		if (pCurrent != NULL)
		{
			return pCurrent;
		}

		it++;
	}

	return CDXControl::GetCurrentMouseOver();
}

BOOL CDXContainer::IsModal()
{
	return (_modalElements.size() > 0);
}

CDXControl* CDXContainer::GetModal()
{
	return _modalElements.size() > 0 ? _modalElements.front() : NULL;
}

void CDXContainer::SetColours(int colour1, int colour2, int colour3, int colour4)
{
	std::list<CDXControl*>::iterator it = _childElements.begin();
	while (it != _childElements.end())
	{
		(*it++)->SetColours(colour1, colour2, colour3, colour4);
	}

	it = _modalElements.begin();
	while (it != _modalElements.end())
	{
		(*it++)->SetColours(colour1, colour2, colour3, colour4);
	}
}
