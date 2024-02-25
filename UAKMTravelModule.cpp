#include "UAKMTravelModule.h"
#include "Utilities.h"
#include "File.h"
#include "GameController.h"
#include "UAKMGame.h"
#include "LocationModule.h"
#include "VideoModule.h"
#include "AmbientAudio.h"
#include "AnimationController.h"

CUAKMTravelModule::CUAKMTravelModule()
{
	_travelDataOffset = UAKM_SAVE_TRAVEL;

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

	_coordinates = &coordinates[0];
	_hotspots = &hotspots[0];
	_resultTable = &resultTable[0];
}

CUAKMTravelModule::~CUAKMTravelModule()
{
	Dispose();
}

void CUAKMTravelModule::Initialize()
{
	CAnimationController::Clear();

	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	// Load palette(s)
	BinaryData bdPal = LoadEntry(L"GRAPHICS.AP", 28);
	if (bdPal.Data != NULL)
	{
		ReadPalette(bdPal.Data);
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
			int count = GetInt(data, 0, 2) - 1;

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

			for (int i = 0; i < count; i++)
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
								ti->Left = _left = (w - requiredWidth * _scale) / 2;
								ti->Right = ti->Left + width;
								ti->Top = _top = -(h - requiredHeight * _scale) / 2;
								ti->Bottom = ti->Top + height;
							}
							else if (j == 0 && i < 17)
							{
								// Location name
								ti->Left = _left + (_coordinates[i * 2] - _coordinates[0]) * _scale;
								ti->Right = ti->Left + width;
								ti->Top = _top - (_coordinates[i * 2 + 1] - _coordinates[1]) * _scale;
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
									ti->Left = _left + 9 * _scale;
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
								ti->Left = _left + 425 * _scale;
								ti->Right = ti->Left + width;
								ti->Top = _top;
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

		int i = 0;
		while (_coordinates[(++i) * 2] != -1)
		{
			if (CGameController::GetData(_travelDataOffset + i) != 0)
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
						XMMATRIX wm = XMMatrixScaling(_scale, _scale, 1.0f) * XMMatrixTranslation(ti->Left + (_hotspots[i * 2 + 0] - 8) * _scale, ti->Top - (_hotspots[i * 2 + 1] - 6) * _scale, -2.0f);
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
						if (it->ParentLocation == i && CGameController::GetData(_travelDataOffset + subix) != 0)
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

void CUAKMTravelModule::Travel()
{
}
