#pragma once

#include "Map.h"
//#include "String.h"
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

class CGameBase
{
public:
	CGameBase();
	virtual ~CGameBase();

	virtual void Render() = NULL;
	virtual void MouseMove(POINT pt) = NULL;
	virtual void MouseDown(POINT pt, int btn) = NULL;
	virtual void KeyDown(WPARAM key, LPARAM lParam) = NULL;
	virtual void KeyUp(WPARAM key, LPARAM lParam) = NULL;

	void LoadFromDMap(int entry);
	void LoadFromMap(int entry, int startupPosition);

	virtual void Start() { Init(); CModuleController::Push(new CVideoModule(VideoType::Single, L"TITLE.AP", 0)); }

	virtual CScriptBase* GetScriptEngine() = NULL;
	virtual CScriptState* GetScriptState() = NULL;

	virtual void LoadGame(LPWSTR fileName) = NULL;
	virtual void SaveGame(LPWSTR fileName) = NULL;
	virtual void NewGame() = NULL;

	virtual BYTE GetParameter(int index) = NULL;
	virtual void SetParameter(int index, BYTE value) = NULL;

	virtual BYTE GetData(int offset) = NULL;
	virtual void SetData(int offset, BYTE value) = NULL;
	virtual void SetData(int offset, char* text) = NULL;
	virtual LPBYTE GetDataPointer() { return _gameData; };

	virtual BYTE GetAskAboutState(int index) = NULL;
	virtual void SetAskAboutState(int index, BYTE value) = NULL;
	virtual int GetAskAboutCount() = NULL;
	virtual int GetAskAboutId(int index) = NULL;

	virtual int GetScore() = NULL;
	virtual void AddScore(int value) = NULL;

	virtual int GetItemCount() = NULL;
	virtual int GetItemId(int index) = NULL;
	virtual int GetItemState(int item) = NULL;
	virtual void SetItemState(int item, int state) = NULL;
	virtual int GetCurrentItemId() = NULL;
	virtual void SetCurrentItemId(int item) = NULL;
	virtual int SelectNextItem() = NULL;
	virtual int SelectPreviousItem() = NULL;

	virtual BYTE GetHintState(int index) = NULL;
	virtual void SetHintState(int index, BYTE state, int score) = NULL;
	virtual BYTE GetHintCategoryState(int index) = NULL;
	virtual void SetHintCategoryState(int index, BYTE state) = NULL;

	virtual void SetTimer(int timer, int duration) = NULL;
	virtual int GetTimerState(int timer) = NULL;
	virtual void ResetTimers() = NULL;
	virtual void Tick(int ticks) = NULL;

	virtual void SetItemExamined(int itemId, int conditionalScore) = NULL;

	virtual BOOL CanCancelVideo() = NULL;

	virtual int GetLocationInitializationScriptId() = NULL;
	virtual int GetLocationEnvironmentScriptId() = NULL;

protected:
	virtual BOOL Init() = NULL;

	virtual int GetWord(int offset) = NULL;
	virtual void SetWord(int offset, int value) = NULL;

	virtual BOOL LoadIcons() { return FALSE; }
	BOOL LoadIcons(BinaryData bd);

	BYTE* _gameData;

	int Timers[32];

	void BuildFileList(int resource);

	virtual CLocationModule* GetLocationModule(int entry, int startupPosition) { return new CLocationModule(entry, startupPosition); }
};
