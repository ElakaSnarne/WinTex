#pragma once
#include "DXControl.h"
#include <list>
#include "DXFont.h"
#include "Globals.h"
#include "DXControls.h"
#include "DXContainer.h"

class CDXScreen : public CDXContainer
{
	friend class CDXControl;

public:
	CDXScreen();
	~CDXScreen();

	static void Init();
	static void Dispose();

	void ClearMouseOver();

	static XMMATRIX WorldMatrix;
	static XMMATRIX ViewMatrix;
	static XMMATRIX ProjectionMatrix;
	static XMMATRIX OrthoMatrix;

	static XMVECTOR UpVector;
	static XMVECTOR PositionVector;
	static XMVECTOR LookAtVector;

	static float FieldOfView;
	static float ScreenAspect;
};
