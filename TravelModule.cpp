#include "TravelModule.h"
#include "ModuleBase.h"
#include "GameController.h"
#include "ModuleController.h"
#include "AmbientAudio.h"

CTravelModule::CTravelModule() : CFullScreenModule(ModuleType::Travel)
{
	_travelDataOffset = 0;

	_selectedLocation = -1;
	_selectedSubLocation = -1;
	_selectedSubLocationEntry = 0;

	_selectionIndicator = NULL;

	_coordinates = NULL;
	_hotspots = NULL;
	_resultTable = NULL;

	_cursorMinX = 0.0;
	_cursorMaxX = dx.GetWidth();
	_cursorMinY = 0.0;
	_cursorMaxY = dx.GetHeight();
}

CTravelModule::~CTravelModule()
{
}

void CTravelModule::Resize(int width, int height)
{
}

void CTravelModule::Dispose()
{
	for (auto it : _images)
	{
		delete it.second;
	}

	_images.clear();

	if (_selectionIndicator != NULL)
	{
		_selectionIndicator->Release();
		_selectionIndicator = NULL;
	}
}

void CTravelModule::BeginAction()
{
	if (_selectedLocation > 0)
	{
		// Check if a sub-location was selected
		int subix = 17;
		int subEntry = 0;
		for (auto it : _subLocations)
		{
			if (it->ParentLocation == _selectedLocation && CGameController::GetData(_travelDataOffset + subix) != 0)
			{
				if (_cursorPosX >= it->Left && _cursorPosX < it->Right && _cursorPosY >= it->Top && _cursorPosY < it->Bottom)
				{
					_selectedSubLocation = subix;
					_selectedSubLocationEntry = subEntry;
					break;
				}
				subEntry++;
			}

			subix++;
		}
	}

	// Check if a main location was selected
	int i = 0;
	while (_coordinates[(++i) * 2] != -1)
	{
		if (CGameController::GetData(_travelDataOffset + i) != 0)
		{
			CTravelImage* name = _images[100 * i];
			if (_cursorPosX >= name->Left && _cursorPosX < name->Right && _cursorPosY >= -name->Top && _cursorPosY < -name->Bottom)
			{
				_selectedLocation = i;
				_selectedSubLocation = -1;
				_selectedSubLocationEntry = 0;
				break;
			}
		}
	}

	// Check if Go To or Cancel were clicked
	if (_selectedLocation > 0 && _images[2]->HitTest((float)_cursorPosX, (float)_cursorPosY))
	{
		CAmbientAudio::Clear();
		CGameController::CanCancelTravel = TRUE;

		pMIDI->Stop();

		int ix = (_selectedSubLocation > 0) ? _selectedSubLocation : _selectedLocation;
		if (_resultTable[ix * 2] != -1)
		{
			// Load location module
			int locationId = _resultTable[ix * 2];
			//CGameController::SetData(UAKM_SAVE_MAP_ENTRY, locationId);
			//CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 0);
			CGameController::SetParameter(249, 0);
			CGameController::AutoSave();
			CModuleController::Push(new CLocationModule(locationId, 0));
		}
		else
		{
			// Load video module
			int id = _resultTable[ix * 2 + 1];
			//CGameController::SetData(UAKM_SAVE_DMAP_ENTRY, id);
			//CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 1);
			CGameController::AutoSave();
			CModuleController::Push(new CVideoModule(VideoType::Scripted, id));
		}

		//CModuleController::Pop(this);
	}
	else if (CGameController::CanCancelTravel && _images[4]->HitTest(_cursorPosX, _cursorPosY))
	{
		CModuleController::Pop(this);
	}
}

void CTravelModule::Back()
{
	if (CGameController::CanCancelTravel)
	{
		CModuleController::Pop(this);
	}
}
