#include "UAKMPusShellGameModule.h"
#include "ModuleController.h"
#include "GameController.h"
#include "AnimationController.h"
#include "MainMenuModule.h"

int Coordinates[] = { 106,129,128,153,57,78,262,284,106,130,237,261,201,230,234,263 };

CUAKMPusShellGameModule::CUAKMPusShellGameModule(int parameter, int correctAnswer) : CModuleBase(ModuleType::Ferrelette)
{
	_parameter = parameter;
	_correctAnswer = correctAnswer;

	float w = (float)dx.GetWidth();
	float h = (float)dx.GetHeight();
	float sx = w / (float)CAnimationController::Width();
	float sy = h / (float)CAnimationController::Height();

	_scale = min(sx, sy);
	float sw = (float)CAnimationController::Width() * _scale;
	float sh = (float)CAnimationController::Height() * _scale;

	_top = -(h - sh) / 2.0f;
	_left = (w - sw) / 2.0f;

	_cursorMinX = static_cast<int>(_left);
	_cursorMaxX = static_cast<int>(CAnimationController::Width() * _scale);
	_cursorMinY = static_cast<int>(_top);
	_cursorMaxY = static_cast<int>(CAnimationController::Height() * _scale);
}

CUAKMPusShellGameModule::~CUAKMPusShellGameModule()
{
}

void CUAKMPusShellGameModule::Resize(int width, int height)
{
}

void CUAKMPusShellGameModule::Dispose()
{
}

void CUAKMPusShellGameModule::Render()
{
	dx.Clear(0.0f, 0.0f, 0.0f);
	dx.DisableZBuffer();

	CAnimationController::UpdateAndRender();

	CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
	CModuleController::Cursors[0].Render();

	dx.EnableZBuffer();

	dx.Present(1, 0);
}

void CUAKMPusShellGameModule::Pause()
{
}

void CUAKMPusShellGameModule::Resume()
{
}

void CUAKMPusShellGameModule::Initialize()
{
	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;
}

void CUAKMPusShellGameModule::BeginAction()
{
	// Coordinates need to be scaled and offset
	int x = 8 + static_cast<int>((_cursorPosX - _left) / _scale);
	int y = 6 + static_cast<int>((_cursorPosY - _top) / _scale);

	// If over a ball, set parameter to 0 or 1 (if the ID matches correct answer), then pop module
	for (int i = 0; i < 4; i++)
	{
		if (y >= Coordinates[i * 4] && y < Coordinates[i * 4 + 1] && x >= Coordinates[i * 4 + 2] && x < Coordinates[i * 4 + 3])
		{
			CGameController::SetParameter(_parameter, ((i + 1) == _correctAnswer) ? 1 : 0);
			CModuleController::Pop(this);
			break;
		}
	}
}

void CUAKMPusShellGameModule::Back()
{
	CModuleController::SendToFront(CMainMenuModule::MainMenuModule);
}
