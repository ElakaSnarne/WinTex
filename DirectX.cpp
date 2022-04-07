#include "DirectX.h"
#include "resource.h"
#include "Globals.h"
#include <sstream>
#include <string>

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

	HRESULT hr;
	//short buffer[128];
	D3D_FEATURE_LEVEL features = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	while (scd.SampleDesc.Count > 0)
	{
		hr = D3D11CreateDeviceAndSwapChain(NULL,										// Adaptor
			D3D_DRIVER_TYPE_HARDWARE,													// Driver type
			NULL,																		// Software
#ifdef _DEBUG
			D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,				// Flags, debug mode
#else
			D3D11_CREATE_DEVICE_BGRA_SUPPORT,											// Flags, release mode
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

