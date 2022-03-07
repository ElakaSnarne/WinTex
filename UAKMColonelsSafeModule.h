#pragma once

#include "FullScreenModule.h"
#include <unordered_map>
#include <d3d11.h>
#include "Texture.h"
#include "AmbientAudio.h"

class CUAKMColonelsSafeModule : public CFullScreenModule
{
public:
	CUAKMColonelsSafeModule(int parameter);
	virtual ~CUAKMColonelsSafeModule();

	virtual void Render();

protected:
	virtual void Initialize();

	int _parameter;

	CAmbientAudio _sound;

	void Render(int entry, int offset_x, int offset_y);

	BYTE _enteredCode[4];
	void ResetCode();
	BOOL _dialChanged;

	void TurnDial(int dial);
	int _dial;

	// Input related
	virtual void BeginAction();
};
