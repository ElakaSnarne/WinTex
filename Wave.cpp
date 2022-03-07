#include "Wave.h"
#include "Utilities.h"

CWave::CWave()
{
	_timeOfStart = 0;
}

CWave::~CWave()
{
}

BOOL CWave::Init(LPBYTE pData, int length)
{
	BOOL ret = CAnimBase::Init(pData, length);

	// Validate that the file is a WAVE
	if (ret && GetInt(pData, 0, 4) == 0x46464952)
	{
		_remainingAudioLength = GetInt(_pInputBuffer, 0x28, 4);
		_frameTime = 100;	// Default to 10 frames per second
	}

	return ret;
}

BOOL CWave::DecodeFrame()
{
	if (_remainingAudioLength > 0)
	{
		// This is a new audio buffer
		if (_sourceVoice == NULL)
		{
			char formatBuff[64];
			WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
			pwfx->wFormatTag = GetInt(_pInputBuffer, 0x14, 2);// WAVE_FORMAT_PCM;
			pwfx->nChannels = GetInt(_pInputBuffer, 0x16, 2);
			pwfx->nSamplesPerSec = GetInt(_pInputBuffer, 0x18, 4);
			pwfx->nAvgBytesPerSec = GetInt(_pInputBuffer, 0x1c, 4);
			pwfx->nBlockAlign = 2;
			pwfx->wBitsPerSample = GetInt(_pInputBuffer, 0x22, 2);
			pwfx->cbSize = 0;
			_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, this);
		}
		_sourceVoice->Start(0, 0);

		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = _remainingAudioLength;
		buf.pAudioData = _pInputBuffer + 0x2c;
		_sourceVoice->SubmitSourceBuffer(&buf);

		_audioFramesQueued = 1;

		_remainingAudioLength = 0;

		_timeOfStart = GetTickCount64();
	}

	return TRUE;
}
