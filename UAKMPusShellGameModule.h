#pragma once

#include "ModuleBase.h"

class CUAKMPusShellGameModule : public CModuleBase
{
public:
	CUAKMPusShellGameModule(int parameter, int correctAnswer);
	virtual ~CUAKMPusShellGameModule();

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();

	virtual void Pause();
	virtual void Resume();

protected:
	virtual void Initialize();

	int _parameter;
	int _correctAnswer;
	float _scale;
	float _top;
	float _left;

	// Input related
	virtual void BeginAction();
	virtual void Back();
};
