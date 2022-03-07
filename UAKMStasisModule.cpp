#include "UAKMStasisModule.h"
#include "ModuleController.h"
#include "GameController.h"
#include "AnimationController.h"
#include "MainMenuModule.h"
#include "Utilities.h"
#include "UAKMGame.h"

int ProgressLightsXOffsets[] = { 108, 130, 152, 174, 241, 263, 285, 307, 377, 399, 421, 443 };

int TemperatureStagePositions[] = { 252, 238, 238, 238, 238, 172, 172, 172, 172, 144, 144, 144, 144 };
int OxygenStagePositions[] = { 252, 252, 218, 218, 218, 218, 218, 218, 178, 178, 144, 144, 144 };

ControlTable StasisControls[] = { {3,309,238},{ 5,113,294 },{ 7,232,294 },{ 9,354,294 },{ 11,93,353 },{ 13,226,353 },{ 15,361,353 },{ 31,379,204 },{ 23,87,47 },{ 25,187,47 },{ 27,286,47 },{ 29,385,47 },{ 34,210,113 },{ 36,309,113 } };

ControlCoordinates StasisControlCoordinates[] = {
	{ KEYCODE_LEFT, 0, 1, 0, 0, 0, ACTION_LEFT },
	{ KEYCODE_RIGHT, 0, 1, 0, 0, 0, ACTION_RIGHT },
	{ KEYCODE_UP, 0, 1, 0, 0, 0, ACTION_UP },
	{ KEYCODE_DOWN, 0, 1, 0, 0, 0, ACTION_DOWN },
	{ KEYCODE_ESCAPE, 4, 113, 137, 309, 357, ACTION_STASIS_EXIT },
	{ KEYCODE_PLUS, 0, 1, 0, 0, 0, ACTION_PLUS },
	{ KEYCODE_MINUS, 0, 1, 0, 0, 0, ACTION_MINUS },
	{ KEYCODE_PAGEUP, 0, 1, 0, 0, 0, ACTION_PAGEUP },
	{ KEYCODE_PAGEDOWN, 0, 1, 0, 0, 0, ACTION_PAGEDOWN },
	{ KEYCODE_NONE, 0, 230, 264, 380, 416, ACTION_STASIS_POWER },
	{ KEYCODE_NONE, 0, 142, 269, 87, 125, ACTION_STASIS_TEMPERATURE },
	{ KEYCODE_NONE, 0, 47, 91, 87, 182, ACTION_STASIS_PENTAHOL },
	{ KEYCODE_NONE, 0, 47, 91, 187, 280, ACTION_STASIS_BICARB },
	{ KEYCODE_NONE, 0, 47, 91, 286, 380, ACTION_STASIS_ADRENALINE },
	{ KEYCODE_NONE, 0, 47, 91, 385, 482, ACTION_STASIS_SHOCK },
	{ KEYCODE_NONE, 0, 142, 269, 446, 478, ACTION_STASIS_OXYGEN },
	{ KEYCODE_H, 0, 113, 137, 210, 258, ACTION_STASIS_HINT } };

char* Entry = "I've seen consoles like this before.  The four buttons along the top must administer injections.  The two slider controls look like they regulate the air temperature and oxygen level inside the cryonic tube.  I seem to remember that slider mechanisms like these are really sensitive and need to be moved slowly.  Well, first things first, I'll need to turn this thing on.";
char* SliderWarning = "\nHmmm ... that yellow warning light doesn't look good ... maybe I should move the slider back to where it was and try again.";
char* ButtonWarning = "\nHmmm ... that yellow warning light doesn't look good ... maybe I should wait a few seconds and see if it goes off before I try anything else.";
char* GoodAction = "\nAha!  A red light went off in one of those Phase 1 boxes.  I must've done something right.  Now I've got to get those other boxes to light up.";

StasisProgression Progression[] = {
	{ 0, 238, 236, 253, 249, 253, 0, 0, 422 },
	{ 5, 218, 209, 247, 216, 253, 1, 0, 423 },
	{ 3, 0, 209, 247, 202, 234, 2, 0, 424 },
	{ 4, 0, 209, 247, 202, 234, 3, 2, 425 },
	{ 0, 172, 170, 247, 202, 234, 4, 0, 426 },
	{ 1, 0, 156, 188, 202, 234, 5, 0, 427 },
	{ 3, 0, 156, 188, 202, 234, 6, 0, 428 },
	{ 5, 178, 156, 188, 176, 234, 7, 4, 429 },
	{ 0, 144, 142, 188, 162, 194, 8, 0, 430 },
	{ 5, 144, 142, 154, 142, 194, 9, 0, 431 },
	{ 3, 0, 142, 154, 142, 154, 10, 0, 432 },
	{ 2, 0, 142, 154, 142, 154, 11, 8, 433 } };

CUAKMStasisModule::CUAKMStasisModule(int parameter) : CFullScreenModule(ModuleType::Stasis)
{
	// A[parameter] will be set to 1 if success, 2 if exit, 3 if fail
	_parameter = parameter;

	_currentPage = 0;
	_heartRate = 0;

	_stasisSettings = 0;
	_stasisStages = 0;
	_stage = 0;
	_movingSlider = 0;

	_nextPulseChange = 0;
	_nextAudio = 0;
	_badPlayed = FALSE;
	_exitAfterTime = 0;
	_nextWarningChange = 0;
	_badSlider = 0;
}

CUAKMStasisModule::~CUAKMStasisModule()
{
	Dispose();
}

void CUAKMStasisModule::Dispose()
{
	CFullScreenModule::Dispose();

	if (_anim1 != NULL)
	{
		delete[] _anim1;
		_anim1 = NULL;
	}

	if (_anim2 != NULL)
	{
		delete[] _anim2;
		_anim2 = NULL;
	}
}

void CUAKMStasisModule::Render()
{
	if (CAnimationController::HasAnim())
	{
		CAnimationController::UpdateAndRender();
	}

	if (!_inputEnabled && (!CAnimationController::HasAnim() || CAnimationController::IsDone()))
	{
		if (_nextAudio > 0)
		{
			CAnimationController::Load(L"STASISW.AP", _nextAudio);
			_caption.SetText(_nextAudio == 2 ? SliderWarning : ButtonWarning);
			_nextAudio = 0;
		}
		else
		{
			_inputEnabled = TRUE;
			_caption.SetText(L"");
		}
	}

	if (_currentPage == 1)
	{
		// Sliding chamber panel out
		SlideOut();
	}
	else if (_movingSlider > 0)
	{
		int y = max(min((_cursorPosY / _scale) - _top - 9, 252), 142);
		int* pSliderValue = (_movingSlider == 1) ? &_temperature : &_oxygen;
		int moved = (*pSliderValue - y);
		if (moved != 0)
		{
			*pSliderValue = y;

			// Render slider background, then render slider at correct position
			int area, x, s, upper, lower;
			if (_movingSlider == 1)
			{
				area = IMG_LEFT_SLIDER_AREA;
				x = 108;
				s = IMG_LEFT_SLIDER;
				upper = Progression[_stage].Slider2Maximum;
				lower = Progression[_stage].Slider2Minimum;
			}
			else
			{
				area = IMG_RIGHT_SLIDER_AREA;
				x = 446;
				s = IMG_RIGHT_SLIDER;
				upper = Progression[_stage].Slider1Maximum;
				lower = Progression[_stage].Slider1Minimum;
			}

			RenderItem(area, x, 142);
			RenderItem(s, x, y);

			UpdateTexture();

			CheckStatus(y, lower, upper);
		}
	}

	if ((_stasisSettings & (STASIS_SETTINGS_STATE_POWER | STASIS_SETTINGS_STATE_DYING_2)) == STASIS_SETTINGS_STATE_POWER && GetTickCount64() > _nextPulseChange)
	{
		_stasisSettings ^= STASIS_SETTINGS_PULSE;
		RenderControl(STASIS_CONTROL_PULSE, _stasisSettings & 1);

		UpdateTexture();

		_nextPulseChange = GetTickCount64() + 30 * ((3 - _stage / 4) * TIMER_SCALE);
	}

	if ((_stasisSettings & (STASIS_SETTINGS_STATE_DYING_1 | STASIS_SETTINGS_STATE_DYING_2)) != 0 && GetTickCount64() > _nextWarningChange && _exitAfterTime == 0)
	{
		_stasisSettings ^= STASIS_SETTINGS_WARNING_ON;
		RenderControl(STASIS_CONTROL_WARNING, (_stasisSettings & STASIS_SETTINGS_WARNING_ON) ? 0 : 1);

		UpdateTexture();

		_nextWarningChange = GetTickCount64() + 500;
	}


	if (_anim != NULL && GetTickCount64() >= _nextAnimFrameTime)
	{
		if (!_animPlayer.DecodeFrame(_screen, 510, 133, 640))
		{
			_badAnimCounter++;
			if (_anim == _anim1)
			{
				if (_badAnimCounter < 6 && (_stasisSettings & (STASIS_SETTINGS_STATE_DYING_1 | STASIS_SETTINGS_STATE_DYING_2)))
				{
					// If status is NOT OK, play anim again, otherwise stop
					_animPlayer.Init(_anim1);
				}
				else
				{
					_stasisSettings &= ~STASIS_SETTINGS_STATE_DYING_1;
					_nextWarningChange = 0;
					RenderControl(STASIS_CONTROL_WARNING, 0);
					_anim = NULL;
				}
			}
			else if (_anim == _anim2)
			{
				// Death, exit
				_stasisSettings |= (STASIS_SETTINGS_FAILED_ON | STASIS_SETTINGS_STATE_FAILURE);
				RenderControl(STASIS_CONTROL_FAILED, 1);
				RenderControl(STASIS_CONTROL_PULSE, 0);
				_exitAfterTime = GetTickCount64() + 2000;
				_inputEnabled = FALSE;
				_anim = NULL;
			}
		}

		UpdateTexture();

		_nextAnimFrameTime = GetTickCount64() + 100;
	}

	if (_vertexBuffer != NULL)
	{
		dx.Clear(0.0f, 0.0f, 0.0f);

		dx.DisableZBuffer();

		UINT stride = sizeof(TEXTURED_VERTEX);
		UINT offset = 0;
		dx.SetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
		dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		CShaders::SelectOrthoShader();
		XMMATRIX wm = XMMatrixIdentity();
		CConstantBuffers::SetWorld(dx, &wm);
		ID3D11ShaderResourceView* pRV = _texture.GetTextureRV();
		dx.SetShaderResources(0, 1, &pRV);
		dx.Draw(4, 0);

		_caption.Render(0.0f, dx.GetHeight() - _caption.Height() - 10.0f);

		if (_inputEnabled)
		{
			CShaders::SelectOrthoShader();
			dx.SetVertexBuffers(0, 1, &_iconVertexBuffer, &stride, &offset);
			dx.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			wm = XMMatrixTranslation(_cursorPosX + 2.0f, -_cursorPosY + 2.0f, -0.5f);
			CConstantBuffers::SetWorld(dx, &wm);
			pRV = _iconTexture.GetTextureRV();
			dx.SetShaderResources(0, 1, &pRV);
			dx.Draw(4, 0);
		}

		dx.EnableZBuffer();

		dx.Present(1, 0);
	}

	if (_exitAfterTime > 0 && GetTickCount64() >= _exitAfterTime)
	{
		Exit();
	}
}

void CUAKMStasisModule::KeyDown(WPARAM key, LPARAM lParam)
{
	if (_inputEnabled)
	{
		CheckKeyAction(key, StasisControlCoordinates, sizeof(StasisControlCoordinates) / sizeof(ControlCoordinates));
	}
}

void CUAKMStasisModule::Initialize()
{
	CFullScreenModule::Initialize();

	_cursorPosX = dx.GetWidth() / 2;
	_cursorPosY = dx.GetHeight() / 2;

	// Load items from STASIS.AP (palette & images, animation?) and STASISW.AP (sound)

	_cursorMinX = _left + 87 * _scale;
	_cursorMaxX = _left + 478 * _scale;
	_cursorMinY = _top + 47 * _scale;
	_cursorMaxY = _top + 269 * _scale;

	// Load palette and compressed image
	DoubleData dd = LoadDoubleEntry(L"STASIS.AP", 0);
	if (dd.File1.Data != NULL)
	{
		_screen = dd.File2.Data;

		LPBYTE pPal = dd.File1.Data;
		for (int c = 0; c < 256; c++)
		{
			double r = pPal[c * 3 + 0];
			double g = pPal[c * 3 + 1];
			double b = pPal[c * 3 + 2];
			int ri = (byte)((r * 255.0) / 63.0);
			int gi = (byte)((g * 255.0) / 63.0);
			int bi = (byte)((b * 255.0) / 63.0);
			int col = 0xff000000 | bi | (gi << 8) | (ri << 16);
			_palette[c] = col;
		}

		CopyMemory(_originalPalette, _palette, sizeof(int) * 256);

		delete[] pPal;
	}

	_stasisSettings = (CGameController::GetData(UAKM_SAVE_STASIS_SETTINGS) | CGameController::GetData(UAKM_SAVE_STASIS_SETTINGS + 1) << 8) & 0xc03e;
	_stasisStages = (CGameController::GetData(UAKM_SAVE_STASIS_STAGES) | CGameController::GetData(UAKM_SAVE_STASIS_STAGES + 1) << 8);

	BinaryData bd = LoadEntry(L"STASIS.AP", 2);
	if (bd.Data != NULL)
	{
		_data = bd.Data;
		int count = GetInt(_data, 0, 2) - 1;
		for (int i = 0; i < count; i++)
		{
			_files[i] = _data + GetInt(_data, 2 + i * 4, 4);
		}
	}

	BinaryData ad = LoadEntry(L"STASIS.AP", 3);
	_anim1 = ad.Data;
	ad = LoadEntry(L"STASIS.AP", 4);
	_anim2 = ad.Data;

	CreateTexturedRectangle(0.0f, 0.0f, -16.0f, 16.0f, &_iconVertexBuffer, "CodePanelIconVertexBuffer");
	_iconTexture.Init(_files[IMG_HAND], 0, 0, &_palette[0], 0, "CodePanelIconTexture");

	_temperature = TemperatureStagePositions[0];
	_oxygen = OxygenStagePositions[0];
	RenderItem(IMG_LEFT_SLIDER, 108, _temperature);
	RenderItem(IMG_RIGHT_SLIDER, 446, _oxygen);

	RenderItem(IMG_GAGUE_INDICATOR, 199, 232, -1, -1, -1, -1, 0);

	UpdateTexture();

	_caption.SetColours(0xff000000, 0xff00c300, 0xff24ff00, 0xff000000);
	if (_stasisStages == 0)
	{
		_caption.SetText(Entry);

		CAnimationController::Load(L"STASISW.AP", 0);
		CAnimationController::UpdateAndRender();
	}

	_badEventCount = 0;
}

void CUAKMStasisModule::PowerOn()
{
	if ((_stasisSettings & STASIS_SETTINGS_POWER_ON) == 0)
	{
		RenderControl(STASIS_CONTROL_ONOFF, 1);

		_currentPage = 1;
		_currentFrame = 0;
		_frameTime = GetTickCount64();

		int stage = _stasisStages;
		_stage = 0;
		while (stage != 0)
		{
			int x1 = ProgressLightsXOffsets[_stage];
			Fill(x1, 383, x1 + 20, 404, STASIS_PROGRESS_LIGHT_COLOUR);
			_stage++;
			stage >>= 1;
		}

		_nextPulseChange = GetTickCount64() + 30 * ((3 - _stage / 4) * TIMER_SCALE);

		// Render sliders at correct positions for current stage
		RenderItem(IMG_LEFT_SLIDER_AREA, 108, 142);
		RenderItem(IMG_RIGHT_SLIDER_AREA, 446, 142);

		_temperature = TemperatureStagePositions[_stage];
		_oxygen = OxygenStagePositions[_stage];
		RenderItem(IMG_LEFT_SLIDER, 108, _temperature);
		RenderItem(IMG_RIGHT_SLIDER, 446, _oxygen);

		if (_stage >= 4)
		{
			RenderControl(STASIS_CONTROL_PHASE1, 1);
		}
		else if (_stage >= 8)
		{
			RenderControl(STASIS_CONTROL_PHASE2, 1);
		}
		else if (_stage >= 12)
		{
			RenderControl(STASIS_CONTROL_PHASE3, 1);
		}
	}
	else
	{
		PowerOnContinue();
	}

	UpdateTexture();
}

void CUAKMStasisModule::PowerOnContinue()
{
	RenderChamber();

	if ((_stasisSettings & STASIS_SETTINGS_STATE_FAILURE) == 0)
	{
		// TODO: Update blink rate
	}

	UpdateTexture();
}

void CUAKMStasisModule::PowerOff()
{
	_stasisSettings &= 0xfffe;
	RenderChamber();

	if ((_stasisSettings & STASIS_SETTINGS_UNKNOWN2) != 0)
	{
		// Same as last OR
	}

	_stasisSettings &= 0xccff;
	_stasisSettings |= (STASIS_SETTINGS_STATE_FAILURE | STASIS_SETTINGS_FAILED_ON);

	RenderControl(STASIS_CONTROL_ONOFF, 0);

	UpdateTexture();
}

void CUAKMStasisModule::SlideOut()
{
	ULONGLONG delta = GetTickCount64() - _frameTime;

	if (delta > TIMER_SCALE)
	{
		RenderItem(0, 403 + _currentFrame, 119, 504, 605, -1, -1, 0);
		UpdateTexture();
		_currentFrame += 4;
		if (_currentFrame >= 101)
		{
			_currentPage = 0;
			_currentFrame = 0;
			PowerOnContinue();
		}

		_frameTime += GetTickCount64();
	}

	/*
	Slide out chamber
	StasisSettings|=0x80 (both current and savegame)
	CGameController::SetHintState(920,1,1)	// Not sure if this should be halved...
	*/
}

void CUAKMStasisModule::RenderChamber()
{
	int img = IMG_BED_EMPTY;
	if ((_stasisSettings & STASIS_SETTINGS_STATE_POWER) != 0)
	{
		// Power is on
		if ((_stasisSettings & (STASIS_SETTINGS_STATE_FAILURE | STASIS_SETTINGS_FAILED_ON)) != 0)
		{
			// Dead
			img = IMG_BED_HEAD_SIDE;
		}
		else
		{
			img = IMG_BED_HEAD_FRONT;
		}
	}

	RenderItem(img, 503, 119, -1, -1, -1, -1, 0);
}

void CUAKMStasisModule::RenderControl(int index, int state)
{
	RenderItem(StasisControls[index].ImageIndex + state, StasisControls[index].X, StasisControls[index].Y);
}

void CUAKMStasisModule::OnAction(int action)
{
	if (action == ACTION_STASIS_POWER)
	{
		// Clear caption
		_caption.SetText("");

		_stasisSettings ^= STASIS_SETTINGS_STATE_POWER;
		if ((_stasisSettings & STASIS_SETTINGS_STATE_POWER) == 0)
		{
			PowerOff();
		}
		else
		{
			PowerOn();
		}
	}
	else if (action == ACTION_ESCAPE || action == ACTION_STASIS_EXIT)
	{
		Exit();
	}
	else if (action == ACTION_STASIS_HINT)
	{

	}
	else if ((_stasisSettings & STASIS_SETTINGS_STATE_POWER) != 0)
	{
		if (action == ACTION_STASIS_TEMPERATURE)
		{
			_cursorMinX = _left + 108 * _scale;
			_cursorMaxX = _left + 125 * _scale;
			_cursorMinY = _top + 142 * _scale;
			_cursorMaxY = _top + 269 * _scale;

			_inputEnabled = FALSE;
			_movingSlider = 1;
		}
		else if (action == ACTION_STASIS_PENTAHOL)
		{
			Button(1);
		}
		else if (action == ACTION_STASIS_BICARB)
		{
			Button(2);
		}
		else if (action == ACTION_STASIS_ADRENALINE)
		{
			Button(3);
		}
		else if (action == ACTION_STASIS_SHOCK)
		{
			Button(4);
		}
		else if (action == ACTION_STASIS_OXYGEN)
		{
			_cursorMinX = _left + 446 * _scale;
			_cursorMaxX = _left + 463 * _scale;
			_cursorMinY = _top + 142 * _scale;
			_cursorMaxY = _top + 269 * _scale;
			_inputEnabled = FALSE;
			_movingSlider = 2;
		}
	}
}

void CUAKMStasisModule::Button(int btn)
{
	_badSlider = 0;
	if (Progression[_stage].ControlToChange == btn)
	{
		// Good
		Progress();
	}
	else
	{
		// Bad
		Bad(FALSE);
	}
}

void CUAKMStasisModule::Exit()
{
	int result = 2;
	if ((_stasisSettings & STASIS_SETTINGS_PHASE_3_ON) != 0)
	{
		result = 1;
	}
	else if ((_stasisSettings & (STASIS_SETTINGS_STATE_FAILURE | STASIS_SETTINGS_FAILED_ON | STASIS_SETTINGS_STATE_DYING_2)) != 0)
	{
		result = 3;
	}

	// Set progress back on save game
	CGameController::SetData(UAKM_SAVE_STASIS_SETTINGS, _stasisSettings & 0xff);
	CGameController::SetData(UAKM_SAVE_STASIS_SETTINGS + 1, (_stasisSettings >> 8) & 0xff);
	CGameController::SetData(UAKM_SAVE_STASIS_STAGES, _stasisStages & 0xff);
	CGameController::SetData(UAKM_SAVE_STASIS_STAGES + 1, (_stasisStages >> 8) & 0xff);

	CGameController::SetParameter(_parameter, result);
	CModuleController::Pop(this);
}

void CUAKMStasisModule::CheckStatus(int y, int lower, int upper)
{
	if ((_stasisSettings & (STASIS_SETTINGS_UNKNOWN2 | STASIS_SETTINGS_STATE_FAILURE)) == 0 && _stage < 12)
	{
		if (_temperature <= Progression[_stage].Slider1Maximum && _temperature >= Progression[_stage].Slider1Minimum && _oxygen <= Progression[_stage].Slider2Maximum && _oxygen >= Progression[_stage].Slider2Minimum)
		{
			_stasisSettings &= ~STASIS_SETTINGS_STATE_DYING_1;
		}

		if ((Progression[_stage].ControlToChange == 0 && abs(_temperature - Progression[_stage].SliderCheckPosition) <= 2) || (Progression[_stage].ControlToChange == 5 && abs(_oxygen - Progression[_stage].SliderCheckPosition) <= 2))
		{
			// Progress to next stage, check if text/sound should be output
			Progress();
		}
		else if (_badSlider != _movingSlider && ((y + 9) < lower || (y - 9) > upper))
		{
			_badSlider = _movingSlider;
			Bad(TRUE);
		}
	}
}

void CUAKMStasisModule::Progress()
{
	_badEventCount = 0;
	_stasisSettings &= ~STASIS_SETTINGS_STATE_DYING_1;	// Warning off
	_badSlider = 0;

	int mask = 1 << Progression[_stage].Stage;
	if ((_stasisStages & mask) == 0)
	{
		_stasisStages |= mask;

		if (_stasisStages == 1)
		{
			// Set caption and play sample
			_caption.SetText(GoodAction);
			CAnimationController::Load(L"STASISW.AP", 4);
			CAnimationController::UpdateAndRender();
			_inputEnabled = FALSE;
			_movingSlider = 0;
			_cursorMinX = _left + 87 * _scale;
			_cursorMaxX = _left + 478 * _scale;
			_cursorMinY = _top + 47 * _scale;
			_cursorMaxY = _top + 269 * _scale;
		}

		_stasisSettings |= Progression[_stage].Setting;

		int x1 = ProgressLightsXOffsets[_stage];
		Fill(x1, 383, x1 + 20, 404, STASIS_PROGRESS_LIGHT_COLOUR);
		_stage++;

		if (_stage == 4)
		{
			RenderControl(STASIS_CONTROL_PHASE1, 1);
		}
		else if (_stage == 8)
		{
			RenderControl(STASIS_CONTROL_PHASE2, 1);
		}
		else if (_stage == 12)
		{
			RenderControl(STASIS_CONTROL_PHASE3, 1);
			_exitAfterTime = GetTickCount64() + 2000;
		}

		UpdateTexture();
	}
}

void CUAKMStasisModule::Bad(BOOL slider)
{
	LPBYTE pAnim = NULL;
	if (_badEventCount == 0)
	{
		pAnim = _anim1;
		_stasisSettings |= (STASIS_SETTINGS_WARNING_ON | STASIS_SETTINGS_STATE_DYING_1);
		_nextWarningChange = GetTickCount64() + 500;

		if (!_badPlayed)
		{
			_inputEnabled = FALSE;
			CAnimationController::Load(L"STASISW.AP", 1);

			if (slider)
			{
				// Play 2 sounds
				_caption.SetText(SliderWarning);
				_nextAudio = 2;
			}
			else
			{
				// Button, play only one sound
				_caption.SetText(ButtonWarning);
				_nextAudio = 3;
			}

			CAnimationController::UpdateAndRender();
			_badPlayed = TRUE;
			MouseUp({ 0,0 }, -1);
		}
	}
	else
	{
		if (_anim != NULL)
		{
			_animPlayer.Merge(_anim2);
			_anim = _anim2;
		}
		else
		{
			pAnim = _anim2;
		}
		_inputEnabled = FALSE;

		_stasisSettings |= (STASIS_SETTINGS_WARNING_ON | STASIS_SETTINGS_STATE_DYING_1 | STASIS_SETTINGS_FAILED_ON | STASIS_SETTINGS_STATE_DYING_2);
		_nextWarningChange = GetTickCount64() + 500;
	}

	_badEventCount++;

	//_stasisSettings |= STASIS_SETTINGS_WARNING_ON;	// Warning on
	// TODO: Start animation (could be dying)

	if (pAnim != NULL)
	{
		_anim = pAnim;
		_badAnimCounter = 0;
		_animPlayer.Init(pAnim);
		_nextAnimFrameTime = GetTickCount64() + 100;
	}

	// 2 bad events in a row causes death
}

void CUAKMStasisModule::BeginAction()
{
	if (_inputEnabled)
	{
		CheckMouseAction(StasisControlCoordinates, sizeof(StasisControlCoordinates) / sizeof(ControlCoordinates));
	}
	else if (CAnimationController::HasAnim())
	{
		CAnimationController::Skip();
	}
}

void CUAKMStasisModule::EndAction()
{
	if (_movingSlider > 0)
	{
		_cursorMinX = _left + 87 * _scale;
		_cursorMaxX = _left + 478 * _scale;
		_cursorMinY = _top + 47 * _scale;
		_cursorMaxY = _top + 269 * _scale;

		_movingSlider = 0;
	}
}

void CUAKMStasisModule::Back()
{
	Exit();
}
