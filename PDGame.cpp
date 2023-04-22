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

	ReadGameXMLInfo(IDR_PD_XML);

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

	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	_gameData[PD_SAVE_GAME_DAY] = 1;
	_gameData[PD_SAVE_YEAR] = sysTime.wYear;
	_gameData[PD_SAVE_MONTH] = sysTime.wMonth;
	_gameData[PD_SAVE_DAY] = sysTime.wDay;
	_gameData[PD_SAVE_HOUR] = sysTime.wHour;
	_gameData[PD_SAVE_MINUTE] = sysTime.wMinute;
	_gameData[PD_SAVE_SECOND] = sysTime.wSecond;

	//UAKM_SAVE_MAP_ENTRY				0x14a
	//UAKM_SAVE_DMAP_ENTRY				0x150
	//SetData(UAKM_SAVE_CODED_MESSAGE, "YE UANE CIAFWBHED RIPB AEEIWALHEAL  YWLU CUAXLWLR AL  LUE XPWLE WA LUE  GIODEA GALE UILEO AL LUE PXPAO LWHE.LUE EAXXYIBD LIDARWX XWOWCIA.       ");

	//UAKM_SAVE_SCRIPT_ID				0x4c9

	//SetData(UAKM_SAVE_TRAVEL + 5, 1);	// Allow travel to Tex's Office

	//_gameData[UAKM_SAVE_CURRENT_ITEM] = -1;
	//_gameData[UAKM_SAVE_CHAPTER] = 1;

	//BinaryData bd = LoadEntry(L"GRAPHICS.AP", 23);
	//if (bd.Data != NULL && bd.Length == 0x1fe)
	//{
	//	CopyMemory(_gameData + UAKM_SAVE_PUZZLE_DATA, bd.Data, bd.Length);
	//	delete[] bd.Data;
	//}

	// Enable Travel to Tex' Office, Tex' Bedroom, Tex' Computer Room

	// Enable AskAbouts: 0,1,2,3,6,7,9,40,56,61
	SetAskAboutState(0, 1);		// Enable AskAbout Tex Murphy
	SetAskAboutState(1, 1);		// Enable AskAbout Chelsee Bando
	SetAskAboutState(2, 1);		// Enable AskAbout Louie LaMintz
	SetAskAboutState(3, 1);		// Enable AskAbout Rook Garner
	SetAskAboutState(6, 1);		// Enable AskAbout Gordon Fitzpatrick
	SetAskAboutState(7, 1);		// Enable AskAbout Thomas Malloy
	SetAskAboutState(9, 1);		// Enable AskAbout Nilo
	SetAskAboutState(40, 1);	// Enable AskAbout Tyson Matthews
	SetAskAboutState(56, 1);	// Enable AskAbout Newspaper photo of Malloy
	SetAskAboutState(61, 1);	// Enable AskAbout Sandra

	// Unknowns 10 and 11 set to 2 and 1

	// Parameter 0x19a, 0 = Entertainment Level, 1 = Game Player Level

	SetWord(PD_SAVE_CASH, 4000);
	SetItemState(0, 1);	// Cash
	SetItemState(1, 1);	// Photo of Malloy
	SetItemState(2, 1);	// Credit card
	SetItemState(4, 1);	// Fitzpatrick's card

	// Unknowns 12 to 16 set to 0,1,1,1,1

	// Clear some tables (hints?)

	LoadFromDMap(42);
	//LoadFromMap(1, 0);
}

BYTE CPDGame::GetParameter(int index)
{
	return (index >= 0 && index < 1024) ? _gameData[PD_SAVE_PARAMETERS + index] : 0;
}

void CPDGame::SetParameter(int index, BYTE value)
{
	if (index >= 0 && index < 1024)
	{
		if (index == 0x1a8)
		{
			int debug = 0;
		}
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
	if (index >= 0 && index <= 100)
	{
		_gameData[PD_SAVE_ASK_ABOUT_STATES + index] = state;

		// Add or remove from list
		int count = GetAskAboutCount();
		if (state == 0 || state == 2)
		{
			int i = 0;
			for (i = 0; i < count && i < 50; i++)
			{
				if (_gameData[PD_SAVE_ASK_ABOUTS + i] == index)
				{
					for (; i < count && i < 49; i++)
					{
						_gameData[PD_SAVE_ASK_ABOUTS + i] = _gameData[PD_SAVE_ASK_ABOUTS + i + 1];
					}
					_gameData[PD_SAVE_ASK_ABOUTS + count] = -1;

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
				if (_gameData[PD_SAVE_ASK_ABOUTS + i] == index)
				{
					return;
				}
			}

			_gameData[PD_SAVE_ASK_ABOUTS + count] = index;
			_gameData[PD_SAVE_ASK_ABOUT_COUNT]++;
		}
		else
		{
			int debug = 0;
		}

		_gameData[PD_SAVE_ASK_ABOUT_STATES + index] = state;
	}
	else
	{
		int debug = 0;
	}

}

int CPDGame::GetAskAboutCount()
{
	return _gameData[PD_SAVE_ASK_ABOUT_COUNT];
}

int CPDGame::GetAskAboutId(int index)
{
	return (index >= 0 && index < 100) ? _gameData[PD_SAVE_ASK_ABOUTS + index] : -1;
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

int CPDGame::GetBuyableItemCount()
{
	return 0;
}

int CPDGame::GetBuyableItemId(int index)
{
	return -1;
}
