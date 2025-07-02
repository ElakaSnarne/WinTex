#include "PuzzlePiece.h"
#include "GameController.h"
#include "Utilities.h"

std::unordered_map<int, CPuzzlePiece*> CPuzzlePiece::Images;
std::vector<CPuzzlePiece*> CPuzzlePiece::ZImages;

float CPuzzlePiece::rotationAngles[] = { 0.0f, -XM_PI / 2.0f, -XM_PI, XM_PI / 2.0f };

ID3D11Buffer* CPuzzlePiece::vertexBuffer = NULL;

void CPuzzlePiece::Reset()
{
	Dispose();

	if (vertexBuffer != NULL)
	{
		vertexBuffer->Release();
		vertexBuffer = NULL;
	}
}

void CPuzzlePiece::Add(int index, LPBYTE pImage, float scale, int palette[], int numberOfImages, float left, float top, int positionOffset)
{
	int width = GetInt(pImage, 0, 2);
	int height = GetInt(pImage, 2, 2);
	CPuzzlePiece* pScrap = new CPuzzlePiece();
	pScrap->RawImage = pImage;
	pScrap->OriginalWidth = width;
	pScrap->OriginalHeight = height;
	pScrap->Width = width * scale;
	pScrap->Height = height * scale;
	pScrap->Texture.Init(width, height);

	D3D11_MAPPED_SUBRESOURCE subRes;
	ID3D11Texture2D* pTexture = pScrap->Texture.GetTexture();
	if (SUCCEEDED(dx.Map(pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
	{
		int inPtr = 4;
		int* pTex = (int*)subRes.pData;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int pix = pImage[inPtr++];
				pTex[y * subRes.RowPitch / 4 + x] = (pix != 0) ? palette[pix] : 0;
			}
		}

		dx.Unmap(pTexture, 0);
	}

	if (index < numberOfImages)
	{
		// Get position, orientation and z-order from save data
		pScrap->Offset = positionOffset + index * 6;
		//;
		pScrap->X = left + CGameController::GetWord(pScrap->Offset) * scale;
		pScrap->Y = top + CGameController::GetWord(pScrap->Offset + 2) * scale;
		pScrap->Z = CGameController::GetData(pScrap->Offset + 4);
		pScrap->Orientation = CGameController::GetData(pScrap->Offset + 5);
	}

	Images[index] = pScrap;
}

void CPuzzlePiece::Sort()
{
	int count = Images.size();
	for (int z = 0; z < count; z++)
	{
		for (int i = 0; i < count; i++)
		{
			if (Images[i]->Z == z)
			{
				ZImages.push_back(Images[i]);
			}
		}
	}
}

CPuzzlePiece* CPuzzlePiece::Get(int index)
{
	return Images[index];
}

void CPuzzlePiece::Dispose()
{
	// Clear unordered maps
	for (auto it : Images)
	{
		delete it.second;
	}

	Images.clear();
	ZImages.clear();
}

void CPuzzlePiece::Render()
{
	if (vertexBuffer == NULL)
	{
		TEXTURED_VERTEX* vertices = new TEXTURED_VERTEX[4];
		if (vertices != NULL)
		{
			vertices[0].position = XMFLOAT3(0.5f, 0.5f, 0.0f);
			vertices[0].texture = XMFLOAT2(1.0f, 0.0f);

			vertices[1].position = XMFLOAT3(0.5f, -0.5f, 0.0f);
			vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

			vertices[2].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
			vertices[2].texture = XMFLOAT2(0.0f, 0.0f);

			vertices[3].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
			vertices[3].texture = XMFLOAT2(0.0f, 1.0f);

			D3D11_BUFFER_DESC vertexBufferDesc;
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(TEXTURED_VERTEX) * 4;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vertexData;
			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer, "DragDropPuzzlePieceVertexBuffer");

			delete[] vertices;
		}
	}

	if (vertexBuffer != NULL)
	{
		dx.DisableZBuffer();

		CConstantBuffers::Setup2D(dx);

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();

		for (auto pPiece : ZImages)
		{
			float w = pPiece->Width;
			float h = pPiece->Height;
			// Scale, rotate, translate by correct dimension depending on rotation, translate x & y
			float itx = (pPiece->Orientation == 0 || pPiece->Orientation == 2) ? w / 2.0f : h / 2.0f;
			float ity = (pPiece->Orientation == 0 || pPiece->Orientation == 2) ? h / 2.0f : w / 2.0f;
			float x = itx + pPiece->X;
			float y = ity + pPiece->Y;

			XMMATRIX wm = XMMatrixScaling(w, h, 1.0f) * XMMatrixRotationZ(rotationAngles[pPiece->Orientation]) * XMMatrixTranslation(x, -y, 0.0f);

			CConstantBuffers::SetWorld(dx, &wm);
			ID3D11ShaderResourceView* pRV = pPiece->Texture.GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			dx.Draw(4, 0);
		}

		dx.EnableZBuffer();
	}
}

CPuzzlePiece* CPuzzlePiece::HitTest(float mx, float my, int offset, float scale)
{
	int numberOfImages = Images.size();
	if (numberOfImages == 0)
	{
		return NULL;
	}

	std::vector<CPuzzlePiece*>::iterator it = ZImages.end();
	std::vector<CPuzzlePiece*>::iterator start = ZImages.begin() + offset;

	do
	{
		it--;
		CPuzzlePiece* pN = *it;

		float w = pN->Width;
		float h = pN->Height;
		// Scale, rotate, translate by correct dimension depending on rotation, translate x & y
		float rw = (pN->Orientation == 0 || pN->Orientation == 2) ? w : h;
		float rh = (pN->Orientation == 0 || pN->Orientation == 2) ? h : w;
		float x = pN->X;
		float y = pN->Y;

		// Check if mouse is roughly inside rectangle
		if (mx >= x && mx < (x + rw) && my >= y && my < (y + rh))
		{
			// Translate mouse coordinates, scale and orientation relative to object

			int cx = static_cast<int>((mx - x) / scale);
			int cy = static_cast<int>((my - y) / scale);
			if (pN->Orientation == 2)
			{
				// Upside down
				cx = pN->OriginalWidth - cx - 1;
				cy = pN->OriginalHeight - cy - 1;
			}
			else if (pN->Orientation == 1)
			{
				// Rotated right
				int tmp = cx;
				cx = cy;
				cy = pN->OriginalHeight - tmp - 1;
			}
			else if (pN->Orientation == 3)
			{
				// Rotated left
				int tmp = cy;
				cy = cx;
				cx = pN->OriginalWidth - tmp - 1;
			}

			// Now check pixel
			LPBYTE pPixels = pN->RawImage + 4;
			if (pPixels[cy * pN->OriginalWidth + cx] != 0)
			{
				// All items from this point should be pushed down in z-order
				std::vector<CPuzzlePiece*>::iterator fix = it + 1;
				while (fix != ZImages.end())
				{
					CPuzzlePiece* pS = *fix++;
					pS->Z--;
					CGameController::SetData(pS->Offset + 4, pS->Z);
				}

				// Remove from queue and place at end
				ZImages.erase(it);
				ZImages.push_back(pN);

				pN->Z = numberOfImages - 1;
				CGameController::SetData(pN->Offset + 4, pN->Z);

				return pN;
			}
		}

	} while (it != start);

	return NULL;
}
