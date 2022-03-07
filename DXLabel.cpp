#include "DXLabel.h"
#include "DXScreen.h"

CDXLabel::CDXLabel(char* text)
{
	_text = new CDXText();
	_text->SetText(text);
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

void CDXLabel::SetText(char* text)
{
	_text->SetText(text);
}
