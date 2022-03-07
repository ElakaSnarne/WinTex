#include "UAKMMainMenuModule.h"
#include "GameController.h"
#include "DXImageButton.h"
#include "resource.h"
#include "Utilities.h"

CUAKMMainMenuModule::CUAKMMainMenuModule()
{
}

CUAKMMainMenuModule::~CUAKMMainMenuModule()
{
}

void CUAKMMainMenuModule::Intro(LPVOID data)
{
	pMIDI->Stop();
	CGameController::LoadFromDMap(29);
}

void CUAKMMainMenuModule::Credits(LPVOID data)
{
	pMIDI->Stop();
	CGameController::LoadFromDMap(41);
}

void CUAKMMainMenuModule::SetupScreen()
{
	DWORD s;
	PBYTE pImg = GetResource(IDB_UAKM_TITLE, L"JPG", &s);
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
	float w = dx.GetWidth();
	float h = dx.GetHeight();

	// Center image
	float imageTop = (h - ih) / 2;
	float imageLeft = (w - iw) / 2;
	pBmp->SetPosition(imageLeft, imageTop);

	// Buttons should be inside moon
	// Moon covers about 67% of the width and 70% of the height
	float moonW = iw * 0.67;
	float moonH = ih * 0.64;

	float moonCenterX = w - imageLeft - moonW / 2.0f;
	float moonCenterY = imageTop + moonH / 2.0f;

	float btnRight = moonCenterX + 32.0f * pConfig->FontScale;
	float btnLeft = btnRight - maxw - 96.0f * pConfig->FontScale;
	float btnMiddle = moonCenterY;
	float btnTop = btnMiddle - 64.0f * pConfig->FontScale;
	float btnBottom = btnMiddle + 64.0f * pConfig->FontScale;

	_pScreen->AddButton(pNG, btnLeft, btnTop, maxw, 32.0f * pConfig->FontScale, NewGame);
	CDXButton* pLoadBtn = new CDXButton(pLG, maxw, 32.0f * pConfig->FontScale, Load);
	_pScreen->AddChild(pLoadBtn, btnLeft, btnMiddle);
	_btnMainSave = new CDXButton(pSG, maxw, 32.0f * pConfig->FontScale, Save);
	_btnMainSave->SetEnabled(FALSE);
	_pScreen->AddChild(_btnMainSave, btnLeft, btnBottom);

	_pScreen->AddButton(pCf, btnRight, btnTop, maxw, 32.0f * pConfig->FontScale, Config);
	_pScreen->AddButton(pIn, btnRight, btnMiddle, maxw, 32.0f * pConfig->FontScale, Intro);
	_pScreen->AddButton(pCr, btnRight, btnBottom, maxw, 32.0f * pConfig->FontScale, Credits);

	// Add resume (only visible when game is in progress) and quit (always visible, but ask if game in progress)
	_btnMainResume = _pScreen->AddButton(pRe, moonCenterX - (maxw + 32.0f * pConfig->FontScale) / 2.0f, btnTop - 64.0f * pConfig->FontScale, maxw, 32.0f * pConfig->FontScale, Resume);
	_btnMainResume->SetVisible(FALSE);

	_pScreen->AddButton(pQu, moonCenterX - (maxw + 32.0f * pConfig->FontScale) / 2.0f, btnBottom + 64.0f * pConfig->FontScale, maxw, 32.0f * pConfig->FontScale, Quit);
}
