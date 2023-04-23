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

	static void SetTextColours(int colour1, int colour2, int colour3, int colour4);
	static void SetHeaderColours(int colour1, int colour2, int colour3, int colour4);

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

	static int TextColour1;
	static int TextColour2;
	static int TextColour3;
	static int TextColour4;
	static int HeaderColour1;
	static int HeaderColour2;
	static int HeaderColour3;
	static int HeaderColour4;
};
