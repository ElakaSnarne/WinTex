#define STB_IMAGE_IMPLEMENTATION
#include "DirectX.h"
#include "resource.h"
#include "Globals.h"
#include <sstream>
#include <string>

#ifndef PLATFORM_LINUX

#define D3D11_CREATE_DEVICE_VIDEO_SUPPORT	0x800

void Disaster(HRESULT hr, LPWSTR text)
{
	std::wstringstream value;
	value << std::hex << hr;
	auto buffer = std::wstring(text) + L" : " + value.str();
	
	MessageBox(NULL, buffer.c_str(), L"Disaster!", 0);
}

void SetDebugName(ID3D11DeviceChild* child, const char* name)
{
	if (child != nullptr && name != nullptr)
	{
		child->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<int>(strlen(name)), name);
	}
}

void SetDebugName(IUnknown* unk, const char* name)
{
	if (unk != nullptr && name != nullptr)
	{
		ID3D11DeviceChild* child;
		unk->QueryInterface(IID_ID3D11DeviceChild, (void**)&child);
		if (child != NULL)
		{
			child->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<int>(strlen(name)), name);
		}
	}
}

CDirectX::CDirectX()
{
	_width = 0;
	_height = 0;

	_dev = NULL;
	_devCon = NULL;
	_swapChain = NULL;
	_backbuffer = NULL;

	_aaSampleState = NULL;
	_sampleState = NULL;

	_enabledStencilState = NULL;
	_disabledStencilState = NULL;
	_depthStencilView = NULL;
	_depthStencilBuffer = NULL;
	_rasterState = NULL;

	_blendState = NULL;

	// Enumerate adaptors and modes
	IDXGIFactory* pFactory = NULL;
	IDXGIAdapter* pAdapter = NULL;
	if (CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory) == S_OK)
	{
		for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			_adapters.push_back(new CDXAdapter(pAdapter));
		}
		pFactory->Release();
	}
}

CDirectX::~CDirectX()
{
	Dispose();
}

BOOL CDirectX::Init(HWND hWnd, int width, int height, BOOL windowed, BOOL anisotropicFilter, int bufferCount)
{
	_hWnd = hWnd;
	_width = width;
	_height = height;

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 8;
	scd.Windowed = windowed;

	// TODO: Check if creation fails when VIDEO_SUPPORT is requested, re-attempt creation without (will have to disable external video playback)
	HRESULT hr;
	//short buffer[128];
	D3D_FEATURE_LEVEL features = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	while (scd.SampleDesc.Count > 0)
	{
		hr = D3D11CreateDeviceAndSwapChain(NULL,										// Adaptor
			D3D_DRIVER_TYPE_HARDWARE,													// Driver type
			NULL,																		// Software
#ifdef _DEBUG
			D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,				// Flags, debug mode
#else
			D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_VIDEO_SUPPORT,		// Flags, release mode
#endif
			& features,																	// Feature levels list
			1,																			// Number of feature levels
			D3D11_SDK_VERSION,															// SDK version
			&scd,																		// Swap chain descriptor
			&_swapChain,																// Swap chain
			&_dev,																		// Device
			NULL,																		//	Feature level (output)
			&_devCon);																	// Device context
		if (hr == S_OK)
		{
			//wchar_t buffer[10];
			//_itow(actualFeatures, buffer, 16);
			//MessageBox(NULL, buffer, L"Features", MB_OK);
			break;
		}
		else
		{
			scd.SampleDesc.Count >>= 1;
		}
	}

	if (scd.SampleDesc.Count == 0)
	{
		Disaster(hr, L"Failed to create D3D11 device");
		return FALSE;
	}

	SetDebugName(_swapChain, "SwapChain");
	_multiSamples = scd.SampleDesc.Count;

	// get the address of the back buffer
	ID3D11Texture2D* pBackBuffer;
	_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if ((hr = _dev->CreateRenderTargetView(pBackBuffer, NULL, &_backbuffer)) != S_OK)
	{
		Disaster(hr, L"Failed to create render target view");
		return FALSE;
	}
	SetDebugName(_backbuffer, "BackBuffer");

	DXGI_FORMAT f1 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT f2 = DXGI_FORMAT_D24_UNORM_S8_UINT;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = f1;
	depthBufferDesc.SampleDesc.Count = scd.SampleDesc.Count;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	if ((hr = _dev->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer)) != S_OK)
	{
		Disaster(hr, L"Failed to create depth buffer");
		//UINT levels = 0;
		//_dev->CheckMultisampleQualityLevels(depthBufferDesc.Format, scd.SampleDesc.Count, &levels);
		//Disaster(levels, L"Multisample quality levels");
		return FALSE;
	}
	SetDebugName(_depthStencilBuffer, "DepthStencilBuffer");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = f2;
	depthStencilViewDesc.ViewDimension = (scd.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	if ((hr = _dev->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView)) != S_OK)
	{
		Disaster(hr, L"Failed to create depth stencil view");
		return FALSE;
	}
	SetDebugName(_depthStencilView, "DepthStencilView");

	// Set the viewport
	ZeroMemory(&_viewport, sizeof(D3D11_VIEWPORT));
	_viewport.Width = static_cast<float>(width);
	_viewport.Height = static_cast<float>(height);
	_viewport.MinDepth = 0.0;
	_viewport.MaxDepth = 1.0;
	_devCon->RSSetViewports(1, &_viewport);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	if ((hr = _dev->CreateDepthStencilState(&depthStencilDesc, &_enabledStencilState)) != S_OK)
	{
		Disaster(hr, L"Failed to create depth stencil");
		return FALSE;
	}
	SetDebugName(_enabledStencilState, "EnabledStencilState");
	EnableZBuffer();

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = FALSE;
	if ((hr = _dev->CreateDepthStencilState(&depthStencilDesc, &_disabledStencilState)) != S_OK)
	{
		Disaster(hr, L"Failed to create depth stencil");
		return FALSE;
	}
	SetDebugName(_disabledStencilState, "DisabledStencilState");

	pBackBuffer->Release();
	pBackBuffer = NULL;

	_devCon->OMSetRenderTargets(1, &_backbuffer, _depthStencilView);

	_aaSampleState = NULL;
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if ((hr = _dev->CreateSamplerState(&samplerDesc, &_aaSampleState)) != S_OK)
	{
		Disaster(hr, L"Failed to create AA sampler state");
		return FALSE;
	}
	SetDebugName(_aaSampleState, "AASampleState");

	_sampleState = NULL;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	if ((hr = _dev->CreateSamplerState(&samplerDesc, &_sampleState)) != S_OK)
	{
		Disaster(hr, L"Failed to create sampler state");
		return FALSE;
	}
	SetDebugName(_sampleState, "SampleState");

	_devCon->PSSetSamplers(0, 1, anisotropicFilter ? &_aaSampleState : &_sampleState);
	_devCon->PSSetSamplers(1, 1, &_sampleState);

	// Setup blend to allow for alpha
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	if ((hr = _dev->CreateBlendState(&blendDesc, &_blendState)) != S_OK)
	{
		Disaster(hr, L"Failed to create blend state");
		return FALSE;
	}
	SetDebugName(_blendState, "BlendState");
	_devCon->OMSetBlendState(_blendState, NULL, 0xffffffff);

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = TRUE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.MultisampleEnable = TRUE;
	rasterDesc.ScissorEnable = TRUE;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	D3D11_RECT rect{ 0, 0, width, height };
	_devCon->RSSetScissorRects(1, &rect);

	// Create the rasterizer state from the description we just filled out.
	if ((hr = _dev->CreateRasterizerState(&rasterDesc, &_rasterState)) != S_OK)
	{
		Disaster(hr, L"Failed to create rasterizer state");
		return FALSE;
	}
	SetDebugName(_rasterState, "RasterState");
	_devCon->RSSetState(_rasterState);

	return TRUE;
}

void CDirectX::SetFullScreen(BOOL fullScreen)
{
	if (_swapChain != NULL)
	{
		HRESULT res = _swapChain->SetFullscreenState(fullScreen, NULL);
	}
}

void CDirectX::Dispose()
{
	if (_swapChain != NULL)
	{
		// switch to windowed mode
		_swapChain->SetFullscreenState(FALSE, NULL);
	}

	if (_aaSampleState != NULL)
	{
		_aaSampleState->Release();
		_aaSampleState = NULL;
	}

	if (_sampleState != NULL)
	{
		_sampleState->Release();
		_sampleState = NULL;
	}

	if (_blendState != NULL)
	{
		_blendState->Release();
		_blendState = NULL;
	}

	if (_depthStencilBuffer != NULL)
	{
		_depthStencilBuffer->Release();
		_depthStencilBuffer = NULL;
	}

	if (_rasterState != NULL)
	{
		_rasterState->Release();
		_rasterState = NULL;
	}

	if (_depthStencilView != NULL)
	{
		_depthStencilView->Release();
		_depthStencilView = NULL;
	}

	if (_disabledStencilState != NULL)
	{
		_disabledStencilState->Release();
		_disabledStencilState = NULL;
	}

	if (_enabledStencilState != NULL)
	{
		_enabledStencilState->Release();
		_enabledStencilState = NULL;
	}

	if (_backbuffer != NULL)
	{
		_backbuffer->Release();
		_backbuffer = NULL;
	}

	if (_swapChain != NULL)
	{
		_swapChain->Release();
		_swapChain = NULL;
	}

	if (_devCon != NULL)
	{
		_devCon->Release();
		_devCon = NULL;
	}

	// Dispose adapters
	std::list<CDXAdapter*>::iterator ait = _adapters.begin();
	std::list<CDXAdapter*>::iterator aend = _adapters.end();
	while (ait != aend)
	{
		delete* (ait++);
	}
	_adapters.clear();

	if (_dev != NULL)
	{
#if _DEBUG
		ID3D11Debug* debug;
		HRESULT hr = _dev->QueryInterface(IID_ID3D11Debug, (void**)&debug);
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debug->Release();
#endif

		_dev->Release();
		_dev = NULL;
	}
}

ID3D11Device* CDirectX::GetDevice()
{
	return _dev;
}

ID3D11DeviceContext* CDirectX::GetDeviceContext()
{
	return _devCon;
}

void CDirectX::Clear(float red, float green, float blue)
{
	if (_backbuffer != NULL && _depthStencilView != NULL)
	{
		XMFLOAT4 col(red, green, blue, 1.0f);
		_devCon->ClearRenderTargetView(_backbuffer, &col.x);
		_devCon->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0xff);
	}
}

void CDirectX::Present(UINT syncInterval, UINT flags)
{
	_swapChain->Present(syncInterval, flags);
}

HRESULT CDirectX::CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer, char* name)
{
	if (name == NULL)
	{
		int dd = 0;
	}

	HRESULT res = _dev->CreateBuffer(pDesc, pInitialData, ppBuffer);
	if (res != S_OK)
	{
		Disaster(res, L"Failed to create buffer");
	}
	else if (name != NULL)
	{
		//WCHAR ttt[256];
		//mbstowcs(ttt, name, 200);
		//Disaster(res, ttt);
		SetDebugName(*ppBuffer, name);
	}
	return res;
}

HRESULT CDirectX::Map(ID3D11Resource* pResource, UINT subResource, D3D11_MAP mapType, UINT mapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource)
{
	return _devCon->Map(pResource, subResource, mapType, mapFlags, pMappedResource);
}

void CDirectX::Unmap(ID3D11Resource* pResource, UINT subResource)
{
	if (_lock.Lock())
	{
		_devCon->Unmap(pResource, subResource);
		_lock.Release();
	}
}

void CDirectX::EnableZBuffer()
{
	_devCon->OMSetDepthStencilState(_enabledStencilState, 1);
}

void CDirectX::DisableZBuffer()
{
	_devCon->OMSetDepthStencilState(_disabledStencilState, 1);
}

void CDirectX::SetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets)
{
	_devCon->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
}

void CDirectX::SetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
{
	_devCon->IASetIndexBuffer(pIndexBuffer, Format, Offset);
}

void CDirectX::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology)
{
	_devCon->IASetPrimitiveTopology(Topology);
}

void CDirectX::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	_devCon->Draw(VertexCount, StartVertexLocation);
}

void CDirectX::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	_devCon->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

HRESULT CDirectX::CreateTexture2D(D3D11_TEXTURE2D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D, char* name)
{
	HRESULT res = _dev->CreateTexture2D(pDesc, pInitialData, ppTexture2D);
	if (res == S_OK && name != NULL) SetDebugName(*ppTexture2D, name);
	return res;
}

HRESULT CDirectX::CreateShaderResourceView(ID3D11Resource* pResource, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView, char* name)
{
	HRESULT res = _dev->CreateShaderResourceView(pResource, pDesc, ppSRView);
	if (res == S_OK && name != NULL) SetDebugName(*ppSRView, name);
	return res;
}

void CDirectX::SetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	_devCon->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

HRESULT CDirectX::ConfigureBackBuffer()
{
	ID3D11Texture2D* pBackBuffer;
	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	hr = _dev->CreateRenderTargetView(pBackBuffer, NULL, &_backbuffer);
	SetDebugName(_backbuffer, "BackBuffer");
	pBackBuffer->Release();

	DXGI_FORMAT f1 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT f2 = DXGI_FORMAT_D24_UNORM_S8_UINT;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = _width;
	depthBufferDesc.Height = _height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = f1;
	depthBufferDesc.SampleDesc.Count = _multiSamples;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	hr = _dev->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = f2;
	depthStencilViewDesc.ViewDimension = (_multiSamples > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = _dev->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);

	ZeroMemory(&_viewport, sizeof(D3D11_VIEWPORT));
	_viewport.Height = (float)_height;
	_viewport.Width = (float)_width;
	_viewport.MinDepth = 0.0;
	_viewport.MaxDepth = 1.0;
	_devCon->RSSetViewports(1, &_viewport);

	return hr;
}

HRESULT CDirectX::ReleaseBackBuffer()
{
	HRESULT hr = S_OK;

	if (_backbuffer != NULL)
	{
		_backbuffer->Release();
		_backbuffer = NULL;
	}

	if (_depthStencilView != NULL)
	{
		_depthStencilView->Release();
		_depthStencilView = NULL;
	}

	if (_depthStencilBuffer != NULL)
	{
		_depthStencilBuffer->Release();
		_depthStencilBuffer = NULL;
	}

	_devCon->Flush();

	return hr;
}

void CDirectX::Resize(int width, int height)
{
	// Recreate backbuffer and depthbuffer
	if (_swapChain != NULL && _width != width && _height != height)
	{
		_width = width;
		_height = height;

		ReleaseBackBuffer();

		DXGI_MODE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Width = width;
		desc.Height = height;
		HRESULT res = _swapChain->ResizeTarget(&desc);

		res = _swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		ConfigureBackBuffer();
		_devCon->OMSetRenderTargets(1, &_backbuffer, _depthStencilView);

		D3D11_RECT rect{ 0, 0, width, height };
		_devCon->RSSetScissorRects(1, &rect);
	}
}

CDXAdapter* CDirectX::GetAdapter()
{
	return (_adapters.size() > 0) ? _adapters.front() : NULL;
}

void CDirectX::SelectSampler(BOOL anisotropic)
{
	_devCon->PSSetSamplers(0, 1, anisotropic ? &_aaSampleState : &_sampleState);
}

void CDirectX::SetViewport(D3D11_VIEWPORT viewport)
{
	_devCon->RSSetViewports(1, &viewport);
}

void CDirectX::SetScissorRect(D3D11_RECT rect)
{
	_devCon->RSSetScissorRects(1, &rect);
}

#else

#include <iostream>

// Linux Implementation
void Disaster(HRESULT hr, LPWSTR text) {
}
void SetDebugName(ID3D11DeviceChild* child, const char* name) {}
void SetDebugName(IUnknown* unk, const char* name) {}

static SDL_Window* g_Window = NULL;
static SDL_GLContext g_GLContext = NULL;
static GLuint g_VAO = 0;

CDirectX::CDirectX() {
    _dev = new ID3D11Device();
    _devCon = new ID3D11DeviceContext();
    _width = 0;
    _height = 0;
}

CDirectX::~CDirectX() {
    Dispose();
}

BOOL CDirectX::Init(HWND hWnd, int width, int height, BOOL windowed, BOOL anisotropicFilter, int bufferCount) {
    _width = width;
    _height = height;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return FALSE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (!windowed) flags |= SDL_WINDOW_FULLSCREEN;

    g_Window = SDL_CreateWindow("WinTex SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (!g_Window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return FALSE;
    }

    g_GLContext = SDL_GL_CreateContext(g_Window);
    if (!g_GLContext) {
        std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        return FALSE;
    }

    // Initialize generic GL state
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE); // Disable culling to be safe

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return TRUE;
}

void CDirectX::Dispose() {
    if (g_VAO) { glDeleteVertexArrays(1, &g_VAO); g_VAO = 0; }
    if (_dev) { delete _dev; _dev = NULL; }
    if (_devCon) { delete _devCon; _devCon = NULL; }
    if (g_GLContext) { SDL_GL_DeleteContext(g_GLContext); g_GLContext = NULL; }
    if (g_Window) { SDL_DestroyWindow(g_Window); g_Window = NULL; }
    SDL_Quit();
}

void CDirectX::SetFullScreen(BOOL fullScreen) {
    if (g_Window) {
        SDL_SetWindowFullscreen(g_Window, fullScreen ? SDL_WINDOW_FULLSCREEN : 0);
    }
}

void CDirectX::Clear(float red, float green, float blue) {
    //if (red == 0.0f && green == 0.0f && blue == 0.0f) {
    //    red = 1.0f; green = 0.0f; blue = 1.0f; // Magenta for debugging
    //}
    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CDirectX::Present(UINT syncInterval, UINT flags) {
    if (g_Window) {
        SDL_GL_SwapWindow(g_Window);
    }
}

HRESULT CDirectX::CreateBuffer(D3D11_BUFFER_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer, char* name) { 
    *ppBuffer = new ID3D11Buffer();
    (*ppBuffer)->byteWidth = pDesc->ByteWidth;
    (*ppBuffer)->bindFlags = pDesc->BindFlags;
    (*ppBuffer)->cpuData.resize(pDesc->ByteWidth);

    if (pDesc->BindFlags & D3D11_BIND_CONSTANT_BUFFER) {
        // Check if this is a large buffer that needs UBO (Visibility/Translation)
        bool createUBO = false;
        if (name != NULL) {
            if (strcmp(name, "Visibility") == 0 || strcmp(name, "Translation") == 0 || strcmp(name, "TexFont") == 0) {
                createUBO = true;
            }
        }

        if (createUBO) {
             glGenBuffers(1, &(*ppBuffer)->glId);
             glBindBuffer(GL_UNIFORM_BUFFER, (*ppBuffer)->glId);
             // Initialize with CPU data if available, or just allocate
             if (pInitialData) {
                 glBufferData(GL_UNIFORM_BUFFER, pDesc->ByteWidth, pInitialData->pSysMem, GL_DYNAMIC_DRAW);
                 memcpy((*ppBuffer)->cpuData.data(), pInitialData->pSysMem, pDesc->ByteWidth);
             } else {
                 glBufferData(GL_UNIFORM_BUFFER, pDesc->ByteWidth, NULL, GL_DYNAMIC_DRAW);
             }
             glBindBuffer(GL_UNIFORM_BUFFER, 0);
        } else {
             // CPU-only for emulation of small constant buffers (matrices etc)
             if (pInitialData) {
                 memcpy((*ppBuffer)->cpuData.data(), pInitialData->pSysMem, pDesc->ByteWidth);
             }
        }
        return S_OK;
    }

    // For Vertex/Index buffers, create GL buffer
    glGenBuffers(1, &(*ppBuffer)->glId);
    
    GLenum target = (pDesc->BindFlags & D3D11_BIND_VERTEX_BUFFER) ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
    
    glBindBuffer(target, (*ppBuffer)->glId);
    if (pInitialData) {
        glBufferData(target, pDesc->ByteWidth, pInitialData->pSysMem, (pDesc->Usage == D3D11_USAGE_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        // Also keep copy in cpuData for Map/Unmap if needed?
        // Actually Map/Unmap implementation in Win32Compat.h uses cpuData and uploads on Unmap.
        // So we should initialize cpuData with pInitialData.
        memcpy((*ppBuffer)->cpuData.data(), pInitialData->pSysMem, pDesc->ByteWidth);
    } else {
        glBufferData(target, pDesc->ByteWidth, NULL, (pDesc->Usage == D3D11_USAGE_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }
    glBindBuffer(target, 0);

    return S_OK;
}

HRESULT CDirectX::Map(ID3D11Resource* pResource, UINT subResource, D3D11_MAP mapType, UINT mapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource) { 
    return _devCon->Map(pResource, subResource, mapType, mapFlags, pMappedResource);
}

void CDirectX::Unmap(ID3D11Resource* pResource, UINT subResource) {
    _devCon->Unmap(pResource, subResource);
}

ID3D11Device* CDirectX::GetDevice() { return _dev; }
ID3D11DeviceContext* CDirectX::GetDeviceContext() { return _devCon; }

HRESULT CDirectX::CreateTexture2D(D3D11_TEXTURE2D_DESC* pDesc, D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D, char* name) { 
    *ppTexture2D = new ID3D11Texture2D();
    (*ppTexture2D)->width = pDesc->Width;
    (*ppTexture2D)->height = pDesc->Height;
    (*ppTexture2D)->format = pDesc->Format;

    glGenTextures(1, &(*ppTexture2D)->glId);
    glBindTexture(GL_TEXTURE_2D, (*ppTexture2D)->glId);
    
    GLint internalFormat = GL_RGBA;
    GLenum format = GL_RGBA;
    if (pDesc->Format == DXGI_FORMAT_B8G8R8A8_UNORM) format = GL_BGRA;
    GLenum type = GL_UNSIGNED_BYTE;
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, pDesc->Width, pDesc->Height, 0, format, type, pInitialData ? pInitialData->pSysMem : NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return S_OK;
}

HRESULT CDirectX::CreateShaderResourceView(ID3D11Resource* pResource, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView, char* name) { 
    *ppSRView = new ID3D11ShaderResourceView();
    // In D3D11, SRV is a view of a resource. In GL, we just use the texture ID.
    // We can copy the GL ID from the resource.
    if (pResource) (*ppSRView)->glId = pResource->glId;
    return S_OK;
}

void CDirectX::SetVertexBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer** ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets) {
    if (NumBuffers > 0 && ppVertexBuffers[0]) {
        glBindBuffer(GL_ARRAY_BUFFER, ppVertexBuffers[0]->glId);
        
        UINT stride = pStrides[0];
        
        // Disable all arrays first to be safe (or at least the ones we might use)
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        // Also disable 2 and 3 just in case they were enabled
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);

        if (stride == 20) { // TEXTURED_VERTEX_ORTHO
             glEnableVertexAttribArray(0); // Position (XMFLOAT3)
             glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
             glEnableVertexAttribArray(1); // TexCoord (XMFLOAT2)
             glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)12);
        }
        else if (stride == 44) { // TEXTURED_VERTEX
             glEnableVertexAttribArray(0); // Position (XMFLOAT3)
             glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
             glEnableVertexAttribArray(1); // TexCoord (XMFLOAT2)
             glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)12);
             glEnableVertexAttribArray(2); // Object (XMFLOAT2)
             glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)20);
             glEnableVertexAttribArray(3); // ObjectParameters (XMFLOAT4)
             glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)28);
        }
        else if (stride == 32) { // COLOURED_VERTEX_ORTHO
             glEnableVertexAttribArray(0); // Position (XMFLOAT4)
             glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
             glEnableVertexAttribArray(1); // Color (XMFLOAT4)
             glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)16);
        }
        else if (stride == 48) { // COLOURED_VERTEX
             glEnableVertexAttribArray(0); // Position (XMFLOAT4)
             glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, (void*)0);
             glEnableVertexAttribArray(1); // Color (XMFLOAT4)
             glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)16);
        }
        else {
             // Default fallback (assume TEXTURED_VERTEX_ORTHO or similar)
             glEnableVertexAttribArray(0); 
             glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
             glEnableVertexAttribArray(1); 
             glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)12);
        }
    }
}

void CDirectX::SetIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset) {
    if (pIndexBuffer) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexBuffer->glId);
}

void CDirectX::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY Topology) {
    _devCon->IASetPrimitiveTopology(Topology);
}

void CDirectX::Draw(UINT VertexCount, UINT StartVertexLocation) {
    _devCon->Draw(VertexCount, StartVertexLocation);
}

void CDirectX::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) {
    _devCon->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

void CDirectX::SetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView** ppShaderResourceViews) {
    _devCon->PSSetShaderResources(StartSlot, NumViews, ppShaderResourceViews);
}

void CDirectX::EnableZBuffer() {
    glEnable(GL_DEPTH_TEST);
}

void CDirectX::DisableZBuffer() {
    glDisable(GL_DEPTH_TEST);
}

void CDirectX::Resize(int width, int height) {
    _width = width;
    _height = height;
    glViewport(0, 0, width, height);
}

CDXAdapter* CDirectX::GetAdapter() { return NULL; }

void CDirectX::SelectSampler(BOOL anisotropic) {}
void CDirectX::SetViewport(D3D11_VIEWPORT viewport) {
    glViewport((GLint)viewport.TopLeftX, (GLint)viewport.TopLeftY, (GLsizei)viewport.Width, (GLsizei)viewport.Height);
}
void CDirectX::SetScissorRect(D3D11_RECT rect) {}

HRESULT CDirectX::ConfigureBackBuffer() { return S_OK; }
HRESULT CDirectX::ReleaseBackBuffer() { return S_OK; }

#endif
