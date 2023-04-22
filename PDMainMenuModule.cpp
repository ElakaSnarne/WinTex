#include "PDMainMenuModule.h"
#include "GameController.h"
#include "DXImageButton.h"
#include "resource.h"
#include "Utilities.h"

CPDMainMenuModule::CPDMainMenuModule()
{
}

CPDMainMenuModule::~CPDMainMenuModule()
{
}

void CPDMainMenuModule::Intro(LPVOID data)
{
	pMIDI->Stop();
	CGameController::LoadFromDMap(30);
}

void CPDMainMenuModule::Credits(LPVOID data)
{
	pMIDI->Stop();
	CGameController::LoadFromDMap(57);
}

void CPDMainMenuModule::SetupScreen()
{
	DWORD s;
	PBYTE pImg = GetResource(IDB_PD_TITLE, L"JPG", &s);
	CDXBitmap* pBmp = _pScreen->AddBitmap(pImg, s, Alignment::CenterX | Alignment::CenterY | Alignment::Scale);

	// TODO: Use string resources, or read from configuration file
	char* pNG = "New game";
	char* pLG = "Load";
	char* pSG = "Save";
	char* pCf = "Config";
	char* pIn = "Intro";
	char* pCr = "Credits";
	char* pRe = "Resume";
	char* pQu = "Quit";

	// Should calculate sizes of buttons, so all can be same size and properly aligned...
	float maxw = max(max(max(max(max(max(max(TexFont.PixelWidth(pNG), TexFont.PixelWidth(pLG)), TexFont.PixelWidth(pSG)), TexFont.PixelWidth(pCf)), TexFont.PixelWidth(pIn)), TexFont.PixelWidth(pCr)), TexFont.PixelWidth(pRe)), TexFont.PixelWidth(pQu));

	float iw = pBmp->GetWidth();
	float ih = pBmp->GetHeight();
	float w = static_cast<float>(dx.GetWidth());
	float h = static_cast<float>(dx.GetHeight());

	// Center image
	float imageTop = (h - ih) / 2;
	float imageLeft = (w - iw) / 2;
	pBmp->SetPosition(imageLeft, imageTop);

	// Buttons should be inside moon
	// Moon covers about 67% of the width and 70% of the height
	float moonW = iw * 0.67f;
	float moonH = ih * 0.64f;

	float moonCenterX = w - imageLeft - moonW / 2.0f;
	float moonCenterY = imageTop + moonH / 2.0f;

	float btnRight = moonCenterX + 32.0f * pConfig->FontScale;
	float btnLeft = btnRight - maxw - 96.0f * pConfig->FontScale;
	float btnMiddle = moonCenterY;
	float btnTop = btnMiddle - 64.0f * pConfig->FontScale;
	float btnBottom = btnMiddle + 64.0f * pConfig->FontScale;

	CDXButton* pNewGameBtn = _pScreen->AddButton(pNG, btnLeft, btnTop, maxw, 32.0f * pConfig->FontScale, NewGame);
	//pNewGameBtn->SetEnabled(FALSE);
	CDXButton* pLoadBtn = new CDXButton(pLG, maxw, 32.0f * pConfig->FontScale, Load);
	pLoadBtn->SetEnabled(FALSE);
	_pScreen->AddChild(pLoadBtn, btnLeft, btnMiddle);
	_btnMainSave = new CDXButton(pSG, maxw, 32.0f * pConfig->FontScale, Save);
	_btnMainSave->SetEnabled(FALSE);
	_pScreen->AddChild(_btnMainSave, btnLeft, btnBottom);

	CDXButton* pConfigBtn = _pScreen->AddButton(pCf, btnRight, btnTop, maxw, 32.0f * pConfig->FontScale, Config);
	CDXButton* pIntroBtn = _pScreen->AddButton(pIn, btnRight, btnMiddle, maxw, 32.0f * pConfig->FontScale, Intro);
	//pIntroBtn->SetEnabled(FALSE);
	CDXButton* pCreditsBtn = _pScreen->AddButton(pCr, btnRight, btnBottom, maxw, 32.0f * pConfig->FontScale, Credits);
	//pCreditsBtn->SetEnabled(FALSE);

	// Add resume (only visible when game is in progress) and quit (always visible, but ask if game in progress)
	_btnMainResume = _pScreen->AddButton(pRe, moonCenterX - (maxw + 32.0f * pConfig->FontScale) / 2.0f, btnTop - 64.0f * pConfig->FontScale, maxw, 32.0f * pConfig->FontScale, Resume);
	_btnMainResume->SetVisible(FALSE);

	_pScreen->AddButton(pQu, moonCenterX - (maxw + 32.0f * pConfig->FontScale) / 2.0f, btnBottom + 64.0f * pConfig->FontScale, maxw, 32.0f * pConfig->FontScale, Quit);
}

void CPDMainMenuModule::Render()
{
	dx.Clear(0.0f, 0.0f, 0.0f);

	CDXFont::SelectBlackFont();
	_pScreen->Render();

	if (SaveMode != SaveMode::Load && (GetTickCount64() / 500) % 2)
	{
		if (SaveMode == SaveMode::Extension)
		{
			// Show cursor at filename extension
			SaveGameInfo info = _saveControl->GetInfo();
			char buffer[32];
			memset(buffer, 0, 32);
			for (int i = 0; i < 32 && i < info.FileName.length(); i++)
			{
				buffer[i] = static_cast<char>(info.FileName.at(i));
			}
			float x = _saveControl->GetX() + 21 * pConfig->FontScale + ceil(TexFont.PixelWidth(buffer));
			float y = _saveControl->GetY() + 8 * pConfig->FontScale;
			_saveCursor.Render(x, y);
		}
		else if (SaveMode == SaveMode::Comment)
		{
			// Show cursor at comment
			//SaveGameInfo info = _saveControl->GetInfo();
			float y = _saveControl->GetY() + 68 * pConfig->FontScale;
			_saveCursor.Render(static_cast<float>(_caretPos), y);
		}
	}

	// Render cursor
	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
	CModuleController::Cursors[0].Render();

	dx.Present(1, 0);
}
