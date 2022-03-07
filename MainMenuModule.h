#pragma once

#include "ModuleBase.h"
#include "DXScreen.h"
#include "DXFrame.h"
#include "SaveGameInfo.h"
#include "SaveGameControl.h"
#include "DXTabControl.h"
#include "DXTabItem.h"
#include "InputMapping.h"
#include <unordered_map>
#include "DXControlButton.h"
#include "DXSlider.h"

enum class SaveMode
{
	Load,
	Extension,
	Comment
};

class CMainMenuModule : public CModuleBase
{
public:
	CMainMenuModule();
	virtual ~CMainMenuModule();

	virtual void Resize(int width, int height);

	virtual void Render();

	static CModuleBase* MainMenuModule;

	static void GameOver();

protected:
	virtual void Initialize();

	virtual void SetupScreen() = NULL;
	virtual void SetupConfigFrame();
	virtual void SetupLoadFrame();
	virtual void SetupSaveFrame();

	static void NewGame(LPVOID data);
	static void Resume(LPVOID data);
	static void Load(LPVOID data);
	static void Save(LPVOID data);
	static void Config(LPVOID data);
	static void Quit(LPVOID data);

	static CDXButton* _btnMainResume;
	static CDXButton* _btnMainSave;

	static void ConfigCancel(LPVOID data);
	static void ConfigAccept(LPVOID data);

	static void ConfigPreviousResolution(LPVOID data);
	static void ConfigNextResolution(LPVOID data);

	static void ConfigPreviousMIDIDevice(LPVOID data);
	static void ConfigNextMIDIDevice(LPVOID data);

	static int cfgWidth;
	static int cfgHeight;
	static int cfgMode;
	static int cfgMIDIDevice;
	static BOOL cfgFullscreen;
	static BOOL cfgCaptions;
	static BOOL cfgAlternativeMedia;
	static BOOL cfgAnisotropicFiltering;
	static float cfgFontScale;
	static float cfgVolume;
	static BOOL cfgPlayMIDI;
	static float cfgMIDIVolume;
	static BOOL cfgInvertY;

	static void UpdateResolutionLabel();
	static void UpdateMIDIDeviceLabel();

	static CDXScreen* _pScreen;
	static CDXFrame* _pConfig;
	static CDXTabControl* _pConfigTab;
	static CDXTabItem* _pConfigVideo;
	static CDXTabItem* _pConfigAudio;
	static CDXTabItem* _pConfigControl;
	static CDXTabItem* _pConfigMisc;
	static CDXTabControl* _pConfigControlTab;
	static CDXTabItem* _pConfigControlKeyMouse;
	static CDXTabItem* _pConfigControlJoystick;
	static CDXFrame* _pLoad;
	static CDXFrame* _pSave;
	static CDXLabel* pResolution;
	static CDXLabel* pMIDIDevice;

	static CDXLabel* pMIDIVolume;
	static CDXLabel* pVolume;
	static CDXLabel* pFontScale;

	static CDXSlider* pMIDIVolumeSlider;
	static CDXSlider* pVolumeSlider;
	static CDXSlider* pFontScaleSlider;

	static CDXButton* _pConfigCancelBtn;
	static CDXButton* _pConfigAcceptBtn;

	static void LoadCancel(LPVOID data);
	static void LoadLoad(LPVOID data);

	static CDXListBox _loadListBox;
	static std::vector<SaveGameInfo> _savedGames;
	static std::vector<CSaveGameControl*> _saveGameControls;

	static void LoadGame(SaveGameInfo info);

	static void LoadSetup();
	static int _loadTopIndex;
	static int _loadVisibleSavesCount;
	static void LoadScroll(int top);

	static void SaveSetup();
	static void SaveCancel(LPVOID data);
	static void SaveSave(LPVOID data);
	static void SaveIncrementSave(LPVOID data);
	static CSaveGameControl* _saveControl;

	static SaveGameInfo CurrentGameInfo;
	static int SaveTypedChars;

	static CDXText _saveCursor;

	static char _commentBuffer[256];
	static int _caretPos;

	void Clear();

	static SaveMode SaveMode;

	static void ConfigureControl(LPVOID data);
	static void ConfigControlsCancel(LPVOID data);
	static BOOL ConfiguringControl;
	static InputAction ControlInputAction;

	static CDXButton* _pCancelConfigControlBtn;
	static std::unordered_map<InputAction, InputMap> _controlMapping;
	static CDXControlButton* _pConfiguredControl;
	static std::unordered_map<InputAction, CDXControlButton*> _mouseKeyControls;
	static std::unordered_map<InputAction, CDXControlButton*> _joystickControls;

	static void MapControl(CControllerData* pControllerData);

	virtual void Scroll(int direction);

	static CDXSlider* _pSliderDragging;

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void EndAction();
	virtual void Back();
	virtual void Next();
	virtual void Prev();

	BOOL IsValidForControlConfiguration(InputSource source);
	virtual void MouseMove(POINT pt);
	virtual void MouseDown(POINT pt, int btn);
	virtual void MouseWheel(int scroll);
	virtual void KeyDown(WPARAM key, LPARAM lParam);
	virtual void GamepadInput(InputSource source, int offset, int data);
};
