#include "DXBitmap.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXScreen.h"

CDXBitmap::CDXBitmap()
{
	_type = ControlType::Bitmap;
}

CDXBitmap::CDXBitmap(PWCHAR fileName)
{
	_type = ControlType::Bitmap;
	_texture.Init(fileName);
	Init();
}

CDXBitmap::CDXBitmap(PBYTE pImage, DWORD size)
{
	_type = ControlType::Bitmap;
	_texture.Init(pImage, size, "BITMAP");
	Init();
}

CDXBitmap::CDXBitmap(int width, int height)
{
	_type = ControlType::Bitmap;
	_texture.Init(width, height);
	Init();
}

void CDXBitmap::Init()
{
	_w = static_cast<float>(_texture.Width());
	_h = static_cast<float>(_texture.Height());

	double sw = dx.GetWidth();
	double sh = dx.GetHeight();
	float imageRatioX = static_cast<float>(_w / sw);
	float imageRatioY = static_cast<float>(_h / sh);

	float imageRatio = max(imageRatioX, imageRatioY);
	if (imageRatio > 1.0f)
	{
		_w /= imageRatio;
		_h /= imageRatio;
	}

	TEXTURED_VERTEX* pVB = new TEXTURED_VERTEX[6];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = x1 + _w;
		float y1 = 0.0f;
		float y2 = y1 - _h;

		pVB[0].position = XMFLOAT3(x1, y1, -0.5f);
		pVB[0].texture = XMFLOAT2(0.0f, 0.0f);
		pVB[1].position = XMFLOAT3(x2, y1, -0.5f);
		pVB[1].texture = XMFLOAT2(1.0f, 0.0f);
		pVB[2].position = XMFLOAT3(x2, y2, -0.5f);
		pVB[2].texture = XMFLOAT2(1.0f, 1.0f);

		pVB[3].position = XMFLOAT3(x1, y1, -0.5f);
		pVB[3].texture = XMFLOAT2(0.0f, 0.0f);
		pVB[4].position = XMFLOAT3(x2, y2, -0.5f);
		pVB[4].texture = XMFLOAT2(1.0f, 1.0f);
		pVB[5].position = XMFLOAT3(x1, y2, -0.5f);
		pVB[5].texture = XMFLOAT2(0.0f, 1.0f);

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

		dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "DXBitmap");

		delete[] pVB;
	}
}

CDXBitmap::~CDXBitmap()
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}
}

void CDXBitmap::Render()
{
	if (_vertexBuffer == NULL) return;

	ID3D11ShaderResourceView* pRV = _texture.GetTextureRV();

	UINT stride = sizeof(TEXTURED_VERTEX);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, -2.0f);

	CConstantBuffers::SetWorld(dx, &wm);
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, 0);
}
