#pragma once

#include "ModuleBase.h"

class CPDTravelModule : public CModuleBase
{
public:
	CPDTravelModule();
	~CPDTravelModule();

	virtual void Render();

protected:
	virtual void Initialize();
};

