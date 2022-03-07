#include "DXImageButton.h"
#include "Globals.h"
#include "resource.h"
#include "Utilities.h"

CTexture CDXImageButton::_ibTexBackground;
CTexture CDXImageButton::_ibTexMouseOver;

CDXImageButton::CDXImageButton(int img, void(*onClick)(LPVOID data))
{
	_clicked = onClick;

	_x = 0.0f;
	_y = 0.0f;
	_w = 32.0f;
	_h = 32.0f;

	TEXTURED_VERTEX* pVB = new TEXTURED_VERTEX[6];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = 32.0f;
		float y1 = 0.0f;
		float y2 = -32.0f;

		float u1 = 0.0f;
		float u2 = 0.5f;
		float v1 = 0.0f, v2 = 1.0f;
		if (img & 1)
		{
			u1 += 0.5f;
			u2 += 0.5f;
		}

		if (!(img & 2))
		{
			pVB[0].position.x = x1;
			pVB[0].position.y = y1;
			pVB[0].position.z = -0.5f;
			pVB[0].texture.x = u1;
			pVB[0].texture.y = v1;

			pVB[1].position.x = x2;
			pVB[1].position.y = y1;
			pVB[1].position.z = -0.5f;
			pVB[1].texture.x = u2;
			pVB[1].texture.y = v1;

			pVB[2].position.x = x2;
			pVB[2].position.y = y2;
			pVB[2].position.z = -0.5f;
			pVB[2].texture.x = u2;
			pVB[2].texture.y = v2;

			pVB[3].position.x = x1;
			pVB[3].position.y = y1;
			pVB[3].position.z = -0.5f;
			pVB[3].texture.x = u1;
			pVB[3].texture.y = v1;

			pVB[4].position.x = x2;
			pVB[4].position.y = y2;
			pVB[4].position.z = -0.5f;
			pVB[4].texture.x = u2;
			pVB[4].texture.y = v2;

			pVB[5].position.x = x1;
			pVB[5].position.y = y2;
			pVB[5].position.z = -0.5f;
			pVB[5].texture.x = u1;
			pVB[5].texture.y = v2;
		}
		else
		{
			pVB[0].position.x = x1;
			pVB[0].position.y = y1;
			pVB[0].position.z = -0.5f;
			pVB[0].texture.x = u1;
			pVB[0].texture.y = v2;

			pVB[1].position.x = x2;
			pVB[1].position.y = y1;
			pVB[1].position.z = -0.5f;
			pVB[1].texture.x = u1;
			pVB[1].texture.y = v1;

			pVB[2].position.x = x2;
			pVB[2].position.y = y2;
			pVB[2].position.z = -0.5f;
			pVB[2].texture.x = u2;
			pVB[2].texture.y = v1;

			pVB[3].position.x = x1;
			pVB[3].position.y = y1;
			pVB[3].position.z = -0.5f;
			pVB[3].texture.x = u1;
			pVB[3].texture.y = v2;

			pVB[4].position.x = x2;
			pVB[4].position.y = y2;
			pVB[4].position.z = -0.5f;
			pVB[4].texture.x = u2;
			pVB[4].texture.y = v1;

			pVB[5].position.x = x1;
			pVB[5].position.y = y2;
			pVB[5].position.z = -0.5f;
			pVB[5].texture.x = u2;
			pVB[5].texture.y = v2;
		}

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

		dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "DXImageButton");

		delete[] pVB;
	}

	_type = ControlType::ImageButton;
}

CDXImageButton::~CDXImageButton()
{
}

void CDXImageButton::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = _mouseOver ? _ibTexMouseOver.GetTextureRV() : _ibTexBackground.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, 0);
}

void CDXImageButton::Init()
{
	DWORD s1, s2;
	PBYTE p1 = GetResource(IDB_IMAGEBUTTON, L"PNG", &s1);
	PBYTE p2 = GetResource(IDB_IMAGEBUTTON_MOUSEOVER, L"PNG", &s2);

	_ibTexBackground.Init(p1, s1, "IMAGEBUTTON1");
	_ibTexMouseOver.Init(p2, s2, "IMAGEBUTTON2");
}

void CDXImageButton::Dispose()
{
	_ibTexBackground.Dispose();
	_ibTexMouseOver.Dispose();
}
