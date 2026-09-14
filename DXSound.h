#pragma once
#include <xaudio2.h>
#include <atlbase.h>

class CDXSound : public IXAudio2VoiceCallback, IXAudio2EngineCallback
{
public:
	CDXSound();
	~CDXSound();

	static void Init();
	static void Dispose();
	static IXAudio2SourceVoice* CreateSourceVoice(WAVEFORMATEX* pwfx, UINT32 Flags = 0, float MaxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO, IXAudio2VoiceCallback* pCallback = NULL);

	void Stop();
	void Play(PBYTE pData, DWORD size);

	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) { }
	STDMETHOD_(void, OnVoiceProcessingPassEnd)() { }
	STDMETHOD_(void, OnStreamEnd)() { }
	STDMETHOD_(void, OnBufferStart)(void*) { }
	STDMETHOD_(void, OnBufferEnd)(void*) { }
	STDMETHOD_(void, OnLoopEnd)(void*) { }
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT) { }

	static void SetVolume(float volume) { if (MasteringVoice != NULL) { MasteringVoice->SetVolume(volume); } }

	STDMETHOD_(void, OnProcessingPassStart) (void) { };
	STDMETHOD_(void, OnProcessingPassEnd) (void) { };
	STDMETHOD_(void, OnCriticalError) (HRESULT Error) { MasteringVoice = NULL; };

protected:
	static IXAudio2* XAudio2;
	static IXAudio2MasteringVoice* MasteringVoice;
	IXAudio2SourceVoice* _sourceVoice;

	static CDXSound* _deviceCallbackReceiver;
};
