#include "PDDragDropPuzzleModule.h"
#include "Globals.h"
#include "Utilities.h"
#include "GameController.h"
#include "PDGame.h"
#include "resource.h"
#include "PDLaptopModule.h"

WCHAR* CPDDragDropPuzzleModule::FileNames[] = { L"PANDORA.AP", L"SPECIAL.AP" };
int CPDDragDropPuzzleModule::PuzzleFiles[] = { 0, 1, 1, 1, 1, 1, 1, 1 };
int CPDDragDropPuzzleModule::PuzzleEntries[] = { 14, 11, 14, 17, 20, 26, 67, 74 };
int CPDDragDropPuzzleModule::PuzzlePiecesCount[] = { 26, 9, 36, 24, 13, 5, 18, 16 };
int CPDDragDropPuzzleModule::PuzzleDataOffsets[] = { PD_SAVE_PANDORA_PUZZLE, PD_SAVE_FIGURES_PUZZLE, PD_SAVE_TORN_NOTE_PUZZLE, PD_SAVE_HOLE_PUNCH_PUZZLE, PD_SAVE_LABYRINTH_PUZZLE, PD_SAVE_DAGGER_PUZZLE, PD_SAVE_TORN_PHOTO_PUZZLE, PD_SAVE_LAPTOP_PUZZLE };

CPDDragDropPuzzleModule::CPDDragDropPuzzleModule(int puzzleIndex) : CFullScreenModule(ModuleType::DragDropPuzzle)
{
	// 00 = Pandora puzzle			PANDORA.AP, 14-15			2A9191		Needs different initialization code
	// 01 = Mayan figures puzzle	SPECIAL.AP, 11-13			2A922D		10th image is exit button
	// 02 = Torn note				SPECIAL.AP	14-16			2A9263
	// 03 = Hole punch				SPECIAL.AP	17-19			2A933B
	// 04 = Mayan labyrinth			SPECIAL.AP	20-22			2A93CB
	// 05 = Dagger puzzle			SPECIAL.AP	26-29			2A941F
	// 06 = Torn photo				SPECIAL.AP	67-69			2A943D
	// 07 = Laptop with Cabin CD	SPECIAL.AP	74-76			2A94A9

	_puzzleIndex = puzzleIndex;

	_hasBonusScore = FALSE;
	_scoreToAdd = 0;
	_bonusScore = 0;
	_bonusDropSpeed = 0;
	_timeOrMoves = 0;
	_timeOrFreeMoves = 0;

	_numberOfPieces = 0;
	_positionOffset = 0;
	_imageOffset = 0;

	_completed = FALSE;

	_selectedPiece = NULL;

	_cheated = FALSE;
}

CPDDragDropPuzzleModule::~CPDDragDropPuzzleModule()
{
}

void CPDDragDropPuzzleModule::Render()
{
	// TODO: Update time/moves left, bonus score etc

	RenderScreen();

	CPuzzlePiece::Render();

	if (_inputEnabled)
	{
		CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
		CModuleController::Cursors[0].Render();
	}
}

void CPDDragDropPuzzleModule::Initialize()
{
	CFullScreenModule::Initialize();

	CPuzzlePiece::Reset();

	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	// Check Z-order of first 2 pieces, if they're both 0 then initial position data has not been loaded
	if (CGameController::GetData(PD_SAVE_DRAG_DROP_PUZZLES + 4) == 0 && CGameController::GetData(PD_SAVE_DRAG_DROP_PUZZLES + 10) == 0)
	{
		// Load initial puzzle positions from PANDORA.AP.16
		BinaryData bd = LoadEntry(L"PANDORA.AP", 16);
		CGameController::Copy(bd.Data, PD_SAVE_DRAG_DROP_PUZZLES, bd.Length);
	}

	_positionOffset = PuzzleDataOffsets[_puzzleIndex];

	int imageEntryOffset = 2;
	if (_puzzleIndex == 0)
	{
		// Palette and screen from combined item
		BinaryData bd = LoadEntry(FileNames[PuzzleFiles[_puzzleIndex]], PuzzleEntries[_puzzleIndex]);
		ReadPalette(bd.Data);
		BinaryData bdImg = CLZ::Decompress(bd.Data + 0x300, bd.Length - 0x300);
		_screen = bdImg.Data;
		delete[] bd.Data;

		imageEntryOffset = 1;
	}
	else
	{
		// Palette and screen from paired items
		DoubleData dd = LoadDoubleEntry(FileNames[PuzzleFiles[_puzzleIndex]], PuzzleEntries[_puzzleIndex]);
		ReadPalette(dd.File1.Data);
		delete[] dd.File1.Data;
		_screen = dd.File2.Data;
	}

	BinaryData bd = LoadEntry(FileNames[PuzzleFiles[_puzzleIndex]], PuzzleEntries[_puzzleIndex] + imageEntryOffset);
	if (bd.Data != NULL)
	{
		_data = bd.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			LPBYTE pImage = _data + GetInt(_data, 2 + i * 4, 4);
			_files[i] = pImage;
			CPuzzlePiece::Add(i, pImage, _scale, _palette, count, _left, _top, _positionOffset);
		}
	}

	_pdRawFont.Init(IDR_RAWFONT_PD);

	if (_puzzleIndex == 7)
	{
		std::unordered_map<int, int> colourMap;
		colourMap[2] = 13;
		int textWidth = _pdRawFont.Measure("Exit", -1);
		_pdRawFont.Render(_screen, 640, 480, (423 - textWidth) / 2, 253, "Exit", colourMap, -1);
	}

	CPuzzlePiece::Sort();

	UpdateTexture();
}

void CPDDragDropPuzzleModule::RenderScreen()
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

		dx.EnableZBuffer();
	}
}

void CPDDragDropPuzzleModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	if (!_completed && _selectedPiece != NULL)
	{
		// Move selected piece
		_selectedPiece->X -= _pt.x - _cursorPosX;
		_selectedPiece->Y -= _pt.y - _cursorPosY;
		_pt.x = static_cast<LONG>(_cursorPosX);
		_pt.y = static_cast<LONG>(_cursorPosY);
	}
}

void CPDDragDropPuzzleModule::BeginAction()
{
	if (_inputEnabled && !_completed)
	{
		CPuzzlePiece* pScrap = CPuzzlePiece::HitTest(_cursorPosX, _cursorPosY, _imageOffset, _scale);
		if (pScrap != NULL)
		{
			_selectedPiece = pScrap;
			_pt.x = static_cast<LONG>(_cursorPosX);
			_pt.y = static_cast<LONG>(_cursorPosY);
			return;
		}

		int x = static_cast<int>((_cursorPosX - _left) / _scale);
		int y = static_cast<int>((_cursorPosY - _top) / _scale);

		switch (_puzzleIndex)
		{
			case 0:
			{
				break;
			}
			case 1:
			{
				break;
			}
			case 2:
			{
				break;
			}
			case 3:
			{
				break;
			}
			case 4:
			{
				break;
			}
			case 5:
			{
				break;
			}
			case 6:
			{
				break;
			}
			case 7:
			{
				if (x >= 186 && x < 237 && y >= 247 && y < 270)
				{
					// Exit button pressed
					Back();
				}
				break;
			}
		}
	}
}

void CPDDragDropPuzzleModule::Back()
{
	if (_inputEnabled)
	{
		CModuleController::Pop(this);
	}
}

BOOL CPDDragDropPuzzleModule::CheckCompleted()
{
	switch (_puzzleIndex)
	{
		case 0:
		{
			return CheckPandoraPuzzleCompleted();
			break;
		}
		case 1:
		{
			return CheckFiguresPuzzleCompleted();
			break;
		}
		case 2:
		{
			return CheckTornNotePuzzleCompleted();
			break;
		}
		case 3:
		{
			return CheckHolePunchPuzzleCompleted();
			break;
		}
		case 4:
		{
			return CheckLabyrinthPuzzleCompleted();
			break;
		}
		case 5:
		{
			return CheckDaggerPuzzleCompleted();
			break;
		}
		case 6:
		{
			return CheckTornPhotoPuzzleCompleted();
			break;
		}
		case 7:
		{
			return CheckLaptopPuzzleCompleted();
			break;
		}
	}

	return FALSE;
}

void CPDDragDropPuzzleModule::EndAction()
{
	if (_selectedPiece != NULL && !_completed)
	{
		// Update save area
		int x = static_cast<int>((_selectedPiece->X - _left) / _scale);
		int y = static_cast<int>((_selectedPiece->Y - _top) / _scale);

		if (_puzzleIndex == 7)
		{
			// Laptop puzzle, snap to inside grid, if close enough
			for (int cx = 121; cx < 304; cx += 46)
			{
				for (int cy = 53; cy < 236; cy += 46)
				{
					int dx = cx - x;
					int dy = cy - y;
					if ((dx * dx + dy * dy) < 100)
					{
						x = cx;
						y = cy;
						_selectedPiece->X = _left + x * _scale;
						_selectedPiece->Y = _top + y * _scale;
					}
				}
			}
		}

		CGameController::SetWord(_selectedPiece->Offset, x);
		CGameController::SetWord(_selectedPiece->Offset + 2, y);
		CGameController::SetData(_selectedPiece->Offset + 4, _selectedPiece->Z);
		CGameController::SetData(_selectedPiece->Offset + 5, _selectedPiece->Orientation);

		_selectedPiece = NULL;

		// Check if note is now assembled
		if (_completed = CheckCompleted())
		{
		}
	}
}

void CPDDragDropPuzzleModule::Cycle()
{
	if (!_completed && _puzzleIndex != 5 && _puzzleIndex != 7)
	{
		if (_selectedPiece != NULL)
		{
			_selectedPiece->Orientation = (_selectedPiece->Orientation + 1) & 3;
		}
		else
		{
			CPuzzlePiece* pScrap = CPuzzlePiece::HitTest(_cursorPosX, _cursorPosY, _imageOffset, _scale);
			if (pScrap != NULL)
			{
				// Right button, rotate selected piece
				pScrap->Orientation = (pScrap->Orientation + 1) & 3;
			}
		}
	}
}

void CPDDragDropPuzzleModule::Next()
{
	if (_selectedPiece != NULL && !_completed && _puzzleIndex != 5 && _puzzleIndex != 7)
	{
		_selectedPiece->Orientation = (_selectedPiece->Orientation + 1) & 3;
	}
}

void CPDDragDropPuzzleModule::Prev()
{
	if (_selectedPiece != NULL && !_completed && _puzzleIndex != 5 && _puzzleIndex != 7)
	{
		_selectedPiece->Orientation = (_selectedPiece->Orientation - 1) & 3;
	}
}

BOOL CPDDragDropPuzzleModule::CheckPandoraPuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckFiguresPuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckTornNotePuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckHolePunchPuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckLabyrinthPuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckDaggerPuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckTornPhotoPuzzleCompleted()
{
	return FALSE;
}

BOOL CPDDragDropPuzzleModule::CheckLaptopPuzzleCompleted()
{
	BYTE grid[16];
	ZeroMemory(grid, 16);

	for (int p = 0; p < 16; p++)
	{
		CPuzzlePiece* pPiece = CPuzzlePiece::Images[p];
		float x = (((pPiece->X - _left) / _scale) - 121) / 46.0f;
		float y = (((pPiece->Y - _top) / _scale) - 53) / 46.0f;

		if (floor(x) == x && floor(y) == y && x < 4 && y < 4)
		{
			// Piece dropped in a cell
			grid[(int)y * 4 + (int)x] = (p < 4) ? 1 : 2 + (p - 4) / 3;
		}
		else
		{
			return FALSE;
		}
	}

	for (int i = 0; i < 16; i++)
	{
		if (grid[i] == 0)
		{
			// All cells must be filled
			return FALSE;
		}
	}

	if (((grid[1] | grid[7] | grid[8] | grid[14]) == 1
		&& grid[0] == grid[6] && grid[0] == grid[13]
		&& grid[2] == grid[9] && grid[2] == grid[15]
		&& grid[3] == grid[4] && grid[3] == grid[10])

		|| ((grid[2] | grid[4] | grid[11] | grid[13]) == 1
		&& grid[0] == grid[7] && grid[0] == grid[9]
		&& grid[1] == grid[10] && grid[1] == grid[12]
		&& grid[3] == grid[5] && grid[3] == grid[14]))
	{
		// TODO: Render correct colours into screen cells
		std::unordered_map<int, int> colourMap;
		colourMap[1] = 0xe;
		colourMap[2] = 0xb6;
		colourMap[3] = 0x53;
		colourMap[4] = 0x37;
		colourMap[5] = 0xb;

		int sy = 53;
		for (int y = 0; y < 4; y++)
		{
			int sx = 121;
			for (int x = 0; x < 4; x++)
			{
				int cellValue = grid[y * 4 + x];
				Fill(sx, sy, sx + 44, sy + 44, colourMap[cellValue]);

				sx += 46;
			}

			sy += 46;
		}

		CGameController::SetHintState(859, 1, _cheated ? 0 : 1);

		CGameController::SetParameter(205, 2);
		CModuleController::Push(new CPDLaptopModule(_screen, _palette));
		_screen = NULL;// Sending screen to laptop module for re-use
		CModuleController::Pop(this);

		return TRUE;
	}

	return FALSE;
}
