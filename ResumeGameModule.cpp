#include "ResumeGameModule.h"
#include "VideoModule.h"
#include "GameController.h"
#include "Utilities.h"
#include "MainMenuModule.h"

int CResumeGameModule::TextColour1 = 0;
int CResumeGameModule::TextColour2 = 0xffc30000;
int CResumeGameModule::TextColour3 = 0xffff1800;
int CResumeGameModule::TextColour4 = 0;

int CResumeGameModule::HeaderColour1 = 0;
int CResumeGameModule::HeaderColour2 = -1;
int CResumeGameModule::HeaderColour3 = -1;
int CResumeGameModule::HeaderColour4 = 0;

CResumeGameModule::CResumeGameModule() : CModuleBase(ModuleType::ResumeGame)
{
	_pScreen = NULL;
	_pFrame = NULL;
	_pLine1 = NULL;
	_pLine2 = NULL;
	_pBtnYes = NULL;
	_pBtnNo = NULL;
}

CResumeGameModule::~CResumeGameModule()
{
}

void CResumeGameModule::Render()
{
	dx.Clear(0.0f, 0.0f, 0.0f);

	_pFrame->Render();

	// Render cursor
	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
	CModuleController::Cursors[0].Render();

	dx.Present(1, 0);
}

void CResumeGameModule::Initialize()
{
	_cursorPosX = static_cast<int>(_cursorMaxX) / 2.0f;
	_cursorPosY = static_cast<int>(_cursorMaxY) / 2.0f;

	int w = dx.GetWidth();
	int h = dx.GetHeight();

	char* pY = "Yes";
	char* pN = "No";
	char* pL1 = "Do you wish to continue the";
	char* pL2 = "current game in progress?";
	char* pH = "GAME IN PROGRESS";
	float maxbtnw = max(TexFont.PixelWidth(pY), TexFont.PixelWidth(pN));
	float maxlabelw = max(TexFont.PixelWidth(pL1), TexFont.PixelWidth(pL2));
	float hw = max(TexFont.PixelWidth(pH), maxlabelw);
	float lineHeight = TexFont.Height() * pConfig->FontScale;
	float fw = hw + 16.0f * pConfig->FontScale;
	float fh = 8.5f * lineHeight;

	_pFrame = new CDXFrame(pH, fw, fh);

	_pLine1 = new CDXLabel(pL1, { 0,0,0,maxlabelw }, CDXText::Alignment::JustifyAlways);
	_pLine2 = new CDXLabel(pL2, { 0,0,0,maxlabelw }, CDXText::Alignment::JustifyAlways);
	_pLine1->SetColours(TextColour1, TextColour2, TextColour3, TextColour4);
	_pLine2->SetColours(TextColour1, TextColour2, TextColour3, TextColour4);

	float fx = (w - fw) / 2.0f;
	float fy = (h - fh) / 2.0f;

	_pFrame->AddChild(_pLine1, fx + 8.0f, fy + lineHeight * 2);
	_pFrame->AddChild(_pLine2, fx + 8.0f, fy + lineHeight * 3);
	_pBtnYes = _pFrame->AddButton(pY, fx + 8.0f, fy + lineHeight * 4.5f, maxbtnw, 20.0f, NULL);
	_pBtnNo = _pFrame->AddButton(pN, fx + fw - maxbtnw - 32.0f * pConfig->FontScale - 8.0f, fy + lineHeight * 4.5f, maxbtnw, 20.0f, NULL);
	_pFrame->SetPosition(fx, fy);
	_pFrame->SetColours(HeaderColour1, HeaderColour2, HeaderColour3, HeaderColour4);
}

void CResumeGameModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnYes->SetMouseOver(_pBtnYes->HitTest(x, y) != NULL);
	_pBtnNo->SetMouseOver(_pBtnNo->HitTest(x, y) != NULL);
}

void CResumeGameModule::BeginAction()
{
	if (_pBtnYes != NULL && _pBtnYes->HitTest(_cursorPosX, _cursorPosY) != NULL)
	{
		Yes();
	}
	else if (_pBtnNo != NULL && _pBtnNo->HitTest(_cursorPosX, _cursorPosY) != NULL)
	{
		No();
	}
}

void CResumeGameModule::Resize(int width, int height)
{
}

void CResumeGameModule::Dispose()
{
	// TODO: Delete and dispose of objects
}

void CResumeGameModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (key == 'Y')
	{
		Yes();
	}
	else if (key == 'N')
	{
		No();
	}
}

void CResumeGameModule::Yes()
{
	// Auto load
	CModuleController::Pop(this);
	CGameController::LoadGame(L"GAMES\\SAVEGAME.000");
	CMainMenuModule::MainMenuModule->EnableSaveAndResume(TRUE);
	CMainMenuModule::UpdateSaveGameData();
}

void CResumeGameModule::No()
{
	// Load title video
	CModuleController::Pop(this);
	CModuleController::Push(new CVideoModule(VideoType::Single, L"TITLE.AP", 0));
}

void CResumeGameModule::SetTextColours(int colour1, int colour2, int colour3, int colour4)
{
	TextColour1 = colour1;
	TextColour2 = colour2;
	TextColour3 = colour3;
	TextColour4 = colour4;
}

void CResumeGameModule::SetHeaderColours(int colour1, int colour2, int colour3, int colour4)
{
	HeaderColour1 = colour1;
	HeaderColour2 = colour2;
	HeaderColour3 = colour3;
	HeaderColour4 = colour4;
}
