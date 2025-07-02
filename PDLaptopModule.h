#pragma once

#include "FullScreenModule.h"
#include "RawFont.h"

class CPDLaptopModule : public CFullScreenModule
{
public:
	CPDLaptopModule(BOOL cdUsed);
	CPDLaptopModule(LPBYTE screen, LPINT palette);
	~CPDLaptopModule();

	virtual void Render();

protected:
	virtual void Initialize();

	CRawFont _pdRawFont;

	void RenderScreen();

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();

	BOOL _cdUsed;

	int _mode;
	int _stage;
	int _dotX;
	ULONGLONG _time;

	static int CellSequence[];

	void RenderButton(int x1, int y1, int x2, int y2, char* text, int boxColour, int textColour);

	void RenderArticleButtons();
};
