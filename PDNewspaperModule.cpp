#include "PDNewspaperModule.h"
#include "Utilities.h"
#include "GameController.h"
#include "PDGame.h"

#define NEWSPAPER_PALETTE		0
#define NEWSPAPER_PAGE			1
#define NEWSPAPER_ARTICLE_1		2
#define NEWSPAPER_ARTICLE_2		3
#define NEWSPAPER_ARTICLE_3		4
#define NEWSPAPER_ARTICLE_4		5
#define NEWSPAPER_ARTICLE_5		6
#define NEWSPAPER_ARTICLE_6		7
#define NEWSPAPER_ARTICLE_7		8
#define NEWSPAPER_ARTICLE_8		9
#define NEWSPAPER_ARTICLE_9		10
#define NEWSPAPER_ARTICLE_10	11

CPDNewspaperModule* CPDNewspaperModule::pPDNPM = NULL;

CPDNewspaperModule::CPDNewspaperModule() : CModuleBase(ModuleType::NewsPaper)
{
	pPDNPM = this;

	_display = 0;
	_highLight = 0;
	_cursorMinX = 0;
	_cursorMaxX = 0;
	_cursorMinY = 0;
	_cursorMaxY = 0;

	float _left = 0.0f;
	float _right = 0.0f;
	float _top = 0.0f;
	float _bottom = 0.0f;
	float _scale = 0.0f;

	ZeroMemory(_palette, 256 * sizeof(int));
}

CPDNewspaperModule::~CPDNewspaperModule()
{
	Dispose();
}

void CPDNewspaperModule::Initialize()
{
	_display = 1;
	_highLight = 0;

	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	int screenWidth = dx.GetWidth();
	int screenHeight = dx.GetHeight();

	_cursorMinX = 0;
	_cursorMaxX = screenWidth - 1;
	_cursorMinY = 0;
	_cursorMaxY = screenHeight - 1;

	// Load newspaper entries
	CFile file;
	if (file.Open(L"NEWS.AP"))
	{
		DWORD length = file.Size();
		LPBYTE data = new BYTE[length];
		if (data != NULL)
		{
			file.Read(data, length);
			file.Close();
			int count = GetInt(data, 0, 2) - 1;
			for (int i = 0; i < count; i++)
			{
				// Locate and decompress each entry (each entry is another AP)
				int offset = GetInt(data, 2 + i * 4, 4);
				int nextOffset = GetInt(data, 6 + i * 4, 4);
				int len = nextOffset - offset;
				if (i == NEWSPAPER_PALETTE)
				{
					for (int c = 0; c < 256; c++)
					{
						float r = data[offset + c * 3 + 0];
						float g = data[offset + c * 3 + 1];
						float b = data[offset + c * 3 + 2];
						int ri = (byte)((r * 255.0f) / 63.0f);
						int gi = (byte)((g * 255.0f) / 63.0f);
						int bi = (byte)((b * 255.0f) / 63.0f);
						int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
						_palette[c] = col;
					}
				}
				else
				{
					BinaryData bd = CLZ::Decompress(data, offset, length);
					if (bd.Data != NULL)
					{
						CNewsPaperView* np = new CNewsPaperView();
						if (GetInt(bd.Data, 0, 2) == 0x100)
						{
							np->Width = GetInt(bd.Data, 2, 2);
							np->Height = GetInt(bd.Data, 4, 2);
							np->Texture.Init(np->Width, np->Height);
							np->Data = new BYTE[np->Width * np->Height];
							memset(np->Data, 0, np->Width * np->Height);

							int inPtr = 16;
							for (int y = 0; y < np->Height; y++)
							{
								int c1 = GetInt(bd.Data, inPtr, 2);
								int c2 = GetInt(bd.Data, inPtr + 2, 2);
								for (int x = 0; x < c2; x++)
								{
									np->Data[y * np->Width + c1 + x] = bd.Data[inPtr + 4 + x];
								}
								inPtr += 4 + c2;
							}

							UpdateTexture(np);

							int width = np->Width, height = np->Height;
							float scale = max(1.0f, min(((float)screenWidth) / (float)width, ((float)screenHeight) / height) * 0.75f);
							float sw = width * scale;
							float sh = height * scale;
							float ox = (screenWidth - sw);
							float oy = (screenHeight - sh);

							float left = (float)(ox / 2.0f);
							float right = left + sw;
							float top = (float)(-oy / 2.0f);
							float bottom = top - sh;

							if (i == NEWSPAPER_PAGE)
							{
								_left = left;
								_right = right;
								_top = -top;
								_bottom = -bottom;
								_scale = scale;
							}

							TEXTURED_VERTEX* vertices = new TEXTURED_VERTEX[4];
							if (vertices != NULL)
							{
								vertices[0].position = XMFLOAT3(right, top, 0.0f);
								vertices[0].texture = XMFLOAT2(1.0f, 0.0f);

								vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
								vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

								vertices[2].position = XMFLOAT3(left, top, 0.0f);
								vertices[2].texture = XMFLOAT2(0.0f, 0.0f);

								vertices[3].position = XMFLOAT3(left, bottom, 0.0f);
								vertices[3].texture = XMFLOAT2(0.0f, 1.0f);

								D3D11_BUFFER_DESC vertexBufferDesc;
								vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
								vertexBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 4;
								vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
								vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
								vertexBufferDesc.MiscFlags = 0;
								vertexBufferDesc.StructureByteStride = 0;

								D3D11_SUBRESOURCE_DATA vertexData;
								vertexData.pSysMem = vertices;
								vertexData.SysMemPitch = 0;
								vertexData.SysMemSlicePitch = 0;

								dx.CreateBuffer(&vertexBufferDesc, &vertexData, &np->Buffer, "NewsPaperBuffer");

								delete[] vertices;
							}
						}

						delete[] bd.Data;

						_newsPaper[i] = np;
					}
				}
			}

			delete[] data;
		}
	}

	// Resume-button
	char* pResume = "Resume";
	_pBtnResume = new CDXButton(pResume, TexFont.PixelWidth(pResume), 32.0f * pConfig->FontScale, OnResume);
	_pBtnResume->SetPosition(dx.GetWidth() - _pBtnResume->GetWidth(), dx.GetHeight() - 40 * pConfig->FontScale);
}

void CPDNewspaperModule::OnResume(LPVOID data)
{
	pPDNPM->Back();
}

void CPDNewspaperModule::UpdateTexture(CNewsPaperView* np)
{
	ID3D11Texture2D* pTex = np->Texture.GetTexture();
	if (pTex != NULL)
	{
		D3D11_MAPPED_SUBRESOURCE subRes;
		if (SUCCEEDED(dx.Map(pTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
		{
			int inPtr = 16;
			int* pScr = (int*)subRes.pData;
			for (int y = 0; y < np->Height; y++)
			{
				for (int x = 0; x < np->Width; x++)
				{
					pScr[y * subRes.RowPitch / 4 + x] = _palette[np->Data[y * np->Width + x]];
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

void CPDNewspaperModule::Dispose()
{
	for (auto it : _newsPaper)
	{
		delete it.second;
	}

	_newsPaper.clear();
}

void CPDNewspaperModule::Render()
{
	CNewsPaperView* np = _newsPaper[_display];
	if (np != NULL && np->Buffer != NULL)
	{
		dx.DisableZBuffer();

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &np->Buffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();
		XMMATRIX wm = XMMatrixIdentity();
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = np->Texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		_pBtnResume->Render();

		CModuleController::Cursors[0].SetPosition((float)_cursorPosX, (float)_cursorPosY);
		CModuleController::Cursors[0].Render();

		dx.EnableZBuffer();
	}
}

void CPDNewspaperModule::Resize(int width, int height)
{
}

void CPDNewspaperModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	_pBtnResume->SetMouseOver(_pBtnResume->HitTest(x, y) != NULL);

	if (_display == NEWSPAPER_PAGE)
	{
		// Check which article the mouse is over, update the palette and update texture
		int sx = (int)((_cursorPosX - _left) / _scale);
		int sy = (int)((_cursorPosY - _top) / _scale);

		if (sx >= 0 && sy >= 0)
		{
			CNewsPaperView* np = _newsPaper[NEWSPAPER_PAGE];
			if (np != NULL && sx < np->Width && sy < np->Height)
			{
				// Trace up or down from point, find byte with value >= 0x80
				while (sy >= 0)
				{
					BYTE pixel = np->Data[sy * np->Width + sx];
					if (pixel >= 0x80)
					{
						if (pixel != _highLight)
						{
							if (_highLight >= 0x80)
							{
								// Deselect previous
								_palette[_highLight] = 0xffffffff;
							}

							if (pixel >= 0x80)
							{
								_palette[pixel] = 0xffffff00;
							}

							_highLight = pixel;

							UpdateTexture(np);
						}

						break;
					}

					sy--;
				}
			}
		}
	}
}

void CPDNewspaperModule::BeginAction()
{
	if (_pBtnResume->HitTest(_cursorPosX, _cursorPosY))
	{
		_pBtnResume->Click();
	}
	else
	{
		if (_display == NEWSPAPER_PAGE && _highLight >= 0x80)
		{
			// Zoom selected image from original location to full screen? Or simply switch immediately to full screen?
			_display = NEWSPAPER_ARTICLE_1 + _highLight - 0x80;

			if (_highLight == 0x80)
			{
				// First article
				//CGameController::SetAskAboutState(27, 1);			// Enable Ask About Mac Malden
				//CGameController::SetData(PD_SAVE_TRAVEL + 4, 1);	// Enable travel to police station
				//CGameController::SetHintState(0x1d8, 1, 1);
				//CGameController::SetItemExamined(94, 4);			// Flag newspaper as examined
			}
		}
		else
		{
			_display = NEWSPAPER_PAGE;
		}
	}
}

void CPDNewspaperModule::Back()
{
	if (_display == NEWSPAPER_PAGE)
	{
		CModuleController::Pop(this);
	}
	else
	{
		_display = NEWSPAPER_PAGE;
	}
}
