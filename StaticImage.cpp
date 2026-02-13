#include "StaticImage.h"

CStaticImage::CStaticImage(int factor)
{
	_rate = 0;
}

CStaticImage::~CStaticImage()
{
}

BOOL CStaticImage::Init(LPBYTE pData, int length)
{
	if (!CAnimBase::Init(pData, length)) return FALSE;

	if (_texture.Init(pData, length, "IMAGE"))
	{
		_width = _texture.Width();
		_height = _texture.Height();

		CreateBuffers(_width, _height);
		return TRUE;
	}

	return FALSE;
}
