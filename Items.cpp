#include "Items.h"
#include "Globals.h"
#include "Utilities.h"
#include "GameController.h"

#define ITEMS_TEXTURE_PIXELS	128
#define ITEMS_TEXTURE_WIDTH		ITEMS_TEXTURE_PIXELS * 16
#define ITEMS_TEXTURE_HEIGHT	ITEMS_TEXTURE_PIXELS * 20

ID3D11Buffer* CItems::_vertexBuffer;

ID3D11Texture2D* CItems::_texture;
ID3D11ShaderResourceView* CItems::_textureRV;

std::unordered_map<int, CItems::Inventory*> CItems::_items;

int CItems::Colour1 = 0xff000000;
int CItems::Colour2 = -1;
int CItems::Colour3 = -1;
int CItems::Colour4 = 0xff000000;

CItems::CItems()
{
	_vertexBuffer = NULL;
	_texture = NULL;
	_textureRV = NULL;
}

CItems::~CItems()
{
	Dispose();
}

BOOL CItems::Init()
{
	BOOL ret = FALSE;

	// Create large texture, load all images from INV.AP (Pandora has ~300 items)
	int w = ITEMS_TEXTURE_WIDTH;
	int h = ITEMS_TEXTURE_HEIGHT;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = w;
	desc.Height = h;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	if (SUCCEEDED(dx.CreateTexture2D(&desc, NULL, &_texture)))
	{
		SetDebugName(_texture, "Inventory Texture");
		if (SUCCEEDED(dx.CreateShaderResourceView(_texture, NULL, &_textureRV)))
		{
			SetDebugName(_textureRV, "Inventory TextureRV");

			D3D11_MAPPED_SUBRESOURCE subRes;
			if (SUCCEEDED(dx.Map(_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subRes)))
			{
				PBYTE pScr = (PBYTE)subRes.pData;
				if (pScr != NULL)
				{
					// Clear texture
					ZeroMemory(pScr, h * subRes.RowPitch);

					CFile file;
					if (file.Open(L"INV.AP"))
					{
						DWORD length = file.Seek(0, CFile::SeekMethod::End);
						BYTE* pFileData = new BYTE[length];
						if (pFileData)
						{
							file.Seek(0, CFile::SeekMethod::Begin);
							file.Read(pFileData, length);

							int maxItems = GetInt(pFileData, 0, 2) - 3;

							PBYTE pPalette = (PBYTE)(pFileData + GetInt(pFileData, 2, 4));
							for (int i = 0; i < 256 * 3; i++)
							{
								pPalette[i] = (BYTE)((((int)pPalette[i]) & 0xff) * 4.04762);
							}

							for (int i = 0; i < 20; i++)
							{
								for (int j = 0; j < 16; j++)
								{
									int index = 2 + (i * 16 + j);
									if (index < maxItems)
									{
										int offset = GetInt(pFileData, 2 + index * 4, 4);
										int compressedSize = GetInt(pFileData, 6 + index * 4, 4) - offset;
										PBYTE pCompressed = pFileData + offset;

										BinaryData bd = CLZ::Decompress(pCompressed, compressedSize);
										if (bd.Data != NULL)
										{
											if (GetInt(bd.Data, 0, 2) == 0x100)
											{
												// Convert image
												int w = 0, h = 0, offset = 0;
												w = GetInt(bd.Data, 2, 2);
												h = GetInt(bd.Data, 4, 2);
												offset = 16;

												for (int y = 0; y < h; y++)
												{
													int c1 = GetInt(bd.Data, offset, 2);
													int c2 = GetInt(bd.Data, offset + 2, 2);
													offset += 4;

													for (int x = 0; x < w && x < c2; x++)
													{
														int colIx = bd.Data[offset + x];
														if (colIx != 0)
														{
															int dst = (i * ITEMS_TEXTURE_PIXELS + y) * subRes.RowPitch + (j * ITEMS_TEXTURE_PIXELS + x + c1) * 4;
															pScr[dst + 0] = pPalette[colIx * 3 + 2];
															pScr[dst + 1] = pPalette[colIx * 3 + 1];
															pScr[dst + 2] = pPalette[colIx * 3 + 0];
															pScr[dst + 3] = 255;
														}
													}

													offset += c2;
												}
											}

											delete[] bd.Data;

											// Create inventory item
											int id = i * 16 + j;
											_items[id] = new Inventory(id, w, h);
										}
									}
								}
							}

							delete[] pFileData;

							TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[6];
							if (pVB != NULL)
							{
								float x1 = 0.0f;
								float x2 = dx.GetWidth() * 1.0f;
								float y1 = 0.0f;
								float y2 = -dx.GetHeight() * 1.6f;

								pVB[0].position = XMFLOAT3(x1, y1, -0.25f);
								pVB[0].texture = XMFLOAT2(0.0f, 0.0f);
								pVB[1].position = XMFLOAT3(x2, y1, -0.25f);
								pVB[1].texture = XMFLOAT2(1.0f, 0.0f);
								pVB[2].position = XMFLOAT3(x2, y2, -0.25f);
								pVB[2].texture = XMFLOAT2(1.0f, 1.0f);

								pVB[3].position = XMFLOAT3(x1, y1, -0.25f);
								pVB[3].texture = XMFLOAT2(0.0f, 0.0f);
								pVB[4].position = XMFLOAT3(x2, y2, -0.25f);
								pVB[4].texture = XMFLOAT2(1.0f, 1.0f);
								pVB[5].position = XMFLOAT3(x1, y2, -0.25f);
								pVB[5].texture = XMFLOAT2(0.0f, 1.0f);

								D3D11_BUFFER_DESC vbDesc;
								vbDesc.Usage = D3D11_USAGE_DYNAMIC;
								vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX_ORTHO) * 6;
								vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
								vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
								vbDesc.MiscFlags = 0;
								vbDesc.StructureByteStride = 0;

								D3D11_SUBRESOURCE_DATA vData;
								vData.pSysMem = pVB;
								vData.SysMemPitch = 0;
								vData.SysMemSlicePitch = 0;

								dx.CreateBuffer(&vbDesc, &vData, &_vertexBuffer, "Items");

								delete[] pVB;
								ret = TRUE;
							}
						}
					}
					else
					{
						MessageBox(NULL, L"Could not open file INV.AP", L"Init game", MB_OK);
					}
				}

				dx.Unmap(_texture, 0);
			}
		}
	}

	return ret;
}

void CItems::Dispose()
{
	if (_textureRV != NULL)
	{
		_textureRV->Release();
		_textureRV = NULL;
	}

	if (_texture != NULL)
	{
		_texture->Release();
		_texture = NULL;
	}

	if (_vertexBuffer != NULL)
	{
		_vertexBuffer->Release();
		_vertexBuffer = NULL;
	}

	for (auto it : _items)
	{
		delete it.second;
	}
}

void CItems::Render()
{
	if (_vertexBuffer == NULL)
	{
		return;
	}

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX wm = XMMatrixTranslation(0.0f, 0.0f, -1.0f);

	CConstantBuffers::SetWorld(dx, &wm);
	dx.SetShaderResources(0, 1, &_textureRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, 0);
}

CItems::Inventory::Inventory(int id, int w, int h)
{
	Id = id;

	ImageBuffer = NULL;

	// Create image buffer
	TEXTURED_VERTEX_ORTHO* pVB = new TEXTURED_VERTEX_ORTHO[6];
	if (pVB != NULL)
	{
		int x = id % 16;
		int y = id / 16;
		float tx1 = (x * ITEMS_TEXTURE_PIXELS) / (float)(ITEMS_TEXTURE_WIDTH);
		float tx2 = ((x + 1) * ITEMS_TEXTURE_PIXELS) / (float)(ITEMS_TEXTURE_WIDTH);
		float ty1 = (y * ITEMS_TEXTURE_PIXELS) / (float)(ITEMS_TEXTURE_HEIGHT);
		float ty2 = ((y + 1) * ITEMS_TEXTURE_PIXELS) / (float)(ITEMS_TEXTURE_HEIGHT);

		pVB[0].position = XMFLOAT3(0.0f, 0.0f, -0.5f);
		pVB[0].texture = XMFLOAT2(tx1, ty1);
		pVB[1].position = XMFLOAT3(ITEMS_TEXTURE_PIXELS, 0.0f, -0.5f);
		pVB[1].texture = XMFLOAT2(tx2, ty1);
		pVB[2].position = XMFLOAT3(ITEMS_TEXTURE_PIXELS, -ITEMS_TEXTURE_PIXELS, -0.5f);
		pVB[2].texture = XMFLOAT2(tx2, ty2);

		pVB[3].position = XMFLOAT3(0.0f, 0.0f, -0.5f);
		pVB[3].texture = XMFLOAT2(tx1, ty1);
		pVB[4].position = XMFLOAT3(ITEMS_TEXTURE_PIXELS, -ITEMS_TEXTURE_PIXELS, -0.5f);
		pVB[4].texture = XMFLOAT2(tx2, ty2);
		pVB[5].position = XMFLOAT3(0.0f, -ITEMS_TEXTURE_PIXELS, -0.5f);
		pVB[5].texture = XMFLOAT2(tx1, ty2);

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.Usage = D3D11_USAGE_DYNAMIC;
		vbDesc.ByteWidth = sizeof(TEXTURED_VERTEX_ORTHO) * 6;
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vbDesc.MiscFlags = 0;
		vbDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vData;
		vData.pSysMem = pVB;
		vData.SysMemPitch = 0;
		vData.SysMemSlicePitch = 0;

		dx.CreateBuffer(&vbDesc, &vData, &ImageBuffer, "Inventory");

		delete[] pVB;
	}

	std::wstring text = CGameController::GetItemName(id);
	if (text.size() > 0)
	{
		Text.SetText(text.c_str());
		Text.SetColours(Colour1, Colour2, Colour3, Colour4);
	}
}

CItems::Inventory::~Inventory()
{
	if (ImageBuffer != NULL)
	{
		ImageBuffer->Release();
		ImageBuffer = NULL;
	}
}

void CItems::Inventory::Render(float x, float y)
{
	RenderImage(x, y);
}

void CItems::Inventory::RenderName(float x, float y, int colour1, int colour2, int colour3, int colour4, BOOL highlight)
{
	Text.SetColours(colour1, colour2, colour3, colour4);
	Text.Render(x, y);
}

void CItems::Inventory::RenderImage(float x, float y, BOOL highlight)
{
	if (ImageBuffer == NULL)
	{
		return;
	}

	UINT stride = sizeof(TEXTURED_VERTEX_ORTHO);
	UINT offset = 0;
	dx.SetVertexBuffers(0, 1, &ImageBuffer, &stride, &offset);

	dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX wm = XMMatrixTranslation(x, -y, -1.0f);
	if (highlight)
	{
		// TODO: Draw selection rectangle
	}

	CConstantBuffers::SetWorld(dx, &wm);
	dx.SetShaderResources(0, 1, &_textureRV);
	CShaders::SelectOrthoShader();
	dx.Draw(6, 0);
}

void CItems::RenderItem(int id, float x, float y)
{
	_items[id]->Render(x, y);
}

void CItems::RenderItemName(int id, float x, float y, BOOL highlight)
{
	_items[id]->RenderName(x, y, Colour1, Colour2, Colour3, Colour4, highlight);
}

void CItems::RenderItemImage(int id, float x, float y, BOOL highlight)
{
	_items[id]->RenderImage(x, y, highlight);
}

int CItems::GetWidestName()
{
	float widest = 0.0f;
	for (auto it : _items)
	{
		float width = it.second->NameWidth();
		if (width > widest)
		{
			widest = width;
		}
	}

	return static_cast<int>(ceil(widest));
}

int CItems::GetItemNameWidth(int id)
{
	return static_cast<int>(_items[id]->NameWidth());
}

void CItems::SetItemName(int id, std::wstring text)
{
	_items[id]->SetItemName(text);
}
