#pragma once

#include "FullScreenModule.h"
#include <unordered_map>
#include "DXText.h"
#include "SimpleAnimPlayer.h"

#define IMG_BED_EMPTY			0
#define IMG_BED_HEAD_FRONT		1
#define IMG_BED_HEAD_SIDE		2
#define IMG_PULSE_LIGHT_OFF		3
#define IMG_PULSE_LIGHT_ON		4
#define IMG_CRYO_LIGHT_OFF		5
#define IMG_CRYO_LIGHT_ON		6
#define IMG_VITAL_LIGHT_OFF		7
#define IMG_VITAL_LIGHT_ON		8
#define IMG_FAIL_LIGHT_OFF		9
#define IMG_FAIL_LIGHT_ON		10
#define IMG_PHASE1_LIGHT_OFF	11
#define IMG_PHASE1_LIGHT_ON		12
#define IMG_PHASE2_LIGHT_OFF	13
#define IMG_PHASE2_LIGHT_ON		14
#define IMG_PHASE3_LIGHT_OFF	15
#define IMG_PHASE3_LIGHT_ON		16
#define IMG_LEFT_SLIDER			17
#define IMG_LEFT_SLIDER_AREA	18
#define IMG_RIGHT_SLIDER_AREA	19
#define IMG_GAGUE				20
#define IMG_GAGUE_INDICATOR		21
#define IMG_HAND				22
#define IMG_PENTATHOL_DOWN		23
#define IMG_PENTATHOL_UP		24
#define IMG_BICARB_DOWN			25
#define IMG_BICARB_UP			26
#define IMG_ADRENALINE_DOWN		27
#define IMG_ADRENALINE_UP		28
#define IMG_SHOCK_DOWN			29
#define IMG_SHOCK_UP			30
#define IMG_OFF					31
#define IMG_ON					32
#define IMG_RIGHT_SLIDER		33
#define IMG_HINT_DOWN			34
#define IMG_HINT_UP				35
#define IMG_EXIT_DOWN			36
#define IMG_EXIT_UP				37

#define STASIS_CONTROL_PULSE		0
#define STASIS_CONTROL_CRYO			1
#define STASIS_CONTROL_WARNING		2
#define STASIS_CONTROL_FAILED		3
#define STASIS_CONTROL_PHASE1		4
#define STASIS_CONTROL_PHASE2		5
#define STASIS_CONTROL_PHASE3		6
#define STASIS_CONTROL_ONOFF		7
#define STASIS_CONTROL_PENTATHOL	8
#define STASIS_CONTROL_BICARB		9
#define STASIS_CONTROL_ADRENALINE	10
#define STASIS_CONTROL_SHOCK		11
#define STASIS_CONTROL_HINT			12
#define STASIS_CONTROL_EXIT			13

// Settings
#define STASIS_SETTINGS_PULSE			0x0001
#define STASIS_SETTINGS_PHASE_1_ON		0x0002
#define STASIS_SETTINGS_PHASE_2_ON		0x0004
#define STASIS_SETTINGS_PHASE_3_ON		0x0008
#define STASIS_SETTINGS_CRYO_ON			0x0010
#define STASIS_SETTINGS_WARNING_ON		0x0020
#define STASIS_SETTINGS_FAILED_ON		0x0040
#define STASIS_SETTINGS_POWER_ON		0x0080
#define STASIS_SETTINGS_UNKNOWN1		0x0100
#define STASIS_SETTINGS_UNKNOWN2		0x0200
#define STASIS_SETTINGS_STATE_FAILURE	0x0400
#define STASIS_SETTINGS_STATE_POWER		0x0800
#define STASIS_SETTINGS_STATE_DYING_1	0x1000
#define STASIS_SETTINGS_STATE_DYING_2	0x2000

#define KEYCODE_ESCAPE		0x01
#define KEYCODE_UP			0x48
#define KEYCODE_LEFT		0x4b
#define KEYCODE_RIGHT		0x4d
#define KEYCODE_DOWN		0x50

#define KEYCODE_PLUS		0x4e
#define KEYCODE_MINUS		0x4a
#define KEYCODE_PAGEUP		0x49
#define KEYCODE_PAGEDOWN	0x51
#define KEYCODE_H			0x23

#define KEYCODE_NONE		0xfe

#define ACTION_UP					0
#define ACTION_DOWN					1
#define ACTION_LEFT					2
#define ACTION_RIGHT				3
#define ACTION_ESCAPE				4
#define ACTION_PLUS					5
#define ACTION_MINUS				6
#define ACTION_PAGEUP				7
#define ACTION_PAGEDOWN				8

#define ACTION_STASIS_EXIT			0x8001
#define ACTION_STASIS_POWER			0x8002
#define ACTION_STASIS_TEMPERATURE	0x8003
#define ACTION_STASIS_PENTAHOL		0x8004
#define ACTION_STASIS_BICARB		0x8005
#define ACTION_STASIS_ADRENALINE	0x8006
#define ACTION_STASIS_SHOCK			0x8007
#define ACTION_STASIS_OXYGEN		0x8008
#define ACTION_STASIS_HINT			0x8009
#define ACTION_STASIS_OTHER_CLICK	0x8010

#define STASIS_PROGRESS_LIGHT_COLOUR	151

struct StasisProgression
{
	int ControlToChange;
	int SliderCheckPosition;
	int Slider2Minimum;
	int Slider2Maximum;
	int Slider1Minimum;
	int Slider1Maximum;
	int Stage;
	int Setting;
	int Hint;
};

class CUAKMStasisModule : public CFullScreenModule
{
public:
	CUAKMStasisModule(int parameter);
	virtual ~CUAKMStasisModule();

	virtual void Dispose();
	virtual void Render();

	virtual void KeyDown(WPARAM key, LPARAM lParam);

protected:
	virtual void Initialize();

	int _parameter;

	int _stasisSettings;
	int _stasisStages;
	int _stage;

	void RenderControl(int index, int state);

	virtual void OnAction(int action);

	CDXText _caption;

	void PowerOn();
	void PowerOnContinue();
	void PowerOff();
	void SlideOut();
	void RenderChamber();
	void Exit();
	void Button(int btn);

	int _heartRate;

	int _movingSlider;
	int _temperature;
	int _oxygen;
	ULONGLONG _nextPulseChange;

	void CheckStatus(int y, int lower, int upper);
	void Progress();
	void Bad(BOOL slider);

	int _badEventCount;
	CSimpleAnimPlayer _animPlayer;
	LPBYTE _anim;
	LPBYTE _anim1;
	LPBYTE _anim2;
	ULONGLONG _nextAnimFrameTime;
	int _badAnimCounter;

	int _nextAudio;
	BOOL _badPlayed;
	ULONGLONG _exitAfterTime;
	ULONGLONG _nextWarningChange;
	int _badSlider;

	// Input related
	virtual void BeginAction();
	virtual void EndAction();
	virtual void Back();
};
