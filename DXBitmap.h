#pragma once
#include "DXControl.h"
#include "Texture.h"

class CDXBitmap : public CDXControl
{
public:
	CDXBitmap();
	CDXBitmap(PWCHAR fileName, Alignment alignment = Alignment::Default);
	CDXBitmap(PBYTE pImage, DWORD size, Alignment alignment = Alignment::Default);
	CDXBitmap(int width, int height, Alignment alignment = Alignment::Default);
	~CDXBitmap();

	virtual void Render();

	CTexture* GetTexture() { return &_texture; }

protected:
	CTexture _texture;

	void Init();
};
