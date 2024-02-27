#pragma once

#include "DXBase.h"
#include "ShaderStructs.h"
#include "Enums.h"

class CDXControl : public CDXBase
{
public:
	CDXControl();
	virtual ~CDXControl();

	virtual void Render() = NULL;

	virtual void MouseEnter() { };
	virtual void MouseMove() { };
	virtual void MouseLeave() { };
	virtual void MouseButtonDown() { };
	virtual void MouseButtonUp() { };
	virtual void KeyDown() { };
	virtual void KeyUp() { };
	virtual void GotFocus() { };
	virtual void LostFocus() { };

	virtual float GetWidth() { return _w; }
	virtual float GetHeight() { return _h; }
	virtual void SetWidth(float w) { _w = w; }
	virtual void SetHeight(float h) { _h = h; }
	virtual void SetPosition(float x, float y) { _x = x; _y = y; }
	virtual void SetSize(float w, float h) { _w = w; _h = h; }

	virtual float GetX() { return _x; }
	virtual float GetY() { return _y; }
	virtual void SetX(float x) { _x = x; }
	virtual void SetY(float y) { _y = y; }

	virtual CDXControl* HitTest(float x, float y);

	BOOL GetVisible() { return _visible; }
	void SetVisible(BOOL visible) { _visible = visible; }

	enum class ControlType
	{
		Undefined = 0,
		Bitmap = 1,
		Button = 2,
		Frame = 3,
		CheckBox = 4,
		ImageButton = 5,
		Label = 6,
		DialogueBubble = 7,
		SaveGameControl = 8,
		TabItem = 9,
		Control = 10,
		Slider = 11,
	};

	ControlType GetType() { return _type; }

	BOOL GetMouseOver() { return _mouseOver; }
	virtual void SetMouseOver(BOOL mouseOver) { _mouseOver = mouseOver; }

	virtual CDXControl* GetCurrentMouseOver();

	virtual void SetEnabled(BOOL enabled) { _enabled = enabled; }
	virtual BOOL GetEnabled() { return _enabled; }

	virtual void SetColours(int colour1, int colour2, int colour3, int colour4) {}

protected:
	float _x;
	float _y;
	float _w;
	float _h;
	BOOL _visible;
	BOOL _mouseOver;
	BOOL _enabled;

	ControlType _type;

	ID3D11Buffer* _vertexBuffer;

	BOOL _focus;

	void SetVertex(TEXTURED_VERTEX_ORTHO* pVB, int index, float x, float y, float z, float u, float v);
	void SetQuadVertex(TEXTURED_VERTEX_ORTHO* pVB, int index, float x1, float x2, float y1, float y2, float u1, float u2, float v1, float v2);

	Alignment _alignment;
};
