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
	virtual int GetCurrentActions(CScriptState* pState, int currentObjectIndex) { return 0; };
	virtual void PermformAction(CScriptState* pState, int id, int action, int item) = NULL;
	virtual void SelectDialogueOption(CScriptState* pState, int option);

	CMapData* _mapEntry;

	CLocation* _pLoc;

	CMutex _scriptLock;
};
