#pragma once

#include "MainMenuModule.h"

class CPDMainMenuModule : public CMainMenuModule
{
public:
	CPDMainMenuModule();
	virtual ~CPDMainMenuModule();

	virtual void Render();

protected:
	virtual void SetupScreen();

	virtual void SetupConfigFrame();
	virtual void SetupLoadFrame();
	virtual void SetupSaveFrame();

	static void Intro(LPVOID data);
	static void Credits(LPVOID data);
};
