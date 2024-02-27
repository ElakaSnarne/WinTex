#pragma once

#include <Windows.h>
#include "Map.h"
#include "D3D11-NoWarn.h"
#include "D3DX11-NoWarn.h"
#include <DirectXPackedVector.h>
#include "InputMapping.h"
#include "Enums.h"

class CControllerData;

using namespace DirectX::PackedVector;
using namespace DirectX;

class CModuleBase {
public:
	CModuleBase(ModuleType type);
	virtual ~CModuleBase() {};

	ModuleType Type;

	void Init();
	virtual void Resize(int width, int height) = NULL;
	virtual void GotFocus();
	virtual void LostFocus();
	virtual void Pause() {}
	virtual void Resume() {}
	virtual void Dispose() {}
	virtual void Render() {}
	virtual void MouseMove(POINT pt) {}
	virtual void MouseDown(POINT pt, int btn) {}
	virtual void MouseUp(POINT pt, int btn) {}
	virtual void MouseWheel(int scroll) {}
	virtual void KeyDown(WPARAM key, LPARAM lParam) {}
	virtual void KeyUp(WPARAM key, LPARAM lParam) {}

	virtual void GamepadInput(InputSource source, int offset, int data) {}

	// Input related
	virtual void CheckInput();
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction() {}
	virtual void EndAction() {}
	virtual void Back() {}
	virtual void Cycle() {}
	virtual void MoveForward(float v) {}
	virtual void MoveBack(float v) {}
	virtual void MoveLeft(float v) {}
	virtual void MoveRight(float v) {}
	virtual void MoveUp(float y) {}
	virtual void MoveDown(float y) {}
	virtual void Run(BOOL run) {}
	virtual void Next() {}
	virtual void Prev() {}
	virtual void Inventory() {}
	virtual void Travel() {}
	virtual void Hints() {}
	virtual void SetCursorClipping();
	virtual void SetCursorClipping(int x1, int y1, int x2, int y2);
	virtual void RefreshCursorClipping();
	virtual void UnsetCursorClipping();

	float _cursorPosX = 0.0f;
	float _cursorPosY = 0.0f;

protected:
	virtual void Initialize() {}

	void CenterMouse();

	void CreateTexturedRectangle(float top, float left, float bottom, float right, ID3D11Buffer** ppBuffer, char* pName);
	void CreateColouredRectangle(float top, float left, float bottom, float right, XMFLOAT4 colour, ID3D11Buffer** ppBuffer, char* pName);

	int _cursorMinX;
	int _cursorMaxX;
	int _cursorMinY;
	int _cursorMaxY;

	BOOL _initialized;

	RECT _oldClippingArea;
	RECT _currentClippingArea;
	bool _cursorIsClipped;
	bool _hasFocus{ true };
};
