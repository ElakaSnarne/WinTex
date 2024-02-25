#pragma once

#include "Overlay.h"

class CPDElevationModOverlay : public COverlay
{
public:
	CPDElevationModOverlay();

	virtual void Render();
	virtual void BeginAction();
	virtual void SetData(int p1, int p2);

protected:
	float _targetY;
	float _speed;
	ULONGLONG _lastUpdate;
};
