#pragma once

#include "ModuleBase.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"
#include "DXButton.h"

class CUAKMEncodedMessageModule : public CModuleBase
{
public:
	CUAKMEncodedMessageModule();
	virtual ~CUAKMEncodedMessageModule();

	virtual void Resize(int width, int height) { }
	virtual void Dispose();
	virtual void Render();
	virtual void KeyDown(WPARAM key, LPARAM lParam);

	CDXButton* _pBtnResume;
	static void OnResume(LPVOID data);

protected:
	virtual void Initialize();

	static CUAKMEncodedMessageModule* pUAKMEMM;

	int _palette[256];

	ID3D11Buffer* _vertexBuffer;
	CTexture _texture;

	float _scale;
	float _left;
	float _top;
	float _width;
	float _height;

	void UpdateTexture();
	LPBYTE _screen;

	std::unordered_map<char, char> _codeMap;

	void RenderChar(int x, int y, char c, BOOL transparent);
	void RenderText(char* pText, int yOffset, BOOL transparent);

	LPBYTE _font;
	std::unordered_map<char, LPBYTE> _fontMap;

	int _col1;
	int _col2;
	int _col3;
	int _col4;

	char* _pSaveMsg;

	ID3D11Buffer* _indicatorVertexBuffer;
	float _indicatorX;
	float _indicatorY;

	int _charPos;

	bool _completed;
	bool CheckCompleted();

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();
};
