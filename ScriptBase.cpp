#include "ScriptBase.h"
#include "Globals.h"
#include "GameBase.h"
#include "GameController.h"

CScriptBase::CScriptBase()
{
	ZeroMemory(&_mapEntry, sizeof(_mapEntry));

	_pLoc = NULL;
}

CScriptBase::~CScriptBase()
{
}
