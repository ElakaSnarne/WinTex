#include "DXControlButton.h"
#include "Globals.h"
#include "DXFont.h"
#include "Gamepad.h"

std::string CDXControlButton::GetMapText(CControllerData* pControllerData)
{
	switch (pControllerData->Source)
	{
		case InputSource::Key:
		{
			CHAR keyName[256];
			int charCount = GetKeyNameTextA(pControllerData->Offset, keyName, 255);
			return keyName;
			break;
		}
		case InputSource::Mouse:
		{
			return "Mouse";
			break;
		}
		case InputSource::MouseButton:
		{
			if (pControllerData->Data == -1)
			{
				return "Left mouse button";
			}
			else if (pControllerData->Data == 1)
			{
				return "Right mouse button";
			}
			break;
		}
		case InputSource::MouseWheel:
		{
			if ((int)pControllerData->Data > 0)
			{
				return "Mouse wheel up";
			}
			else if ((int)pControllerData->Data < 0)
			{
				return "Mouse wheel down";
			}
			break;
		}
		case InputSource::JoystickAxis:
		{
			// Lookup axis name
			return CGamepadController::GamepadController->GetName(pControllerData->Offset, pControllerData->Data);
			break;
		}
		case InputSource::JoystickButton:
		{
			// Lookup button name
			return CGamepadController::GamepadController->GetName(pControllerData->Offset, pControllerData->Data);
			break;
		}
		case InputSource::JoystickDPad:
		{
			// Lookup POV name
			return CGamepadController::GamepadController->GetName(pControllerData->Offset, pControllerData->Data);
			break;
		}
	}

	return "";
}

CDXControlButton::CDXControlButton(LPSTR function, std::unordered_map<InputAction, InputMap>* pMapping, BOOL isJoystick, float w, float h, float textX, void(*onClick)(InputAction), InputAction action)
{
	_isBeingConfigured = FALSE;
	_controlClicked = onClick;
	_action = action;
	_isJoystick = isJoystick;

	_pText = new CDXText();
	_pText->SetText(function);
	CControllerData cdata;
	InputMap map = (*pMapping)[action];
	if (isJoystick)
	{
		cdata.Source = map.JoystickSource;
		cdata.Offset = cdata.Data = map.JoystickIdentifier;
	}
	else
	{
		cdata.Source = map.MouseKeySource;
		cdata.Offset = cdata.Data = map.MouseKeyIdentifier;
	}
	_binding.SetText((LPSTR)GetMapText(&cdata).c_str());

	float textW = _binding.PixelWidth(function);

	_x = 0.0f;
	_y = 0.0f;
	_w = max(w, textW);
	_h = TexFont.Height() * pConfig->FontScale;

	_textX = textX;

	_type = ControlType::Control;
}

CDXControlButton::~CDXControlButton()
{
}

void CDXControlButton::Render()
{
	// Draw text
	int mask = _isBeingConfigured ? 0xff00ff00 : (_mouseOver && _enabled) ? 0xffffffff : 0x80ffffff;
	_pText->SetColours(Colour1&mask, Colour2 & mask, Colour3 & mask, Colour4 & mask);
	_binding.SetColours(Colour1 & mask, Colour2 & mask, Colour3 & mask, Colour4 & mask);

	float fontHeight = TexFont.Height();
	_pText->Render(_x, _y + fontHeight * pConfig->FontScale);
	_binding.Render(_textX, _y + fontHeight * pConfig->FontScale);
}

CDXControl* CDXControlButton::HitTest(float x, float y)
{
	return (x >= _x && x < (_x + 500) && y >= (_y + _h) && y <= (_y + 2 * _h)) ? this : NULL;
}

void CDXControlButton::SetMouseOver(BOOL mouseOver)
{
	BOOL oldMouseOver = _mouseOver;
	_mouseOver = mouseOver;
}

void CDXControlButton::UpdateControlText(CControllerData* pControllerData)
{
	_binding.SetText((LPSTR)GetMapText(pControllerData).c_str());
}

void CDXControlButton::Click()
{
	if (_controlClicked != nullptr) {
		_controlClicked(_action);
	}
}

void CDXControlButton::UpdateControlText(std::unordered_map<InputAction, InputMap>* pMapping, InputAction action)
{
	InputMap map = (*pMapping)[action];
	CControllerData data;

	if (action == InputAction::Action || action == InputAction::Cycle)
	{
		int debug = 0;
	}

	if (map.MouseKeySource != InputSource::Unknown)
	{
		data.Source = map.MouseKeySource;
		data.Data = data.Offset = map.MouseKeyIdentifier;
	}
	else if (map.JoystickSource != InputSource::Unknown)
	{
		data.Source = map.JoystickSource;
		data.Data = data.Offset = map.JoystickIdentifier;
	}
	else if (map.CurrentSource != InputSource::Unknown)
	{
		data.Source = map.CurrentSource;
		data.Data = data.Offset = map.CurrentData;
	}

	_binding.SetText((LPSTR)GetMapText(&data).c_str());
}

void CDXControlButton::SetColours(int colour1, int colour2, int colour3, int colour4)
{
	_pText->SetColours(colour1, colour2, colour3, colour4);
	_binding.SetColours(colour1, colour2, colour3, colour4);
}
