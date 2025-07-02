#pragma once

#include "Texture.h"
#include <unordered_map>
#include <vector>

class CPuzzlePiece
{
public:
	static std::unordered_map<int, CPuzzlePiece*> Images;

	CTexture Texture;
	float X;
	float Y;
	int Orientation;
	LPBYTE RawImage;
	float Width;
	float Height;
	int OriginalWidth;
	int OriginalHeight;
	int Offset;
	int Z;

	static void Reset();
	static void Add(int index, LPBYTE pImage, float scale, int palette[], int numberOfImages, float left, float top, int positionOffset);
	static void Sort();
	static CPuzzlePiece* Get(int index);
	static void Dispose();
	static void Render();
	static CPuzzlePiece* HitTest(float mx, float my, int offset, float scale);

protected:
	static std::vector<CPuzzlePiece*> ZImages;
	static float rotationAngles[];
	static ID3D11Buffer* vertexBuffer;
};
