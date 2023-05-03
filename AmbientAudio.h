#pragma once

#include <xaudio2.h>
#include <unordered_map>
#include "Utilities.h"
#include <Windows.h>
#include "Map.h"
#include "Mutex.h"

class CAmbientAudio : public IXAudio2VoiceCallback
{
public:
	CAmbientAudio(BinaryData bd);
	CAmbientAudio()
	{
		_pData = NULL;
		_length = 0;
		_sourceVoice = NULL;
		_finished = FALSE;
	};
	~CAmbientAudio();

	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) { }
	STDMETHOD_(void, OnVoiceProcessingPassEnd)() { }
	STDMETHOD_(void, OnStreamEnd)() { }
	STDMETHOD_(void, OnBufferStart)(void*) { }
	STDMETHOD_(void, OnBufferEnd)(void*) { _finished = TRUE; }
	STDMETHOD_(void, OnLoopEnd)(void*) { }
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT) { }

	static void Clear();
	static void Loop(CMapData* mapEntry, int entry1, int entry2);
	static void LoopPD(CMapData* mapEntry, int entry1, int entry2);
	static void Play(CMapData* mapEntry, int entry);
	static void Stop(int entry);
	static void StopAll();
	void Loop();
	void Play();
	void Play(LPBYTE pData);
	void Stop();

	void static SetVolume(int entry, float volume);
	void SetVolume(float volume);

	void static SetPan(int entry, float pan);
	void SetPan(float pan);

protected:
	static BOOL Load(CMapData* mapEntry, int entry);
	static BOOL LoadPD(CMapData* mapEntry, int entry);

	static CAmbientAudio* Find(int entry);
	BOOL Finished() { return _finished; }
	BOOL _finished;

	void Play(BOOL loop);

	LPBYTE _pData;
	int _length;
	IXAudio2SourceVoice* _sourceVoice;

	static std::unordered_map<int, CAmbientAudio*> Sounds;
};
