#include "PDTravelModule.h"
#include "GameController.h"
#include "PDGame.h"
#include "AmbientAudio.h"
#include "AnimationController.h"
#include "LZ.h"

//short pdcoordinates[] = { 8, 6, 245, 71, 26, 272, 168, 188, 349, 178, 335, 58, 26, 272, 171, 260, 276, 139, 349, 75, 168, 188, 18, 144, 130, 37, 307, 231, 36, 229, 19, 27, 266, 45 };
//short pdhotspots[] = { 0, 0, 340, 74, 148, 275, 171, 191, 352, 181, 338, 61, 148, 275, 292, 263, 376, 142, 352, 78, 171, 191, 21, 147, 231, 40, 334, 234, 39, 232, 22, 30, 340, 48 };
//signed char pdresultTable[] = { -1, -1, 2, -1, -1, 12, -1, 31, -1, 14, 1, -1, 15, -1, -1, 13, -1, 43, -1, 5, 21, -1, -1, 15, -1, 16, 26, -1, 33, -1, -1, 18, -1, 9, 2, -1, -1, 2, -1, 4, -1, 3, 14, -1, 5, -1, 8, -1, 9, -1, -1, 43, 20, -1, 16, -1, 19, -1, -1, 5, 22, -1, 23, -1, 25, -1, 24, -1, 26, -1, 27, -1, 30, -1, 28, -1, 29, -1 };

CPDTravelModule::CPDTravelModule()
{
	_travelDataOffset = PD_SAVE_TRAVEL;

	//_subLocations.push_back(new CSubLocation(1, "STREET or NEWSSTAND"));
	//_subLocations.push_back(new CSubLocation(1, "BREW & STEW"));
	//_subLocations.push_back(new CSubLocation(1, "SLICE O'HEAVEN PIZZA"));
	//_subLocations.push_back(new CSubLocation(1, "ROOK'S PAWNSHOP"));
	//_subLocations.push_back(new CSubLocation(1, "SNO WHITE WAREHOUSE"));
	//_subLocations.push_back(new CSubLocation(1, "ALLEY (behind PAWNSHOP)"));
	//_subLocations.push_back(new CSubLocation(1, "RUSTY'S FUN HOUSE"));
	//_subLocations.push_back(new CSubLocation(1, "ELECTRONICS SHOP"));
	//_subLocations.push_back(new CSubLocation(8, "LIBRARY"));
	//_subLocations.push_back(new CSubLocation(8, "HALLWAY"));
	//_subLocations.push_back(new CSubLocation(8, "STUDY"));
	//_subLocations.push_back(new CSubLocation(8, "SECRET ROOM"));
	//_subLocations.push_back(new CSubLocation(9, "HOTEL LOBBY"));
	//_subLocations.push_back(new CSubLocation(9, "SUITE ENTRYWAY"));
	//_subLocations.push_back(new CSubLocation(9, "SUITE BEDROOM"));
	//_subLocations.push_back(new CSubLocation(9, "SUITE PIANO ROOM"));
	//_subLocations.push_back(new CSubLocation(9, "SUITE HOT TUB"));
	//_subLocations.push_back(new CSubLocation(13, "HALLWAY"));
	//_subLocations.push_back(new CSubLocation(13, "R & D OFFICE"));
	//_subLocations.push_back(new CSubLocation(13, "SUPERVISOR'S OFFICE"));
	//_subLocations.push_back(new CSubLocation(13, "CONFERENCE ROOM"));
	//_subLocations.push_back(new CSubLocation(13, "MARCUS TUCKER'S OFFICE"));

	/*
				{ 00,"San Francisco" },
				{ 01,"Tex' office"},
				{ 02,"Ritz lobby"},
				{ 03,"Malloy's room at the Ritz"},
				{ 04,"Chandler Avenue"},
				{ 05,"Behind Ritz"},
				{ 06,"Rusty's Fun House"},
				{ 07,"Rusty's roof"},
				{ 08,"Watertower"},
				{ 09,"ACME warehouse"},
				{ 11,"Alley behind Pawnshop"},
				{ 12,"Electronics shop"},
				{ 15,"Cabin"},
				{ 17,"Easter egg room"},
				{ 20,"Sandra Collins' room"},
				{ 21,"Autotech(NSA) Hallway"},
				{ 22,"Autotech(NSA) Lobby"},
				{ 23,"NSA Evidence room"},
				{ 24,"NSA Horton's office"},
				{ 25,"Morgue"},
				{ 26,"Roswell hallway 1"},
				{ 27,"Roswell hallway 2"},
				{ 28,"Malloy's room (Garden house)"},
				{ 29,"Malloy's Warehouse"},
				{ 31,"Roswell Security Station"},
				{ 32,"Roswell Compound Exterior"},
				{ 33,"Roswell Air Ducts, level 2"},
				{ 34,"Roswell Air Ducts, level 3"},
				{ 35,"Roswell Mess hall"},
				{ 36,"Roswell Recreation hall"},
				{ 37,"Roswell Dorms"},
				{ 38,"Roswell storage#104"},
				{ 39,"Roswell storage#102"},
				{ 40,"Misc Storage, level 3"},
				{ 41,"Roswell Generator room"},
				{ 42,"Roswell War Room"},
				{ 43,"Roswell Linguistics Lab"},
				{ 44,"Roswell Metallurgy Lab"},
				{ 45,"Roswell Bio Lab"},
				{ 46,"Roswell Computer/ Science Lab"},
				{ 47,"Roswell Hangar"},
				{ 48,"Roswell Air Ducts, level 1"},
				{ 49,"Roswell Storage 101 - 200"},
				{ 54,"Elijah Witt's apartment"},
				{ 57,"Mayan Chamber 1"},
				{ 58,"Mayan Chamber 2"},
				{ 59,"Mayan Chamber 3"},
				{ 60,"Mayan Chamber 4"},
				{ 61,"Mayan Chamber 5" },
				{ 62,"Mayan Labyrinth 1"},
				{ 63,"Mayan Labyrinth 2"},
				{ 64,"Mayan Ascension chamber"},
				{ 65,"Alley behind Ritz"},
				{ 66,"Mayan Fireball room"},
				{ 67,"Sewer"},
				{ 68,"Stairway Alley"},
				{ 69,"Alley behind Golden Gate"},
				{ 70,"Tex' Bedroom"},
				{ 71,"Tex' Computer room"},
				{ 84,"Police Station" },
				{ 85,"Rook's PawnShop" },
				{ 86,"Brew & Stew" },
				{ 87,"Coit Tower" },
				{ 88,"Fuchsia Flamingo" },
				{ 91,"Emily's Apt" },
				{ 93,"Chelsee's Apt" },
				{ 94,"Cosmic Connection" },
				{ 96,"Imperial Lounge" },
				{ 97,"Twilight Lounge" },
				{ 98,"Post Office" },
				{ 99,"Savoy Hotel" } };
	*/

	//_coordinates = &coordinates[0];
	//_hotspots = &hotspots[0];
	//_resultTable = &resultTable[0];
}

CPDTravelModule::~CPDTravelModule()
{
	Dispose();
}

void CPDTravelModule::Initialize()
{
	CFullScreenModule::Initialize();

	CAnimationController::Clear();

	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	// TODO: Load graphics
	//LoadDoubleEntry(L"TRAVEl.AP", 0);

	// TRAVEL.AP
	// 0-1 = main page
	// 2-3 = APs with buttons/icons
	// 4-5 = empty
	// 6-7 = San Fransisco
	// 8-9 = Chandler Avenue
	// 10-11 = North America
	// 12-13 = Roswell level 1
	// 14-15 = Roswell level 2
	// 16-17 = Roswell level 3
	// 18-21 = empty
	// 22-23> = Location images
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
			//float w = (float)dx.GetWidth();
			//float h = (float)dx.GetHeight();

			//float sx = w / 640.0;
			//float sy = h / 480.0;
			//_scale = min(sx, sy);

			ReadPalette(data + GetInt(data, 2, 4));

			_screen = CLZ::Decompress(data + GetInt(data, 6, 4), GetInt(data, 10, 4) - GetInt(data, 6, 4)).Data;

			UpdateTexture();

			for (int i = 0; i < count; i++)
			{
				break;
				// Inside maps at approx 209x16

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
								//ti->Left = _left = (w - 640 * _scale) / 2;
								//ti->Right = ti->Left + width;
								//ti->Top = _top = -(h - 480 * _scale) / 2;
								//ti->Bottom = ti->Top + height;
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
}

/*
void CPDTravelModule::BeginAction()
{
	if (_selectedLocation > 0)
	{
		// Check if a sub-location was selected
		int subix = 17;
		int subEntry = 0;
		for (auto it : _subLocations)
		{
			if (it->ParentLocation == _selectedLocation && CGameController::GetData(PD_SAVE_TRAVEL + subix) != 0)
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
		if (CGameController::GetData(PD_SAVE_TRAVEL + i) != 0)
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
		if (pdresultTable[ix * 2] != -1)
		{
			// Load location module
			int locationId = pdresultTable[ix * 2];
			//CGameController::SetData(PD_SAVE_MAP_ENTRY, locationId);
			//CGameController::SetData(PD_SAVE_DMAP_FLAG, 0);
			//CGameController::SetParameter(249, 0);
			//CGameController::AutoSave();
			//CModuleController::Push(new CPDLocationModule(locationId, 0));
		}
		else
		{
			// Load video module
			int id = pdresultTable[ix * 2 + 1];
			//CGameController::SetData(PD_SAVE_DMAP_ENTRY, id);
			//CGameController::SetData(PD_SAVE_DMAP_FLAG, 1);
			//CGameController::AutoSave();
			//CModuleController::Push(new CVideoModule(VideoType::Scripted, id));
		}

		//CModuleController::Pop(this);
	}
	else if (CGameController::CanCancelTravel && _images[4]->HitTest(_cursorPosX, _cursorPosY))
	{
		CModuleController::Pop(this);
	}
}
*/

void CPDTravelModule::Render()
{
	if (_vertexBuffer != NULL)
	{
		dx.DisableZBuffer();

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();
		XMMATRIX wm = XMMatrixIdentity();
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = _texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		//if (_page > 0 && _page <= 8 && _page != 5)
		//{
		//	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
		//	CModuleController::Cursors[0].Render();
		//}

		dx.EnableZBuffer();
	}
}


void CPDTravelModule::Travel()
{
}

/*
Map data (primary screen*100+location id)

;0	0,29,24,25,26,27,28,30,31,32,33,34,35,-1
;1	101,102,103,104,105,106,107,108,110,111,112,113,114,115,109,-1
;2	236,237,238,239,240,-1
;3	352,350,351,348,349,343,344,-1
;4	453,447,-1
;5	542,545,546,-1
*/
/*
Table 2
;0	16,19,20,21,17,18,-1
;1	122,-1
;2	223,-1
;3	341,-1
;4	455,454,457,456,-1
*/
