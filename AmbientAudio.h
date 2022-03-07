#pragma once

#include <xaudio2.h>
#include <unordered_map>
#include "Utilities.h"
#include <Windows.h>
#include "Map.h"

class CAmbientAudio// : public IXAudio2VoiceCallback
{
public:
	CAmbientAudio(BinaryData bd);
	CAmbientAudio()
	{
		_pData = NULL;
		_length = 0;
		_sourceVoice = NULL;
	};
	~CAmbientAudio();

	/*STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) { }
	STDMETHOD_(void, OnVoiceProcessingPassEnd)() { }
	STDMETHOD_(void, OnStreamEnd)() { }
	STDMETHOD_(void, OnBufferStart)(void*) { }
	STDMETHOD_(void, OnBufferEnd)(void*) { }
	STDMETHOD_(void, OnLoopEnd)(void*) { }
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT) { }*/

	static void Clear();
	static void Loop(CMapData* mapEntry, int entry1, int entry2);
	static void Play(CMapData* mapEntry, int entry);
	static void Stop(int entry);
	static void StopAll();
	void Loop();
	void Play();
	void Play(LPBYTE pData);
	void Stop();

	void static SetVolume(int entry, float volume);
	void SetVolume(float volume);

protected:
	static BOOL Load(CMapData* mapEntry, int entry);

	void Play(BOOL loop);

	// TODO: Need a master voice
	// TODO: Need a number of connected voices
	// TODO: Should be able to play looping sounds (wind in main street)
	// TODO: Should be able to keep waves in memory, so that they won't have to be reloaded every time they play
	// TODO: Need a function to clear and init

	LPBYTE _pData;
	int _length;
	IXAudio2SourceVoice* _sourceVoice;

	static std::unordered_map<int, CAmbientAudio*> Sounds;
};
