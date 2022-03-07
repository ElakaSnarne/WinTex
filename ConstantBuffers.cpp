#include "ConstantBuffers.h"
#include <DirectXMath.h>

ID3D11Buffer* CConstantBuffers::_vop = NULL;
ID3D11Buffer* CConstantBuffers::_world = NULL;
ID3D11Buffer* CConstantBuffers::_texFont = NULL;
ID3D11Buffer* CConstantBuffers::_multiColouredFont = NULL;
ID3D11Buffer* CConstantBuffers::_visibility = NULL;
ID3D11Buffer* CConstantBuffers::_translation = NULL;

void CConstantBuffers::SetVOP(CDirectX& dx, XMMATRIX* view, XMMATRIX* ortho, XMMATRIX* projection)
{
	if (_vop == NULL)
	{
		// Create constant buffer
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(VOPBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		dx.CreateBuffer(&matrixBufferDesc, NULL, &_vop, "ViewOrthoProjection");
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx.GetDeviceContext()->Map(_vop, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VOPBufferType* dataPtr = (VOPBufferType*)mappedResource.pData;
	dataPtr->view = XMMatrixTranspose(*view);
	dataPtr->ortho = XMMatrixTranspose(*ortho);
	dataPtr->projection = XMMatrixTranspose(*projection);
	dx.GetDeviceContext()->Unmap(_vop, 0);
	dx.GetDeviceContext()->VSSetConstantBuffers(0, 1, &_vop);
	dx.GetDeviceContext()->PSSetConstantBuffers(0, 1, &_vop);
}

void CConstantBuffers::SetWorld(CDirectX& dx, XMMATRIX* world)
{
	if (_world == NULL)
	{
		// Create constant buffer
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(WorldBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		dx.CreateBuffer(&matrixBufferDesc, NULL, &_world, "World");
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (dx.GetDeviceContext()->Map(_world, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) == S_OK)
	{
		WorldBufferType* dataPtr = (WorldBufferType*)mappedResource.pData;
		dataPtr->world = XMMatrixTranspose(*world);
		dx.GetDeviceContext()->Unmap(_world, 0);
		dx.GetDeviceContext()->VSSetConstantBuffers(1, 1, &_world);
		dx.GetDeviceContext()->PSSetConstantBuffers(1, 1, &_world);
	}
	else
	{
		MessageBox(NULL, L"Map failed", NULL, 0);
	}
}

void CConstantBuffers::SetMultiColouredFont(CDirectX& dx, XMVECTOR* colour1, XMVECTOR* colour2, XMVECTOR* colour3, XMVECTOR* colour4, XMVECTOR* colour5, XMVECTOR* colour6)
{
	if (_multiColouredFont == NULL)
	{
		// Create constant buffer
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MultiColouredFontBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		dx.CreateBuffer(&matrixBufferDesc, NULL, &_multiColouredFont, "Font");
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx.GetDeviceContext()->Map(_multiColouredFont, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MultiColouredFontBufferType* dataPtr = (MultiColouredFontBufferType*)mappedResource.pData;
	dataPtr->colour1 = *colour1;
	dataPtr->colour2 = *colour2;
	dataPtr->colour3 = *colour3;
	dataPtr->colour4 = *colour4;
	dataPtr->colour5 = *colour5;
	dataPtr->colour6 = *colour6;
	dx.GetDeviceContext()->Unmap(_multiColouredFont, 0);
	dx.GetDeviceContext()->VSSetConstantBuffers(2, 1, &_multiColouredFont);
	dx.GetDeviceContext()->PSSetConstantBuffers(2, 1, &_multiColouredFont);
}

void CConstantBuffers::SetTexFont(CDirectX& dx, XMVECTOR* colour1, XMVECTOR* colour2, XMVECTOR* colour3, XMVECTOR* colour4)
{
	if (_texFont == NULL)
	{
		// Create constant buffer
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(TexFontBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		dx.CreateBuffer(&matrixBufferDesc, NULL, &_texFont, "TexFont");
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx.GetDeviceContext()->Map(_texFont, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TexFontBufferType* dataPtr = (TexFontBufferType*)mappedResource.pData;
	dataPtr->colour1 = *colour1;
	dataPtr->colour2 = *colour2;
	dataPtr->colour3 = *colour3;
	dataPtr->colour4 = *colour4;
	dx.GetDeviceContext()->Unmap(_texFont, 0);
	dx.GetDeviceContext()->VSSetConstantBuffers(4, 1, &_texFont);
	dx.GetDeviceContext()->PSSetConstantBuffers(4, 1, &_texFont);
}

void CConstantBuffers::SetVisibility(CDirectX& dx, VisibilityBufferType visibility)
{
	if (_visibility == NULL)
	{
		// Create constant buffer
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(VisibilityBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		dx.CreateBuffer(&matrixBufferDesc, NULL, &_visibility, "Visibility");
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx.GetDeviceContext()->Map(_visibility, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	VisibilityBufferType* dataPtr = (VisibilityBufferType*)mappedResource.pData;
	CopyMemory(&dataPtr->visibility[0], &visibility.visibility[0], sizeof(XMFLOAT4) * 3000);
	dx.GetDeviceContext()->Unmap(_visibility, 0);
	dx.GetDeviceContext()->VSSetConstantBuffers(3, 1, &_visibility);
	dx.GetDeviceContext()->PSSetConstantBuffers(3, 1, &_visibility);
}

void CConstantBuffers::SetTranslation(CDirectX& dx, TranslationBufferType translation)
{
	if (_translation == NULL)
	{
		// Create constant buffer
		D3D11_BUFFER_DESC matrixBufferDesc;
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(TranslationBufferType);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		dx.CreateBuffer(&matrixBufferDesc, NULL, &_translation, "Translation");
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dx.GetDeviceContext()->Map(_translation, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TranslationBufferType* dataPtr = (TranslationBufferType*)mappedResource.pData;
	CopyMemory(&dataPtr->translation[0], &translation.translation[0], sizeof(XMFLOAT4) * 256);
	dx.GetDeviceContext()->Unmap(_translation, 0);
	dx.GetDeviceContext()->VSSetConstantBuffers(5, 1, &_translation);
	dx.GetDeviceContext()->PSSetConstantBuffers(5, 1, &_translation);
}

void CConstantBuffers::Dispose()
{
	if (_vop != NULL)
	{
		_vop->Release();
		_vop = NULL;
	}

	if (_world != NULL)
	{
		_world->Release();
		_world = NULL;
	}

	if (_multiColouredFont != NULL)
	{
		_multiColouredFont->Release();
		_multiColouredFont = NULL;
	}

	if (_visibility != NULL)
	{
		_visibility->Release();
		_visibility = NULL;
	}
}

void CConstantBuffers::Setup2D(CDirectX& dx)
{
	float w = (float)dx.GetWidth();
	float h = (float)dx.GetHeight();

	SetupVOP(dx, w, h, w / 2, -h / 2, -10.0f);
}

void CConstantBuffers::Setup3D(CDirectX& dx)
{
	float w = (float)dx.GetWidth();
	float h = (float)dx.GetHeight();

	SetupVOP(dx, w, h, 0.0f, 0.0f, 0.0f);
}

void CConstantBuffers::SetupVOP(CDirectX& dx, float w, float h, float camera_x, float camera_y, float camera_z)
{
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR position = XMVectorSet(camera_x, camera_y, camera_z, 0.0f);
	XMVECTOR lookAt = XMVectorSet(camera_x, camera_y, 1.0f, 0.0f);

	XMMATRIX vm = XMMatrixLookAtLH(position, lookAt, up);
	XMMATRIX om = XMMatrixOrthographicLH(w, h, 0.1f, 1000.0f);

	float fieldOfView = (float)XM_PI / 4.0f;
	float screenAspect = w / h;
	XMMATRIX pm = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, 0.1f, 1000.0f);

	SetVOP(dx, &vm, &om, &pm);
}
