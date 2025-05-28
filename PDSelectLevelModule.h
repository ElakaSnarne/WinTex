#pragma once

#include "ModuleBase.h"
#include "DXMultiColouredText.h"
#include "DXCheckBox.h"
#include "DXButton.h"
#include "DXFrame.h"

class CPDSelectLevelModule : public CModuleBase
{
public:
	CPDSelectLevelModule(BYTE* pGameData);
	~CPDSelectLevelModule();

	virtual void Resize(int width, int height);
	virtual void Dispose();
	virtual void Render();
	virtual void KeyDown(WPARAM key, LPARAM lParam);
	virtual void Cursor(float x, float y, BOOL relative);

protected:
	virtual void Initialize();

	// Input related
	virtual void BeginAction();
	virtual void Back();

	BYTE* _gameData;
	void NewGame();

	CDXMultiColouredText _line1;
	CDXMultiColouredText _line2;
	CDXMultiColouredText _line3;
	CDXMultiColouredText _line4;

	// Checkbox buttons for Level (Entertainment/Game player)
	CDXCheckBox* _pCBEntertainment;
	CDXCheckBox* _pCBGamePlayer;

	// Buttons for OK & Cancel
	CDXButton* _pBtnOK;
	CDXButton* _pBtnCancel;

	BOOL _bEntertainment;
	BOOL _bGamePlayer;
};
