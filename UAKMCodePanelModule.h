#pragma once

#include "FullScreenModule.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"
#include "AmbientAudio.h"

class CUAKMCodePanelModule : public CFullScreenModule
{
public:
	CUAKMCodePanelModule(int parameter);
	virtual ~CUAKMCodePanelModule();

	virtual void Render();
	virtual void KeyDown(WPARAM key, LPARAM lParam);

protected:
	virtual void Initialize();

	int _parameter;

	void RenderDot(int pos);

	void ResetCode();
	void Key(int key);
	BYTE _enteredCode[8];
	int _keyPos;

	CAmbientAudio _sound;

	ULONGLONG _passwordMessageTime;
	int _lastMessageOffset;

	int _wrongFrame;
	ULONGLONG _wrongFrameTime;

	int _correctFrame;
	ULONGLONG _correctFrameTime;

	void Render(int entry, int offset_x, int offset_y);

	// Input related
	virtual void BeginAction();
};
