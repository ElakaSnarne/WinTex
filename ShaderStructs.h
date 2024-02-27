#pragma once

#include "D3DX11-NoWarn.h"
#include <DirectXPackedVector.h>

using namespace DirectX::PackedVector;
using namespace DirectX;

struct TEXTURED_VERTEX_ORTHO
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

struct TEXTURED_VERTEX
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT2 object;			// Use for visibility, then create a visibility buffer for the objects and lookup in the shader
	XMFLOAT4 objectParameters;	// Use for triangle transparency indicator in shader (some textures are used both as opaque and transparent)
};

struct MULTICOLOURED_FONT_VERTEX
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMFLOAT4 colour;
};

struct COLOURED_VERTEX
{
	XMFLOAT4 position;
	XMFLOAT4 colour;
	XMFLOAT4 object;
};

struct COLOURED_VERTEX_ORTHO
{
	XMFLOAT4 position;
	XMFLOAT4 colour;
};

struct VOPBufferType
{
	XMMATRIX view;
	XMMATRIX ortho;
	XMMATRIX projection;
};

struct WorldBufferType
{
	XMMATRIX world;
};

struct MultiColouredFontBufferType
{
	XMVECTOR colour1;
	XMVECTOR colour2;
	XMVECTOR colour3;
	XMVECTOR colour4;
	XMVECTOR colour5;
	XMVECTOR colour6;
};

struct TexFontBufferType
{
	XMVECTOR colour1;
	XMVECTOR colour2;
	XMVECTOR colour3;
	XMVECTOR colour4;
};

struct VisibilityBufferType
{
	XMFLOAT4 visibility[4096];
};

struct TranslationBufferType
{
	XMFLOAT4 translation[256];
};
