#pragma once

#include "ModuleBase.h"
#include <unordered_map>
#include <d3d11.h>
#include "Texture.h"
#include "AmbientAudio.h"

class CUAKMSafeModule : public CModuleBase
{
public:
	CUAKMSafeModule(int parameter, BOOL alternatePalette);
	virtual ~CUAKMSafeModule();

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();
	virtual void KeyDown(WPARAM key, LPARAM lParam);

protected:
	virtual void Initialize();

	int _parameter;
	BOOL _alternatePalette;

	LPBYTE _screen;

	void SetCursorArea(int x1, int y1, int x2, int y2);
	void ClipMouse(BOOL move);
	float _left;
	float _top;
	float _right;
	float _bottom;
	float _scale;

	int _palette[256];
	std::unordered_map<int, LPBYTE> _safeImageOffsets;
	std::unordered_map<int, LPBYTE> _safeSoundOffsets;

	LPBYTE _pImages;
	LPBYTE _pSounds;

	ID3D11Buffer* _vertexBuffer;
	CTexture _texture;

	ID3D11Buffer* _handVertexBuffer;
	CTexture _handTexture;

	void UpdateTexture();
	void PartialRender(int entry, int offsetX, int offsetY, BOOL updateTexture);

	BOOL _ready;
	DWORD _frameDelay;
	ULONGLONG _frameTime;
	int _startupFrame;
	int _keyDown;
	BYTE _enteredCode[8];
	int _keyPos;

	void Start();
	void Enter();
	void Number(int number);
	void Exit();

	CAmbientAudio _sound;

	void Press(int key, int sound);
	BOOL _codeCorrect;

	BOOL _flashingLightOn;
	int _rollingLightPosition;
	ULONGLONG _rollingLightTime;

	int _openSafeSequence;

	// Input related
	virtual void BeginAction();
	virtual void Back();
};
