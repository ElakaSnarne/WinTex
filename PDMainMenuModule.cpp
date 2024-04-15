#include "PDMainMenuModule.h"
#include "GameController.h"
#include "resource.h"
#include "Utilities.h"
#include "ResumeGameModule.h"
#include "PDGame.h"
#include <codecvt>
#include <algorithm>

CPDMainMenuModule::CPDMainMenuModule() : CMainMenuModule()
{
	_saveCursor.SetColours(0, 0, 0xffae0000, 0);
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

	// Buttons should be inside moon (need to find different reference point for pd)
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
	CDXButton* pLoadBtn = new CDXButton(pLG, maxw, 32.0f * pConfig->FontScale, Load);
	_pScreen->AddChild(pLoadBtn, btnLeft, btnMiddle);
	_btnMainSave = new CDXButton(pSG, maxw, 32.0f * pConfig->FontScale, Save);
	_btnMainSave->SetEnabled(FALSE);
	_pScreen->AddChild(_btnMainSave, btnLeft, btnBottom);

	CDXButton* pConfigBtn = _pScreen->AddButton(pCf, btnRight, btnTop, maxw, 32.0f * pConfig->FontScale, Config);
	CDXButton* pIntroBtn = _pScreen->AddButton(pIn, btnRight, btnMiddle, maxw, 32.0f * pConfig->FontScale, Intro);
	//pIntroBtn->SetEnabled(FALSE);
	CDXButton* pCreditsBtn = _pScreen->AddButton(pCr, btnRight, btnBottom, maxw, 32.0f * pConfig->FontScale, Credits);

	// Add resume (only visible when game is in progress) and quit (always visible, but ask if game in progress)
	_btnMainResume = _pScreen->AddButton(pRe, moonCenterX - (maxw + 32.0f * pConfig->FontScale) / 2.0f, btnTop - 64.0f * pConfig->FontScale, maxw, 32.0f * pConfig->FontScale, Resume);
	_btnMainResume->SetVisible(FALSE);

	_pScreen->AddButton(pQu, moonCenterX - (maxw + 32.0f * pConfig->FontScale) / 2.0f, btnBottom + 64.0f * pConfig->FontScale, maxw, 32.0f * pConfig->FontScale, Quit);

	_pScreen->SetColours(0, 0, -1, 0);
}

void CPDMainMenuModule::Render()
{
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
}

void CPDMainMenuModule::SetupConfigFrame()
{
	CMainMenuModule::SetupConfigFrame();

	// Update colours on config controls
	_pConfig->SetColours(0, 0, -1, 0);
}

void CPDMainMenuModule::SetupLoadFrame()
{
	CMainMenuModule::SetupLoadFrame();
}

void CPDMainMenuModule::SetupSaveFrame()
{
	CMainMenuModule::SetupSaveFrame();
}

void CPDMainMenuModule::SetupSave()
{
	SaveMode = SaveMode::Extension;
	SaveTypedChars = 3;

	memset(_commentBuffer, 0, 256);

	SaveGameInfo info;
	info.FileName = L"GAMES\\";
	auto nameLength = CurrentGameInfo.Player.length();
	while (nameLength > 0 && CurrentGameInfo.Player.at(nameLength - 1) == ' ')
	{
		nameLength--;
	}

	for (int i = 0; i < 6; i++)
	{
		info.FileName += (WCHAR)((i < nameLength) ? CurrentGameInfo.Player.at(i) : '_');
	}
	info.FileName += L"00.";
	// Append 3 digit number (from current savegame)
	auto lastDot = CurrentGameInfo.FileName.find_last_of('.');
	int fileIndex = lastDot > 0 ? min(999, std::stoi(CurrentGameInfo.FileName.c_str() + lastDot + 1)) : 1;
	if (fileIndex < 100)
	{
		info.FileName += L"0";
	}
	if (fileIndex < 10)
	{
		info.FileName += L"0";
	}
	info.FileName += std::to_wstring(fileIndex);

	info.Player = CurrentGameInfo.Player;
	std::wstring sit;
	if (CGameController::GetParameter(252) == 0)
	{
		// Dialogue
		sit = CGameController::GetSituationDescriptionD(CGameController::GetData(PD_SAVE_DMAP_ID) + 1);
	}
	else
	{
		// Location
		sit = CGameController::GetSituationDescriptionL(CGameController::GetData(PD_SAVE_LOCATION_ID) + 1);
	}
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	info.Location = conv.to_bytes(sit);
	info.DayInGame = "Day " + std::to_string(max(1, min(10, CGameController::GetData(PD_SAVE_PARAMETERS_DAY_IN_GAME))));
	SYSTEMTIME st;
	GetLocalTime(&st);
	info.DateTime = IntToString(st.wYear, 4) + "-" + IntToString(st.wMonth, 2) + +"-" + IntToString(st.wDay, 2) + " " + IntToString(st.wHour, 2) + ":" + IntToString(st.wMinute, 2) + ":" + IntToString(st.wSecond, 2);
	_saveControl->SetInfo(info);
	_saveControl->SetPDColours();
}

void CPDMainMenuModule::SetupLoad()
{
	// Get list of current save games
	_savedGames.clear();
	WIN32_FIND_DATA fd;
	HANDLE hFF = FindFirstFile(L"GAMES\\*.*", &fd);
	BYTE buffer[0xd8];
	if (hFF != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.nFileSizeLow > 0)
			{
				std::wstring name = std::wstring(fd.cFileName);
				if (name != L"SAVEGAME.000")
				{
					// Real file, load header and extract info
					CFile file;
					std::wstring fileName = L"GAMES\\" + name;
					if (file.Open((LPWSTR)fileName.c_str()))
					{
						if (file.Read(buffer, 0xd8) == 0xd8)
						{
							SaveGameInfo info;
							info.FileName = fileName;
							info.Player = std::string((const char*)(buffer + PD_SAVE_PLAYER), 10);
							//info.Location = std::string((const char*)(buffer + UAKM_SAVE_LOCATION), UAKM_SAVE_GAME_DAY - UAKM_SAVE_LOCATION);
							std::wstring sit;
							if (buffer[PD_SAVE_PARAMETERS + 252] == 0)
							{
								// Dialogue
								sit = CGameController::GetSituationDescriptionD(buffer[PD_SAVE_DMAP_ID] + 1);
							}
							else
							{
								// Location
								sit = CGameController::GetSituationDescriptionL(buffer[PD_SAVE_LOCATION_ID] + 1);
							}
							std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
							info.Location = conv.to_bytes(sit);

							info.DayInGame = std::string("Day ") + std::to_string(buffer[PD_SAVE_GAME_DAY]);
							info.DateTime = IntToString(buffer[PD_SAVE_YEAR] | (buffer[PD_SAVE_YEAR + 1] << 8), 4) + "-" + IntToString(buffer[PD_SAVE_MONTH], 2) + +"-" + IntToString(buffer[PD_SAVE_DAY], 2) + " " + IntToString(buffer[PD_SAVE_HOUR], 2) + ":" + IntToString(buffer[PD_SAVE_MINUTE], 2) + ":" + IntToString(buffer[PD_SAVE_SECOND], 2);
							info.Comment = std::string((const char*)(buffer + PD_SAVE_COMMENT), PD_SAVE_PADDING1 - PD_SAVE_COMMENT);

							_savedGames.push_back(info);
						}

						file.Close();
					}
				}
			}

		} while (FindNextFile(hFF, &fd));

		FindClose(hFF);
	}

	// Sort list (by last written or by save game date?)
	std::sort(_savedGames.begin(), _savedGames.end());

	LoadSetup();

	int ix = 0;
	while (ix < _saveGameControls.size())
	{
		CSaveGameControl* sgc = _saveGameControls.at(ix);
		sgc->SetPDColours();
		ix++;
	}

	_pScreen->ShowModal(_pLoad);
}
