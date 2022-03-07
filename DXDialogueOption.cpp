#include "DXDialogueOption.h"
#include "Globals.h"
#include "Utilities.h"
#include "resource.h"

CTexture CDXDialogueOption::_texBubble;

CDXDialogueOption::CDXDialogueOption()
{
	_pText = new CDXText();

	_vertexBuffer = NULL;
}

CDXDialogueOption::~CDXDialogueOption()
{
	Clear();
}

void CDXDialogueOption::Init()
{
	DWORD s1, s2;
	PBYTE p1 = GetResource(IDB_BUBBLE, L"PNG", &s1);

	_texBubble.Init(p1, s1, "BUTTON1");
}

void CDXDialogueOption::Dispose()
{
	_texBubble.Dispose();
}

void CDXDialogueOption::Clear()
{
	// Dispose buffers and text
	if (_pText != NULL)
	{
		delete _pText;
		_pText = NULL;
	}

	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}
}

void CDXDialogueOption::SetClick(void(*Clicked)(LPVOID data))
{
	_clicked = Clicked;
}

void CDXDialogueOption::SetText(char* text, Size sz, float x)
{
	Clear();
	_pText = new CDXText();

	_text = text;
	_sz = sz;

	Rect rc;
	rc.Left = 0.0f;
	rc.Right = sz.Width;
	rc.Top = 0.0f;
	rc.Bottom = sz.Height;

	_pText->SetText(text, rc, CDXText::Alignment::Center);

	_textX = 32.0f * pConfig->FontScale;

	// Create vertex buffer

	_x = x;
	_y = dx.GetHeight() - 64.0f * pConfig->FontScale;
	_w = sz.Width;
	_h = _texBubble.Height() * pConfig->FontScale;

	TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[36];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = 32.0f * pConfig->FontScale;
		float x3 = x2 + sz.Width;
		float x4 = x3 + 32.0f * pConfig->FontScale;
		float y1 = 0.0f;
		float y2 = -32.0f * pConfig->FontScale;
		float y3 = -64.0f * pConfig->FontScale;

		float u1 = 0.0f;
		float u2 = 0.5f;
		float u3 = 0.5f;
		float u4 = 1.0f;
		float v1 = 1.0f;
		float v2 = 0.5f;
		float v3 = 0.0f;

		SetQuadVertex(pVB, 0, x1, x2, y1, y2, u1, u2, v1, v2);
		SetQuadVertex(pVB, 1, x2, x3, y1, y2, u2, u3, v1, v2);
		SetQuadVertex(pVB, 2, x3, x4, y1, y2, u3, u4, v1, v2);
		SetQuadVertex(pVB, 3, x1, x2, y2, y3, u1, u2, v2, v3);
		SetQuadVertex(pVB, 4, x2, x3, y2, y3, u2, u3, v2, v3);
		SetQuadVertex(pVB, 5, x3, x4, y2, y3, u3, u4, v2, v3);

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX_ORTHO) * 36;
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

	_w += 64.0f * pConfig->FontScale;
	_h = 64.0f * pConfig->FontScale;

	_type = ControlType::DialogueBubble;
}

void CDXDialogueOption::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = _texBubble.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();

	dx.Draw(36, 0);

	// Draw text
	_pText->SetColours((_mouseOver && _enabled) ? 0xff000000 : 0x80000000);
	_pText->Render(_textX + _x, _y + 32 * pConfig->FontScale - _pText->Lines() * TexFont.Height() * pConfig->FontScale / 2.0f);
}

void CDXDialogueOption::Resize(int index, int width, int height)
{
	float w = 0.0f;
	float h = 0.0f;

	float maxw = (width - 64.0f * pConfig->FontScale * DialogueOptionsCount) / DialogueOptionsCount;
	for (int i = 0; i < 3; i++)
	{
		Size sz = TexFont.GetSize((char*)DialogueOptions[i]._text.c_str(), maxw);
		if (sz.Width > w)
		{
			w = sz.Width;
		}
		if (sz.Height > h)
		{
			h = sz.Height;
		}
	}

	Size sz;
	sz.Width = w;
	sz.Height = h;

	float dow = w + 64.0f * pConfig->FontScale;
	float sx = (width - DialogueOptionsCount * dow) / DialogueOptionsCount;

	if (DialogueOptionsCount == 1 && index == 0)
	{
		// Center
		SetText((char*)_text.c_str(), sz, (width - dow) / 2.0f);
	}
	else if (DialogueOptionsCount == 2)
	{
		// Center left, center right
		float sx3 = sx / 3.0f;
		if (index == 0)
		{
			SetText((char*)_text.c_str(), sz, sx3);
		}
		else if (index == 1)
		{
			SetText((char*)_text.c_str(), sz, width - dow - sx3);
		}
	}
	else if (DialogueOptionsCount == 3)
	{
		// Left, center, right
		if (index == 0)
		{
			SetText((char*)_text.c_str(), sz, 0.0f);
		}
		else if (index == 1)
		{
			SetText((char*)_text.c_str(), sz, (width - dow) / 2.0f);
		}
		else if (index == 2)
		{
			SetText((char*)_text.c_str(), sz, width - dow);
		}
	}
}
