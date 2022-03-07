#include "DXScreen.h"
#include "Globals.h"
#include "DXBitmap.h"
#include "DXButton.h"
#include "resource.h"
#include "Utilities.h"

XMMATRIX CDXScreen::WorldMatrix;
XMMATRIX CDXScreen::ViewMatrix;
XMMATRIX CDXScreen::ProjectionMatrix;
XMMATRIX CDXScreen::OrthoMatrix;

XMVECTOR CDXScreen::UpVector;
XMVECTOR CDXScreen::PositionVector;
XMVECTOR CDXScreen::LookAtVector;

float CDXScreen::FieldOfView;
float CDXScreen::ScreenAspect;

CDXScreen::CDXScreen()
{
}

CDXScreen::~CDXScreen()
{
}

void CDXScreen::Init()
{
	DWORD size;
	PBYTE pFont = GetResource(isUAKM ? IDB_TEXFONT : IDB_PANDORAFONT, L"PNG", &size);
	TexFont.Init(pFont, size);

	UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	float x = (float)dx.GetWidth() / 2.0f;
	float y = (float)dx.GetHeight() / 2.0f;
	PositionVector = XMVectorSet(x, y, -10.0f, 0.0f);
	LookAtVector = XMVectorSet(x, y, 1.0f, 0.0f);

	ViewMatrix = XMMatrixLookAtLH(PositionVector, LookAtVector, UpVector);
	OrthoMatrix = XMMatrixOrthographicLH((float)dx.GetWidth(), (float)dx.GetHeight(), 0.1f, 1000.0f);

	FieldOfView = (float)XM_PI / 4.0f;
	ScreenAspect = (float)dx.GetWidth() / (float)dx.GetHeight();

	ProjectionMatrix = XMMatrixPerspectiveFovLH(FieldOfView, ScreenAspect, 0.1f, 1000.0f);
}

void CDXScreen::Dispose()
{
	TexFont.Dispose();
}

void CDXScreen::ClearMouseOver()
{
	std::list<CDXControl*>::iterator it = _childElements.begin();
	std::list<CDXControl*>::iterator end = _childElements.end();
	while (it != end)
	{
		(*it)->SetMouseOver(FALSE);
		it++;
	}
}
