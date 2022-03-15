#include "UAKMTravelModule.h"
#include "Utilities.h"
#include "File.h"
#include "GameController.h"
#include "UAKMGame.h"
#include "LocationModule.h"
#include "VideoModule.h"
#include "AmbientAudio.h"

#define TRAVEL_MAP_FILE	0
//Chandler Ave
//Countess' Mansion
//Colonel's Office
//Police Station
//Tex' Office
//Countess' Mansion
//Alhambra Theater
//Knickerbocker (Eddie Ching's)
//Golden Gate Hotel
//Colonel's Office
//Melahn Tode's Apartment
//Roadside Motel
//GRS
//Bastion of Sanctity
//Broken Skull -Off Planet-
//Coit Tower

short coordinates[] = { 8, 6, 245, 71, 26, 272, 168, 188, 349, 178, 335, 58, 26, 272, 171, 260, 276, 139, 349, 75, 168, 188, 18, 144, 130, 37, 307, 231, 36, 229, 19, 27, 266, 45 };
short hotspots[] = { 0, 0, 340, 74, 148, 275, 171, 191, 352, 181, 338, 61, 148, 275, 292, 263, 376, 142, 352, 78, 171, 191, 21, 147, 231, 40, 334, 234, 39, 232, 22, 30, 340, 48 };
signed char resultTable[] = { -1, -1, 2, -1, -1, 12, -1, 31, -1, 14, 1, -1, 15, -1, -1, 13, -1, 43, -1, 5, 21, -1, -1, 15, -1, 16, 26, -1, 33, -1, -1, 18, -1, 9, 2, -1, -1, 2, -1, 4, -1, 3, 14, -1, 5, -1, 8, -1, 9, -1, -1, 43, 20, -1, 16, -1, 19, -1, -1, 5, 22, -1, 23, -1, 25, -1, 24, -1, 26, -1, 27, -1, 30, -1, 28, -1, 29, -1 };

CUAKMTravelModule::CUAKMTravelModule() : CModuleBase(ModuleType::Travel)
{
	_scale = 0.0f;
	_offsetX = 0.0f;
	_offsetY = 0.0f;

	_selectedLocation = -1;
	_selectedSubLocation = -1;
	_selectedSubLocationEntry = 0;

	_subLocations.push_back(new CSubLocation(1, "STREET or NEWSSTAND"));
	_subLocations.push_back(new CSubLocation(1, "BREW & STEW"));
	_subLocations.push_back(new CSubLocation(1, "SLICE O'HEAVEN PIZZA"));
	_subLocations.push_back(new CSubLocation(1, "ROOK'S PAWNSHOP"));
	_subLocations.push_back(new CSubLocation(1, "SNO WHITE WAREHOUSE"));
	_subLocations.push_back(new CSubLocation(1, "ALLEY (behind PAWNSHOP)"));
	_subLocations.push_back(new CSubLocation(1, "RUSTY'S FUN HOUSE"));
	_subLocations.push_back(new CSubLocation(1, "ELECTRONICS SHOP"));
	_subLocations.push_back(new CSubLocation(8, "LIBRARY"));
	_subLocations.push_back(new CSubLocation(8, "HALLWAY"));
	_subLocations.push_back(new CSubLocation(8, "STUDY"));
	_subLocations.push_back(new CSubLocation(8, "SECRET ROOM"));
	_subLocations.push_back(new CSubLocation(9, "HOTEL LOBBY"));
	_subLocations.push_back(new CSubLocation(9, "SUITE ENTRYWAY"));
	_subLocations.push_back(new CSubLocation(9, "SUITE BEDROOM"));
	_subLocations.push_back(new CSubLocation(9, "SUITE PIANO ROOM"));
	_subLocations.push_back(new CSubLocation(9, "SUITE HOT TUB"));
	_subLocations.push_back(new CSubLocation(13, "HALLWAY"));
	_subLocations.push_back(new CSubLocation(13, "R & D OFFICE"));
	_subLocations.push_back(new CSubLocation(13, "SUPERVISOR'S OFFICE"));
	_subLocations.push_back(new CSubLocation(13, "CONFERENCE ROOM"));
	_subLocations.push_back(new CSubLocation(13, "MARCUS TUCKER'S OFFICE"));

	_selectionIndicator = NULL;
}

CUAKMTravelModule::~CUAKMTravelModule()
{
	Dispose();
}

void CUAKMTravelModule::Initialize()
{
	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	// Load palette(s)
	BinaryData bdPal = LoadEntry(L"GRAPHICS.AP", 28);
	if (bdPal.Data != NULL)
	{
		for (int c = 0; c < 256; c++)
		{
			double r = bdPal.Data[c * 3 + 2];
			double g = bdPal.Data[c * 3 + 1];
			double b = bdPal.Data[c * 3 + 0];
			int ri = (byte)((r * 255.0) / 63.0);
			int gi = (byte)((g * 255.0) / 63.0);
			int bi = (byte)((b * 255.0) / 63.0);
			int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
			_palette[c] = col;
		}

		delete[] bdPal.Data;
	}

	CFile file;
	if (file.Open(L"TRAVEL.AP"))
	{
		DWORD length = file.Seek(0, CFile::SeekMethod::End);
		LPBYTE data = new BYTE[length];
		if (data != NULL)
		{
			file.Seek(0);
			file.Read(data, length);
			file.Close();
			int count = GetInt(data, 0, 2);

			int activePalette[256];
			CopyMemory(activePalette, _palette, 256 * sizeof(int));

			// Calculate scaling factor
			// Need enough room for the location image and 8 sublocation texts/images
			float requiredWidth = 425 + 182;	// Map width+location image width
			float requiredHeight = 330 + 4 * 14;// Assuming 14 pixels height for sub-locations
			float w = (float)dx.GetWidth();
			float h = (float)dx.GetHeight();

			float sx = w / requiredWidth;
			float sy = h / requiredHeight;
			_scale = min(sx, sy);

			for (int i = 0; i < (count - 1); i++)
			{
				// Locate and decompress each entry (each entry is another AP)
				int offset = GetInt(data, 2 + i * 4, 4);
				int nextOffset = GetInt(data, 6 + i * 4, 4);
				int len = nextOffset - offset;
				BinaryData bd = CLZ::Decompress(data, offset, len);
				if (bd.Data != NULL)
				{
					// This is an AP, extract images and palettes
					int subCount = GetInt(bd.Data, 0, 2);
					for (int j = 0; j < (subCount - 1); j++)
					{
						// Find palette in AP, patch current palette
						LPBYTE pPal = NULL;
						int first = 0, last = 256;
						if (i == 0)
						{
							// Palette is first file
							pPal = bd.Data + GetInt(bd.Data, 2, 4);
							last = 96;
						}
						else
						{
							// Palette is last file
							pPal = bd.Data + GetInt(bd.Data, 2 + (subCount - 2) * 4, 4);
							first = 96;
						}

						if (pPal != NULL)
						{
							for (int c = first; c < last; c++)
							{
								int r = pPal[c * 3 + 0];
								int g = pPal[c * 3 + 1];
								int b = pPal[c * 3 + 2];
								int col = 0xff000000 | b | (g << 8) | (r << 16);
								activePalette[c] = col;
							}
						}

						int subOffset = GetInt(bd.Data, 2 + j * 4, 4);
						int nextSubOffset = GetInt(bd.Data, 6 + j * 4, 4);
						int subLen = nextSubOffset - subOffset;

						if (GetInt(bd.Data, subOffset, 2) == 0x100)
						{
							// Image
							CTravelImage* ti = new	CTravelImage();

							ti->Texture.Init(bd.Data, bd.Length, subOffset, activePalette, 0, "Travel Texture");

							float width = ti->Texture.Width() * _scale, height = -ti->Texture.Height() * _scale;
							if (i == 0 && j == 1)
							{
								// Main map image
								ti->Left = _offsetX = (w - requiredWidth * _scale) / 2;
								ti->Right = ti->Left + width;
								ti->Top = _offsetY = -(h - requiredHeight * _scale) / 2;
								ti->Bottom = ti->Top + height;
							}
							else if (j == 0 && i < 17)
							{
								// Location name
								ti->Left = _offsetX + (coordinates[i * 2] - coordinates[0]) * _scale;
								ti->Right = ti->Left + width;
								ti->Top = _offsetY - (coordinates[i * 2 + 1] - coordinates[1]) * _scale;
								ti->Bottom = ti->Top + height;
							}
							else if (i == 0)
							{
								// Buttons
								ti->Left = 0.0f;
								ti->Right = width;
								ti->Top = 0.0f;
								ti->Bottom = height;

								if (j == 2)
								{
									ti->Left = _offsetX + 9 * _scale;
									ti->Right = ti->Left + width;
									ti->Top = _images[1]->Bottom - height;
									ti->Bottom = _images[1]->Bottom;
								}
								else if (j == 4)
								{
									ti->Left = _images[1]->Right - width - 10 * _scale;
									ti->Right = ti->Left + width;
									ti->Top = _images[1]->Bottom - height;
									ti->Bottom = _images[1]->Bottom;
								}
							}
							else
							{
								// Location images
								ti->Left = _offsetX + 425 * _scale;
								ti->Right = ti->Left + width;
								ti->Top = _offsetY;
								ti->Bottom = ti->Top + height;
							}

							CreateTexturedRectangle(ti->Top, ti->Left, ti->Bottom, ti->Right, &ti->Buffer, "Travel Buffer");

							_images[i * 100 + j] = ti;
						}
					}

					delete[] bd.Data;
				}
			}

			delete[] data;
		}
	}

	if (_selectionIndicator == NULL)
	{
		COLOURED_VERTEX_ORTHO* pVB = new COLOURED_VERTEX_ORTHO[5];
		if (pVB != NULL)
		{
			float x1 = 0.0f;
			float x2 = 5.0f;
			float y1 = 0.0f;
			float y2 = -5.0f;

			pVB[0].position = XMFLOAT4(x2, y1, 0.0f, 0.0f);
			pVB[0].colour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			pVB[1].position = XMFLOAT4(x2, y2, 0.0f, 0.0f);
			pVB[1].colour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			pVB[2].position = XMFLOAT4(x1, y1, 0.0f, 0.0f);
			pVB[2].colour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			pVB[3].position = XMFLOAT4(x1, y2, 0.0f, 0.0f);
			pVB[3].colour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
			pVB[4].position = XMFLOAT4(x2, y2, 0.0f, 0.0f);
			pVB[4].colour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);

			D3D11_BUFFER_DESC vbDesc;
			vbDesc.Usage = D3D11_USAGE_DYNAMIC;
			vbDesc.ByteWidth = sizeof(COLOURED_VERTEX_ORTHO) * 5;
			vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbDesc.MiscFlags = 0;
			vbDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA vData;
			vData.pSysMem = pVB;
			vData.SysMemPitch = 0;
			vData.SysMemSlicePitch = 0;

			dx.CreateBuffer(&vbDesc, &vData, &_selectionIndicator, "TravelIndicator");

			delete[] pVB;
		}
	}
}

void CUAKMTravelModule::Dispose()
{
	for (auto it : _images)
	{
		delete it.second;
	}

	_images.clear();

	if (_selectionIndicator != NULL)
	{
		_selectionIndicator->Release();
		_selectionIndicator = NULL;
	}
}

void CUAKMTravelModule::Render()
{
	CTravelImage* ti = _images[1];
	if (ti->Buffer != NULL)
	{
		dx.Clear(0.0f, 0.0f, 0.0f);

		dx.DisableZBuffer();

		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();

		ti->Render();

		for (int i = 1; i <= 16; i++)
		{
			if (CGameController::GetData(UAKM_SAVE_TRAVEL + i) != 0)
			{
				dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				CShaders::SelectOrthoShader();
				CTravelImage* name = _images[100 * i];
				name->Render();

				if (_selectedLocation == i)
				{
					CTravelImage* loc = _images[100 * i + 1];
					loc->Render();

					// TODO: Render selection indicator
					BOOL renderSelection = (((GetTickCount64() / 500) & 1) == 0);
					if (renderSelection)
					{
						CShaders::SelectColourShader();
						UINT stride = sizeof(COLOURED_VERTEX_ORTHO);
						UINT offset = 0;
						XMMATRIX wm = XMMatrixScaling(_scale, _scale, 1.0f) * XMMatrixTranslation(ti->Left + (hotspots[i * 2 + 0] - 8) * _scale, ti->Top - (hotspots[i * 2 + 1] - 6) * _scale, -2.0f);
						CConstantBuffers::SetWorld(dx, &wm);
						dx.SetVertexBuffers(0, 1, &_selectionIndicator, &stride, &offset);
						dx.Draw(4, 0);

						CShaders::SelectOrthoShader();
					}

					int subix = 17;
					float x = ti->Left;
					float y = ti->Bottom;
					CTravelImage* aa = _images[6];
					int subCount = 0;
					for (auto it : _subLocations)
					{
						if (it->ParentLocation == i && CGameController::GetData(UAKM_SAVE_TRAVEL + subix) != 0)
						{
							// Render sublocation box and name
							aa->Render(x, y);

							// Center text
							float ty = ((12.0f * _scale) - it->RealText.Height()) / 2.0f;
							it->RealText.Render(x + 8 * _scale, -y + ty);
							if (it->Top == 0.0f)
							{
								it->Top = -y;
								it->Left = x;
								it->Bottom = it->RealText.Height() - y + ty;
								it->Right = x + 22 * _scale + it->RealText.Width();
							}
							y += aa->Bottom - 2;

							// If 4 rows added, go to next column
							if (++subCount == 4)
							{
								y = ti->Bottom;
								x += (ti->Right - ti->Left) / 2;
							}

							dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
							CShaders::SelectOrthoShader();
							if (subix == _selectedSubLocation)
							{
								CTravelImage* sl = _images[subix * 100];
								sl->Render();

								if (renderSelection)
								{
									dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
									CShaders::SelectColourShader();
									UINT stride = sizeof(COLOURED_VERTEX_ORTHO);
									UINT offset = 0;
									XMMATRIX wm = XMMatrixScaling(_scale, _scale, 1.0f) * XMMatrixTranslation(it->Left + 0.7f + 3 * _scale, -it->Top - 3 * _scale - 0.5f, -2.0f);
									CConstantBuffers::SetWorld(dx, &wm);
									dx.SetVertexBuffers(0, 1, &_selectionIndicator, &stride, &offset);
									dx.Draw(4, 0);

									CShaders::SelectOrthoShader();
								}
							}
						}

						subix++;
					}
				}
			}
		}

		if (_selectedLocation > 0)
		{
			CTravelImage* ti_goto = _images[2];
			ti_goto->Render();
		}

		if (CGameController::CanCancelTravel)
		{
			CTravelImage* ti_cancel = _images[4];
			ti_cancel->Render();
		}

		CModuleController::Cursors[0].SetPosition((float)_cursorPosX, (float)_cursorPosY);
		CModuleController::Cursors[0].Render();

		dx.EnableZBuffer();

		dx.Present(1, 0);
	}
}

void CUAKMTravelModule::CTravelImage::Render()
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

void CUAKMTravelModule::CTravelImage::Render(float x, float y)
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

void CUAKMTravelModule::Resize(int width, int height)
{
}

void CUAKMTravelModule::BeginAction()
{
	if (_selectedLocation > 0)
	{
		// Check if a sub-location was selected
		int subix = 17;
		int subEntry = 0;
		for (auto it : _subLocations)
		{
			if (it->ParentLocation == _selectedLocation && CGameController::GetData(UAKM_SAVE_TRAVEL + subix) != 0)
			{
				if (_cursorPosX >= it->Left && _cursorPosX < it->Right && _cursorPosY >= it->Top && _cursorPosY < it->Bottom)
				{
					_selectedSubLocation = subix;
					_selectedSubLocationEntry = subEntry;
					break;
				}
				subEntry++;
			}

			subix++;
		}
	}

	// Check if a main location was selected
	for (int i = 1; i <= 16; i++)
	{
		if (CGameController::GetData(UAKM_SAVE_TRAVEL + i) != 0)
		{
			CTravelImage* name = _images[100 * i];
			if (_cursorPosX >= name->Left && _cursorPosX < name->Right && _cursorPosY >= -name->Top && _cursorPosY < -name->Bottom)
			{
				_selectedLocation = i;
				_selectedSubLocation = -1;
				_selectedSubLocationEntry = 0;
				break;
			}
		}
	}

	// Check if Go To or Cancel were clicked
	if (_selectedLocation > 0 && _images[2]->HitTest((float)_cursorPosX, (float)_cursorPosY))
	{
		CAmbientAudio::Clear();
		CGameController::CanCancelTravel = TRUE;

		pMIDI->Stop();

		int ix = (_selectedSubLocation > 0) ? _selectedSubLocation : _selectedLocation;
		if (resultTable[ix * 2] != -1)
		{
			// Load location module
			int locationId = resultTable[ix * 2];
			CGameController::SetData(UAKM_SAVE_MAP_ENTRY, locationId);
			CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 0);
			CGameController::SetParameter(249, 0);
			CGameController::AutoSave();
			CModuleController::Push(new CLocationModule(locationId, 0));
		}
		else
		{
			// Load video module
			int id = resultTable[ix * 2 + 1];
			CGameController::SetData(UAKM_SAVE_DMAP_ENTRY, id);
			CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 1);
			CGameController::AutoSave();
			CModuleController::Push(new CVideoModule(VideoType::Scripted, id));
		}

		//CModuleController::Pop(this);
	}
	else if (CGameController::CanCancelTravel && _images[4]->HitTest(_cursorPosX, _cursorPosY))
	{
		CModuleController::Pop(this);
	}
}

void CUAKMTravelModule::Back()
{
	if (CGameController::CanCancelTravel)
	{
		CModuleController::Pop(this);
	}
}
