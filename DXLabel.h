#pragma once
#include "DXControl.h"
#include "DXText.h"

class CDXLabel : public CDXControl
{
public:
	CDXLabel(char* text);
	~CDXLabel();

	virtual void Render();

	void SetText(char* text);

private:
	CDXText* _text;
};
