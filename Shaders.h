#pragma once
#include "DXShader.h"

class CShaders
{
public:
	static void SelectOrthoShader();
	static void SelectTextureShader();
	static void SelectFontShader();
	static void SelectTexFontShader();
	static void SelectColourShader();
	static void SelectTransparentColourShader();

	static void Dispose();

	static CDXShader* _orthoShader;
	static CDXShader* _textureShader;
	static CDXShader* _fontShader;
	static CDXShader* _texFontShader;
	static CDXShader* _colourShader;
	static CDXShader* _transparentColourShader;
};
