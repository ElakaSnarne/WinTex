#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
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
	virtual ~CScriptBase();

	virtual void Execute(CScriptState* pState, int id) = 0;
	virtual void Resume(CScriptState* pState, BOOL breakWait = FALSE) = 0;
	virtual ActionType GetCurrentActions(CScriptState* pState, int currentObjectIndex) { return ActionType::None; };
	virtual void PermformAction(CScriptState* pState, int id, ActionType action, int item) = 0;
	virtual void SelectDialogueOption(CScriptState* pState, int option) = 0;

	CMapData* _mapEntry;

	CLocation* _pLoc;

	CMutex _scriptLock;
};
