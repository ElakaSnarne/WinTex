#pragma once

#include "LocationModule.h"

class CPDLocationModule : public CLocationModule
{
public:
	CPDLocationModule(int locationId, int startupPosition);

	virtual void Render();

protected:
	virtual void Inventory();
	virtual void Travel();

	BOOL _abductorMode;
};
