#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include "DXControl.h"
#include <list>

class COverlay
{
public:
	COverlay();

	virtual void KeyDown(WPARAM key, LPARAM lParam) { }
	virtual void Render() = 0;
	virtual void BeginAction() = 0;
	virtual void SetData(int p1, int p2) {}

	virtual void Cursor(float x, float y, BOOL relative);

	int GetDecision() { return _decision; }
	void ClearDecision() { _decision = 0; }

protected:
	float _x;
	float _y;

	int _decision;

	std::list<CDXControl*> _hitTestControls;
};
