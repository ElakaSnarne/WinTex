#pragma once
#include "DXBase.h"
#include "DirectX.h"

class CDXShader : public CDXBase
{
public:
	CDXShader(CDirectX* pDX, int resource, LPCSTR vsFunctionName, LPCSTR vsProfileName, LPCSTR psFunctionName, LPCSTR psProfileName, D3D11_INPUT_ELEMENT_DESC* ied, int numDescriptors);
	~CDXShader();

	void Dispose();

	ID3D11VertexShader* _vs;
	ID3D11PixelShader* _ps;
	ID3D11InputLayout* _layout;

	void Activate(CDirectX* pDX);
};
