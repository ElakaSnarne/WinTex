#pragma once

#include "MainMenuModule.h"

class CUAKMMainMenuModule : public CMainMenuModule
{
public:
	CUAKMMainMenuModule();
	virtual ~CUAKMMainMenuModule();

protected:
	virtual void SetupScreen();
	virtual void SetupSave();
	virtual void SetupLoad();

	static void Intro(LPVOID data);
	static void Credits(LPVOID data);
};
