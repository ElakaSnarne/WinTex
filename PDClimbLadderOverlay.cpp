#include "PDClimbLadderOverlay.h"
#include "resource.h"
#include "Utilities.h"

CPDClimbLadderOverlay::CPDClimbLadderOverlay()
{
	_pFrame = NULL;
	_pLine = NULL;
	_pBtnYes = NULL;
	_pBtnNo = NULL;
}

CPDClimbLadderOverlay::~CPDClimbLadderOverlay()
{
}

void CPDClimbLadderOverlay::Render()
{
	if (_pFrame == NULL)
	{
		Initialize();
	}

	_pFrame->Render();

	CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Arrow].SetPosition(_x, _y);
	CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Arrow].Render();
}

void CPDClimbLadderOverlay::BeginAction()
{
	// Check if a button has been hit
	if (_pBtnYes->HitTest(_x, _y) != NULL)
	{
		return Yes();
	}
	else if (_pBtnNo->HitTest(_x, _y) != NULL)
	{
		return No();
	}
}

void CPDClimbLadderOverlay::KeyDown(WPARAM key, LPARAM lParam)
{
	if (key == 'Y')
	{
		return Yes();
	}
	else if (key == 'N')
	{
		return No();
	}
}

void CPDClimbLadderOverlay::Yes()
{
	_decision = 1;
}

void CPDClimbLadderOverlay::No()
{
	_decision = -1;
}

void CPDClimbLadderOverlay::Initialize()
{
	int w = dx.GetWidth();
	int h = dx.GetHeight();

	_x = w / 2.0f;
	_y = h / 2.0f;

	char* pY = "Yes";
	char* pN = "No";
	char* pL = "Do you want to climb this ladder?";
	char* pH = "Ladder Check";
	float maxbtnw = max(TexFont.PixelWidth(pY), TexFont.PixelWidth(pN));
	float labelw = TexFont.PixelWidth(pL);
	float hw = max(TexFont.PixelWidth(pH), labelw);
	float lineHeight = TexFont.Height() * pConfig->FontScale;
	float fw = hw + 16.0f * pConfig->FontScale;
	float fh = 8.5f * lineHeight;

	_pFrame = new CDXFrame(pH, fw, fh);

	_pLine = new CDXLabel(pL, { 0,0,0,labelw }, CDXText::Alignment::JustifyAlways);
	_pLine->SetColours(0, 0, 0xff22ff00, 0);
	_pFrame->SetColours(0, 0, 0xffdb9a69, 0);

	float fx = (w - fw) / 2.0f;
	float fy = (h - fh) / 2.0f;

	_pFrame->AddChild(_pLine, fx + 8.0f, fy + lineHeight * 2);
	_pBtnYes = _pFrame->AddButton(pY, fx + 8.0f, fy + lineHeight * 4.5f, maxbtnw, 20.0f, NULL);
	_pBtnNo = _pFrame->AddButton(pN, fx + fw - maxbtnw - 32.0f * pConfig->FontScale - 8.0f, fy + lineHeight * 4.5f, maxbtnw, 20.0f, NULL);
	_pFrame->SetPosition(fx, fy);

	_hitTestControls.push_back(_pBtnYes);
	_hitTestControls.push_back(_pBtnNo);
}
