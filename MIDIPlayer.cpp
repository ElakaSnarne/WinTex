#include "MIDIPlayer.h"
#include "Utilities.h"
#include "Configuration.h"

BOOL CMIDIPlayer::_midiEnabled = FALSE;

HMIDIOUT CMIDIPlayer::_handle = NULL;
BinaryData CMIDIPlayer::_data;

LPBYTE CMIDIPlayer::_channels[16];
int CMIDIPlayer::_delays[16];
int CMIDIPlayer::_volumes[16];

int CMIDIPlayer::_division = 0;
int CMIDIPlayer::_duration = 0;

CMutex CMIDIPlayer::_midiMutex;
BOOL CMIDIPlayer::_changed = FALSE;

CMIDIPlayer::CMIDIPlayer()
{
	_midiThreadId = 0;
	_hMIDIThread = NULL;

	_data.Data = NULL;
	_data.Length = 0;

	ZeroMemory(_channels, sizeof(LPBYTE) * 16);
	ZeroMemory(_delays, sizeof(int) * 16);
	ZeroMemory(_volumes, sizeof(int) * 16);

	OpenDevice(pConfig->MIDIDeviceId);

	_hMIDIThread = CreateThread(NULL, 1048576, PlayerThread, NULL, 0, &_midiThreadId);
}

void CMIDIPlayer::CloseDevice()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		midiOutClose(_handle);
	}
}

void CMIDIPlayer::OpenDevice(UINT deviceId)
{
	CloseDevice();
	if (midiOutOpen(&_handle, deviceId, NULL, NULL, CALLBACK_NULL) == MMSYSERR_NOERROR)
	{
		SetVolume((pConfig->MIDIVolume) / 100.0f);
	}
}

CMIDIPlayer::~CMIDIPlayer()
{
	Stop();

	if (_handle != INVALID_HANDLE_VALUE && _handle != NULL)
	{
		midiOutClose(_handle);
		_handle = NULL;
	}

	// Destroy MIDI thread
	DWORD dwWaitResult = WaitForSingleObject(_hMIDIThread, INFINITE);

	if (_data.Data != NULL)
	{
		delete _data.Data;
	}
}

void CMIDIPlayer::Init(BinaryData data)
{
	Stop();

	if (_midiMutex.Lock())
	{
		if (_data.Data != NULL)
		{
			delete _data.Data;
			_data.Data = NULL;
		}

		_changed = TRUE;
		_midiEnabled = FALSE;
		while (_changed == TRUE)
		{
			Sleep(10);
		}

		// Reset on device
		midiOutShortMsg(_handle, 0xff);

		_data = data;

		ZeroMemory(_channels, sizeof(LPBYTE) * 16);
		ZeroMemory(_delays, sizeof(int) * 16);
		ZeroMemory(_volumes, sizeof(int) * 16);

		if (data.Data != NULL)
		{
			LPBYTE pMIDI = data.Data;

			int chunkCount = GetInt(pMIDI, 0x30, 4);
			int check = GetInt(pMIDI, 0x34, 4);
			_division = GetInt(pMIDI, 0x38, 4);
			_duration = GetInt(pMIDI, 0x3c, 4);

			LPBYTE pChannel = pMIDI + 0x308;
			for (int i = 0; i < chunkCount && i < 16; i++)
			{
				int chunkNumber = GetInt(pChannel, 0, 4);
				int chunkLength = GetInt(pChannel, 4, 4);
				int trackNumber = GetInt(pChannel, 8, 4);

				// Extract initial track delay
				int div = 0, v = 0, check = 12;
				int shift = 0;
				do
				{
					v = pChannel[check++];
					div |= (v & 0x7f) << shift;
					shift += 7;
				} while ((v & 0x80) == 0);

				_delays[i] = static_cast<int>(div * TIMER_SCALE / 2);
				_channels[i] = pChannel + check;
				pChannel += chunkLength;
			}
		}

		_midiEnabled = TRUE;

		_midiMutex.Release();
	}
}

void CMIDIPlayer::Stop()
{
	if (_midiMutex.Lock())
	{
		_midiEnabled = FALSE;

		if (_handle != NULL && _handle != INVALID_HANDLE_VALUE)
		{
			for (int i = 0; i < 16; i++)
			{
				midiOutShortMsg(_handle, 0x00007bb0 | i);
			}
		}

		_midiMutex.Release();
	}
}

void CMIDIPlayer::Start()
{
	if (_midiMutex.Lock())
	{
		_midiEnabled = TRUE;
		_midiMutex.Release();
	}
}

void CMIDIPlayer::Pause()
{
	if (_midiMutex.Lock())
	{
		_midiEnabled = FALSE;

		// Set all channels' volumes to 0
		for (int i = 0; i < 16; i++)
		{
			midiOutShortMsg(_handle, 0x000007b0 | i);
		}

		_midiMutex.Release();
	}
}

void CMIDIPlayer::Resume()
{
	if (_midiMutex.Lock())
	{
		for (int i = 0; i < 16; i++)
		{
			midiOutShortMsg(_handle, 0x000007b0 | i | (_volumes[i] << 16));
		}

		_midiEnabled = TRUE;

		_midiMutex.Release();
	}
}

void CMIDIPlayer::SetVolume(float volume)
{
	MIDIHDR hdr;
	ZeroMemory(&hdr, sizeof(hdr));
	int v = static_cast<int>(0x3fff * volume);
	BYTE data[8];
	data[0] = 0xf0;
	data[1] = 0x7f;
	data[2] = 0x7f;
	data[3] = 0x04;
	data[4] = 0x01;
	data[5] = v & 0x7f;
	data[6] = (v >> 7) & 0x7f;
	data[7] = 0xf7;

	hdr.lpData = (LPSTR)data;// data"\xf0\x7f\x7f\x04\x01\x7f\x3f\xf7";	// Set master volume
	hdr.dwBufferLength = 8;
	if (midiOutPrepareHeader(_handle, &hdr, sizeof(hdr)) == MMSYSERR_NOERROR)
	{
		midiOutLongMsg(_handle, &hdr, sizeof(hdr));
	}
}

DWORD WINAPI CMIDIPlayer::PlayerThread(LPVOID lpParameter)
{
	while (_handle != NULL && _handle != INVALID_HANDLE_VALUE)
	{
		_changed = FALSE;

		if (pConfig->PlayMIDI && _midiEnabled && _data.Data != NULL)
		{
			while (_midiEnabled)
			{
				// Find shortest delay of all tracks, sleep, then execute
				int sleepTime = 1000000000;
				if (_midiMutex.Lock())
				{
					for (int i = 0; _midiEnabled && i < 16; i++)
					{
						if (_channels[i] != NULL && _delays[i] < sleepTime)
						{
							sleepTime = _delays[i];
						}
					}

					_midiMutex.Release();
				}

				if (sleepTime == 1000000000)
				{
					// Must assume end of data
					_midiEnabled = FALSE;
					_data.Data = NULL;
					break;
				}

				int remainingSleepTime = sleepTime;
				while (_midiEnabled && remainingSleepTime > 0)
				{
					int sleep = min(remainingSleepTime, 200);
					Sleep(sleep);
					remainingSleepTime -= sleep;

					if (_changed || !_midiEnabled)
					{
						break;
					}
				}

				if (_changed || !_midiEnabled)
				{
					break;
				}

				if (_midiMutex.Lock())
				{
					for (int i = 0; !_changed && _midiEnabled && i < 16; i++)
					{
						if (_channels[i] != NULL)
						{
							_delays[i] -= sleepTime;
							LPBYTE pChannel = _channels[i];
							while (pChannel != NULL && _delays[i] <= 0)
							{
								int cmd = pChannel[0];
								cmd |= pChannel[1] << 8;
								pChannel += 2;
								if ((cmd & 0xf0) <= 0xb0 || (cmd & 0xf0) >= 0xe0 || cmd == 0xff)
								{
									// One extra byte
									cmd |= pChannel[0] << 16;
									pChannel++;
								}

								if (cmd == 0x002fff)
								{
									pChannel = NULL;
									break;
								}

								midiOutShortMsg(_handle, cmd);

								if ((cmd & 0xfff0) == 0x07b0)
								{
									_volumes[cmd & 0xf] = (cmd >> 16) & 0xff;
								}

								// Calculate new delay
								int div = 0, v = 0, check = 12;
								int shift = 0;
								do
								{
									if (i == 3)
									{
										int gfrt = 0;
									}

									v = *pChannel++;
									div |= ((v & 0x7f) << shift);
									shift += 7;
								} while ((v & 0x80) == 0);
								_delays[i] = div * 8;// TIMER_SCALE / 2;
							}

							_channels[i] = pChannel;
						}
					}

					_midiMutex.Release();
				}
			}

			_changed = FALSE;
		}
		else
		{
			Sleep(200);
		}
	}

	return 0;
}
