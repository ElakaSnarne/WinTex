#include "PDElevationModOverlay.h"
#include "Utilities.h"

CPDElevationModOverlay::CPDElevationModOverlay()
{
	_targetY = 0.0f;
	_speed = 0.0f;
	_lastUpdate = 0;
}

void CPDElevationModOverlay::Render()
{
	ULONGLONG now = GetTickCount64();
	float diff = (float)(now - _lastUpdate);
	if (diff > 0.0f)
	{
		CLocation::_y_player_adjustment -= _speed / diff;
		if ((_speed > 0.0f && CLocation::_y_player_adjustment <= _targetY) || (_speed < 0.0f && CLocation::_y_player_adjustment >= _targetY))
		{
			BeginAction();
		}
		_lastUpdate = now;
	}
}

void CPDElevationModOverlay::BeginAction()
{
	CLocation::_y_player_adjustment = _targetY;
	pOverlay = NULL;
}

void CPDElevationModOverlay::SetData(int p1, int p2)
{
	_targetY = -From12_4(p1);
	_speed = From12_4(p2) * 3;
	_lastUpdate = GetTickCount64();
}
