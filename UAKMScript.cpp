#include "UAKMScript.h"
#include "Globals.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "GameBase.h"
#include "UAKMCrimeLinkModule.h"
#include "LocationModule.h"
#include "VideoModule.h"
#include "GameController.h"
#include "AmbientAudio.h"
#include "UAKMTravelModule.h"
#include "UAKMGame.h"
#include "UAKMSafeModule.h"
#include "UAKMCodePanelModule.h"
#include "UAKMColonelsSafeModule.h"
#include "UAKMColonelsComputerModule.h"
#include "MainMenuModule.h"
#include "UAKMPusShellGameModule.h"
#include "UAKMGRSComputerModule.h"
#include "UAKMStasisModule.h"
#include "PictureModule.h"

Point _lastKnownEyeBotPosition;

CUAKMScript::CUAKMScript()
{
	_functions[0x00] = &CUAKMScript::Function_80;
	_functions[0x01] = &CUAKMScript::Function_81;
	_functions[0x02] = &CUAKMScript::Function_82;
	_functions[0x03] = &CUAKMScript::Function_83;
	_functions[0x04] = &CUAKMScript::Function_84;
	_functions[0x05] = &CUAKMScript::Function_85;
	_functions[0x06] = &CUAKMScript::Function_86;
	_functions[0x07] = &CUAKMScript::Function_87;
	_functions[0x08] = &CUAKMScript::Function_88;
	_functions[0x09] = &CUAKMScript::Function_89;
	_functions[0x0a] = &CUAKMScript::Function_8A;
	_functions[0x0b] = &CUAKMScript::Function_8B;
	_functions[0x0c] = &CUAKMScript::Function_8C;
	_functions[0x0d] = &CUAKMScript::Function_8D;
	_functions[0x0e] = &CUAKMScript::Function_8E;
	_functions[0x0f] = &CUAKMScript::Function_8F;
	_functions[0x10] = &CUAKMScript::Function_90;
	_functions[0x11] = &CUAKMScript::Function_91;
	_functions[0x12] = &CUAKMScript::Function_92;
	_functions[0x13] = &CUAKMScript::Function_93;
	_functions[0x14] = &CUAKMScript::Function_94;
	_functions[0x15] = &CUAKMScript::Function_95;
	_functions[0x16] = &CUAKMScript::Function_96;
	_functions[0x17] = &CUAKMScript::Function_97;
	_functions[0x18] = &CUAKMScript::Function_98;
	_functions[0x19] = &CUAKMScript::Function_99;
	_functions[0x1a] = &CUAKMScript::Function_9A;
	_functions[0x1b] = &CUAKMScript::Function_9B;
	_functions[0x1c] = &CUAKMScript::Function_9C;
	_functions[0x1d] = &CUAKMScript::Function_9D;
	_functions[0x1e] = &CUAKMScript::Function_9E;
	_functions[0x1f] = &CUAKMScript::Function_9F;
	_functions[0x20] = &CUAKMScript::Function_A0;
	_functions[0x21] = &CUAKMScript::Function_A1;
	_functions[0x22] = &CUAKMScript::Function_A2;
	_functions[0x23] = &CUAKMScript::Function_A3;
	_functions[0x24] = &CUAKMScript::Function_A4;
	_functions[0x25] = &CUAKMScript::Function_A5;
	_functions[0x26] = &CUAKMScript::Function_A6;
	_functions[0x27] = &CUAKMScript::Function_A7;
	_functions[0x28] = &CUAKMScript::Function_A8;
	_functions[0x29] = &CUAKMScript::Function_A9;
	_functions[0x2a] = &CUAKMScript::Function_AA;
	_functions[0x2b] = &CUAKMScript::Function_AB;
	_functions[0x2c] = &CUAKMScript::Function_AC;
	_functions[0x2d] = &CUAKMScript::Function_AD;
	_functions[0x2e] = &CUAKMScript::Function_AE;
	_functions[0x2f] = &CUAKMScript::Function_AF;
	_functions[0x30] = &CUAKMScript::Function_B0;
	_functions[0x31] = &CUAKMScript::Function_B1;
	_functions[0x32] = &CUAKMScript::Function_B2;
	_functions[0x33] = &CUAKMScript::Function_B3;
	_functions[0x34] = &CUAKMScript::Function_B4;
	_functions[0x35] = &CUAKMScript::Function_B5;
	_functions[0x36] = &CUAKMScript::Function_B6;
	_functions[0x37] = &CUAKMScript::Function_B7;
	_functions[0x38] = &CUAKMScript::Function_B8;
	_functions[0x39] = &CUAKMScript::Function_B9;
	_functions[0x3a] = &CUAKMScript::Function_BA;
	_functions[0x3b] = &CUAKMScript::Function_BB;
	_functions[0x3c] = &CUAKMScript::Function_BC;
	_functions[0x3d] = &CUAKMScript::Function_BD;
	_functions[0x3e] = &CUAKMScript::Function_BE;
	_functions[0x3f] = &CUAKMScript::Function_BF;
	_functions[0x40] = &CUAKMScript::Function_C0;
	_functions[0x41] = &CUAKMScript::Function_C1;
	_functions[0x42] = &CUAKMScript::Function_C2;
	_functions[0x43] = &CUAKMScript::Function_C3;
	_functions[0x44] = &CUAKMScript::Function_C4;
	_functions[0x45] = &CUAKMScript::Function_C5;
	_functions[0x46] = &CUAKMScript::Function_C6;
	_functions[0x47] = &CUAKMScript::Function_C7;
	_functions[0x48] = &CUAKMScript::Function_C8;
	_functions[0x49] = &CUAKMScript::Function_C9;
	_functions[0x4a] = &CUAKMScript::Function_CA;
	_functions[0x4b] = &CUAKMScript::Function_CB;
	_functions[0x4c] = &CUAKMScript::Function_CC;
	_functions[0x4d] = &CUAKMScript::Function_CD;
	_functions[0x4e] = &CUAKMScript::Function_CE;
	_functions[0x4f] = &CUAKMScript::Function_CF;
	_functions[0x50] = &CUAKMScript::Function_D0;
	_functions[0x51] = &CUAKMScript::Function_D1;
	_functions[0x52] = &CUAKMScript::Function_D2;
	_functions[0x53] = &CUAKMScript::Function_D3;
	_functions[0x54] = &CUAKMScript::Function_D4;
	_functions[0x55] = &CUAKMScript::Function_D5;
	_functions[0x56] = &CUAKMScript::Function_D6;
	_functions[0x57] = &CUAKMScript::Function_D7;
	_functions[0x58] = &CUAKMScript::Function_D8;
	_functions[0x59] = &CUAKMScript::Function_D9;
	_functions[0x5a] = &CUAKMScript::Function_DA;
	_functions[0x5b] = &CUAKMScript::Function_DB;
	_functions[0x5c] = &CUAKMScript::Function_DC;
	_functions[0x5d] = &CUAKMScript::Function_DD;
	_functions[0x5e] = &CUAKMScript::Function_DE;
	_functions[0x5f] = &CUAKMScript::Function_DF;
	_functions[0x60] = &CUAKMScript::Function_E0;
}

CUAKMScript::~CUAKMScript()
{
}

void CUAKMScript::PermformAction(CScriptState* pState, int id, ActionType action, int item)
{
	pState->CurrentAction = action;
	if (item > 0)
	{
		pState->CurrentAction = ActionType::Use;
	}
	CGameController::SetParameter(99, item);
	Execute(pState, id);
}

void CUAKMScript::Execute(CScriptState* pState, int id)
{
	// Find execution pointer and call resume
	pState->ExecutionPointer = pState->GetScript(id);
	pState->WaitingForMediaToFinish = FALSE;
	pState->LastDialoguePoint = 0;
	Resume(pState);
}

void CUAKMScript::Resume(CScriptState* pState, BOOL breakWait)
{
	_scriptLock.Lock();
	CModuleBase* pThisModule = CModuleController::CurrentModule;

	if (breakWait)
	{
		pState->WaitingForMediaToFinish = FALSE;
		pState->WaitingForInput = FALSE;
		pState->WaitingForExternalModule = FALSE;
	}

	//WCHAR buffer[40];

	while (CModuleController::CurrentModule == pThisModule && pState->ExecutionPointer > -1 && pState->ExecutionPointer < pState->Length && pState->Script != NULL && !pState->WaitingForMediaToFinish && !pState->WaitingForInput && !pState->WaitingForExternalModule)
	{
		//_itow(pState->ExecutionPointer, buffer, 16);
		//OutputDebugString(buffer);
		//OutputDebugString(L" - ");

		// Execute script
		byte cmd = pState->Script[pState->ExecutionPointer++];
		if (cmd < 0x80 || cmd > 0xe0)
		{
			// This should never happen, terminate script
			DebugTrace(pState, L"Invalid command, terminating script");
			pState->ExecutionPointer = -1;
		}
		else
		{
			(this->*_functions[cmd - 0x80])(pState);
		}
	}

	if (CModuleController::CurrentModule != pThisModule)
	{
		// A new module has been loaded
		if (!pState->WaitingForInput)
		{
			CModuleController::Pop(pThisModule);
		}
	}
	_scriptLock.Release();
}

void CUAKMScript::Function_80(CScriptState* pState)
{
	DebugTrace(pState, L"Function_80");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_81(CScriptState* pState)
{
	DebugTrace(pState, L"Function_81 - If Action=Use, play \"That's not gonna work\" and end script");
	if (pState->CurrentAction == ActionType::Use)
	{
		// Call script 89 to print "That's not gonna work." and play media 0 in SOUNDS.AP
		pAddCaptions->clear();
		pAddCaptions->push_back(new CCaption(0, "That's not gonna work.", TRUE));

		SwapCaptions();

		CAnimationController::Load(L"SOUND.AP", 0);

		pState->WaitingForMediaToFinish = TRUE;
	}

	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_83(CScriptState* pState)
{
	DebugTrace(pState, L"Function_83 - Jump on Abort");
	pState->ExecutionPointer = (conversationOption == ConversationOption::Abort) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_85(CScriptState* pState)
{
	DebugTrace(pState, L"Function_85 - If Action=Move jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Move) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_86(CScriptState* pState)
{
	DebugTrace(pState, L"Function_86 - If Action=Use jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Use) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_87(CScriptState* pState)
{
	DebugTrace(pState, L"Function_87 - If Action=Talk jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Talk) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_88(CScriptState* pState)
{
	DebugTrace(pState, L"Function_88 - If Action=Open jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Open) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_89(CScriptState* pState)
{
	DebugTrace(pState, L"Function_89");
	//text += string.Format("Print: {0}", print);	print up to 0-termination
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_8F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8F - Set Player Position");

	int x = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int y = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int z = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int a = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	if ((x & 0x8000) != 0) x |= 0xffff0000;
	if ((y & 0x8000) != 0) y |= 0xffff0000;
	if ((z & 0x8000) != 0) z |= 0xffff0000;
	if ((a & 0x8000) != 0) a |= 0xffff0000;

	float fx = ((float)x) / 16.0f;
	float fy = ((float)y) / 16.0f;
	float fz = ((float)z) / 16.0f;
	float fa = ((float)a);

	if (x != -1) CLocation::_x = -fx;
	if (y != -1) CLocation::_y = -fy;
	if (z != -1) CLocation::_z = -fz;
	if (a != -1) CLocation::_angle2 = fa;

	CLocationModule::_movement_x = 0.0f;
	CLocationModule::_movement_y = 0.0f;
	CLocationModule::_movement_z = 0.0f;
	CLocationModule::_speed = 0.2f;

	pState->ExecutionPointer += 8;
}

void CUAKMScript::Function_92(CScriptState* pState)
{
	DebugTrace(pState, L"Function_92");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_94(CScriptState* pState)
{
	DebugTrace(pState, L"Function_94 - Load Files");

	CAmbientAudio::Clear();
	pMIDI->Stop();
	CGameController::SetParameter(252, 0);

	// Conversations & media
	// Load files from DMAP.LZ
	int ix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	pState->ExecutionPointer += 2;

	CGameController::SetData(UAKM_SAVE_DMAP_ENTRY, ix);
	CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 1);
	CGameController::SetData(UAKM_SAVE_SCRIPT_ID, (BYTE)0);

	CGameController::AutoSave();

	CModuleController::Push(new CVideoModule(VideoType::Scripted, ix));

	pState->WaitingForInput = TRUE;
}

void CUAKMScript::Function_95(CScriptState* pState)
{
	DebugTrace(pState, L"Function_95 - Jump on animation at frame");
	pState->ExecutionPointer = (_pLoc != NULL && _pLoc->GetAnimationFrame(pState->Script[pState->ExecutionPointer]) == GetInt(pState->Script, pState->ExecutionPointer + 2, 1)) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer + 4, 2)) : pState->ExecutionPointer + 6;
}

void CUAKMScript::Function_98(CScriptState* pState)
{
	DebugTrace(pState, L"Function_98 - Jump on player Y > value");
	int ytest = GetInt(pState->Script, pState->ExecutionPointer, 2);
	if (ytest & 0x8000)
	{
		ytest |= ~0xffff;
	}

	float ylimit = ((float)ytest) / 16.0f;
	float playerY = -_pLoc->GetPlayerPosition().Y;
	pState->ExecutionPointer = (playerY > ylimit) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer + 2, 2)) : pState->ExecutionPointer + 4;
}

void CUAKMScript::Function_9A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_9A - Jump on player inside rectangle");

	int p1 = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int p3 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int p4 = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	int script = GetInt(pState->Script, pState->ExecutionPointer + 8, 2);

	if ((p1 & 0x8000) != 0) p1 = (int)(p1 | 0xffff0000);
	if ((p2 & 0x8000) != 0) p2 = (int)(p2 | 0xffff0000);
	if ((p3 & 0x8000) != 0) p3 = (int)(p3 | 0xffff0000);
	if ((p4 & 0x8000) != 0) p4 = (int)(p4 | 0xffff0000);

	float f1 = ((float)p1) / 16.0f;
	float f2 = ((float)p2) / 16.0f;
	float f3 = ((float)p3) / 16.0f;
	float f4 = ((float)p4) / 16.0f;

	float x1 = min(f1, f3);
	float x2 = max(f1, f3);
	float z1 = min(f2, f4);
	float z2 = max(f2, f4);

	if (_pLoc != NULL)
	{
		Point p = _pLoc->GetPlayerPosition();
		if (p.X >= x1 && p.X <= x2 && p.Z >= z1 && p.Z <= z2)
		{
			pState->ExecutionPointer = pState->GetScript(script);
		}
		else
		{
			pState->ExecutionPointer += 10;
		}
	}
	else
	{
		pState->ExecutionPointer += 10;
	}
}

void CUAKMScript::Function_9B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_9B");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_9C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_9C - Offer from Inventory mode");

	CGameController::SetParameter(253, 1);
	CGameController::SetParameter(100, 1);
	pState->Mode = InteractionMode::Offer;
	pState->Offer = FALSE;
	pState->AskAbout = FALSE;
}

void CUAKMScript::Function_9E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_9E - Start Timer");

	int timer = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int duration = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	CGameController::SetTimer(timer, duration);
	pState->ExecutionPointer += 4;

	//Trace(L"Timer ");
	//Trace(timer);
	//Trace(L" set to ");
	//Trace((int)(duration*TIMER_SCALE));
	//TraceLine(L" ms");
}

void CUAKMScript::Function_9F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_9F - Conditional jump on Timer");
	int timer = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int timerState = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int script = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int currentState = CGameController::GetTimerState(timer);
	BOOL stateMatches = (currentState == timerState);
	pState->ExecutionPointer = stateMatches ? pState->GetScript(script) : pState->ExecutionPointer + 6;

	//Trace(L"Timer ");
	//Trace(timer);
	//if (stateMatches)
	//{
	//	Trace(L" is in state ");
	//	Trace(timerState);
	//	Trace(L", branching to script ");
	//	TraceLine(script);
	//}
	//else
	//{
	//	Trace(L" is NOT in state ");
	//	Trace(timerState);
	//	Trace(L", but in state ");
	//	Trace(currentState);
	//	TraceLine(L", not branching");
	//}
}

void CUAKMScript::Function_A0(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A0 - Change Travel Location state");

	CGameController::SetData(UAKM_SAVE_TRAVEL + pState->Script[pState->ExecutionPointer], pState->Script[pState->ExecutionPointer + 1]);

	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_A2(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A2");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_A3(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A3");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_A4(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A4");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_A6(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A6");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_A8(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A8 - Set Continuation Point");

	CGameController::SetData(UAKM_SAVE_SCRIPT_ID, pState->Script[pState->ExecutionPointer]);
	CGameController::SetData(UAKM_SAVE_SCRIPT_ID + 1, pState->Script[pState->ExecutionPointer + 1]);

	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_A9(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A9");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_AB(CScriptState* pState)
{
	DebugTrace(pState, L"Function_AB - Show Picture");
	// Show picture, mapped media
	int ix = pState->Script[pState->ExecutionPointer];
	pState->ExecutionPointer++;
	Show(pState, ix);
}

void CUAKMScript::Function_AC(CScriptState* pState)
{
	DebugTrace(pState, L"Function_AC - Disallow cancelling of travel");
	CGameController::CanCancelTravel = FALSE;
}

void CUAKMScript::Function_AD(CScriptState* pState)
{
	DebugTrace(pState, L"Function_AD - Travel");
	CModuleController::Push(new CUAKMTravelModule());
	pState->WaitingForInput = TRUE;
}

void CUAKMScript::Function_AE(CScriptState* pState)
{
	DebugTrace(pState, L"Function_AE - If AskAboutState[x]=y go to z");

	int ix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int val = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int id = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);

	pState->ExecutionPointer = (CGameController::GetAskAboutState(ix) == val) ? pState->GetScript(id) : pState->ExecutionPointer + 6;
}

void CUAKMScript::Function_AF(CScriptState* pState)
{
	DebugTrace(pState, L"Function_AF - Special");

	int i1 = pState->Script[pState->ExecutionPointer];
	int i2 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int i3 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);

	switch (i1)
	{
	case 0:
	{
		// Nothing?
		int debug = 0;
		break;
	}
	case 1:
	{
		if (CGameController::GetData(UAKM_SAVE_AF_ACTIONS + i3) == 0)
		{
			int debug = 0;
		}

		break;
	}
	case 2:
	{
		// In warehouse (palette functions only)
		break;
	}
	case 3:
	{
		// Colonel's Computer
		CModuleController::Push(new CUAKMColonelsComputerModule());
		pState->WaitingForInput = TRUE;
		break;
	}
	case 4:
	{
		// Pus' Shell game
		CModuleController::Push(new CUAKMPusShellGameModule(i2, i3));
		pState->WaitingForExternalModule = TRUE;
		break;
	}
	case 5:
	{
		// Crime Link computer
		CModuleBase* pModule = new CUAKMCrimeLinkModule(i2);
		CModuleController::Push(pModule);
		pState->WaitingForInput = TRUE;
		break;
	}
	case 6:
	{
		int debug = 0;
		break;
	}
	case 7:
	{
		// Eddie Ching's safe
		CModuleController::Push(new CUAKMSafeModule(i2, i3 == 0));
		pState->WaitingForInput = TRUE;
		break;
	}
	case 8:
	{
		// Colonel's safe
		CModuleController::Push(new CUAKMColonelsSafeModule(i2));
		pState->WaitingForInput = TRUE;
		break;
	}
	case 9:
	{
		// Stasis chamber
		CModuleController::Push(new CUAKMStasisModule(i2));
		pState->WaitingForInput = TRUE;
		//pState->WaitingForMediaToFinish = TRUE;
		pState->WaitingForExternalModule = TRUE;
		break;
	}
	case 10:
	{
		// Looping sound
		CAmbientAudio::Loop(_mapEntry, i2, i3);
		break;
	}
	case 11:
	{
		// Hotel door code
		CModuleController::Push(new CUAKMCodePanelModule(i2));
		pState->WaitingForInput = TRUE;
		break;
	}
	case 12:
	{
		// GRS, Paul Dubois' computer
		CModuleController::Push(new CUAKMGRSComputerModule());
		pState->WaitingForInput = TRUE;
		break;
	}
	case 13:
	{
		if (i2 == 1)
		{
			// Restore travelable locations
			for (int i = 0; i < 40; i++)
			{
				CGameController::SetData(UAKM_SAVE_TRAVEL + i, CGameController::GetData(UAKM_SAVE_TRAVEL_BACKUP + i));
			}
		}
		else
		{
			// Reset travelable locations
			for (int i = 0; i < 40; i++)
			{
				CGameController::SetData(UAKM_SAVE_TRAVEL_BACKUP + i, CGameController::GetData(UAKM_SAVE_TRAVEL + i));
				CGameController::SetData(UAKM_SAVE_TRAVEL + i, (BYTE)0);
			}
			CGameController::SetData(UAKM_SAVE_TRAVEL + 5, 1);	// Enable travel to Tex' office
		}

		break;
	}
	case 14:
	{
		CGameController::SetParameter(i2, CGameController::GetParameter(i2) + 1);	// Used for checking hint categories in Tex' office, possibly other places...
		break;
	}
	case 15:
	{
		// Moon Child computer hack? Looks like just fade palette out and in
		break;
	}
	case 16:
	{
		int debug = 0;
		break;
	}
	case 17:
	{
		// After intro
		//TraceLine(L"After Intro");	// Probably flags the intro has been played so it doesn't auto play again next time a new game is started
		break;
	}
	case 18:
	{
		int debug = 0;
		break;
	}
	case 19:
	{
		// Select held item
		//TraceLine(L"Select held item");

		// TODO: Location module, select current item? Only if Location Module is current (or have to make held item static in Game Controller)...

		break;
	}
	case 20:
	{
		//TraceLine(L"Set text and background colour");	// Indexes in the active palette
		break;
	}
	case 21:
	{
		int debug = 0;
		break;
	}
	case 22:
	{
		// Arrive at GRS, get pointer to probe position?
		break;
	}
	case 23:
	{
		// Check distance from probe, set A205 to 1 if too close
		CGameController::SetParameter(205, 0);

		// Location item 0x10-0x16 (whichever is visible)
		Point player = _pLoc->GetPlayerPosition();
		double distance = -1.0;
		for (int i = 0x10; i <= 0x16; i++)
		{
			SpritePosInfo info = _pLoc->GetSpriteInfo(i);
			if (info.Visible)
			{
				_lastKnownEyeBotPosition = info.Position;

				double x = player.X - info.Position.X;
				double z = player.Z - info.Position.Z;
				double distance = sqrt(x * x + z * z);
				if (distance < 35)
				{
					CGameController::SetParameter(205, 1);
				}

				break;
			}
		}

		if (distance < 0.0)
		{
			double x = player.X - _lastKnownEyeBotPosition.X;
			double z = player.Z - _lastKnownEyeBotPosition.Z;
			distance = sqrt(x * x + z * z);
		}

		// Set volume of probe audio
		CAmbientAudio::SetVolume(0, (100.0f - (float)min(100.0f, max(0, distance - 35))) / 100.0f);

		break;
	}
	case 24:
	{
		// Play security probe looping sound (with correct volume)
		CAmbientAudio::Loop(_mapEntry, i2, i3);
		break;
	}
	case 25:
	{
		int debug = 0;
		// GRS related?
		break;
	}
	default:
	{
		// Illegal
		//TraceLine(L"Illegal!");
		break;
	}
	}

	pState->ExecutionPointer += 6;
}

void CUAKMScript::Function_B0(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B0");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_B1(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B1");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_B2(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B2");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_B3(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B3");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_B4(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B4 - Display options");

	pState->LastDialoguePoint = pState->ExecutionPointer - 1;

	//ClearCaption();

	char* pT[3];
	//float w = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		pT[i] = (char*)(pState->Script + pState->ExecutionPointer);
		//w = max(Font.PixelWidth(pT[i]), w);

		while (pState->Script[pState->ExecutionPointer++] != 0);
	}

	DialogueOptionsCount = (strlen(pT[2]) == 0) ? (strlen(pT[1]) == 0) ? 1 : 2 : 3;

	float w = 0.0f;
	float h = 0.0f;
	// TODO: Make a max size based on number of dialogue options, calculate required size for all items
	float maxw = (dx.GetWidth() - 64.0f * pConfig->FontScale * DialogueOptionsCount) / DialogueOptionsCount;
	for (int i = 0; i < 3; i++)
	{
		Size sz = TexFont.GetSize(pT[i], maxw);
		if (sz.Width > w)
		{
			w = sz.Width;
		}
		if (sz.Height > h)
		{
			h = sz.Height;
		}

		DialogueOptions[i].SetValue(i + 1);
	}

	// TODO: Should calculate height, in case the text needs to be wrapped...

	Size sz;
	sz.Width = w;
	sz.Height = h;

	float dow = w + 64.0f * pConfig->FontScale;
	float sx = (dx.GetWidth() - DialogueOptionsCount * dow) / DialogueOptionsCount;
	/*
	if (sx < 10.0f)
	{
		// TODO: Either need multi-line, or will need different y-position for options

		// TODO: Attempt 1, multi line
		float allowedSpace = (dx.GetWidth() - DialogueOptionsCount * 74.0f) / DialogOptionsCount;
		for (int i = 0; i < DialogueOptionsCount; i++)
		{
			sz = Font.GetSize(pT[i], allowedSpace);
		}
	}
	*/

	if (DialogueOptionsCount == 1)
	{
		// Center
		DialogueOptions[0].SetText(pT[0], sz, (dx.GetWidth() - dow) / 2.0f);
	}
	else if (DialogueOptionsCount == 2)
	{
		// Center left, center right
		float sx3 = sx / 3.0f;
		DialogueOptions[0].SetText(pT[0], sz, sx3);
		DialogueOptions[1].SetText(pT[1], sz, dx.GetWidth() - dow - sx3);
	}
	else if (DialogueOptionsCount == 3)
	{
		// Left, center, right
		DialogueOptions[0].SetText(pT[0], sz, 0.0f);
		DialogueOptions[1].SetText(pT[1], sz, (dx.GetWidth() - dow) / 2.0f);
		DialogueOptions[2].SetText(pT[2], sz, dx.GetWidth() - dow);
	}

	pState->SelectedOption = -1;

	// Should now wait for input
	pState->WaitingForInput = TRUE;
}

void CUAKMScript::Function_B5(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B5");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_B7(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B7");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_B9(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B9 - Return to dialogue/options");
	pState->ExecutionPointer = pState->LastDialoguePoint;
}

void CUAKMScript::Function_BB(CScriptState* pState)
{
	DebugTrace(pState, L"Function_BB");
	int ix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	// Possibly stop all ambient sounds and play from media map (ix<<1)
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_BC(CScriptState* pState)
{
	DebugTrace(pState, L"Function_BC");
	//text += "Abort?";
	//text += "???ResetPosition???";
}

void CUAKMScript::Function_BD(CScriptState* pState)
{
	DebugTrace(pState, L"Function_BD");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_BE(CScriptState* pState)
{
	DebugTrace(pState, L"Function_BE");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_BF(CScriptState* pState)
{
	DebugTrace(pState, L"Function_BF - Jump on item collected or used");
	int bix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int ns = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	pState->ExecutionPointer = (CGameController::GetItemState(bix) != 0) ? pState->GetScript(ns) : pState->ExecutionPointer + 4;
}

void CUAKMScript::Function_C0(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C0 - Stop Animation");
	int anim = pState->Script[pState->ExecutionPointer++];
	if (_pLoc != NULL)
	{
		_pLoc->StopMappedAnimation(anim);
	}
}

void CUAKMScript::Function_C1(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C1");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_C2(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C2");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_C3(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C3 - Preserve palette");
	// PreservePalette = true
}

void CUAKMScript::Function_C6(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C6 - Game Over");
	pState->ExecutionPointer = -1;

	if (_lock.Lock())
	{
		// Find main menu module in module controller
		CMainMenuModule* pMMM = (CMainMenuModule*)CModuleController::Get(ModuleType::MainMenu);
		if (pMMM != NULL)
		{
			// Call GameOver on module
			pMMM->GameOver();

			// Clear all modules except the main menu
			CModuleController::ClearExcept(pMMM);

			// Delete the autosave (which will only reload the death scene)
			DeleteFile(L"GAMES\\SAVEGAME.000");
		}
	}
}

void CUAKMScript::Function_C7(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C7 - Get & Set Player Death");
	//pState->ExecutionPointer = -1;
	// Set A[232] to current PlayerHasDied flag
	// Set PlayerHasDied flag to true and save player data
}

void CUAKMScript::Function_C8(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C8");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_CA(CScriptState* pState)
{
	DebugTrace(pState, L"Function_CA - Set Hint Category State");
	int ix = GetInt(pState->Script, pState->ExecutionPointer, 1);
	if (CGameController::GetHintCategoryState(ix) != 2) CGameController::SetHintCategoryState(ix, pState->Script[pState->ExecutionPointer + 1]);
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_CB(CScriptState* pState)
{
	DebugTrace(pState, L"Function_CB - Start Location MIDI");
}

void CUAKMScript::Function_CC(CScriptState* pState)
{
	DebugTrace(pState, L"Function_CC - Conditional Score Increment");
	int val = GetInt(pState->Script, pState->ExecutionPointer, 2);
	pState->ExecutionPointer += 2;

	if (val >= 1000)
	{
		val -= 1000;
		CGameController::SetHintState(val, 1, 0);
	}
	else
	{
		int currentState = CGameController::GetHintState(val);
		CGameController::SetHintState(val, currentState | 1, 0);
		if (currentState == 0)
		{
			CGameController::AddScore(1);
		}
	}
}

void CUAKMScript::Function_D0(CScriptState* pState)
{
	// TODO: Only used to check a condition when talking to Ardo, condition is unknown
	DebugTrace(pState, L"Function_D0");
	int conditionalScript = GetInt(pState->Script, pState->ExecutionPointer, 2);
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_D1(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D1 - Play Wave");
	int index = pState->Script[pState->ExecutionPointer];
	pState->ExecutionPointer++;

	//Play(state, index, (_dmapEntry.ScriptFileEntry != 0 || _dmapEntry.ScriptFileIndex != 0) ? 1 : 0);
	// Swap caption pointers
	SwapCaptions();

	if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		FileMap fm = _mapEntry->AudioMap.at(index);
		std::wstring fn = CGameController::GetFileName(fm.File);
		if (fn != L"")
		{
			CAnimationController::Load(fn.c_str(), fm.Entry);
		}
	}
	else if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		FileMap fm = _mapEntry->AudioMap.at(index);
		std::wstring fn = CGameController::GetFileName(fm.File);
		if (fn != L"")
		{
			CAnimationController::Load(fn.c_str(), fm.Entry);
		}
	}

	pState->WaitingForMediaToFinish = TRUE;
}

void CUAKMScript::Function_D2(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D2");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_D3(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D3");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_D4(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D4 - Stop Audio");

	CAmbientAudio::Stop((pState->Script[pState->ExecutionPointer++]) & 0x7f);
}

void CUAKMScript::Function_D5(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D5 - Load MIDI");
	// TODO: Should get filename from file list, but for UAKM it is always MUSIC.AP
	int entry = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	//Trace(L"Loading MIDI #");
	//Trace(entry);
	//Trace(L" from script at offset ");
	//TraceLine(pState->ExecutionPointer - 1, 16);
	BinaryData bd = LoadEntry(L"MUSIC.AP", entry);
	pMIDI->Init(bd);
	pState->ExecutionPointer += 4;
}

void CUAKMScript::Function_D6(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D6 - Start MIDI");
	pMIDI->Start();
}

void CUAKMScript::Function_DA(CScriptState* pState)
{
	DebugTrace(pState, L"Function_DA - Jump on player close to point");

	int p1 = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int p3 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int p4 = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);

	if ((p1 & 0x8000) != 0) p1 = (int)(p1 | 0xffff0000);
	if ((p2 & 0x8000) != 0) p2 = (int)(p2 | 0xffff0000);

	float f1 = ((float)p1) / 16.0f;
	float f2 = ((float)p2) / 16.0f;
	float f3 = ((float)p3) / 16.0f;

	//text += string.Format("If player distance from ({0:0.00},{1:0.00})<{2:0.00} jump to script {3}", f1, f2, f3, p4);
	double distance = (_pLoc != NULL) ? _pLoc->GetPlayerDistanceFromPoint(f1, f2) : 0.0;
	f3 *= f3;
	//Trace(L"Player distance from <");
	//Trace(f1);
	//Trace(L", ");
	//Trace(f2);
	//Trace(L"> = ");
	//Trace((float)distance);
	//Trace(L", test against ");
	//TraceLine(f3);

	if (distance <= f3)
	{
		pState->ExecutionPointer = pState->GetScript(p4);
	}
	else
	{
		pState->ExecutionPointer += 8;
	}
}

void CUAKMScript::Function_DB(CScriptState* pState)
{
	DebugTrace(pState, L"Function_DB - Play BIC");
	//int offx = GetInt(pState->Script, pState->ExecutionPointer, 2);
	//int offy = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int index = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int rate = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	pState->ExecutionPointer += 8;

	Play(pState, index, 1, rate);
}

void CUAKMScript::Function_DC(CScriptState* pState)
{
	DebugTrace(pState, L"Function_DC - Play Silent BIC");
	//int offx = GetInt(pState->Script, pState->ExecutionPointer, 2);
	//int offy = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int index = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int rate = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	pState->ExecutionPointer += 8;

	Play(pState, index, 1, rate);
}

void CUAKMScript::Function_DD(CScriptState* pState)
{
	DebugTrace(pState, L"Function_DD");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_DE(CScriptState* pState)
{
	DebugTrace(pState, L"Function_DE");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_DF(CScriptState* pState)
{
	DebugTrace(pState, L"Function_DF");
	pState->ExecutionPointer = -1;
}

// Completed script commands

void CUAKMScript::Function_82(CScriptState* pState)
{
	DebugTrace(pState, L"Function_82 - If Action=Look jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Look) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_84(CScriptState* pState)
{
	DebugTrace(pState, L"Function_84 - If Action=Get jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Get) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_8A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8A - End script");
	pState->ExecutionPointer = -1;
}

void CUAKMScript::Function_8B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8B - Start Animation");
	int anim = pState->Script[pState->ExecutionPointer++];
	if (_pLoc != NULL)
	{
		_pLoc->StartMappedAnimation(anim);
	}
}

void CUAKMScript::Function_8C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8C - Set Parameter");

	// Set parameter
	int index = GetInt(pState->Script, pState->ExecutionPointer, 1);
	int value = pState->Script[pState->ExecutionPointer + 1];
	if (index == 252)
	{
		if (value == 1 && CGameController::GetParameter(index) == 0)
		{
			pState->Offer = FALSE;
			pState->AskAbout = FALSE;
			pState->TopItemOffset = -1;
		}
	}
	else if (index == 100)
	{
		pState->Mode = (value != 0) ? InteractionMode::Offer : InteractionMode::AskAbout;
		pState->AskAbout = FALSE;
		pState->Offer = FALSE;
		pState->TopItemOffset = -1;
	}

	CGameController::SetParameter(index, (byte)value);
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_8D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8D - Jump on A[X]=Y");

	if (pState->ExecutionPointer == 0x27d)
	{
		int debug = 0;
	}

	int ix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int cmp = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int ns = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);

	if (CGameController::GetParameter(ix) == cmp)
	{
		pState->ExecutionPointer = pState->GetScript(ns);
	}
	else
	{
		pState->ExecutionPointer += 6;
	}
}

void CUAKMScript::Function_8E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8E - Jump");
	pState->ExecutionPointer = pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2));
}

void CUAKMScript::Function_90(CScriptState* pState)
{
	DebugTrace(pState, L"Function_90 - Set Item State");

	CGameController::SetItemState(pState->Script[pState->ExecutionPointer], pState->Script[pState->ExecutionPointer + 1]);
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_91(CScriptState* pState)
{
	DebugTrace(pState, L"Function_91 - Jump on item state");
	int bix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	BYTE cmp = pState->Script[pState->ExecutionPointer + 2];
	int ns = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	pState->ExecutionPointer = (CGameController::GetItemState(bix) == cmp) ? pState->GetScript(ns) : pState->ExecutionPointer + 6;
}

void CUAKMScript::Function_93(CScriptState* pState)
{
	DebugTrace(pState, L"Function_93 - Load Location");
	// Load files from MAP.LZ

	CAmbientAudio::Clear();
	CGameController::SetParameter(252, 0);

	pMIDI->Stop();

	int locationId = pState->Script[pState->ExecutionPointer];
	CGameController::SetData(UAKM_SAVE_MAP_ENTRY, locationId);
	CGameController::SetData(UAKM_SAVE_DMAP_FLAG, (BYTE)0);

	CGameController::AutoSave();

	CModuleController::Push(new CLocationModule(locationId, CGameController::GetParameter(249)));
	pState->ExecutionPointer = -1;	// This should be the end of the script, but check anyway...
}

void CUAKMScript::Function_96(CScriptState* pState)
{
	DebugTrace(pState, L"Function_96 - Jump on Animation Completed");
	pState->ExecutionPointer = (_pLoc != NULL && _pLoc->IsAnimationFinished(pState->Script[pState->ExecutionPointer])) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer + 2, 2)) : pState->ExecutionPointer + 4;
}

void CUAKMScript::Function_97(CScriptState* pState)
{
	DebugTrace(pState, L"Function_97 - Play Sound");
	PlayAudio(pState, pState->Script[pState->ExecutionPointer++] - 1);

	// pState->WaitingForMediaToFinish = TRUE;
}

void CUAKMScript::Function_99(CScriptState* pState)
{
	DebugTrace(pState, L"Function_99 - If Query Action = true, end script");
	if (pState->QueryAction)
	{
		pState->ExecutionPointer = -1;
	}
}

void CUAKMScript::Function_9D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_9D - Set Ask About State");
	int aa = GetInt(pState->Script, pState->ExecutionPointer, 1);
	BYTE aastate = pState->Script[pState->ExecutionPointer + 1];
	CGameController::SetAskAboutState(aa, aastate);
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_A1(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A1 - If Action=On/Off jump to script");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::OnOff) ? pState->GetScript(GetInt(pState->Script, pState->ExecutionPointer, 2)) : pState->ExecutionPointer + 2;
}

void CUAKMScript::Function_A5(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A5 - Set Embedded Video Mode");
	videoMode = VideoMode::Embedded;
}

void CUAKMScript::Function_A7(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A7 - Play PTF");
	//int offx = GetInt(pState->Script, pState->ExecutionPointer, 2);
	//int offy = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int index = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int rate = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	pState->ExecutionPointer += 8;

	Play(pState, index, 1, rate);
}

void CUAKMScript::Function_AA(CScriptState* pState)
{
	DebugTrace(pState, L"Function_AA - Set Fullscreen Video Mode");
	videoMode = VideoMode::FullScreen;
}

void CUAKMScript::Function_B6(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B6 - Allow Action");

	int action = GetInt(pState->Script, pState->ExecutionPointer, 2);
	if (action == 1) pState->AllowedAction |= ActionType::Look;
	else if (action == 2)  pState->AllowedAction |= ActionType::Move;
	else if (action == 3)  pState->AllowedAction |= ActionType::Open;
	else if (action == 4)  pState->AllowedAction |= ActionType::Get;
	else if (action == 5)  pState->AllowedAction |= ActionType::Talk;
	else if (action == 6)  pState->AllowedAction |= ActionType::OnOff;
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_B8(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B8 - Jump on Selected Option = X");
	int option = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int ns = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	pState->ExecutionPointer = (pState->SelectedOption == option) ? pState->GetScript(ns) : pState->ExecutionPointer + 4;
}

void CUAKMScript::Function_BA(CScriptState* pState)
{
	DebugTrace(pState, L"Function_BA - Clear allowed actions");
	pState->AllowedAction = ActionType::None;
}

void CUAKMScript::Function_C4(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C4 - Hide location object");
	if (_pLoc != NULL)
	{
		_pLoc->SetObjectVisibility(GetInt(pState->Script, pState->ExecutionPointer, 2), FALSE);
	}
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_C5(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C5 - Show location object");
	if (_pLoc != NULL)
	{
		_pLoc->SetObjectVisibility(GetInt(pState->Script, pState->ExecutionPointer, 2), TRUE);
	}
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_C9(CScriptState* pState)
{
	DebugTrace(pState, L"Function_C9 - Add Score");
	CGameController::AddScore(GetInt(pState->Script, pState->ExecutionPointer, 2));
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_CD(CScriptState* pState)
{
	DebugTrace(pState, L"Function_CD - Set Frame Trigger");
	pState->FrameTrigger = GetInt(pState->Script, pState->ExecutionPointer, 2);
	pState->ExecutionPointer += 2;
}

void CUAKMScript::Function_CE(CScriptState* pState)
{
	DebugTrace(pState, L"Function_CE - Print at Frame Trigger (Tex)");
	AddCaption(pState, TRUE);
}

void CUAKMScript::AddCaption(CScriptState* pState, BOOL TexTalk)
{
	// Print at frame trigger (ce = Tex talks, cf = others talk)
	char* pST = (char*)(pState->Script + pState->ExecutionPointer);

	while (pState->Script[pState->ExecutionPointer++] != 0);
	pAddCaptions->push_back(new CCaption(pState->FrameTrigger, pST, TexTalk));
}

void CUAKMScript::Function_CF(CScriptState* pState)
{
	DebugTrace(pState, L"Function_CF - Print at Frame Trigger (Other)");
	AddCaption(pState, FALSE);
}

void CUAKMScript::Function_D7(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D7 - Pause MIDI");
	pMIDI->Pause();
}

void CUAKMScript::Function_D8(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D8 - Resume MIDI");
	pMIDI->Resume();
}

void CUAKMScript::Function_D9(CScriptState* pState)
{
	DebugTrace(pState, L"Function_D9 - Stop MIDI");
	pMIDI->Stop();
}

void CUAKMScript::Function_E0(CScriptState* pState)
{
	DebugTrace(pState, L"Function_E0 - Script ID");
	pState->ExecutionPointer += 2;	// Skip ID
}

void CUAKMScript::Play(CScriptState* pState, int index, int bank, int rate)
{
	// TODO: Pass rate on to animation loader

	// Swap caption pointers
	SwapCaptions();

	if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		if (bank == 1)
		{
			FileMap fm = _mapEntry->VideoMap.at(index);
			std::wstring fn = CGameController::GetFileName(fm.File);
			if (fn != L"")
			{
				//CModuleController::Push(new CVideoModule(VideoType::Scripted, fn, mm.FileEntry));
				CAnimationController::Load(fn.c_str(), fm.Entry);
			}
		}
		else
		{
			FileMap fm = _mapEntry->AudioMap.at(index);
			std::wstring fn = CGameController::GetFileName(fm.File);
			if (fn != L"")
			{
				CAnimationController::Load(fn.c_str(), fm.Entry);
			}
		}
	}
	else if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		FileMap fm;
		if (bank == 0)
		{
			fm = _mapEntry->AudioMap.at(index);
		}
		else if (bank == 1)
		{
			fm = _mapEntry->VideoMap.at(index);
		}

		std::wstring fn = CGameController::GetFileName(fm.File);
		if (fn != L"")
		{
			CAnimationController::Load(fn.c_str(), fm.Entry);
		}
	}

	pState->WaitingForMediaToFinish = TRUE;
}

void CUAKMScript::PlayAudio(CScriptState* pState, int index)
{
	CAmbientAudio::Play(_mapEntry, index, TRUE);
}

ActionType CUAKMScript::GetCurrentActions(CScriptState* pState, int currentObjectIndex)
{
	if (currentObjectIndex >= 0)
	{
		// Find script with id same as current object index
		pState->ExecutionPointer = pState->GetScript(currentObjectIndex);
		if (pState->ExecutionPointer >= 0)
		{
			// Set query action flag
			pState->QueryAction = TRUE;

			// Execute script
			Resume(pState, TRUE);

			// Clear query action flag
			pState->QueryAction = FALSE;
		}
	}

	return pState->AllowedAction;
}

void CUAKMScript::Show(CScriptState* pState, int index)
{
	if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		FileMap fm = _mapEntry->ImageMap.at(index);
		pState->WaitingForMediaToFinish = TRUE;	// Set to break out of the script loop
		CModuleController::Push(new CPictureModule(fm.File, fm.Entry, this, pState));
	}
}

void CUAKMScript::SelectDialogueOption(CScriptState* pState, int option)
{
	// 0x9c enables inventory
	// if a[253]=1 then ask about

	if (pState->Mode == InteractionMode::Offer && option == 1)
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
			pState->Mode = InteractionMode::None;
			pState->SelectedOption = option;
			Resume(pState, TRUE);
		}
	}
}
