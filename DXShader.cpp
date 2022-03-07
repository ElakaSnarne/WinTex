#include "DXShader.h"
#include <d3dcompiler.h>

CDXShader::CDXShader(CDirectX* pDX, int resource, LPCSTR vsFunctionName, LPCSTR vsProfileName, LPCSTR psFunctionName, LPCSTR psProfileName, D3D11_INPUT_ELEMENT_DESC* ied, int numDescriptors)
{
	_vs = NULL;
	_ps = NULL;
	_layout = NULL;

	HRSRC hShader = FindResource(NULL, MAKEINTRESOURCE(resource), L"SHADER");
	DWORD size = SizeofResource(NULL, hShader);
	HGLOBAL hShaderGlobal = LoadResource(NULL, hShader);
	char* pShader = (char*)LockResource(hShaderGlobal);

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	// Create vertex shader
	ID3D10Blob* pVSb;
	D3DX11CompileFromMemory(pShader, size, "SHADER", NULL, NULL, vsFunctionName, vsProfileName, flags, 0, NULL, &pVSb, NULL, NULL);
	pDX->GetDevice()->CreateVertexShader(pVSb->GetBufferPointer(), pVSb->GetBufferSize(), NULL, &_vs);

	// Create input layout
	pDX->GetDevice()->CreateInputLayout(ied, numDescriptors, pVSb->GetBufferPointer(), pVSb->GetBufferSize(), &_layout);
	if (pVSb != NULL)pVSb->Release();
	pVSb = NULL;

	// Create pixel shader
	ID3D10Blob* pPSb;
	D3DX11CompileFromMemory(pShader, size, "SHADER", NULL, NULL, psFunctionName, psProfileName, 0, 0, NULL, &pPSb, NULL, NULL);
	pDX->GetDevice()->CreatePixelShader(pPSb->GetBufferPointer(), pPSb->GetBufferSize(), NULL, &_ps);
	if (pPSb != NULL) pPSb->Release();
	pPSb = NULL;
}

CDXShader::~CDXShader()
{
	Dispose();
}

void CDXShader::Activate(CDirectX* pDX)
{
	pDX->GetDeviceContext()->VSSetShader(_vs, 0, 0);
	pDX->GetDeviceContext()->PSSetShader(_ps, 0, 0);
	pDX->GetDeviceContext()->IASetInputLayout(_layout);
}

void CDXShader::Dispose()
{
	if (_vs != NULL)
	{
		_vs->Release();
		_vs = NULL;
	}

	if (_ps != NULL)
	{
		_ps->Release();
		_ps = NULL;
	}

	if (_layout != NULL)
	{
		_layout->Release();
		_layout = NULL;
	}
}
