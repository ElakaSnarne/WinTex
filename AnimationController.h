#pragma once

#include "AnimBase.h"
#include "Mutex.h"
#include "DXText.h"
#include "Image.h"

class CAnimationController
{
public:
	static void Init();
	static void Clear();
	static BOOL Load(LPCWSTR fileName, int itemIndex);
	static BOOL Skip();
	static BOOL IsWave();
	static BOOL HasAnim();
	static BOOL NoAnimOrWave();
	static BOOL UpdateAndRender(BOOL render = TRUE);
	static BOOL UpdateAndRender(CAnimBase* pAnim, BOOL render = TRUE);
	static BOOL IsDone();
	static BOOL AnimNotDoneOrCondition(BOOL condition);
	static BOOL NoVideoAnim();
	static int Frame();
	static int Frame(CAnimBase* pAnim);
	static int Exists() { return (_anim != NULL); };
	static void Resize(int width, int height);
	static void SetOutputBuffer(LPBYTE pBuffer, int width, int height, int offsetX, int offsetY, LPINT pPalette, int minColAllowChange, int maxColAllowChange);

	static CAnimBase* Load(BinaryData bd, int factor = 1);
	static CImage* LoadImage(DoubleData bd, int width, int height, int factor = 1);
	static CImage* LoadImage(LPBYTE palette, BinaryData image, int width, int height, int factor = 1);

	static int Width() { return (_anim != NULL) ? _anim->Width() : 0; }
	static int Height() { return (_anim != NULL) ? _anim->Height() : 0; }

	static void SetCaptionColours(int texColour1, int texColour2, int texColour3, int texColour4, int otherColour1, int otherColour2, int otherColour3, int otherColour4);

	static void RenderCaptions(float z);

protected:
	static CAnimBase* _anim;
	static CDXText* _pCaption;

	static int _texCaptionColour1;
	static int _texCaptionColour2;
	static int _texCaptionColour3;
	static int _texCaptionColour4;
	static int _otherCaptionColour1;
	static int _otherCaptionColour2;
	static int _otherCaptionColour3;
	static int _otherCaptionColour4;
};
