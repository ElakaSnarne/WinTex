#include "DXSound.h"
#include "Globals.h"

IXAudio2* CDXSound::XAudio2 = NULL;
IXAudio2MasteringVoice* CDXSound::MasteringVoice = NULL;

CDXSound::CDXSound()
{
	_sourceVoice = NULL;
}

CDXSound::~CDXSound()
{
	if (_sourceVoice != NULL)
	{
		_sourceVoice->DestroyVoice();
		_sourceVoice = NULL;
	}
}

void CDXSound::Init()
{
	XAudio2Create(&XAudio2, 0);
	XAudio2->CreateMasteringVoice(&MasteringVoice);
	SetVolume((pConfig->Volume) / 100.0f);
}

void CDXSound::Dispose()
{
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
}

void CDXSound::Play(PBYTE pData, DWORD size)
{
	Stop();

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
}

void CDXSound::Stop()
{
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_sourceVoice->FlushSourceBuffers();
	}
}

IXAudio2SourceVoice* CDXSound::CreateSourceVoice(WAVEFORMATEX* pwfx, UINT32 Flags, float MaxFrequencyRatio, IXAudio2VoiceCallback* pCallback)
{
	IXAudio2SourceVoice* ret = NULL;
	XAudio2->CreateSourceVoice(&ret, pwfx, Flags, MaxFrequencyRatio, pCallback);
	return ret;
}
