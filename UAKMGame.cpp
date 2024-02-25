#include "UAKMGame.h"
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
#include "UAKMMainMenuModule.h"
#include "Items.h"
#include "LocationModule.h"
#include "InputMapping.h"
#include "UAKMMap.h"
#include "UAKMDMap.h"

BOOL CUAKMGame::Init()
{
	if (CModuleController::Init(new CUAKMMap(), new CUAKMDMap()) && LoadIcons() && CItems::Init())
	{
		// Make sure Tex player exists
		if (!CFile::Exists(L"PLAYERS\\TEX___00.PLR"))
		{
			CFile file;
			if (file.Open(L"PLAYERS\\TEX___00.PLR", CFile::Mode::Write))
			{
				BYTE buffer[256];
				ZeroMemory(buffer, 256);
				buffer[1] = 1;
				buffer[2] = 'T';
				buffer[3] = 'E';
				buffer[4] = 'X';
				memset(buffer + 5, ' ', 21);
				buffer[26] = 0;		// Player has died?
				buffer[27] = 1;		// Watched intro?

				file.Write(buffer, 256);
				file.Close();
			}
		}

		CModuleController::Push(new CUAKMMainMenuModule());

		return TRUE;
	}

	return FALSE;
}

CUAKMGame::CUAKMGame()
{
	_gameData = new BYTE[UAKM_SAVE_SIZE];
	ZeroMemory(_gameData, UAKM_SAVE_SIZE);

	_lastDialoguePoint = -1;
	_frameTrigger = -1;

	ReadGameXMLInfo(IDR_UAKM_XML);

	SetGamePath(L".\\");
}

CUAKMGame::~CUAKMGame()
{
	// Get rid of items, cursors and anim
	CItems::Dispose();

	CAnimationController::Clear();

	// DialogueOptions
	for (int i = 0; i < 3; i++)
	{
		DialogueOptions[i].Clear();
	}
}

void CUAKMGame::Render()
{
	// Call Render on active module
	CModuleController::Render();
}

void CUAKMGame::MouseDown(POINT pt, int btn)
{
	CModuleController::MouseDown(pt, btn);
}

// TODO: Perhaps these should be in the base class?

void CUAKMGame::MouseMove(POINT pt)
{
	CModuleController::MouseMove(pt);
}

void CUAKMGame::KeyDown(WPARAM key, LPARAM lParam)
{
	CModuleController::KeyDown(key, lParam);
}

void CUAKMGame::KeyUp(WPARAM key, LPARAM lParam)
{
	CModuleController::KeyUp(key, lParam);
}

void CUAKMGame::NewGame()
{
	// Reset game buffer
	ZeroMemory(_gameData, UAKM_SAVE_SIZE);
	FillMemory(_gameData + 2, UAKM_SAVE_PADDING1 - 2, ' ');

	_gameData[UAKM_SAVE_UNKNOWN1 + 1] = 1;
	SetData(UAKM_SAVE_PLAYER, "TEX");

	_gameData[UAKM_SAVE_GAME_DAY] = 1;

	SetData(UAKM_SAVE_CODED_MESSAGE, "YE UANE CIAFWBHED RIPB AEEIWALHEAL  YWLU CUAXLWLR AL  LUE XPWLE WA LUE  GIODEA GALE UILEO AL LUE PXPAO LWHE.LUE EAXXYIBD LIDARWX XWOWCIA.       ");

	SetAskAboutState(0, 1);							// Rook Garner
	SetAskAboutState(1, 1);							// Chelsee Bando
	SetAskAboutState(2, 1);							// Louie Lamintz
	SetAskAboutState(3, 1);							// Francesca Lucido
	SetAskAboutState(4, 1);							// Sal Lucido
	SetAskAboutState(5, 1);							// Ardo Newpop
	SetAskAboutState(17, 1);						// Colonel

	SetData(UAKM_SAVE_TRAVEL + 5, 1);				// Allow travel to Tex's Office

	_gameData[UAKM_SAVE_CURRENT_ASK] = -1;			// No ask about selected
	_gameData[UAKM_SAVE_CURRENT_ITEM] = -1;			// No selected item
	_gameData[UAKM_SAVE_CHAPTER] = 1;
	_gameData[UAKM_SAVE_PARAMETERS + 99] = -1;
	_gameData[UAKM_SAVE_PARAMETERS + 251] = 2;
	_gameData[UAKM_SAVE_PARAMETERS + 250] = 1;		// Game day
	_gameData[UAKM_SAVE_HINT_CATEGORY_STATES + 1] = 1;

	BinaryData bd = LoadEntry(L"GRAPHICS.AP", 23);
	if (bd.Data != NULL && bd.Length == 0x1fe)
	{
		CopyMemory(_gameData + UAKM_SAVE_PUZZLE_DATA, bd.Data, bd.Length);
		delete[] bd.Data;
	}

	LoadFromDMap(0);
}

void CUAKMGame::LoadGame(LPWSTR fileName)
{
	BYTE data[UAKM_SAVE_SIZE];
	CFile file;
	if (file.Open(fileName))
	{
		int read = file.Read(data, UAKM_SAVE_SIZE);
		file.Close();

		if (read == UAKM_SAVE_SIZE)
		{
			CopyMemory(_gameData, data, UAKM_SAVE_SIZE);

			// Validate inventory, check current item
			int currentItem = _gameData[UAKM_SAVE_CURRENT_ITEM];
			if (currentItem != 0xff)
			{
				int itemCount = _gameData[UAKM_SAVE_ITEM_COUNT];
				bool found = false;
				for (int i = 0; i < itemCount; i++)
				{
					if (_gameData[UAKM_SAVE_INVENTORY + i] == currentItem)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					_gameData[UAKM_SAVE_CURRENT_ITEM] = 0xff;
				}
			}

			// Should now load location or dialogue
			if (_gameData[UAKM_SAVE_DMAP_FLAG])
			{
				CModuleController::Push(new CVideoModule(VideoType::Scripted, _gameData[UAKM_SAVE_DMAP_ENTRY], GetWord(UAKM_SAVE_SCRIPT_ID)));
			}
			else
			{
				CModuleController::Push(new CLocationModule(_gameData[UAKM_SAVE_MAP_ENTRY], CGameController::GetParameter(249)));
			}
		}
	}
}

void CUAKMGame::SaveGame(LPWSTR fileName)
{
	CFile file;
	if (file.Open(fileName, CFile::Mode::Write))
	{
		// Populate situation description
		std::wstring sit;
		if (_gameData[UAKM_SAVE_DMAP_FLAG] == 0)
		{
			// Location
			sit = CGameController::GetSituationDescriptionL(_gameData[UAKM_SAVE_MAP_ENTRY]);
		}
		else
		{
			// Dialogue
			sit = CGameController::GetSituationDescriptionD(_gameData[UAKM_SAVE_DMAP_ENTRY]);
		}

		memset(_gameData + UAKM_SAVE_LOCATION, ' ', 30);
		for (int i = 0; i < sit.size() && i < 0x1e; i++)
		{
			_gameData[UAKM_SAVE_LOCATION + i] = sit[i] & 0xFF;
		}

		SYSTEMTIME time;
		GetLocalTime(&time);
		_gameData[UAKM_SAVE_GAME_DAY] = min(7, max(1, _gameData[UAKM_SAVE_PARAMETERS + 250]));
		_gameData[UAKM_SAVE_YEAR] = (BYTE)(time.wYear & 0xff);
		_gameData[UAKM_SAVE_YEAR + 1] = (BYTE)((time.wYear >> 8) & 0xff);
		_gameData[UAKM_SAVE_MONTH] = (BYTE)time.wMonth;
		_gameData[UAKM_SAVE_DAY] = (BYTE)time.wDay;
		_gameData[UAKM_SAVE_HOUR] = (BYTE)time.wHour;
		_gameData[UAKM_SAVE_MINUTE] = (BYTE)time.wMinute;
		_gameData[UAKM_SAVE_SECOND] = (BYTE)time.wSecond;

		file.Write(_gameData, UAKM_SAVE_SIZE);
		file.Close();
	}
}

BYTE CUAKMGame::GetParameter(int index)
{
	return (index >= 0 && index < 256) ? _gameData[UAKM_SAVE_PARAMETERS + index] : 0;
}

void CUAKMGame::SetParameter(int index, BYTE value)
{
	if (index >= 0 && index < 256)
	{
		_gameData[UAKM_SAVE_PARAMETERS + index] = value;
	}
}

int CUAKMGame::GetWord(int offset, BOOL signExtend)
{
	int result = (offset >= 0 && offset < (UAKM_SAVE_SIZE - 1)) ? (_gameData[offset + 1] << 8) | _gameData[offset] : 0;
	if (signExtend && result & 0x8000)
	{
		result |= ~0xffff;
	}

	return result;
}

void CUAKMGame::SetWord(int offset, int value)
{
	if (offset >= 0 && offset < (UAKM_SAVE_SIZE - 1))
	{
		_gameData[offset] = value;
		_gameData[offset + 1] = value >> 8;
	}
}

BYTE CUAKMGame::GetAskAboutState(int index)
{
	return (index >= 0 && index < 45) ? _gameData[UAKM_SAVE_ASK_ABOUT_STATES + index] : 0;
}

void CUAKMGame::SetAskAboutState(int index, BYTE state)
{
	if (index >= 0 && index <= 45)
	{
		// Add or remove from list
		int count = GetAskAboutCount();
		if (state == 0 || state == 2)
		{
			int i = 0;
			for (i = 0; i < count && i < 50; i++)
			{
				if (_gameData[UAKM_SAVE_ASK_ABOUTS + i] == index)
				{
					for (; i < count && i < 49; i++)
					{
						_gameData[UAKM_SAVE_ASK_ABOUTS + i] = _gameData[UAKM_SAVE_ASK_ABOUTS + i + 1];
					}
					_gameData[UAKM_SAVE_ASK_ABOUTS + count] = -1;

					_gameData[UAKM_SAVE_ASK_ABOUT_COUNT]--;

					break;
				}
			}
		}
		else if (state == 1)
		{
			// Check if state already set
			for (int i = 0; i < count; i++)
			{
				if (_gameData[UAKM_SAVE_ASK_ABOUTS + i] == index)
				{
					return;
				}
			}

			_gameData[UAKM_SAVE_ASK_ABOUTS + count] = index;
			_gameData[UAKM_SAVE_ASK_ABOUT_COUNT]++;
		}

		_gameData[UAKM_SAVE_ASK_ABOUT_STATES + index] = state;
	}
}

int CUAKMGame::GetAskAboutCount()
{
	return _gameData[UAKM_SAVE_ASK_ABOUT_COUNT];
}

int CUAKMGame::GetAskAboutId(int index)
{
	return _gameData[UAKM_SAVE_ASK_ABOUTS + index];
}

int CUAKMGame::GetScore()
{
	return GetWord(UAKM_SAVE_SCORE, TRUE);
}

void CUAKMGame::AddScore(int value)
{
	SetWord(UAKM_SAVE_SCORE, GetWord(UAKM_SAVE_SCORE) + value);
}

int CUAKMGame::GetItemCount()
{
	return _gameData[UAKM_SAVE_ITEM_COUNT];
}

int CUAKMGame::GetItemId(int index)
{
	return (index >= 0 && index < UAKM_MAX_ITEM_COUNT) ? _gameData[UAKM_SAVE_INVENTORY + index] : 0;
}

int CUAKMGame::GetItemState(int item)
{
	return _gameData[UAKM_SAVE_ITEM_STATES + item];
}

void CUAKMGame::SetItemState(int item, int state)
{
	if (item >= 0 && item < UAKM_MAX_ITEM_COUNT)
	{
		_gameData[UAKM_SAVE_ITEM_STATES + item] = state;

		int count = _gameData[UAKM_SAVE_ITEM_COUNT];

		// Add or remove from list
		if (state == 0 || state == 2)
		{
			for (int i = 0; i < count && i < UAKM_MAX_ITEM_COUNT; i++)
			{
				if (_gameData[UAKM_SAVE_INVENTORY + i] == item)
				{
					for (int j = i + 1; j < count && j < UAKM_MAX_ITEM_COUNT; j++)
					{
						_gameData[UAKM_SAVE_INVENTORY + j - 1] = _gameData[UAKM_SAVE_INVENTORY + j];
					}

					_gameData[UAKM_SAVE_ITEM_COUNT]--;

					break;
				}
			}

			if (_gameData[UAKM_SAVE_CURRENT_ITEM] == item)
			{
				_gameData[UAKM_SAVE_CURRENT_ITEM] = -1;
			}
		}
		else if (state == 1)
		{
			BOOL alreadyInInventory = FALSE;
			for (int i = 0; i < count; i++)
			{
				if (_gameData[UAKM_SAVE_INVENTORY + i] == item)
				{
					alreadyInInventory = TRUE;
					break;
				}
			}

			if (!alreadyInInventory)
			{
				_gameData[UAKM_SAVE_INVENTORY + count] = item;
				_gameData[UAKM_SAVE_ITEM_COUNT]++;
				_gameData[UAKM_SAVE_CURRENT_ITEM] = item;
			}
		}
	}
}

int CUAKMGame::GetCurrentItemId()
{
	return (_gameData[UAKM_SAVE_CURRENT_ITEM] == 0xff) ? -1 : _gameData[UAKM_SAVE_CURRENT_ITEM];
}

void CUAKMGame::SetCurrentItemId(int item)
{
	_gameData[UAKM_SAVE_CURRENT_ITEM] = item;
}

int CUAKMGame::SelectNextItem()
{
	int count = _gameData[UAKM_SAVE_ITEM_COUNT];
	int currentItem = _gameData[UAKM_SAVE_CURRENT_ITEM];
	int newIndex = IndexOfItemId(currentItem) + 1;
	if (newIndex >= count)
	{
		newIndex = -1;
	}

	_gameData[UAKM_SAVE_CURRENT_ITEM] = newIndex < 0 ? -1 : _gameData[UAKM_SAVE_INVENTORY + newIndex];

	return _gameData[UAKM_SAVE_CURRENT_ITEM];
}

int CUAKMGame::SelectPreviousItem()
{
	int count = _gameData[UAKM_SAVE_ITEM_COUNT];
	int currentItem = _gameData[UAKM_SAVE_CURRENT_ITEM];
	int newIndex = IndexOfItemId(currentItem) - 1;
	if (newIndex < -1)
	{
		newIndex = count - 1;
	}

	_gameData[UAKM_SAVE_CURRENT_ITEM] = newIndex < 0 ? -1 : _gameData[UAKM_SAVE_INVENTORY + newIndex];

	return _gameData[UAKM_SAVE_CURRENT_ITEM];
}

int CUAKMGame::IndexOfItemId(int item)
{
	int count = _gameData[UAKM_SAVE_ITEM_COUNT];
	for (int i = 0; i < count; i++)
	{
		if (_gameData[UAKM_SAVE_INVENTORY + i] == item)
		{
			return i;
		}
	}

	return -1;
}

BYTE CUAKMGame::GetHintState(int index)
{
	int val = 0;
	if (index >= 0 && index < 1352)
	{
		int byte = index / 4;
		int shift = (index & 3) * 2;
		val = (_gameData[UAKM_SAVE_HINT_STATES + byte] >> shift) & 3;
	}

	return val;
}

int HintStatePairs[] = { 3, 21, 56, 61, 263, 289, 12, 11, 264, 290, 434, 440, 434, 446, 234, 186, 343, 342 };
void CUAKMGame::SetHintState(int index, BYTE state, int score)
{
	if (index >= 0 && index < 1352)
	{
		int byte = index / 4;
		int shift = (index & 3) * 2;
		int oldState = (_gameData[UAKM_SAVE_HINT_STATES + byte] >> shift) & 3;
		_gameData[UAKM_SAVE_HINT_STATES + byte] |= (state & 3) << shift;
		if (oldState == 0 && score > 0)
		{
			AddScore(score);
		}

		for (int i = 0; i < 9; i++)
		{
			if (HintStatePairs[i * 2] == index)
			{
				int pair = HintStatePairs[i * 2 + 1];
				byte = pair / 4;
				shift = (pair & 3) * 2;
				oldState = (_gameData[UAKM_SAVE_HINT_STATES + byte] >> shift) & 3;
				_gameData[UAKM_SAVE_HINT_STATES + byte] |= (state & 3) << shift;
				if (oldState == 0)
				{
					AddScore(1);
				}
			}
		}
	}
}

BYTE CUAKMGame::GetHintCategoryState(int index)
{
	return (index >= 0 && index < 86) ? _gameData[UAKM_SAVE_HINT_CATEGORY_STATES + index] : 0;
}

void CUAKMGame::SetHintCategoryState(int index, BYTE state)
{
	if (index >= 0 && index < 86)
	{
		_gameData[UAKM_SAVE_HINT_CATEGORY_STATES + index] = state;
	}
}

void CUAKMGame::SetTimer(int timer, int duration)
{
	if (timer >= 0 && timer < 32)
	{
		_gameData[UAKM_SAVE_TIMERS + timer] = 1;

		SetWord(UAKM_SAVE_TIMERS_INITIAL + timer * 2, duration);
		SetWord(UAKM_SAVE_TIMERS_CURRENT + timer * 2, duration);
		Timers[timer] = static_cast<int>(duration * TIMER_SCALE);
	}

	//Trace(L"Setting timer ");
	//Trace(timer);
	//Trace(L" to ");
	//Trace(Timers[timer]);
	//TraceLine(L" ms");
}

int CUAKMGame::GetTimerState(int timer)
{
	int state = (timer >= 0 && timer < 32) ? _gameData[UAKM_SAVE_TIMERS + timer] : 0;

	//Trace(L"State of timer ");
	//Trace(timer);
	//Trace(L" is ");
	//Trace(state);
	//Trace(L", time left ");
	//Trace(Timers[timer]);
	//TraceLine(L" ms");

	return state;
}

void CUAKMGame::ResetTimers()
{
	// TODO: Disable timers
	ZeroMemory(Timers, sizeof(Timers));
	ZeroMemory(_gameData + UAKM_SAVE_TIMERS_INITIAL, 32 * 5);
}

void CUAKMGame::Tick(int ticks)
{
	// TODO: Timer 9 can be interrupted (showing picture of beaten Tex)
	// Timer 11 looks like MIDI
	// Timer 12, 13 & 14 are used for car sounds in main street

	// Check timers
	for (int i = 0; i < 32; i++)
	{
		if (_gameData[UAKM_SAVE_TIMERS + i] > 0)
		{
			// Timer is active, reduce by ticks
			Timers[i] = max(0, Timers[i] - ticks);
			if (Timers[i] == 0)
			{
				_gameData[UAKM_SAVE_TIMERS + i] = 0;
				SetWord(UAKM_SAVE_TIMERS_CURRENT + i * 2, GetWord(UAKM_SAVE_TIMERS_INITIAL + i * 2));
			}
		}
	}
}

BYTE CUAKMGame::GetData(int offset)
{
	return (offset >= 0 && offset < UAKM_SAVE_SIZE) ? _gameData[offset] : 0;
}

void CUAKMGame::SetData(int offset, BYTE value)
{
	if (offset >= 0 && offset < UAKM_SAVE_SIZE)
	{
		_gameData[offset] = value;
	}
}

void CUAKMGame::SetData(int offset, char* text)
{
	if (offset >= 0)
	{
		while (offset < UAKM_SAVE_SIZE && *text)
		{
			_gameData[offset++] = *(text++);
		}
	}
}

void CUAKMGame::SetItemExamined(int itemId, int conditionalScore)
{
	if (itemId >= 0 && itemId < UAKM_MAX_ITEM_COUNT)
	{
		int byte = itemId / 8;
		int shift = itemId & 7;

		int oldState = (_gameData[UAKM_SAVE_ITEMS_EXAMINED_FLAGS + byte] & (1 << shift));
		_gameData[UAKM_SAVE_ITEMS_EXAMINED_FLAGS + byte] |= (1 << shift);

		if (oldState == 0 && conditionalScore > 0)
		{
			AddScore(conditionalScore);
		}
	}
}

BOOL CUAKMGame::LoadIcons()
{
	BOOL result = FALSE;
	BinaryData bd = CLZ::Decompress(L"ICONS.LZ");
	if (bd.Data != NULL && bd.Length > 0)
	{
		result = CGameBase::LoadIcons(bd);
		delete[] bd.Data;
	}

	return result;
}
