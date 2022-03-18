#include "MainMenuModule.h"
#include "DXImageButton.h"
#include "resource.h"
#include "Utilities.h"
#include "GameController.h"
#include "UAKMGame.h"
#include "SaveGameControl.h"
#include <algorithm>
#include <codecvt>
#include "AmbientAudio.h"
#include "DXControlButton.h"
#include "Gamepad.h"

int CMainMenuModule::cfgWidth = 0;
int CMainMenuModule::cfgHeight = 0;
int CMainMenuModule::cfgMode = 0;
int CMainMenuModule::cfgMIDIDevice = 0;
BOOL CMainMenuModule::cfgFullscreen = FALSE;
BOOL CMainMenuModule::cfgCaptions = TRUE;
BOOL CMainMenuModule::cfgAlternativeMedia = FALSE;
BOOL CMainMenuModule::cfgPlayMIDI = FALSE;
BOOL CMainMenuModule::cfgInvertY = FALSE;
float CMainMenuModule::cfgMouselookScaling = 1.0f;
float CMainMenuModule::cfgFontScale = 1.0f;
BOOL CMainMenuModule::cfgAnisotropicFiltering = TRUE;
float CMainMenuModule::cfgVolume = 50.0f;
float CMainMenuModule::cfgMIDIVolume = 50.0f;

CDXScreen* CMainMenuModule::_pScreen = NULL;
CDXFrame* CMainMenuModule::_pLoad = NULL;
CDXFrame* CMainMenuModule::_pSave = NULL;
CDXLabel* CMainMenuModule::pResolution = NULL;
CDXLabel* CMainMenuModule::pMIDIDevice = NULL;

CDXLabel* CMainMenuModule::pMIDIVolume = NULL;
CDXLabel* CMainMenuModule::pVolume = NULL;
CDXLabel* CMainMenuModule::pFontScale = NULL;

CDXSlider* CMainMenuModule::pMIDIVolumeSlider = NULL;
CDXSlider* CMainMenuModule::pVolumeSlider = NULL;
CDXSlider* CMainMenuModule::pFontScaleSlider = NULL;

CDXFrame* CMainMenuModule::_pConfig = NULL;
CDXTabControl* CMainMenuModule::_pConfigTab = NULL;
CDXTabItem* CMainMenuModule::_pConfigVideo = NULL;
CDXTabItem* CMainMenuModule::_pConfigAudio = NULL;
CDXTabItem* CMainMenuModule::_pConfigControl = NULL;
CDXTabItem* CMainMenuModule::_pConfigMisc = NULL;
CDXTabControl* CMainMenuModule::_pConfigControlTab = NULL;
CDXTabItem* CMainMenuModule::_pConfigControlKeyMouse = NULL;
CDXTabItem* CMainMenuModule::_pConfigControlJoystick = NULL;

CDXButton* CMainMenuModule::_btnMainResume = NULL;
CDXButton* CMainMenuModule::_btnMainSave = NULL;
CMainMenuModule* CMainMenuModule::MainMenuModule = NULL;

CDXButton* CMainMenuModule::_pConfigCancelBtn = NULL;
CDXButton* CMainMenuModule::_pConfigAcceptBtn = NULL;

CDXListBox CMainMenuModule::_loadListBox;
std::vector<SaveGameInfo> CMainMenuModule::_savedGames;
std::vector<CSaveGameControl*> CMainMenuModule::_saveGameControls;
int CMainMenuModule::_loadTopIndex = 0;
int CMainMenuModule::_loadVisibleSavesCount = 0;

SaveGameInfo CMainMenuModule::CurrentGameInfo;

CSaveGameControl* CMainMenuModule::_saveControl = NULL;
SaveMode CMainMenuModule::SaveMode = SaveMode::Load;
int CMainMenuModule::SaveTypedChars = 0;
CDXText CMainMenuModule::_saveCursor;
char CMainMenuModule::_commentBuffer[256];
int CMainMenuModule::_caretPos = 0;

BOOL CMainMenuModule::ConfiguringControl = FALSE;
InputAction CMainMenuModule::ControlInputAction = InputAction::Cursor;
CDXButton* CMainMenuModule::_pCancelConfigControlBtn = NULL;
std::unordered_map<InputAction, InputMap> CMainMenuModule::_controlMapping;
CDXControlButton* CMainMenuModule::_pConfiguredControl = NULL;

std::unordered_map<InputAction, CDXControlButton*> CMainMenuModule::_mouseKeyControls;
std::unordered_map<InputAction, CDXControlButton*> CMainMenuModule::_joystickControls;

CDXSlider* CMainMenuModule::_pSliderDragging = NULL;

CMainMenuModule::CMainMenuModule() : CModuleBase(ModuleType::MainMenu)
{
	_pScreen = new CDXScreen();
	_pScreen->SetSize(static_cast<float>(dx.GetWidth()), static_cast<float>(dx.GetHeight()));

	MainMenuModule = this;

	_saveCursor.SetText(L"_");
	_saveCursor.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);
}

CMainMenuModule::~CMainMenuModule()
{
	Clear();
}

void CMainMenuModule::Initialize()
{
	_cursorPosX = _cursorMaxX / 2.0f;
	_cursorPosY = _cursorMaxY / 2.0f;

	SetupScreen();
}

void CMainMenuModule::ConfigCancel(LPVOID data)
{
	_pScreen->PopModal();

	// Revert config changes
	cfgWidth = pConfig->Width;
	cfgHeight = pConfig->Height;
	cfgMode = pConfig->ScreenMode;
	cfgFullscreen = pConfig->FullScreen;
	cfgCaptions = pConfig->Captions;
	cfgAlternativeMedia = pConfig->AlternativeMedia;
	cfgPlayMIDI = pConfig->PlayMIDI;
	cfgInvertY = pConfig->InvertY;
	cfgMouselookScaling = pConfig->MouselookScaling;
	cfgMIDIDevice = pConfig->MIDIDeviceId;
	cfgFontScale = pConfig->FontScale;
	cfgVolume = static_cast<float>(pConfig->Volume);
	cfgMIDIVolume = static_cast<float>(pConfig->MIDIVolume);
	cfgAnisotropicFiltering = pConfig->AnisotropicFilter;

	pMIDIVolumeSlider->CalculateSliderPosition();
	pMIDIVolumeSlider->UpdateValueText();
	pVolumeSlider->CalculateSliderPosition();
	pVolumeSlider->UpdateValueText();
	pFontScaleSlider->CalculateSliderPosition();
	pFontScaleSlider->UpdateValueText();

	// TODO: Reload all previous control configuration
	_controlMapping = CInputMapping::ControlsMap;

	for (auto control : _mouseKeyControls)
	{
		control.second->UpdateControlText(&_controlMapping, control.first);
	}
}

void CMainMenuModule::ConfigAccept(LPVOID data)
{
	_pScreen->PopModal();

	// Set values back on configuration, change resolution if neccessary

	BOOL change_size = (pConfig->Width != cfgWidth) || (pConfig->Height != cfgHeight);
	BOOL change_fs = (pConfig->FullScreen != cfgFullscreen);
	BOOL changeMIDIDevice = (pConfig->PlayMIDI && pConfig->MIDIDeviceId != cfgMIDIDevice);
	BOOL changeFilter = (pConfig->AnisotropicFilter != cfgAnisotropicFiltering);

	pConfig->Width = cfgWidth;
	pConfig->Height = cfgHeight;
	pConfig->ScreenMode = cfgMode;
	pConfig->FullScreen = cfgFullscreen;
	pConfig->Captions = cfgCaptions;
	pConfig->AlternativeMedia = cfgAlternativeMedia;
	pConfig->PlayMIDI = cfgPlayMIDI;
	pConfig->InvertY = cfgInvertY;
	pConfig->MouselookScaling = cfgMouselookScaling;
	pConfig->MIDIDeviceId = cfgMIDIDevice;
	pConfig->FontScale = cfgFontScale;
	pConfig->Volume = static_cast<int>(cfgVolume);
	pConfig->MIDIVolume = static_cast<int>(cfgMIDIVolume);
	pConfig->AnisotropicFilter = cfgAnisotropicFiltering;
	pConfig->Save();

	if (change_fs)
	{
		dx.SetFullScreen(cfgFullscreen);
	}

	if (change_size)
	{
		dx.Resize(cfgWidth, cfgHeight);

		CModuleController::Resize(cfgWidth, cfgHeight);

		CConstantBuffers::Setup2D(dx);
	}

	if (changeFilter)
	{
		dx.SelectSampler(cfgAnisotropicFiltering);
	}

	if (changeMIDIDevice)
	{
		pMIDI->OpenDevice(cfgMIDIDevice);
	}

	pMIDI->SetVolume(((float)cfgMIDIVolume) / 100.0f);
	CDXSound::SetVolume(((float)cfgVolume) / 100.0f);

	// Apply and save controls
	CInputMapping::ControlsMap = _controlMapping;

	CInputMapping::SaveControlsMap();
}

void CMainMenuModule::ConfigPreviousResolution(LPVOID data)
{
	if (cfgMode > pConfig->MinAcceptedMode&& pConfig->pAdapter != NULL)
	{
		cfgMode--;
		cfgWidth = pConfig->pAdapter->_displayModeList[cfgMode].Width;
		cfgHeight = pConfig->pAdapter->_displayModeList[cfgMode].Height;

		UpdateResolutionLabel();
	}
}

void CMainMenuModule::ConfigNextResolution(LPVOID data)
{
	if (pConfig->pAdapter != NULL && cfgMode < (static_cast<int>(pConfig->pAdapter->_numModes) - 1))
	{
		cfgMode++;
		cfgWidth = pConfig->pAdapter->_displayModeList[cfgMode].Width;
		cfgHeight = pConfig->pAdapter->_displayModeList[cfgMode].Height;

		UpdateResolutionLabel();
	}
}

void CMainMenuModule::UpdateResolutionLabel()
{
	char buffer[40];
	char* pp = buffer;
	itoa(cfgWidth, pp, 10);
	pp += strlen(pp);
	strcat(pp, " x ");
	pp += 3;
	itoa(cfgHeight, pp, 10);

	pResolution->SetText(buffer);
}

void CMainMenuModule::ConfigPreviousMIDIDevice(LPVOID data)
{
	if (pConfig->pMIDIDevices != NULL && cfgMIDIDevice > 0)
	{
		cfgMIDIDevice--;
		UpdateMIDIDeviceLabel();
	}
}

void CMainMenuModule::ConfigNextMIDIDevice(LPVOID data)
{
	if (pConfig->pMIDIDevices != NULL && cfgMIDIDevice < (pConfig->NumberOfMIDIOutDevices - 1))
	{
		cfgMIDIDevice++;
		UpdateMIDIDeviceLabel();
	}
}

void CMainMenuModule::UpdateMIDIDeviceLabel()
{
	MIDIOUTCAPSA midiCaps = pConfig->pMIDIDevices[cfgMIDIDevice];
	pMIDIDevice->SetText(midiCaps.szPname);
}

void CMainMenuModule::NewGame(LPVOID data)
{
	pMIDI->Stop();
	CurrentGameInfo.Player = "TEX";
	CurrentGameInfo.FileName = L"GAMES\\TEX___00.000";
	EnableSaveAndResume(TRUE);
	CGameController::NewGame();
}

void CMainMenuModule::Resume(LPVOID data)
{
	CModuleController::SendToBack(MainMenuModule);
}

std::string IntToString(int value, int size)
{
	char buffer[10];
	char format[20];
	sprintf(format, "%%0%ii", size);
	sprintf(buffer, format, value);
	return buffer;
}

bool CompareSaveGames(const SaveGameInfo& first, const SaveGameInfo& second)
{
	unsigned int i = 0;
	while ((i < first.DateTime.length()) && (i < second.DateTime.length()))
	{
		if (tolower(first.DateTime[i]) < tolower(second.DateTime[i])) return false;
		else if (tolower(first.DateTime[i]) > tolower(second.DateTime[i])) return true;
		++i;
	}

	return (first.DateTime.length() > second.DateTime.length());
}

void CMainMenuModule::Load(LPVOID data)
{
	if (_pLoad == NULL)
	{
		((CMainMenuModule*)MainMenuModule)->SetupLoadFrame();
	}

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

void CMainMenuModule::Save(LPVOID data)
{
	// TODO: Ask for file name
	//CGameController::SaveGame(L"games\\savegame.999");

	if (_pSave == NULL)
	{
		((CMainMenuModule*)MainMenuModule)->SetupSaveFrame();
	}

	SaveSetup();

	_pScreen->ShowModal(_pSave);
}

void CMainMenuModule::Config(LPVOID data)
{
	cfgWidth = pConfig->Width;
	cfgHeight = pConfig->Height;
	cfgMode = pConfig->ScreenMode;
	cfgFullscreen = pConfig->FullScreen;
	cfgCaptions = pConfig->Captions;
	cfgAlternativeMedia = pConfig->AlternativeMedia;
	cfgPlayMIDI = pConfig->PlayMIDI;
	cfgInvertY = pConfig->InvertY;
	cfgMouselookScaling = pConfig->MouselookScaling;
	cfgFontScale = pConfig->FontScale;
	cfgAnisotropicFiltering = pConfig->AnisotropicFilter;
	cfgVolume = static_cast<float>(pConfig->Volume);
	cfgMIDIVolume = static_cast<float>(pConfig->MIDIVolume);

	if (_pConfig == NULL)
	{
		((CMainMenuModule*)MainMenuModule)->SetupConfigFrame();
	}

	UpdateResolutionLabel();
	UpdateMIDIDeviceLabel();

	_pScreen->ShowModal(_pConfig);
}

void CMainMenuModule::Quit(LPVOID data)
{
	// TODO: Check if game is in progress, query if yes
	PostThreadMessage(CModuleController::MainThreadId, WM_CLOSE, 0, 0);
}

void CMainMenuModule::Render()
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
				buffer[i] = info.FileName.at(i) & 0xFF;
			}
			float x = _saveControl->GetX() + 21 * pConfig->FontScale + ceil(TexFont.PixelWidth(buffer));
			float y = _saveControl->GetY() + 8 * pConfig->FontScale;
			_saveCursor.Render(x, y);
		}
		else if (SaveMode == SaveMode::Comment)
		{
			// Show cursor at comment
			float y = _saveControl->GetY() + 68 * pConfig->FontScale;
			_saveCursor.Render(static_cast<float>(_caretPos), y);
		}
	}

	// Render cursor
	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
	CModuleController::Cursors[0].Render();

	dx.Present(1, 0);
}

void CMainMenuModule::Resize(int width, int height)
{
	// Is it easier to destroy the old screen and recreate???
	BOOL saveEnabled = _btnMainSave->GetEnabled();
	BOOL resumeVisible = _btnMainResume->GetVisible();

	Clear();

	if (!cfgFullscreen)
	{
		int sw = GetSystemMetrics(SM_CXSCREEN);
		int sh = GetSystemMetrics(SM_CYSCREEN);
		RECT wr = { 0, 0, width, height };
		AdjustWindowRect(&wr, WS_CAPTION, FALSE);
		MoveWindow(::_hWnd, (sw - width) / 2 + wr.left, (sh - height) / 2 + wr.top, wr.right - wr.left, wr.bottom - wr.top, TRUE);
	}

	_pScreen = new CDXScreen();
	_pScreen->SetSize(static_cast<float>(width), static_cast<float>(height));
	SetupScreen();

	_cursorMaxX = width - 1;
	_cursorMaxY = height - 1;

	_cursorPosX = width / 2.0f;
	_cursorPosY = height / 2.0f;

	_btnMainSave->SetEnabled(saveEnabled);
	_btnMainResume->SetVisible(resumeVisible);
}

void CMainMenuModule::GameOver()
{
	EnableSaveAndResume(FALSE);
}

void CMainMenuModule::LoadCancel(LPVOID data)
{
	_pScreen->PopModal();
}

void CMainMenuModule::LoadLoad(LPVOID info)
{
	_pScreen->PopModal();
}

void CMainMenuModule::LoadGame(SaveGameInfo info)
{
	CurrentGameInfo = info;

	_pScreen->PopModal();
	POINT pt;
	pt.x = dx.GetWidth() / 2;
	pt.y = dx.GetHeight() / 2;
	ClientToScreen(_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);
	CAmbientAudio::Clear();
	CGameController::LoadGame((LPWSTR)info.FileName.c_str());
	EnableSaveAndResume(TRUE);
}

void CMainMenuModule::LoadSetup()
{
	int ix = 0;
	int w = dx.GetWidth();
	int h = dx.GetHeight();
	int y = 32;

	while (ix < _saveGameControls.size() && ix < _savedGames.size())
	{
		CSaveGameControl* sgc = _saveGameControls.at(ix);
		SaveGameInfo sgi = _savedGames.at(_loadTopIndex + ix);
		if ((y + sgc->GetHeight()) > (h - (64 * pConfig->FontScale)))
		{
			break;
		}

		sgc->SetInfo(sgi);
		sgc->SetPosition(16.0f, static_cast<float>(y));
		sgc->SetWidth(w - 80.0f);
		sgc->SetVisible(TRUE);
		y += static_cast<int>(sgc->GetHeight() + 8 * pConfig->FontScale);
		ix++;
	}

	_loadVisibleSavesCount = ix;

	while (ix < _saveGameControls.size())
	{
		CSaveGameControl* sgc = _saveGameControls.at(ix++);
		sgc->SetVisible(FALSE);
	}
}

void CMainMenuModule::LoadScroll(int top)
{
	_loadTopIndex = min(_savedGames.size() - _loadVisibleSavesCount - 1, max(0, top));
	LoadSetup();
}

void CMainMenuModule::SaveCancel(LPVOID data)
{
	SaveMode = SaveMode::Load;
	_pScreen->PopModal();
}

void CMainMenuModule::SaveSave(LPVOID data)
{
	if (_saveControl != NULL)
	{
		for (int i = 0; i < UAKM_SAVE_PADDING1 - UAKM_SAVE_COMMENT; i++)
		{
			CGameController::SetData(UAKM_SAVE_COMMENT + i, _commentBuffer[i]);
		}

		SaveGameInfo info = _saveControl->GetInfo();
		CGameController::SaveGame((LPWSTR)info.FileName.c_str());
		CurrentGameInfo.FileName = info.FileName;
	}

	SaveMode = SaveMode::Load;
	_pScreen->PopModal();
}

void CMainMenuModule::SaveIncrementSave(LPVOID data)
{
	if (_saveControl != NULL)
	{
		SaveGameInfo info = _saveControl->GetInfo();
		LPWSTR fn = (LPWSTR)info.FileName.c_str();
		int len = wcslen(fn);

		// Increment extension, fail if already 999
		int index = _wtoi(fn + len - 3) + 1;
		if (index < 1000)
		{
			fn[len - 1] = '0' + index % 10;
			fn[len - 2] = '0' + (index / 10) % 10;
			fn[len - 3] = '0' + (index / 100) % 10;

			CurrentGameInfo.FileName = fn;

			// Clear comment
			for (int i = 0; i < UAKM_SAVE_PADDING1 - UAKM_SAVE_COMMENT; i++)
			{
				CGameController::SetData(UAKM_SAVE_COMMENT + i, 0);
			}

			CGameController::SaveGame(fn);

			SaveMode = SaveMode::Load;
			_pScreen->PopModal();
		}
	}
}

void CMainMenuModule::SaveSetup()
{
	SaveMode = SaveMode::Extension;
	SaveTypedChars = 3;

	memset(_commentBuffer, 0, 256);

	SaveGameInfo info;
	info.FileName = L"GAMES\\";
	int nameLength = CurrentGameInfo.Player.length();
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
	int lastDot = CurrentGameInfo.FileName.find_last_of('.');
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

void CMainMenuModule::Clear()
{
	if (_pScreen != NULL)
	{
		delete _pScreen;
		_pScreen = NULL;
	}

	_saveGameControls.clear();

	for (auto sgc : _saveGameControls)
	{
		delete sgc;
	}

	if (_pConfig != NULL)
	{
		//delete _pConfig;
		_pConfig = NULL;
	}
}

void CMainMenuModule::ConfigureControl(InputAction data)
{
	ConfiguringControl = TRUE;
	ControlInputAction = data;
	_pCancelConfigControlBtn->SetVisible(TRUE);
	_pConfigCancelBtn->SetVisible(FALSE);
	_pConfigAcceptBtn->SetVisible(FALSE);
	_pConfiguredControl->SetIsBeingConfigured(TRUE);
}

void CMainMenuModule::ConfigControlsCancel(InputAction data)
{
	ConfiguringControl = FALSE;
	_pConfiguredControl->SetIsBeingConfigured(FALSE);
	_pCancelConfigControlBtn->SetVisible(FALSE);
	_pConfigCancelBtn->SetVisible(TRUE);
	_pConfigAcceptBtn->SetVisible(TRUE);
}

void CMainMenuModule::MapControl(CControllerData* pControllerData)
{
	bool valid = (IsJoystickSource(pControllerData->Source) == _pConfiguredControl->IsJoystickConfigControl());
	if (valid)
	{
		if (IsJoystickSource(pControllerData->Source))
		{
			int offset = pControllerData->Offset;
			if (offset == 0 || offset == 4)
			{
				offset = 0;
			}
			else if (offset == 8 || offset == 20)
			{
				offset = 20;
			}
			else if (offset == 12 || offset == 16)
			{
				offset = 12;
			}

			if (ControlInputAction == InputAction::MoveForward || ControlInputAction == InputAction::MoveBack || ControlInputAction == InputAction::MoveLeft || ControlInputAction == InputAction::MoveRight)
			{
				// All 4 directions should be mapped to these axes
				_controlMapping[InputAction::MoveForward].JoystickSource = _controlMapping[InputAction::MoveBack].JoystickSource = _controlMapping[InputAction::MoveLeft].JoystickSource = _controlMapping[InputAction::MoveRight].JoystickSource = pControllerData->Source;
				_controlMapping[InputAction::MoveForward].JoystickIdentifier = _controlMapping[InputAction::MoveBack].JoystickIdentifier = _controlMapping[InputAction::MoveLeft].JoystickIdentifier = _controlMapping[InputAction::MoveRight].JoystickIdentifier = offset;

				_joystickControls[InputAction::MoveForward]->UpdateControlText(pControllerData);
			}
			else
			{
				_controlMapping[ControlInputAction].JoystickSource = pControllerData->Source;
				_controlMapping[ControlInputAction].JoystickIdentifier = pControllerData->Offset;
			}
		}
		else
		{
			_controlMapping[ControlInputAction].MouseKeySource = pControllerData->Source;
			_controlMapping[ControlInputAction].MouseKeyIdentifier = pControllerData->Offset;
		}

		_pConfiguredControl->UpdateControlText(pControllerData);
		_pConfiguredControl->SetIsBeingConfigured(FALSE);
		_pCancelConfigControlBtn->SetVisible(FALSE);
		_pConfigCancelBtn->SetVisible(TRUE);
		_pConfigAcceptBtn->SetVisible(TRUE);
		ConfiguringControl = FALSE;
	}
}

void CMainMenuModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	if (_pSliderDragging == NULL)
	{
		CDXControl* pPrevMouseOver = _pScreen->GetCurrentMouseOver();
		CDXControl* pHit = _pScreen->HitTest(_cursorPosX, _cursorPosY);
		if (pPrevMouseOver != NULL && (pHit == NULL || pHit != pPrevMouseOver))
		{
			pPrevMouseOver->SetMouseOver(FALSE);
		}

		if (pHit != NULL && pHit->GetEnabled())
		{
			CDXControl::ControlType ct = pHit->GetType();
			if (ct == CDXControl::ControlType::Button || ct == CDXControl::ControlType::ImageButton || ct == CDXControl::ControlType::CheckBox || ct == CDXControl::ControlType::SaveGameControl || ct == CDXControl::ControlType::Control || ct == CDXControl::ControlType::Slider)
			{
				CDXControl* pBtn = (CDXControl*)pHit;
				if (pBtn != NULL && !pBtn->GetMouseOver())
				{
					pBtn->SetMouseOver(TRUE);
				}
			}
		}
	}
	else
	{
		_pSliderDragging->Drag(_cursorPosX, _cursorPosY);
	}
}

void CMainMenuModule::BeginAction()
{
	float x = _cursorPosX;
	float y = _cursorPosY;
	CDXControl* pHit = _pScreen->HitTest(x, y);
	if (pHit != NULL && pHit->GetEnabled())
	{
		CDXControl::ControlType ct = pHit->GetType();
		if (ct == CDXControl::ControlType::Button || ct == CDXControl::ControlType::ImageButton || ct == CDXControl::ControlType::Control)
		{
			CDXButton* pBtn = (CDXButton*)pHit;
			if (pBtn != NULL)
			{
				if (!ConfiguringControl || pBtn == _pCancelConfigControlBtn)
				{
					if (ct == CDXControl::ControlType::Control)
					{
						_pConfiguredControl = (CDXControlButton*)pHit;
					}
					pBtn->Click();
				}
			}
		}
		else if (ct == CDXControl::ControlType::CheckBox)
		{
			CDXCheckBox* pCB = (CDXCheckBox*)pHit;
			if (pCB != NULL)
			{
				pCB->SetCheck(!pCB->GetCheck());
			}
		}
		else if (ct == CDXControl::ControlType::SaveGameControl)
		{
			CSaveGameControl* pSGC = (CSaveGameControl*)pHit;
			if (pSGC != NULL)
			{
				if (SaveMode == SaveMode::Load)
				{
					// Load save game
					pSGC->Click();
				}
				else if (SaveMode == SaveMode::Extension)
				{
					// Check if comment area has been clicked

					float x = _cursorPosX - _saveControl->GetX();
					float y = _cursorPosY - _saveControl->GetY();
					if (y >= 68 * pConfig->FontScale && y <= 83 * pConfig->FontScale && SaveTypedChars == 3)
					{
						SaveMode = SaveMode::Comment;
						SaveGameInfo info = _saveControl->GetInfo();
						SaveTypedChars = info.Comment.length();
						_caretPos = static_cast<int>(_saveControl->GetX() + 68 * pConfig->FontScale + ceil(TexFont.PixelWidth(_commentBuffer)));
					}
				}
				else if (SaveMode == SaveMode::Comment)
				{
					// Check if extension area has been clicked
					float x = _cursorPosX - _saveControl->GetX();
					float y = _cursorPosY - _saveControl->GetY();
					if (y >= 8 * pConfig->FontScale && y <= 22 * pConfig->FontScale && x < 350 * pConfig->FontScale)
					{
						SaveMode = SaveMode::Extension;
						SaveTypedChars = 3;// TODO: Check actual number of extension chars
					}
				}
			}
		}
		else if (ct == CDXControl::ControlType::TabItem)
		{
			// Ensure tab item is selected
			((CDXTabItem*)pHit)->Select();
		}
		else if (ct == CDXControl::ControlType::Slider)
		{
			_pSliderDragging = (CDXSlider*)pHit;
		}
	}
}

void CMainMenuModule::EndAction()
{
	if (_pSliderDragging != NULL)
	{
		_pSliderDragging = NULL;
	}
}

void CMainMenuModule::Back()
{
	if (_pScreen->IsModal() && _pConfiguredControl == NULL)
	{
		_pScreen->PopModal();
		SaveMode = SaveMode::Load;
	}
}

void CMainMenuModule::Next()
{
	Scroll(1);
}

void CMainMenuModule::Prev()
{
	Scroll(-1);
}

void CMainMenuModule::Scroll(int direction)
{
	if (_pScreen->GetModal() == _pLoad)
	{
		LoadScroll(_loadTopIndex + direction);
	}
}

BOOL CMainMenuModule::IsValidForControlConfiguration(InputSource source)
{
	return (ConfiguringControl && (_controlMapping[ControlInputAction].AcceptableSource & source) == source);
}

void CMainMenuModule::MouseMove(POINT pt)
{
	if (IsValidForControlConfiguration(InputSource::Mouse) && !_pConfiguredControl->IsJoystickConfigControl())
	{
		// Set binding
		CControllerData cdata;
		cdata.Source = InputSource::Mouse;
		cdata.Data = 0;
		MapControl(&cdata);
	}
}

void CMainMenuModule::MouseDown(POINT pt, int btn)
{
	if (IsValidForControlConfiguration(InputSource::MouseButton) && !_pConfiguredControl->IsJoystickConfigControl())
	{
		// Set binding
		CControllerData cdata;
		cdata.Source = InputSource::MouseButton;
		cdata.Data = btn;
		MapControl(&cdata);
	}
}

void CMainMenuModule::MouseWheel(int scroll)
{
	if (IsValidForControlConfiguration(InputSource::MouseWheel) && !_pConfiguredControl->IsJoystickConfigControl())
	{
		// Set binding
		CControllerData cdata;
		cdata.Source = InputSource::MouseWheel;
		cdata.Offset = cdata.Data = scroll;
		MapControl(&cdata);
	}
}

void CMainMenuModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (ConfiguringControl)
	{
		if (IsValidForControlConfiguration(InputSource::Key) && !_pConfiguredControl->IsJoystickConfigControl())
		{
			// Set binding
			CControllerData cdata;
			cdata.Source = InputSource::Key;
			cdata.Offset = cdata.Data = lParam & 0x00ff0000;
			MapControl(&cdata);
		}
	}
	else if (_pScreen->GetModal() == _pSave)
	{
		// Check selected field (extension or comment)
		// Return should trigger save
		if (SaveMode == SaveMode::Extension)
		{
			// Extension
			if (key == VK_LEFT || key == VK_BACK)
			{
				if (SaveTypedChars > 0)
				{
					// Remove one character from the extension, update filename in save game control
					SaveGameInfo info = _saveControl->GetInfo();
					std::wstring fileName = info.FileName.substr(0, info.FileName.length() - 1);
					_saveControl->SetFileName(fileName);
					SaveTypedChars--;
				}
			}
			else if (key >= '0' && key <= '9')
			{
				if (SaveTypedChars < 3)
				{
					SaveGameInfo info = _saveControl->GetInfo();
					info.FileName += (wchar_t)key;
					_saveControl->SetFileName(info.FileName);
					SaveTypedChars++;
				}
			}
			else if ((key == VK_TAB || key == VK_RETURN) && SaveTypedChars == 3)
			{
				SaveMode = SaveMode::Comment;
				SaveGameInfo info = _saveControl->GetInfo();
				SaveTypedChars = info.Comment.length();
				_caretPos = static_cast<int>(_saveControl->GetX() + 68 * pConfig->FontScale);
			}
		}
		else if (SaveMode == SaveMode::Comment)
		{
			// Comment
			if (key == VK_LEFT || key == VK_BACK)
			{
				if (SaveTypedChars > 0)
				{
					_commentBuffer[--SaveTypedChars] = 0;
					_saveControl->SetComment(_commentBuffer);
					_caretPos = static_cast<int>(_saveControl->GetX() + 68 * pConfig->FontScale + ceil(TexFont.PixelWidth(_commentBuffer)));
				}
			}
			else if (key == VK_RETURN)
			{
				// Save
				SaveSave(NULL);
			}
			else if (SaveTypedChars < 0x90)
			{
				BYTE keyStates[256];
				GetKeyboardState(keyStates);
				WORD ascii = 0;
				if (ToAscii(static_cast<UINT>(key), 0, keyStates, &ascii, 0) == 1)
				{
					if (ascii >= 0x20 && ascii <= 0x7f)
					{
						// Check if comment is too long to be display at current resolution
						float x = _saveControl->GetX() + 68 + ceil(TexFont.PixelWidth(_commentBuffer));
						if (x < (dx.GetWidth() - 48))
						{
							_commentBuffer[SaveTypedChars++] = (char)ascii;
							_saveControl->SetComment(_commentBuffer);
							_caretPos = static_cast<int>(_saveControl->GetX() + 68 * pConfig->FontScale + ceil(TexFont.PixelWidth(_commentBuffer)));
						}
					}
				}
			}
		}
	}
	else if (_pScreen->GetModal() == _pLoad)
	{
		if (key == VK_UP)
		{
			LoadScroll(_loadTopIndex - 1);
		}
		else if (key == VK_DOWN)
		{
			LoadScroll(_loadTopIndex + 1);
		}
		else if (key == VK_PRIOR)
		{
			LoadScroll(_loadTopIndex - _loadVisibleSavesCount);
		}
		else if (key == VK_NEXT)
		{
			LoadScroll(_loadTopIndex + _loadVisibleSavesCount);
		}
	}
}

void CMainMenuModule::GamepadInput(InputSource source, int offset, int data)
{
	if (IsValidForControlConfiguration(source) && _pConfiguredControl->IsJoystickConfigControl())
	{
		// Set binding
		CControllerData cdata;
		cdata.Source = source;
		cdata.Offset = offset;
		cdata.Data = data;
		MapControl(&cdata);
	}
}

void CMainMenuModule::SetupConfigFrame()
{
	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_pConfig = new CDXFrame("Configuration", w - 64.0f, h - 64.0f);
	_pScreen->AddChild(_pConfig, 32.0f, 32.0f);

	// Add tabs for video, audio, control, misc
	_pConfigTab = new CDXTabControl(w - 80.0f, h - 80.0f);

	_pConfig->AddChild(_pConfigTab, 8.0f, 8.0f);

	float fontHeight = (TexFont.Height() * pConfig->FontScale);
	float buttonHeight = fontHeight + 24.0f;
	float checkBoxWidth = 250.0f + 72 * pConfig->FontScale;

	// Video config
	float tw = (w - 80.0f) / 3 - 2.0f;
	_pConfigVideo = new CDXTabItem(_pConfigTab, "Video", tw, h - 80.0f);
	_pConfigTab->AddChild(_pConfigVideo, 0.0f, 0.0f);

	float y = 46.0f;

	// Resolution and full screen
	CDXImageButton* pLBtn = new CDXImageButton(0, ConfigPreviousResolution);
	_pConfigVideo->AddChild(pLBtn, checkBoxWidth - 50.0f, y);

	CDXImageButton* pRBtn = new CDXImageButton(1, ConfigNextResolution);
	_pConfigVideo->AddChild(pRBtn, checkBoxWidth + 30.0f, y);
	y += buttonHeight;

	CDXLabel* pLabel = new CDXLabel("Resolution");
	_pConfigVideo->AddChild(pLabel, 22.0f, y);

	pResolution = new CDXLabel("");
	_pConfigVideo->AddChild(pResolution, checkBoxWidth - 60.0f, y);
	y += buttonHeight;

	_pConfigVideo->AddChild(new CDXCheckBox("Full screen", &cfgFullscreen, checkBoxWidth), 22.0f, y);
	y += buttonHeight;

	_pConfigVideo->AddChild(new CDXCheckBox("Captions", &cfgCaptions, checkBoxWidth), 22.0f, y);
	y += buttonHeight;

	_pConfigVideo->AddChild(new CDXCheckBox("Alternative media", &cfgAlternativeMedia, checkBoxWidth), 22.0f, y);
	y += buttonHeight;

	_pConfigVideo->AddChild(new CDXCheckBox("Anisotropic filter", &cfgAnisotropicFiltering, checkBoxWidth), 22.0f, y);
	y += buttonHeight;

	_pConfigVideo->AddChild(pFontScaleSlider = new CDXSlider("Font scale", 1.0f, 3.0f, 0.25f, &cfgFontScale, 5), 22.0f, y);

	// Audio config
	_pConfigAudio = new CDXTabItem(_pConfigTab, "Audio", tw, h - 80.0f);
	_pConfigTab->AddChild(_pConfigAudio, 0.0f, 0.0f);

	y = 46.0f;

	_pConfigAudio->AddChild(pVolumeSlider = new CDXSlider("Volume", 0.0f, 100.0f, 1.0f, &cfgVolume, 0), 22.0f, y);
	y += buttonHeight * 1.5f;

	// MIDI
	_pConfigAudio->AddChild(new CDXCheckBox("MIDI", &cfgPlayMIDI, checkBoxWidth), 22.0f, y);
	y += buttonHeight;

	CDXImageButton* pMLBtn = new CDXImageButton(0, ConfigPreviousMIDIDevice);
	_pConfigAudio->AddChild(pMLBtn, checkBoxWidth - 50.0f, y);

	CDXImageButton* pMRBtn = new CDXImageButton(1, ConfigNextMIDIDevice);
	_pConfigAudio->AddChild(pMRBtn, checkBoxWidth + 30.0f, y);
	y += buttonHeight;

	CDXLabel* pMIDILabel = new CDXLabel("MIDI Device");
	_pConfigAudio->AddChild(pMIDILabel, 22.0f, y);

	pMIDIDevice = new CDXLabel("");
	_pConfigAudio->AddChild(pMIDIDevice, checkBoxWidth - 60.0f, y);
	y += buttonHeight;

	_pConfigAudio->AddChild(pMIDIVolumeSlider = new CDXSlider("MIDI volume", 0.0f, 100.0f, 1.0f, &cfgMIDIVolume, 0), 22.0f, y);

	// Control config
	_pConfigControl = new CDXTabItem(_pConfigTab, "Controls", tw, h - 80.0f);
	_pConfigTab->AddChild(_pConfigControl, 0.0f, 0.0f);

	y = 66.0f;

	_pConfigControlTab = new CDXTabControl(w - 80.0f, h - 80.0f);
	_pConfigControl->AddChild(_pConfigControlTab, 0.0f, 20.0f);

	float ctw = (w - 80.0f) / 2 - 2.0f;
	_pConfigControlKeyMouse = new CDXTabItem(_pConfigControlTab, "Mouse & Keyboard", ctw, h - 80.0f);
	_pConfigControlTab->AddChild(_pConfigControlKeyMouse, 0.0f, 10.0f);
	_pConfigControlJoystick = new CDXTabItem(_pConfigControlTab, "Joystick", ctw, h - 80.0f);
	_pConfigControlTab->AddChild(_pConfigControlJoystick, 0.0f, 10.0f);

	_pConfigControl->AddChild(new CDXCheckBox("Invert Y", &cfgInvertY, checkBoxWidth), 22.0f, y);
	//y += buttonHeight;

	// Make a copy of the current controls
	_controlMapping = CInputMapping::ControlsMap;

	// TODO: Change ConfigControlsCancel to take no params?
	_pCancelConfigControlBtn = new CDXButton("Cancel", 64.0f * pConfig->FontScale, 32.0f * ::pConfig->FontScale, [](LPVOID) {
		ConfigControlsCancel(InputAction::Cursor); 
		});
	_pCancelConfigControlBtn->SetVisible(FALSE);
	_pConfigControl->AddChild(_pCancelConfigControlBtn, 24.0f, h - 72.0f - 54.0f * ::pConfig->FontScale);

	_mouseKeyControls.clear();
	_mouseKeyControls[InputAction::Cursor] = new CDXControlButton("Cursor", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Cursor);
	_mouseKeyControls[InputAction::Action] = new CDXControlButton("Action", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Action);
	_mouseKeyControls[InputAction::Cycle] = new CDXControlButton("Cycle", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Cycle);
	_mouseKeyControls[InputAction::Back] = new CDXControlButton("Back", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Back);
	_mouseKeyControls[InputAction::Travel] = new CDXControlButton("Travel", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Travel);
	_mouseKeyControls[InputAction::Inventory] = new CDXControlButton("Inventory", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Inventory);
	_mouseKeyControls[InputAction::Run] = new CDXControlButton("Run", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Run);
	_mouseKeyControls[InputAction::Next] = new CDXControlButton("Next", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Next);
	_mouseKeyControls[InputAction::Prev] = new CDXControlButton("Previous", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Prev);
	_mouseKeyControls[InputAction::MoveForward] = new CDXControlButton("Move forward", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveForward);
	_mouseKeyControls[InputAction::MoveBack] = new CDXControlButton("Move back", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveBack);
	_mouseKeyControls[InputAction::MoveLeft] = new CDXControlButton("Move left", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveLeft);
	_mouseKeyControls[InputAction::MoveRight] = new CDXControlButton("Move right", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveRight);
	_mouseKeyControls[InputAction::MoveUp] = new CDXControlButton("Move up", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveUp);
	_mouseKeyControls[InputAction::MoveDown] = new CDXControlButton("Move down", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveDown);
	_mouseKeyControls[InputAction::Hints] = new CDXControlButton("Hints", &_controlMapping, FALSE, 0.0f, 0.0f, ConfigureControl, InputAction::Hints);

	_mouseKeyControls[InputAction::Cursor]->SetEnabled(FALSE);	// Should not be possible to reconfigure this one

	_joystickControls.clear();
	_joystickControls[InputAction::Cursor] = new CDXControlButton("Cursor", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Cursor);
	_joystickControls[InputAction::Action] = new CDXControlButton("Action", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Action);
	_joystickControls[InputAction::Cycle] = new CDXControlButton("Cycle", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Cycle);
	_joystickControls[InputAction::Back] = new CDXControlButton("Back", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Back);
	_joystickControls[InputAction::Travel] = new CDXControlButton("Travel", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Travel);
	_joystickControls[InputAction::Inventory] = new CDXControlButton("Inventory", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Inventory);
	_joystickControls[InputAction::Run] = new CDXControlButton("Run", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Run);
	_joystickControls[InputAction::Next] = new CDXControlButton("Next", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Next);
	_joystickControls[InputAction::Prev] = new CDXControlButton("Previous", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Prev);
	_joystickControls[InputAction::MoveForward] = new CDXControlButton("Movement", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveForward);
	_joystickControls[InputAction::MoveUp] = new CDXControlButton("Move up", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveUp);
	_joystickControls[InputAction::MoveDown] = new CDXControlButton("Move down", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::MoveDown);
	_joystickControls[InputAction::Hints] = new CDXControlButton("Hints", &_controlMapping, TRUE, 0.0f, 0.0f, ConfigureControl, InputAction::Hints);

	float x = 22.0f;

	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Cursor], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Cursor], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Action], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Action], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Back], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Back], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Cycle], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Cycle], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Next], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Next], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Prev], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Prev], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::MoveForward], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::MoveForward], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::MoveBack], x, y);
	//_pConfigControlJoystick->AddChild(_joystickControls[InputAction::MoveBack], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::MoveUp], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::MoveLeft], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::MoveDown], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::MoveRight], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Run], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::MoveUp], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Inventory], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::MoveDown], x, y);
	_pConfigControlJoystick->AddChild(_joystickControls[InputAction::Travel], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Run], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Inventory], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Travel], x, y);
	y += fontHeight;
	_pConfigControlKeyMouse->AddChild(_mouseKeyControls[InputAction::Hints], x, y);
	y += fontHeight * 2.0f;
	_pConfigControlKeyMouse->AddChild(new CDXSlider("Mouse sensitivity", 0.25f, 2.0f, 0.05f, &cfgMouselookScaling, 2), x, y);
	y += fontHeight * 2.0f;

	/*
	Extra configurations for joysticks;
	Gain (1-20?)
	Dead zone (0-200?)
	Sensitivity (1-3?) (used as 'a' in gain function)
	*/

	// Accept/cancel buttons
	_pConfigCancelBtn = new CDXButton("Cancel", 64.0f * pConfig->FontScale, 32.0f * pConfig->FontScale, ConfigCancel);
	_pConfig->AddChild(_pConfigCancelBtn, 32.0f, h - 64.0f - 54.0f * pConfig->FontScale);

	_pConfigAcceptBtn = new CDXButton("Accept", 64.0f * pConfig->FontScale, 32.0f * pConfig->FontScale, ConfigAccept);
	_pConfig->AddChild(_pConfigAcceptBtn, w - 88.0f - 100.0f * pConfig->FontScale, h - 64.0f - 54.0f * pConfig->FontScale);
}

void CMainMenuModule::SetupLoadFrame()
{
	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_pLoad = new CDXFrame("Load Game", w - 8.0f, h - 8.0f);
	_pScreen->AddChild(_pLoad, 4.0f, 4.0f);

	_pLoad->AddChild(new CDXButton("Cancel", 64.0f * pConfig->FontScale, 32.0f * pConfig->FontScale, LoadCancel), 12.0f, h - 64.0f * pConfig->FontScale);

	// Create a number of placeholder controls
	for (int i = 0; i < 10; i++)
	{
		CSaveGameControl* sgc = new CSaveGameControl(LoadGame);
		_saveGameControls.push_back(sgc);
		_pLoad->AddChild(sgc, 16.0f, 0.0f);
	}
}

void CMainMenuModule::SetupSaveFrame()
{
	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_pSave = new CDXFrame("Save Game", w - 8.0f, h - 8.0f);
	_pScreen->AddChild(_pSave, 4.0f, 4.0f);

	// When saving, should use default profile name TEX on file (only required for new game)

	_pSave->AddChild(new CDXButton("Cancel", 64.0f * pConfig->FontScale, 32.0f * pConfig->FontScale, SaveCancel), 12.0f, h - 64.0f * pConfig->FontScale);
	_saveControl = new CSaveGameControl(NULL, true);
	_pSave->AddChild(_saveControl, 12, (h - _saveControl->GetHeight()) / 2);

	// Save button
	CDXButton* pSaveBtn = new CDXButton("Save", 64.0f * pConfig->FontScale, 32.0f * pConfig->FontScale, SaveSave);
	_pSave->AddChild(pSaveBtn, w - 128.0f * pConfig->FontScale, h - 64.0f * pConfig->FontScale);

	// Increment and Save button
	CDXButton* pIncrementAndSaveBtn = new CDXButton("Increment", 64.0f * pConfig->FontScale, 32.0f * pConfig->FontScale, SaveIncrementSave);
	_pSave->AddChild(pIncrementAndSaveBtn, w / 2 - 32.0f, h - 64.0f * pConfig->FontScale);
}

void CMainMenuModule::EnableSaveAndResume(BOOL enable)
{
	_btnMainResume->SetVisible(enable);
	_btnMainSave->SetEnabled(enable);
}

void CMainMenuModule::UpdateSaveGameData()
{
	// TODO: Copy player name from savegame
	// TODO: Find last save index for player
	CurrentGameInfo.Player = "TEX";
	CurrentGameInfo.FileName = L"GAMES\\TEX___00.000";
}
