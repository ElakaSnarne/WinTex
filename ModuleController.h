#pragma once

#include "ModuleBase.h"
#include <list>
#include "AnimatedCursor.h"
#include "Map.h"
#include "Mutex.h"

class CControllerData;

class CModuleController
{
public:
	static BOOL Init(CMap* map, CMap* dmap);

	static void Push(CModuleBase* pModule, BOOL overrideCurrent = FALSE);
	static void Pop(CModuleBase* pModule);
	static void SendToBack(CModuleBase* pModule);
	static void SendToFront(CModuleBase* pModule);

	static CModuleBase* Get(ModuleType type);
	static void ClearExcept(CModuleBase* pModule);

	static void Render();
	static void MouseMove(POINT pt);
	static void MouseDown(POINT pt, int btn);
	static void MouseUp(POINT pt, int btn);
	static void MouseWheel(int scroll);
	static void KeyDown(WPARAM key, LPARAM lParam);
	static void KeyUp(WPARAM key, LPARAM lParam);
	static void GamepadInput(InputSource source, int offset, int data);

	static void Resize(int width, int height);

	static CAnimatedCursor Cursors[12];

	static DWORD MainThreadId;
	static DWORD D3DThreadId;
	static DWORD TimerThreadId;

	static CModuleBase* CurrentModule;
	static CModuleBase* NextModule;

	static CMap* pMap;
	static CMap* pDMap;

	static void Resume();

protected:
	static std::list<CModuleBase*> Modules;

	static void PauseModule(CModuleBase* pModule);
	static void ResumeModule(CModuleBase* pModule);
};
