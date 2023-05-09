#pragma once

#include "FullScreenModule.h"

class CPDCrosswordModule : public CFullScreenModule
{
public:
	CPDCrosswordModule();
	~CPDCrosswordModule();

	virtual void Render();

protected:
	virtual void Initialize();
};
