#pragma once

#include "Map.h"
#include "Location.h"
#include "DXText.h"
#include "DXBitmap.h"
#include "AnimatedCursor.h"
#include "UAKMScript.h"
#include "DXFrame.h"
#include "DXScreen.h"
#include "DXLabel.h"
#include "VideoModule.h"
#include "LocationModule.h"
#include "HintCategory.h"
#include "HintModule.h"

class CGameBase
{
public:
	CGameBase();
	virtual ~CGameBase();

	virtual void Render() = 0;
	virtual void MouseMove(POINT pt) = 0;
	virtual void MouseDown(POINT pt, int btn) = 0;
	virtual void KeyDown(WPARAM key, LPARAM lParam) = 0;
	virtual void KeyUp(WPARAM key, LPARAM lParam) = 0;

	virtual void LoadFromDMap(int entry);
	virtual void LoadFromMap(int entry, int startupPosition);

	virtual void Start();

	virtual CScriptBase* GetScriptEngine() = 0;
	virtual CScriptState* GetScriptState() = 0;

	virtual void LoadGame(LPWSTR fileName) = 0;
	virtual void SaveGame(LPWSTR fileName) = 0;
	virtual void NewGame() = 0;

	virtual int GetSaveCommentOffset() = 0;
	virtual int GetSaveCommentLength() = 0;

	virtual BYTE GetParameter(int index) = 0;
	virtual void SetParameter(int index, BYTE value) = 0;

	virtual BYTE GetData(int offset) = 0;
	virtual void SetData(int offset, BYTE value) = 0;
	virtual void SetData(int offset, char* text) = 0;
	virtual LPBYTE GetDataPointer() { return _gameData; };
	virtual int GetWord(int offset, BOOL signExtend = FALSE) = 0;
	virtual void SetWord(int offset, int value) = 0;
	virtual void Copy(LPBYTE source, int destinationOffset, int length) = 0;

	virtual BYTE GetAskAboutState(int index) = 0;
	virtual void SetAskAboutState(int index, BYTE value) = 0;
	virtual int GetAskAboutCount() = 0;
	virtual int GetAskAboutId(int index) = 0;

	virtual int GetScore() = 0;
	virtual void AddScore(int value) = 0;

	virtual int GetItemCount() = 0;
	virtual int GetItemId(int index) = 0;
	virtual int GetItemState(int item) = 0;
	virtual void SetItemState(int item, int state) = 0;
	virtual int GetCurrentItemId() = 0;
	virtual void SetCurrentItemId(int item) = 0;
	virtual int SelectNextItem() = 0;
	virtual int SelectPreviousItem() = 0;

	virtual int GetItemState(int base, int item) = 0;
	virtual void SetItemState(int base, int item, int state) = 0;

	virtual BYTE GetHintState(int index) = 0;
	virtual void SetHintState(int index, BYTE state, int score) = 0;
	virtual BYTE GetHintCategoryState(int index) = 0;
	virtual void SetHintCategoryState(int index, BYTE state) = 0;

	virtual void SetTimer(int timer, int duration) = 0;
	virtual int GetTimerState(int timer) = 0;
	virtual void ResetTimers() = 0;
	virtual void Tick(int ticks) = 0;

	virtual void SetItemExamined(int itemId, int conditionalScore) = 0;

	BOOL _allowCancelVideo;
	virtual BOOL CanCancelVideo() = 0;
	virtual void CanCancelVideo(BOOL allow) { _allowCancelVideo = allow; }

	virtual int GetLocationInitializationScriptId() = 0;
	virtual int GetLocationEnvironmentScriptId() = 0;

	int GetHintCategoryCount() { return _hintCategoryCount; }
	CHintCategory* GetHintCategory(int index);

	virtual CHintModule* GetHintModule() = 0;

	virtual void SetSelectedItem(int item) { _selectedItem = item; }
	virtual int GetSelectedItem(int item) { return _selectedItem; }

	virtual int GetBuyableItemCount() { return 0; }
	virtual int GetBuyableItemId(int index) { return -1; }
	virtual void SetBuyableItemState(int index, int state) { }

protected:
	virtual BOOL Init() = 0;

	virtual BOOL LoadIcons() { return FALSE; }
	BOOL LoadIcons(BinaryData bd);

	BYTE* _gameData;

	int Timers[32];

	void ReadGameXMLInfo(int resource);

	virtual CLocationModule* GetLocationModule(int entry, int startupPosition) { return new CLocationModule(entry, startupPosition); }

	std::unordered_map<int, CHintCategory*> _hintCategories;
	int _hintCategoryCount;

	int _selectedItem;
};
