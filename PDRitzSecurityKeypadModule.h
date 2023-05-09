#pragma once

#include "FullScreenModule.h"

class CPDRitzSecurityKeypadModule : public CFullScreenModule
{
public:
	CPDRitzSecurityKeypadModule();
	~CPDRitzSecurityKeypadModule();

	virtual void Render();

protected:
	virtual void Initialize();
};
