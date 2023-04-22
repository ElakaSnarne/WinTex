#pragma once

#include <windows.h>
#include <mmsystem.h>
#include "LZ.h"
#include "Mutex.h"

class CMIDIPlayer
{
public:
	CMIDIPlayer();
	virtual ~CMIDIPlayer();

	void CloseDevice();
	void OpenDevice(UINT deviceId);

	virtual void Init(BinaryData data);
	void Stop();
	void Start();
	void Pause();
	void Resume();
	void SetVolume(float volume);

protected:
	static HMIDIOUT _handle;
	static BinaryData _data;

	HANDLE _hMIDIThread;
	DWORD _midiThreadId;

	static BOOL _midiEnabled;
	virtual DWORD Player();
	static DWORD WINAPI PlayerThread(LPVOID lpParameter);

	static LPBYTE _channels[16];
	static int _delays[16];
	static int _division;
	static int _duration;
	static int _volumes[16];

	static CMutex _midiMutex;
	static BOOL _changed;
};
