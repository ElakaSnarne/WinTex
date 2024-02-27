#include "PictureModule.h"
#include "Globals.h"
#include "Utilities.h"
#include "GameController.h"
#include "AnimationController.h"

CPictureModule::CPictureModule(int fileId, int entryIndex, CScriptBase* pScript, CScriptState* pScriptState) : CFullScreenModule(ModuleType::Picture)
{
	_script = pScript;
	_state = pScriptState;
	_image = NULL;
	_rendered = FALSE;

	// Load file and create buffer
	std::wstring fn = CGameController::GetFileName(fileId);
	if (fn != L"")
	{
		BinaryData bd = LoadEntry(fn.c_str(), entryIndex);
		if (bd.Data != NULL)
		{
			if (bd.Length > 0x304 && bd.Data[0x300] == 'D' && bd.Data[0x301] == 'B' && bd.Data[0x302] == 'E' && bd.Data[0x303] == 1)
			{
				// Combined, palette + compressed image
				BinaryData imageData = CLZ::Decompress(bd.Data + 0x300, bd.Length - 0x300);

				// Setup fullscreen image
				_image = CAnimationController::LoadImage(bd.Data, imageData, 640, 480);

				delete[] imageData.Data;
			}

			delete[] bd.Data;
		}
	}
}

CPictureModule::~CPictureModule()
{
	Dispose();
}

void CPictureModule::Render()
{
	// Allow one frame to render before resuming script
	if (_image != NULL && !_rendered)
	{
		_image->Render();

		_rendered = TRUE;
		_state->WaitingForMediaToFinish = FALSE;
	}
	else if (_state->WaitingForMediaToFinish)
	{
		if (CAnimationController::HasAnim() && !CAnimationController::IsDone())
		{
			CAnimationController::UpdateAndRender();
		}
		else
		{
			_state->WaitingForMediaToFinish = FALSE;
		}
	}
	else if (_script != NULL)
	{
		// Resume script
		_script->Resume(_state, TRUE);
	}
}

void CPictureModule::BeginAction()
{
	CAnimationController::Skip();
}
