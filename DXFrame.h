#pragma once
#include <list>
#include "Texture.h"
#include "DXText.h"
#include "DXControl.h"
#include "DXContainer.h"

class CDXFrame : public CDXContainer
{
public:
	CDXFrame(LPSTR title, float w, float h);
	~CDXFrame();

	static void Init();
	static void Dispose();

	virtual void Render();

	virtual void MouseEnter();
	virtual void MouseMove();
	virtual void MouseLeave();
	virtual void MouseButtonDown();
	virtual void MouseButtonUp();
	virtual void KeyDown();
	virtual void KeyUp();
	virtual void GotFocus();
	virtual void LostFocus();

	void SetColours(int colour1, int colour2, int colour3, int colour4);

protected:
	CDXFrame() {}
	static CTexture _texBackground;
	CDXText* _pText{};
	int _textW{};
};
