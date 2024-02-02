#include "FullScreenModule.h"
#include "Utilities.h"
#include "GameController.h"

CFullScreenModule::CFullScreenModule(ModuleType type) : CModuleBase(type)
{
	_screen = NULL;

	float w = (float)dx.GetWidth();
	float h = (float)dx.GetHeight();
	float sx = w / 640.0f;
	float sy = h / 480.0f;
	_scale = min(sx, sy);
	float sw = 640.0f * _scale;
	float sh = 480.0f * _scale;

	_top = -(h - sh) / 2.0f;
	_left = (w - sw) / 2.0f;
	_bottom = _top - sh;
	_right = _left + sw;

	_cursorMinX = static_cast<int>(_left);
	_cursorMaxX = static_cast<int>(_left + 639 * _scale);
	_cursorMinY = static_cast<int>(-_top);
	_cursorMaxY = static_cast<int>(479 * _scale - _top);

	_vertexBuffer = NULL;
	_iconVertexBuffer = NULL;

	_data = NULL;

	_inputEnabled = TRUE;

	_currentPage = -1;
	_currentFrame = 0;
}

CFullScreenModule::~CFullScreenModule()
{
	Dispose();
}

void CFullScreenModule::Dispose()
{
	if (_screen != NULL)
	{
		delete[] _screen;
		_screen = NULL;
	}

	for (auto it : _files)
	{
		// TODO: Sometimes crashes, threading issue?
		//delete[] it.second;
	}

	_files.clear();

	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	if (_iconVertexBuffer != NULL)
	{
		_iconVertexBuffer->Release();
		_iconVertexBuffer = NULL;
	}

	if (_data != NULL)
	{
		delete[] _data;
		_data = NULL;
	}
}

void CFullScreenModule::Render()
{
	if (_vertexBuffer != NULL)
	{
		dx.Clear(0.0f, 0.0f, 0.0f);

		dx.DisableZBuffer();

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();
		XMMATRIX wm = XMMatrixIdentity();
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = _texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		if (_inputEnabled)
		{
			dx.SetVertexBuffers(0, 1, &_iconVertexBuffer, &stride, &offset);
			dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			wm = XMMatrixTranslation(_cursorPosX, -_cursorPosY, -0.5f);
			CConstantBuffers::SetWorld(dx, &wm);
			pRV = _iconTexture.GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			dx.Draw(4, 0);
		}

		dx.EnableZBuffer();

		dx.Present(1, 0);
	}
}

void CFullScreenModule::Initialize()
{
	CreateTexturedRectangle(_top, _left, _bottom, _right, &_vertexBuffer, "FullScreenVertexBuffer");

	_texture.Init(640, 480);
}

void CFullScreenModule::UpdateTexture()
{
	ID3D11Texture2D* pTex = _texture.GetTexture();
	if (pTex != NULL)
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(dx.Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
		{
			int* pScr = (int*)subRes.pData;
			for (int y = 0; y < 480; y++)
			{
				for (int x = 0; x < 640; x++)
				{
					pScr[y * subRes.RowPitch / 4 + x] = _palette[_screen[y * 640 + x]];
				}
			}

			dx.Unmap(pTex, 0);
		}
	}
}

void CFullScreenModule::ClearArea(int x1, int y1, int x2, int y2)
{
	Fill(x1, y1, x2, y2, 0);
}

void CFullScreenModule::Resize(int width, int height)
{
}

void CFullScreenModule::RenderItem(int entry, int offset_x, int offset_y, int x1, int x2, int y1, int y2, int transparent)
{
	RenderItem(_files[entry], offset_x, offset_y, x1, x2, y1, y2, transparent);
}

void CFullScreenModule::RenderItem(LPBYTE data, int offset_x, int offset_y, int x1, int x2, int y1, int y2, int transparent)
{
	if (x1 < 0)
	{
		x1 = 0;
	}
	if (x2 < 0)
	{
		x2 = 639;
	}
	if (y1 < 0)
	{
		y1 = 0;
	}
	if (y2 < 0)
	{
		y2 = 479;
	}

	int w = GetInt(data, 2, 2);
	int h = GetInt(data, 4, 2);
	int inPtr = 16;

	for (int y = 0; y < h; y++)
	{
		int c1 = GetInt(data, inPtr, 2);
		int c2 = GetInt(data, inPtr + 2, 2);

		int ry = offset_y + y;
		if (ry >= y1 && ry <= y2 && ry < 480)
		{
			for (int x = 0; x < w; x++)
			{
				int rx = offset_x + x;
				if (rx >= x1 && rx <= x2 && rx < 640)
				{
					int pix = (x >= c1 && x < (c1 + c2)) ? data[inPtr + 4 + x - c1] : 0;
					if (pix != transparent)
					{
						_screen[ry * 640 + offset_x + x] = pix;
					}
				}
			}
		}

		inPtr += 4 + c2;
	}
}

void CFullScreenModule::RenderRaw(int entry, int offset_x, int offset_y, int width, int height)
{
	LPBYTE pImg = _files[entry];
	int l = GetInt(_data, 6 + entry * 4, 4) - GetInt(_data, 2 + entry * 4, 4);
	BinaryData bd = CLZ::Decompress(pImg, l);
	LPBYTE pRaw = bd.Data;

	RenderRaw(bd.Data, offset_x, offset_y, width, height);

	delete pRaw;
}

void CFullScreenModule::RenderRaw(LPBYTE data, int offset_x, int offset_y, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			_screen[(offset_y + y) * 640 + offset_x + x] = data[y * width + x];
		}
	}
}

void CFullScreenModule::DrawRectangle(int x1, int y1, int x2, int y2, BYTE colour)
{
	if (x1 >= 0 && x2 < 640 && y1 >= 0 && y2 < 480)
	{
		for (int x = x1; x <= x2; x++)
		{
			_screen[y1 * 640 + x] = colour;
			_screen[y2 * 640 + x] = colour;
		}

		for (int y = y1; y <= y2; y++)
		{
			_screen[y * 640 + x1] = colour;
			_screen[y * 640 + x2] = colour;
		}
	}
}

void CFullScreenModule::Fill(int x1, int y1, int x2, int y2, BYTE colour)
{
	if (x1 >= 0 && x2 < 640 && y1 >= 0 && y2 < 480)
	{
		for (int y = y1; y <= y2; y++)
		{
			for (int x = x1; x <= x2; x++)
			{
				_screen[y * 640 + x] = colour;
			}
		}
	}
}

void CFullScreenModule::ReplaceColour(int x1, int y1, int x2, int y2, BYTE src, BYTE dst)
{
	for (int y = y1; y < y2; y++)
	{
		for (int x = x1; x < x2; x++)
		{
			if (_screen[y * 640 + x] == src)
			{
				_screen[y * 640 + x] = dst;
			}
		}
	}
}

void CFullScreenModule::FadeOut(int from, int to, int lowFrame, int highFrame)
{
	// Fade out
	int span = highFrame - lowFrame;
	for (int i = from; i < to; i++)
	{
		int c = _originalPalette[i];
		int r = (((c >> 16) & 0xff) * (highFrame - _currentFrame)) / span;
		int g = (((c >> 8) & 0xff) * (highFrame - _currentFrame)) / span;
		int b = ((c & 0xff) * (highFrame - _currentFrame)) / span;
		_palette[i] = 0xff000000 | (r << 16) | (g << 8) | b;
	}

	_frameTime = GetTickCount64();
	UpdateTexture();
	_currentFrame++;
}

void CFullScreenModule::FadeIn(int from, int to, int lowFrame, int highFrame)
{
	// Fade in
	int span = highFrame - lowFrame;
	for (int i = from; i < to; i++)
	{
		int c = _originalPalette[i];
		int r = (((c >> 16) & 0xff) * (_currentFrame - lowFrame)) / span;
		int g = (((c >> 8) & 0xff) * (_currentFrame - lowFrame)) / span;
		int b = ((c & 0xff) * (_currentFrame - lowFrame)) / span;
		_palette[i] = 0xff000000 | (r << 16) | (g << 8) | b;
	}

	_frameTime = GetTickCount64();
	UpdateTexture();
	_currentFrame++;
}

void CFullScreenModule::CheckKeyAction(int key, ControlCoordinates coordinates[], int count)
{
	// TODO: Map Virtual Key to Key Code
}

void CFullScreenModule::CheckMouseAction(ControlCoordinates coordinates[], int count)
{
	int x = static_cast<int>((_cursorPosX - _left) / _scale);
	int y = static_cast<int>((_cursorPosY - _top) / _scale);

	for (int c = 0; c < count; c++)
	{
		if (y >= coordinates[c].MinY && y <= coordinates[c].MaxY && x >= coordinates[c].MinX && x <= coordinates[c].MaxX)
		{
			OnAction(coordinates[c].Action);
			break;
		}
	}
}

void CFullScreenModule::Back()
{
	if (_inputEnabled)
	{
		CModuleController::Pop(this);
	}
}

void CFullScreenModule::ReadPalette(LPBYTE pPalette)
{
	for (int c = 0; c < 256; c++)
	{
		double r = pPalette[c * 3 + 0];
		double g = pPalette[c * 3 + 1];
		double b = pPalette[c * 3 + 2];
		int ri = (byte)((r * 255.0) / 63.0);
		int gi = (byte)((g * 255.0) / 63.0);
		int bi = (byte)((b * 255.0) / 63.0);
		int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
		_palette[c] = col;
	}
}
