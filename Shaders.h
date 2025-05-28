#pragma once
#include "DXShader.h"

class CShaders
{
public:
	static void SelectOrthoShader();
	static void SelectTextureShader();
	static void SelectTexFontShader();
	static void SelectMultiColouredFontShader();
	static void SelectMultiColouredFontShaderPD();
	static void SelectColourShader();
	static void SelectTransparentColourShader();
	static void SelectYUVShader();
	static void SelectBasicShader();

	static void Dispose();

	static CDXShader* _orthoShader;
	static CDXShader* _textureShader;
	static CDXShader* _texFontShader;
	static CDXShader* _multiColouredFontShader;
	static CDXShader* _colourShader;
	static CDXShader* _transparentColourShader;
	static CDXShader* _texFontShader_AA;
	static CDXShader* _yuvShader;
	static CDXShader* _basicShader;
};
