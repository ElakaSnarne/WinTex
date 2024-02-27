#pragma once

#include "DirectX.h"
#include "D3D11-NoWarn.h"
//#include <D3DX10math.h>
#include "ShaderStructs.h"

class CConstantBuffers
{
public:
	static void SetVOP(CDirectX& dx, XMMATRIX* view, XMMATRIX* ortho, XMMATRIX* projection);
	static void SetWorld(CDirectX& dx, XMMATRIX* world);
	static void SetTexFont(CDirectX& dx, XMVECTOR* colour1, XMVECTOR* colour2, XMVECTOR* colour3, XMVECTOR* colour4);
	static void SetVisibility(CDirectX& dx, VisibilityBufferType visibility);
	static void SetTranslation(CDirectX& dx, TranslationBufferType translation);
	static void SetMultiColouredFont(CDirectX& dx, XMVECTOR* colour1, XMVECTOR* colour2, XMVECTOR* colour3, XMVECTOR* colour4, XMVECTOR* colour5, XMVECTOR* colour6);

	static void Setup2D(CDirectX& dx);
	static void Setup3D(CDirectX& dx);

	static void Dispose();

	static ID3D11Buffer* _vop;
	static ID3D11Buffer* _world;
	static ID3D11Buffer* _texFont;
	static ID3D11Buffer* _multiColouredFont;

	static ID3D11Buffer* _visibility;
	static ID3D11Buffer* _translation;

private:
	static void SetupVOP(CDirectX& pDX, float w, float h, float camera_x, float camera_y, float camera_z);
};
