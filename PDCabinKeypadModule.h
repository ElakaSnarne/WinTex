#pragma once

#include "FullScreenModule.h"
#include "AmbientAudio.h"
#include "RawFont.h"
#include <Windows.h>

class CPDCabinKeypadModule : public CFullScreenModule
{
public:
	CPDCabinKeypadModule();
	~CPDCabinKeypadModule();

	virtual void Dispose();
	virtual void Render();

protected:
	virtual void Initialize();

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();

	CRawFont _rawFont;

	void RenderScreen();
	void RenderMessage(char* message, int colour);

	int _enteredCode;
	CAmbientAudio _sound;

	int _keyStates[16];
	int _keyStateDirections[16];
	ULONGLONG _keyStateUpdateTime[16];

	static int CabinKeyPositions[];

	int _mode;
	int _flashCount;
	ULONGLONG _flashtime;
	int _redBackup;
};
