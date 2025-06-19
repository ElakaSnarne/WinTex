#include "PDTravelModule.h"
#include "GameController.h"
#include "PDGame.h"
#include "AmbientAudio.h"
#include "AnimationController.h"
#include "LZ.h"
#include "PDSubLocation.h"
#include "resource.h"

#define PD_TRAVEL_AREA_SAN_FRANCISCO	0
#define PD_TRAVEL_AREA_CHANDLER_AVENUE	1
#define PD_TRAVEL_AREA_NORTH_AMERICA	2
#define PD_TRAVEL_AREA_ROSWELL_LEVEL_1	3
#define PD_TRAVEL_AREA_ROSWELL_LEVEL_2	4
#define PD_TRAVEL_AREA_ROSWELL_LEVEL_3	5

#define PD_MAP_ENTRY_SAN_FRANCISCO		7
#define PD_MAP_ENTRY_CHANDLER_AVENUE	9
#define PD_MAP_ENTRY_NORTH_AMERICA		11
#define PD_MAP_ENTRY_ROSWELL_LV1		13
#define PD_MAP_ENTRY_ROSWELL_LV2		15
#define PD_MAP_ENTRY_ROSWELL_LV3		17

#define PD_TRAVEL_INDICATOR_AREA		57
#define PD_TRAVEL_INDICATOR_LOCATION	58

CPDTravelModule::CPDTravelModule()
{
	_currentPage = 0;
	_travelDataOffset = PD_SAVE_TRAVEL;
	_area = PD_TRAVEL_AREA_SAN_FRANCISCO;

	_blinkTime = 0;
	_pDestination = NULL;

	_subLocations.push_back(new CPDSubLocation(0, 0, 4, 530, 80, 440, 78, 116, 1, -1, 4, 1, -1, 6, 0, "Chandler Ave."));
	_subLocations.push_back(new CPDSubLocation(1, 1, 86, 381, 351, 293, 349, 126, -1, 2, -1, 0, 86, 0, 0, "Brew & Stew"));
	_subLocations.push_back(new CPDSubLocation(1, 2, 87, 247, 79, 218, 51, 122, -1, 4, -1, 0, 87, 0, 4, "Coit Tower"));
	_subLocations.push_back(new CPDSubLocation(1, 3, 12, 381, 292, 329, 277, 123, -1, -1, 12, 1, 12, 0, 0, "Electronics\rShop"));
	_subLocations.push_back(new CPDSubLocation(1, 4, 88, 381, 163, 326, 155, 121, 2, 9, -1, 0, 88, 0, 4, "Fuchsia\rFlamingo"));
	_subLocations.push_back(new CPDSubLocation(1, 5, 4, 451, 231, 467, 230, 120, -1, -1, 4, 1, 4, 0, 63, "Street or NewsStand"));
	_subLocations.push_back(new CPDSubLocation(1, 6, 85, 464, 260, 481, 253, 125, -1, 3, -1, 0, 85, 0, 0, "Rook's PawnShop"));
	_subLocations.push_back(new CPDSubLocation(1, 7, 1, 381, 219, 348, 218, 120, 0, -1, 1, 1, 1, 0, 23, "Ritz"));
	_subLocations.push_back(new CPDSubLocation(1, 8, 6, 464, 189, 481, 188, 123, 1, -1, 6, 1, 6, 0, 15, "Rusty's Fun House"));
	_subLocations.push_back(new CPDSubLocation(1, 9, 67, 494, 150, 511, 150, 118, -1, -1, 67, 2, 72, 0, 15, "Sewer"));
	_subLocations.push_back(new CPDSubLocation(1, 10, 9, 516, 117, 531, 109, 126, -1, 69, -1, 0, 9, 0, 5, "ACME\rWarehouse"));
	_subLocations.push_back(new CPDSubLocation(1, 11, 11, 464, 288, 481, 287, 118, -1, -1, 11, 1, 11, 0, 15, "Alley"));
	_subLocations.push_back(new CPDSubLocation(1, 12, 65, 293, 173, 283, 155, 118, -1, -1, 65, 1, 65, 0, 15, "Alley"));
	_subLocations.push_back(new CPDSubLocation(1, 13, 69, 433, 22, 450, 21, 118, -1, -1, 69, 1, 69, 0, 4, "Alley"));
	_subLocations.push_back(new CPDSubLocation(1, 14, 68, 558, 349, 549, 330, 118, -1, -1, 68, 1, 68, 0, 15, "Alley"));
	_subLocations.push_back(new CPDSubLocation(1, 15, 67, 544, 268, 559, 267, 118, -1, -1, 67, 1, 67, 0, 15, "Sewer"));
	_subLocations.push_back(new CPDSubLocation(7, 16, 1, 0, 0, 0, 0, 120, 0, -1, 1, 1, 1, 0, 23, "Tex's Office"));
	_subLocations.push_back(new CPDSubLocation(7, 19, 3, 0, 0, 0, 0, 120, 0, -1, 3, 1, 3, 0, 3, "Malloy's Room"));
	_subLocations.push_back(new CPDSubLocation(7, 20, 2, 0, 0, 0, 0, 120, 0, -1, 2, 1, 2, 0, 3, "Lobby"));
	_subLocations.push_back(new CPDSubLocation(7, 21, 5, 0, 0, 0, 0, 120, -1, -1, 5, 4, 5, 0, 3, "Stairs/Hallway"));
	_subLocations.push_back(new CPDSubLocation(7, 17, 70, 0, 0, 0, 0, 120, -1, -1, 70, 1, 70, 0, 19, "Tex's Bedroom"));
	_subLocations.push_back(new CPDSubLocation(7, 18, 71, 0, 0, 0, 0, 120, -1, -1, 71, 1, 71, 0, 19, "Tex's Computer room"));
	_subLocations.push_back(new CPDSubLocation(8, 22, 7, 0, 0, 0, 0, 123, -1, -1, 7, 1, 7, 0, 15, "Fun House Roof"));
	_subLocations.push_back(new CPDSubLocation(4, 23, 91, 0, 0, 0, 0, 121, -1, 9, -1, 0, 91, 0, 4, "Emily's Apt"));
	_subLocations.push_back(new CPDSubLocation(0, 24, 22, 414, 336, 430, 336, 123, 4, 58, -1, 0, 92, 0, 21, "Autotech"));
	_subLocations.push_back(new CPDSubLocation(0, 25, 93, 425, 50, 441, 50, 121, -1, 24, -1, 0, 93, 0, 0, "Chelsee's Apt"));
	_subLocations.push_back(new CPDSubLocation(0, 26, 94, 220, 146, 236, 138, 121, -1, 16, -1, 0, 94, 0, 0, "Cosmic\rConnection"));
	_subLocations.push_back(new CPDSubLocation(0, 27, 25, 312, 391, 328, 391, 114, -1, -1, 25, 1, 25, 0, 1, "Morgue"));
	_subLocations.push_back(new CPDSubLocation(0, 28, 96, 333, 124, 349, 124, 120, -1, 22, -1, 0, 96, 0, 16, "Imperial Lounge"));
	_subLocations.push_back(new CPDSubLocation(0, 29, 84, 542, 187, 558, 180, 124, -1, 11, -1, 0, 84, 0, 0, "Police\rStation"));
	_subLocations.push_back(new CPDSubLocation(0, 30, 98, 480, 225, 496, 225, 127, -1, 54, -1, 0, 98, 0, 1, "Post Office"));
	_subLocations.push_back(new CPDSubLocation(0, 31, 20, 293, 209, 309, 209, 126, -1, -1, 20, 1, 20, 0, 21, "Sandra Collin's House"));
	_subLocations.push_back(new CPDSubLocation(0, 32, 99, 301, 74, 317, 74, 122, -1, -1, -1, 0, 99, 0, 0, "Savoy Hotel"));
	_subLocations.push_back(new CPDSubLocation(0, 33, 97, 236, 293, 252, 293, 118, -1, -1, -1, 0, 97, 0, 0, "Twilight Lounge"));
	_subLocations.push_back(new CPDSubLocation(0, 34, 29, 594, 377, 515, 368, 123, -1, 10, -1, 0, 29, 0, 1, "Waterfront\rWarehouse"));
	_subLocations.push_back(new CPDSubLocation(0, 35, 28, 456, 248, 472, 248, 125, -1, 18, -1, 0, 28, 0, 0, "Garden House"));
	_subLocations.push_back(new CPDSubLocation(2, 36, 0, 221, 62, 237, 61, 116, 0, -1, 4, 1, -1, 6, 0, "San Francisco"));
	_subLocations.push_back(new CPDSubLocation(2, 37, 54, 270, 16, 286, 15, 126, -1, -1, 54, 1, 54, 0, 16, "Elijah Witt's Apt"));
	_subLocations.push_back(new CPDSubLocation(2, 38, 31, 313, 113, 329, 112, 123, 3, 60, -1, 0, 95, 0, 8, "Roswell"));
	_subLocations.push_back(new CPDSubLocation(2, 39, 62, 415, 235, 334, 234, 125, -1, -1, 57, 1, 57, 0, 32, "Mayan Ruins"));
	_subLocations.push_back(new CPDSubLocation(2, 40, 15, 250, 43, 266, 42, 119, -1, 72, -1, 0, 15, 0, 2, "Cabin"));
	_subLocations.push_back(new CPDSubLocation(38, 41, 26, 0, 0, 0, 0, 123, -1, -1, 26, 11, 26, 0, 8, "Roswell interior"));
	_subLocations.push_back(new CPDSubLocation(5, 42, 47, 416, 252, 434, 251, 127, -1, -1, 47, 1, 47, 0, 8, "Hangar"));
	_subLocations.push_back(new CPDSubLocation(3, 43, 38, 338, 302, 300, 283, 116, -1, -1, 38, 1, 38, 0, 8, "Storage#104"));
	_subLocations.push_back(new CPDSubLocation(3, 44, 39, 385, 143, 294, 142, 116, -1, -1, 39, 1, 39, 0, 8, "Storage#102"));
	_subLocations.push_back(new CPDSubLocation(5, 45, 40, 386, 158, 296, 157, 116, -1, -1, 40, 1, 40, 0, 8, "Misc Storage"));
	_subLocations.push_back(new CPDSubLocation(5, 46, 49, 386, 303, 273, 303, 116, -1, -1, 49, 1, 49, 0, 8, "Storage 101-200"));
	_subLocations.push_back(new CPDSubLocation(4, 47, 42, 433, 217, 410, 231, 125, -1, -1, 42, 1, 42, 0, 8, "War room"));
	_subLocations.push_back(new CPDSubLocation(3, 48, 35, 385, 232, 325, 231, 125, -1, -1, 35, 1, 35, 0, 8, "Mess hall"));
	_subLocations.push_back(new CPDSubLocation(3, 49, 36, 505, 381, 488, 360, 122, -1, -1, 36, 1, 36, 0, 8, "Rec hall"));
	_subLocations.push_back(new CPDSubLocation(3, 50, 37, 269, 322, 222, 321, 125, -1, -1, 37, 1, 37, 0, 8, "Dorms"));
	_subLocations.push_back(new CPDSubLocation(3, 51, 41, 315, 83, 264, 53, 127, -1, -1, 41, 1, 41, 0, 8, "Generator room"));
	_subLocations.push_back(new CPDSubLocation(3, 52, 26, 448, 395, 396, 394, 127, -1, -1, 26, 11, 26, 0, 8, "Hallway"));
	_subLocations.push_back(new CPDSubLocation(4, 53, 27, 509, 293, 490, 308, 127, -1, -1, 27, 1, 27, 0, 8, "Hallway"));
	_subLocations.push_back(new CPDSubLocation(24, 55, 22, 0, 0, 0, 0, 123, -1, -1, 22, 1, 22, 0, 21, "Lobby"));
	_subLocations.push_back(new CPDSubLocation(24, 54, 21, 0, 0, 0, 0, 123, -1, -1, 21, 1, 21, 0, 21, "Hallway"));
	_subLocations.push_back(new CPDSubLocation(24, 57, 24, 0, 0, 0, 0, 123, -1, -1, 24, 1, 24, 0, 21, "Horton's office"));
	_subLocations.push_back(new CPDSubLocation(24, 56, 23, 0, 0, 0, 0, 123, -1, -1, 23, 1, 23, 0, 21, "Evidence room"));

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
}

CPDTravelModule::~CPDTravelModule()
{
	Dispose();

	for (auto pData : _pdImages)
	{
		delete[] pData.second.Data;
	}
}

void CPDTravelModule::Initialize()
{
	CFullScreenModule::Initialize();

	CAnimationController::Clear();

	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	_rawFont.Init(IDR_RAWFONT_PD);

	// TRAVEL.AP
	// 0-1 = main page
	// 2-3 = APs with buttons & icons
	// 4-5 = empty
	// 6-7 = San Francisco
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
		_data = new BYTE[length];
		if (_data != NULL)
		{
			file.Seek(0);
			file.Read(_data, length);
			file.Close();
			int count = GetInt(_data, 0, 2) - 1;

			ReadPalette(_data + GetInt(_data, 2, 4), 0, 0x23);
			ReadPalette(_data + GetInt(_data, 2 + 6 * 4, 4), 0x23, 0x5d);	// Palette for Chandler Avenue, San Francisco and North America
			//ReadPalette(_data + GetInt(_data, 2 + 12 * 4, 4), 0x23, 0x5d);	// Palette for Roswell
			for (int i = 0; i < 256; i++)
			{
				_originalPalette[i] = _palette[i];
			}

			// TODO: Screen to use depends on default map / current player location
			_screen = new BYTE[640 * 480];
			FillMemory(_screen, 640 * 480, 0);

			RenderImage(1, 0, 0, 640, 445);

			RenderArea();

			RenderSubImage(2, 51, 3, 13, 0, 455, 124, 15);			// Travel button
			RenderSubImage(2, 7, 5, 13, 640 - 124, 455, 124, 15);	// Cancel button

			UpdateTexture();
		}
	}
}

void CPDTravelModule::RenderArea()
{
	if (_area == PD_TRAVEL_AREA_SAN_FRANCISCO)
	{
		RenderSanFrancisco();
		RenderSubImage(2, 18, 0, 0, 13, 33, 176, 21);
		RenderSubImage(2, 44, 0, 0, 13, 53, 176, 21);
		RenderSubImage(2, 5, 0, 0, 13, 75, 176, 21);
	}
	else if (_area == PD_TRAVEL_AREA_CHANDLER_AVENUE)
	{
		RenderChandlerAvenue();
		RenderSubImage(2, 19, 0, 0, 13, 33, 176, 21);
		RenderSubImage(2, 43, 0, 0, 13, 53, 176, 21);
		RenderSubImage(2, 5, 0, 0, 13, 75, 176, 21);
	}
	else if (_area == PD_TRAVEL_AREA_NORTH_AMERICA)
	{
		RenderNorthAmerica();
		RenderSubImage(2, 18, 0, 0, 13, 33, 176, 21);
		RenderSubImage(2, 43, 0, 0, 13, 53, 176, 21);
		RenderSubImage(2, 6, 0, 0, 13, 75, 176, 21);
	}
	else if (_area >= PD_TRAVEL_AREA_ROSWELL_LEVEL_1 && _area <= PD_TRAVEL_AREA_ROSWELL_LEVEL_3)
	{
		RenderRoswell(_area - PD_TRAVEL_AREA_ROSWELL_LEVEL_1);
	}

	RenderLocations();
}

void CPDTravelModule::RenderChandlerAvenue()
{
	// TODO: Apply regular palette
	RenderImage(PD_MAP_ENTRY_CHANDLER_AVENUE, 208, 15, 415, 415);
}

void CPDTravelModule::RenderSanFrancisco()
{
	// TODO: Apply regular palette
	RenderImage(PD_MAP_ENTRY_SAN_FRANCISCO, 208, 15, 415, 415);
}

void CPDTravelModule::RenderNorthAmerica()
{
	// TODO: Apply regular palette
	RenderImage(PD_MAP_ENTRY_NORTH_AMERICA, 208, 15, 415, 415);
}

void CPDTravelModule::RenderRoswell(int level)
{
	// TODO: Apply Roswell palette
	RenderImage(PD_MAP_ENTRY_ROSWELL_LV1 + (level - 1) * 2, 208, 15, 415, 415);

	RenderSubImage(3, level == 0 ? 19 : 18, 0, 0, 13, 33, 176, 21);
	RenderSubImage(3, level == 1 ? 44 : 43, 0, 0, 13, 53, 176, 21);
	RenderSubImage(3, level == 2 ? 6 : 5, 0, 0, 13, 75, 176, 21);
}

void CPDTravelModule::Render()
{
	if (_currentPage < 0)
	{
		// Fade out
		FadeOut(0x23, 0x80, 0, 10);
		if (_currentFrame == 10)
		{
			RenderArea();
			_currentPage = 1;
			_currentFrame = 0;
		}
	}
	else if (_currentPage > 0)
	{
		// Fade in
		FadeIn(0x23, 0x80, 0, 10);
		if (_currentFrame == 10)
		{
			_currentPage = 0;
			_currentFrame = 0;
		}
	}

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

		CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
		CModuleController::Cursors[0].Render();

		// TODO: Add flashing indicator of selected location and sub location

		dx.EnableZBuffer();
	}
}

void CPDTravelModule::BeginAction()
{
	int x = (int)((_cursorPosX - _left) / _scale);
	int y = (int)((_cursorPosY - _top) / _scale);

	for (auto location : _subLocations)
	{
		CPDSubLocation* pPDSL = (CPDSubLocation*)location;
		if (pPDSL->ParentLocation == _area && (pPDSL->Type == 6 || CGameController::GetData(PD_SAVE_TRAVEL + pPDSL->TravelIndex) != 0) && pPDSL->HitTest(x, y))
		{
			Fill(16, 114, 191, 236, 0);	// Clear location/sub-location image (update to render default image)
			Fill(18, 253, 191, 429, 0);	// Clear sub-locations list

			if (pPDSL->Type == 6)
			{
				// Navigate to area
				_area = pPDSL->Area;
				//_inputEnabled = FALSE;
				_currentPage = -1;
				_currentFrame = 0;
				_pDestination = NULL;
			}
			else
			{
				_pDestination = pPDSL;

				// Render selected location name
				std::unordered_map<int, int> colourMap;
				colourMap[2] = pPDSL->ColourIndex;
				_rawFont.Render(_screen, 640, 480, 20, 255, (char*)pPDSL->Text.c_str(), colourMap, -1, -1, TRUE);

				// Render location image
				RenderLocationImage(pPDSL->TravelIndex);

				// Render accessible sub-locations
				int y = 32;
				for (auto subLocation : _subLocations)
				{
					CPDSubLocation* pSPDSL = (CPDSubLocation*)subLocation;
					if (pSPDSL->IndicatorX == 0 && pSPDSL->ParentLocation == pPDSL->Id && (pPDSL->Type == 6 || CGameController::GetData(PD_SAVE_TRAVEL + pSPDSL->TravelIndex) != 0))
					{
						pSPDSL->IconHitBox = RenderSubImage(2, PD_TRAVEL_INDICATOR_LOCATION, 35, 251 + y);
						pSPDSL->NameHitBox = _rawFont.Render(_screen, 640, 480, 50, 251 + y, (char*)pSPDSL->Text.c_str(), colourMap, -1, -1, TRUE);
						y += _rawFont.GetHeight() - 2;
					}
				}

				_selectedLocation = pPDSL->Id;
				_selectedSubLocation = -1;
			}

			UpdateTexture();
			break;
		}
		else if (pPDSL->ParentLocation == _selectedLocation && pPDSL->HitTest(x, y))
		{
			_selectedSubLocation = pPDSL->Id;
			_pDestination = pPDSL;
			// Render location image
			RenderLocationImage(pPDSL->TravelIndex);
			UpdateTexture();
			break;
		}
	}

	// Check if any of the map buttons is clicked
	if (x >= 13 && x < 189 && y >= 33 && y < 54)
	{
		// Top map
		_area = PD_TRAVEL_AREA_CHANDLER_AVENUE;
		_currentPage = -1;
		_currentFrame = 0;
	}
	else if (x >= 13 && x < 189 && y >= 53 && y < 74)
	{
		// Middle map
		_area = PD_TRAVEL_AREA_SAN_FRANCISCO;
		_currentPage = -1;
		_currentFrame = 0;
	}
	else if (x >= 13 && x < 189 && y >= 75 && y < 96)
	{
		// Bottom map
		_area = PD_TRAVEL_AREA_NORTH_AMERICA;
		_currentPage = -1;
		_currentFrame = 0;
	}

	// Check if cancel is clicked
	if (x >= 516 && x < 640 && y >= 455 && y < 470)
	{
		CModuleController::Pop(this);
	}

	// Check if travel is clicked
	if (x >= 0 && x < 124 && y >= 455 && y < 470)
	{
		CAmbientAudio::Clear();
		CGameController::CanCancelTravel = TRUE;

		pMIDI->Stop();

		if (_pDestination != NULL)
		{
			if (_pDestination->DMapId == -1)
			{
				// Load location module
				//int locationId = _resultTable[ix * 2];
				//CGameController::SetData(UAKM_SAVE_MAP_ENTRY, locationId);
				//CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 0);
				//CGameController::SetParameter(249, 0);
				//CGameController::AutoSave();
				CModuleController::Push(new CPDLocationModule(_pDestination->MapId, 0));
			}
			else
			{
				// Load video module
				//int id = _resultTable[ix * 2 + 1];
				//CGameController::SetData(UAKM_SAVE_DMAP_ENTRY, id);
				//CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 1);
				//CGameController::AutoSave();
				CModuleController::Push(new CVideoModule(VideoType::Scripted, _pDestination->DMapId));
			}
		}
	}
}

void CPDTravelModule::RenderImage(int entryIndex, int x, int y, int w, int h)
{
	BinaryData bd = _pdImages[entryIndex];
	if (bd.Data == NULL)
	{
		LPBYTE pCompressed = _data + GetInt(_data, 2 + entryIndex * 4, 4);
		int length = GetInt(_data, 6 + entryIndex * 4, 4) - GetInt(_data, 2 + entryIndex * 4, 4);
		bd = CLZ::Decompress(pCompressed, length);
		_pdImages[entryIndex] = bd;
	}

	if (bd.Data != NULL)
	{
		RenderRaw(bd.Data, x, y, w, h);
	}
}

RECT CPDTravelModule::RenderSubImage(int entryIndex, int subEntryIndex, int x, int y)
{
	BinaryData bd = _pdImages[entryIndex];
	if (bd.Data == NULL)
	{
		LPBYTE pCompressed = _data + GetInt(_data, 2 + entryIndex * 4, 4);
		int length = GetInt(_data, 6 + entryIndex * 4, 4) - GetInt(_data, 2 + entryIndex * 4, 4);
		bd = CLZ::Decompress(pCompressed, length);
		_pdImages[entryIndex] = bd;
	}

	if (bd.Data != NULL)
	{
		// Now find sub image
		LPBYTE pImage = bd.Data + GetInt(bd.Data, 2 + subEntryIndex * 4, 4);
		return RenderItem(pImage, x, y, -1, -1, -1, -1, 0);
	}

	return { 0 };
}

void CPDTravelModule::RenderSubImage(int entryIndex, int subEntryIndex, int srcOffsetX, int srcOffsetY, int dstX, int dstY, int w, int h)
{
	BinaryData bd = _pdImages[entryIndex];
	if (bd.Data == NULL)
	{
		LPBYTE pCompressed = _data + GetInt(_data, 2 + entryIndex * 4, 4);
		int length = GetInt(_data, 6 + entryIndex * 4, 4) - GetInt(_data, 2 + entryIndex * 4, 4);
		bd = CLZ::Decompress(pCompressed, length);
		_pdImages[entryIndex] = bd;
	}

	if (bd.Data != NULL)
	{
		// Now find sub image
		LPBYTE pImage = bd.Data + GetInt(bd.Data, 2 + subEntryIndex * 4, 4);
		//RenderItem(pImage, dstX, dstY, dstX - srcOffsetX, dstX + w, dstY - srcOffsetY, dstY + h, 0);
		RenderItemOffset(pImage, srcOffsetX, srcOffsetY, dstX, dstY, w, h);
	}
}


void CPDTravelModule::RenderLocations()
{
	for (auto location : _subLocations)
	{
		CPDSubLocation* pPDSL = (CPDSubLocation*)location;
		if (pPDSL->ParentLocation == _area)
		{
			if (pPDSL->Type == 6 || CGameController::GetData(PD_SAVE_TRAVEL + pPDSL->TravelIndex) != 0)
			{
				std::unordered_map<int, int> colourMap;
				colourMap[2] = pPDSL->ColourIndex;

				pPDSL->IconHitBox = RenderSubImage(2, pPDSL->Type == 6 ? PD_TRAVEL_INDICATOR_AREA : PD_TRAVEL_INDICATOR_LOCATION, pPDSL->IndicatorX, pPDSL->IndicatorY);
				pPDSL->NameHitBox = _rawFont.Render(_screen, 640, 480, pPDSL->NameX, pPDSL->NameY, (char*)pPDSL->Text.c_str(), colourMap, -1, -1, FALSE);
			}
		}
	}
}

void CPDTravelModule::RenderLocationImage(int locationId)
{
	// Palette/compressed image pairs start at entry 22
	int entry = 20 + locationId * 2;
	ReadPalette(_data + GetInt(_data, 2 + entry * 4, 4), 0x80, 0x80);

	BinaryData bd = _pdImages[entry + 101];	// Offsetting by 100 to share container
	if (bd.Data == NULL)
	{
		bd = CLZ::Decompress(_data + GetInt(_data, 6 + entry * 4, 4), GetInt(_data, 10 + entry * 4, 4) - GetInt(_data, 6 + entry * 4, 4));
		_pdImages[entry + 101] = bd;
	}

	if (bd.Data != NULL)
	{
		RenderRaw(bd.Data, 16, 114, 175, 122);
	}
}

// 0-6		North America
// 7-12		Cancel
// 13-19	Chandler Avenue
// 20-25	Help
// 26-31	Hint
// 32-37	Load
// 38-44	San Francisco
// 45-50	Save
// 51-56	Travel to
// 57		Ball
// 58		Square
// 59		?
// 60		?
// 61		Indicator?
// 62		Location on current CD indicator

