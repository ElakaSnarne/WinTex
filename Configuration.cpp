#include "Configuration.h"
#include <Windows.h>
#include "Globals.h"
#include "Utilities.h"
#include "GameController.h"

#define CONFIG_FLAGS_FULLSCREEN			1
#define CONFIG_FLAGS_CAPTIONS			2
#define CONFIG_FLAGS_ALTERNATIVEMEDIA	64
#define CONFIG_FLAGS_PLAYMIDI			128
#define CONFIG_FLAGS_INVERTY			256
#define CONFIG_FLAGS_ANISOTROPICFILTER	512

CConfiguration::CConfiguration(LPWSTR gameName)
{
	_gameName = gameName;

	Width = 640;
	Height = 480;
	ScreenMode = -1;
	pAdapter = NULL;

	FullScreen = FALSE;
	MinAcceptedMode = 0;

	Captions = TRUE;
	AlternativeMedia = FALSE;

	PlayMIDI = TRUE;
	NumberOfMIDIOutDevices = 0;
	MIDIDeviceId = -1;

	InvertY = FALSE;
	MouselookScaling = 1.0f;
	FontScale = 1.0f;

	AnisotropicFilter = TRUE;
	Volume = 100;
	MIDIVolume = 100;

	// Read from registry
	HKEY hk;
	std::wstring key = L"SOFTWARE\\Access Software\\" + _gameName;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hk) == 0)
	{
		Width = GetRegistryInt(hk, L"Width", 640);
		Height = GetRegistryInt(hk, L"Height", 480);

		int flags = GetRegistryInt(hk, L"Flags", 0);
		FullScreen = ((flags & CONFIG_FLAGS_FULLSCREEN) != 0);
		Captions = ((flags & CONFIG_FLAGS_CAPTIONS) != 0);
		AlternativeMedia = ((flags & CONFIG_FLAGS_ALTERNATIVEMEDIA) != 0);
		PlayMIDI = ((flags & CONFIG_FLAGS_PLAYMIDI) != 0);
		InvertY = ((flags & CONFIG_FLAGS_INVERTY) != 0);
		MouselookScaling = GetRegistryFloat(hk, L"MouselookScaling", 1.0f);
		AnisotropicFilter = ((flags & CONFIG_FLAGS_ANISOTROPICFILTER) == 0);	// Inverted

		MIDIDeviceId = GetRegistryInt(hk, L"MIDIDeviceId", -1);
		FontScale = max(1.0f, min(GetRegistryFloat(hk, L"FontScale", 1.0f), 3.0f));
		Volume = max(0, min(GetRegistryInt(hk, L"Volume", 100), 100));
		MIDIVolume = max(0, min(GetRegistryInt(hk, L"MIDIVolume", 100), 100));

		RegCloseKey(hk);
	}

	// Get display modes, choose the closest
	pAdapter = dx.GetAdapter();
	if (pAdapter != NULL)
	{
		int currentModeSize = Width * Height;
		int currentMode = -1;
		int currentModeDiff = 1000000000;

		for (int m = 0; m < static_cast<int>(pAdapter->_numModes); m++)
		{
			if (pAdapter->_displayModeList[m].Width >= 640 && pAdapter->_displayModeList[m].Height >= 480)
			{
				int modeSize = pAdapter->_displayModeList[m].Width * pAdapter->_displayModeList[m].Height;
				int modeDiff = abs(modeSize - currentModeSize);
				if (modeDiff < currentModeDiff)
				{
					currentMode = m;
					currentModeDiff = modeDiff;
				}

				if (Width == pAdapter->_displayModeList[m].Width && Height == pAdapter->_displayModeList[m].Height)
				{
					ScreenMode = m;
					break;
				}
			}
			else
			{
				MinAcceptedMode = m + 1;
			}
		}

		if (currentMode >= 0)
		{
			Width = pAdapter->_displayModeList[currentMode].Width;
			Height = pAdapter->_displayModeList[currentMode].Height;
		}
	}

	// Get list of MIDI devices
	NumberOfMIDIOutDevices = midiOutGetNumDevs();
	pMIDIDevices = new MIDIOUTCAPSA[NumberOfMIDIOutDevices];
	if (pMIDIDevices != NULL)
	{
		for (int midiDeviceId = 0; midiDeviceId < NumberOfMIDIOutDevices; midiDeviceId++)
		{
			MIDIOUTCAPSA midiCaps;
			if (midiOutGetDevCapsA(midiDeviceId, &midiCaps, sizeof(midiCaps)) == MMSYSERR_NOERROR)
			{
				pMIDIDevices[midiDeviceId] = midiCaps;
			}
			else
			{
				ZeroMemory(&pMIDIDevices[midiDeviceId], sizeof(midiCaps));
			}
		}
	}
}

CConfiguration::~CConfiguration()
{
	if (pMIDIDevices != NULL)
	{
		delete[] pMIDIDevices;
	}
}

void CConfiguration::Save()
{
	// Write to registry
	HKEY hk;
	std::wstring key = L"SOFTWARE\\Access Software\\" + _gameName;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, 0, 0, KEY_WRITE, NULL, &hk, NULL) == 0)
	{
		// Save width, height, fullscreen, debug flags
		RegSetValueEx(hk, L"Width", 0, REG_DWORD, (PBYTE)&Width, sizeof(Width));
		RegSetValueEx(hk, L"Height", 0, REG_DWORD, (PBYTE)&Height, sizeof(Height));
		int flags = 0;
		if (FullScreen) flags |= CONFIG_FLAGS_FULLSCREEN;
		if (Captions) flags |= CONFIG_FLAGS_CAPTIONS;
		if (AlternativeMedia) flags |= CONFIG_FLAGS_ALTERNATIVEMEDIA;
		if (PlayMIDI) flags |= CONFIG_FLAGS_PLAYMIDI;
		if (InvertY) flags |= CONFIG_FLAGS_INVERTY;
		if (!AnisotropicFilter) flags |= CONFIG_FLAGS_ANISOTROPICFILTER;	// Inverted

		RegSetValueEx(hk, L"Flags", 0, REG_DWORD, (PBYTE)&flags, sizeof(flags));
		RegSetValueEx(hk, L"MIDIDeviceId", 0, REG_DWORD, (PBYTE)&MIDIDeviceId, sizeof(MIDIDeviceId));
		SetRegistryFloat(hk, L"MouselookScaling", MouselookScaling);
		SetRegistryFloat(hk, L"FontScale", FontScale);
		SetRegistryInt(hk, L"Volume", Volume);
		SetRegistryInt(hk, L"MIDIVolume", MIDIVolume);

		RegCloseKey(hk);
	}
}
