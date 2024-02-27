#include "TravelImage.h"
#include "ShaderStructs.h"
#include "Globals.h"

void CTravelImage::Render()
{
	if (Buffer != NULL)
	{
		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &Buffer, &stride, &offset);
		XMMATRIX wm = XMMatrixIdentity();
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = Texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);
	}
}

void CTravelImage::Render(float x, float y)
{
	if (Buffer != NULL)
	{
		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &Buffer, &stride, &offset);
		XMMATRIX wm = XMMatrixTranslation(floor(x) + 0.5f, floor(y), 0.0f);
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = Texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);
	}
}
