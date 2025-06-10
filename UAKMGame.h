#pragma once

#include "GameBase.h"
#include "UAKMScript.h"
#include "UAKMScriptState.h"
#include "UAKMHintModule.h"

#define UAKM_SAVE_UNKNOWN1					0
#define UAKM_SAVE_PLAYER					2
#define UAKM_SAVE_LOCATION					0x1a
#define UAKM_SAVE_GAME_DAY					0x38
#define UAKM_SAVE_YEAR						0x39
#define UAKM_SAVE_MONTH						0x3b
#define UAKM_SAVE_DAY						0x3c
#define UAKM_SAVE_HOUR						0x3d
#define UAKM_SAVE_MINUTE					0x3e
#define UAKM_SAVE_SECOND					0x3f
#define UAKM_SAVE_COMMENT					0x40
#define UAKM_SAVE_PADDING1					0xd0
#define UAKM_SAVE_PADDING2					0x100
#define UAKM_SAVE_CRIMELINK_SELECTIONS		0x12c
#define UAKM_SAVE_MAP_ENTRY					0x14a
#define UAKM_SAVE_UNKNOWN2					0x14c
#define UAKM_SAVE_DMAP_ENTRY				0x150
#define UAKM_SAVE_UNKNOWN3					0x152
#define UAKM_SAVE_TIMERS_INITIAL			0x15a
#define UAKM_SAVE_TIMERS_CURRENT			0x19a
#define UAKM_SAVE_TIMERS					0x1da
#define UAKM_SAVE_PARAMETERS				0x1fa
#define UAKM_SAVE_ITEM_STATES				0x2fa
#define UAKM_SAVE_HINT_STATES				0x388
#define UAKM_SAVE_COLONELS_SAFE_CODE		0x431
#define UAKM_SAVE_CODED_MESSAGE				0x435
#define UAKM_SAVE_STASIS_SLIDER1			0x4c5
#define UAKM_SAVE_STASIS_SLIDER2			0x4c7
#define UAKM_SAVE_SCRIPT_ID					0x4c9
#define UAKM_SAVE_ITEMS_EXAMINED_FLAGS		0x4cb
#define UAKM_SAVE_STASIS_SETTINGS			0x4dd
#define UAKM_SAVE_STASIS_STAGES				0x4df
#define UAKM_SAVE_ASK_ABOUT_COUNT			0x4e1
#define UAKM_SAVE_CURRENT_ASK				0x4e3
#define UAKM_SAVE_UNKNOWN5					0x4e4
#define UAKM_SAVE_ASK_ABOUTS				0x4e6
#define UAKM_SAVE_ITEM_COUNT				0x518
#define UAKM_SAVE_CURRENT_ITEM				0x51a
#define UAKM_SAVE_UNKNOWN6					0x51b
#define UAKM_SAVE_INVENTORY					0x51d
#define UAKM_SAVE_HINT_CATEGORY_STATES		0x5ae
#define UAKM_SAVE_ASK_ABOUT_STATES			0x604
#define UAKM_SAVE_CHAPTER					0x636
#define UAKM_SAVE_AF_ACTIONS				0x638
#define UAKM_SAVE_DMAP_FLAG					0x69c
#define UAKM_SAVE_PUZZLE_DATA				0x69d
#define UAKM_SAVE_TRAVEL					0x89b
#define UAKM_SAVE_TRAVEL_BACKUP				0x8c2
#define UAKM_SAVE_UNKNOWN7					0x8e9
#define UAKM_SAVE_SCORE						0x8ea

#define UAKM_SAVE_SIZE						0x8ec

#define UAKM_MAX_ITEM_COUNT					142

//#define UAKM_TRAVEL_TEXS_OFFICE				1
//#define UAKM_TRAVEL_CHANDLER_AVE			2

class CUAKMGame : public CGameBase
{
protected:
	virtual BOOL Init();

public:
	CUAKMGame();
	~CUAKMGame();

	virtual void Render();

	virtual void MouseMove(POINT pt);
	virtual void MouseDown(POINT pt, int btn);
	virtual void KeyDown(WPARAM key, LPARAM lParam);
	virtual void KeyUp(WPARAM key, LPARAM lParam);

	virtual CScriptBase* GetScriptEngine() { return new CUAKMScript(); };
	virtual CScriptState* GetScriptState() { return new CUAKMScriptState(); }

	virtual void LoadGame(LPWSTR fileName);
	virtual void SaveGame(LPWSTR fileName);
	virtual void NewGame();

	virtual int GetSaveCommentOffset() { return UAKM_SAVE_COMMENT; }
	virtual int GetSaveCommentLength() { return UAKM_SAVE_PADDING1 - UAKM_SAVE_COMMENT; }

	virtual BYTE GetParameter(int index);
	virtual void SetParameter(int index, BYTE value);

	virtual BYTE GetData(int offset);
	virtual void SetData(int offset, BYTE value);
	virtual void SetData(int offset, char* text);
	virtual int GetWord(int offset, BOOL signExtend = FALSE);
	virtual void SetWord(int offset, int value);

	BYTE GetAskAboutState(int index);
	void SetAskAboutState(int index, BYTE state);
	virtual int GetAskAboutCount();
	virtual int GetAskAboutId(int index);

	virtual int GetScore();
	virtual void AddScore(int value);

	virtual int GetItemCount();
	virtual int GetItemId(int index);
	virtual int GetItemState(int item);
	virtual void SetItemState(int item, int state);
	virtual int GetCurrentItemId();
	virtual void SetCurrentItemId(int item);
	virtual int SelectNextItem();
	virtual int SelectPreviousItem();

	virtual int GetItemState(int base, int item) { return 0; }
	virtual void SetItemState(int base, int item, int state) { }

	virtual BYTE GetHintState(int index);
	virtual void SetHintState(int index, BYTE state, int score);
	virtual BYTE GetHintCategoryState(int index);
	virtual void SetHintCategoryState(int index, BYTE state);

	virtual void SetTimer(int timer, int duration);
	virtual int GetTimerState(int timer);
	virtual void ResetTimers();
	virtual void Tick(int ticks);

	virtual void SetItemExamined(int itemId, int conditionalScore = 0);

	virtual BOOL CanCancelVideo() { return (GetParameter(247) == 0); }

	virtual int GetLocationInitializationScriptId() { return 1000; }
	virtual int GetLocationEnvironmentScriptId() { return 2000; }

	virtual CHintModule* GetHintModule() { return new CUAKMHintModule(); }

	virtual void SetSelectedItem(int item) { _selectedItem = item; SetParameter(99, item); }

protected:
	int _lastDialoguePoint;
	int _frameTrigger;

	int IndexOfItemId(int item);

	virtual BOOL LoadIcons();
};
