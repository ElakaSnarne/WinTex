#pragma once

#include "FullScreenModule.h"
#include "RawFont.h"
#include "PuzzlePiece.h"

class CPDDragDropPuzzleModule : public CFullScreenModule
{
public:
	CPDDragDropPuzzleModule(int puzzleIndex);
	~CPDDragDropPuzzleModule();

	virtual void Render();

protected:
	virtual void Initialize();

	CRawFont _pdRawFont;

	void RenderScreen();

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void EndAction();
	virtual void Back();
	virtual void Cycle();
	virtual void Next();
	virtual void Prev();

	int _puzzleIndex;

	BOOL _hasBonusScore;
	int _scoreToAdd;
	int _bonusScore;
	int _bonusDropSpeed;
	int _timeOrMoves;
	int _timeOrFreeMoves;

	static WCHAR* FileNames[];
	static int PuzzleFiles[];
	static int PuzzleEntries[];
	static int PuzzlePiecesCount[];
	static int PuzzleDataOffsets[];

	int _numberOfPieces;
	int _positionOffset;
	int _imageOffset;

	CPuzzlePiece* _selectedPiece;
	POINT _pt;

	BOOL _completed;
	BOOL CheckCompleted();
	BOOL CheckPandoraPuzzleCompleted();
	BOOL CheckFiguresPuzzleCompleted();
	BOOL CheckTornNotePuzzleCompleted();
	BOOL CheckHolePunchPuzzleCompleted();
	BOOL CheckLabyrinthPuzzleCompleted();
	BOOL CheckDaggerPuzzleCompleted();
	BOOL CheckTornPhotoPuzzleCompleted();
	BOOL CheckLaptopPuzzleCompleted();

	BOOL _cheated;
};
