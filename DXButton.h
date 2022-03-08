#pragma once
#include "DXControl.h"
#include "DXText.h"
#include "Texture.h"
#include "DXSound.h"

class CDXButton : public CDXControl
{
public:
	CDXButton(LPSTR text, float w, float h, void(*onClick)(LPVOID) = NULL, LPVOID data = NULL);
	virtual ~CDXButton();

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

	virtual void Click();

	virtual void SetMouseOver(BOOL mouseOver);

protected:
	void(*_clicked)(LPVOID data);
	LPVOID _data{};

	static CTexture _texBackground;
	static CTexture _texMouseOver;
	CDXText* _pText;

	float _textX;
	float _textY;

	static CDXSound* _pSound;

	CDXButton() { _textX = 0.0f; _textY = 0.0f; _clicked = NULL; _pText = NULL; }
};
