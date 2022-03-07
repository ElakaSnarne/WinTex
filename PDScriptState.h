#pragma once

#include "ScriptState.h"
#include "Utilities.h"

class CPDScriptState : public CScriptState
{
public:
	virtual void Init(LPBYTE script, int length, std::wstring file, int entry)
	{
		Script = script;
		Length = length;
		ScriptFile = file;
		ScriptEntry = entry;

		// Generate list of script entries
		_scriptEntries.clear();

		int count = GetInt(script, 0, 2);
		for (int i = 0; i < count; i++)
		{
			_scriptEntries[i] = GetInt(script, 16 + i * 2, 2);
		}
	}
};
