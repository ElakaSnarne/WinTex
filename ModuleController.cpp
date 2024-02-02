#include "ModuleController.h"
#include "Globals.h"
#include "AmbientAudio.h"
#include "Gamepad.h"
#include "GameController.h"

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
		BOOL ret = (map->Init() && dmap->Init());

#ifdef DEBUGx
		// Output all information from maps
		int mapIndex = 0;
		CMapData* pMap = NULL;
		while ((pMap = map->Get(mapIndex)) != NULL)
		{
			Trace(L"DMap ");
			TraceLine(mapIndex++);
			Trace(L"Dialogue file: ");
			TraceLine(CGameController::GetFileName(pMap->LocationFileIndex).c_str());
			Trace(L"Script file: ");
			TraceLine(CGameController::GetFileName(pMap->ScriptFileIndex).c_str());
			Trace(L"Script entry: ");
			TraceLine(pMap->ScriptFileEntry);
			TraceLine(L"Animations");
			int ix = 0;
			for (auto anim : pMap->AnimationMap)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = ");
				TraceLine(anim);
			}
			TraceLine(L"Audio");
			ix = 0;
			for (auto audio : pMap->AudioMap)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = ");
				Trace(CGameController::GetFileName(audio.File).c_str());
				Trace(L" #");
				TraceLine(audio.Entry);
			}
			TraceLine(L"Environmental audio");
			ix = 0;
			for (auto audio : pMap->EnvironmentAudioMap)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = ");
				Trace(CGameController::GetFileName(audio.File).c_str());
				Trace(L" #");
				TraceLine(audio.Entry);
			}
			TraceLine(L"Video");
			ix = 0;
			for (auto video : pMap->VideoMap)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = ");
				Trace(CGameController::GetFileName(video.File).c_str());
				Trace(L" #");
				TraceLine(video.Entry);
			}
			TraceLine(L"Images");
			ix = 0;
			for (auto image : pMap->ImageMap)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = ");
				Trace(CGameController::GetFileName(image.File).c_str());
				Trace(L" #");
				TraceLine(image.Entry);
			}
			TraceLine(L"Objects");
			ix = 0;
			for (auto obj : pMap->ObjectMap)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = ");
				TraceLine(obj, 16);
			}
			TraceLine(L"Startup positions");
			ix = 0;
			for (auto pos : pMap->StartupPositions)
			{
				Trace(L"\t");
				Trace(ix++);
				Trace(L" = (");
				Trace(pos.X);
				Trace(L", ");
				Trace(pos.Y);
				Trace(L", ");
				Trace(pos.Z);
				Trace(L"), angle = ");
				TraceLine(pos.Angle);
			}

			TraceLine(L"");
		}
#endif

		return ret;
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
		if (CurrentModule == pModule) {
			CurrentModule->Pause();
		}
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

void CModuleController::GotFocus()
{
	if (CurrentModule != NULL) {
		CurrentModule->GotFocus();
	}
}

void CModuleController::LostFocus()
{
	if (CurrentModule != NULL) {
		CurrentModule->LostFocus();
	}
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
