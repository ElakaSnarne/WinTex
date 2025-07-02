#include "PDCabinKeypadModule.h"
#include "Utilities.h"
#include "GameController.h"
#include "resource.h"

#define CABIN_KEYPAD_MODE_DEFAULT			0
#define CABIN_KEYPAD_MODE_INCORRECT			1
#define CABIN_KEYPAD_MODE_CORRECT			2
#define CABIN_KEYPAD_MODE_CORRECT_CLOSE		3

#define CABIN_KEYPAD						42
#define CABIN_KEYPAD_FILES					44

#define CABIN_KEYPAD_SOUND_BUTTON			68
#define CABIN_KEYPAD_SOUND_INVALID			69
#define CABIN_KEYPAD_SOUND_CORRECT			70

#define CABIN_KEYPAD_KEY_DELAY				100
#define CABIN_KEYPAD_FLASH_DELAY			350
#define CABIN_KEYPAD_CORRECT_DELAY			500
#define CABIN_KEYPAD_CORRECT_CLOSE_DELAY	2000

#define CABIN_KEYPAD_CORRECT_CODE		0x1482

int CPDCabinKeypadModule::CabinKeyPositions[] = { 183, 111, 250, 111, 313, 111, 377, 111, 183, 175, 250, 175, 313, 175, 377, 175, 183, 238, 250, 238, 313, 238, 377, 238, 183, 302, 250, 302, 313, 302, 377, 302 };

CPDCabinKeypadModule::CPDCabinKeypadModule() : CFullScreenModule(ModuleType::CodePanel)
{
	_enteredCode = 0;

	for (int i = 0; i < 16; i++)
	{
		_keyStates[i] = 0;
		_keyStateDirections[i] = 0;
		_keyStateUpdateTime[i] = 0;
	}

	_flashCount = 0;
	_mode = CABIN_KEYPAD_MODE_DEFAULT;
	_redBackup = 0;
}

CPDCabinKeypadModule::~CPDCabinKeypadModule()
{
}

void CPDCabinKeypadModule::Dispose()
{
}

void CPDCabinKeypadModule::Render()
{
	BOOL updateTexture = FALSE;

	ULONGLONG now = GetTickCount64();

	for (int i = 0; i < 16; i++)
	{
		if (((_keyStateDirections[i] < 0 && _keyStates[i] > 0) || (_keyStateDirections[i] > 0 && _keyStates[i] < 3)) && (now - _keyStateUpdateTime[i]) > CABIN_KEYPAD_KEY_DELAY)
		{
			_keyStates[i] += _keyStateDirections[i] < 0 ? -1 : 1;

			// Render button transition
			RenderItem(_files[4 + i * 4 + _keyStates[i]], CabinKeyPositions[i * 2], CabinKeyPositions[i * 2 + 1]);
			updateTexture = TRUE;

			_keyStateUpdateTime[i] += CABIN_KEYPAD_KEY_DELAY;

			if (_keyStates[i] == 0 || _keyStates[i] == 3)
			{
				_keyStateDirections[i] = 0;
				_sound.Play(_files[CABIN_KEYPAD_SOUND_BUTTON]);
			}
		}
	}

	if (_mode == CABIN_KEYPAD_MODE_INCORRECT)
	{
		// Should flash and beep 6 times, then clear code and re-enable input
		if ((now - _flashtime) > CABIN_KEYPAD_FLASH_DELAY)
		{
			_flashtime += CABIN_KEYPAD_FLASH_DELAY;
			_flashCount--;
			if (_flashCount == 0)
			{
				// Clear all buttons
				for (int i = 0; i < 16; i++)
				{
					_keyStates[i] = 0;
					_keyStateDirections[i] = 0;
					RenderItem(_files[4 + i * 4 + _keyStates[i]], CabinKeyPositions[i * 2], CabinKeyPositions[i * 2 + 1]);
				}

				_mode = CABIN_KEYPAD_MODE_DEFAULT;
				_inputEnabled = TRUE;
				_enteredCode = 0;
			}

			_palette[9] = _flashCount & 1 ? _redBackup : 0;

			updateTexture = TRUE;

			if (_flashCount & 1)
			{
				_sound.Play(_files[CABIN_KEYPAD_SOUND_INVALID]);
			}
		}
	}
	else if (_mode == CABIN_KEYPAD_MODE_CORRECT)
	{
		// Show message for x seconds, then close
		if ((now - _flashtime) >= CABIN_KEYPAD_CORRECT_DELAY)
		{
			RenderMessage("Code Accepted", 13);
			_sound.Play(_files[CABIN_KEYPAD_SOUND_CORRECT]);
			_mode = CABIN_KEYPAD_MODE_CORRECT_CLOSE;
			_flashtime += CABIN_KEYPAD_CORRECT_DELAY;
			updateTexture = TRUE;
		}
	}
	else if (_mode == CABIN_KEYPAD_MODE_CORRECT_CLOSE)
	{
		if ((now - _flashtime) >= CABIN_KEYPAD_CORRECT_CLOSE_DELAY)
		{
			CModuleController::Pop(this);
			return;
		}
	}

	if (updateTexture)
	{
		UpdateTexture();
	}

	RenderScreen();
}

void CPDCabinKeypadModule::Initialize()
{
	CFullScreenModule::Initialize();

	DoubleData dd = LoadDoubleEntry(L"SPECIAL.AP", CABIN_KEYPAD);
	ReadPalette(dd.File1.Data);

	_redBackup = _originalPalette[9];

	delete[] dd.File1.Data;

	_screen = dd.File2.Data;

	UpdateTexture();

	BinaryData bd = LoadEntry(L"SPECIAL.AP", CABIN_KEYPAD_FILES);
	if (bd.Data != NULL)
	{
		_data = bd.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
		}
	}

	_rawFont.Init(IDR_RAWFONT_PD);

	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_cursorPosX = static_cast<float>(w) / 2.0f;
	_cursorPosY = static_cast<float>(h) / 2.0f;

	_cursorMinX = 0;
	_cursorMaxX = w;
	_cursorMinY = 0;
	_cursorMaxY = h;

	CGameController::SetParameter(205, 0);
}

void CPDCabinKeypadModule::Cursor(float x, float y, BOOL relative)
{
	if (_inputEnabled)
	{
		CModuleBase::Cursor(x, y, relative);
	}
}

void CPDCabinKeypadModule::BeginAction()
{
	if (_inputEnabled)
	{
		int x = static_cast<int>((_cursorPosX - _left) / _scale);
		int y = static_cast<int>((_cursorPosY - _top) / _scale);

		// Check buttons
		if (x >= 580 && x <= 639 && y >= 420 && y <= 449)
		{
			// Exit
			CModuleController::Pop(this);
		}
		else if (y >= 417 && y <= 443)
		{
			if (x >= 173 && x <= 260)
			{
				// Clear
				_enteredCode = 0;

				// Clear all buttons
				for (int i = 0; i < 16; i++)
				{
					_keyStates[i] = 0;
					_keyStateDirections[i] = 0;
					RenderItem(_files[4 + i * 4 + _keyStates[i]], CabinKeyPositions[i * 2], CabinKeyPositions[i * 2 + 1]);
				}

				UpdateTexture();

				_sound.Play(_files[CABIN_KEYPAD_SOUND_BUTTON]);
			}
			else if (x >= 368 && x <= 455)
			{
				// Enter
				_sound.Play(_files[CABIN_KEYPAD_SOUND_BUTTON]);

				if (_enteredCode == CABIN_KEYPAD_CORRECT_CODE)
				{
					_mode = CABIN_KEYPAD_MODE_CORRECT;
					CGameController::SetParameter(205, 1);
				}
				else
				{
					_mode = CABIN_KEYPAD_MODE_INCORRECT;
					RenderMessage("Invalid Code", 9);
					_flashCount = 12;
				}

				UpdateTexture();
				_flashtime = GetTickCount64();
				_inputEnabled = FALSE;
			}
		}
		else if (x >= 192 && x <= 439 && y >= 111 && y <= 356)
		{
			// Key-pad area
			int kx = (x - 192) / 64;
			int ky = ((y - 111) / 64);
			int keyIndex = ky * 4 + kx;

			_enteredCode ^= (1 << keyIndex);

			_keyStateDirections[keyIndex] = (_keyStates[keyIndex] == 0) ? 1 : -1;
			_keyStateUpdateTime[keyIndex] = GetTickCount64();
		}
	}
}

void CPDCabinKeypadModule::Back()
{
	if (_inputEnabled)
	{
		CModuleController::Pop(this);
	}
}

void CPDCabinKeypadModule::RenderScreen()
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

void CPDCabinKeypadModule::RenderMessage(char* message, int colour)
{
	std::unordered_map<int, int> colourMap;
	colourMap[2] = colour;
	int w = _rawFont.Measure(message, -1);
	_rawFont.Render(_screen, 640, 480, 312 - w / 2, 39, message, colourMap, -1, -1);
}
