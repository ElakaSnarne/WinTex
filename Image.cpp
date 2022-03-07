#include "Image.h"

CImage::CImage(DoubleData dd, int width, int height, int factor)
{
	Init(dd.File1.Data, dd.File2, width, height, factor);
}

CImage::CImage(LPBYTE palette, BinaryData bd, int width, int height, int factor)
{
	Init(palette, bd, width, height, factor);

	CopyMemory(_pVideoOutputBuffer, bd.Data, min(width*height, bd.Length));

	//// Delete palette
	//delete[] dd.File1.Data;

	_framePointer = 1;
}

CImage::~CImage()
{
}

void CImage::Update()
{
	CAnimBase::Update();
	_done = FALSE;
}

void CImage::Init(LPBYTE palette, BinaryData bd, int width, int height, int factor)
{
	CAnimBase::Init(bd);

	_width = width;
	_height = height;

	// Copy palette
	for (int c = 0; c < 256; c++)
	{
		double r = palette[c * 3 + 0];
		double g = palette[c * 3 + 1];
		double b = palette[c * 3 + 2];
		int ri = (byte)((r * 255.0) / 63.0);
		int gi = (byte)((g * 255.0) / 63.0);
		int bi = (byte)((b * 255.0) / 63.0);
		int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
		_pPalette[c] = col;
	}

	// Create image buffer
	CreateBuffers(width, height, factor);
	_texture.Init(_width, _height);

	CopyMemory(_pVideoOutputBuffer, bd.Data, min(width*height, bd.Length));

	// Delete palette
	//delete[] dd.File1.Data;

	_framePointer = 1;

	// Replace texture if new video frame is required (frame time has lapsed, video frame exists)
	ID3D11Texture2D* pTex = _texture.GetTexture();
	if (pTex != NULL)
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(dx.Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
		{
			int* pScr = (int*)subRes.pData;
			for (int y = 0; y < _height; y++)
			{
				for (int x = 0; x < _width; x++)
				{
					pScr[y * subRes.RowPitch / 4 + x] = _pPalette[_pVideoOutputBuffer[y * _width + x]];
				}
			}

			dx.Unmap(pTex, 0);
		}
		else
		{
			int debug = 0;
		}
	}
}
