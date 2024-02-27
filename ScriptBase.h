#pragma once

#include <Windows.h>
#include <unordered_map>
#include "DXText.h"
#include "ScriptState.h"
#include "Map.h"
#include "Location.h"
#include "Mutex.h"

class CScriptBase
{
public:
	CScriptBase();
	~CScriptBase();

	virtual void Execute(CScriptState* pState, int id) = NULL;
	virtual void Resume(CScriptState* pState, BOOL breakWait = FALSE) = NULL;
	virtual ActionType GetCurrentActions(CScriptState* pState, int currentObjectIndex) { return ActionType::None; };
	virtual void PermformAction(CScriptState* pState, int id, ActionType action, int item) = NULL;
	virtual void SelectDialogueOption(CScriptState* pState, int option) = NULL;

	CMapData* _mapEntry;

	CLocation* _pLoc;

	CMutex _scriptLock;
};
