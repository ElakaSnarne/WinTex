#pragma once

#include "FullScreenModule.h"
#include <unordered_map>
#include "D3D11-NoWarn.h"
#include "Texture.h"
#include "AreaData.h"

#define CL_ANIMATION_FRAMES	36

class CUAKMCrimeLinkModule : public CFullScreenModule
{
public:
	CUAKMCrimeLinkModule(int parameter);
	virtual ~CUAKMCrimeLinkModule();

	virtual void Render();
	virtual void Pause();

protected:
	virtual void Initialize();

	static short CorrectSelections[15];
	static short PlayerSelections[15];
	static short Animation[CL_ANIMATION_FRAMES * 4];

	ULONGLONG _animationFrameTime;
	ULONGLONG _animationFrameDuration;
	int _animationIndex;
	int _windowX;
	int _windowY;
	int _flashFrame;

	void SetCursorArea(int x1, int y1, int x2, int y2);

	int _page;
	CAreaData _areaData;
	int _pageAnswerOffset;

	void PartialRender(int entry, int offsetX, int offsetY, BOOL updateTexture);

	POINT GetMouseOver();
	POINT _lastMouseOver;
	void DrawRectangle(int entry, BYTE colour, BOOL isCategory);
	void ReplaceColour(BOOL isCategory, int entry, BYTE src, BYTE dst);

	void ShowPage1();
	void ShowPage2();
	void ShowPage3();
	void ShowPage4();
	void ShowPage5();
	void ShowPage6();
	void ShowPage7();
	void ShowPage8();

	BOOL CheckSelection();
	void Click(int entry, int category);

	void UpdateSelection();
	int GetAnswerIndex(int category, int answer);

	int _parameter;

	void Render1(int x, int y);
	void Render0(int x, int y);
	void RenderPlus(int x, int y);

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
};
