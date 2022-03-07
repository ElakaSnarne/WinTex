#pragma once

#include "DXBase.h"
#include "DirectX.h"

class CTexture : public CDXBase
{
public:
	CTexture();
	CTexture(CDirectX* pDX, int w, int h, PBYTE input, PINT palette, int rotate);

	~CTexture();

	BOOL Init(int width, int height);
	BOOL Init(PWSTR file);
	BOOL Init(PBYTE pImage, DWORD size, char* name);
	BOOL Init(PBYTE pImage, DWORD size, DWORD offset, PINT pPalette, int transparentIndex, char* name);
	void Dispose();

	ID3D11Texture2D* GetTexture() { return _texture; }
	ID3D11ShaderResourceView* GetTextureRV() { return _textureRV; }

	int Width() { return _width; }
	int Height() { return _height; }

protected:
	int _width;
	int _height;

	ID3D11Texture2D* _texture;
	ID3D11ShaderResourceView* _textureRV;
};
