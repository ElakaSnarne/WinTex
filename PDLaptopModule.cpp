#include "PDLaptopModule.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "resource.h"

#define LAPTOP_PUZZLE			74
#define LAPTOP_NO_CD			79
#define LAPTOP_BOOT_ANIM		81
#define LAPTOP_ARTICLES			82

#define LAPTOP_MODE_BOOTING		0
#define LAPTOP_MODE_SEARCHING	1
#define LAPTOP_MODE_NOT_FOUND	2
#define LAPTOP_MODE_TRANSITION	3
#define LAPTOP_MODE_ARTICLES	4

#define DOT_TIME				250
#define TRANSITION_TIME			350

int CPDLaptopModule::CellSequence[] = { 0, 6, 8, 15, 2, 9, 4, 13, 3, 11, 5, 14, 10, 12, 7, 1 };

CPDLaptopModule::CPDLaptopModule(BOOL cdUsed) : CFullScreenModule(ModuleType::Laptop)
{
	_cdUsed = cdUsed;

	_mode = cdUsed ? LAPTOP_MODE_TRANSITION : LAPTOP_MODE_BOOTING;

	_stage = cdUsed ? 16 : 0;
	_time = 0;

	_inputEnabled = FALSE;
}

CPDLaptopModule::CPDLaptopModule(LPBYTE screen, LPINT palette) : CFullScreenModule(ModuleType::Laptop)
{
	_screen = screen;
	CopyMemory(_palette, palette, 256 * 4);

	_cdUsed = TRUE;
	_mode = LAPTOP_MODE_TRANSITION;

	_stage = 0;
	_time = GetTickCount64();

	_inputEnabled = FALSE;
}

CPDLaptopModule::~CPDLaptopModule()
{
}

void CPDLaptopModule::Render()
{
	if (_mode == LAPTOP_MODE_BOOTING)
	{
		if (CAnimationController::Exists())
		{
			CAnimationController::SetOutputBuffer(_screen, 640, 480, 1, -96, _palette, 0, 256);
			if (!CAnimationController::UpdateAndRender() && CAnimationController::IsDone())
			{
				_mode = LAPTOP_MODE_SEARCHING;
				std::unordered_map<int, int> colourMap;
				colourMap[2] = 31;
				RECT rc = _pdRawFont.Render(_screen, 640, 480, 170, 58, "Searching for CD", colourMap, -1, -1, TRUE);
				_dotX = rc.right + 3;
				_time = GetTickCount64();
			}

			UpdateTexture();
		}
	}
	else if (_mode == LAPTOP_MODE_SEARCHING)
	{
		// Print 10 dots while searching for CD
		if ((GetTickCount64() - _time) > DOT_TIME)
		{
			_time += DOT_TIME;
			std::unordered_map<int, int> colourMap;
			colourMap[2] = 31;
			_pdRawFont.Render(_screen, 640, 480, _dotX, 58, ".", colourMap);
			_dotX += 4;

			if (++_stage == 10)
			{
				_mode = LAPTOP_MODE_NOT_FOUND;
				_pdRawFont.Render(_screen, 640, 480, 170, 78, "CD not found.", colourMap, -1, -1, TRUE);

				// Render exit button
				RenderButton(433, 220, 474, 244, "Exit", 9, 13);

				// Limit mouse to box
				_cursorMinX = 432;
				_cursorMaxX = 472;
				_cursorMinY = 220;
				_cursorMaxY = 244;

				_cursorPosX = 472;
				_cursorPosY = 244;

				_inputEnabled = TRUE;
			}

			UpdateTexture();
		}
	}
	else if (_mode == LAPTOP_MODE_TRANSITION)
	{
		// Remove some cells, then turn the correct cells orange
		ULONGLONG now = GetTickCount64();
		if (_time < now && (now - _time) > TRANSITION_TIME)
		{
			if (_stage == 16)
			{
				_mode = LAPTOP_MODE_ARTICLES;
				_stage = 0;

				// Remove Exit button
				Fill(186, 247, 237, 270, 0);

				RenderArticleButtons();

				RenderButton(449, 248, 490, 270, "Exit", 0, 13);

				_inputEnabled = TRUE;
			}
			else
			{
				// Update cells
				int cellToClear = CellSequence[_stage++];
				int cellX = cellToClear & 3;
				int cellY = cellToClear / 4;
				int x = 121 + cellX * 46;
				int y = 53 + cellY * 46;
				Fill(x, y, x + 44, y + 44, (_stage > 12) ? 26 : 0);

				_time += (_stage == 16) ? TRANSITION_TIME * 6 : TRANSITION_TIME;
			}

			UpdateTexture();
		}
	}
	else if (_mode == LAPTOP_MODE_ARTICLES)
	{
		// TODO: Bother with fade in/out?
	}

	RenderScreen();
}

void CPDLaptopModule::Initialize()
{
	CFullScreenModule::Initialize();

	if (_screen == NULL)
	{
		DoubleData dd = LoadDoubleEntry(L"SPECIAL.AP", _cdUsed ? LAPTOP_PUZZLE : LAPTOP_NO_CD);
		ReadPalette(dd.File1.Data);

		delete[] dd.File1.Data;

		_screen = dd.File2.Data;
	}

	if (_mode == LAPTOP_MODE_TRANSITION || _cdUsed)
	{
		// Load article files
		BinaryData bd = LoadEntry(L"SPECIAL.AP", LAPTOP_ARTICLES);
		if (bd.Data != NULL)
		{
			_data = bd.Data;
			int count = GetInt(_data, 0, 2) - 1;
			for (int i = 0; i < count; i++)
			{
				_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
			}
		}
	}

	_pdRawFont.Init(IDR_RAWFONT_PD);

	UpdateTexture();

	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_cursorPosX = static_cast<float>(w) / 2.0f;
	_cursorPosY = static_cast<float>(h) / 2.0f;

	_cursorMinX = 0;
	_cursorMaxX = w;
	_cursorMinY = 0;
	_cursorMaxY = h;

	if (!_cdUsed)
	{
		CAnimationController::Load(L"SPECIAL.AP", LAPTOP_BOOT_ANIM);
	}
}

void CPDLaptopModule::Cursor(float x, float y, BOOL relative)
{
	if (_inputEnabled)
	{
		CModuleBase::Cursor(x, y, relative);

		int x = static_cast<int>((_cursorPosX - _left) / _scale);
		int y = static_cast<int>((_cursorPosY - _top) / _scale);

		DrawRectangle(193, 248, 239, 270, (_stage != 0 && x >= 193 && x < 239 && y >= 248 && y < 270) ? 9 : 0);
		DrawRectangle(321, 248, 368, 270, (_stage != 4 && x >= 321 && x < 368 && y >= 248 && y < 270) ? 9 : 0);
		DrawRectangle(449, 248, 490, 270, (x >= 449 && x < 490 && y >= 248 && y < 270) ? 9 : 0);
		UpdateTexture();
	}
}

void CPDLaptopModule::BeginAction()
{
	if (_inputEnabled)
	{
		if (_mode == LAPTOP_MODE_NOT_FOUND)
		{
			CModuleController::Pop(this);
		}
		else if (_mode == LAPTOP_MODE_ARTICLES)
		{
			// Check if prev/next/exit has been clicked
			int x = static_cast<int>((_cursorPosX - _left) / _scale);
			int y = static_cast<int>((_cursorPosY - _top) / _scale);

			if (_stage != 0 && x >= 193 && x < 239 && y >= 248 && y < 270)
			{
				// Previous
				_stage--;
				RenderArticleButtons();
				UpdateTexture();
			}
			else if (_stage != 4 && x >= 321 && x < 368 && y >= 248 && y < 270)
			{
				// Next
				_stage++;
				RenderArticleButtons();
				UpdateTexture();
			}
			else if (x >= 449 && x < 490 && y >= 248 && y < 270)
			{
				// Exit
				CModuleController::Pop(this);
			}
		}
	}
}

void CPDLaptopModule::Back()
{
	CModuleController::Pop(this);
}

void CPDLaptopModule::RenderScreen()
{
	if (_vertexBuffer != NULL)
	{
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
			CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
			CModuleController::Cursors[0].Render();
		}

		dx.EnableZBuffer();
	}
}

void CPDLaptopModule::RenderButton(int x1, int y1, int x2, int y2, char* text, int boxColour, int textColour)
{
	DrawRectangle(x1, y1, x2, y2, boxColour);
	std::unordered_map<int, int> colourMap;
	colourMap[2] = textColour;
	int pixels = _pdRawFont.Measure(text, -1);
	_pdRawFont.Render(_screen, 640, 480, (x1 + x2 - pixels) / 2, (y1 + y2 - _pdRawFont.GetHeight()) / 2 + 2, text, colourMap, -1, -1, TRUE);
}

void CPDLaptopModule::RenderArticleButtons()
{
	int x = static_cast<int>((_cursorPosX - _left) / _scale);
	int y = static_cast<int>((_cursorPosY - _top) / _scale);

	if (_stage != 0)
	{
		RenderButton(193, 248, 239, 270, "Prev", (_stage != 0 && x >= 193 && x < 239 && y >= 248 && y < 270) ? 9 : 0, 13);
	}
	else
	{
		Fill(193, 248, 239, 270, 0);
	}

	if (_stage != 4)
	{
		RenderButton(321, 248, 368, 270, "Next", (_stage != 4 && x >= 321 && x < 368 && y >= 248 && y < 270) ? 9 : 0, 13);
	}
	else
	{
		Fill(321, 248, 368, 270, 0);
	}

	// Apply partial palette
	LPBYTE pPalette = _files[_stage * 2];
	for (int c = 0xc0; c < 0x100; c++)
	{
		double r = pPalette[c * 3 + 0];
		double g = pPalette[c * 3 + 1];
		double b = pPalette[c * 3 + 2];
		int ri = (byte)((r * 255.0) / 63.0);
		int gi = (byte)((g * 255.0) / 63.0);
		int bi = (byte)((b * 255.0) / 63.0);
		int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
		_palette[c] = col;
		_originalPalette[c] = col;
	}

	Fill(108, 14, 532, 246, 0);
	LPBYTE pCompressed = _files[_stage * 2 + 1];
	BinaryData bd = CLZ::Decompress(pCompressed, GetInt(pCompressed, 4, 4));
	if (_stage == 0)
	{
		RenderItem(bd.Data, 120, 52);
	}
	else
	{
		RenderItem(bd.Data, 160, 14);
	}
	delete[] bd.Data;
}
