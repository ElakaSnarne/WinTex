#pragma once

#include "FullScreenModule.h"
#include "RawFont.h"

class CPDLaptopModule : public CFullScreenModule
{
public:
	CPDLaptopModule(BOOL cdUsed);
	~CPDLaptopModule();

	virtual void Resize(int width, int height);
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
	int _dots;
	int _dotX;
	ULONGLONG _dotTime;
};
