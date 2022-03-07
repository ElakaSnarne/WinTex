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

BOOL CPDGame::Init()
{
	if (CModuleController::Init(new CPDMap(), new CPDDMap()) && LoadIcons() && CItems::Init())
	{
		CAnimationController::SetCaptionColours(0xff000000, 0xff000000, 0xff00c300, 0xff000000, 0xff000000, 0xff000000, 0xff0096ff, 0xff000000);

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

	BuildFileList(IDR_PD_XML);

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
}

void CPDGame::SaveGame(LPWSTR fileName)
{
}

void CPDGame::NewGame()
{
	// Reset game buffer
	ZeroMemory(_gameData, PD_SAVE_SIZE);
	FillMemory(_gameData + 2, PD_SAVE_PADDING1 - 2, ' ');

	_gameData[PD_SAVE_UNKNOWN1 + 1] = 1;
	SetData(PD_SAVE_PLAYER, "TEX");

	//SetData(UAKM_SAVE_DESCRIPTION, "Tex's Office");

	//SYSTEMTIME sysTime;
	//GetSystemTime(&sysTime);
	_gameData[PD_SAVE_GAME_DAY] = 1;
	//_data[UAKM_SAVE_YEAR] = sysTime.wYear;
	//_data[UAKM_SAVE_MONTH] = sysTime.wMonth;
	//_data[UAKM_SAVE_DAY] = sysTime.wDay;
	//_data[UAKM_SAVE_HOUR] = sysTime.wHour;
	//_data[UAKM_SAVE_MINUTE] = sysTime.wMinute;
	//_data[UAKM_SAVE_SECOND] = sysTime.wSecond;

	//UAKM_SAVE_MAP_ENTRY				0x14a
	//UAKM_SAVE_DMAP_ENTRY				0x150
	//SetData(UAKM_SAVE_CODED_MESSAGE, "YE UANE CIAFWBHED RIPB AEEIWALHEAL  YWLU CUAXLWLR AL  LUE XPWLE WA LUE  GIODEA GALE UILEO AL LUE PXPAO LWHE.LUE EAXXYIBD LIDARWX XWOWCIA.       ");

	//UAKM_SAVE_SCRIPT_ID				0x4c9

	//SetAskAboutState(0, 1);		// Rook Garner
	//SetAskAboutState(1, 1);		// Chelsee Bando
	//SetAskAboutState(2, 1);		// Louie Lamintz
	//SetAskAboutState(3, 1);		// Francesca Lucido
	//SetAskAboutState(4, 1);		// Sal Lucido
	//SetAskAboutState(5, 1);		// Ardo Newpop
	//SetAskAboutState(17, 1);	// Colonel

	//SetData(UAKM_SAVE_TRAVEL + 5, 1);	// Allow travel to Tex's Office

	//_gameData[UAKM_SAVE_CURRENT_ITEM] = -1;
	//_gameData[UAKM_SAVE_CHAPTER] = 1;

	//BinaryData bd = LoadEntry(L"GRAPHICS.AP", 23);
	//if (bd.Data != NULL && bd.Length == 0x1fe)
	//{
	//	CopyMemory(_gameData + UAKM_SAVE_PUZZLE_DATA, bd.Data, bd.Length);
	//	delete[] bd.Data;
	//}

	//LoadFromDMap(0);
	LoadFromMap(1, 0);
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
	return 0;
}

void CPDGame::SetAskAboutState(int index, BYTE state)
{
}

int CPDGame::GetAskAboutCount()
{
	return 0;
}

int CPDGame::GetAskAboutId(int index)
{
	return 0;
}

int CPDGame::GetScore()
{
	return GetWord(PD_SAVE_SCORE);
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
		id = _gameData[PD_SAVE_INVENTORY + index];
	}

	return id;
}

int CPDGame::GetItemState(int item)
{
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
				if (_gameData[PD_SAVE_INVENTORY + i] == item)
				{
					for (int j = i + 1; j < count && j < PD_MAX_ITEM_COUNT; j++)
					{
						_gameData[PD_SAVE_INVENTORY + j - 1] = _gameData[PD_SAVE_INVENTORY + j];
					}

					_gameData[PD_SAVE_ITEM_COUNT]--;

					break;
				}
			}

			if (_gameData[PD_SAVE_CURRENT_ITEM] == item)
			{
				_gameData[PD_SAVE_CURRENT_ITEM] = -1;
			}
		}
		else if (state == 1)
		{
			// TODO: Do not add if item already in inventory!
			BOOL alreadyInInventory = FALSE;
			for (int i = 0; i < count; i++)
			{
				if (_gameData[PD_SAVE_INVENTORY + i] == item)
				{
					alreadyInInventory = TRUE;
					break;
				}
			}

			if (!alreadyInInventory)
			{
				_gameData[PD_SAVE_INVENTORY + count] = item;
				_gameData[PD_SAVE_ITEM_COUNT]++;
				_gameData[PD_SAVE_CURRENT_ITEM] = item;
			}
		}
	}
}

int CPDGame::GetCurrentItemId()
{
	return (_gameData[PD_SAVE_CURRENT_ITEM] == 0xff) ? -1 : _gameData[PD_SAVE_CURRENT_ITEM];
}

void CPDGame::SetCurrentItemId(int item)
{
	_gameData[PD_SAVE_CURRENT_ITEM] = item;
}

int CPDGame::SelectNextItem()
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	int currentItem = _gameData[PD_SAVE_CURRENT_ITEM];
	int newIndex = IndexOfItemId(currentItem) + 1;
	if (newIndex >= count)
	{
		newIndex = -1;
	}

	_gameData[PD_SAVE_CURRENT_ITEM] = newIndex < 0 ? -1 : _gameData[PD_SAVE_INVENTORY + newIndex];

	return _gameData[PD_SAVE_CURRENT_ITEM];
}

int CPDGame::SelectPreviousItem()
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	int currentItem = _gameData[PD_SAVE_CURRENT_ITEM];
	int newIndex = IndexOfItemId(currentItem) - 1;
	if (newIndex < -1)
	{
		newIndex = count - 1;
	}

	_gameData[PD_SAVE_CURRENT_ITEM] = newIndex < 0 ? -1 : _gameData[PD_SAVE_INVENTORY + newIndex];

	return _gameData[PD_SAVE_CURRENT_ITEM];
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
	return 0;
}

void CPDGame::SetHintCategoryState(int index, BYTE state)
{
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
		Timers[timer] = duration * TIMER_SCALE;
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
}

int CPDGame::GetWord(int offset)
{
	return (offset >= 0 && offset < (PD_SAVE_SIZE - 1)) ? (_gameData[offset + 1] << 8) | _gameData[offset] : 0;
}

void CPDGame::SetWord(int offset, int value)
{
	if (offset >= 0 && offset < (PD_SAVE_SIZE - 1))
	{
		_gameData[offset] = value;
		_gameData[offset + 1] = value >> 8;
	}
}

int CPDGame::IndexOfItemId(int item)
{
	int count = _gameData[PD_SAVE_ITEM_COUNT];
	for (int i = 0; i < count; i++)
	{
		if (_gameData[PD_SAVE_INVENTORY + i] == item)
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
