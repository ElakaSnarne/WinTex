#pragma once

#include "Globals.h"
#include "D3D11-NoWarn.h"
#include <dsound.h>
#include "LZ.h"
#include <xaudio2.h>
#include "DirectX.h"
#include "Texture.h"
#include "Mutex.h"

class CAnimBase : public CDXBase, public IXAudio2VoiceCallback
{
public:
	CAnimBase();
	~CAnimBase();

	virtual BOOL Init(LPBYTE pData, int length);
	BOOL Init(BinaryData bd);

	void Render();
	virtual void Update();

	virtual BOOL IsWave() { return FALSE; }
	virtual BOOL HasVideo() { return TRUE; }

	virtual BOOL ShouldClearDXBuffer() { return TRUE; }

	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) { }
	STDMETHOD_(void, OnVoiceProcessingPassEnd)() { }
	STDMETHOD_(void, OnStreamEnd)() { }
	STDMETHOD_(void, OnBufferStart)(void*) { }
	STDMETHOD_(void, OnBufferEnd)(void*)
	{
		_audioFramesProcessed++;

		if (_lock.Lock())
		{
			if (_sourceVoice != NULL)
			{
				if (_audioBuffers.size() > 0)
				{
					// Enqueue the next buffer
					Buffer ab = _audioBuffers.front();
					_audioBuffers.pop_front();

					XAUDIO2_BUFFER buf = { 0 };
					buf.AudioBytes = ab.Size;
					buf.pAudioData = ab.pData;
					_sourceVoice->SubmitSourceBuffer(&buf);

					_audioFramesQueued++;
				}
			}

			_lock.Release();
		}
	}
	STDMETHOD_(void, OnLoopEnd)(void*) { }
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT) { }

	virtual BOOL IsDone() { return _done; }
	virtual void Skip();
	int Frame() { return _frame; }

	int Width() { return _width; }
	int Height() { return _height; }

	void Resize(int width, int height);

protected:
	std::list<Buffer> _audioBuffers;
	LPBYTE _pInputBuffer;
	LPBYTE _pVideoOutputBuffer;
	int _inputBufferLength;
	int _videoFramePointer;
	int _audioFramePointer;
	LPINT _pPalette;

	int _width;
	int _height;
	int _rate;
	int _frameTime;
	int _frame;

	ULONGLONG _lastFrameUpdate;

	virtual BOOL DecodeFrame() { return FALSE; }
	int _framePointer;

	virtual void CreateBuffers(int width, int height, int factor = 1);

	ID3D11Buffer* _vertexBuffer;

	IXAudio2SourceVoice* _sourceVoice;
	int _remainingAudioLength;

	int _audioFramesQueued;
	int _audioFramesProcessed;
	int _videoFramesProcessed;

	int _screenWidth;
	int _screenHeight;

	CTexture _texture;

	BOOL _done;

	BYTE _colourTranslationTable[64];

	CMutex _lock;
};
