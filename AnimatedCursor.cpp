#include "AnimatedCursor.h"
#include "Globals.h"

CAnimatedCursor::CAnimatedCursor()
{
	_x = 0.0f;
	_y = 0.0f;
	_hotspotX = 0.0f;
	_hotspotY = 0.0f;
	_lastChange = 0;
	_ppIcons = NULL;
	_iconCount = 0;
	_currentIcon = 0;
	_direction = 0;
	_interval = 50;
	_forwardInterval = 50;
	_reverseInterval = 50;
	_loopDelay = 0;
	_loopDelayCounter = 0;
}

CAnimatedCursor::~CAnimatedCursor()
{
	Dispose();
}

void CAnimatedCursor::Dispose()
{
	if (_ppIcons != NULL)
	{
		for (int i = 0; i < _iconCount; i++)
		{
			delete _ppIcons[i];
		}

		delete[] _ppIcons;

		_ppIcons = NULL;
	}
}

void CAnimatedCursor::Render()
{
	// TODO: Get current image, set position and render
	if (_ppIcons != NULL)
	{
		CDXBitmap* currentCursor = _ppIcons[_currentIcon];
		if (currentCursor != NULL)
		{
			currentCursor->SetPosition(_x, _y);	// TODO: Adjust for hotspot
			currentCursor->Render();
		}

		if (_iconCount > 1)
		{
			// TODO: Fix animation, not all bounce front to back, some play faster backwards etc...

			ULONGLONG now = GetTickCount64();
			if ((now - _lastChange) > _interval)
			{
				_lastChange = now;
				_currentIcon += _direction;
				if (_currentIcon >= _iconCount)
				{
					if (_type == CursorType::Grab || _type == CursorType::Loading)
					{
						if (_loopDelayCounter < _loopDelay)
						{
							_loopDelayCounter++;
							_currentIcon -= _direction;
						}
						else
						{
							_currentIcon = 0;
							_loopDelayCounter = 0;
						}
					}
					else
					{
						_direction = -1;
						_currentIcon += _direction;
						_interval = _reverseInterval;
					}
				}
				else if (_currentIcon < 0)
				{
					if (_loopDelayCounter < _loopDelay)
					{
						_loopDelayCounter++;
						_currentIcon -= _direction;
					}
					else
					{
						_direction = 1;
						_currentIcon += _direction;
						_interval = _forwardInterval;
						_loopDelayCounter = 0;
					}
				}
			}
		}
	}
}

void CAnimatedCursor::SetIcons(CursorType type, std::list<CDXBitmap*> icons)
{
	_type = type;
	_iconCount = static_cast<int>(icons.size());
	_ppIcons = new CDXBitmap * [_iconCount];
	for (int i = 0; i < _iconCount; i++)
	{
		_ppIcons[i] = icons.front();
		icons.pop_front();
	}

	_direction = 1;

	float iconHeight = _ppIcons[0]->GetHeight();

	if (type == CursorType::Arrow)
	{
		_hotspotX = 0;
		_hotspotY = 0;
	}
	else if (type == CursorType::Crosshair)
	{
		if (isUAKM)
		{
			_hotspotX = 7;
			_hotspotY = 7;
		}
		else
		{
			_hotspotX = 2;
			_hotspotY = 3;
		}
	}
	else if (type == CursorType::Look)
	{
		_loopDelay = 10;
		_hotspotX = 14;

		if (isUAKM)
		{
			_hotspotY = 4;
		}
		else
		{
			_hotspotY = 12;
		}
	}
	else if (type == CursorType::Move)
	{
		_hotspotX = 11;
		_hotspotY = 29;
		_loopDelay = 10;
	}
	else if (type == CursorType::Grab)
	{
		_hotspotX = 13;
		_hotspotY = 25;
		_loopDelay = 10;
	}
	else if (type == CursorType::OnOff)
	{
		_hotspotX = 10;
		_hotspotY = 4;
		_loopDelay = 10;
	}
	else if (type == CursorType::Talk)
	{
		_hotspotX = 10;
		_hotspotY = 4;
		_loopDelay = 10;
	}
	else if (type == CursorType::Open)
	{
		_reverseInterval = 10;
		_loopDelay = 10;

		if (isUAKM)
		{
			_hotspotX = 12;
			_hotspotY = 4;
		}
		else
		{
			_hotspotX = 14;
			_hotspotY = 7;
		}
	}
}
