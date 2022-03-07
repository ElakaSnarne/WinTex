#include "DXControls.h"
#include "DXScreen.h"
#include "DXFrame.h"
#include "DXImageButton.h"
#include "DXCheckBox.h"
#include "DXDialogueOption.h"
#include "SaveGameControl.h"
#include "DXTabItem.h"
#include "DXSlider.h"

void CDXControls::Init()
{
	// Initialize control classes
	CDXScreen::Init();
	CDXButton::Init();
	CDXFrame::Init();
	CDXImageButton::Init();
	CDXCheckBox::Init();
	CDXDialogueOption::Init();
	CDXSound::Init();
	CDXListBox::Init();
	CDXTabItem::Init();
	CDXSlider::Init();

	CSaveGameControl::Init();
}

void CDXControls::Dispose()
{
	// Initialize control classes
	CDXScreen::Dispose();
	CDXButton::Dispose();
	CDXFrame::Dispose();
	CDXImageButton::Dispose();
	CDXCheckBox::Dispose();
	CDXDialogueOption::Dispose();
	CDXSound::Dispose();
	CDXListBox::Dispose();
	CDXTabItem::Dispose();
	CDXSlider::Dispose();

	CSaveGameControl::Dispose();
}
