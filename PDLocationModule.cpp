#include "PDLocationModule.h"
#include "PDInventoryModule.h"
#include "PDTravelModule.h"

CPDLocationModule::CPDLocationModule(int locationId, int startupPosition) : CLocationModule(locationId, startupPosition)
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

	_abductorMode = FALSE;
}

void CPDLocationModule::Inventory()
{
	CModuleController::Push(new CPDInventoryModule());
}

void CPDLocationModule::Render()
{
	// TODO: Check if alien abductor is active (set clipping area and overlay image+animation)
	// TODO: Check if popup is active (asking to use ladders or convert points to money)
	_presentOnRender = FALSE;

	CLocationModule::Render();

	if (!_presentOnRender)
	{
		dx.Present(1, 0);
	}
}

void CPDLocationModule::Travel()
{
	CModuleController::Push(new CPDTravelModule());
}
