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

	int GetInt(int offset, int size);
	int Read8();
	int Read8s();
	int Read16();
	int Read16s();
	int Read32();
	float Read16_16();
	float Read12_4();

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
	BOOL AskingAboutBuyables;

	int TopItemOffset;

	ActionType AllowedAction;
	BOOL QueryAction;
	ActionType CurrentAction;

	int LastDialoguePoint;
	int FrameTrigger;

	LPBYTE Script;
	int Length;

	BOOL DebugMode;

	InteractionMode Mode;

	int Parameter;

protected:
	std::unordered_map<int, int> _scriptEntries;
};
