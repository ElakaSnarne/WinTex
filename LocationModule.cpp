#include "LocationModule.h"
#include "AnimationController.h"
#include "AnimatedCursor.h"
#include "Globals.h"
#include "GameBase.h"
#include "GameController.h"
#include "Utilities.h"
#include "MainMenuModule.h"
#include "UAKMNewsPaperModule.h"
#include "InventoryModule.h"
#include "UAKMTravelModule.h"
#include "Items.h"
#include "AmbientAudio.h"
#include "HintModule.h"

float CLocationModule::_movement_forward = 0.0f;
float CLocationModule::_movement_backward = 0.0f;
float CLocationModule::_movement_left = 0.0f;
float CLocationModule::_movement_right = 0.0f;
float CLocationModule::_movement_x = 0.0f;
float CLocationModule::_movement_y = 0.0f;
float CLocationModule::_movement_z = 0.0f;
float CLocationModule::_smooth_movement_x = 0.0f;
float CLocationModule::_smooth_movement_z = 0.0f;
float CLocationModule::_speed = 0.0f;

int CLocationModule::CurrentAction = 0;

CLocationModule::CLocationModule(int locationId, int startupPosition) : CModuleBase(ModuleType::Location)
{
	_locationId = locationId;
	_startupPosition = startupPosition;

	CurrentObjectIndex = -1;
	CurrentActions = 0;
	CurrentAction = 0;
	CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Crosshair;

	_movement_forward = 0.0f;
	_movement_backward = 0.0f;
	_movement_left = 0.0f;
	_movement_right = 0.0f;
	_movement_x = 0.0f;
	_movement_y = 0.0f;
	_movement_z = 0.0f;
	_smooth_movement_x = 0.0f;
	_smooth_movement_z = 0.0f;
	_speed = MOVEMENT_WALK_SPEED;

	_oldPoint.x = 0;
	_oldPoint.y = 0;

	_scriptEngine = CGameController::GetScriptEngine();
	_scriptEngine->_pLoc = &_location;

	Resize(0, 0);

	_actionScriptState = CGameController::GetScriptState();
	_environmentScriptState = CGameController::GetScriptState();
	_queryActionScriptState = CGameController::GetScriptState();
	_initScriptState = CGameController::GetScriptState();

	_actionColour1 = _actionColour4 = 0;
	_actionColour2 = _actionColour3 = -1;
	_currentActionColour1 = _currentActionColour4 = 0;
	_currentActionColour2 = _currentActionColour3 = 0xffffff00;
	_unavailableActionColour1 = _unavailableActionColour4 = 0;
	_unavailableActionColour2 = _unavailableActionColour3 = 0xff808080;
}

CLocationModule::~CLocationModule()
{
	if (_actionScriptState != NULL && _actionScriptState->Script != NULL)
	{
		delete[] _actionScriptState->Script;
		_actionScriptState->Script = NULL;
	}

	if (_environmentScriptState != NULL)
	{
		_environmentScriptState->Script = NULL;
	}

	if (_queryActionScriptState != NULL)
	{
		_queryActionScriptState->Script = NULL;
	}
}

void CLocationModule::Initialize()
{
	CenterMouse();
	SetCursorClipping();

	// Reset all timers
	CGameController::ResetTimers();

	_scriptEngine->_mapEntry = _location._mapEntry = _scriptEngine->_mapEntry = CModuleController::pMap->Get(_locationId);

	// Load script file
	std::wstring scriptFile = CGameController::GetFileName(_scriptEngine->_mapEntry->ScriptFileIndex);
	BinaryData bd = LoadEntry(scriptFile.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);

	CAnimationController::Clear();

	// Load world data from location file
	LoadLocation(_scriptEngine->_mapEntry->LocationFileIndex, bd, scriptFile.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);

	// Extract startup coordinates from map
	SetLocationPosition(CModuleController::pMap->GetStartupPosition(_locationId, _startupPosition));

	// Make sure screen is black before starting initialization script
	dx.Clear();
	dx.Present();

	// Execute initialization script (1000 for UAKM, 0 for PD) on load
	_initScriptState->Init(bd.Data, bd.Length, scriptFile.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);
	_scriptEngine->Execute(_initScriptState, CGameController::GetLocationInitializationScriptId());
}

void CLocationModule::Render()
{
	dx.Clear(0.0f, 0.0f, 0.0f);

	if (CAnimationController::NoVideoAnim())
	{
		BOOL waitingForInput = (_initScriptState->WaitingForInput || _actionScriptState->WaitingForInput || _environmentScriptState->WaitingForInput);
		if (_initScriptState->WaitingForInput)
		{
			_scriptEngine->Resume(_initScriptState, TRUE);
		}
		else if (_actionScriptState->WaitingForInput)
		{
			_scriptEngine->Resume(_actionScriptState, TRUE);
		}
		else if (_environmentScriptState->WaitingForInput)
		{
			_scriptEngine->Resume(_environmentScriptState, TRUE);
		}
		else if (_initScriptState->ExecutionPointer < 0)
		{
			if (!CAnimationController::HasAnim())
			{
				_location.Animate();
			}
			_location.Render();
		}

		if (!waitingForInput && !_environmentScriptState->WaitingForMediaToFinish && !_actionScriptState->WaitingForMediaToFinish && !_initScriptState->WaitingForMediaToFinish)
		{
			_scriptEngine->Execute(_environmentScriptState, CGameController::GetLocationEnvironmentScriptId());
		}
		else if (!CAnimationController::HasAnim() || CAnimationController::IsDone())
		{
			if (_initScriptState->WaitingForMediaToFinish)
			{
				_scriptEngine->Resume(_initScriptState, TRUE);
			}
			else if (_actionScriptState->WaitingForMediaToFinish)
			{
				_scriptEngine->Resume(_actionScriptState, TRUE);
			}
			else if (_environmentScriptState->WaitingForMediaToFinish)
			{
				_scriptEngine->Resume(_environmentScriptState, TRUE);
			}
		}
	}

	// At this point, module could have changed (and this might be deleted)
	if (CModuleController::CurrentModule != this)
	{
		return;
	}

	CAnimationController::UpdateAndRender();
	if (CAnimationController::IsDone())// && CModuleController::CurrentModule == this && CModuleController::NextModule == NULL)
	{
		if (_actionScriptState->WaitingForMediaToFinish)
		{
			_scriptEngine->Resume(_actionScriptState, TRUE);
		}
		else
		{
			CAnimationController::Clear();
		}
	}

	if (!CAnimationController::HasAnim())
	{
		float mx{ _movement_x };
		float mz{ _movement_z };
		float tmx = mx;
		float tmz = mz;

		if (mx != 0.0 || mz != 0.0)
		{
			mx = _speed * mx;
			mz = _speed * mz;

			float totalmovement = sqrt(mx * mx + mz * mz);
			if (totalmovement > _speed)
			{
				mx /= (totalmovement / _speed);
				mz /= (totalmovement / _speed);
			}
		}

		// Exponential interpolation
		_smooth_movement_x = _smooth_movement_x * 0.85f + mx * 0.15f;
		_smooth_movement_z = _smooth_movement_z * 0.85f + mz * 0.15f;

		// Damp out near-zero motion
		if (abs(_smooth_movement_x) < 0.01f && _movement_x == 0.0f) { _smooth_movement_x = 0.0f; }
		if (abs(_smooth_movement_z) < 0.01f && _movement_z == 0.0f) { _smooth_movement_z = 0.0f; }

		int currentItemId = CGameController::GetCurrentItemId();
		if (CurrentAction == ACTION_USE && currentItemId == -1)
		{
			CurrentAction = 0;
		}

		_location.Move(_smooth_movement_x, _movement_y, _smooth_movement_z, tmz);

		if (_location.PointingChanged)
		{
			// Calculate object at centre
			int objectId = -1, subObjectId = -1;
			int hitObject = _location.GetPickObject(objectId, subObjectId);
			if (hitObject == -1)
			{
				CurrentObjectIndex = -1;
				CurrentActions = 0;
				// If action == Use, should not set Current Action to 0
				if (CurrentAction != ACTION_USE)
				{
					CurrentAction = 0;
				}
				CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Crosshair;
			}
			else
			{
				BOOL resetAction = (hitObject != CurrentObjectIndex);
				CurrentObjectIndex = hitObject;
				CurrentActions = _scriptEngine->GetCurrentActions(_queryActionScriptState, CurrentObjectIndex);

				if (CurrentAction != ACTION_USE && resetAction)
				{
					if (CurrentActions != 0)
					{
						// Set current action to be first in list of actions
						CurrentAction = 1;
						while ((CurrentActions & CurrentAction) == 0)
						{
							CurrentAction <<= 1;
						}

						SelectMouseAction();
					}
					else
					{
						CurrentAction = 0;
						CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Crosshair;
					}
				}
			}
		}

		// Render texts in correct colour
		int actions = CurrentActions;
		BOOL useYellow = TRUE;
		if (currentItemId >= 0)
		{
			actions |= ACTION_USE;
			useYellow = (CurrentObjectIndex >= 0);
		}
		int action = CurrentAction;

		int mask = 1;
		int colour = -1;

		float space = (dx.GetWidth() - _actionText[6].Width() - 20.0f) / 6;
		for (int i = 0; i < 7; i++)
		{
			if (mask == action && CAnimationController::NoAnimOrWave() && useYellow)
			{
				// yellow
				_actionText[i].SetColours(_currentActionColour1, _currentActionColour2, _currentActionColour3, _currentActionColour4);
			}
			else if ((actions & mask) != 0)
			{
				// white
				_actionText[i].SetColours(_actionColour1, _actionColour2, _actionColour3, _actionColour4);
			}
			else
			{
				// dark grey
				_actionText[i].SetColours(_unavailableActionColour1, _unavailableActionColour2, _unavailableActionColour3, _unavailableActionColour4);
			}
			mask <<= 1;

			_actionText[i].Render(space * i, 10.0f);
		}

		if (CAnimationController::NoAnimOrWave())
		{
			CConstantBuffers::Setup2D(dx);
			CShaders::SelectOrthoShader();
			dx.DisableZBuffer();

			if (currentItemId >= 0)
			{
				// Render in lower right corner
				CItems::RenderItemImage(currentItemId, (float)(dx.GetWidth() - 98), (float)(dx.GetHeight() - 78), FALSE);
			}

			if (action == ACTION_USE && currentItemId >= 0)
			{
				CModuleController::Cursors[(int)CAnimatedCursor::CursorType::Crosshair].Render();
				CItems::RenderItemImage(currentItemId, (float)(dx.GetWidth() / 2), (float)(dx.GetHeight() / 2), FALSE);
			}
			else
			{
				CModuleController::Cursors[CurrentActionMousePointerIndex].Render();
			}
		}
	}

	dx.Present(1, 0);
}

void CLocationModule::SelectMouseAction()
{
	switch (CurrentAction)
	{
	case ActionType::Look:
	{
		CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Look;
		break;
	}
	case  ActionType::Move:
	{
		CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Move;
		break;
	}
	case  ActionType::Get:
	{
		CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Grab;
		break;
	}
	case  ActionType::OnOff:
	{
		CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::OnOff;
		break;
	}
	case  ActionType::Talk:
	{
		CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Talk;
		break;
	}
	case  ActionType::Open:
	{
		CurrentActionMousePointerIndex = (int)CAnimatedCursor::CursorType::Open;
		break;
	}
	}
}

void CLocationModule::CycleActions(BOOL allowUse)
{
	int actions = CurrentActions;
	if (allowUse && CGameController::GetCurrentItemId() >= 0)
	{
		actions |= ACTION_USE;
		if (CurrentAction == 0)
		{
			CurrentAction = ACTION_USE >> 1;
		}
	}

	if (actions != 0)
	{
		CurrentAction <<= 1;
		while (CurrentAction > 0 && (actions & CurrentAction) == 0)
		{
			if (CurrentAction == ACTION_USE && allowUse && CGameController::GetCurrentItemId() >= 0)
			{
				break;
			}

			CurrentAction <<= 1;
			if (CurrentAction == 0x8000)
			{
				CurrentAction = 1;
			}
		}

		SelectMouseAction();
	}
}

void CLocationModule::LoadLocation(int locationFileIndex, BinaryData script, std::wstring file, int entry)
{
	_movement_x = _movement_y = _movement_z = 0.0f;

	if (_location.Load(locationFileIndex))
	{
		_queryActionScriptState->Clear();
		_queryActionScriptState->Init(script.Data, script.Length, file, entry);
		_actionScriptState->Clear();
		_actionScriptState->Init(script.Data, script.Length, file, entry);
		_environmentScriptState->Clear();
		_environmentScriptState->Init(script.Data, script.Length, file, entry);
		_queryActionScriptState->DebugMode = FALSE;
		_environmentScriptState->DebugMode = FALSE;

		_oldPoint.x = dx.GetWidth() / 2;
		_oldPoint.y = dx.GetHeight() / 2;

		if (CGameController::GetCurrentItemId() >= 0)
		{
			CurrentActions |= ACTION_USE;
		}
	}
	else
	{
		//MessageBox(NULL, L"Location Module failed to load location!", NULL, 0);
	}
}

void CLocationModule::Pause()
{
	//_midi.Stop();

	// TODO: When exiting this module, should put timers on hold
	UnsetCursorClipping();
}

void CLocationModule::Resume()
{
	CenterMouse();
	SetCursorClipping();

	if (_actionScriptState->WaitingForInput)
	{
		_scriptEngine->Resume(_actionScriptState, TRUE);
	}
}

void CLocationModule::Resize(int width, int height)
{
	_actionText[0].SetText("Look");
	_actionText[1].SetText("Move");
	_actionText[2].SetText("Get");
	_actionText[3].SetText("On/off");
	_actionText[4].SetText("Talk");
	_actionText[5].SetText("Open");
	_actionText[6].SetText("Use");
}

void CLocationModule::Cursor(float x, float y, BOOL relative)
{
	// Change view angle
	if (_hasFocus && CAnimationController::NoAnimOrWave() && (CInputMapping::IgnoreNextMouseInput == FALSE))
	{
		float delta_x = relative ? x : x - dx.GetWidth() / 2;
		float delta_y = relative ? y : y - dx.GetHeight() / 2;

		if (pConfig->InvertY) delta_y = -delta_y;
		auto scale = pConfig->MouselookScaling;

		_location.DeltaAngles(((float)delta_y) / 2000.0f * scale, ((float)delta_x) / 2000.0f * scale);
	}

	CenterMouse();
}

void CLocationModule::BeginAction()
{
	if (!CAnimationController::HasAnim())
	{
		if (CurrentAction != 0)
		{
			ClearCaptions(pDisplayCaptions);

			// Execute action on script
			if (CurrentObjectIndex >= 0)
			{
				// Find script with id same as current object index
				int currentItemId = CGameController::GetCurrentItemId();
				_scriptEngine->PermformAction(_actionScriptState, CurrentObjectIndex, CurrentAction, CurrentAction == ACTION_USE ? currentItemId : -1);

				if (CurrentAction != ACTION_USE || CGameController::GetCurrentItemId() != currentItemId)
				{
					CycleActions(FALSE);
				}

				_location.PointingChanged = TRUE;
			}
		}
	}
	else
	{
		CAnimationController::Skip();
		ClearCaptions(pDisplayCaptions);

		// Continue script
		if (_actionScriptState->WaitingForMediaToFinish)
		{
			_scriptEngine->Resume(_actionScriptState, TRUE);
		}
	}
}

void CLocationModule::Back()
{
	CModuleController::SendToFront(CMainMenuModule::MainMenuModule);
}

void CLocationModule::Cycle()
{
	// Right button
	if (CAnimationController::HasAnim())
	{
		CAnimationController::Skip();
		ClearCaptions(pDisplayCaptions);

		// Continue script
		if (_actionScriptState->WaitingForMediaToFinish)
		{
			_scriptEngine->Resume(_actionScriptState, TRUE);
		}
	}
	else if (CurrentActions != 0 || CGameController::GetCurrentItemId() >= 0)
	{
		CycleActions(TRUE);
	}
}

void CLocationModule::MoveForward(float v)
{
	_movement_forward = v;
	_movement_z = _movement_backward - _movement_forward;
}

void CLocationModule::MoveBack(float v)
{
	_movement_backward = v;
	_movement_z = _movement_backward - _movement_forward;
}

void CLocationModule::MoveLeft(float v)
{
	_movement_left = v;
	_movement_x = _movement_left - _movement_right;
}

void CLocationModule::MoveRight(float v)
{
	_movement_right = v;
	_movement_x = _movement_left - _movement_right;
}

void CLocationModule::MoveUp(float y)
{
	_movement_y = -y / 10.0f;
}

void CLocationModule::MoveDown(float y)
{
	_movement_y = y / 10.0f;
}

void CLocationModule::Run(BOOL run)
{
	_speed = run ? MOVEMENT_RUN_SPEED : MOVEMENT_WALK_SPEED;
}

void CLocationModule::Next()
{
	CycleItems(1);
}

void CLocationModule::Prev()
{
	CycleItems(-1);
}

void CLocationModule::Inventory()
{
	CModuleController::Push(new CInventoryModule());
}

void CLocationModule::Travel()
{
	// Show the travel module
	CModuleController::Push(new CUAKMTravelModule());
}

void CLocationModule::CycleItems(int direction)
{
	if (direction > 0)
	{
		// Next item
		CGameController::SelectNextItem();
	}
	else
	{
		// Previous item
		CGameController::SelectPreviousItem();
	}

	if (CGameController::GetCurrentItemId() == -1 && CurrentAction == ActionType::Use)
	{
		CycleActions(FALSE);
	}
}

void CLocationModule::Hints()
{
	CModuleController::Push(CGameController::GetHintModule());
}

#ifdef DEBUG
void CLocationModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (key == VK_F1)
	{
		_location._renderPoints = !_location._renderPoints;
	}
	else if (key == VK_F2)
	{
		_location._renderLines = !_location._renderLines;
	}
	else if (key == VK_F3)
	{
		_location._renderPaths = !_location._renderPaths;
	}
	else if (key == VK_F4)
	{
		_location._renderTextured = !_location._renderTextured;
	}
	else if (key == VK_F5)
	{
		_location._disableClipping = !_location._disableClipping;
	}
}

void CLocationModule::MouseWheel(int scroll)
{
	// Modify translation of current object
	BOOL ctrl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
	SHORT alt = (GetKeyState(VK_LMENU) & 0x80) != 0;
	SHORT shift = (GetKeyState(VK_LSHIFT) & 0x80) != 0;

	if (ctrl || alt || shift)
	{
		_location.MoveObject(scroll > 0 ? 0.1f : -0.1f, ctrl, alt, shift);
	}
}
#endif
