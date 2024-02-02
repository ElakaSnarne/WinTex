#include "Shaders.h"
#include "D3D11-NoWarn.h" 
#include "resource.h"
#include "Globals.h"

CDXShader* CShaders::_orthoShader = NULL;
CDXShader* CShaders::_textureShader = NULL;
CDXShader* CShaders::_texFontShader = NULL;
CDXShader* CShaders::_texFontShader_AA = NULL;
CDXShader* CShaders::_multiColouredFontShader = NULL;
CDXShader* CShaders::_colourShader = NULL;
CDXShader* CShaders::_transparentColourShader = NULL;
CDXShader* CShaders::_yuvShader = NULL;
CDXShader* CShaders::_basicShader = NULL;

const char* vertexShaderProfile = "vs_5_0";
const char* pixelShaderProfile = "ps_5_0";

void CShaders::SelectOrthoShader()
{
	if (_orthoShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC tsied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_orthoShader = new CDXShader(&dx, IDR_SHADER, "OrthoVS", vertexShaderProfile, "TexturedPS", pixelShaderProfile, tsied, 4);
	}

	_orthoShader->Activate(&dx);
}

void CShaders::SelectTextureShader()
{
	if (_textureShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC tsied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_textureShader = new CDXShader(&dx, IDR_SHADER, "TexturedVS", vertexShaderProfile, "TexturedPS", pixelShaderProfile, tsied, 4);
	}

	_textureShader->Activate(&dx);
}

void CShaders::SelectTexFontShader()
{
	if (rintf(pConfig->FontScale) == pConfig->FontScale)
	{
		// Integer, use normal shader
		if (_texFontShader == NULL)
		{
			D3D11_INPUT_ELEMENT_DESC tsied[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			_texFontShader = new CDXShader(&dx, IDR_SHADER, "OrthoVS", vertexShaderProfile, "TexFontPS", pixelShaderProfile, tsied, 2);
		}

		_texFontShader->Activate(&dx);
	}
	else
	{
		// Fraction, use AA shader
		if (_texFontShader_AA == NULL)
		{
			D3D11_INPUT_ELEMENT_DESC tsied[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			_texFontShader_AA = new CDXShader(&dx, IDR_SHADER, "OrthoVS", vertexShaderProfile, "TexFontPS_AA", pixelShaderProfile, tsied, 2);
		}

		_texFontShader_AA->Activate(&dx);
	}
}

void CShaders::SelectMultiColouredFontShader()
{
	if (_multiColouredFontShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC tsied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_multiColouredFontShader = new CDXShader(&dx, IDR_SHADER, "MultiColouredFontVS", vertexShaderProfile, "MultiColouredFontPS", pixelShaderProfile, tsied, 3);
	}

	_multiColouredFontShader->Activate(&dx);
}

void CShaders::SelectColourShader()
{
	if (_colourShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC csied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_colourShader = new CDXShader(&dx, IDR_SHADER, "ColouredVS", vertexShaderProfile, "ColouredPS", pixelShaderProfile, csied, 2);
	}

	_colourShader->Activate(&dx);
}

void CShaders::SelectTransparentColourShader()
{
	if (_transparentColourShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC csied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_transparentColourShader = new CDXShader(&dx, IDR_SHADER, "TransparentVS", vertexShaderProfile, "TransparentPS", pixelShaderProfile, csied, 3);
	}

	_transparentColourShader->Activate(&dx);
}

void CShaders::SelectYUVShader()
{
	if (_yuvShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC tsied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_yuvShader = new CDXShader(&dx, IDR_SHADER, "OrthoVS", vertexShaderProfile, "YUVPS", pixelShaderProfile, tsied, 2);
	}

	_yuvShader->Activate(&dx);
}

void CShaders::SelectBasicShader()
{
	if (_basicShader == NULL)
	{
		D3D11_INPUT_ELEMENT_DESC tsied[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		_basicShader = new CDXShader(&dx, IDR_SHADER, "BasicVS", vertexShaderProfile, "BasicPS", pixelShaderProfile, tsied, 1);
	}

	_basicShader->Activate(&dx);
}

void CShaders::Dispose()
{
	if (_orthoShader != NULL)
	{
		_orthoShader->Dispose();
		_orthoShader = NULL;
	}

	if (_textureShader != NULL)
	{
		_textureShader->Dispose();
		_textureShader = NULL;
	}

	if (_texFontShader != NULL)
	{
		_texFontShader->Dispose();
		_texFontShader = NULL;
	}

	if (_multiColouredFontShader != NULL)
	{
		_multiColouredFontShader->Dispose();
		_multiColouredFontShader = NULL;
	}

	if (_colourShader != NULL)
	{
		_colourShader->Dispose();
		_colourShader = NULL;
	}

	if (_transparentColourShader != NULL)
	{
		_transparentColourShader->Dispose();
		_transparentColourShader = NULL;
	}

	if (_yuvShader != NULL)
	{
		_yuvShader->Dispose();
		_yuvShader = NULL;
	}

	if (_basicShader != NULL)
	{
		_basicShader->Dispose();
		_basicShader = NULL;
	}
}
