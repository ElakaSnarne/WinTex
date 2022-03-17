#pragma once
#include "DXButton.h"
#include <unordered_map>
#include "InputMapping.h"
#include <string>

class CControllerData;

class CDXControlButton : public CDXButton
{
public:
	CDXControlButton(LPSTR function, std::unordered_map<InputAction, InputMap>* pMapping, BOOL isJoystick, float w, float h, void(*onClick)(LPVOID) = NULL, LPVOID data = NULL);
	~CDXControlButton();

	virtual void Render();
	virtual CDXControl* HitTest(float x, float y);
	virtual void SetMouseOver(BOOL mouseOver);

	std::string GetMapText(CControllerData* pControllerData);

	void UpdateControlText(CControllerData* pControllerData);
	void UpdateControlText(std::unordered_map<InputAction, InputMap>* pMapping, InputAction action);

	void SetIsBeingConfigured(BOOL configuring) { _isBeingConfigured = configuring; }

	BOOL IsJoystickConfigControl() { return _isJoystick; }

protected:
	BOOL _isJoystick;
	CDXText _binding;
	BOOL _isBeingConfigured;
};
