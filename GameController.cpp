#include "GameController.h"
#include "Utilities.h"

CGameBase* CGameController::Game = NULL;

std::unordered_map<int, std::wstring> CGameController::FileMap;

BOOL CGameController::CanCancelTravel = TRUE;

std::unordered_map<int, std::wstring> CGameController::AskAboutMap;
std::unordered_map<int, std::wstring> CGameController::ItemMap;

BOOL CGameController::ItemsChanged = FALSE;
BOOL CGameController::AskAboutChanged = FALSE;

std::unordered_map<int, std::wstring> CGameController::_lSituations;
std::unordered_map<int, std::wstring> CGameController::_dSituations;

void CGameController::Init()
{
}

BOOL CGameController::StartGame(CGameBase* pGame)
{
	Game = pGame;

	// Play title animation
	Game->Start();

	return (Game != NULL);
}

BYTE CGameController::GetParameter(int index)
{
	return Game->GetParameter(index);
}

void CGameController::SetParameter(int index, BYTE value)
{
	Game->SetParameter(index, value);
}

BYTE CGameController::GetAskAboutState(int index)
{
	return Game->GetAskAboutState(index);
}

void CGameController::SetAskAboutState(int index, BYTE state)
{
	AskAboutChanged = TRUE;

	Game->SetAskAboutState(index, state);
}

BYTE CGameController::GetHintState(int index)
{
	return Game->GetHintState(index);
}

void CGameController::SetHintState(int index, BYTE state, int score)
{
	Game->SetHintState(index, state, score);
}

BYTE CGameController::GetHintCategoryState(int index)
{
	return Game->GetHintCategoryState(index);
}

void CGameController::SetHintCategoryState(int index, BYTE state)
{
	Game->SetHintCategoryState(index, state);
}

int CGameController::GetItemState(int item)
{
	return Game->GetItemState(item);
}

void CGameController::SetItemState(int item, int state)
{
	Game->SetItemState(item, state);
	ItemsChanged = TRUE;
}

void CGameController::SetFileName(int index, std::wstring name)
{
	FileMap[index] = name;
}

std::wstring CGameController::GetFileName(int index)
{
	return FileMap[index];
}

void CGameController::SetItemName(int index, std::wstring name)
{
	ItemMap[index] = name;
}

std::wstring CGameController::GetItemName(int index)
{
	return ItemMap[index];
}

void CGameController::SetAskAboutName(int index, std::wstring name)
{
	AskAboutMap[index] = name;
}

std::wstring CGameController::GetAskAboutName(int index)
{
	return AskAboutMap[index];
}

void CGameController::SetTimer(int timer, int duration)
{
	Game->SetTimer(timer, duration);
}

int CGameController::GetTimerState(int timer)
{
	return Game->GetTimerState(timer);
}

void CGameController::Tick(int ticks)
{
	Game->Tick(ticks);
}

void CGameController::ResetTimers()
{
	Game->ResetTimers();
}

void CGameController::SetSituationDescriptionL(int ix, std::wstring value)
{
	_lSituations[ix] = value;
}

std::wstring CGameController::GetSituationDescriptionL(int ix)
{
	if (_lSituations.find(ix) == _lSituations.end())
	{
		ix = -1;
	}

	if (_lSituations.find(ix) != _lSituations.end())
	{
		return _lSituations[ix];
	}

	return L"";
}

void CGameController::SetSituationDescriptionD(int ix, std::wstring value)
{
	_dSituations[ix] = value;
}

std::wstring CGameController::GetSituationDescriptionD(int ix)
{
	if (_dSituations.find(ix) == _dSituations.end())
	{
		ix = -1;
	}

	if (_dSituations.find(ix) != _dSituations.end())
	{
		return _dSituations[ix];
	}

	return L"";
}
