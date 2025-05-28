#pragma once

#include "FullScreenModule.h"
#include "RawFont.h"
#include "BinaryData.h"
#include "Utilities.h"
#include "AmbientAudio.h"

class CPDCrosswordModule : public CFullScreenModule
{
public:
	CPDCrosswordModule();
	~CPDCrosswordModule();

	virtual void Dispose();
	virtual void Render();
	virtual void KeyDown(WPARAM key, LPARAM lParam);

protected:
	virtual void Initialize();
	CRawFont _font;
	CAmbientAudio _sound;
	BOOL _updateTexture;
	static char* Solution;
	static char* CheatSequence;

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();

	BOOL CheckCompleted();

	int _currentCellX;
	int _currentCellY;
	int _advanceX;
	int _advanceY;

	BOOL IsBlocked(int x, int y);
};
