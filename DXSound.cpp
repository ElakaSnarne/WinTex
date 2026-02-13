#include "DXSound.h"
#include "Globals.h"
#include <iostream>
#include <algorithm>

IXAudio2* CDXSound::XAudio2 = NULL;
IXAudio2MasteringVoice* CDXSound::MasteringVoice = NULL;

#ifdef PLATFORM_LINUX
SDL_AudioDeviceID CDXSound::_audioDevice = 0;
std::vector<CDXSound*> CDXSound::_activeSounds;
std::vector<CDXSourceVoice*> _activeVoices; // Global list of active voices
std::mutex CDXSound::_mutex;
float CDXSound::_masterVolume = 1.0f;
int CDXSound::_outputFreq = 44100;

// CDXSourceVoice Implementation
CDXSourceVoice::CDXSourceVoice(const WAVEFORMATEX* pwfx, IXAudio2VoiceCallback* pCallback) {
    _playing = false;
    _volume = 1.0f;
    _pCallback = pCallback;
    if (pwfx) _format = *pwfx;
    else memset(&_format, 0, sizeof(_format));
    
    std::lock_guard<std::mutex> lock(CDXSound::_mutex);
    _activeVoices.push_back(this);
}

CDXSourceVoice::~CDXSourceVoice() {
    DestroyVoice();
}

void CDXSourceVoice::DestroyVoice() {
    std::lock_guard<std::mutex> lock(CDXSound::_mutex);
    auto it = std::find(_activeVoices.begin(), _activeVoices.end(), this);
    if (it != _activeVoices.end()) {
        _activeVoices.erase(it);
    }
}

HRESULT CDXSourceVoice::SubmitSourceBuffer(const XAUDIO2_BUFFER *pBuffer, const void *pBufferWMA) {
    if (!pBuffer) return E_FAIL;
    
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    QueuedBuffer qb;
    qb.data = pBuffer->pAudioData;
    qb.size = pBuffer->AudioBytes;
    qb.position = 0.0;
    qb.ownsData = false; // We assume caller keeps data alive as per XAudio2 spec
    _buffers.push_back(qb);
    return S_OK;
}

HRESULT CDXSourceVoice::Start(UINT32 Flags, UINT32 OperationSet) {
    _playing = true;
    return S_OK;
}

HRESULT CDXSourceVoice::Stop(UINT32 Flags, UINT32 OperationSet) {
    _playing = false;
    return S_OK;
}

HRESULT CDXSourceVoice::SetVolume(float Volume, UINT32 OperationSet) {
    _volume = Volume;
    return S_OK;
}

HRESULT CDXSourceVoice::FlushSourceBuffers() {
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    _buffers.clear();
    return S_OK;
}

void CDXSourceVoice::Mix(int32_t* dst, int numSamples) {
    if (!_playing) return;
    
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    if (_buffers.empty()) return;

    int samplesMixed = 0;
    
    // Calculate bytes per sample based on format
    int bytesPerSample = (_format.wBitsPerSample / 8) * _format.nChannels;
    if (bytesPerSample == 0) bytesPerSample = 2; // Default to 16-bit mono

    float step = 1.0f;
    if (_format.nSamplesPerSec > 0) {
        step = (float)_format.nSamplesPerSec / (float)CDXSound::_outputFreq;
    }

    while (samplesMixed < numSamples && !_buffers.empty()) {
        QueuedBuffer& buf = _buffers.front();
        
        while (samplesMixed < numSamples) {
             int pos = (int)buf.position;
             // Align to block
             pos -= (pos % bytesPerSample);
             
             if (pos >= buf.size) {
                 // buffer exhausted
                 break;
             }
             
             int16_t sample = 0;
             if (_format.wBitsPerSample == 8) {
                 // 8-bit audio is unsigned 0-255
                 if (_format.nChannels == 1) {
                     // 8-bit Mono
                     if (pos < buf.size) {
                         uint8_t val = buf.data[pos];
                         sample = (int16_t)((val - 128) * 256);
                     }
                 } else {
                     // 8-bit Stereo
                     if (pos + 1 < buf.size) {
                         uint8_t l = buf.data[pos];
                         uint8_t r = buf.data[pos + 1];
                         sample = (int16_t)((((l - 128) + (r - 128)) / 2) * 256);
                     }
                 }
             } else {
                 // Assume 16-bit
                 if (_format.nChannels == 1) {
                     // 16-bit Mono
                     if (pos + 1 < buf.size) {
                        sample = ((const int16_t*)(buf.data + pos))[0];
                     }
                 } else {
                     // 16-bit Stereo or more: Average channels to mono
                     if (pos + 3 < buf.size) {
                        int16_t l = ((const int16_t*)(buf.data + pos))[0];
                        int16_t r = ((const int16_t*)(buf.data + pos))[1];
                        sample = (l + r) / 2;
                     }
                 }
             }
             
             dst[samplesMixed++] += (int32_t)(sample * _volume);
             
             buf.position += (step * bytesPerSample);
        }

        if ((int)buf.position >= buf.size) {
            _buffers.erase(_buffers.begin());
            if (_pCallback) {
                _pCallback->OnBufferEnd(NULL);
            }
        }
    }
}

void CDXSound::AudioCallback(void* userdata, Uint8* stream, int len) {
    std::lock_guard<std::mutex> lock(_mutex);
    SDL_memset(stream, 0, len);

    // Mix into a temporary buffer to handle volume and clipping
    // Assuming 16-bit signed audio (S16SYS)
    // len is in bytes. Samples = len / 2
    int sampleCount = len / 2;
    std::vector<int32_t> mixBuffer(sampleCount, 0);

    // Mix Sounds
    if (!_activeSounds.empty()) {
        float step = 44100.0f / (float)_outputFreq;
        
        for (auto it = _activeSounds.begin(); it != _activeSounds.end(); ) {
            CDXSound* sound = *it;
            if (!sound->_audioData.playing) {
                it = _activeSounds.erase(it);
                continue;
            }
            
            if (sound->_audioData.data == NULL) {
                 it = _activeSounds.erase(it);
                 continue;
            }

            // Mix
            for (int i = 0; i < sampleCount; ++i) {
                int pos = (int)sound->_audioData.position;
                // Align to 2 bytes (16-bit mono)
                pos -= (pos % 2);
                
                if (pos + 1 >= sound->_audioData.length) {
                    sound->_audioData.playing = false;
                    break;
                }
                
                const int16_t* src = (const int16_t*)(sound->_audioData.data + pos);
                mixBuffer[i] += *src;
                
                sound->_audioData.position += (step * 2.0); // 2 bytes per sample
            }
            
            ++it;
        }
    }
    
    // Mix Voices (BIC)
    if (!_activeVoices.empty()) {
        for (auto voice : _activeVoices) {
            voice->Mix(mixBuffer.data(), sampleCount);
        }
    }

    // Clip and write back
    int16_t* dst = (int16_t*)stream;
    for (int i = 0; i < sampleCount; ++i) {
        int32_t val = (int32_t)(mixBuffer[i] * _masterVolume);
        if (val > 32767) val = 32767;
        if (val < -32768) val = -32768;
        dst[i] = (int16_t)val;
    }
}
#endif

CDXSound::CDXSound()
{
	_sourceVoice = NULL;
#ifdef PLATFORM_LINUX
    _audioData.playing = false;
    _audioData.data = NULL;
    _audioData.length = 0;
    _audioData.position = 0.0;
#endif
}

CDXSound::~CDXSound()
{
#ifdef PLATFORM_LINUX
    Stop();
#endif
	if (_sourceVoice != NULL)
	{
		_sourceVoice->DestroyVoice();
		_sourceVoice = NULL;
	}
}

void CDXSound::Init()
{
#ifdef PLATFORM_WINDOWS
	XAudio2Create(&XAudio2, 0);
	XAudio2->CreateMasteringVoice(&MasteringVoice);
	SetVolume((pConfig->Volume) / 100.0f);
#else
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Audio Init Failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048; // Larger buffer to prevent stutter
    want.callback = AudioCallback;
    
    _audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (_audioDevice == 0) {
        std::cerr << "SDL OpenAudioDevice Failed: " << SDL_GetError() << std::endl;
    } else {
        SDL_PauseAudioDevice(_audioDevice, 0);
        _outputFreq = have.freq;
    }
    SetVolume((pConfig->Volume) / 100.0f);
#endif
}

void CDXSound::Dispose()
{
#ifdef PLATFORM_WINDOWS
	if (MasteringVoice != NULL)
	{
		MasteringVoice->DestroyVoice();
		MasteringVoice = NULL;
	}

	if (XAudio2 != NULL)
	{
		XAudio2->Release();
		XAudio2 = NULL;
	}
#else
    if (_audioDevice != 0) {
        SDL_CloseAudioDevice(_audioDevice);
        _audioDevice = 0;
    }
#endif
}

void CDXSound::Play(PBYTE pData, DWORD size)
{
	Stop();

#ifdef PLATFORM_WINDOWS
	char formatBuff[64];
	WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
	pwfx->wFormatTag = WAVE_FORMAT_PCM;
	pwfx->nChannels = 1;
	pwfx->nSamplesPerSec = 44100;
	pwfx->nAvgBytesPerSec = 44100 * 2;
	pwfx->nBlockAlign = 2;
	pwfx->wBitsPerSample = 16;
	pwfx->cbSize = 0;

	if (_sourceVoice == NULL)
	{
		_sourceVoice = CreateSourceVoice(pwfx, 0, 1.0f, this);
	}

	if (_sourceVoice != NULL)
	{
		_sourceVoice->Start(0, 0);
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = size - 64;
		buf.pAudioData = pData + 64;
		_sourceVoice->SubmitSourceBuffer(&buf);
	}
#else
    if (_audioDevice == 0) {
        std::cerr << "Play called but audio device not initialized" << std::endl;
        return;
    }
    
    if (pData == NULL) {
        std::cerr << "Play called with NULL data" << std::endl;
        return;
    }

    // Validate size (header skip)
    if (size <= 64) {
        std::cerr << "Play called with invalid size: " << size << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_mutex);
        _audioData.data = pData + 64;
        _audioData.length = size - 64;
        _audioData.position = 0.0;
        _audioData.playing = true;
        
        bool found = false;
        for (auto s : _activeSounds) {
            if (s == this) {
                found = true;
                break;
            }
        }
        if (!found) {
            _activeSounds.push_back(this);
        }
    }
#endif
}

void CDXSound::Stop()
{
#ifdef PLATFORM_WINDOWS
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_sourceVoice->FlushSourceBuffers();
	}
#else
    std::lock_guard<std::mutex> lock(_mutex);
    _audioData.playing = false;
    // We remove it immediately to be safe
    for (auto it = _activeSounds.begin(); it != _activeSounds.end(); ++it) {
        if (*it == this) {
            _activeSounds.erase(it);
            break;
        }
    }
#endif
}

IXAudio2SourceVoice* CDXSound::CreateSourceVoice(WAVEFORMATEX* pwfx, UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* pCallback)
{
	IXAudio2SourceVoice* ret = NULL;
#ifdef PLATFORM_WINDOWS
    if (XAudio2) {
	    XAudio2->CreateSourceVoice(&ret, pwfx, Flags, MaxFrequencyRatio, pCallback);
    }
#else
    ret = new CDXSourceVoice(pwfx, pCallback);
#endif
	return ret;
}

void CDXSound::SetVolume(float volume)
{
#ifdef PLATFORM_WINDOWS
    if (MasteringVoice != NULL) { MasteringVoice->SetVolume(volume); } 
#else
    _masterVolume = volume;
#endif
}
