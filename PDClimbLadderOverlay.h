#pragma once

#include "Overlay.h"
#include "DXScreen.h"
#include "DXFrame.h"
#include "DXLabel.h"
#include "DXButton.h"

class CPDClimbLadderOverlay : public COverlay
{
public:
	CPDClimbLadderOverlay();
	~CPDClimbLadderOverlay();

	virtual void KeyDown(WPARAM key, LPARAM lParam);
	virtual void Render();
	virtual void BeginAction();

protected:
	CDXFrame* _pFrame;
	CDXLabel* _pLine;
	CDXButton* _pBtnYes;
	CDXButton* _pBtnNo;

	void Yes();
	void No();

	void Initialize();
};
