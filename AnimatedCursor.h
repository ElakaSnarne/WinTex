#pragma once

#include <list>
#include "DXBitmap.h"

class CAnimatedCursor
{
public:
	CAnimatedCursor();
	~CAnimatedCursor();

	void Dispose();

	enum class CursorType
	{
		Arrow = 0,
		Note = 1,
		Crosshair = 2,
		Diskette = 3,
		Look = 4,
		Move = 5,
		Grab = 6,
		OnOff = 7,
		Talk = 8,
		Hint = 9,
		Open = 10,
		Loading = 11,
		Special = 12
	};

	void SetIcons(CursorType type, std::list<CDXBitmap*> icons);

	void SetPosition(float x, float y)
	{
		_x = x - _hotspotX;
		_y = y - _hotspotY;
	}
	void Render();

protected:
	CDXBitmap** _ppIcons;
	int _iconCount;
	float _x;
	float _y;
	float _hotspotX;
	float _hotspotY;

	ULONGLONG _lastChange;
	int _currentIcon;
	int _direction;
	DWORD _interval;
	DWORD _forwardInterval;
	DWORD _reverseInterval;
	DWORD _loopDelay;
	DWORD _loopDelayCounter;

	CursorType _type;
};
