#pragma once

#include "DXBase.h"
#include <DirectXMath.h>
#include <list>
#include "DXAdapter.h"

class CDirectX : public CDXBase
{
public:
	CDirectX();
	~CDirectX();

	BOOL Init(HWND hWnd, int width, int height, BOOL windowed, BOOL anisotropicFilter, int bufferCount = 1);
	void Dispose();

	void SetFullScreen(BOOL fullScreen);

	void Clear(float red = 0.0f, float green = 0.0f, float blue = 0.0f);
	void Present(UINT syncInterval = 0, UINT flags = 0);

	HRESULT CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer, char* name = NULL);
	HRESULT Map(ID3D11Resource* pResource, UINT subResource, D3D11_MAP mapType, UINT mapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource);
	void Unmap(ID3D11Resource* pResource, UINT subResource);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	HRESULT CreateTexture2D(D3D11_TEXTURE2D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D, char* name = NULL);
	HRESULT CreateShaderResourceView(ID3D11Resource* pResource, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView, char* name = NULL);

	void SetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets);
	void SetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset);
	void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology);
	void Draw(UINT VertexCount, UINT StartVertexLocation);
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);

	void SetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews);

	void EnableZBuffer();
	void DisableZBuffer();

	int GetWidth() { return _width; }
	int GetHeight() { return _height; }

	void Resize(int width, int height);

	CDXAdapter* GetAdapter();

	LPCTSTR ErrorMessage = NULL;

	void SelectSampler(BOOL anisotropic);
	void SetViewport(D3D11_VIEWPORT viewport);
	void SetScissorRect(D3D11_RECT rect);

protected:
	HWND _hWnd;
	int _width;
	int _height;
	int _multiSamples;

	HRESULT ConfigureBackBuffer();
	HRESULT ReleaseBackBuffer();

	ID3D11Device* _dev;
	ID3D11DeviceContext* _devCon;
	IDXGISwapChain* _swapChain;
	ID3D11RenderTargetView* _backbuffer;
	D3D11_VIEWPORT _viewport;

	ID3D11DepthStencilState* _enabledStencilState;
	ID3D11DepthStencilState* _disabledStencilState;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11RasterizerState* _rasterState;
	ID3D11Texture2D* _depthStencilBuffer;

	ID3D11BlendState* _blendState;

	ID3D11SamplerState* _aaSampleState;
	ID3D11SamplerState* _sampleState;

	std::list<CDXAdapter*> _adapters;
};

void SetDebugName(ID3D11DeviceChild* child, const char* name);
void SetDebugName(IUnknown* unk, const char* name);
