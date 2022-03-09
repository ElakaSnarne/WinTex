#include "DXFrame.h"
#include "Globals.h"
#include "Utilities.h"
#include "resource.h"
#include "DXScreen.h"

CTexture CDXFrame::_texBackground;

CDXFrame::CDXFrame(LPSTR title, float w, float h)
{
	_pText = new CDXText();
	Rect rc;
	rc.Top = 0.0f;
	rc.Left = 0.0f;
	rc.Bottom = -h;
	rc.Right = w;
	_pText->SetText(title, rc);

	_textW = static_cast<int>(_pText->PixelWidth(title));

	_x = 0.0f;
	_y = 0.0f;
	_w = max(w, _textW);
	_h = max(h, _texBackground.Height());

	TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[54];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = 16.0f;
		float x3 = _w - 16.0f;
		float x4 = _w;
		float y1 = 0.0f;
		float y2 = -_pText->Height() - 6;
		float y3 = -_h - y2;
		float y4 = -_h;

		float u1 = 0.0f;
		float u2 = 0.25f;
		float u3 = 0.75f;
		float u4 = 1.0f;
		float v1 = 0.0f;
		float v2 = 0.26f;
		float v3 = 0.75f;
		float v4 = 1.0f;

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

		dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "DXFrame");

		delete[] pVB;
	}

	_type = ControlType::Frame;
}

CDXFrame::~CDXFrame()
{
	if (_pText != NULL)
	{
		delete _pText;
		_pText = NULL;
	}
}

void CDXFrame::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = _texBackground.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.Draw(54, 0);

	// Draw text
	_pText->Render(_x + (_w - _textW) / 2, _y + 3);

	// Render child controls
	std::list<CDXControl*>::iterator it = _childElements.begin();
	std::list<CDXControl*>::iterator end = _childElements.end();
	while (it != end)
	{
		if ((*it)->GetVisible())
		{
			(*it)->Render();
		}

		it++;
	}
}

void CDXFrame::MouseEnter()
{
}

void CDXFrame::MouseMove()
{
}

void CDXFrame::MouseLeave()
{
}

void CDXFrame::MouseButtonDown()
{
}

void CDXFrame::MouseButtonUp()
{
}

void CDXFrame::KeyDown()
{
}

void CDXFrame::KeyUp()
{
}

void CDXFrame::GotFocus()
{
}

void CDXFrame::LostFocus()
{
}

void CDXFrame::Init()
{
	DWORD s;
	PBYTE p = GetResource(IDB_FRAME, L"PNG", &s);
	_texBackground.Init(p, s, "FRAME");
}

void CDXFrame::Dispose()
{
	_texBackground.Dispose();
}
