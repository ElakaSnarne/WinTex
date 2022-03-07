#pragma once
#include "DXControl.h"
#include "Texture.h"

class CDXBitmap : public CDXControl
{
public:
	CDXBitmap();
	CDXBitmap(PWCHAR fileName);
	CDXBitmap(PBYTE pImage, DWORD size);
	CDXBitmap(int width, int height);
	~CDXBitmap();

	virtual void Render();

	CTexture* GetTexture() { return &_texture; }

protected:
	CTexture _texture;

	void Init();
};
