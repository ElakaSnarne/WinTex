#include "ModuleController.h"
#include "Globals.h"
#include "AmbientAudio.h"
#include "Gamepad.h"

CModuleBase* CModuleController::CurrentModule = NULL;
CModuleBase* CModuleController::NextModule = NULL;
std::list<CModuleBase*> CModuleController::Modules;

CAnimatedCursor CModuleController::Cursors[12];

DWORD CModuleController::MainThreadId = 0;
DWORD CModuleController::D3DThreadId = 0;
DWORD CModuleController::TimerThreadId = 0;

CMap* CModuleController::pMap = NULL;
CMap* CModuleController::pDMap = NULL;

BOOL CModuleController::Init(CMap* map, CMap* dmap)
{
	if (map != NULL && dmap != NULL)
	{
		pMap = map;
		pDMap = dmap;
		return (map->Init() && dmap->Init());
	}

	return FALSE;
}

void CModuleController::Push(CModuleBase* pModule, BOOL overrideCurrent)
{
	_lock.Lock();

	NextModule = pModule;
	Modules.push_front(pModule);
	pModule->Init();

	_lock.Release();
}

void CModuleController::Pop(CModuleBase* pModule)
{
	_lock.Lock();

	if (pModule != NULL)
	{
		Modules.remove(pModule);

		if (CurrentModule == pModule)
		{
			if (Modules.size() > 0)
			{
				CurrentModule = Modules.front();
				ResumeModule(CurrentModule);
			}
		}

		//ModulesToDelete.push_back(pModule);
		delete pModule;
	}

	_lock.Release();
}

void CModuleController::SendToBack(CModuleBase* pModule)
{
	if (pModule != NULL)
	{
		Modules.remove(pModule);
		if (Modules.size() > 0)
		{
			pModule->Pause();
			Modules.push_back(pModule);
			CurrentModule = Modules.front();
			CurrentModule->Resume();
		}
		else
		{
			PostQuitMessage(0);
		}
	}
}

void CModuleController::SendToFront(CModuleBase* pModule)
{
	if (pModule != NULL)
	{
		PauseModule(CurrentModule);
		Modules.remove(pModule);
		Modules.push_front(pModule);
		CurrentModule = pModule;
		CurrentModule->Resume();
	}
}

void CModuleController::Render()
{
	_lock.Lock();

	if (NextModule != NULL)
	{
		PauseModule(CurrentModule);

		// TODO: Only allow one module of each type simultaneously
		std::list<CModuleBase*> modulesToDelete;
		for (auto it : Modules)
		{
			if (it->Type == NextModule->Type && it != NextModule)
			{
				modulesToDelete.push_front(it);
			}
		}

		for (auto it : modulesToDelete)
		{
			Modules.remove(it);
			delete it;
		}

		CurrentModule = NextModule;
		//Modules.push_front(NextModule);
		NextModule = NULL;
	}

	if (CurrentModule != NULL)
	{
		CurrentModule->Render();
		CurrentModule->CheckInput();
	}

	_lock.Release();
}

void CModuleController::MouseMove(POINT pt)
{
	CInputMapping::Input(InputSource::Mouse, 0, ((pt.x & 0xffff) << 16) | (pt.y & 0xffff));
	if (CurrentModule != NULL)
	{
		CurrentModule->MouseMove(pt);
	}
}

void CModuleController::MouseDown(POINT pt, int btn)
{
	CInputMapping::Input(InputSource::MouseButton, btn, 1);
	if (CurrentModule != NULL)
	{
		CurrentModule->MouseDown(pt, btn);
	}
}

void CModuleController::MouseUp(POINT pt, int btn)
{
	CInputMapping::Input(InputSource::MouseButton, btn, 0);
	if (CurrentModule != NULL)
	{
		CurrentModule->MouseUp(pt, btn);
	}
}

void CModuleController::MouseWheel(int scroll)
{
	CInputMapping::Input(InputSource::MouseWheel, scroll > 0 ? 1 : -1, 1);
	if (CurrentModule != NULL)
	{
		CurrentModule->MouseWheel(scroll);
	}
}

void CModuleController::KeyDown(WPARAM key, LPARAM lParam)
{
	CInputMapping::Input(InputSource::Key, lParam & 0xff0000, 1);
	if (CurrentModule != NULL)
	{
		CurrentModule->KeyDown(key, lParam);
	}
}

void CModuleController::KeyUp(WPARAM key, LPARAM lParam)
{
	CInputMapping::Input(InputSource::Key, lParam & 0xff0000, 0);
	if (CurrentModule != NULL)
	{
		CurrentModule->KeyUp(key, lParam);
	}
}

void CModuleController::GamepadInput(InputSource source, int offset, int data)
{
	CInputMapping::Input(source, offset, data);
	if (CurrentModule != NULL)
	{
		CurrentModule->GamepadInput(source, offset, data);
	}
}

void CModuleController::PauseModule(CModuleBase* pModule)
{
	if (pModule != NULL)
	{
		pModule->Pause();
	}
}

void CModuleController::ResumeModule(CModuleBase* pModule)
{
	if (pModule != NULL)
	{
		pModule->Resume();
	}
}

CModuleBase* CModuleController::Get(ModuleType type)
{
	CModuleBase* pRet = NULL;

	_lock.Lock();

	for (auto it : Modules)
	{
		if (it->Type == type)
		{
			pRet = it;
			break;
		}
	}

	_lock.Release();

	return pRet;
}

void CModuleController::ClearExcept(CModuleBase* pModule)
{
	_lock.Lock();

	for (auto it : Modules)
	{
		if (it != pModule && it != CurrentModule)
		{
			delete it;
		}
	}

	Modules.clear();
	Modules.push_back(pModule);
	if (CurrentModule != NULL)
	{
		Modules.push_back(CurrentModule);
	}
	CurrentModule = pModule;

	_lock.Release();
}

void CModuleController::Resize(int width, int height)
{
	_lock.Lock();

	for (auto it : Modules)
	{
		it->Resize(width, height);
	}

	for (int c = 0; c < 12; c++)
	{
		Cursors[c].SetPosition(width / 2.0f, height / 2.0f);
	}

	for (int i = 0; i < 3; i++)
	{
		DialogueOptions[i].Resize(i, width, height);
	}

	_lock.Release();
}

void CModuleController::Resume()
{
	_lock.Lock();

	if (CurrentModule != NULL)
	{
		CurrentModule->Resume();
	}

	_lock.Release();
}
