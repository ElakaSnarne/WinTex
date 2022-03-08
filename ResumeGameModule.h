#pragma once

#include "ModuleBase.h"
#include "D3D11-NoWarn.h"
#include "DXButton.h"
#include "AnimBase.h"
#include "DXScreen.h"
#include "DXFrame.h"
#include "DXLabel.h"

class CResumeGameModule : public CModuleBase
{
public:
	CResumeGameModule();
	virtual ~CResumeGameModule();

	virtual void Dispose();
	virtual void Render();
	virtual void Resize(int width, int height);

protected:
	virtual void Initialize();

	CDXScreen* _pScreen;
	CDXFrame* _pFrame;
	CDXLabel* _pLine1;
	CDXLabel* _pLine2;
	CDXButton* _pBtnYes;
	CDXButton* _pBtnNo;

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();

	virtual void KeyDown(WPARAM key, LPARAM lParam);

	void Yes();
	void No();
};
