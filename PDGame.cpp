#include "PDGame.h"
#include "Utilities.h"
#include "DXText.h"
#include "Globals.h"
#include "DXControls.h"
#include "resource.h"
#include "DXImageButton.h"
#include "DXCheckBox.h"
#include "Configuration.h"
#include "AnimationController.h"
#include "ModuleBase.h"
#include "VideoModule.h"
#include "GameController.h"
#include "PDMainMenuModule.h"
#include "Items.h"
#include "LocationModule.h"
#include "PDMap.h"
#include "PDDMap.h"
#include "ResumeGameModule.h"
#include "PDClimbLadderOverlay.h"
#include "PDConvertPointsOverlay.h"
#include "PDElevationModOverlay.h"
#include "PDSelectLevelModule.h"

BOOL CPDGame::Init()
{
	if (CModuleController::Init(new CPDMap(), new CPDDMap()) && LoadIcons() && CItems::Init())
	{
		CAnimationController::SetCaptionColours(0xff000000, 0xff000000, 0xff00c300, 0xff000000, 0xff000000, 0xff000000, 0xff0096ff, 0xff000000);
		CDXListBox::SetGreyColours(0, 0, 0xffc3c3c3, 0);
		CDXListBox::SetBlackColours(0, 0, 0xff000000, 0);
		CDXButton::SetButtonColours(0, 0, -1, 0);
		CResumeGameModule::SetTextColours(0, 0, 0xffff0000, 0);
		CResumeGameModule::SetHeaderColours(0, 0, -1, 0);
		CDXDialogueOption::SetColours(0, 0, 0xff000000, 0);
		CElevation::ElevationModifier = 0.0f;
		CElevation::ElevationCheckModifier = 0.2f;

		// Make sure Tex player exists
		if (!CFile::Exists(L"PLAYERS\\TEX___00.PLR"))
		{
			CFile file;
			if (file.Open(L"PLAYERS\\TEX___00.PLR", CFile::Mode::Write))
			{
				BYTE buffer[256];
				ZeroMemory(buffer, 256);
				buffer[2] = 'T';
				buffer[3] = 'E';
				buffer[4] = 'X';

				file.Write(buffer, 256);
				file.Close();
			}
		}

		// Create overlay controls
		pClimbLadderOverlay = new CPDClimbLadderOverlay();
		pConvertPointsOverlay = new CPDConvertPointsOverlay();
		pElevationModOverlay = new CPDElevationModOverlay();

		CModuleController::Push(new CPDMainMenuModule());

		return TRUE;
	}

	return FALSE;
}

CPDGame::CPDGame()
{
	_gameData = new BYTE[PD_SAVE_SIZE];
	ZeroMemory(_gameData, PD_SAVE_SIZE);

	_lastDialoguePoint = -1;
	_frameTrigger = -1;

	ReadGameXMLInfo(IDR_XML_PD);

	SetGamePath(L".\\");
}

CPDGame::~CPDGame()
{
}

void CPDGame::Render()
{
}

void CPDGame::MouseMove(POINT pt)
{
}

void CPDGame::MouseDown(POINT pt, int btn)
{
}

void CPDGame::KeyDown(WPARAM key, LPARAM lParam)
{
}

void CPDGame::KeyUp(WPARAM key, LPARAM lParam)
{
}

void CPDGame::LoadGame(LPWSTR fileName)
{
	BYTE data[PD_SAVE_SIZE];
	CFile file;
	if (file.Open(fileName))
	{
		int read = file.Read(data, PD_SAVE_SIZE);
		file.Close();

		if (read == PD_SAVE_SIZE)
		{
			CopyMemory(_gameData, data, PD_SAVE_SIZE);

			// Validate inventory, check current item
			int currentItem = _gameData[PD_SAVE_CURRENT_ITEM];
			if (currentItem != 0xff)
			{
				int itemCount = _gameData[PD_SAVE_ITEM_COUNT];
				bool found = false;
				for (int i = 0; i < itemCount; i++)
				{
					if (GetInt(_gameData, PD_SAVE_INVENTORY + i * 2, 2) == currentItem)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					SetInt(_gameData, PD_SAVE_CURRENT_ITEM, -1, 2);
				}
			}

			// Update cash item text
			std::wstring cash = CGameController::GetItemName(0) + L" $" + std::to_wstring(GetInt(_gameData, PD_SAVE_CASH, 2));
			CItems::SetItemName(0, cash);

			// Should now load location or dialogue
			if (_gameData[PD_SAVE_PARAMETERS + 252])
			{
				CModuleController::Push(new CPDLocationModule(_gameData[PD_SAVE_LOCATION_ID], _gameData[PD_SAVE_STARTUP_POSITION]));
			}
			else
			{
				CModuleController::Push(new CVideoModule(VideoType::Scripted, _gameData[PD_SAVE_DMAP_ID], GetWord(PD_SAVE_SCRIPT_ID)));
			}
		}
	}
}

void CPDGame::SaveGame(LPWSTR fileName)
{
	// TODO: Move to shared code
	CFile file;
	if (file.Open(fileName, CFile::Mode::Write))
	{
		// Populate situation description
		std::wstring sit;
		//if (_gameData[PD_SAVE_DMAP_FLAG] == 0)
		//{
		//	// Location
		//	sit = CGameController::GetSituationDescriptionL(_gameData[PD_SAVE_MAP_ENTRY]);
		//}
		//else
		//{
		//	// Dialogue
		//	sit = CGameController::GetSituationDescriptionD(_gameData[PD_SAVE_DMAP_ENTRY]);
		//}

		//memset(_gameData + PD_SAVE_LOCATION, ' ', 30);
		//for (int i = 0; i < sit.size() && i < 0x1e; i++)
		//{
		//	_gameData[PD_SAVE_LOCATION + i] = sit[i] & 0xFF;
		//}

		SYSTEMTIME time;
		GetLocalTime(&time);
		//_gameData[PD_SAVE_GAME_DAY] = min(7, max(1, _gameData[PD_SAVE_PARAMETERS + 250]));
		_gameData[PD_SAVE_YEAR] = (BYTE)(time.wYear & 0xff);
		_gameData[PD_SAVE_YEAR + 1] = (BYTE)((time.wYear >> 8) & 0xff);
		_gameData[PD_SAVE_MONTH] = (BYTE)time.wMonth;
		_gameData[PD_SAVE_DAY] = (BYTE)time.wDay;
		_gameData[PD_SAVE_HOUR] = (BYTE)time.wHour;
		_gameData[PD_SAVE_MINUTE] = (BYTE)time.wMinute;
		_gameData[PD_SAVE_SECOND] = (BYTE)time.wSecond;

		file.Write(_gameData, PD_SAVE_SIZE);
		file.Close();
	}
}

void CPDGame::NewGame()
{
	CPDSelectLevelModule* pSelectLevelModule = new CPDSelectLevelModule(_gameData);
	CModuleController::Push(pSelectLevelModule);
}

BYTE CPDGame::GetParameter(int index)
{
	return (index >= 0 && index < 1024) ? _gameData[PD_SAVE_PARAMETERS + index] : 0;
}

void CPDGame::SetParameter(int index, BYTE value)
{
	if (index >= 0 && index < 1024)
	{
		_gameData[PD_SAVE_PARAMETERS + index] = value;
	}
}

BYTE CPDGame::GetData(int offset)
{
	return (offset >= 0 && offset < PD_SAVE_SIZE) ? _gameData[offset] : 0;
}

void CPDGame::SetData(int offset, BYTE value)
{
	if (offset >= 0 && offset < PD_SAVE_SIZE)
	{
		_gameData[offset] = value;
	}
}

void CPDGame::SetData(int offset, char* text)
{
	if (offset >= 0)
	{
		while (offset < PD_SAVE_SIZE && *text)
		{
			_gameData[offset++] = *(text++);
		}
	}
}

BYTE CPDGame::GetAskAboutState(int index)
{
	return (index >= 0 && index < 200) ? _gameData[PD_SAVE_ASK_ABOUT_STATES + index] : 0;
}

void CPDGame::SetAskAboutState(int index, BYTE state)
{
	if (index >= 0 && index <= 125)
	{
		// Add or remove from list
		int count = GetAskAboutCount();
		if (state == 0 || state == 2)
		{
			int i = 0;
			for (i = 0; i < count && i < 50; i++)
			{
				if (GetInt(_gameData, PD_SAVE_ASK_ABOUTS + i * 2, 2) == index)
				{
					for (; i < count && i < 49; i++)
					{
						SetInt(_gameData, PD_SAVE_ASK_ABOUTS + i * 2, GetInt(_gameData, PD_SAVE_ASK_ABOUTS + (i + 1) * 2, 2), 2);
					}
					SetInt(_gameData, PD_SAVE_ASK_ABOUTS + count * 2, -1, 2);

					_gameData[PD_SAVE_ASK_ABOUT_COUNT]--;

					break;
				}
			}
		}
		else if (state == 1)
		{
			// Check if state already set
			for (int i = 0; i < count; i++)
			{
				if (GetInt(_gameData, PD_SAVE_ASK_ABOUTS + i * 2, 2) == index)
				{
					return;
				}
			}

			SetInt(_gameData, PD_SAVE_ASK_ABOUTS + count * 2, index, 2);
			_gameData[PD_SAVE_ASK_ABOUT_COUNT]++;
		}

		_gameData[PD_SAVE_ASK_ABOUT_STATES + index] = state;
	}
}

int CPDGame::GetAskAboutCount()
{
	return _gameData[PD_SAVE_ASK_ABOUT_COUNT];
}

int CPDGame::GetAskAboutId(int index)
{
	return (index >= 0 && index < 50) ? GetInt(_gameData, PD_SAVE_ASK_ABOUTS + index * 2, 2) : -1;
}

int CPDGame::GetScore()
{
	return GetWord(PD_SAVE_SCORE, TRUE);
}

void CPDGame::AddScore(int value)
{
	SetWord(PD_SAVE_SCORE, GetWord(PD_SAVE_SCORE) + value);
}

int CPDGame::GetItemCount()
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	return count;
}

int CPDGame::GetItemId(int index)
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	int id = -1;
	if (index < count)
	{
		id = GetInt(_gameData, PD_SAVE_INVENTORY + index * 2, 2);
	}

	return id;
}

int CPDGame::GetItemState(int item)
{
	if (item >= 0 && item < PD_MAX_ITEM_COUNT)
	{
		return _gameData[PD_SAVE_ITEM_STATES + item];
	}

	return 0;
}

void CPDGame::SetItemState(int item, int state)
{
	if (item >= 0 && item < PD_MAX_ITEM_COUNT)
	{
		_gameData[PD_SAVE_ITEM_STATES + item] = state;

		int count = _gameData[PD_SAVE_ITEM_COUNT];

		// Add or remove from list
		if (state == 0 || state == 2)
		{
			for (int i = 0; i < count && i < PD_MAX_ITEM_COUNT; i++)
			{
				if (GetInt(_gameData, PD_SAVE_INVENTORY + i * 2, 2) == item)
				{
					for (int j = i + 1; j < count && j < PD_MAX_ITEM_COUNT; j++)
					{
						SetInt(_gameData, PD_SAVE_INVENTORY + (j - 1) * 2, GetInt(_gameData, PD_SAVE_INVENTORY + j * 2, 2), 2);
					}

					_gameData[PD_SAVE_ITEM_COUNT]--;

					break;
				}
			}

			if (GetInt(_gameData, PD_SAVE_CURRENT_ITEM, 2) == item)
			{
				SetInt(_gameData, PD_SAVE_CURRENT_ITEM, -1, 2);
			}
		}
		else if (state == 1)
		{
			BOOL alreadyInInventory = FALSE;
			for (int i = 0; i < count; i++)
			{
				if (GetInt(_gameData, PD_SAVE_INVENTORY + i * 2, 2) == item)
				{
					alreadyInInventory = TRUE;
					break;
				}
			}

			if (!alreadyInInventory)
			{
				SetInt(_gameData, PD_SAVE_INVENTORY + count * 2, item, 2);
				_gameData[PD_SAVE_ITEM_COUNT]++;
				SetInt(_gameData, PD_SAVE_CURRENT_ITEM, item, 2);
			}
		}
	}
}

int CPDGame::GetCurrentItemId()
{
	int itemId = GetInt(_gameData, PD_SAVE_CURRENT_ITEM, 2);
	return (itemId == 0xffff) ? -1 : itemId;
}

void CPDGame::SetCurrentItemId(int item)
{
	_gameData[PD_SAVE_CURRENT_ITEM] = item;
}

int CPDGame::SelectNextItem()
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	int currentItem = GetInt(_gameData, PD_SAVE_CURRENT_ITEM, 2);
	int newIndex = IndexOfItemId(currentItem) + 1;
	if (newIndex >= count)
	{
		newIndex = -1;
	}

	SetInt(_gameData, PD_SAVE_CURRENT_ITEM, newIndex < 0 ? -1 : GetInt(_gameData, PD_SAVE_INVENTORY + newIndex * 2, 2), 2);

	return GetInt(_gameData, PD_SAVE_CURRENT_ITEM, 2);
}

int CPDGame::SelectPreviousItem()
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	int currentItem = GetInt(_gameData, PD_SAVE_CURRENT_ITEM, 2);
	int newIndex = IndexOfItemId(currentItem) - 1;
	if (newIndex < -1)
	{
		newIndex = count - 1;
	}

	SetInt(_gameData, PD_SAVE_CURRENT_ITEM, newIndex < 0 ? -1 : GetInt(_gameData, PD_SAVE_INVENTORY + newIndex * 2, 2), 2);

	return GetInt(_gameData, PD_SAVE_CURRENT_ITEM, 2);
}

BYTE CPDGame::GetHintState(int index)
{
	return 0;
}

void CPDGame::SetHintState(int index, BYTE state, int score)
{
}

BYTE CPDGame::GetHintCategoryState(int index)
{
	return (index >= 0 && index < 1000) ? _gameData[PD_SAVE_HINT_CATEGORY_STATES + index] : 0;
}

void CPDGame::SetHintCategoryState(int index, BYTE state)
{
	if (index >= 0 && index < 1000)//TODO: Find hint category count
	{
		_gameData[PD_SAVE_HINT_CATEGORY_STATES + index] = state;
	}
}

void CPDGame::SetTimer(int timer, int duration)
{
	if (timer >= 0 && timer < 32)
	{
		//if (_gameData[UAKM_SAVE_TIMERS + timer] < 0)
		//{
		//	_gameData[UAKM_SAVE_TIMERS + timer] = 0;
		//}

		_gameData[PD_SAVE_TIMERS + timer] = 1;

		SetWord(PD_SAVE_TIMERS_INITIAL + timer * 2, duration);
		SetWord(PD_SAVE_TIMERS_CURRENT + timer * 2, duration);
		Timers[timer] = static_cast<int>(duration * TIMER_SCALE);
	}
}

int CPDGame::GetTimerState(int timer)
{
	int state = (timer >= 0 && timer < 32) ? _gameData[PD_SAVE_TIMERS + timer] : 0;

	//Trace(L"State of timer ");
	//Trace(timer);
	//Trace(L" is ");
	//Trace(state);
	//Trace(L", time left ");
	//Trace(Timers[timer]);
	//TraceLine(L" ms");

	return state;
}

void CPDGame::ResetTimers()
{
	// TODO: Disable timers
	ZeroMemory(Timers, sizeof(Timers));
	ZeroMemory(_gameData + PD_SAVE_TIMERS_INITIAL, 32 * 5);
}

void CPDGame::Tick(int ticks)
{
	// Check timers
	for (int i = 0; i < 32; i++)
	{
		if (_gameData[PD_SAVE_TIMERS + i] > 0)
		{
			// Timer is active, reduce by ticks
			Timers[i] = max(0, Timers[i] - ticks);
			if (Timers[i] == 0)
			{
				_gameData[PD_SAVE_TIMERS + i] = 0;
				SetWord(PD_SAVE_TIMERS_CURRENT + i * 2, GetWord(PD_SAVE_TIMERS_INITIAL + i * 2));
			}
		}
	}
}

void CPDGame::SetItemExamined(int itemId, int conditionalScore)
{
	if (itemId >= 0 && itemId < PD_MAX_ITEM_COUNT)
	{
		int byte = itemId / 8;
		int shift = itemId & 7;

		int oldState = (_gameData[PD_SAVE_ITEMS_EXAMINED_FLAGS + byte] & (1 << shift));
		_gameData[PD_SAVE_ITEMS_EXAMINED_FLAGS + byte] |= (1 << shift);

		if (oldState == 0)
		{
			if (conditionalScore > 0)
			{
				AddScore(conditionalScore);
			}

			// TODO: Check if e.g. extra cash should be added
			if (itemId == 274)
			{
				// Disc player with CD
				//ds:word_2A8774= 600
				// Offset 1B8 in save data
				//ds:byte_2A87A5= 1
				// Offset 1E9 in save data
				// Timer enabled and time?
			}
			else if (itemId == 285)
			{
				// Nilo's wallet
				AddCash(100);
			}
			else if (itemId == 288)
			{
				// Orphanage letter
				AddCash(500);
			}
			else if (itemId == 43)
			{
				// Prize letter
				AddCash(100);
			}
			else if (itemId == 225)
			{
				// Money belt
				AddCash(300);
			}
		}
	}
}

int CPDGame::GetWord(int offset, BOOL signExtend)
{
	int result = (offset >= 0 && offset < (PD_SAVE_SIZE - 1)) ? (_gameData[offset + 1] << 8) | _gameData[offset] : 0;
	if (signExtend && result & 0x8000)
	{
		result |= ~0xffff;
	}

	return result;
}

void CPDGame::SetWord(int offset, int value)
{
	if (offset >= 0 && offset < (PD_SAVE_SIZE - 1))
	{
		_gameData[offset] = value;
		_gameData[offset + 1] = value >> 8;

		if (offset == PD_SAVE_CASH)
		{
			// Update item text
			std::wstring cash = CGameController::GetItemName(0) + L" $" + std::to_wstring(GetInt(_gameData, PD_SAVE_CASH, 2));
			CItems::SetItemName(0, cash);
		}
	}
}

int CPDGame::IndexOfItemId(int item)
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	for (int i = 0; i < count; i++)
	{
		if (GetInt(_gameData, PD_SAVE_INVENTORY + i * 2, 2) == item)
		{
			return i;
		}
	}

	return -1;
}

BOOL CPDGame::LoadIcons()
{
	BOOL result = FALSE;
	BinaryData bd = LoadEntry(L"ICONS.AP", 0);
	if (bd.Data != NULL && bd.Length > 0)
	{
		result = CGameBase::LoadIcons(bd);
		delete[] bd.Data;
	}

	return result;
}

int CPDGame::GetBuyableItemCount()
{
	return GetInt(_gameData, PD_SAVE_BUYABLES_COUNT, 2);
}

int CPDGame::GetBuyableItemId(int index)
{
	if (index >= 0 && index < 25)
	{
		return GetInt(_gameData, PD_SAVE_BUYABLES + index * 2, 2);
	}

	return -1;
}

void CPDGame::SetBuyableItemState(int index, int state)
{
	if (index >= 0 && index < 25)
	{
		// Add or remove from list
		int count = GetBuyableItemCount();
		if (state == 0 || state == 2)
		{
			int i = 0;
			for (i = 0; i < count && i < 25; i++)
			{
				if (_gameData[PD_SAVE_BUYABLES + i * 2] == index)
				{
					for (; i < count && i < 25; i++)
					{
						_gameData[PD_SAVE_BUYABLES + i * 2] = _gameData[PD_SAVE_BUYABLES + (i + 1) * 2];
					}
					SetWord(PD_SAVE_BUYABLES + count * 2, -1);

					_gameData[PD_SAVE_BUYABLES_COUNT]--;

					break;
				}
			}
		}
		else if (state == 1)
		{
			// Check if state already set
			for (int i = 0; i < count; i++)
			{
				if (GetInt(_gameData, PD_SAVE_BUYABLES + i * 2, 2) == index)
				{
					return;
				}
			}

			SetWord(PD_SAVE_BUYABLES + count * 2, index);
			_gameData[PD_SAVE_BUYABLES_COUNT]++;
		}
		else
		{
			int debug = 0;
		}

		_gameData[PD_SAVE_BUYABLES_ASK_ABOUT_STATES + index] = state;
	}
}

void CPDGame::AddCash(int cashToAdd)
{
	int currentCash = GetWord(PD_SAVE_CASH);
	SetWord(PD_SAVE_CASH, currentCash + cashToAdd);
}
