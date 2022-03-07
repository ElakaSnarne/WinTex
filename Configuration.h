#pragma once

#include "DXAdapter.h"

class CConfiguration
{
public:
	CConfiguration(LPWSTR gameName);
	~CConfiguration();

	int Width;
	int Height;
	BOOL FullScreen;
	int ScreenMode;
	CDXAdapter* pAdapter;

	int MinAcceptedMode;
	BOOL Captions;
	BOOL AlternativeMedia;
	BOOL PlayMIDI;
	int MIDIDeviceId;

	BOOL InvertY;
	float FontScale;

	BOOL AnisotropicFilter;
	int Volume;
	int MIDIVolume;

	int NumberOfMIDIOutDevices;
	MIDIOUTCAPSA* pMIDIDevices;

	void Save();

	std::wstring GetGameName() { return _gameName; }

protected:
	std::wstring _gameName;
};
