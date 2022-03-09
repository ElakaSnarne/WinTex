#include "DXControl.h"
#include "D3DX11-NoWarn.h"
#include "Utilities.h"

using namespace DirectX::PackedVector;
using namespace DirectX;

CDXControl::CDXControl()
{
	_x = 0.0f;
	_y = 0.0f;

	_vertexBuffer = NULL;
	_focus = FALSE;
	_visible = TRUE;

	_type = ControlType::Undefined;
	_mouseOver = FALSE;
	_enabled = TRUE;
}

CDXControl::~CDXControl()
{
	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}
}

CDXControl* CDXControl::HitTest(float x, float y)
{
	return (_enabled && _visible && x >= _x && y >= _y && x < (_x + _w) && y < (_y + _h)) ? this : NULL;
}

void CDXControl::SetVertex(TEXTURED_VERTEX_ORTHO* pVB, int index, float x, float y, float z, float u, float v)
{
	pVB[index].position = XMFLOAT3(x, y, z);
	pVB[index].texture = XMFLOAT2(u, v);
}

void CDXControl::SetQuadVertex(TEXTURED_VERTEX_ORTHO* pVB, int index, float x1, float x2, float y1, float y2, float u1, float u2, float v1, float v2)
{
	SetVertex(pVB, index * 6 + 0, x1, y1, -0.5f, u1, v1);
	SetVertex(pVB, index * 6 + 1, x2, y1, -0.5f, u2, v1);
	SetVertex(pVB, index * 6 + 2, x2, y2, -0.5f, u2, v2);
	SetVertex(pVB, index * 6 + 3, x1, y1, -0.5f, u1, v1);
	SetVertex(pVB, index * 6 + 4, x2, y2, -0.5f, u2, v2);
	SetVertex(pVB, index * 6 + 5, x1, y2, -0.5f, u1, v2);
}

CDXControl* CDXControl::GetCurrentMouseOver()
{
	return _mouseOver ? this : NULL;
}
