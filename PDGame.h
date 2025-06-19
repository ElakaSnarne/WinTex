#pragma once

#include "GameBase.h"
#include "PDScript.h"
#include "PDScriptState.h"
#include "PDLocationModule.h"
#include "PDHintModule.h"

// Verified
#define PD_SAVE_HEADER_UNKNOWN1				0
#define PD_SAVE_HEADER_PLAYER				2
#define PD_SAVE_HEADER_GAME_DAY				0x38
#define PD_SAVE_HEADER_YEAR					0x39
#define PD_SAVE_HEADER_MONTH				0x3b
#define PD_SAVE_HEADER_DAY					0x3c
#define PD_SAVE_HEADER_HOUR					0x3d
#define PD_SAVE_HEADER_MINUTE				0x3e
#define PD_SAVE_HEADER_SECOND				0x3f
#define PD_SAVE_HEADER_HUNDREDTHS_OF_SECOND	0x40
#define PD_SAVE_HEADER_COMMENT				0x41
#define PD_SAVE_HEADER_PADDING				0xd0
#define PD_SAVE_HEADER_MAP_ID				0xd1
#define PD_SAVE_HEADER_DMAP_ID				0xd3
#define PD_SAVE_HEADER_MAP_FLAG				0xd5
#define PD_SAVE_HEADER_GAME_LEVEL			0xd6
#define PD_SAVE_HEADER_SCORE				0xd7

#define PD_SAVE_MAP_ID						0x14a
#define PD_SAVE_DMAP_ID						0x150

#define PD_SAVE_TIMERS_INITIAL				0x15a
#define PD_SAVE_TIMERS_CURRENT				0x19a
#define PD_SAVE_TIMERS						0x1da
#define PD_SAVE_PARAMETERS					0x1fa
#define PD_SAVE_PARAMETERS_DAY_IN_GAME		0x2f4
#define PD_SAVE_MAP_FLAG					0x2f6
#define PD_SAVE_PARAMETERS_GAME_LEVEL		0x394

#define PD_SAVE_STARTUP_POSITION			0x3e4

#define PD_SAVE_ITEM_STATES					0x499

#define PD_SAVE_HINT_STATES					0x5c5

#define PD_SAVE_SCRIPT_ID					0x6b5
#define PD_SAVE_ITEMS_EXAMINED_FLAGS		0x6b7
#define PD_SAVE_ASK_ABOUT_BASE				0x6dd
#define PD_SAVE_ASK_ABOUT_COUNT				0x6dd
#define PD_SAVE_CURRENT_ASK					0x6e1
#define PD_SAVE_ASK_ABOUT_UNKNOWN			0x6e6
#define PD_SAVE_ASK_ABOUTS					0x6e7

#define PD_SAVE_BUYABLES_COUNT				0x7b1
#define PD_SAVE_BUYABLES					0x7b9

#define PD_SAVE_ITEMS_BASE					0x7f7
#define PD_SAVE_ITEM_COUNT					0x7f7
#define PD_SAVE_CURRENT_ITEM				0x7fb
#define PD_SAVE_INVENTORY					0x801

#define PD_SAVE_HINT_CATEGORY_STATES		0xa5a

//0xab9-0xabc

//0xad3

#define PD_SAVE_ASK_ABOUT_STATES			0xabd
#define PD_SAVE_BUYABLES_ASK_ABOUT_STATES	0xb22

#define PD_SAVE_CROSSWORD_PUZZLE			0xf9b
#define PD_SAVE_CROSSWORD_PUZZLE_END		0x102a

#define PD_SAVE_VIDPHONE_BASE				0x1053
#define PD_SAVE_VIDPHONE_ENTRIES_COUNT		0x1053
#define PD_SAVE_VIDPHONE_ENTRIES			0x105d

#define PD_SAVE_UNKNOWN_1					0x108f
#define PD_SAVE_UNKNOWN_2					0x10a8

#define PD_SAVE_TRAVEL						0x10c1
#define PD_SAVE_TRAVEL_END					0x1124

#define PD_SAVE_SCORE						0x1189
#define PD_SAVE_CASH						0x118b

#define PD_SAVE_UNKNOWN12					0x11f2
#define PD_SAVE_SIZE						0x11f3

#define PD_MAX_ITEM_COUNT					300

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

	virtual int GetSaveCommentOffset() { return PD_SAVE_HEADER_COMMENT; }
	virtual int GetSaveCommentLength() { return PD_SAVE_HEADER_PADDING - PD_SAVE_HEADER_COMMENT; }

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

	virtual int GetItemState(int base, int item);
	virtual void SetItemState(int base, int item, int state);

	virtual BYTE GetHintState(int index);
	virtual void SetHintState(int index, BYTE state, int score);
	virtual BYTE GetHintCategoryState(int index);
	virtual void SetHintCategoryState(int index, BYTE state);
	void SetHintCategoryStateFromHint(int hintIndex);

	virtual void SetTimer(int timer, int duration);
	virtual int GetTimerState(int timer);
	virtual void ResetTimers();
	virtual void Tick(int ticks);

	virtual void SetItemExamined(int itemId, int conditionalScore = 0);

	virtual BOOL CanCancelVideo() { return _allowCancelVideo; }

	virtual int GetLocationInitializationScriptId() { return 0; }
	virtual int GetLocationEnvironmentScriptId() { return 1; }

	virtual CHintModule* GetHintModule() { return new CPDHintModule(); }

	virtual int GetBuyableItemCount();
	virtual int GetBuyableItemId(int index);
	virtual void SetBuyableItemState(int index, int state);

protected:
	virtual int GetWord(int offset, BOOL signExtend = FALSE);
	virtual void SetWord(int offset, int value);

	int _lastDialoguePoint;
	int _frameTrigger;

	int IndexOfItemId(int item);

	virtual BOOL LoadIcons();
	virtual CLocationModule* GetLocationModule(int entry, int startupPosition) { return new CPDLocationModule(entry, startupPosition); }

	void AddCash(int cashToAdd);
};
