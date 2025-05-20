#pragma once

#include "InventoryModule.h"
#include "PDExamStruct.h"

#define PD_INVENTORY_CROSSWORD_PUZZLE			269
#define PD_INVENTORY_FINISHED_CROSSWORD_PUZZLE	282

class CPDInventoryModule : public CInventoryModule
{
public:
	CPDInventoryModule();

protected:
	virtual void Examine();
	virtual void Resume();

	bool _closeOnResume;
};
