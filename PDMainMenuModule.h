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

	static void Intro(LPVOID data);
	static void Credits(LPVOID data);
};
