#include "ScriptBase.h"
#include "Globals.h"
#include "GameBase.h"
#include "GameController.h"

CScriptBase::CScriptBase()
{
	ZeroMemory(&_mapEntry, sizeof(_mapEntry));

	_pLoc = NULL;
}

CScriptBase::~CScriptBase()
{
}

void CScriptBase::SelectDialogueOption(CScriptState* pState, int option)
{
	// 0x9c enables inventory
	// if a[253]=1 then ask about

	if (pState->OfferMode && option == 1)
	{
		// Offer
		pState->Offer = !pState->Offer;
		pState->TopItemOffset = -1;
	}
	else if (CGameController::GetParameter(253) == 1 && option == 1)
	{
		// AskAbout
		pState->AskAbout = !pState->AskAbout;
		pState->TopItemOffset = -1;
	}
	else
	{
		pState->AskAbout = pState->Offer = FALSE;
		if (pState->WaitingForInput)
		{
			pState->OfferMode = FALSE;
			pState->SelectedOption = option;
			Resume(pState, TRUE);
		}
	}
}
