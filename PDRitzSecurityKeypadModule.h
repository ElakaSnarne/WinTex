#pragma once

#include "FullScreenModule.h"
#include "AmbientAudio.h"

class CPDRitzSecurityKeypadModule : public CFullScreenModule
{
public:
	CPDRitzSecurityKeypadModule();
	~CPDRitzSecurityKeypadModule();

	virtual void Dispose();
	virtual void Render();
	virtual void KeyDown(WPARAM key, LPARAM lParam);

protected:
	virtual void Initialize();

	void ResetCode();
	void Key(int key);
	signed char _enteredCode[5];
	int _keyPos;
	ULONGLONG _keyTimes[11];
	BOOL _updateTexture;

	// Input related
	virtual void BeginAction();

	BOOL _codeCorrect;
	int _blinkFrame;
	ULONGLONG _blinkFrameTime;

	CAmbientAudio _sound;
	ULONGLONG _soundStartTime;
};
