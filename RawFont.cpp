#include "RawFont.h"
#include "Utilities.h"
#include "DirectX.h"

CRawFont::CRawFont()
{
	_pFontData = NULL;
	_fontDataSize = 0;
	_bitsPerPixel = 0;
	_fontHeight = 0;
}

CRawFont::CRawFont(int resource)
{
	_pFontData = NULL;
	_fontDataSize = 0;
	_bitsPerPixel = 0;
	_fontHeight = 0;

	Init(resource);
}

CRawFont::~CRawFont()
{
	if (_pFontData != NULL)
	{
		delete[] _pFontData;
	}

	_pFontData = NULL;
	_fontDataSize = 0;
	_bitsPerPixel = 0;
	_fontHeight = 0;

	_fontMap.clear();
}

void CRawFont::Init(int resource)
{
	DWORD fontDataSize;
	LPBYTE pFontData = GetResource(resource, L"BIN", &fontDataSize);
	MapAndCreateTexture(pFontData, fontDataSize);
	_pFontData = NULL;// Prevent deleting resource memory
}

void CRawFont::Init(BinaryData& data)
{
	MapAndCreateTexture(data.Data, data.Length);
}

void CRawFont::MapAndCreateTexture(LPBYTE pFontData, int fontDataSize, BOOL createTexture)
{
	if (pFontData != NULL)
	{
		_pFontData = pFontData;
		_fontDataSize = fontDataSize;
		int charCount = pFontData[0];
		_bitsPerPixel = pFontData[1];
		_fontHeight = pFontData[2];
		int widest = 0;
		for (int i = 0; i < charCount; i++)
		{
			int offset = GetInt(pFontData + 3, i * 4, 4);
			LPBYTE pChar = pFontData + offset;
			_fontMap[' ' + i] = pChar;

			// Find widest character
			if (pChar[0] > widest)
			{
				widest = pChar[0];
			}
		}

		if (createTexture)
		{
			int textureWidth = charCount * widest;
			int textureHeight = _fontHeight;

			D3D11_TEXTURE2D_DESC desc;
			desc.Width = textureWidth;
			desc.Height = textureHeight * 4;	// For now, shader only supports 4 colour fonts
			desc.MipLevels = desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.MiscFlags = 0;

			ID3D11Texture2D* pTexture = NULL;
			if (SUCCEEDED(dx.CreateTexture2D(&desc, NULL, &pTexture)))
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				if (SUCCEEDED(dx.Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
				{
					// Write to texture
					LPBYTE pData = (LPBYTE)mappedResource.pData;
					int textureOffset = 0;
					for (int i = 0; i < charCount; i++)
					{
						int offset = GetInt(pFontData + 3, i * 4, 4);
						LPBYTE pChar = pFontData + offset;
						int charWidth = *(pChar++);
						int bytesPerRow = (charWidth * _bitsPerPixel + 7) / 8;
						for (int fy = 0; fy < _fontHeight; fy++)
						{
							int bitOffset = 0;

							for (int fx = 0; fx < charWidth; fx++)
							{
								int pixel = ReadBits(pChar, _bitsPerPixel, bitOffset);
								if (pixel > 0 && pixel < 5)
								{
									pData[textureOffset + ((fy + (pixel - 1) * _fontHeight) * desc.Width + fx) * 4 + 0] = 255;
									pData[textureOffset + ((fy + (pixel - 1) * _fontHeight) * desc.Width + fx) * 4 + 1] = 255;
									pData[textureOffset + ((fy + (pixel - 1) * _fontHeight) * desc.Width + fx) * 4 + 2] = 255;
									pData[textureOffset + ((fy + (pixel - 1) * _fontHeight) * desc.Width + fx) * 4 + 3] = 255;
								}
							}

							pChar += bytesPerRow;
						}

						textureOffset += widest * 4;
					}

					dx.Unmap(pTexture, 0);
				}
			}
		}
	}
}

void CRawFont::Render(LPBYTE screen, int screenWidth, int screenHeight, int x, int y, BYTE character, int colourBase, BOOL center)
{
	LPBYTE pCharData = _fontMap[character];
	if (pCharData != NULL)
	{
		int charWidth = *(pCharData++);
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
				int pixel = ReadBits(pCharData, _bitsPerPixel, bitOffset);
				if (pixel != 0)
				{
					screen[(y + fy) * screenWidth + x + fx] = (BYTE)(pixel + colourBase);
				}
			}

			pCharData += bytesPerRow;
		}
	}
}

RECT CRawFont::Render(LPBYTE screen, int screenWidth, int screenHeight, int x, int y, char* text, std::unordered_map<int, int> colourMap, int horizontalAdjustment, int verticalAdjustment, BOOL ignoreReturn)
{
	RECT box{ x, y, 0, 0 };

	int originalX = x;
	while (*text != 0)
	{
		char character = *(text++);
		if ((character == '\r' || character == '\n') && ignoreReturn)
		{
			character = ' ';
		}

		if (character == '\r' || character == '\n')
		{
			// New line
			x = originalX;
			y += _fontHeight + verticalAdjustment;
		}
		else
		{
			LPBYTE pCharData = _fontMap[character];
			if (pCharData != NULL)
			{
				int charWidth = pCharData[0];
				int bytesPerRow = (charWidth * _bitsPerPixel + 7) / 8;
				int byteOffset = 1;
				for (int fy = 0; fy < _fontHeight; fy++)
				{
					int bitOffset = byteOffset * 8;

					for (int fx = 0; fx < charWidth; fx++)
					{
						int pixel = ReadBits(pCharData, _bitsPerPixel, bitOffset);
						int colour = colourMap[pixel];
						if (colour != 0)
						{
							screen[(y + fy) * screenWidth + x + fx] = (BYTE)(colour);
						}
					}

					byteOffset += bytesPerRow;
				}

				x += charWidth + horizontalAdjustment;
				if (x > box.right)
				{
					box.right = x;
				}
			}
		}
	}

	box.bottom = y + _fontHeight - verticalAdjustment;

	return box;
}
