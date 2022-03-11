#include "UAKMCodePanelModule.h"
#include "Utilities.h"
#include "LZ.h"
#include "GameBase.h"
#include "GameController.h"

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

// TODO: change to signed char, or alter constants to unsigned equivalent
BYTE CodePanelCorrectCode[] = { 18, 8, 11, 8, 2, 14, 13, -1 };

CUAKMCodePanelModule::CUAKMCodePanelModule(int parameter) : CFullScreenModule(ModuleType::CodePanel)
{
	_parameter = parameter;

	_wrongFrame = 0;
	_wrongFrameTime = 0;

	_correctFrame = 0;
	_correctFrameTime = 0;
}

CUAKMCodePanelModule::~CUAKMCodePanelModule()
{
}

void CUAKMCodePanelModule::Render()
{
	if (_correctFrame > 0)
	{
		auto diff = GetTickCount64() - _correctFrameTime;
		if (_correctFrame == 2 && diff > 500)
		{
			Render(IMG_PATRONAGE, 207, 121);
			_sound.Play(_files[SND_7]);
			UpdateTexture();
			_correctFrame--;
			_correctFrameTime = GetTickCount64();
		}
		else if (_correctFrame == 1 && diff >= 1000)
		{
			CModuleController::Pop(this);
			return;
		}
	}
	else if (_wrongFrame > 0)
	{
		if ((GetTickCount64() - _wrongFrameTime) > 166)
		{
			if ((_wrongFrame & 1) == 0)
			{
				// Play sound
				_sound.Play(_files[SND_7]);

				// Render message
				Render(IMG_INCORRECT, 243, 121);
			}
			else
			{
				ClearArea(159, 121, 477, 127);
			}

			UpdateTexture();

			_wrongFrame--;
			_wrongFrameTime = GetTickCount64();

			if (_wrongFrame == 0)
			{
				_inputEnabled = TRUE;
				ResetCode();
			}
		}
	}
	else if (_inputEnabled && _enteredCode[0] == 0xff)
	{
		int offset = static_cast<int>((GetTickCount64() - _passwordMessageTime) / 200);
		if (offset > _lastMessageOffset)
		{
			if (offset == 70)
			{
				_passwordMessageTime = GetTickCount64();
				offset = 0;
			}

			_lastMessageOffset = offset;

			LPBYTE pImg = _files[IMG_ENTER_PW];

			int w = GetInt(pImg, 2, 2);
			int h = GetInt(pImg, 4, 2);
			int inPtr = 16;

			int minX = 159;
			int maxX = 477;
			int ox = 478 - offset * 8;

			for (int y = 0; y < h; y++)
			{
				int c1 = GetInt(pImg, inPtr, 2);
				int c2 = GetInt(pImg, inPtr + 2, 2);

				int rx = ox;

				for (int x = 0; x < w; x++)
				{
					if (rx >= minX && rx <= maxX)
					{
						int pix = (x >= c1 && x < (c1 + c2)) ? pImg[inPtr + 4 + x - c1] : 0;
						_screen[(y + 121) * 640 + rx] = pix;
					}

					rx++;
				}

				inPtr += 4 + c2;
			}

			UpdateTexture();
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

		dx.SetVertexBuffers(0, 1, &_iconVertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		wm = XMMatrixTranslation(_cursorPosX, -_cursorPosY, -0.5f);
		CConstantBuffers::SetWorld(dx, &wm);
		pRV = _iconTexture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		dx.EnableZBuffer();

		dx.Present(1, 0);
	}

	bool coloursChanged = false;
	for (int i = 0xe0; i < 0xfc; i++)
	{
		if (_palette[i] > 0xff000000)
		{
			_palette[i] -= 0x000f0000;
			coloursChanged = true;
		}
	}

	if (coloursChanged)
	{
		UpdateTexture();
	}
}

void CUAKMCodePanelModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (_inputEnabled)
	{
		if (key >= 'A' && key <= 'Z')
		{
			// Enter code
			Key(static_cast<int>(key - 'A'));
		}
		else if (key == VK_RETURN)
		{
			// Try code
			Key(27);
		}
	}
}

void CUAKMCodePanelModule::Initialize()
{
	CFullScreenModule::Initialize();

	CGameController::SetParameter(_parameter, 0);

	DoubleData dd = LoadDoubleEntry(L"SPECIAL.AP", 59);
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

		delete[] pPal;
	}

	// Update palette (all keys black)
	for (int i = 0xe0; i < 0xfc; i++)
	{
		_palette[i] = 0xff000000;
	}

	_cursorMinX = static_cast<int>(_left + 2 * _scale);
	_cursorMaxX = static_cast<int>(_left + 627 * _scale);
	_cursorMinY = static_cast<int>(-_top + 236 * _scale);
	_cursorMaxY = static_cast<int>(-_top + 467 * _scale);

	UpdateTexture();

	// Load extra files
	BinaryData bd = LoadEntry(L"SPECIAL.AP", 61);
	if (bd.Data != NULL)
	{
		_data = bd.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
		}
	}

	ResetCode();

	CreateTexturedRectangle(0.0f, 0.0f, -16.0f, 16.0f, &_iconVertexBuffer, "CodePanelIconVertexBuffer");
	_iconTexture.Init(_files[IMG_HAND], 0, 0, &_palette[0], 0, "CodePanelIconTexture");
}

void CUAKMCodePanelModule::RenderDot(int pos)
{
	if (pos >= 0 && pos < 7)
	{
		int x = 160 + pos * 51;
		int y = 183;

		Render(IMG_DOT, x, y);
	}
}

void CUAKMCodePanelModule::Key(int key)
{
	_palette[0xe0 + key] = 0xffff0000;

	if (key < 26)
	{
		if (_keyPos < 8)
		{
			RenderDot(_keyPos);
			_enteredCode[_keyPos++] = key;

			ClearArea(159, 121, 477, 127);

			UpdateTexture();
		}
	}
	else if (key == 26)
	{
		ResetCode();
		UpdateTexture();
	}
	else if (key == 27)
	{
		bool correct = true;
		for (int i = 0; i < 8; i++)
		{
			if (_enteredCode[i] != CodePanelCorrectCode[i])
			{
				correct = false;
				break;
			}
		}

		_inputEnabled = FALSE;
		if (correct)
		{
			CGameController::SetParameter(_parameter, 1);
			_correctFrame = 2;
			_correctFrameTime = GetTickCount64();
		}
		else
		{
			_wrongFrame = 13;
			_wrongFrameTime = GetTickCount64();
		}
	}

	// Play sound
	_sound.Play(_files[7 + (key % 6)]);
}

void CUAKMCodePanelModule::ResetCode()
{
	for (int i = 0; i < 8; i++)
	{
		_enteredCode[i] = -1;
	}

	_keyPos = 0;

	ClearArea(160, 183, 476, 193);

	_passwordMessageTime = GetTickCount64();
	_lastMessageOffset = -1;
}

void CUAKMCodePanelModule::Render(int entry, int offset_x, int offset_y)
{
	LPBYTE pImg = _files[entry];

	int w = GetInt(pImg, 2, 2);
	int h = GetInt(pImg, 4, 2);
	int inPtr = 16;

	for (int y = 0; y < h; y++)
	{
		int c1 = GetInt(pImg, inPtr, 2);
		int c2 = GetInt(pImg, inPtr + 2, 2);

		for (int x = 0; x < w; x++)
		{
			int pix = (x >= c1 && x < (c1 + c2)) ? pImg[inPtr + 4 + x - c1] : 0;
			_screen[(y + offset_y) * 640 + offset_x + x] = pix;
		}

		inPtr += 4 + c2;
	}
}

void CUAKMCodePanelModule::BeginAction()
{
	if (_inputEnabled)
	{
		// Check if key is hit
		LPBYTE pTest1 = _files[DAT_COORDS1];
		LPBYTE pTest2 = _files[DAT_COORDS2];

		int x = static_cast<int>((_cursorPosX - _left) / _scale);
		int y = static_cast<int>((_cursorPosY - _top) / _scale);

		LPBYTE scan = pTest2;
		int i = 0;
		int hit = -1;
		while (*scan != 0xff)
		{
			int y1 = GetInt(scan, 2, 2);
			int y2 = GetInt(scan, 4, 2);
			int x1 = GetInt(scan, 6, 2);
			int x2 = GetInt(scan, 8, 2);

			if (x >= x1 && x < x2 && y >= y1 && y < y2)
			{
				Trace(L"Clicked on button ");
				Trace(i);
				Trace(L" with ID ");
				hit = GetInt(scan, 10, 4);
				TraceLine(hit);
				break;
			}

			i++;
			scan += 14;
		}

		if (hit == 0x500)
		{
			CGameController::SetParameter(_parameter, 2);
			CModuleController::Pop(this);
		}
		else if (hit >= 0)
		{
			if (hit < 26)
			{
				Key(hit);
			}
			else
			{
				if (hit < 29)
				{
					// Cancel
					Key(26);
				}
				else
				{
					// Enter
					Key(27);
				}
			}
		}
	}
}
