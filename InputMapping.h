#pragma once

#include <Windows.h>
#include <unordered_map>

enum class InputAction
{
	Cursor,
	Action,
	Back,
	Cycle,
	MoveForward,
	MoveBack,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	Run,
	Next,
	Prev,
	Inventory,
	Travel,
};

enum class InputSource
{
	Unknown = 0,
	Mouse = 1,
	MouseButton = 2,
	MouseWheel = 4,
	Key = 8,
	JoystickAxis = 16,
	JoystickButton = 32,
	JoystickDPad = 64
};

BOOL IsJoystickSource(InputSource source);

inline InputSource operator|(InputSource a, InputSource b)
{
	return static_cast<InputSource>(static_cast<int>(a) | static_cast<int>(b));
}

inline InputSource operator&(InputSource a, InputSource b)
{
	return static_cast<InputSource>(static_cast<int>(a)& static_cast<int>(b));
}

struct InputMap
{
	InputSource JoystickSource;
	int JoystickIdentifier;
	InputSource MouseKeySource;
	int MouseKeyIdentifier;
	InputSource AcceptableSource;
	int CurrentData;
	InputSource CurrentSource;
	BOOL IsActive;
	int CurrentJoystickData;
};

class CInputMapping
{
public:
	static std::unordered_map<InputAction, InputMap> ControlsMap;
	static BOOL IgnoreNextMouseInput;
	static void LoadControlsMap();
	static void SaveControlsMap();

	static void Input(InputSource source, int identifier, int value);

private:
	static void WriteConfig(int* pConfig, int ix, InputAction action);
	static void ReadConfig(int* pConfig, int ix, InputAction action);
};
