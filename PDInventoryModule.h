#pragma once

#include "InventoryModule.h"
#include "PDExamStruct.h"

class CPDInventoryModule : public CInventoryModule
{
public:
	CPDInventoryModule();

protected:
	virtual void Examine();
	virtual void Resume();
};
