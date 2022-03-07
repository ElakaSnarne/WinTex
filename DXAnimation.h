#pragma once
#include "DXControl.h"
#include "AnimBase.h"

class CDXAnimation : public CDXControl, public CAnimBase
{
public:
	CDXAnimation();
	~CDXAnimation();

	void Render();
};
