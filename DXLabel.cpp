#include "DXLabel.h"
#include "DXScreen.h"

CDXLabel::CDXLabel(char* text, CDXText::Alignment alignment)
{
	_text = new CDXText();
	_text->SetText(text, alignment);
	_text->SetColours(0xffffffff);

	_type = ControlType::Label;
}

CDXLabel::CDXLabel(char* text, Rect rect, CDXText::Alignment alignment)
{
	_text = new CDXText();
	_text->SetText(text, rect, alignment);
	_text->SetColours(0xffffffff);

	_type = ControlType::Label;
}

CDXLabel::~CDXLabel()
{
	if (_text != NULL)
	{
		delete _text;
		_text = NULL;
	}
}

void CDXLabel::Render()
{
	// Draw text
	_text->Render(_x, _y);
}

void CDXLabel::SetText(const char* text)
{
	_text->SetText(text);
}

void CDXLabel::SetColours(int colour1, int colour2, int colour3, int colour4)
{
	_text->SetColours(colour1, colour2, colour3, colour4);
}
