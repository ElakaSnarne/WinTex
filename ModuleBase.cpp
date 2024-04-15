#include "ModuleBase.h"
#include "Globals.h"
#include "Gamepad.h"
#include "Utilities.h"
#include "Gamepad.h"

CModuleBase::CModuleBase(ModuleType type)
{
	_cursorMinX = 0;
	_cursorMaxX = dx.GetWidth() - 1;
	_cursorMinY = 0;
	_cursorMaxY = dx.GetHeight() - 1;

	Type = type;
	_initialized = FALSE;
}

void CModuleBase::SetCursorClipping()
{
	RECT clientRect{};
	GetClientRect(_hWnd, &clientRect);
	SetCursorClipping(clientRect.left, clientRect.top,
					  clientRect.right, clientRect.bottom);
}

void CModuleBase::SetCursorClipping(int x1, int y1, int x2, int y2)
{
	if (!_cursorIsClipped) {
		GetClipCursor(&_oldClippingArea);
	}
	POINT clientOrigin{ 0,0 };
	ClientToScreen(_hWnd, &clientOrigin);
	RECT clientRect{ x1, y1, x2, y2 };
	clientRect.left += clientOrigin.x;
	clientRect.top += clientOrigin.y;
	clientRect.right += clientOrigin.x;
	clientRect.bottom += clientOrigin.y;
	_currentClippingArea = clientRect;
	ClipCursor(&clientRect);
	_cursorIsClipped = true;
}

void CModuleBase::RefreshCursorClipping()
{
	if (_cursorIsClipped) {
		ClipCursor(&_currentClippingArea);
	}
	else {
		// TODO: Something, probably...
	}
}

void CModuleBase::UnsetCursorClipping()
{
	if (_cursorIsClipped) {
		ClipCursor(&_oldClippingArea);
		_cursorIsClipped = false;
	}
}

void CModuleBase::CenterMouse()
{
	if (GetForegroundWindow() == _hWnd)
	{
		POINT pt;
		pt.x = dx.GetWidth() / 2;
		pt.y = dx.GetHeight() / 2;
		ClientToScreen(_hWnd, &pt);
		SetCursorPos(pt.x, pt.y);

		CInputMapping::IgnoreNextMouseInput = TRUE;
	}
}

void CModuleBase::Init()
{
	if (!_initialized)
	{
		_initialized = TRUE;
		Initialize();
	}
}

void CModuleBase::GotFocus()
{
	ShowCursor(false);
	_hasFocus = true;
	CenterMouse();
	RefreshCursorClipping();
}

void CModuleBase::LostFocus()
{
	ShowCursor(true);
	_hasFocus = false;
	UnsetCursorClipping();
}

void CModuleBase::CreateTexturedRectangle(float top, float left, float bottom, float right, ID3D11Buffer** ppBuffer, char* pName)
{
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

		dx.CreateBuffer(&vertexBufferDesc, &vertexData, ppBuffer, pName);

		delete[] vertices;
	}
}

void CModuleBase::CreateColouredRectangle(float top, float left, float bottom, float right, XMFLOAT4 colour, ID3D11Buffer** ppBuffer, char* pName)
{
	COLOURED_VERTEX_ORTHO* vertices = new COLOURED_VERTEX_ORTHO[4];
	if (vertices != NULL)
	{
		vertices[0].position = XMFLOAT4(right, top, 0.0f, 0.0f);
		vertices[0].colour = colour;

		vertices[1].position = XMFLOAT4(right, bottom, 0.0f, 0.0f);
		vertices[1].colour = colour;

		vertices[2].position = XMFLOAT4(left, top, 0.0f, 0.0f);
		vertices[2].colour = colour;

		vertices[3].position = XMFLOAT4(left, bottom, 0.0f, 0.0f);
		vertices[3].colour = colour;

		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(COLOURED_VERTEX_ORTHO) * 4;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&vertexBufferDesc, &vertexData, ppBuffer, pName);

		delete[] vertices;
	}
}

double Gain(double x)
{
	// Assuming a 10% dead zone
	x = max(0.0, x - 0.1) / 0.9;
	double a = 5.5;
	a = 20.0;
	double y = (pow(a, x) - 1.0) / (a - 1.0);
	return y * 1.0;
}

void CModuleBase::CheckInput()
{
	for (auto action : CInputMapping::ControlsMap)
	{
		if (action.second.IsActive)
		{
			switch (action.first)
			{
				case InputAction::Cursor:
				{
					int x = (action.second.CurrentData >> 16) & 0xffff;
					if (x & 0x8000) x |= ~0xffff;
					int y = action.second.CurrentData & 0xffff;
					if (y & 0x8000) y |= ~0xffff;

					float fx = (float)x;
					float fy = (float)y;

					if (action.second.CurrentSource == InputSource::JoystickAxis)
					{
						// Scale values
						fx = ((fx < 0) ? static_cast<float>(-Gain(-fx / 1000.0f)) : static_cast<float>(Gain(fx / 1000.0f))) * 10.0f;	// TODO: Make gain configurable?
						fy = ((fy < 0) ? static_cast<float>(-Gain(-fy / 1000.0f)) : static_cast<float>(Gain(fy / 1000.0f))) * 10.0f;
					}

					Cursor(fx, fy, (action.second.CurrentSource != InputSource::Mouse));
					break;
				}
				case InputAction::Action:
				{
					if (action.second.CurrentData != 0)
					{
						BeginAction();
					}
					else
					{
						EndAction();
					}
					break;
				}
				case InputAction::Back:
				{
					if (action.second.CurrentData != 0)
					{
						Back();
					}
					break;
				}
				case InputAction::Cycle:
				{
					if (action.second.CurrentData != 0)
					{
						Cycle();
					}
					break;
				}
				case InputAction::MoveForward:
				{
					if (action.second.CurrentSource == InputSource::JoystickAxis)
					{
						// Could be 2 directions
						int x = (action.second.CurrentData >> 16) & 0xffff;
						if (x & 0x8000) x |= ~0xffff;
						int y = action.second.CurrentData & 0xffff;
						if (y & 0x8000) y |= ~0xffff;

						float fx = (float)x;
						float fy = (float)y;

						// Scale values
						fx = (fx < 0) ? static_cast<float>(-Gain(-fx / 1000.0f)) : static_cast<float>(Gain(fx / 1000.0f));
						fy = (fy < 0) ? static_cast<float>(-Gain(-fy / 1000.0f)) : static_cast<float>(Gain(fy / 1000.0f));

						MoveForward(-fy);
						MoveLeft(-fx);
					}
					else
					{
						float v = (float)action.second.CurrentData;
						MoveForward(v);
					}

					break;
				}
				case InputAction::MoveBack:
				{
					if (action.second.CurrentSource != InputSource::JoystickAxis)
					{
						float v = (float)action.second.CurrentData;
						MoveBack(v);
					}
					break;
				}
				case InputAction::MoveLeft:
				{
					if (action.second.CurrentSource != InputSource::JoystickAxis)
					{
						float v = (float)action.second.CurrentData;
						MoveLeft(v);
					}
					break;
				}
				case InputAction::MoveRight:
				{
					if (action.second.CurrentSource != InputSource::JoystickAxis)
					{
						float v = (float)action.second.CurrentData;
						MoveRight(v);
					}
					break;
				}
				case InputAction::MoveUp:
				{
					float v = (float)action.second.CurrentData;
					MoveUp(v);
					break;
				}
				case InputAction::MoveDown:
				{
					float v = (float)action.second.CurrentData;
					MoveDown(v);
					break;
				}
				case InputAction::Run:
				{
					Run((BOOL)action.second.CurrentData);
					break;
				}
				case InputAction::Next:
				{
					if (action.second.CurrentData != 0)
					{
						Next();
					}
					break;
				}
				case InputAction::Prev:
				{
					if (action.second.CurrentData != 0)
					{
						Prev();
					}
					break;
				}
				case InputAction::Inventory:
				{
					if (action.second.CurrentData != 0)
					{
						Inventory();
					}
					break;
				}
				case InputAction::Travel:
				{
					if (action.second.CurrentData != 0)
					{
						Travel();
					}
					break;
				}
				case InputAction::Hints:
				{
					Hints();
					break;
				}
			}

			if (action.second.CurrentSource != InputSource::JoystickAxis || action.second.CurrentData == 0)// InputSource::Mouse || action.second.CurrentSource == InputSource::MouseWheel || (action.second.CurrentSource == InputSource::Key && action.second.CurrentData == 0))
			{
				CInputMapping::ControlsMap[action.first].IsActive = false;
			}
		}
	}
}

void CModuleBase::Cursor(float x, float y, BOOL relative)
{
	_cursorPosX = min(max(relative ? _cursorPosX + x : x, _cursorMinX), _cursorMaxX);
	_cursorPosY = min(max(relative ? _cursorPosY + y : y, _cursorMinY), _cursorMaxY);
}
