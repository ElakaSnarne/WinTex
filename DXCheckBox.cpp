#include "DXCheckBox.h"
#include "resource.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXScreen.h"

CTexture CDXCheckBox::_cbTexBackground;
CTexture CDXCheckBox::_cbTexMouseOver;
CTexture CDXCheckBox::_cbTexChecked;

CDXCheckBox::CDXCheckBox(char* text, BOOL* pValue, float width)
{
	_textX = 0.0f;
	_textY = 0.0f;

	_pChecked = pValue;

	_pText = new CDXText();
	_pText->SetText(text);
	_pText->SetColours(0xffffffff);

	_x = 0.0f;
	_y = 0.0f;
	_w = max(_pText->PixelWidth(text) + 79.0f, width);
	_h = 32.0f;

	TEXTURED_VERTEX* pVB = new TEXTURED_VERTEX[6];
	if (pVB != NULL)
	{
		float x1 = _w - 32.0f;
		float x2 = _w;
		float y1 = 0.0f;
		float y2 = -32.0f;

		pVB[0].position.x = x1;
		pVB[0].position.y = y1;
		pVB[0].position.z = -0.5f;
		pVB[0].texture.x = 0.0f;
		pVB[0].texture.y = 0.0f;

		pVB[1].position.x = x2;
		pVB[1].position.y = y1;
		pVB[1].position.z = -0.5f;
		pVB[1].texture.x = 1.0f;
		pVB[1].texture.y = 0.0f;

		pVB[2].position.x = x2;
		pVB[2].position.y = y2;
		pVB[2].position.z = -0.5f;
		pVB[2].texture.x = 1.0f;
		pVB[2].texture.y = 1.0f;

		pVB[3].position.x = x1;
		pVB[3].position.y = y1;
		pVB[3].position.z = -0.5f;
		pVB[3].texture.x = 0.0f;
		pVB[3].texture.y = 0.0f;

		pVB[4].position.x = x2;
		pVB[4].position.y = y2;
		pVB[4].position.z = -0.5f;
		pVB[4].texture.x = 1.0f;
		pVB[4].texture.y = 1.0f;

		pVB[5].position.x = x1;
		pVB[5].position.y = y2;
		pVB[5].position.z = -0.5f;
		pVB[5].texture.x = 0.0f;
		pVB[5].texture.y = 1.0f;

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 6;
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

	_h = max(40.0f, _h);

	_type = ControlType::CheckBox;
}

CDXCheckBox::~CDXCheckBox()
{
	if (_pText != NULL)
	{
		delete _pText;
		_pText = NULL;
	}
}

void CDXCheckBox::Init()
{
	DWORD s1, s2, s3;
	PBYTE p1 = GetResource(IDB_BUTTON, L"PNG", &s1);
	PBYTE p2 = GetResource(IDB_BUTTON_MOUSEOVER, L"PNG", &s2);
	PBYTE p3 = GetResource(IDB_CHECKMARK, L"PNG", &s3);

	_cbTexBackground.Init(p1, s1, "CHECKBOX1");
	_cbTexMouseOver.Init(p2, s2, "CHECKBOX2");
	_cbTexChecked.Init(p3, s3, "CHECKBOX3");
}

void CDXCheckBox::Dispose()
{
	_cbTexBackground.Dispose();
	_cbTexMouseOver.Dispose();
	_cbTexChecked.Dispose();
}

void CDXCheckBox::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = _mouseOver ? _cbTexMouseOver.GetTextureRV() : _cbTexBackground.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, 0);

	if (*_pChecked)
	{
		pRV = _cbTexChecked.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(6, 0);
	}

	// Draw text
	_pText->Render(_textX + _x, _y + 8);
}

void CDXCheckBox::SetColours(int colour1, int colour2, int colour3, int colour4)
{
	_pText->SetColours(colour1, colour2, colour3, colour4);
}
