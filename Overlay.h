#pragma once

#include <Windows.h>
#include "DXControl.h"
#include <list>

class COverlay
{
public:
	COverlay();

	virtual void KeyDown(WPARAM key, LPARAM lParam) { }
	virtual void Render() = NULL;
	virtual void BeginAction() = NULL;
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
