#include "SimpleAnimPlayer.h"
#include "Utilities.h"

CSimpleAnimPlayer::CSimpleAnimPlayer()
{
	_currentFrame = 0;
	_animationFrames = 0;
	_animationWidth = 0;
	_animationHeight = 0;
	_firstFrameFull = 0;
	_animationPointer = NULL;
	_dataPointer = NULL;
}

CSimpleAnimPlayer::~CSimpleAnimPlayer()
{
}

void CSimpleAnimPlayer::Init(LPBYTE pData)
{
	_lock.Lock();

	_currentFrame = 0;
	_animationFrames = GetInt(pData, 0, 2);
	_animationWidth = GetInt(pData, 2, 2);
	_animationHeight = GetInt(pData, 4, 2);
	_firstFrameFull = GetInt(pData, 7, 1);
	_animationPointer = pData + 8;
	_dataPointer = pData;

	_lock.Release();
}

void CSimpleAnimPlayer::Merge(LPBYTE pData)
{
	_lock.Lock();

	// Assuming identical animations, extract frame count and update animation pointer
	_animationFrames = GetInt(pData, 0, 2);
	_animationPointer = pData + (_animationPointer - _dataPointer);
	_dataPointer = pData;

	_lock.Release();
}

BOOL CSimpleAnimPlayer::DecodeFrame(LPBYTE pScreen, int ox, int oy, int w)
{
	_lock.Lock();

	BOOL success = FALSE;

	// Video
	if (_animationPointer != NULL)
	{
		int chunkSize = GetInt(_animationPointer, 0, 2);
		_animationPointer += 2;
		LPBYTE nextFrame = _animationPointer + chunkSize;

		if (_currentFrame == 0 && _firstFrameFull)
		{
			// Initial frame
			for (int y = 0; y < _animationHeight; y++)
			{
				for (int x = 0; x < _animationWidth; x++)
				{
					pScreen[(oy + y) * w + ox + x] = *(_animationPointer++);
				}
			}

			success = TRUE;
		}
		else if (_currentFrame < _animationFrames)
		{
			// Update frame
			int x = 0;
			int y = 0;
			while (chunkSize > 0)
			{
				int b = *(_animationPointer++);
				chunkSize--;
				if ((b & 0x80) != 0)
				{
					// Skip this many bytes
					x += (b & 0x7f);
					while (x >= _animationWidth)
					{
						y++;
						x -= _animationWidth;
					}
				}
				else
				{
					// Copy this many bytes
					for (int i = 0; i < b; i++)
					{
						pScreen[(oy + y) * w + ox + x++] = *(_animationPointer++);
						if (x >= _animationWidth)
						{
							x = 0;
							y++;
						}

						chunkSize--;
					}
				}
			}

			_animationPointer = nextFrame;
			_currentFrame++;

			if (_currentFrame == _animationFrames)
			{
				_animationPointer = NULL;
			}

			success = TRUE;
		}
	}

	_lock.Release();

	return success;
}
