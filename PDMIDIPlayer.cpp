#include "PDMIDIPlayer.h"
#include "Utilities.h"
#include "Configuration.h"

void CPDMIDIPlayer::Init(BinaryData data)
{
	// Track/Channel list at 0x172
	// Assuming max 16 channels
	// Supposed to support looping and conditional branching
	// Number of tracks @ 0xE4

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

			int channelCount = GetInt(pMIDI, 0xe4, 4);
			int check = GetInt(pMIDI, 0x34, 4);
			_division = GetInt(pMIDI, 0xd2, 2);
			_duration = GetInt(pMIDI, 0xd4, 4);

			//LPBYTE pChannel = pMIDI + 0x308;
			for (int i = 0; i < channelCount && i < 16; i++)
			{
				_channels[i] = pMIDI + GetInt(pMIDI, 0x172 + i * 4, 4);
				_channels[i] += GetInt(_channels[i], 0x57, 4);

				//int channelNumber = GetInt(pChannel, 0, 4);
				//int channelLength = GetInt(pChannel, 4, 4);
				//int trackNumber = GetInt(pChannel, 8, 4);

				// Extract initial track delay
				//int div = 0, v = 0, check = 12;
				//int shift = 0;
				//do
				//{
				//	v = pChannel[check++];
				//	div |= (v & 0x7f) << shift;
				//	shift += 7;
				//} while ((v & 0x80) == 0);
				//
				//_delays[i] = static_cast<int>(div * TIMER_SCALE / 2);
				//_channels[i] = pChannel + check;
				//pChannel += channelLength;
			}
		}

		_midiEnabled = TRUE;

		_midiMutex.Release();
	}
}

DWORD CPDMIDIPlayer::Player()
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
									v = *pChannel++;
									div |= ((v & 0x7f) << shift);
									shift += 7;
								} while ((v & 0x80) == 0);
								_delays[i] = div * 8;
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
