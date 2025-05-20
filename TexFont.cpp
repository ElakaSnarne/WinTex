#include "TexFont.h"
#include "Utilities.h"

CTexFont::CTexFont()
{
	_pFontData = NULL;
	_fontDataSize = 0;
	_fontHeight = 0;
	_bitsPerPixel = 0;
}

CTexFont::~CTexFont()
{
	if (_pFontData != NULL)
	{
		delete[] _pFontData;
	}

	_pFontData = NULL;
	_fontDataSize = 0;
}

void CTexFont::Init(BinaryData& data)
{
	_pFontData = data.Data;
	_fontDataSize = data.Length;

	if (_pFontData != NULL)
	{
		for (int i = 0; i < _pFontData[0]; i++)
		{
			char ix = ' ' + i;
			int offset = GetInt(_pFontData + 3, i * 4, 4);
			LPBYTE pChar = _pFontData + offset;
			_fontMap[ix] = pChar;
		}
		_bitsPerPixel = _pFontData[1];
		_fontHeight = _pFontData[2];
	}
}

void CTexFont::Render(LPBYTE screen, int screenWidth, int screenHeight, int x, int y, BYTE character, int colourBase, BOOL center)
{
	LPBYTE pChar = _fontMap[character];
	if (pChar != NULL)
	{
		int charWidth = *(pChar++);
		int bytesPerRow = (charWidth * _bitsPerPixel + 7) / 8;

		if (center)
		{
			x -= charWidth / 2;
			y -= _fontHeight / 2;
		}

		for (int fy = 0; fy < _fontHeight; fy++)
		{
			int bitOffset = 0;

			for (int fx = 0; fx < charWidth; fx++)
			{
				int pixel = ReadBits(pChar, _bitsPerPixel, bitOffset);
				if (pixel != 0)
				{
					screen[(y + fy) * screenWidth + x + fx] = (BYTE)(pixel + colourBase);
				}
			}

			pChar += bytesPerRow;
		}
	}
}
