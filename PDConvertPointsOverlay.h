#pragma once

#include "Overlay.h"

class CPDConvertPointsOverlay : public COverlay
{
public:
	CPDConvertPointsOverlay();
	~CPDConvertPointsOverlay();

	virtual void Render();
	virtual void BeginAction();
};
