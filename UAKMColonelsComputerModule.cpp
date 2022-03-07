#include "UAKMColonelsComputerModule.h"
#include "Utilities.h"
#include "GameController.h"

short _colonelsComputerRectCoords[] = { 305, 235, 314, 240,
										264, 209, 365, 272,
										103, 78, 530, 375 };

short _colonelsComputerAnimData[] = { 9, 72, 82, 6,
									9, 92, 105, 6,
									9, 112, 122, 6,
									9, 152, 165, 6,
									9, 172, 182, 6,
									9, 191, 204, 6,
									9, 232, 243, 6,
									11, 232, 243, 18,
									10, 232, 243, 18,
									11, 232, 243, 18,
									10, 232, 243, 18,
									12, 232, 243, 18,
									13, 232, 243, 18,
									12, 232, 243, 18,
									13, 232, 243, 18,
									12, 232, 243, 18,
									15, 232, 243, 18,
									14, 232, 243, 18,
									15, 232, 243, 18,
									14, 232, 243, 18,
									15, 232, 243, 18,
									9, 260, 270, 6,
									9, 280, 293, 90 };

CUAKMColonelsComputerModule::CUAKMColonelsComputerModule() : CFullScreenModule(ModuleType::ColonelsComputer)
{
	_inputEnabled = FALSE;

	_currentPage = -1;
}

CUAKMColonelsComputerModule::~CUAKMColonelsComputerModule()
{
	Dispose();
}

/*
#define IMG_HAND		0
#define IMG_ENTER_PW	1
#define IMG_PATRONAGE	2
#define IMG_INCORRECT	3
#define IMG_DOT			4
#define DAT_COORDS1		5
#define DAT_COORDS2		6
#define SND_1			7
#define SND_2			8
#define SND_3			9
#define SND_4			10
#define SND_5			11
#define SND_6			12
#define SND_7			13
*/

void CUAKMColonelsComputerModule::Render()
{
	BOOL popOnEnd = FALSE;

	DWORD delta = GetTickCount64() - _frameTime;
	if (_currentPage == 0)
	{
		// Part of the startup animation

		if (_currentFrame == 0 && delta >= 500)
		{
			DrawRectangle(_colonelsComputerRectCoords[0], _colonelsComputerRectCoords[1], _colonelsComputerRectCoords[2], _colonelsComputerRectCoords[3], 1);
			UpdateTexture();
			_frameTime = GetTickCount64();
			_currentFrame++;
		}
		else if (_currentFrame >= 1 && _currentFrame <= 3 && delta >= 166)
		{
			DrawRectangle(_colonelsComputerRectCoords[_currentFrame * 4 - 4], _colonelsComputerRectCoords[_currentFrame * 4 - 3], _colonelsComputerRectCoords[_currentFrame * 4 - 2], _colonelsComputerRectCoords[_currentFrame * 4 - 1], 0);
			if (_currentFrame < 3)
			{
				DrawRectangle(_colonelsComputerRectCoords[_currentFrame * 4], _colonelsComputerRectCoords[_currentFrame * 4 + 1], _colonelsComputerRectCoords[_currentFrame * 4 + 2], _colonelsComputerRectCoords[_currentFrame * 4 + 3], 1);
			}
			UpdateTexture();
			_frameTime = GetTickCount64();
			_currentFrame++;
			if (_currentFrame == 4)
			{
				_currentPage = 1;
				_currentFrame = 0;
			}
		}
	}
	else if (_currentPage == 1)
	{
		int requiredDelta = (_currentFrame > 0) ? _colonelsComputerAnimData[4 * _currentFrame + 3] * TIMER_SCALE : 0;
		if (delta >= requiredDelta)
		{
			int img = _colonelsComputerAnimData[4 * _currentFrame];
			int y1 = _colonelsComputerAnimData[4 * _currentFrame + 1];
			int y2 = _colonelsComputerAnimData[4 * _currentFrame + 2];
			int x = 135;
			int y = 232;
			if (img == 9)
			{
				x = 90;
				y = 67;
			}

			Render(img, x, y, 0, 639, y1, y2);
			UpdateTexture();
			_currentFrame++;
			if (_currentFrame == 23)
			{
				_currentPage = 2;
				_currentFrame = 0;
			}
			_frameTime = GetTickCount64();
		}
	}
	else if (_currentPage == 2)
	{
		if (_currentFrame > 0 || delta >= 1500)
		{
			// Flash loading
			if (_currentFrame == 0)
			{
				ClearArea(90, 67, 544, 388);
			}

			if (_currentFrame == 0 || delta >= 10 * TIMER_SCALE)
			{
				if ((_currentFrame & 1) == 0)
				{
					Render(16, 105, 80);
				}
				else
				{
					ClearArea(105, 80, 164, 93);
				}
				UpdateTexture();

				_currentFrame++;
				_frameTime = GetTickCount64();

				if (_currentFrame == 12)
				{
					_currentPage = 3;
					_currentFrame = 0;
				}
			}
		}
	}
	else if (_currentPage == 3)
	{
		// Show Perfect Word page
		if (_currentFrame == 0)
		{
			// Initialize, set colours to black
			for (int i = 250; i < 256; i++)
			{
				_palette[i] = 0xff000000;
			}

			Render(17, 143, 86);
			UpdateTexture();
			_currentFrame++;
		}
		else if (_currentFrame >= 1 && _currentFrame <= 10 && delta >= 10)
		{
			FadeIn(250, 256, 1, 10);
		}
		else if (_currentFrame == 11 && delta >= 1500)
		{
			// Wait 1.5 seconds
			_currentFrame++;
		}
		else if (_currentFrame == 12)
		{
			_currentPage = 4;
			_currentFrame = 0;
		}
	}
	else if (_currentPage == 4)
	{
		if (_currentFrame >= 0 && _currentFrame <= 9 && delta >= 10)
		{
			FadeOut(250, 256, 0, 9);
		}
		else if (_currentFrame == 10)
		{
			RenderRaw(0, 0x5a, 0x43);
			Render(5, 336, 363);	// Exit
			Render(3, 421, 363);	// Down/next
			UpdateTexture();
			_currentFrame++;
		}
		else if (_currentFrame >= 11 && _currentFrame <= 20 && delta >= 10)
		{
			FadeIn(250, 256, 11, 20);
		}
		else if (_currentFrame == 21)
		{
			_inputEnabled = TRUE;
			_currentFrame++;
		}
	}
	else if (_currentPage == 5)
	{
		if (_currentFrame >= 0 && _currentFrame <= 9 && delta >= 10)
		{
			FadeOut(250, 256, 0, 9);
		}
		else if (_currentFrame == 10)
		{
			RenderRaw(1, 0x5a, 0x43);
			Render(5, 336, 363);	// Exit
			Render(3, 421, 363);	// Down/next
			Render(7, 456, 363);	// Up/prev
			UpdateTexture();
			_currentFrame++;
		}
		else if (_currentFrame >= 11 && _currentFrame <= 20 && delta >= 10)
		{
			FadeIn(250, 256, 11, 20);
		}
		else if (_currentFrame == 21)
		{
			_inputEnabled = TRUE;
			_currentFrame++;
		}
	}
	else if (_currentPage == 6)
	{
		if (_currentFrame >= 0 && _currentFrame <= 9 && delta >= 10)
		{
			FadeOut(250, 256, 0, 9);
		}
		else if (_currentFrame == 10)
		{
			RenderRaw(2, 0x5a, 0x43);
			Render(5, 336, 363);	// Exit
			Render(7, 456, 363);	// Up/prev
			UpdateTexture();
			_currentFrame++;
		}
		else if (_currentFrame >= 11 && _currentFrame <= 20 && delta >= 10)
		{
			FadeIn(250, 256, 11, 20);
		}
		else if (_currentFrame == 21)
		{
			_inputEnabled = TRUE;
			_currentFrame++;
		}
	}
	else if (_currentPage == 7)
	{
		if (_currentFrame >= 0 && _currentFrame <= 9 && delta >= 10)
		{
			FadeOut(250, 256, 0, 9);
		}
		else if (_currentFrame == 10 && delta >= 1000)
		{
			_currentFrame++;
		}
		else if (_currentFrame >= 11 && _currentFrame <= 20 && delta >= 10)
		{
			FadeOut(0, 250, 11, 20);
		}
		else if (_currentFrame == 21)
		{
			popOnEnd = TRUE;
		}
	}

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

	if (popOnEnd)
	{
		CModuleController::Pop(this);
	}
}

void CUAKMColonelsComputerModule::Initialize()
{
	CFullScreenModule::Initialize();

	DoubleData dd = LoadDoubleEntry(L"SPECIAL.AP", 49);
	if (dd.File1.Data != NULL)
	{
		_screen = dd.File2.Data;

		LPBYTE pPal = dd.File1.Data;
		for (int c = 0; c < 256; c++)
		{
			double r = pPal[c * 3 + 0];
			double g = pPal[c * 3 + 1];
			double b = pPal[c * 3 + 2];
			int ri = (byte)((r * 255.0) / 63.0);
			int gi = (byte)((g * 255.0) / 63.0);
			int bi = (byte)((b * 255.0) / 63.0);
			int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
			_palette[c] = col;
		}

		CopyMemory(_originalPalette, _palette, sizeof(int) * 256);

		delete[] pPal;
	}

	UpdateTexture();

	// Load extra files
	dd = LoadDoubleEntry(L"SPECIAL.AP", 51);
	if (dd.File1.Data != NULL)
	{
		_data = dd.File1.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
		}

		CreateTexturedRectangle(0.0f, 0.0f, -16.0f, 16.0f, &_iconVertexBuffer, "ComputerIconVertexBuffer");
		_iconTexture.Init(dd.File2.Data, 0, 0, &_palette[0], 0, "ComputerIconTexture");

		delete dd.File2.Data;
	}

	_currentPage = 0;
	_currentFrame = 0;
	_frameTime = GetTickCount64();
}

void CUAKMColonelsComputerModule::Render(int entry, int offset_x, int offset_y, int x1, int x2, int y1, int y2)
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

	LPBYTE pImg = _files[entry];
	int w = GetInt(pImg, 2, 2);
	int h = GetInt(pImg, 4, 2);
	int inPtr = 16;

	for (int y = 0; y < h; y++)
	{
		int c1 = GetInt(pImg, inPtr, 2);
		int c2 = GetInt(pImg, inPtr + 2, 2);

		int ry = offset_y + y;
		if (ry >= y1 && ry <= y2)
		{
			for (int x = 0; x < w; x++)
			{
				int rx = offset_x + x;
				if (rx >= x1 && rx <= x2)
				{
					int pix = (x >= c1 && x < (c1 + c2)) ? pImg[inPtr + 4 + x - c1] : 0;
					_screen[ry * 640 + offset_x + x] = pix;
				}
			}
		}

		inPtr += 4 + c2;
	}
}

void CUAKMColonelsComputerModule::RenderRaw(int entry, int offset_x, int offset_y)
{
	int w = 455, h = 322;
	LPBYTE pImg = _files[entry];
	int l = GetInt(_data, 6 + entry * 4, 4) - GetInt(_data, 2 + entry * 4, 4);
	BinaryData bd = CLZ::Decompress(pImg, l);
	LPBYTE pRaw = bd.Data;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			_screen[(offset_y + y) * 640 + offset_x + x] = pRaw[y * w + x];
		}
	}

	delete pRaw;
}

void CUAKMColonelsComputerModule::BeginAction()
{
	if (_inputEnabled)
	{
		// Check if key is hit
		//LPBYTE pTest1 = _codePanelFiles[DAT_COORDS1];
		//LPBYTE pTest2 = _codePanelFiles[DAT_COORDS2];

		int x = (_cursorPosX - _left) / _scale;
		int y = (_cursorPosY - _top) / _scale;

		if (y >= 363 && y < 383)
		{
			if (x >= 336 && x < 393)
			{
				// Exit, trigger shutdown anim (fade and pop)
				_currentPage = 7;
				_currentFrame = 0;
				_inputEnabled = FALSE;
			}
			else if (x >= 421 && x < 451 && _currentPage != 6)
			{
				// Next
				_currentPage++;
				_currentFrame = 0;
				_inputEnabled = FALSE;
			}
			else if (x >= 456 && x < 486 && _currentPage != 4)
			{
				// Prev
				_currentPage--;
				_currentFrame = 0;
				_inputEnabled = FALSE;
			}
		}
	}
}
