#pragma once

#include "ModuleBase.h"
#include "ScriptBase.h"
#include <unordered_map>
//#include "String.h"
#include "DXListBox.h"
#include "ScriptState.h"

enum VideoType
{
	Single = 1,
	Scripted = 2,
};

class CVideoModule : public CModuleBase
{
public:
	CVideoModule(VideoType type, int dmapIndex, int activeScript = -1);
	CVideoModule(VideoType type, LPCWSTR fileName, int itemIndex);
	virtual ~CVideoModule();

	VideoType Type;

	virtual void Resize(int width, int height);

	virtual void Dispose();
	virtual void Render();

	virtual void Resume();

protected:
	virtual void Initialize();

	CScriptBase* _scriptEngine;
	CScriptState* _scriptState;
	CDXListBox _listBox;

	static void DialogueOptionA(LPVOID data);
	static void DialogueOptionB(LPVOID data);
	static void DialogueOptionC(LPVOID data);

	static void SelectOption(int option);
	void SelectDialogueOption(int option);

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();
};
