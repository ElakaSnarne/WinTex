#pragma once

#include "DXAdapter.h"
#include <vector>

class CConfiguration
{
public:

	CConfiguration();
	CConfiguration(LPWSTR gameName);

	int Width				{640};
	int Height				{480};
	BOOL FullScreen			{FALSE};
	int ScreenMode			{-1};
	CDXAdapter* pAdapter    {nullptr};

	int MinAcceptedMode		{0};
	BOOL Captions			{TRUE};
	BOOL AlternativeMedia	{FALSE};
	BOOL PlayMIDI			{TRUE};
	int MIDIDeviceId		{-1};

	BOOL InvertY			{FALSE};
	float MouselookScaling  {1.0f};
	float FontScale			{1.0f};

	BOOL AnisotropicFilter  {TRUE};
	float Volume			{ 100.0f };
	float MIDIVolume		{ 100.0f };

	int NumberOfMIDIOutDevices {0};
	std::vector<MIDIOUTCAPSA> MIDIDevices{};

	void Save();

	std::wstring GetGameName() { return _gameName; }

protected:
	std::wstring _gameName{};
};
