#pragma once
#include "DXControl.h"
#include "DXText.h"
#include "Texture.h"

class CDXCheckBox : public CDXControl
{
public:
	CDXCheckBox(char* text, BOOL* pValue, float width);
	~CDXCheckBox();

	static void Init();
	static void Dispose();

	virtual void Render();

	void SetCheck(BOOL check) { *_pChecked = check; }
	BOOL GetCheck() { return *_pChecked; }

	virtual void SetColours(int colour1, int colour2, int colour3, int colour4);

protected:
	static CTexture _cbTexBackground;
	static CTexture _cbTexMouseOver;
	static CTexture _cbTexChecked;
	CDXText* _pText;

	float _textX;
	float _textY;

	BOOL* _pChecked;
};
