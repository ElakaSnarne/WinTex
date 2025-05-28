#include "PDSelectLevelModule.h"
#include "ModuleController.h"
#include "PDGame.h"
#include "GameController.h"
#include "MainMenuModule.h"

CPDSelectLevelModule::CPDSelectLevelModule(BYTE* pGameData) : CModuleBase(ModuleType::NewGame)
{
	_pCBEntertainment = NULL;
	_pCBGamePlayer = NULL;

	_pBtnOK = NULL;
	_pBtnCancel = NULL;

	_gameData = pGameData;

	_bEntertainment = TRUE;
	_bGamePlayer = FALSE;
}

CPDSelectLevelModule::~CPDSelectLevelModule()
{
	Dispose();
}

void CPDSelectLevelModule::Dispose()
{
	if (_pCBEntertainment != NULL)
	{
		delete _pCBEntertainment;
		_pCBEntertainment = NULL;
	}

	if (_pCBGamePlayer != NULL)
	{
		delete _pCBGamePlayer;
		_pCBGamePlayer = NULL;
	}

	if (_pBtnOK != NULL)
	{
		delete _pBtnOK;
		_pBtnOK = NULL;
	}

	if (_pBtnCancel != NULL)
	{
		delete _pBtnCancel;
		_pBtnCancel = NULL;
	}
}

void CPDSelectLevelModule::Initialize()
{
	Rect r1{ 0, 0, dx.GetHeight(), dx.GetWidth() };
	_line1.SetColours(0xffffffff, 0, 0);
	_line1.SetTextPD(L"Play level selection", r1);

	Rect r2{ 0, 10, dx.GetHeight() - 100, dx.GetWidth() - 10 };
	_line2.SetColours(0xff26ff00, 0xff0096ff, 0xffffffff);
	_line2.SetTextPD2(L"^.Do you wish to play the ^-Entertainment Level^. or the ^-Game Players Level^.?\n\nWe recommend that everyone except experienced game players select the ^-Entertainment Level^. the first time through.  There are hints available on this level as well as an option to bypass the more difficult puzzles (refer to the Hint System.)\n\nThe ^-Game Players Level^. is very challenging and should be selected only by experienced game players, or by players who have already gone through the ^-Entertainment Level^..  There are no hints available, but instead of ^?1500^. possible points, there are ^?4000^..  In addition, there are bonus locations and puzzles.\n\nNote:  Both levels have three narrative paths through the story leading to a total of seven combined endings.", r2);

	Rect r3{ 0, 0, dx.GetHeight() - 100, dx.GetWidth() };
	_line3.SetColours(0xffffffff, 0, 0);
	_line3.SetTextPD(L"Play level", r3);

	_pCBEntertainment = new CDXCheckBox("Entertainment", &_bEntertainment, 0.0f);
	_pCBEntertainment->SetColours(0, 0, -1, 0);
	_pCBEntertainment->SetPosition(0, dx.GetHeight() / 2);
	_pCBGamePlayer = new CDXCheckBox("Game player", &_bGamePlayer, 0.0f);
	_pCBGamePlayer->SetColours(0, 0, -1, 0);
	_pCBGamePlayer->SetPosition(dx.GetWidth() - _pCBGamePlayer->GetWidth() - 10, dx.GetHeight() / 2);

	_pBtnOK = new CDXButton("OK", 80, 20);
	_pBtnOK->SetPosition(10, dx.GetHeight() - _pBtnOK->GetHeight() - 10);
	_pBtnCancel = new CDXButton("Cancel", 80, 20);
	_pBtnCancel->SetPosition(dx.GetWidth() - _pBtnCancel->GetWidth() - 10, dx.GetHeight() - _pBtnCancel->GetHeight() - 10);

	POINT pt;
	GetCursorPos(&pt);
	::ScreenToClient(_hWnd, &pt);
	_cursorPosX = pt.x;
	_cursorPosY = pt.y;
}

void CPDSelectLevelModule::Resize(int width, int height)
{
}

void CPDSelectLevelModule::Render()
{
	_line1.Render((dx.GetWidth() - _line1.Width()) / 2.0f, 50);
	_line2.Render(0, 100);
	_line3.Render((dx.GetWidth() - _line3.Width()) / 2.0f, 370);

	_pCBEntertainment->Render();
	_pCBGamePlayer->Render();

	_pBtnOK->Render();
	_pBtnCancel->Render();

	// Render cursor
	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
	CModuleController::Cursors[0].Render();
}

void CPDSelectLevelModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (key == VK_ESCAPE)
	{
		return Back();
	}
}

void CPDSelectLevelModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnOK->SetMouseOver(_pBtnOK->HitTest(x, y) != NULL);
	_pBtnCancel->SetMouseOver(_pBtnCancel->HitTest(x, y) != NULL);
	_pCBEntertainment->SetMouseOver(_pCBEntertainment->HitTest(x, y) != NULL);
	_pCBGamePlayer->SetMouseOver(_pCBGamePlayer->HitTest(x, y) != NULL);
}

void CPDSelectLevelModule::BeginAction()
{
	// Check checkboxes and buttons
	if (_pCBEntertainment->HitTest(_cursorPosX, _cursorPosY))
	{
		_pCBEntertainment->SetCheck(TRUE);
		_pCBGamePlayer->SetCheck(FALSE);
	}
	else if (_pCBGamePlayer->HitTest(_cursorPosX, _cursorPosY))
	{
		_pCBGamePlayer->SetCheck(TRUE);
		_pCBEntertainment->SetCheck(FALSE);
	}
	else if (_pBtnOK->HitTest(_cursorPosX, _cursorPosY))
	{
		NewGame();
	}
	else if (_pBtnCancel->HitTest(_cursorPosX, _cursorPosY))
	{
		Back();
	}
}

void CPDSelectLevelModule::Back()
{
	CModuleController::Pop(this);
}

void CPDSelectLevelModule::NewGame()
{
	CMainMenuModule::SetPlayerNameAndEnableButtons();

	// Reset game buffer
	ZeroMemory(_gameData, PD_SAVE_SIZE);

	_gameData[PD_SAVE_UNKNOWN1] = 6;
	CGameController::SetData(PD_SAVE_PLAYER, "TEX");

	CGameController::SetData(PD_SAVE_TRAVEL + 1, 1);	// Tex' Office
	CGameController::SetData(PD_SAVE_TRAVEL + 70, 1);	// Tex' Bedroom
	CGameController::SetData(PD_SAVE_TRAVEL + 71, 1);	// Tex' Computer Room

	CGameController::SetAskAboutState(0, 1);			// Enable AskAbout Tex Murphy
	CGameController::SetAskAboutState(1, 1);			// Enable AskAbout Chelsee Bando
	CGameController::SetAskAboutState(2, 1);			// Enable AskAbout Louie LaMintz
	CGameController::SetAskAboutState(3, 1);			// Enable AskAbout Rook Garner
	CGameController::SetAskAboutState(6, 1);			// Enable AskAbout Gordon Fitzpatrick
	CGameController::SetAskAboutState(7, 1);			// Enable AskAbout Thomas Malloy
	CGameController::SetAskAboutState(9, 1);			// Enable AskAbout Nilo
	CGameController::SetAskAboutState(40, 1);			// Enable AskAbout Tyson Matthews
	CGameController::SetAskAboutState(56, 1);			// Enable AskAbout Newspaper photo of Malloy
	CGameController::SetAskAboutState(61, 1);			// Enable AskAbout Sandra

	// Set Unknowns 10 and 11 to 2 and 1
	_gameData[PD_SAVE_PARAMETERS + 251] = 2;
	_gameData[PD_SAVE_PARAMETERS + 250] = 1;			// Day in game

	// Parameter 0x19a, 0 = Entertainment Level, 1 = Game Player Level
	_gameData[PD_SAVE_PARAMETERS_GAME_LEVEL] = _bGamePlayer;

	CGameController::SetWord(PD_SAVE_CASH, 4000);
	CGameController::SetItemState(0, 1);				// Cash
	CGameController::SetItemState(1, 1);				// Photo of Malloy
	CGameController::SetItemState(2, 1);				// Credit card
	CGameController::SetItemState(4, 1);				// Fitzpatrick's card

	// Set hint category states 1-4
	CGameController::SetHintCategoryState(1, 1);
	CGameController::SetHintCategoryState(2, 1);
	CGameController::SetHintCategoryState(3, 1);
	CGameController::SetHintCategoryState(4, 1);

	// TODO: Clear some tables (hints?)

	CGameController::LoadFromDMap(42);
}
