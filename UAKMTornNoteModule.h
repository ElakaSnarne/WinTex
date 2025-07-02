#pragma once

#include "ModuleBase.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"
#include <vector>
#include "DXText.h"
#include "DXButton.h"
#include "PuzzlePiece.h"

class CUAKMTornNoteModule : public CModuleBase
{
public:
	CUAKMTornNoteModule(int item);
	virtual ~CUAKMTornNoteModule();

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();

	CDXButton* _pBtnResume;
	static void OnResume(LPVOID data);

protected:
	virtual void Initialize();

	static CUAKMTornNoteModule* pUAKMTNM;

	int _item;
	int _newItem;
	int _palette[256];

	ID3D11Buffer* _vertexBuffer;

	int _numberOfImages;
	LPBYTE _pImageData;

	int _positionOffset;
	float _scale;
	float _screenHeight;

	CPuzzlePiece* _selectedScrap;
	POINT _pt;

	BOOL _completed;
	BOOL CheckCompleted();

	ULONGLONG _timeToExit;

	CDXText _caption;

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void EndAction();
	virtual void Back();
	virtual void Cycle();
	virtual void Next();
	virtual void Prev();
};
