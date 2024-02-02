#pragma once

#include "LocationModule.h"

class CPDLocationModule : public CLocationModule
{
public:
	CPDLocationModule(int locationId, int startupPosition);

	virtual void Inventory();

	virtual void Render();

private:
	BOOL _abductorMode;
};
