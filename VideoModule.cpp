#include "VideoModule.h"
#include "Utilities.h"
#include "Globals.h"
#include "GameBase.h"
#include "AnimationController.h"
#include "GameController.h"
#include "MainMenuModule.h"

CVideoModule::CVideoModule(VideoType type, int dmapIndex, int activeScript) : CModuleBase(ModuleType::Video)
{
	Type = type;

	// Get DMAPData, copy script, reset pointer or set to active script
	_scriptEngine = CGameController::GetScriptEngine();
	_scriptEngine->_mapEntry = CModuleController::pDMap->Get(dmapIndex);
	std::wstring fileName = CGameController::GetFileName(_scriptEngine->_mapEntry->ScriptFileIndex);
	if (fileName != L"")
	{
		BinaryData bd = LoadEntry(fileName.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);
		_scriptState = CGameController::GetScriptState();
		_scriptState->Init(bd.Data, bd.Length, fileName.c_str(), _scriptEngine->_mapEntry->ScriptFileEntry);
		if (activeScript >= 0)
		{
			_scriptState->ExecutionPointer = _scriptState->GetScript(activeScript);
		}

		if (_scriptState->ExecutionPointer < 0)
		{
			_scriptState->ExecutionPointer = 0;
		}
	}
}

CVideoModule::CVideoModule(VideoType type, LPCWSTR fileName, int itemIndex) : CModuleBase(ModuleType::Video)
{
	Type = type;

	_scriptEngine = NULL;
	_scriptState = CGameController::GetScriptState();

	CAnimationController::Load(fileName, itemIndex);
}

CVideoModule::~CVideoModule()
{
	Dispose();
}

void CVideoModule::Initialize()
{
	_cursorPosX = dx.GetWidth() / 2.0f;
	_cursorPosY = dx.GetHeight() / 2.0f;

	DialogueOptions[0].SetClick(DialogueOptionA);
	DialogueOptions[1].SetClick(DialogueOptionB);
	DialogueOptions[2].SetClick(DialogueOptionC);

	if (_scriptEngine != NULL)
	{
		_scriptEngine->Resume(_scriptState);
	}
}

void CVideoModule::Dispose()
{
	if (_lock.Lock())
	{
		if (_scriptEngine != NULL)
		{
			delete _scriptEngine;
			_scriptEngine = NULL;
		}

		if (_scriptState != NULL && _scriptState->Script != NULL)
		{
			delete[] _scriptState->Script;
			_scriptState->Script = NULL;
		}

		_lock.Release();
	}
}

void CVideoModule::Render()
{
	if (CAnimationController::Exists())
	{
		dx.Clear(0.0f, 0.0f, 0.0f);

		CAnimationController::UpdateAndRender();

		if (_scriptState->WaitingForInput && CAnimationController::IsDone() && CAnimationController::IsDone())
		{
			if (DialogueOptionsCount >= 1)
			{
				DialogueOptions[0].Render();
			}
			if (DialogueOptionsCount >= 2)
			{
				DialogueOptions[1].Render();
			}
			if (DialogueOptionsCount >= 3)
			{
				DialogueOptions[2].Render();
			}

			if (_scriptState->AskAbout || _scriptState->Offer)
			{
				BOOL recreate = ((_scriptState->AskAbout && CGameController::AskAboutChanged) || (_scriptState->Offer && CGameController::ItemsChanged));

				if (_scriptState->TopItemOffset < 0 || recreate)
				{
					// TODO: Create new buffers...
					std::vector<ListBoxItem> items;
					if (_scriptState->AskAbout)
					{
						int askAboutCount = CGameController::GetAskAboutCount();
						for (int i = 0; i < 256 && i < askAboutCount; i++)
						{
							ListBoxItem lbi;
							lbi.Id = CGameController::GetAskAboutId(i);
							lbi.Text = CGameController::GetAskAboutName(lbi.Id);
							items.push_back(lbi);
						}

						CGameController::AskAboutChanged = FALSE;
					}
					else if (_scriptState->Offer)
					{
						int itemCount = CGameController::GetItemCount();
						for (int i = 0; i < 256 && i < itemCount; i++)
						{
							ListBoxItem lbi;
							lbi.Id = CGameController::GetItemId(i);
							lbi.Text = CGameController::GetItemName(lbi.Id);
							items.push_back(lbi);
						}

						CGameController::ItemsChanged = FALSE;
					}

					_scriptState->TopItemOffset = 0;
					_listBox.Init(items, DialogueOptions[0].GetX() + DialogueOptions[0].GetWidth() / 2.0f);
				}

				_listBox.Render();
			}

			// Render arrow cursor
			CModuleController::Cursors[0].SetPosition(_cursorPosX, _cursorPosY);
			CModuleController::Cursors[0].Render();
		}

		dx.Present(1, 0);
	}

	if (!CAnimationController::HasAnim() || CAnimationController::IsDone())
	{
		if (Type == VideoType::Single)
		{
			CAnimationController::Clear();
			CModuleController::Pop(this);
		}
		else if (Type == VideoType::Scripted)
		{
			if (_scriptState->WaitingForMediaToFinish)
			{
				_scriptEngine->Resume(_scriptState, TRUE);
			}
			else if (_scriptState->ExecutionPointer == -1)
			{
				CAnimationController::Clear();
				CModuleController::Pop(this);
				// TODO: Should only clear when truly done
			}
		}
	}
}

void CVideoModule::DialogueOptionA(LPVOID data)
{
	SelectOption(1);
}

void CVideoModule::DialogueOptionB(LPVOID data)
{
	SelectOption(2);
}

void CVideoModule::DialogueOptionC(LPVOID data)
{
	SelectOption(3);
}

void CVideoModule::SelectOption(int option)
{
	if (CModuleController::CurrentModule != NULL && CModuleController::CurrentModule->Type == ModuleType::Video)
	{
		((CVideoModule*)CModuleController::CurrentModule)->SelectDialogueOption(option);
	}
}

void CVideoModule::SelectDialogueOption(int option)
{
	_scriptState->TopItemOffset = -1;
	_scriptEngine->SelectDialogueOption(_scriptState, option);
}

// TODO: When asking about topics, may have to regenerate the ask about list, as things may have been added or even removed...

void CVideoModule::Resize(int width, int height)
{
	CAnimationController::Resize(width, height);
}

void CVideoModule::Resume()
{
	//if (_scriptState->WaitingForMediaToFinish)
	//{
	//	//_scriptEngine->Resume(_scriptState, TRUE);
	//}
	//else if (_scriptState->WaitingForInput)
	//{
	//	_scriptEngine->Resume(_scriptState, TRUE);
	//}
	if (_scriptState->WaitingForExternalModule)
	{
		_scriptEngine->Resume(_scriptState, TRUE);
	}
}

void CVideoModule::Cursor(float x, float y, BOOL relative)
{
	CModuleBase::Cursor(x, y, relative);

	if (_scriptState->WaitingForInput)
	{
		x = _cursorPosX;
		y = _cursorPosY;

		// Check mouse over options buttons (only the ones that are visible)
		for (int i = 0; i < 3; i++)
		{
			if (DialogueOptionsCount > i)
			{
				// Check if mouse is over button...
				DialogueOptions[i].SetMouseOver(DialogueOptions[i].HitTest(x, y) != NULL);
			}
		}

		if (_scriptState->AskAbout || _scriptState->Offer)
		{
			_listBox.HitTestLB(x, y);
		}
	}
}

void CVideoModule::BeginAction()
{
	if (CGameController::CanCancelVideo())
	{
		CAnimationController::Skip();

		float x = _cursorPosX;
		float y = _cursorPosY;

		if (_scriptState->WaitingForInput && (_scriptState->AskAbout || _scriptState->Offer))
		{
			int hitId = _listBox.HitTestLB(x, y);
			if (hitId >= 0)
			{
				// Set item id and option and resume script
				int option = _scriptState->AskAbout ? 4 : _scriptState->Offer ? 7 : -1;
				CGameController::SetParameter(99, hitId);
				_scriptState->SelectedOption = option;
				_scriptEngine->Resume(_scriptState, TRUE);
			}
		}

		for (int i = 0; i < 3; i++)
		{
			if (DialogueOptionsCount > i&& DialogueOptions[i].HitTest(x, y) != NULL)
			{
				DialogueOptions[i].Click();
				break;
			}
		}
	}
}

void CVideoModule::Back()
{
	CModuleController::SendToFront(CMainMenuModule::MainMenuModule);
}
