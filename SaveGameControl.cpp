#include "SaveGameControl.h"
#include "Globals.h"
#include "Utilities.h"
#include "DXScreen.h"
#include "resource.h"

CTexture CSaveGameControl::_texBackground;
CTexture CSaveGameControl::_texMouseOver;

CSaveGameControl::CSaveGameControl(void(*onClick)(SaveGameInfo), bool isSave)
{
	ZeroMemory(&_info, sizeof(_info));
	_clicked = onClick;

	_tFileName1.SetText("Filename:");
	_tPlayer1.SetText("Player:");
	_tDay1.SetText("Day:");
	_tDateTime1.SetText("Date/Time:");
	_tLocation1.SetText("Location:");
	_tComment1.SetText("Comment:");

	_tFileName1.SetColours(0xff8e8e8e, 0xff3c0c9e, 0xff3c0c9e, 0xffffffff);
	_tFileName2.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);
	_tPlayer1.SetColours(0xff8e8e8e, 0xff3c0c9e, 0xff3c0c9e, 0xffffffff);
	_tPlayer2.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);
	_tDay1.SetColours(0xff8e8e8e, 0xff3c0c9e, 0xff3c0c9e, 0xffffffff);
	_tDay2.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);
	_tDateTime1.SetColours(0xff8e8e8e, 0xff3c0c9e, 0xff3c0c9e, 0xffffffff);
	_tDateTime2.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);
	_tLocation1.SetColours(0xff8e8e8e, 0xff3c0c9e, 0xff3c0c9e, 0xffffffff);
	_tLocation2.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);
	_tComment1.SetColours(0xff8e8e8e, 0xff3c0c9e, 0xff3c0c9e, 0xffffffff);
	_tComment2.SetColours(0xff616161, 0xff710000, 0xffae0000, 0xffffffff);

	_x = 0.0f;
	_y = 0.0f;
	_w = dx.GetWidth() - (isSave ? 64 : 80.0f) * pConfig->FontScale;
	_h = 90.0f * pConfig->FontScale;

	TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[54];
	if (pVB != NULL)
	{
		float x1 = 0.0f;
		float x2 = 16.0f * pConfig->FontScale;
		float x3 = 16.0f * pConfig->FontScale + _w;
		float x4 = x3 + 16.0f * pConfig->FontScale;
		float y1 = 0.0f;
		float y2 = -16.0f * pConfig->FontScale;
		float y3 = -73.0f * pConfig->FontScale;
		float y4 = -89.0f * pConfig->FontScale;

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

		dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "SaveGame");

		delete[] pVB;
	}

	_type = ControlType::SaveGameControl;

	_isSave = isSave;
}

CSaveGameControl::~CSaveGameControl()
{
}

void CSaveGameControl::Render()
{
	if (_vertexBuffer == NULL) return;

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	XMMATRIX wm = XMMatrixTranslation(_x, -_y, 0.0f);
	CConstantBuffers::SetWorld(dx, &wm);

	ID3D11ShaderResourceView* pRV = (_mouseOver || _isSave) ? _texMouseOver.GetTextureRV() : _texBackground.GetTextureRV();
	dx.SetShaderResources(0, 1, &pRV);
	CShaders::SelectOrthoShader();
	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dx.Draw(54, 0);

	// Draw text
	float x1 = _x, x2 = _x + 68 * pConfig->FontScale, x3 = _w / 2, x4 = x3 + 78 * pConfig->FontScale;
	_tFileName1.Render(x1, _y + 8 * pConfig->FontScale);
	_tFileName2.Render(x2, _y + 8 * pConfig->FontScale);
	_tDateTime1.Render(x3, _y + 8 * pConfig->FontScale);
	_tDateTime2.Render(x4, _y + 8 * pConfig->FontScale);
	_tPlayer1.Render(x1, _y + 28 * pConfig->FontScale);
	_tPlayer2.Render(x2, _y + 28 * pConfig->FontScale);
	_tDay1.Render(x3, _y + 28 * pConfig->FontScale);
	_tDay2.Render(x4, _y + 28 * pConfig->FontScale);
	_tLocation1.Render(x1, _y + 48 * pConfig->FontScale);
	_tLocation2.Render(x2, _y + 48 * pConfig->FontScale);
	_tComment1.Render(x1, _y + 68 * pConfig->FontScale);
	_tComment2.Render(x2, _y + 68 * pConfig->FontScale);
}

void CSaveGameControl::MouseEnter()
{
}

void CSaveGameControl::MouseMove()
{
}

void CSaveGameControl::MouseLeave()
{
}

void CSaveGameControl::MouseButtonDown()
{
}

void CSaveGameControl::MouseButtonUp()
{
}

void CSaveGameControl::KeyDown()
{
}

void CSaveGameControl::KeyUp()
{
}

void CSaveGameControl::GotFocus()
{
}

void CSaveGameControl::LostFocus()
{
	//_clicked();
}

void CSaveGameControl::Init()
{
	DWORD s1, s2;
	PBYTE p1 = GetResource(IDB_SAVEGAMEBOX, L"PNG", &s1);
	PBYTE p2 = GetResource(IDB_BUTTON_MOUSEOVER, L"PNG", &s2);

	_texBackground.Init(p1, s1, "BUTTON1");
	_texMouseOver.Init(p2, s2, "BUTTON2");
}

void CSaveGameControl::Dispose()
{
	_texBackground.Dispose();
	_texMouseOver.Dispose();
}

void CSaveGameControl::SetMouseOver(BOOL mouseOver)
{
	BOOL oldMouseOver = _mouseOver;
	_mouseOver = mouseOver;
}

void CSaveGameControl::SetInfo(SaveGameInfo info)
{
	_info = info;

	// Reinitialize texts
	_tFileName2.SetText((WCHAR*)info.FileName.c_str() + 6);
	_tPlayer2.SetText((char*)info.Player.c_str());
	_tDay2.SetText((char*)info.DayInGame.c_str());
	_tDateTime2.SetText((char*)info.DateTime.c_str());
	_tLocation2.SetText((char*)info.Location.c_str());
	_tComment2.SetText((char*)info.Comment.c_str());
}

void CSaveGameControl::Click()
{
	if (_clicked != NULL && !_isSave)
	{
		_clicked(_info);
	}
}

SaveGameInfo CSaveGameControl::GetInfo()
{
	return _info;
}

void CSaveGameControl::SetFileName(std::wstring fileName)
{
	_info.FileName = fileName;
	_tFileName2.SetText((WCHAR*)fileName.c_str() + 6);
}

void CSaveGameControl::SetComment(std::string comment)
{
	_info.Comment = comment;
	_tComment2.SetText((char*)comment.c_str());
}

void CSaveGameControl::SetPDColours()
{
	_tFileName1.SetColours(0, 0, 0xff3c0c9e, 0);
	_tFileName2.SetColours(0, 0, 0xffae0000, 0);
	_tPlayer1.SetColours(0, 0, 0xff3c0c9e, 0);
	_tPlayer2.SetColours(0, 0, 0xffae0000, 0);
	_tDay1.SetColours(0, 0, 0xff3c0c9e, 0);
	_tDay2.SetColours(0, 0, 0xffae0000, 0);
	_tDateTime1.SetColours(0, 0, 0xff3c0c9e, 0);
	_tDateTime2.SetColours(0, 0, 0xffae0000, 0);
	_tLocation1.SetColours(0, 0, 0xff3c0c9e, 0);
	_tLocation2.SetColours(0, 0, 0xffae0000, 0);
	_tComment1.SetColours(0, 0, 0xff3c0c9e, 0);
	_tComment2.SetColours(0, 0, 0xffae0000, 0);
}
