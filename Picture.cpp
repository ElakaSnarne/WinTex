#include "Picture.h"

CPicture::CPicture()
{
}

CPicture::~CPicture()
{
}

BOOL CPicture::Init(LPBYTE pData, int length)
{
	int w = 0, h = 0;

	CreateBuffers(w, h);

	return FALSE;
}

BOOL CPicture::DecodeFrame()
{
	return FALSE;
}
