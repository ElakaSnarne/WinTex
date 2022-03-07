#pragma once
#include "DXButton.h"
#include "Texture.h"

class CDXImageButton : public CDXButton
{
public:
	CDXImageButton(int img, void(*onClick)(LPVOID data) = NULL);
	~CDXImageButton();

	static void Init();
	static void Dispose();

	virtual void Render();

protected:
	static CTexture _ibTexBackground;
	static CTexture _ibTexMouseOver;
};
