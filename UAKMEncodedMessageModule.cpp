#include "UAKMEncodedMessageModule.h"
#include "Globals.h"
#include "GameController.h"
#include "Utilities.h"
#include "UAKMGame.h"
#include "resource.h"

char* pMsg = "YV UZNV SIAKWBHVG RIPB ZEEIWALHVAL  YWLU SUZXLWLR ZL  LUV XPWLV WA LUV  CIOGVA CZLV UILVO ZL LUV PXPZO LWHV.LUV EZXXYIBG LIGZRWX XWOWSIA.";
char* pDec = "WE HAVE CONFIRMED YOUR APPOINTMENT  WITH CHASTITY AT  THE SUITE IN THE  GOLDEN GATE HOTEL AT THE USUAL TIME.THE PASSWORD TODAYIS SILICON.";

int LineOffsets[] = { 35, 70, 104, 139, 173, 209, 243, 278 };

CUAKMEncodedMessageModule* CUAKMEncodedMessageModule::pUAKMEMM = NULL;

CUAKMEncodedMessageModule::CUAKMEncodedMessageModule() : CModuleBase(ModuleType::EncodedMessage)
{
	pUAKMEMM = this;

	_cursorMinX = 0;
	_cursorMaxX = dx.GetWidth() - 1;
	_cursorMinY = 0;
	_cursorMaxY = dx.GetHeight() - 1;

	_screen = NULL;

	_col1 = 0x1f;
	_col2 = 0x10;
	_col3 = 0x18;
	_col4 = 0x1f;

	_col2 = 22;
	_col3 = 16;
	_col4 = 0;

	_left = 0.0f;
	_top = 0.0f;
	_width = 0.0f;
	_height = 0.0f;

	_pSaveMsg = NULL;

	_vertexBuffer = NULL;
	_indicatorVertexBuffer = NULL;

	_indicatorX = -1.0f;
	_indicatorY = -1.0f;

	_charPos = -1;
	_completed = false;
}

CUAKMEncodedMessageModule::~CUAKMEncodedMessageModule()
{
	Dispose();
}

void CUAKMEncodedMessageModule::Dispose()
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	if (_indicatorVertexBuffer != NULL)
	{
		_indicatorVertexBuffer->Release();
		_indicatorVertexBuffer = NULL;
	}

	if (_screen != NULL)
	{
		delete[] _screen;
		_screen = NULL;
	}
}

void CUAKMEncodedMessageModule::Render()
{
	if (_vertexBuffer != NULL)
	{
		dx.Clear(0.0f, 0.0f, 0.0f);

		dx.DisableZBuffer();

		CConstantBuffers::Setup2D(dx);

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

		if (_charPos >= 0)
		{
			UINT stride = sizeof(COLOURED_VERTEX_ORTHO);
			UINT offset = 0;
			dx.SetVertexBuffers(0, 1, &_indicatorVertexBuffer, &stride, &offset);
			dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			CShaders::SelectColourShader();
			XMMATRIX wm = XMMatrixTranslation(_indicatorX, -_indicatorY, -0.5f);
			CConstantBuffers::SetWorld(dx, &wm);
			dx.Draw(5, 0);
		}

		_pBtnResume->Render();

		CModuleController::Cursors[0].SetPosition((float)_cursorPosX, (float)_cursorPosY);
		CModuleController::Cursors[0].Render();

		dx.EnableZBuffer();

		dx.Present(1, 0);
	}
}

void CUAKMEncodedMessageModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (key >= 'A' && key <= 'Z' && _charPos >= 0)
	{
		// Get original character at position
		char orig = pMsg[_charPos];

		char* pUsr = _pSaveMsg;
		char* pSrc = pMsg;
		char* pDst = pDec;

		// Update buffer
		for (int y = 0; y < 8; y++)
		{
			int ox = 41;
			int oy = LineOffsets[y] - 1;

			for (int x = 0; x < 18; x++)
			{
				char d = *pSrc++;
				char c = *pUsr++;
				char b = *pDst++;

				if (d == orig)
				{
					c = *(pUsr - 1) = (char)key;

					if (b == c)
					{
						// Choose red
						_col2 = 9;
						_col3 = 8;
					}
					else
					{
						// Choose green
						_col2 = 12;
						_col3 = 12;
					}

					RenderChar(ox, oy, c, FALSE);
				}

				ox += 13;
			}
		}

		UpdateTexture();

		// Check if message is decoded, add score (hint state?)
		_completed = CheckCompleted();
		if (_completed)
		{
			CGameController::SetHintState(178, 1, 1);
			CGameController::SetHintCategoryState(28, 2);
		}
	}
}

void CUAKMEncodedMessageModule::Initialize()
{
	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	DWORD s;
	_font = GetResource(IDR_RAWFONT, L"BIN", &s);
	if (_font != NULL)
	{
		for (int i = 0; i < 224; i++)
		{
			char ix = ' ' + i;
			int offset = GetInt(_font + 3, i * 4, 4);
			LPBYTE pChar = _font + offset;
			_fontMap[ix] = pChar;
		}
	}

	// Background is in GRAPHICS.AP entry 13 with palette in 12
	DoubleData bd = LoadDoubleEntry(L"GRAPHICS.AP", 12);
	if (bd.File1.Data != NULL)
	{
		// Copy palette
		LPBYTE pPal = bd.File1.Data;
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

	_texture.Init(286, 322);	// Size of background image

	_screen = bd.File2.Data;

	_codeMap['V'] = 'E';
	_codeMap['Z'] = 'A';
	_codeMap['S'] = 'C';
	_codeMap['K'] = 'F';
	_codeMap['G'] = 'D';
	_codeMap['C'] = 'G';

	// Render original text
	RenderText(pMsg, 11, TRUE);

	_pSaveMsg = (char*)(CGameController::GetDataPointer() + UAKM_SAVE_CODED_MESSAGE);
	char* pUsr = _pSaveMsg;
	char* pDst = pDec;

	// Render decrypted text
	for (int y = 0; y < 8; y++)
	{
		int ox = 41;
		int oy = LineOffsets[y] - 1;

		for (int x = 0; x < 18; x++)
		{
			char c = *pUsr++;
			char d = *pDst++;
			if (c == d)
			{
				// Choose red
				_col2 = 9;
				_col3 = 8;
			}
			else
			{
				// Choose green
				_col2 = 12;
				_col3 = 12;
			}

			RenderChar(ox, oy, c, FALSE);
			ox += 13;
		}
	}

	UpdateTexture();

	// Calculate screen offsets
	float w = (float)dx.GetWidth();
	float h = (float)dx.GetHeight();
	float sx = w / 286.0f;
	float sy = h / 322.0f;
	_scale = min(sx, sy);
	_width = 286 * _scale;
	_height = 322 * _scale;
	_left = (w - _width) / 2.0f;
	_top = (h - _height) / 2.0f;

	CreateTexturedRectangle(-_top, _left, -_height - _top, _left + _width, &_vertexBuffer, "EncodedNoteVertexBuffer");

	COLOURED_VERTEX_ORTHO* pVB = new COLOURED_VERTEX_ORTHO[5];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = floor(14.0f * _scale);
		float y1 = 0.0f;
		float y2 = floor(y1 - 12.0f * _scale);

		pVB[0].position = XMFLOAT4(x1, y1, 0.0f, 0.0f);
		pVB[0].colour = XMFLOAT4(0.0f, 0.588f, 1.0f, 1.0f);
		pVB[1].position = XMFLOAT4(x2, y1, 0.0f, 0.0f);
		pVB[1].colour = XMFLOAT4(0.0f, 0.588f, 1.0f, 1.0f);
		pVB[2].position = XMFLOAT4(x2, y2, 0.0f, 0.0f);
		pVB[2].colour = XMFLOAT4(0.0f, 0.588f, 1.0f, 1.0f);
		pVB[3].position = XMFLOAT4(x1, y2, 0.0f, 0.0f);
		pVB[3].colour = XMFLOAT4(0.0f, 0.588f, 1.0f, 1.0f);
		pVB[4].position = XMFLOAT4(x1, y1, 0.0f, 0.0f);
		pVB[4].colour = XMFLOAT4(0.0f, 0.588f, 1.0f, 1.0f);

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(COLOURED_VERTEX_ORTHO) * 5;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbDesc.MiscFlags = 0;
		vbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vData;
		vData.pSysMem = pVB;
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&vbDesc, &vData, &_indicatorVertexBuffer, "EncodedNoteIndicatorVertexBuffer");

		delete[] pVB;
	}

	// Resume-button
	char* pResume = "Resume";
	_pBtnResume = new CDXButton(pResume, TexFont.PixelWidth(pResume), 32.0f * pConfig->FontScale, OnResume);
	_pBtnResume->SetPosition(dx.GetWidth() - _pBtnResume->GetWidth(), dx.GetHeight() - 40 * pConfig->FontScale);

	_completed = CheckCompleted();
}

void CUAKMEncodedMessageModule::UpdateTexture()
{
	ID3D11Texture2D* pTex = _texture.GetTexture();
	if (pTex != NULL)
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(dx.Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
		{
			int* pScr = (int*)subRes.pData;
			for (int y = 0; y < 322; y++)
			{
				for (int x = 0; x < 286; x++)
				{
					pScr[y * subRes.RowPitch / 4 + x] = _palette[_screen[y * 286 + x]];
				}
			}

			dx.Unmap(pTex, 0);
		}
	}
}

void CUAKMEncodedMessageModule::RenderChar(int x, int y, char c, BOOL transparent)
{
	LPBYTE pChar = _fontMap[c];
	if (pChar != NULL)
	{
		int width = *pChar++;
		int bytesPerLine = (width + 1) / 2;
		pChar += bytesPerLine;	// Skip 1 line
		for (int cy = 1; cy < 13; cy++)
		{
			int cx = 0;
			for (; cx < bytesPerLine; cx++)
			{
				int p = *pChar++;
				int p1 = (p >> 4) & 7;
				int p2 = p & 7;
				int c1 = (p1 == 1) ? _col1 : (p1 == 2) ? _col2 : (p1 == 3) ? _col3 : (p1 == 4) ? _col4 : 0x1f;
				int c2 = (p2 == 1) ? _col1 : (p2 == 2) ? _col2 : (p2 == 3) ? _col3 : (p2 == 4) ? _col4 : 0x1f;

				if (!transparent || p1 != 0)
				{
					_screen[(y + cy) * 286 + x + cx * 2] = c1;
				}
				if (!transparent || p2 != 0)
				{
					_screen[(y + cy) * 286 + x + cx * 2 + 1] = c2;
				}
			}
			if (!transparent)
			{
				while (cx < 6)
				{
					_screen[(y + cy) * 286 + x + cx * 2] = 0x1f;
					_screen[(y + cy) * 286 + x + cx * 2 + 1] = 0x1f;
					cx++;
				}
			}
		}
	}
}

void CUAKMEncodedMessageModule::RenderText(char* pText, int yOffset, BOOL transparent)
{
	char* pPrint = pMsg;
	int pos = 0;
	int line = 0;
	int x = 41;
	int y = LineOffsets[line] + yOffset;
	while (*pPrint != 0)
	{
		char c = *(pPrint++);
		RenderChar(x, y, c, transparent);
		x += 13;
		pos++;
		if (pos == 18)
		{
			pos = 0;
			line++;
			x = 41;
			y = LineOffsets[line] + yOffset;
		}
	}
}

bool CUAKMEncodedMessageModule::CheckCompleted()
{
	for (int i = 0; i < 18 * 8; i++)
	{
		if (_pSaveMsg[i] != pDec[i])
		{
			return false;
		}
	}

	return true;
}

void CUAKMEncodedMessageModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnResume->SetMouseOver(_pBtnResume->HitTest(x, y) != NULL);

	// Check if the cursor is over a letter
	_indicatorX = -1.0f;
	_indicatorY = -1.0f;
	_charPos = -1;
	int cy = (int)((_cursorPosY - _top) / _scale);
	int cx = (int)((_cursorPosX - _left) / _scale);
	for (int sy = 0; sy < 8; sy++)
	{
		if (cy >= LineOffsets[sy] && cy < LineOffsets[sy] + 11)
		{
			if (cx >= 41 && cx < (41 + 18 * 13))
			{
				int p = sy * 18 + (cx - 41) / 13;
				char c = pDec[p];
				if (c >= 'A' && c <= 'Z')
				{
					_indicatorX = floor(_left + (39 + ((cx - 41) / 13) * 13.0f) * _scale) + 0.5f;
					_indicatorY = floor(_top + LineOffsets[sy] * _scale) - 0.5f;
					_charPos = p;
				}
			}
			break;
		}
	}
}

void CUAKMEncodedMessageModule::Back()
{
	CModuleController::Pop(this);
}

void CUAKMEncodedMessageModule::BeginAction()
{
	if (_pBtnResume->HitTest(_cursorPosX, _cursorPosY))
	{
		_pBtnResume->Click();
	}
}

void CUAKMEncodedMessageModule::OnResume(LPVOID data)
{
	pUAKMEMM->Back();
}
