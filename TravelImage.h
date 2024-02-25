#pragma once

#include "Texture.h"

class CTravelImage
{
public:
	float Top;
	float Left;
	float Bottom;
	float Right;
	CTexture Texture;
	ID3D11Buffer* Buffer;

	void Render();
	void Render(float x, float y);

	BOOL HitTest(float x, float y) { return (x >= Left && x < Right&& y >= -Top && y < -Bottom); }
};
