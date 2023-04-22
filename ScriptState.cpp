#include "ScriptState.h"

CScriptState::CScriptState()
{
	Script = NULL;
	Length = 0;

	DebugMode = FALSE;

	Clear();
}

void CScriptState::Clear()
{
	ExecutionPointer = 0;
	WaitingForMediaToFinish = FALSE;
	WaitingForInput = FALSE;
	WaitingForExternalModule = FALSE;
	SelectedOption = 0;
	SelectedValue = 0;

	AskAbout = FALSE;
	Offer = FALSE;
	TopItemOffset = 0;

	Mode = InteractionMode::None;

	AllowedAction = 0;
	QueryAction = FALSE;
	CurrentAction = 0;

	LastDialoguePoint = -1;
	FrameTrigger = -1;
}

int CScriptState::GetScript(int id)
{
	int i = _scriptEntries[id];
	return (i > 0) ? i : -1;
}
