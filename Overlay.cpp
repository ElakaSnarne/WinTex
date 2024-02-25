#include "Overlay.h"

COverlay::COverlay()
{
	_x = 0.0;
	_y = 0.0;

	_decision = 0;
}

void COverlay::Cursor(float x, float y, BOOL relative)
{
	_x = relative ? _x + x : x;
	_y = relative ? _y + y : y;

	for (auto it : _hitTestControls)
	{
		it->SetMouseOver(it->HitTest(_x, _y) != NULL);
	}
}
