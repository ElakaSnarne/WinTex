#pragma once

#include "FullScreenModule.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"

class CUAKMColonelsComputerModule : public CFullScreenModule
{
public:
	CUAKMColonelsComputerModule();
	virtual ~CUAKMColonelsComputerModule();

	virtual void Render();

protected:
	virtual void Initialize();

	void Render(int entry, int offset_x, int offset_y, int x1 = -1, int x2 = -1, int y1 = -1, int y2 = -1);
	void RenderRaw(int entry, int offset_x, int offset_y);

	// Input related
	virtual void BeginAction();
};
