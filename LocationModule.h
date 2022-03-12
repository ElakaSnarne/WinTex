#pragma once

#include "ModuleBase.h"
#include "Location.h"
#include "ScriptBase.h"
#include "DXText.h"
#include "ScriptState.h"

#define ACTION_USE	64
#define MOVEMENT_WALK_SPEED 0.2f
#define MOVEMENT_RUN_SPEED 0.5f

class CLocationModule : public CModuleBase
{
public:
	CLocationModule(int locationId, int startupPosition);
	virtual ~CLocationModule();

	virtual void Resize(int width, int height);

	virtual void Pause();
	virtual void Resume();
	virtual void Render();

	static float _movement_x;
	static float _movement_y;
	static float _movement_z;
	static float _speed;

	static int CurrentAction;

protected:
	virtual void Initialize();

	int _actionColour1;
	int _actionColour2;
	int _actionColour3;
	int _actionColour4;
	int _currentActionColour1;
	int _currentActionColour2;
	int _currentActionColour3;
	int _currentActionColour4;
	int _unavailableActionColour1;
	int _unavailableActionColour2;
	int _unavailableActionColour3;
	int _unavailableActionColour4;

	int _locationId;
	int _startupPosition;

	CLocation _location;

	// Properies for location object selection
	int CurrentObjectIndex;
	int CurrentActions;
	int CurrentActionMousePointerIndex;
	CDXText _actionText[7];

	virtual void SelectMouseAction();
	void CycleActions(BOOL allowUse);

	CScriptState* _environmentScriptState;
	CScriptState* _actionScriptState;
	CScriptState* _queryActionScriptState;
	CScriptBase* _scriptEngine;
	CScriptState* _initScriptState;

	POINT _oldPoint;

	virtual void LoadLocation(int locationFileIndex, BinaryData script, std::wstring file, int entry);
	virtual void SetLocationPosition(StartupPosition pos) { _location.SetPosition(pos); _location.UpdateSprites(); }

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();
	virtual void Cycle();
	virtual void MoveForward(float v);
	virtual void MoveBack(float v);
	virtual void MoveLeft(float v);
	virtual void MoveRight(float v);
	virtual void MoveUp(float y);
	virtual void MoveDown(float y);
	virtual void Run(BOOL run);
	virtual void Next();
	virtual void Prev();
	virtual void Inventory();
	virtual void Travel();
	virtual void Hints();

	void CycleItems(int direction);
};
