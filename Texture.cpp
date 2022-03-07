#include "Texture.h"
#include "Globals.h"
#include "Utilities.h"

CTexture::CTexture()
{
	_width = 0;
	_height = 0;

	_texture = NULL;
	_textureRV = NULL;
}

CTexture::CTexture(CDirectX* pDX, int w, int h, PBYTE input, PINT palette, int rotate)
{
	_texture = NULL;
	_textureRV = NULL;

	if (rotate != 0)
	{
		int t = w;
		w = h;
		h = t;
	}

	_width = w;
	_height = h;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	if (SUCCEEDED(dx.CreateTexture2D(&desc, NULL, &_texture)))
	{
		SetDebugName(_texture, "Texture 1");

		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(pDX->Map(_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
		{
			int* pTex = (int*)subRes.pData;
			if (rotate == 0)
			{
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						pTex[y * subRes.RowPitch / 4 + x] = palette[input[y * _width + x]];
					}
				}
			}
			else
			{
				for (int y = 0; y < h; y++)
				{
					for (int x = 0; x < w; x++)
					{
						pTex[y * subRes.RowPitch / 4 + x] = palette[input[x * _height + y]];
					}
				}
			}

			pDX->Unmap(_texture, 0);
		}

		if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
		{
			SetDebugName(_textureRV, "TextureRV 1");
		}
		else
		{
			int debug = 0;
		}
	}
	else
	{
		int debug = 0;
	}
}

CTexture::~CTexture()
{
	if (_textureRV != NULL) _textureRV->Release();
	_textureRV = NULL;

	if (_texture != NULL)_texture->Release();
	_texture = NULL;
}

BOOL CTexture::Init(int width, int height, D3D11_USAGE usageFlags, DWORD miscFlags, ID3D11Device* pD3D)
{
	Dispose();

	BOOL ret = FALSE;

	_width = width;
	_height = height;

	if (width > 0 && height > 0)
	{
		// Create buffer and shader resource view
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = usageFlags;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = miscFlags;

		if (pD3D == NULL)
		{
			if (SUCCEEDED(dx.CreateTexture2D(&desc, NULL, &_texture)))
			{
				SetDebugName(_texture, "Texture 2");
				if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
				{
					SetDebugName(_textureRV, "TextureRV 2");
					ret = TRUE;
				}
			}
		}
		else
		{
			if (SUCCEEDED(pD3D->CreateTexture2D(&desc, NULL, &_texture)))
			{
				if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
				{
					SetDebugName(_textureRV, "TextureRV 2");
					ret = TRUE;
				}
			}
		}
	}

	return ret;
}

BOOL CTexture::Init(PWSTR file)
{
	Dispose();

	BOOL ret = FALSE;

	D3DX11_IMAGE_LOAD_INFO li;
	ZeroMemory(&li, sizeof(li));
	li.MipLevels = 1;
	li.Usage = D3D11_USAGE_STAGING;
	if (SUCCEEDED(D3DX11CreateTextureFromFile(dx.GetDevice(), file, NULL, NULL, (ID3D11Resource**)&_texture, NULL)))
	{
		SetDebugName(_texture, "Texture 3");

		D3D11_TEXTURE2D_DESC desc;
		_texture->GetDesc(&desc);
		_width = desc.Width;
		_height = desc.Height;

		if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
		{
			SetDebugName(_textureRV, "TextureRV 3");
			ret = TRUE;
		}
	}

	return ret;
}

BOOL CTexture::Init(PBYTE pImage, DWORD size, char* name)
{
	Dispose();

	BOOL ret = FALSE;

	D3DX11_IMAGE_LOAD_INFO li;
	ZeroMemory(&li, sizeof(li));
	li.MipLevels = 1;
	li.Usage = D3D11_USAGE_STAGING;
	if (SUCCEEDED(D3DX11CreateTextureFromMemory(dx.GetDevice(), pImage, size, NULL, NULL, (ID3D11Resource**)&_texture, NULL)))
	{
		SetDebugName(_texture, name);

		D3D11_TEXTURE2D_DESC desc;
		_texture->GetDesc(&desc);
		_width = desc.Width;
		_height = desc.Height;

		if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
		{
			SetDebugName(_textureRV, name);
			ret = TRUE;
		}
	}

	return ret;
}

BOOL CTexture::Init(PBYTE pData, DWORD size, DWORD offset, PINT pPalette, int transparentIndex, char* name, int sx, int sy, int sw, int sh, bool rawImage, int rawWidth, int rawHeight)
{
	Dispose();

	BOOL ret = FALSE;
	PBYTE pImage = pData + offset;

	int imageHeight = rawImage ? rawHeight : GetInt(pImage, 4, 2);
	int width = sw < 0 ? rawImage ? rawWidth : GetInt(pImage, 2, 2) : sw;
	int height = sh < 0 ? rawImage ? rawHeight : imageHeight : sh;

	_width = width;
	_height = height;

	if (width > 0 && height > 0)
	{
		// Create buffer and shader resource view
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		if (SUCCEEDED(dx.CreateTexture2D(&desc, NULL, &_texture)))
		{
			SetDebugName(_texture, name);

			D3D11_MAPPED_SUBRESOURCE subRes;
			if (SUCCEEDED(dx.Map(_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
			{
				int inPtr = 16;
				int* pTex = (int*)subRes.pData;
				ZeroMemory(pTex, height * subRes.RowPitch);

				if (rawImage)
				{
					inPtr += rawWidth * sy - 16;
					for (int y = 0; y < height; y++)
					{
						for (int x = 0; x < width; x++)
						{
							int pix = pImage[inPtr + sx + x];
							pTex[y * subRes.RowPitch / 4 + x] = (pix != transparentIndex) ? pPalette[pix] : 0;
						}

						inPtr += rawWidth;
					}
				}
				else
				{
					for (int y = 0; y < imageHeight; y++)
					{
						int c1 = GetInt(pImage, inPtr, 2);
						int c2 = GetInt(pImage, inPtr + 2, 2);

						if (y >= sy && y < (sy + height))
						{
							for (int x = 0; x < width; x++)
							{
								int pix = (x >= c1 && x < (c1 + c2)) ? pImage[inPtr + 4 + x - c1] : 0;
								pTex[(y - sy) * subRes.RowPitch / 4 + x + sx] = (pix != transparentIndex) ? pPalette[pix] : 0;
							}
						}

						inPtr += 4 + c2;
					}
				}

				dx.Unmap(_texture, 0);

				if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
				{
					SetDebugName(_textureRV, name);
					ret = TRUE;
				}
			}
		}
	}

	return ret;
}

void CTexture::Dispose()
{
	if (_texture != NULL)
	{
		_texture->Release();
		_texture = NULL;
	}

	if (_textureRV != NULL)
	{
		_textureRV->Release();
		_textureRV = NULL;
	}
}
