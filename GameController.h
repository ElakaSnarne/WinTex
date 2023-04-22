#pragma once

#include "GameBase.h"
#include "ScriptBase.h"
#include <unordered_map>
//#include "String.h"
#include <string>

class CGameController
{
public:
	static void Init();
	static BOOL StartGame(CGameBase* pGame);

	// Game functions
	static int GetScore() { return Game->GetScore(); }
	static void AddScore(int val) { Game->AddScore(val); }
	static BYTE GetParameter(int index);
	static void SetParameter(int index, BYTE value);
	static void SetHintState(int index, BYTE state, int score);
	static BYTE GetHintState(int index);
	static void SetHintCategoryState(int index, BYTE state);
	static BYTE GetHintCategoryState(int index);

	static void SetTimer(int timer, int duration);
	static int GetTimerState(int timer);
	static void ResetTimers();

	static CScriptBase* GetScriptEngine() { return Game->GetScriptEngine(); }
	static CScriptState* GetScriptState() { return Game->GetScriptState(); }

	static void SetFileName(int index, std::wstring name);
	static std::wstring GetFileName(int index);
	static void SetItemName(int index, std::wstring name);
	static std::wstring GetItemName(int index);
	static void SetAskAboutName(int index, std::wstring name);
	static std::wstring GetAskAboutName(int index);

	static void SetSituationDescriptionL(int ix, std::wstring value);
	static std::wstring GetSituationDescriptionL(int ix);
	static void SetSituationDescriptionD(int ix, std::wstring value);
	static std::wstring GetSituationDescriptionD(int ix);

	static BYTE GetData(int offset) { return Game->GetData(offset); }
	static void SetData(int offset, BYTE value) { Game->SetData(offset, value); }
	static LPBYTE GetDataPointer() { return Game->GetDataPointer(); }
	static WORD GetWord(int offset) { return Game->GetWord(offset); }
	static void SetWord(int offset, WORD value) { Game->SetWord(offset, value); }

	static void SetItemState(int item, int state);
	static int GetItemState(int item);
	static int GetItemCount() { return Game->GetItemCount(); }
	static int GetItemId(int index) { return Game->GetItemId(index); }
	static int GetCurrentItemId() { return Game->GetCurrentItemId(); }
	static void SetCurrentItemId(int item) { return Game->SetCurrentItemId(item); }
	static int SelectNextItem() { return Game->SelectNextItem(); }
	static int SelectPreviousItem() { return Game->SelectPreviousItem(); }

	static void SetAskAboutState(int index, BYTE state);
	static BYTE GetAskAboutState(int index);
	static int GetAskAboutCount() { return Game->GetAskAboutCount(); }
	static int GetAskAboutId(int index) { return Game->GetAskAboutId(index); }

	static void SetItemExamined(int itemId, int conditionalScore = 0) { return Game->SetItemExamined(itemId, conditionalScore); }

	static void Tick(int ticks);

	static BOOL ItemsChanged;
	static BOOL AskAboutChanged;
	static BOOL BuyChanged;

	static void LoadFromDMap(int entry) { Game->LoadFromDMap(entry); }

	static void NewGame() { Game->NewGame(); }
	static void LoadGame(LPWSTR fileName) { Game->LoadGame(fileName); }
	static void SaveGame(LPWSTR fileName) { Game->SaveGame(fileName); }

	static BOOL CanCancelTravel;
	static BOOL CanCancelVideo() { return Game->CanCancelVideo(); }
	static void CanCancelVideo(BOOL allow) { Game->CanCancelVideo(allow); }

	static int GetLocationInitializationScriptId() { return Game->GetLocationInitializationScriptId(); }
	static int GetLocationEnvironmentScriptId() { return Game->GetLocationEnvironmentScriptId(); }

	static void AutoSave() { SaveGame(L"GAMES\\SAVEGAME.000"); }

	static int GetHintCategoryCount() { return Game->GetHintCategoryCount(); }
	static CHintCategory* GetHintCategory(int index) { return Game->GetHintCategory(index); }

	static CHintModule* GetHintModule() { return Game->GetHintModule(); }

	static void SetSelectedItem(int item) { Game->SetSelectedItem(item); }

	static int GetBuyableItemCount() { return Game->GetBuyableItemCount(); }
	static int GetBuyableItemId(int index) { return Game->GetBuyableItemId(index); }
	static void SetBuyableItemName(int index, std::wstring name);
	static std::wstring GetBuyableItemName(int index);

protected:
	static CGameBase* Game;

	static std::unordered_map<int, std::wstring> FileMap;

	static std::unordered_map<int, std::wstring> AskAboutMap;
	static std::unordered_map<int, std::wstring> ItemMap;
	static std::unordered_map<int, std::wstring> BuyableItemMap;

	static std::unordered_map<int, std::wstring> _lSituations;
	static std::unordered_map<int, std::wstring> _dSituations;
};
