#pragma once

#include "FullScreenModule.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"

class CUAKMGRSComputerModule : public CFullScreenModule
{
public:
	CUAKMGRSComputerModule();
	virtual ~CUAKMGRSComputerModule();

	virtual void Dispose();
	virtual void Render();

	virtual void KeyDown(WPARAM key, LPARAM lParam);

protected:
	virtual void Initialize();

	LPBYTE _animation;
	int _animationLength;

	void RenderButton(int x, int y, int image);

	LPBYTE _animationPointer;
	int _animationFrames;
	int _animationWidth;
	int _animationHeight;
	BOOL _animationActive;

	int _previousPage;

	// Input related
	virtual void BeginAction();
	virtual void Back();
};
