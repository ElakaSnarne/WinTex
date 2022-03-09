#include "DXTabItem.h"
#include "Utilities.h"
#include "resource.h"
#include "DXTabControl.h"

CTexture CDXTabItem::_texBackgroundTabItem;

CDXTabItem::CDXTabItem(CDXTabControl* pOwner, LPSTR title, float w, float h)
{
	_pOwner = pOwner;

	_pText = new CDXText();
	Rect rc;
	rc.Top = 0.0f;
	rc.Left = 0.0f;
	rc.Bottom = -256.0f;
	rc.Right = 1024.0f;
	_pText->SetText(title, rc);

	_textW = static_cast<int>(_pText->PixelWidth(title));

	_x = 0.0f;
	_y = 0.0f;
	_w = w;
	_h = h;

	TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[54];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = w;
		float y1 = 0.0f;
		float y2 = -_pText->Height() - 6;

		float u1 = 0.0f;
		float u2 = 0.25f;
		float u3 = 0.5f;
		float u4 = 1.0f;
		float v1 = 0.0f;
		float v2 = 0.25f;
		float v3 = 0.5f;
		float v4 = 1.0f;

		SetQuadVertex(pVB, 0, x1, x2, y1, y2, u1, u2, v1, v2);
		SetQuadVertex(pVB, 1, x1, x2, y1, y2, u3, u4, v3, v4);

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

	_type = ControlType::TabItem;
}

CDXTabItem::~CDXTabItem()
{
	//if (_pText != NULL)
	//{
	//	delete _pText;
	//	_pText = NULL;
	//}
}

void CDXTabItem::Render(float x, float y, float hx, float hy, bool selected)
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(hx, -hy, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = _texBackgroundTabItem.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, selected ? 0 : 6);

	// Draw text
	_pText->SetColours(selected ? 0xffffffff : 0xffc0c0c0);
	_pText->Render(hx + (_w - _pText->Width()) / 2, hy + 3);

	if (selected)
	{
		// Render child controls
		for (auto child : _childElements)
		{
			if (child->GetVisible())
			{
				child->Render();
			}
		}
	}
}

void CDXTabItem::Init()
{
	DWORD s;
	PBYTE p = GetResource(IDB_TABHEADER, L"PNG", &s);
	_texBackgroundTabItem.Init(p, s, "TABITEMHEADER");
}

void CDXTabItem::Dispose()
{
	_texBackgroundTabItem.Dispose();
}

void CDXTabItem::MouseButtonDown()
{
	int debug = 0;
}

CDXControl* CDXTabItem::HitTest(float x, float y)
{
	for (auto child : _childElements)
	{
		CDXControl* pHit = child->HitTest(x, y);
		if (pHit != NULL)
		{
			return pHit;
		}
	}

	return NULL;
}

void CDXTabItem::Select()
{
	if (_pOwner != NULL)
	{
		_pOwner->Select(this);
	}
}
