#include "InputMapping.h"
#include <Windows.h>
#include "Utilities.h"
#include "GameController.h"

std::unordered_map<InputAction, InputMap> CInputMapping::ControlsMap;
BOOL CInputMapping::IgnoreNextMouseInput = FALSE;

BOOL IsJoystickSource(InputSource source)
{
	return (source == InputSource::JoystickAxis || source == InputSource::JoystickButton || source == InputSource::JoystickDPad);
}

void CInputMapping::ReadConfig(int* pConfig, int ix, InputAction action)
{
	ControlsMap[action].MouseKeySource = (InputSource)pConfig[ix * 4 + 0];
	ControlsMap[action].MouseKeyIdentifier = pConfig[ix * 4 + 1];
	ControlsMap[action].JoystickSource = (InputSource)pConfig[ix * 4 + 2];
	ControlsMap[action].JoystickIdentifier = pConfig[ix * 4 + 3];
}

void CInputMapping::LoadControlsMap()
{
	// Setup default controls map
	ControlsMap[InputAction::Cursor] = { InputSource::Unknown,0,InputSource::Mouse,0,InputSource::Mouse | InputSource::JoystickAxis,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Action] = { InputSource::Unknown,0,InputSource::MouseButton,-1,InputSource::MouseButton | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Back] = { InputSource::Unknown,0,InputSource::Key,0x10000,InputSource::MouseButton | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Cycle] = { InputSource::Unknown,0,InputSource::MouseButton,1,InputSource::MouseButton | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::MoveForward] = { InputSource::Unknown,0,InputSource::Key,0x0110000, InputSource::Key | InputSource::JoystickAxis | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::MoveBack] = { InputSource::Unknown,0,InputSource::Key,0x1f0000, InputSource::Key | InputSource::JoystickAxis | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::MoveLeft] = { InputSource::Unknown,0,InputSource::Key,0x1e0000, InputSource::Key | InputSource::JoystickAxis | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::MoveRight] = { InputSource::Unknown,0,InputSource::Key,0x200000, InputSource::Key | InputSource::JoystickAxis | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::MoveUp] = { InputSource::Unknown,0,InputSource::Key,0x120000, InputSource::Key | InputSource::JoystickAxis | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::MoveDown] = { InputSource::Unknown,0,InputSource::Key,0x100000, InputSource::Key | InputSource::JoystickAxis | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Run] = { InputSource::Unknown,0,InputSource::Key,0x2a0000, InputSource::MouseButton | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Next] = { InputSource::Unknown,0,InputSource::MouseWheel,-1, InputSource::MouseWheel | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Prev] = { InputSource::Unknown,0,InputSource::MouseWheel,1, InputSource::MouseWheel | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Inventory] = { InputSource::Unknown,0,InputSource::Key,0x170000, InputSource::MouseButton | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };
	ControlsMap[InputAction::Travel] = { InputSource::Unknown,0,InputSource::Key,0x140000, InputSource::MouseButton | InputSource::Key | InputSource::JoystickButton,0,InputSource::Unknown,FALSE,0 };

	HKEY hk;
	std::wstring key = L"SOFTWARE\\Access Software\\" + pConfig->GetGameName();
	if (RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, KEY_READ, &hk) == 0)
	{
		int config[15 * 4];
		// Load blob from registry

		DWORD size = sizeof(config);
		if (RegGetValue(hk, L"", L"Controls", RRF_RT_REG_BINARY, NULL, (PVOID)&config, &size) == ERROR_SUCCESS)
		{
			int ix = 0;
			ReadConfig(config, ix++, InputAction::Cursor);
			ReadConfig(config, ix++, InputAction::Action);
			ReadConfig(config, ix++, InputAction::Back);
			ReadConfig(config, ix++, InputAction::Cycle);
			ReadConfig(config, ix++, InputAction::MoveForward);
			ReadConfig(config, ix++, InputAction::MoveBack);
			ReadConfig(config, ix++, InputAction::MoveLeft);
			ReadConfig(config, ix++, InputAction::MoveRight);
			ReadConfig(config, ix++, InputAction::MoveUp);
			ReadConfig(config, ix++, InputAction::MoveDown);
			ReadConfig(config, ix++, InputAction::Run);
			ReadConfig(config, ix++, InputAction::Next);
			ReadConfig(config, ix++, InputAction::Prev);
			ReadConfig(config, ix++, InputAction::Inventory);
			ReadConfig(config, ix++, InputAction::Travel);
		}

		RegCloseKey(hk);
	}
}

void CInputMapping::WriteConfig(int* pConfig, int ix, InputAction action)
{
	pConfig[ix * 4 + 0] = (int)ControlsMap[action].MouseKeySource;
	pConfig[ix * 4 + 1] = ControlsMap[action].MouseKeyIdentifier;
	pConfig[ix * 4 + 2] = (int)ControlsMap[action].JoystickSource;
	pConfig[ix * 4 + 3] = ControlsMap[action].JoystickIdentifier;
}

void CInputMapping::SaveControlsMap()
{
	HKEY hk;
	std::wstring key = L"SOFTWARE\\Access Software\\" + pConfig->GetGameName();
	if (RegCreateKeyEx(HKEY_CURRENT_USER, key.c_str(), 0, 0, 0, KEY_WRITE, NULL, &hk, NULL) == 0)
	{
		// Save controls map to registry
		int config[15 * 4];
		int ix = 0;
		WriteConfig(config, ix++, InputAction::Cursor);
		WriteConfig(config, ix++, InputAction::Action);
		WriteConfig(config, ix++, InputAction::Back);
		WriteConfig(config, ix++, InputAction::Cycle);
		WriteConfig(config, ix++, InputAction::MoveForward);
		WriteConfig(config, ix++, InputAction::MoveBack);
		WriteConfig(config, ix++, InputAction::MoveLeft);
		WriteConfig(config, ix++, InputAction::MoveRight);
		WriteConfig(config, ix++, InputAction::MoveUp);
		WriteConfig(config, ix++, InputAction::MoveDown);
		WriteConfig(config, ix++, InputAction::Run);
		WriteConfig(config, ix++, InputAction::Next);
		WriteConfig(config, ix++, InputAction::Prev);
		WriteConfig(config, ix++, InputAction::Inventory);
		WriteConfig(config, ix++, InputAction::Travel);

		// Save blob to registry
		DWORD size = sizeof(config);
		RegSetValueEx(hk, L"Controls", 0, REG_BINARY, (PBYTE)&config, size);

		RegCloseKey(hk);
	}
}

void CInputMapping::Input(InputSource source, int identifier, int value)
{
	// Workaround for the location module requiring to center the mouse
	if (source == InputSource::Mouse && IgnoreNextMouseInput)
	{
		IgnoreNextMouseInput = FALSE;
		return;
	}

	// Pair current axis
	int pairedXAxis = -1;
	if (source == InputSource::JoystickAxis)
	{
		if (identifier == 0 || identifier == 4)
		{
			pairedXAxis = 0;
		}
		else if (identifier == 8 || identifier == 20)
		{
			pairedXAxis = 20;
		}
		else if (identifier == 12 || identifier == 16)
		{
			pairedXAxis = 12;
		}
	}

	// Set or clear input
	for (auto bind : ControlsMap)
	{
		InputAction action = bind.first;
			
		if ((bind.second.JoystickSource == source && (bind.second.JoystickIdentifier == identifier || bind.second.JoystickIdentifier == pairedXAxis)) ||
			(bind.second.MouseKeySource == source && bind.second.MouseKeyIdentifier == identifier))
		{
			ControlsMap[bind.first].CurrentSource = source;

			// If axis data, only modify correct half
			if (source == InputSource::JoystickAxis)
			{
				int oldData = bind.second.CurrentJoystickData;
				int newData = 0;

				//Trace(L"Joystick input: Offset = ");
				//Trace(identifier);
				//Trace(L", paired x-axis: ");
				//Trace(pairedXAxis);

				if (action == InputAction::MoveForward || action == InputAction::MoveBack || action == InputAction::MoveLeft || action == InputAction::MoveRight)
				{
					action = InputAction::MoveForward;
					oldData = ControlsMap[action].CurrentJoystickData;
				}

				if (identifier == pairedXAxis)
				{
					// X data
					newData = ((value << 16) & ~0xffff) | (oldData & 0xffff);
				}
				else
				{
					// Y data
					newData = (value & 0xffff) | (oldData & ~0xffff);
				}

				ControlsMap[action].CurrentJoystickData = ControlsMap[action].CurrentData = newData;
			}
			else
			{
				ControlsMap[action].CurrentData = value;
			}

			ControlsMap[action].IsActive = TRUE;
		}
	}
}
