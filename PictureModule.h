#pragma once

#include "FullScreenModule.h"
#include "ScriptBase.h"
#include "Image.h"

class CPictureModule : public CFullScreenModule
{
public:
	CPictureModule(int fileId, int entryIndex, CScriptBase* pScript, CScriptState* scriptState);
	virtual ~CPictureModule();

	virtual void Render();

protected:
	CScriptBase* _script;
	CScriptState* _state;
	CImage* _image;
	BOOL _rendered;

	// Input related
	virtual void BeginAction();
};
