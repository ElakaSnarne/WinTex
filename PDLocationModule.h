#pragma once

#include "LocationModule.h"

class CPDLocationModule : public CLocationModule
{
public:
	CPDLocationModule(int locationId, int startupPosition) : CLocationModule(locationId, startupPosition)
	{
		_actionColour1 = 0xff000000;
		_actionColour2 = 0xff000000;
		_actionColour3 = -1;
		_actionColour4 = 0xff000000;
		_currentActionColour1 = 0xff000000;
		_currentActionColour2 = 0xff000000;
		_currentActionColour3 = 0xffffff00;
		_currentActionColour4 = 0xff000000;
		_unavailableActionColour1 = 0xff000000;
		_unavailableActionColour2 = 0xff000000;
		_unavailableActionColour3 = 0xff808080;
		_unavailableActionColour4 = 0xff000000;
	}
};
