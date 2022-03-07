#pragma once

#include <dinput.h>
#include <list>
#include "Utilities.h"
#include <string>

#define UM_GAMEPAD		WM_USER+0x2000

class CControllerData
{
public:
	InputSource Source;
	DWORD Offset;
	DWORD Type;
	DWORD Data;
};

class CGamepad
{
public:
	CGamepad(IDirectInput8* pInput, HWND hWnd, GUID deviceId);

	void Update();

	std::string GetName(int offset, int data);

private:
	IDirectInput8* _pInput;
	HWND _hWnd;
	GUID _deviceId;

	IDirectInputDevice8* _pDevice;

	static BOOL EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* instance, void* context);

	DIJOYSTATE2 _state;

	std::list<CControllerData*> _controllerData;
};

class CGamepadController
{
public:
	CGamepadController(HWND hWnd);
	~CGamepadController();

	static void Init(HWND hWnd);
	static void Dispose();

	void AddGamepad(GUID deviceId);

	void Update();

	static CGamepadController* GamepadController;

	std::string GetName(int offset, int data);

private:
	static BOOL CALLBACK DeviceEnumCallback(const DIDEVICEINSTANCE* instance, void* context);

	IDirectInput8* _pInput;
	HWND _hWnd;

	std::list<CGamepad*> _pads;
};
