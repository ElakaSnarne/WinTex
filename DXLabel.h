#pragma once
#include "DXControl.h"
#include "DXText.h"

class CDXLabel : public CDXControl
{
public:
	CDXLabel(char* text, CDXText::Alignment alignment = CDXText::Alignment::Left);
	CDXLabel(char* text, Rect rect, CDXText::Alignment alignment = CDXText::Alignment::Left);
	~CDXLabel();

	virtual void Render();

	void SetText(const char* text);
	void SetColours(int colour1, int colour2, int colour3, int colour4);

private:
	CDXText* _text;
};
