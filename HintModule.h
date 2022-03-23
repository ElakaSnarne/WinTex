#pragma once

#include "ModuleBase.h"
#include "HintCategory.h"
#include "Texture.h"
#include "DXButton.h"

class CHintModule : public CModuleBase
{
public:
	CHintModule();
	~CHintModule();

	virtual void Resize(int width, int height);
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();

protected:
	std::list<CHintCategory*> _activeHintCategories;

	CTexture _blankTexture;
	CTexture _dotTexture;
	CTexture _checkTexture;
	CTexture _questionmarkTexture;

	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _hintVertexBuffer;
	ID3D11Buffer* _categoryVertexBuffer;

	CHintCategory* _pCurrentHintCategory;

	int _colBlack;
	int _colBlue;
	int _colCategory;
	int _colOrange;
	int _colGreen;
	int _colScore;
	int _colHighlight;
	int _colShade;

	CDXButton* _pBtnResume;
	CDXButton* _pBtnDirectory;

	CTexture _hintTexture;
	CTexture _categoryTexture;
};
