#pragma once

#include "GameBase.h"
#include "PDScript.h"
#include "PDScriptState.h"
#include "PDLocationModule.h"
#include "PDHintModule.h"

// Verified
#define PD_SAVE_PARAMETERS					0x1fa

#define PD_SAVE_ASK_ABOUTS					0x6e7

#define PD_SAVE_TRAVEL						0x10c1

#define PD_SAVE_SCORE						0x1189
#define PD_SAVE_CASH						0x118b

#define PD_SAVE_SIZE						0x11f3

// Unverified
#define PD_SAVE_UNKNOWN1					0
#define PD_SAVE_PLAYER						2
#define PD_SAVE_LOCATION					0x1a
#define PD_SAVE_GAME_DAY					0x38
#define PD_SAVE_YEAR						0x39
#define PD_SAVE_MONTH						0x3b
#define PD_SAVE_DAY							0x3c
#define PD_SAVE_HOUR						0x3d
#define PD_SAVE_MINUTE						0x3e
#define PD_SAVE_SECOND						0x3f
#define PD_SAVE_COMMENT						0x40
#define PD_SAVE_PADDING1					0xd0

#define PD_SAVE_PADDING2					0x100
#define PD_SAVE_CRIMELINK_SELECTIONS		0x12c
#define PD_SAVE_MAP_ENTRY					0x14a
#define PD_SAVE_UNKNOWN2					0x14c
#define PD_SAVE_DMAP_ENTRY					0x150
#define PD_SAVE_UNKNOWN3					0x152
#define PD_SAVE_TIMERS_INITIAL				0x15a
#define PD_SAVE_TIMERS_CURRENT				0x19a
#define PD_SAVE_TIMERS						0x1da
#define PD_SAVE_ITEM_STATES					0x2fa
#define PD_SAVE_HINT_STATES					0x388
#define PD_SAVE_COLONELS_SAFE_CODE			0x431
#define PD_SAVE_CODED_MESSAGE				0x435
#define PD_SAVE_STASIS_SLIDER1				0x4c5
#define PD_SAVE_STASIS_SLIDER2				0x4c7
#define PD_SAVE_SCRIPT_ID					0x4c9
#define PD_SAVE_ITEMS_EXAMINED_FLAGS		0x4cb
#define PD_SAVE_STASIS_SETTINGS				0x4dd
#define PD_SAVE_STASIS_STAGES				0x4df
#define PD_SAVE_ASK_ABOUT_COUNT				0x4e1
#define PD_SAVE_CURRENT_ASK					0x4e3
#define PD_SAVE_UNKNOWN5					0x4e4
#define PD_SAVE_ITEM_COUNT					0x518
#define PD_SAVE_CURRENT_ITEM				0x51a
#define PD_SAVE_UNKNOWN6					0x51b
#define PD_SAVE_INVENTORY					0x51d
#define PD_SAVE_HINT_CATEGORY_STATES		0x5ae
#define PD_SAVE_ASK_ABOUT_STATES			0x604
#define PD_SAVE_CHAPTER						0x636
#define PD_SAVE_AF_ACTIONS					0x638
#define PD_SAVE_DMAP_FLAG					0x69c
#define PD_SAVE_PUZZLE_DATA					0x69d
#define PD_SAVE_TRAVEL_BACKUP				0x8c2
#define PD_SAVE_UNKNOWN7					0x8e9

#define PD_MAX_ITEM_COUNT					142

class CPDGame : public CGameBase
{
protected:
	virtual BOOL Init();

public:
	CPDGame();
	~CPDGame();

	virtual void Render();

	virtual void MouseMove(POINT pt);
	virtual void MouseDown(POINT pt, int btn);
	virtual void KeyDown(WPARAM key, LPARAM lParam);
	virtual void KeyUp(WPARAM key, LPARAM lParam);

	virtual CScriptBase* GetScriptEngine() { return new CPDScript(); };
	virtual CScriptState* GetScriptState() { return new CPDScriptState(); }

	virtual void LoadGame(LPWSTR fileName);
	virtual void SaveGame(LPWSTR fileName);
	virtual void NewGame();

	virtual BYTE GetParameter(int index);
	virtual void SetParameter(int index, BYTE value);

	virtual BYTE GetData(int offset);
	virtual void SetData(int offset, BYTE value);
	virtual void SetData(int offset, char* text);

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

	virtual BYTE GetHintState(int index);
	virtual void SetHintState(int index, BYTE state, int score);
	virtual BYTE GetHintCategoryState(int index);
	virtual void SetHintCategoryState(int index, BYTE state);

	virtual void SetTimer(int timer, int duration);
	virtual int GetTimerState(int timer);
	virtual void ResetTimers();
	virtual void Tick(int ticks);

	virtual void SetItemExamined(int itemId, int conditionalScore = 0);

	virtual BOOL CanCancelVideo() { return TRUE;/* (GetParameter(247) == 0);*/ }

	virtual int GetLocationInitializationScriptId() { return 0; }
	virtual int GetLocationEnvironmentScriptId() { return 1; }

	virtual CHintModule* GetHintModule() { return new CPDHintModule(); }

protected:
	virtual int GetWord(int offset, BOOL signExtend = FALSE);
	virtual void SetWord(int offset, int value);

	int _lastDialoguePoint;
	int _frameTrigger;

	int IndexOfItemId(int item);

	virtual BOOL LoadIcons();
	virtual CLocationModule* GetLocationModule(int entry, int startupPosition) { return new CPDLocationModule(entry, startupPosition); }
};
