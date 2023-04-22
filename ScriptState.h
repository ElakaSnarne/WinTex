#pragma once

#include <Windows.h>
#include <unordered_map>
#include <string>
#include "Enums.h"

class CScriptState
{
public:
	CScriptState();

	virtual void Init(LPBYTE script, int length, std::wstring file, int entry) = NULL;
	void Clear();

	std::wstring ScriptFile;
	int ScriptEntry;

	int GetScript(int id);

	int ExecutionPointer;
	BOOL WaitingForMediaToFinish;
	BOOL WaitingForInput;
	BOOL WaitingForExternalModule;
	int SelectedOption;
	int SelectedValue;

	BOOL AskAbout;
	BOOL Offer;
	BOOL Buy;

	int TopItemOffset;

	int AllowedAction;
	BOOL QueryAction;
	int CurrentAction;

	int LastDialoguePoint;
	int FrameTrigger;

	LPBYTE Script;
	int Length;

	BOOL DebugMode;

	InteractionMode Mode;

protected:
	std::unordered_map<int, int> _scriptEntries;
};
