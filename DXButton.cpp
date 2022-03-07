#include "DXButton.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXScreen.h"
#include "resource.h"

CTexture CDXButton::_texBackground;
CTexture CDXButton::_texMouseOver;
CDXSound* CDXButton::_pSound;

CDXButton::CDXButton(LPSTR text, float w, float h, void(*onClick)(LPVOID), LPVOID data)
{
	_clicked = onClick;
	_data = data;

	_pText = new CDXText();
	_pText->SetText(text);

	float textW = _pText->PixelWidth(text);

	_x = 0.0f;
	_y = 0.0f;
	_w = _w = ((w > 0) ? w : textW);
	_h = 40.0f * pConfig->FontScale;

	TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[54];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = 16.0f * pConfig->FontScale;
		float x3 = 16.0f * pConfig->FontScale + _w;
		float x4 = x3 + 16.0f * pConfig->FontScale;
		float y1 = 0.0f;
		float y2 = -16.0f * pConfig->FontScale;
		float y3 = -24.0f * pConfig->FontScale;
		float y4 = -40.0f * pConfig->FontScale;

		float u1 = 0.0f;
		float u2 = 0.25f;
		float u3 = 0.75f;
		float u4 = 1.0f;
		float v1 = 1.0f;
		float v2 = 0.75f;
		float v3 = 0.25f;
		float v4 = 0.0f;

		SetQuadVertex(pVB, 0, x1, x2, y1, y2, u1, u2, v1, v2);
		SetQuadVertex(pVB, 1, x2, x3, y1, y2, u2, u3, v1, v2);
		SetQuadVertex(pVB, 2, x3, x4, y1, y2, u3, u4, v1, v2);

		SetQuadVertex(pVB, 3, x1, x2, y2, y3, u1, u2, v2, v3);
		SetQuadVertex(pVB, 4, x2, x3, y2, y3, u2, u3, v2, v3);
		SetQuadVertex(pVB, 5, x3, x4, y2, y3, u3, u4, v2, v3);

		SetQuadVertex(pVB, 6, x1, x2, y3, y4, u1, u2, v3, v4);
		SetQuadVertex(pVB, 7, x2, x3, y3, y4, u2, u3, v3, v4);
		SetQuadVertex(pVB, 8, x3, x4, y3, y4, u3, u4, v3, v4);

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX_ORTHO) * 54;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbDesc.MiscFlags = 0;
		vbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vData;
		vData.pSysMem = pVB;
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, text);

		delete[] pVB;
	}

	_textX = 16.0f * pConfig->FontScale - 10.0f + (_w - textW) / 2.0f;
	_w += 32.0f * pConfig->FontScale;

	_type = ControlType::Button;
}

CDXButton::~CDXButton()
{
	if (_pText != NULL)
	{
		delete _pText;
		_pText = NULL;
	}
}

void CDXButton::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = _mouseOver ? _texMouseOver.GetTextureRV() : _texBackground.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dx.Draw(54, 0);

	// Draw text
	_pText->SetColours((_mouseOver && _enabled) ? 0xffffffff : 0x80ffffff);
	float fontHeight = TexFont.Height();
	_pText->Render(_textX + _x, _y + fontHeight * pConfig->FontScale);
}

void CDXButton::MouseEnter()
{
}

void CDXButton::MouseMove()
{
}

void CDXButton::MouseLeave()
{
}

void CDXButton::MouseButtonDown()
{
}

void CDXButton::MouseButtonUp()
{
}

void CDXButton::KeyDown()
{
}

void CDXButton::KeyUp()
{
}

void CDXButton::GotFocus()
{
}

void CDXButton::LostFocus()
{
}

void CDXButton::Init()
{
	DWORD s1, s2;
	PBYTE p1 = GetResource(IDB_BUTTON, L"PNG", &s1);
	PBYTE p2 = GetResource(IDB_BUTTON_MOUSEOVER, L"PNG", &s2);

	_texBackground.Init(p1, s1, "BUTTON1");
	_texMouseOver.Init(p2, s2, "BUTTON2");

	_pSound = new CDXSound();
}

void CDXButton::Dispose()
{
	if (_pSound != NULL)
	{
		delete _pSound;
		_pSound = NULL;
	}

	_texBackground.Dispose();
	_texMouseOver.Dispose();
}

void CDXButton::SetMouseOver(BOOL mouseOver)
{
	BOOL oldMouseOver = _mouseOver;
	_mouseOver = mouseOver;

	if (mouseOver && !oldMouseOver)
	{
		// Play sound
		DWORD s;
		PBYTE pWave = GetResource(IDR_BUTTON_MOUSEOVER_WAVE, L"WAVE", &s);
		_pSound->Play(pWave, s);
	}
}

void CDXButton::Click()
{
	if (_clicked != NULL)
	{
		_clicked(_data);
	}
}
