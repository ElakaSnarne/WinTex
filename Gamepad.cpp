#include "Gamepad.h"
#include <Windows.h>

#pragma comment(lib, "dinput8.lib")

CGamepadController* CGamepadController::GamepadController = NULL;

CGamepadController::CGamepadController(HWND hWnd)
{
	_hWnd = hWnd;
	_pInput = NULL;

	if (SUCCEEDED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_pInput, NULL)))
	{
		_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, DeviceEnumCallback, (void*)this, DIEDFL_ATTACHEDONLY);
	}
}

CGamepadController::~CGamepadController()
{
	if (_pInput != NULL)
	{
		_pInput->Release();
		_pInput = NULL;
	}

	// TODO: Delete gamepads
}

void CGamepadController::Init(HWND hWnd)
{
	GamepadController = new CGamepadController(hWnd);
}

void CGamepadController::Dispose()
{
	delete GamepadController;
}

BOOL CGamepadController::DeviceEnumCallback(const DIDEVICEINSTANCE* instance, void* context)
{
	CGamepadController* pHC = (CGamepadController*)context;
	pHC->AddGamepad(instance->guidProduct);
	return DIENUM_CONTINUE;
}

void CGamepadController::AddGamepad(GUID deviceId)
{
	_pads.push_back(new CGamepad(_pInput, _hWnd, deviceId));
}

void CGamepadController::Update()
{
	// Call update on every controller in list
	for (auto pad : _pads)
	{
		pad->Update();
	}
}

std::string CGamepadController::GetName(int offset, int data)
{
	for (auto pad : _pads)
	{
		return pad->GetName(offset, data);
	}

	return "Unknown";
}

CGamepad::CGamepad(IDirectInput8* pInput, HWND hWnd, GUID deviceId)
{
	_pInput = pInput;
	_hWnd = hWnd;
	_deviceId = deviceId;
	ZeroMemory(&_state, sizeof(_state));

	pInput->CreateDevice(deviceId, &_pDevice, NULL);
	HRESULT hr = _pDevice->SetDataFormat(&c_dfDIJoystick2);
	DIPROPDWORD prop;
	prop.diph.dwHeaderSize = sizeof(prop.diph);
	prop.diph.dwSize = sizeof(prop);
	prop.diph.dwHow = DIPH_DEVICE;
	prop.diph.dwObj = 0;
	prop.dwData = 256;
	_pDevice->SetProperty(DIPROP_BUFFERSIZE, &prop.diph);
	_pDevice->EnumObjects(EnumObjectsCallback, this, DIDFT_ALL);
	_pDevice->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE);
}

BOOL CGamepad::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* instance, void* context)
{
	CGamepad* pPad = (CGamepad*)context;

	CControllerData* data = new CControllerData();
	data->Source = (instance->dwType & DIDFT_AXIS) ? InputSource::JoystickAxis : (instance->dwType & DIDFT_BUTTON) ? InputSource::JoystickButton : (instance->dwType & DIDFT_POV) ? InputSource::JoystickDPad : InputSource::Unknown;
	data->Offset = instance->dwOfs;
	data->Type = instance->dwType;
	//data->Name = ToString(instance->tszName);
	pPad->_controllerData.push_back(data);

	if (instance->dwType & DIDFT_AXIS)
	{
		DIPROPRANGE range;
		range.diph.dwSize = sizeof(DIPROPRANGE);
		range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		range.diph.dwHow = DIPH_BYID;
		range.diph.dwObj = instance->dwType;
		range.lMin = -1000;
		range.lMax = 1000;
		pPad->_pDevice->SetProperty(DIPROP_RANGE, &range.diph);

		DIPROPDWORD dw;
		dw.diph.dwSize = sizeof(DIPROPDWORD);
		dw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dw.diph.dwHow = DIPH_BYID;
		dw.diph.dwObj = instance->dwType;
		dw.dwData = 100;
		pPad->_pDevice->SetProperty(DIPROP_DEADZONE, &dw.diph);
	}

	return DIENUM_CONTINUE;
}

void CGamepad::Update()
{
	if (SUCCEEDED(_pDevice->Acquire()) && SUCCEEDED(_pDevice->Poll()) && SUCCEEDED(_pDevice->GetDeviceState(sizeof(_state), &_state)))
	{
		DWORD dwItemCount = 20;
		DIDEVICEOBJECTDATA* data = new DIDEVICEOBJECTDATA[dwItemCount];
		if (data != NULL)
		{
			if (SUCCEEDED(_pDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), data, &dwItemCount, 0)))
			{
				for (int i = 0; i < dwItemCount; i++)
				{
					InputSource source = InputSource::Unknown;
					int dwData = data[i].dwData;

					if (data[i].dwOfs >= offsetof(DIJOYSTATE2, rgbButtons[0]) && data[i].dwOfs <= offsetof(DIJOYSTATE2, rgbButtons[127]))	// Button
					{
						source = InputSource::JoystickButton;
						if (dwData & 0x80)
						{
							dwData = (dwData & 0x7f) | 0x80000000;
						}
					}
					else if (data[i].dwOfs == offsetof(DIJOYSTATE2, lX) || data[i].dwOfs == offsetof(DIJOYSTATE2, lY)	// Left stick
						|| data[i].dwOfs == offsetof(DIJOYSTATE2, lRx) || data[i].dwOfs == offsetof(DIJOYSTATE2, lRy)	// Right stick
						|| data[i].dwOfs == offsetof(DIJOYSTATE2, lZ) || data[i].dwOfs == offsetof(DIJOYSTATE2, lRz))	// ?
					{
						source = InputSource::JoystickAxis;
					}
					else if (data[i].dwOfs >= offsetof(DIJOYSTATE2, rgdwPOV[0]) && data[i].dwOfs <= offsetof(DIJOYSTATE2, rgdwPOV[3]))	// DPad
					{
						source = InputSource::JoystickDPad;
					}

					if (source != InputSource::Unknown)
					{
						PostMessage(_hWnd, UM_GAMEPAD, (WPARAM)((((int)source) << 16) | data[i].dwOfs), (LPARAM)dwData);
					}
				}
			}

			delete[] data;
		}
	}
}

/*
double Gain(double x)
{
	// Assuming a 10% deadspace
	x = max(0.0, x - 0.1) / 0.9;
	double a = 5.5;
	double y = (pow(a, x) - 1.0) / (a - 1.0);
	return y * 10;
}

POINTF CGamepad::GetDelta()
{
	POINTF pt{ ((float)_state.lX) / 1000.0f , ((float)_state.lY) / 1000.0f };

	pt.x *= Gain(abs(pt.x));
	pt.y *= Gain(abs(pt.y));

	return pt;
}
*/

std::string CGamepad::GetName(int offset, int data)
{
	if (offset == 0 || offset == 4)
	{
		return "X/Y-Axes";
	}
	else if (offset == 8 || offset == 20)
	{
		return "Z-Axis/rotation";
	}
	else if (offset == 12 || offset == 16)
	{
		return "X/Y-Axes rotation";
	}
	else if (offset == 24)
	{
		return "Slider 1";
	}
	else if (offset == 28)
	{
		return "Slider 2";
	}
	else if (offset >= 32 && offset < 48)
	{
		return "POV " + std::to_string(data / 100);
	}
	else if (offset >= 48 && offset < 176)
	{
		return "Button " + std::to_string(offset - 48);
	}

	/*
		LONG    lVX;                    /* x-axis velocity
		LONG    lVY;                    /* y-axis velocity
		LONG    lVZ;                    /* z-axis velocity
		LONG    lVRx;                   /* x-axis angular velocity
		LONG    lVRy;                   /* y-axis angular velocity
		LONG    lVRz;                   /* z-axis angular velocity
		LONG    rglVSlider[2];          /* extra axes velocities
		LONG    lAX;                    /* x-axis acceleration
		LONG    lAY;                    /* y-axis acceleration
		LONG    lAZ;                    /* z-axis acceleration
		LONG    lARx;                   /* x-axis angular acceleration
		LONG    lARy;                   /* y-axis angular acceleration
		LONG    lARz;                   /* z-axis angular acceleration
		LONG    rglASlider[2];          /* extra axes accelerations
		LONG    lFX;                    /* x-axis force
		LONG    lFY;                    /* y-axis force
		LONG    lFZ;                    /* z-axis force
		LONG    lFRx;                   /* x-axis torque
		LONG    lFRy;                   /* y-axis torque
		LONG    lFRz;                   /* z-axis torque
		LONG    rglFSlider[2];          /* extra axes forces
	*/

	/*
	for (auto data : _controllerData)
	{
		if (data->Offset == offset)
		{
			return data->Name;
		}
	}
	*/

	return "Unknown";
}
