#include "AmbientAudio.h"
#include "ModuleController.h"
#include "GameController.h"

std::unordered_map<int, CAmbientAudio*> CAmbientAudio::Sounds;

CAmbientAudio::CAmbientAudio(BinaryData bd)
{
	_sourceVoice = NULL;
	_pData = bd.Data;
	_length = bd.Length;
}

CAmbientAudio::~CAmbientAudio()
{
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_sourceVoice->DestroyVoice();
		_sourceVoice = NULL;
	}

	if (_pData != NULL)
	{
		delete[] _pData;
		_pData = NULL;
	}

	_length = 0;
}

void CAmbientAudio::Clear()
{
	for (auto it : Sounds)
	{
		delete it.second;
	}

	Sounds.clear();
}

BOOL CAmbientAudio::Load(CMapData* mapEntry, int entry)
{
	if (Sounds.find(entry) != Sounds.end())
	{
		return TRUE;
	}

	// Entry does not exist, load the file
	if (entry < mapEntry->EnvironmentAudioMap.size())
	{
		FileMap fm = mapEntry->EnvironmentAudioMap[entry];
		std::wstring file = CGameController::GetFileName(fm.File);
		BinaryData bd = LoadEntry(file.c_str(), fm.Entry);
		if (bd.Data != NULL)
		{
			Sounds[entry] = new CAmbientAudio(bd);
			return TRUE;
		}
	}

	return FALSE;
}

void CAmbientAudio::Loop(CMapData* mapEntry, int entry1, int entry2)
{
	int entry = entry1 > 0 ? entry1 : entry2;
	entry = entry1 - 1;

	if (Load(mapEntry, entry))
	{
		Sounds[entry]->Loop();
	}
}

void CAmbientAudio::Play(CMapData* mapEntry, int entry)
{
	if (Load(mapEntry, entry))
	{
		Sounds[entry]->Play();
	}
}

void CAmbientAudio::Stop(int entry)
{
	entry--;
	if (Sounds.size() > 0 && Sounds.find(entry) != Sounds.end() && Sounds.find(entry)->second != NULL)
	{
		Sounds[entry]->Stop();
	}
}

void CAmbientAudio::StopAll()
{
	for (auto v : Sounds)
	{
		v.second->Stop();
	}

	Clear();
}

void CAmbientAudio::Loop()
{
	Play(TRUE);
}

void CAmbientAudio::Play()
{
	Play(FALSE);
}

void CAmbientAudio::Stop()
{
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop(0, 0);
	}
}

void CAmbientAudio::Play(BOOL loop)
{
	char formatBuff[64];
	WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
	pwfx->wFormatTag = GetInt(_pData, 0x14, 2);
	pwfx->nChannels = GetInt(_pData, 0x16, 2);
	pwfx->nSamplesPerSec = GetInt(_pData, 0x18, 4);
	pwfx->nAvgBytesPerSec = GetInt(_pData, 0x1c, 4);
	pwfx->nBlockAlign = 2;
	pwfx->wBitsPerSample = GetInt(_pData, 0x22, 2);
	pwfx->cbSize = 0;

	_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, NULL);// this);
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Start(0, 0);

		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = _length - 0x2c;
		buf.pAudioData = _pData + 0x2c;
		if (loop)
		{
			buf.LoopCount = XAUDIO2_LOOP_INFINITE;
		}
		_sourceVoice->SubmitSourceBuffer(&buf);
	}
}

void CAmbientAudio::Play(LPBYTE pData)
{
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Stop();
		_sourceVoice->DestroyVoice();
		_sourceVoice = NULL;
	}

	char formatBuff[64];
	WAVEFORMATEX* pwfx = reinterpret_cast<WAVEFORMATEX*>(&formatBuff);
	pwfx->wFormatTag = GetInt(pData, 0x14, 2);
	pwfx->nChannels = GetInt(pData, 0x16, 2);
	pwfx->nSamplesPerSec = GetInt(pData, 0x18, 4);
	pwfx->nAvgBytesPerSec = GetInt(pData, 0x1c, 4);
	pwfx->nBlockAlign = 2;
	pwfx->wBitsPerSample = GetInt(pData, 0x22, 2);
	pwfx->cbSize = 0;

	_sourceVoice = CDXSound::CreateSourceVoice(pwfx, 0, 1.0f, NULL);
	if (_sourceVoice != NULL)
	{
		_sourceVoice->Start(0, 0);

		int length = GetInt(pData, 0x28, 4);
		XAUDIO2_BUFFER buf = { 0 };
		buf.AudioBytes = length - 0x2c;
		buf.pAudioData = pData + 0x2c;
		_sourceVoice->SubmitSourceBuffer(&buf);
	}
}

void CAmbientAudio::SetVolume(int entry, float volume)
{
	if (Sounds.find(entry) != Sounds.end())
	{
		Sounds[entry]->SetVolume(volume);
	}
}

void CAmbientAudio::SetVolume(float volume)
{
	_sourceVoice->SetVolume(volume);
}
