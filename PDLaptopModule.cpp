#include "PDLaptopModule.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "resource.h"

#define LAPTOP_PUZZLE			74
#define LAPTOP_NO_CD			79
#define LAPTOP_BOOT_ANIM		81

#define LAPTOP_MODE_BOOTING		0
#define LAPTOP_MODE_SEARCHING	1
#define LAPTOP_MODE_NOT_FOUND	2

CPDLaptopModule::CPDLaptopModule(BOOL cdUsed) : CFullScreenModule(ModuleType::Laptop)
{
	_cdUsed = cdUsed;

	_mode = LAPTOP_MODE_BOOTING;

	_dots = 0;
	_dotTime = 0;

	_inputEnabled = FALSE;
}

CPDLaptopModule::~CPDLaptopModule()
{
}

void CPDLaptopModule::Resize(int width, int height)
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
				_dotTime = GetTickCount64();
			}

			UpdateTexture();
		}
	}
	else if (_mode == LAPTOP_MODE_SEARCHING)
	{
		// Print 10 dots while searching for CD
		if ((GetTickCount64() - _dotTime) > 250)
		{
			_dotTime += 250;
			std::unordered_map<int, int> colourMap;
			colourMap[2] = 31;
			_pdRawFont.Render(_screen, 640, 480, _dotX, 58, ".", colourMap);
			_dotX += 4;

			if (++_dots == 10)
			{
				_mode = LAPTOP_MODE_NOT_FOUND;
				_pdRawFont.Render(_screen, 640, 480, 170, 78, "CD not found.", colourMap, -1, -1, TRUE);

				// Render exit button
				DrawRectangle(432, 220, 473, 244, 9);
				colourMap[2] = 13;
				_pdRawFont.Render(_screen, 640, 480, 440, 226, "Exit", colourMap, -1, -1, TRUE);

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

	RenderScreen();
}

void CPDLaptopModule::Initialize()
{
	CFullScreenModule::Initialize();

	DoubleData dd = LoadDoubleEntry(L"SPECIAL.AP", LAPTOP_NO_CD);
	ReadPalette(dd.File1.Data);
	for (int i = 0; i < 256; i++)
	{
		_originalPalette[i] = _palette[i];
	}

	delete[] dd.File1.Data;

	_screen = dd.File2.Data;

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

	CAnimationController::Load(L"SPECIAL.AP", LAPTOP_BOOT_ANIM);
}

void CPDLaptopModule::Cursor(float x, float y, BOOL relative)
{
	if (_inputEnabled)
	{
		CModuleBase::Cursor(x, y, relative);
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
