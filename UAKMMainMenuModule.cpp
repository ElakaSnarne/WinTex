#include "UAKMMainMenuModule.h"
#include "GameController.h"
#include "DXImageButton.h"
#include "resource.h"
#include "Utilities.h"
#include "AmbientAudio.h"
#include "UAKMGame.h"
#include <codecvt>
#include <algorithm>

CUAKMMainMenuModule::CUAKMMainMenuModule()
{
}

CUAKMMainMenuModule::~CUAKMMainMenuModule()
{
}

void CUAKMMainMenuModule::Intro(LPVOID data)
{
	pMIDI->Stop();
	CAmbientAudio::StopAll();
	CAmbientAudio::Clear();
	CGameController::LoadFromDMap(29);
}

void CUAKMMainMenuModule::Credits(LPVOID data)
{
	pMIDI->Stop();
	CAmbientAudio::StopAll();
	CAmbientAudio::Clear();
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

void CUAKMMainMenuModule::SetupSave()
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
	if (CGameController::GetData(UAKM_SAVE_DMAP_FLAG) == 0)
	{
		// Location
		sit = CGameController::GetSituationDescriptionL(CGameController::GetData(UAKM_SAVE_MAP_ENTRY));
	}
	else
	{
		// Dialogue
		sit = CGameController::GetSituationDescriptionD(CGameController::GetData(UAKM_SAVE_DMAP_ENTRY));
	}
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	info.Location = conv.to_bytes(sit);
	info.DayInGame = "Day " + std::to_string(max(1, min(7, CGameController::GetData(UAKM_SAVE_GAME_DAY))));
	SYSTEMTIME st;
	GetLocalTime(&st);
	info.DateTime = IntToString(st.wYear, 4) + "-" + IntToString(st.wMonth, 2) + +"-" + IntToString(st.wDay, 2) + " " + IntToString(st.wHour, 2) + ":" + IntToString(st.wMinute, 2) + ":" + IntToString(st.wSecond, 2);
	_saveControl->SetInfo(info);
}

void CUAKMMainMenuModule::SetupLoad()
{
	// Get list of current save games
	_savedGames.clear();
	WIN32_FIND_DATA fd;
	HANDLE hFF = FindFirstFile(L"GAMES\\*.*", &fd);
	BYTE buffer[0xd1];
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
						if (file.Read(buffer, 0xd0) == 0xd0)
						{
							SaveGameInfo info;
							info.FileName = fileName;
							info.Player = std::string((const char*)(buffer + UAKM_SAVE_PLAYER), UAKM_SAVE_LOCATION - UAKM_SAVE_PLAYER);
							info.Location = std::string((const char*)(buffer + UAKM_SAVE_LOCATION), UAKM_SAVE_GAME_DAY - UAKM_SAVE_LOCATION);
							info.DayInGame = std::string("Day ") + std::to_string(buffer[UAKM_SAVE_GAME_DAY]);
							info.DateTime = IntToString(buffer[UAKM_SAVE_YEAR] | (buffer[UAKM_SAVE_YEAR + 1] << 8), 4) + "-" + IntToString(buffer[UAKM_SAVE_MONTH], 2) + +"-" + IntToString(buffer[UAKM_SAVE_DAY], 2) + " " + IntToString(buffer[UAKM_SAVE_HOUR], 2) + ":" + IntToString(buffer[UAKM_SAVE_MINUTE], 2) + ":" + IntToString(buffer[UAKM_SAVE_SECOND], 2);
							info.Comment = std::string((const char*)(buffer + UAKM_SAVE_COMMENT), UAKM_SAVE_PADDING1 - UAKM_SAVE_COMMENT);

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

	_pScreen->ShowModal(_pLoad);
}
