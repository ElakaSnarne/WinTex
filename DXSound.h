#pragma once
#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <xaudio2.h>
#include <atlbase.h>
#else
#include "Win32Compat.h"
#include <vector>
#include <mutex>
#endif

class CDXSound : public IXAudio2VoiceCallback
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

	static void SetVolume(float volume);

#ifdef PLATFORM_LINUX
	struct AudioData {
		const uint8_t* data;
		uint32_t length;
		double position;
		bool playing;
	};
	AudioData _audioData;

	static void AudioCallback(void* userdata, Uint8* stream, int len);
	static SDL_AudioDeviceID _audioDevice;
	static std::vector<CDXSound*> _activeSounds;
	static std::mutex _mutex;
	static float _masterVolume;
	static int _outputFreq;
#endif

protected:
	static IXAudio2* XAudio2;
	static IXAudio2MasteringVoice* MasteringVoice;
	IXAudio2SourceVoice* _sourceVoice;
};

#ifdef PLATFORM_LINUX
class CDXSourceVoice : public IXAudio2SourceVoice {
public:
    CDXSourceVoice(const WAVEFORMATEX* pwfx, IXAudio2VoiceCallback* pCallback = NULL);
    ~CDXSourceVoice();

    HRESULT SubmitSourceBuffer(const XAUDIO2_BUFFER *pBuffer, const void *pBufferWMA = NULL) override;
    HRESULT Stop(UINT32 Flags = 0, UINT32 OperationSet = 0) override;
    HRESULT Start(UINT32 Flags = 0, UINT32 OperationSet = 0) override;
    void DestroyVoice() override;
    HRESULT SetVolume(float Volume, UINT32 OperationSet = 0) override;
    HRESULT FlushSourceBuffers() override;

    // Internal
    void Mix(int32_t* dst, int numSamples);
    bool IsPlaying() const { return _playing; }
    
private:
    bool _playing;
    float _volume;
    WAVEFORMATEX _format;
    
    struct QueuedBuffer {
        const uint8_t* data;
        uint32_t size;
        double position;
        bool ownsData; // If we need to copy
    };
    std::vector<QueuedBuffer> _buffers;
    std::recursive_mutex _mutex;
    IXAudio2VoiceCallback* _pCallback;
};
#endif
