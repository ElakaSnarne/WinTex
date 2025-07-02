//#include "VideoPlayer.h"
#include "AnimationController.h"
#include "Globals.h"
#include "PTF.h"
#include "BIC.h"
#include "SilentBIC.h"
#include "Wave.h"
#include "Utilities.h"
#include "Image.h"
#include "MediaIdentifiers.h"

CAnimBase* CAnimationController::_anim = NULL;
CDXText* CAnimationController::_pCaption = NULL;

int CAnimationController::_texCaptionColour1 = 0xff000000;
int CAnimationController::_texCaptionColour2 = 0xff00c300;
int CAnimationController::_texCaptionColour3 = 0xff24ff00;
int CAnimationController::_texCaptionColour4 = 0xff000000;
int CAnimationController::_otherCaptionColour1 = 0xff000000;
int CAnimationController::_otherCaptionColour2 = 0xff0096ff;
int CAnimationController::_otherCaptionColour3 = 0xff00cfff;
int CAnimationController::_otherCaptionColour4 = 0xff000000;

void CAnimationController::Init()
{
	_pCaption = new CDXText();
}

void CAnimationController::Clear()
{
	if (_anim != NULL)
	{
		delete _anim;
		_anim = NULL;
	}

	if (_pCaption != NULL)
	{
		_pCaption->SetText("");
	}
}

BOOL CAnimationController::Load(LPCWSTR fileName, int itemIndex)
{
	Clear();

	// TODO: Check if a video file replacement exists
	/*
	if (pConfig->AlternativeMedia)
	{
		wchar_t alternateName[1024];
		ZeroMemory(alternateName, sizeof(alternateName));
		wcscat(alternateName, fileName);
		wchar_t* pAlter = alternateName + wcslen(fileName) - 3;
		*pAlter = L'\\';
		pAlter++;

		pAlter = _itow(itemIndex, pAlter, 10) + wcslen(pAlter);
		*pAlter = L'.';
		pAlter++;
		*pAlter = L'm';
		pAlter++;
		*pAlter = L'k';
		pAlter++;
		*pAlter = L'v';
		pAlter++;

		if (CFile::Exists(alternateName))
		{
			//(*tit)->pTexture->Init(alternateName);

			// TODO: Try to load external video file

			CVideoPlayer* pVid = new CVideoPlayer();
			pVid->Init(_hWnd, alternateName);
			_anim = (CAnimBase*)pVid;

			return TRUE;
		}
	}*/

	// Read entry
	BinaryData bd = LoadEntry(fileName, itemIndex);
	if (bd.Data != NULL && bd.Length > 0)
	{
		_anim = Load(bd);
		return (_anim != NULL);
	}
	else
	{
		std::wstring err = L"Failed to open file ";
		err += fileName;
		MessageBox(NULL, err.c_str(), L"Load AP Entry", MB_OK);
	}

	return FALSE;
}

BOOL CAnimationController::Skip()
{
	BOOL skip = FALSE;
	if (_anim != NULL && !_anim->IsDone())
	{
		_anim->Skip();
		skip = TRUE;
	}

	return skip;
}

BOOL CAnimationController::IsWave()
{
	BOOL wave = (_anim != NULL && _anim->IsWave());
	return wave;
}

BOOL CAnimationController::HasAnim()
{
	BOOL anim = (_anim != NULL && !_anim->IsDone());
	return anim;
}

BOOL CAnimationController::NoAnimOrWave()
{
	BOOL naow = (_anim == NULL || _anim->IsDone() || _anim->IsWave());
	return naow;
}

BOOL CAnimationController::UpdateAndRender(BOOL render)
{
	return UpdateAndRender(_anim, render);
}

BOOL CAnimationController::UpdateAndRender(CAnimBase* pAnim, BOOL render)
{
	BOOL updated = FALSE;

	if (pAnim != NULL)
	{
		int frame = Frame(pAnim);
		CCaption* pC = GetFrameCaption(frame);
		if (pC != NULL && !pC->Processed())
		{
			_pCaption->SetText(pC->Text(), CDXText::Alignment::Justify);
			if (pC->Tex())
			{
				_pCaption->SetColours(_texCaptionColour1, _texCaptionColour2, _texCaptionColour3, _texCaptionColour4);
			}
			else
			{
				_pCaption->SetColours(_otherCaptionColour1, _otherCaptionColour2, _otherCaptionColour3, _otherCaptionColour4);
			}

			pC->SetProcessed(TRUE);
		}

		updated = pAnim->Update();
		if (render)
		{
			pAnim->Render();
		}

		if (pConfig->Captions && !pAnim->IsDone())
		{
			_pCaption->Render(0.0f, dx.GetHeight() - _pCaption->Height() - 10.0f, -1.0f);
		}
	}

	return updated;
}

BOOL CAnimationController::IsDone()
{
	BOOL done = (_anim != NULL && _anim->IsDone());
	return done;
}

BOOL CAnimationController::AnimNotDoneOrCondition(BOOL condition)
{
	BOOL done = (_anim != NULL && (!_anim->IsDone() || condition));
	return done;
}

BOOL CAnimationController::NoVideoAnim()
{
	BOOL noVideo = (_anim == NULL || !_anim->HasVideo() || _anim->IsDone());
	return noVideo;
}

int CAnimationController::Frame()
{
	return Frame(_anim);
}

int CAnimationController::Frame(CAnimBase* pAnim)
{
	int frame = pAnim->Frame();
	return frame;
}

CAnimBase* CAnimationController::Load(BinaryData bd, int factor)
{
	// TODO: Determine type (unless predetermined)
	// TODO: Create and return correct class
	CAnimBase* pAnim = NULL;

	if (GetInt(bd.Data, 4, 4) == PTF)
	{
		pAnim = new CPTF(factor);
	}
	else if (GetInt(bd.Data, 0, 4) == BIC)
	{
		pAnim = new CBIC(factor);
	}
	else if (GetInt(bd.Data, 0, 4) == RIFF)
	{
		pAnim = new CWave();
	}
	else if ((GetInt(bd.Data, 0, 4) + 0x30c) == bd.Length)
	{
		// Assuming silent BIC
		pAnim = new CSilentBIC(factor);
	}
	else if (GetInt(bd.Data, 0, 4) == H2O)
	{
		pAnim = new CH2O(factor);
	}
	//else if... Embedded palette + picture

	if (pAnim != NULL) pAnim->Init(bd);

	return pAnim;
}

CImage* CAnimationController::LoadImage(DoubleData dd, int width, int height, int factor)
{
	return new CImage(dd, width, height, factor);
}

CImage* CAnimationController::LoadImage(LPBYTE palette, BinaryData image, int width, int height, int factor)
{
	return new CImage(palette, image, width, height, factor);
}

void CAnimationController::Resize(int width, int height)
{
	if (_anim != NULL)
	{
		_anim->Resize(width, height);
	}
}

void CAnimationController::SetCaptionColours(int texColour1, int texColour2, int texColour3, int texColour4, int otherColour1, int otherColour2, int otherColour3, int otherColour4)
{
	_texCaptionColour1 = texColour1;
	_texCaptionColour2 = texColour2;
	_texCaptionColour3 = texColour3;
	_texCaptionColour4 = texColour4;
	_otherCaptionColour1 = otherColour1;
	_otherCaptionColour2 = otherColour2;
	_otherCaptionColour3 = otherColour3;
	_otherCaptionColour4 = otherColour4;
}

void CAnimationController::SetOutputBuffer(LPBYTE pBuffer, int width, int height, int offsetX, int offsetY, LPINT pPalette, int minColAllowChange, int maxColAllowChange)
{
	// Only supported by H2O
	((CH2O*)_anim)->SetOutputBuffer(pBuffer, width, height, offsetX, offsetY, pPalette, minColAllowChange, maxColAllowChange);
}

void CAnimationController::RenderCaptions(float z)
{
	if (pConfig->Captions && _anim != NULL && !_anim->IsDone())
	{
		_pCaption->Render(0.0f, dx.GetHeight() - _pCaption->Height() - 10.0f, z);
	}
}
