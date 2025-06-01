#include "PDScript.h"
#include "Globals.h"
#include "Utilities.h"
#include "AnimationController.h"
#include "GameBase.h"
#include "LocationModule.h"
#include "PDLocationModule.h"
#include "VideoModule.h"
#include "GameController.h"
#include "AmbientAudio.h"
#include "UAKMGame.h"
#include "MainMenuModule.h"
#include "PictureModule.h"
#include "PDGame.h"
#include <algorithm>
#include "PDVidPhoneModule.h"
#include "PDRitzSecurityKeypadModule.h"
#include "PDTravelModule.h"

CPDScript::CPDScript()
{
	_functions[0x00] = &CPDScript::Function_00;
	_functions[0x01] = &CPDScript::Function_01;
	_functions[0x02] = &CPDScript::Function_02;
	_functions[0x03] = &CPDScript::Function_03;
	_functions[0x04] = &CPDScript::Function_04;
	_functions[0x05] = &CPDScript::Function_05;
	_functions[0x06] = &CPDScript::Function_06;
	_functions[0x07] = &CPDScript::Function_07;
	_functions[0x08] = &CPDScript::Function_08;
	_functions[0x09] = &CPDScript::Function_09;
	_functions[0x0a] = &CPDScript::Function_0A;
	_functions[0x0b] = &CPDScript::Function_0B;
	_functions[0x0c] = &CPDScript::Function_0C;
	_functions[0x0d] = &CPDScript::Function_0D;
	_functions[0x0e] = &CPDScript::Function_0E;
	_functions[0x0f] = &CPDScript::Function_0F;
	_functions[0x10] = &CPDScript::Function_10;
	_functions[0x11] = &CPDScript::Function_11;
	_functions[0x12] = &CPDScript::Function_12;
	_functions[0x13] = &CPDScript::Function_13;
	_functions[0x14] = &CPDScript::Function_14;
	_functions[0x15] = &CPDScript::Function_15;
	_functions[0x16] = &CPDScript::Function_16;
	_functions[0x17] = &CPDScript::Function_17;
	_functions[0x18] = &CPDScript::Function_18;
	_functions[0x19] = &CPDScript::Function_19;
	_functions[0x1a] = &CPDScript::Function_1A;
	_functions[0x1b] = &CPDScript::Function_1B;
	_functions[0x1c] = &CPDScript::Function_1C;
	_functions[0x1d] = &CPDScript::Function_1D;
	_functions[0x1e] = &CPDScript::Function_1E;
	_functions[0x1f] = &CPDScript::Function_1F;
	_functions[0x20] = &CPDScript::Function_20;
	_functions[0x21] = &CPDScript::Function_21;
	_functions[0x22] = &CPDScript::Function_22;
	_functions[0x23] = &CPDScript::Function_23;
	_functions[0x24] = &CPDScript::Function_24;
	_functions[0x25] = &CPDScript::Function_25;
	_functions[0x26] = &CPDScript::Function_26;
	_functions[0x27] = &CPDScript::Function_27;
	_functions[0x28] = &CPDScript::Function_28;
	_functions[0x29] = &CPDScript::Function_29;
	_functions[0x2a] = &CPDScript::Function_2A;
	_functions[0x2b] = &CPDScript::Function_2B;
	_functions[0x2c] = &CPDScript::Function_2C;
	_functions[0x2d] = &CPDScript::Function_2D;
	_functions[0x2e] = &CPDScript::Function_2E;
	_functions[0x2f] = &CPDScript::Function_2F;
	_functions[0x30] = &CPDScript::Function_30;
	_functions[0x31] = &CPDScript::Function_31;
	_functions[0x32] = &CPDScript::Function_32;
	_functions[0x33] = &CPDScript::Function_33;
	_functions[0x34] = &CPDScript::Function_34;
	_functions[0x35] = &CPDScript::Function_35;
	_functions[0x36] = &CPDScript::Function_36;
	_functions[0x37] = &CPDScript::Function_37;
	_functions[0x38] = &CPDScript::Function_38;
	_functions[0x39] = &CPDScript::Function_39;
	_functions[0x3a] = &CPDScript::Function_3A;
	_functions[0x3b] = &CPDScript::Function_3B;
	_functions[0x3c] = &CPDScript::Function_3C;
	_functions[0x3d] = &CPDScript::Function_3D;
	_functions[0x3e] = &CPDScript::Function_3E;
	_functions[0x3f] = &CPDScript::Function_3F;
	_functions[0x40] = &CPDScript::Function_40;
	_functions[0x41] = &CPDScript::Function_41;
	_functions[0x42] = &CPDScript::Function_42;
	_functions[0x43] = &CPDScript::Function_43;
	_functions[0x44] = &CPDScript::Function_44;
	_functions[0x45] = &CPDScript::Function_45;
	_functions[0x46] = &CPDScript::Function_46;
	_functions[0x47] = &CPDScript::Function_47;
	_functions[0x48] = &CPDScript::Function_48;
	_functions[0x49] = &CPDScript::Function_49;
	_functions[0x4a] = &CPDScript::Function_4A;
	_functions[0x4b] = &CPDScript::Function_4B;
	_functions[0x4c] = &CPDScript::Function_4C;
	_functions[0x4d] = &CPDScript::Function_4D;
	_functions[0x4e] = &CPDScript::Function_4E;
	_functions[0x4f] = &CPDScript::Function_4F;
	_functions[0x50] = &CPDScript::Function_50;
	_functions[0x51] = &CPDScript::Function_51;
	_functions[0x52] = &CPDScript::Function_52;
	_functions[0x53] = &CPDScript::Function_53;
	_functions[0x54] = &CPDScript::Function_54;
	_functions[0x55] = &CPDScript::Function_55;
	_functions[0x56] = &CPDScript::Function_56;
	_functions[0x57] = &CPDScript::Function_57;
	_functions[0x58] = &CPDScript::Function_58;
	_functions[0x59] = &CPDScript::Function_59;
	_functions[0x5a] = &CPDScript::Function_5A;
	_functions[0x5b] = &CPDScript::Function_5B;
	_functions[0x5c] = &CPDScript::Function_5C;
	_functions[0x5d] = &CPDScript::Function_5D;
	_functions[0x5e] = &CPDScript::Function_5E;
	_functions[0x5f] = &CPDScript::Function_5F;
	_functions[0x60] = &CPDScript::Function_60;
	_functions[0x61] = &CPDScript::Function_61;
	_functions[0x62] = &CPDScript::Function_62;
	_functions[0x63] = &CPDScript::Function_63;
	_functions[0x64] = &CPDScript::Function_64;
	_functions[0x65] = &CPDScript::Function_65;
	_functions[0x66] = &CPDScript::Function_66;
	_functions[0x67] = &CPDScript::Function_67;
	_functions[0x68] = &CPDScript::Function_68;
	_functions[0x69] = &CPDScript::Function_69;
	_functions[0x6a] = &CPDScript::Function_6A;
	_functions[0x6b] = &CPDScript::Function_6B;
	_functions[0x6c] = &CPDScript::Function_6C;
	_functions[0x6d] = &CPDScript::Function_6D;
	_functions[0x6e] = &CPDScript::Function_6E;
	_functions[0x6f] = &CPDScript::Function_6F;
	_functions[0x70] = &CPDScript::Function_70;
	_functions[0x71] = &CPDScript::Function_71;
	_functions[0x72] = &CPDScript::Function_72;
	_functions[0x73] = &CPDScript::Function_73;
	_functions[0x74] = &CPDScript::Function_74;
	_functions[0x75] = &CPDScript::Function_75;
	_functions[0x76] = &CPDScript::Function_76;
	_functions[0x77] = &CPDScript::Function_77;
	_functions[0x78] = &CPDScript::Function_78;
	_functions[0x79] = &CPDScript::Function_79;
	_functions[0x7a] = &CPDScript::Function_7A;
	_functions[0x7b] = &CPDScript::Function_7B;
	_functions[0x7c] = &CPDScript::Function_7C;
	_functions[0x7d] = &CPDScript::Function_7D;
	_functions[0x7e] = &CPDScript::Function_7E;
	_functions[0x7f] = &CPDScript::Function_7F;
	_functions[0x80] = &CPDScript::Function_80;
	_functions[0x81] = &CPDScript::Function_81;
	_functions[0x82] = &CPDScript::Function_82;
	_functions[0x83] = &CPDScript::Function_83;
	_functions[0x84] = &CPDScript::Function_84;
	_functions[0x85] = &CPDScript::Function_85;
	_functions[0x86] = &CPDScript::Function_86;
	_functions[0x87] = &CPDScript::Function_87;
	_functions[0x88] = &CPDScript::Function_88;
	_functions[0x89] = &CPDScript::Function_89;
	_functions[0x8a] = &CPDScript::Function_8A;
	_functions[0x8b] = &CPDScript::Function_8B;
	_functions[0x8c] = &CPDScript::Function_8C;
	_functions[0x8d] = &CPDScript::Function_8D;
	_functions[0x8e] = &CPDScript::Function_8E;
	_functions[0x8f] = &CPDScript::Function_8F;
	_functions[0x90] = &CPDScript::Function_90;
	_functions[0x91] = &CPDScript::Function_91;
	_functions[0x92] = &CPDScript::Function_92;

	_alternatingIndex = 0;
}

CPDScript::~CPDScript()
{
}

void CPDScript::PermformAction(CScriptState* pState, int id, ActionType action, int item)
{
	//pState->ExecutionPointer = pState->GetScript(id);
	pState->CurrentAction = action;
	if (item > 0)
	{
		pState->CurrentAction = ActionType::Use;
		pState->SelectedValue = item;
	}
	//CGameController::SetParameter(99, item);
	Execute(pState, id + 2);
}

void CPDScript::Execute(CScriptState* pState, int id)
{
	// Find execution pointer and call resume
	pState->ExecutionPointer = pState->GetScript(id);
	pState->WaitingForMediaToFinish = FALSE;
	pState->LastDialoguePoint = 0;
	Resume(pState);
}

void CPDScript::Resume(CScriptState* pState, BOOL breakWait)
{
	_scriptLock.Lock();
	CModuleBase* pThisModule = CModuleController::CurrentModule;

	if (breakWait)
	{
		pState->WaitingForMediaToFinish = FALSE;
		pState->WaitingForInput = FALSE;
		pState->WaitingForExternalModule = FALSE;
	}

	while (CModuleController::CurrentModule == pThisModule && pState->ExecutionPointer > -1 && pState->ExecutionPointer < pState->Length && pState->Script != NULL && !pState->WaitingForMediaToFinish && !pState->WaitingForInput && !pState->WaitingForExternalModule)
	{
		// Execute script
		byte cmd = pState->Script[pState->ExecutionPointer++];
		if (cmd < 0 || cmd > 0x92)
		{
			// This should never happen, terminate script
			DebugTrace(pState, L"Invalid command, terminating script");
			pState->ExecutionPointer = -1;
		}
		else
		{
			(this->*_functions[cmd])(pState);
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

void CPDScript::Function_00(CScriptState* pState)
{
	DebugTrace(pState, L"Function_00 - End script");
	pState->ExecutionPointer = -1;
}

void CPDScript::Function_01(CScriptState* pState)
{
	DebugTrace(pState, L"Function_01");
	pState->ExecutionPointer = -1;
}

void CPDScript::Function_02(CScriptState* pState)
{
	DebugTrace(pState, L"Function_02 - If Action = Use, play \"Well, that won't work.\" and end script");

	if (pState->CurrentAction == ActionType::Use)
	{
		pAddCaptions->clear();

		char* text[2] = { "Well, that won't work.", "That doesn't seem to do anything." };

		pAddCaptions->push_back(new CCaption(0, text[_alternatingIndex], TRUE));

		SwapCaptions();

		CAnimationController::Load(L"SOUND.AP", _alternatingIndex);

		_alternatingIndex = (_alternatingIndex + 1) & 1;
		pState->WaitingForMediaToFinish = TRUE;
	}

	pState->ExecutionPointer = -1;
}

void CPDScript::Function_03(CScriptState* pState)
{
	DebugTrace(pState, L"Function_03 - If Action = Look jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Look) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_04(CScriptState* pState)
{
	DebugTrace(pState, L"Function_04");
	pState->ExecutionPointer += 2;	// Jump to address if skip requested (enter pressed)
}

void CPDScript::Function_05(CScriptState* pState)
{
	DebugTrace(pState, L"Function_05 - If Action = Get jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Get) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_06(CScriptState* pState)
{
	DebugTrace(pState, L"Function_06 - If Action = Move jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Move) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_07(CScriptState* pState)
{
	DebugTrace(pState, L"Function_07 - If Action = Use jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Use) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_08(CScriptState* pState)
{
	DebugTrace(pState, L"Function_08 - If Action = Talk jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Talk) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_09(CScriptState* pState)
{
	DebugTrace(pState, L"Function_09 - If Action = Open jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Open) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_0A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0A - Print");
	// TODO: Fix this, it is not correct
	pState->FrameTrigger = 1;
	AddCaption(pState, TRUE);
}

void CPDScript::AddCaption(CScriptState* pState, BOOL TexTalk)
{
	// Print at frame trigger
	int stringOffset = pState->Read16();
	pAddCaptions->push_back(new CCaption(pState->FrameTrigger, (char*)(pState->Script + stringOffset), TexTalk));
}

void CPDScript::Function_0B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0B - Start Animation");

	int anim = pState->Read8();
	if (_pLoc != NULL)
	{
		_pLoc->StartMappedAnimation(anim);
	}
}

void CPDScript::Function_0C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0C - Set Parameter");

	// Set parameter
	int index = pState->Read16();
	int value = pState->Read8();

	//if (index == 252)
	//{
	//	if (value == 1 && CGameController::GetParameter(index) == 0)
	//	{
	//		pState->Offer = FALSE;
	//		pState->AskAbout = FALSE;
	//		pState->TopItemOffset = -1;
	//	}
	//}
	//else if (index == 100)
	//{
	//	pState->OfferMode = (value != 0);
	//	pState->AskAbout = FALSE;
	//	pState->Offer = FALSE;
	//	pState->TopItemOffset = -1;
	//}

	CGameController::SetParameter(index, (byte)value);
}

void CPDScript::Function_0D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0D - Jump on Parameter[X]=Y");

	int ix = pState->Read16();
	int cmp = pState->Read8();
	int address = pState->Read16();

	if (CGameController::GetParameter(ix) == cmp)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_0E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0E - Jump to offset");
	pState->ExecutionPointer = pState->Read16();
}

void CPDScript::Function_0F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0F - Set Player Position");

	int ix = pState->Read16s();
	int iy = pState->Read16s();
	int iz = pState->Read16s();
	int ia = pState->Read16s();

	Point ppos = _pLoc->GetPlayerPosition();

	float x = (ix == -1) ? -ppos.X : -From12_4(ix);
	float y = (iy == -1) ? ppos.Y : -From12_4(iy);
	float z = (iz == -1) ? -ppos.Z : -From12_4(iz);
	float angle = (ia == -1) ? CLocation::_angle2 : -(XM_PI * ia) / 1800.0f;

	CLocation::SetPosition(x, y, z, angle);
}

void CPDScript::Function_10(CScriptState* pState)
{
	DebugTrace(pState, L"Function_10 - Set item state");

	int itemIndex = pState->Read16();
	int itemState = pState->Read8();
	CGameController::SetItemState(itemIndex, itemState);
}

void CPDScript::Function_11(CScriptState* pState)
{
	DebugTrace(pState, L"Function_11 - Jump on Item State X = Y");
	// word, byte, word

	int item = pState->Read16();
	int state = pState->Read8();
	int address = pState->Read16();

	if (CGameController::GetItemState(item) == state)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_12(CScriptState* pState)
{
	DebugTrace(pState, L"Function_12");
	pState->ExecutionPointer = -1;
}

void CPDScript::Function_13(CScriptState* pState)
{
	DebugTrace(pState, L"Function_13 - Load Location");

	CAmbientAudio::Clear();
	//CGameController::SetParameter(252, 0);

	int locationId = pState->Read8();
	int startupPosition = pState->Read8();
	int unknown = pState->Read8();
	//CGameController::SetData(UAKM_SAVE_MAP_ENTRY, locationId);

	if (startupPosition == 0xff)
	{
		startupPosition = CGameController::GetData(PD_SAVE_STARTUP_POSITION);
	}

	if (unknown != 0xff)
	{
		CGameController::SetParameter(241, unknown);	// Will always be set to 0 later in LoadLocation
	}

	CGameController::SetParameter(252, 1);
	//CGameController::SetData(PD_SAVE_MAP_ENTRY_A, locationId);
	CGameController::SetData(PD_SAVE_LOCATION_ID, locationId);
	CGameController::SetData(PD_SAVE_STARTUP_POSITION, startupPosition);
	//CGameController::SetData(PD_SAVE_MAP_FLAG_A, 1);

	CGameController::AutoSave();

	CModuleController::Push(new CPDLocationModule(locationId, startupPosition));
	pState->ExecutionPointer = -1;
}

void CPDScript::Function_14(CScriptState* pState)
{
	DebugTrace(pState, L"Function_14 - Load file");

	CAmbientAudio::Clear();
	pMIDI->Stop();
	//CGameController::SetParameter(252, 0);

	// Conversations & media
	// Load files from DMAP.LZ
	int ix = pState->Read8();
	int unknown = pState->Read8();

	if (unknown != 0xff)
	{
		CGameController::SetParameter(241, unknown);	// Will always be set to 0 later in LoadLocation
	}

	//CGameController::SetData(PD_SAVE_DMAP_ENTRY_A, ix);
	//CGameController::SetData(PD_SAVE_DMAP_ENTRY_B, ix);
	//CGameController::SetData(PD_SAVE_MAP_FLAG_A, 0);
	CGameController::SetData(PD_SAVE_DMAP_ID, ix);
	CGameController::SetData(PD_SAVE_SCRIPT_ID, (BYTE)0);
	CGameController::SetParameter(252, 0);

	CGameController::AutoSave();

	CModuleController::Push(new CVideoModule(VideoType::Scripted, ix, unknown));

	pState->ExecutionPointer = -1;
}

void CPDScript::Function_15(CScriptState* pState)
{
	DebugTrace(pState, L"Function_15 - Jump if animation at frame");

	int anim = pState->Read8();
	int frame = pState->Read16();
	int address = pState->Read16();

	if (_pLoc != NULL && _pLoc->GetAnimationFrame(anim) == frame)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_16(CScriptState* pState)
{
	DebugTrace(pState, L"Function_16 - Jump if animation ended");
	int anim = pState->Read8();
	int address = pState->Read16();

	if (_pLoc != NULL && _pLoc->IsIndexedAnimationFinished(anim))
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_17(CScriptState* pState)
{
	// This will NOT wait for media completion
	DebugTrace(pState, L"Function_17 - Play Sound");
	int sound = pState->Read8();
	PlaySound(sound - 1);
	pState->WaitingForMediaToFinish = FALSE;
}

void CPDScript::Function_18(CScriptState* pState)
{
	DebugTrace(pState, L"Function_18 - Jump on player Y > value");

	// word, word
	float ylimit = pState->Read12_4();
	int address = pState->Read16();
	float playerY = -_pLoc->GetUnadjustedPlayerPosition().Y;
	if (playerY > ylimit)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_19(CScriptState* pState)
{
	DebugTrace(pState, L"Function_19 - Exit script if not querying actions");
	if (pState->QueryAction)
	{
		pState->ExecutionPointer = -1;
	}
}

void CPDScript::Function_1A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1A - Jump if player inside rectangle");

	float f1 = pState->Read12_4();
	float f2 = pState->Read12_4();
	float f3 = pState->Read12_4();
	float f4 = pState->Read12_4();
	int address = pState->Read16();

	float x1 = min(f1, f3);
	float x2 = max(f1, f3);
	float z1 = min(f2, f4);
	float z2 = max(f2, f4);

	if (_pLoc != NULL)
	{
		Point p = _pLoc->GetPlayerPosition();
		if (p.X >= x1 && p.X <= x2 && p.Z >= z1 && p.Z <= z2)
		{
			pState->ExecutionPointer = address;
		}
	}
}

void CPDScript::Function_1B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1B");

	// byte, byte, word
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}, {GetInt(data, offset + 2, 2):X4}";
	pState->ExecutionPointer += 4;
}

void CPDScript::Function_1C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1C");
	//text += $"???";
}

void CPDScript::Function_1D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1D - Set AskAboutState[X] to Y");
	// byte, byte
	int askAboutIndex = pState->Read8();
	int state = pState->Read8();
	CGameController::SetAskAboutState(askAboutIndex, state);
}

void CPDScript::Function_1E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1E - Set Timer");

	// byte, word
	//text += $"Set Timer {GetInt(data, offset, 1):X2} to {GetInt(data, offset + 1, 2) * 16.6666666667:0} ms";

	int timer = pState->Read8();
	int duration = pState->Read16();
	CGameController::SetTimer(timer, duration);
}

void CPDScript::Function_1F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1F - Jump on timer state");

	// byte, word, word
	int timer = pState->Read8();
	int compare = pState->Read16() & 0xff;
	int address = pState->Read16();
	if (CGameController::GetTimerState(timer) == compare)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_20(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A0 - Change Travel Location state");

	int location = pState->Read8();
	int state = pState->Read8();
	CGameController::SetData(PD_SAVE_TRAVEL + location, state);
}

void CPDScript::Function_21(CScriptState* pState)
{
	DebugTrace(pState, L"Function_21 - If Action = On/Off jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::OnOff) ? pState->Read16() : pState->ExecutionPointer + 2;
}

void CPDScript::Function_22(CScriptState* pState)
{
	DebugTrace(pState, L"Function_22 - Play Cached Sound Buffer");

	int buffer = pState->Read8();
	int  unknown = pState->Read8();

	CAmbientAudio::LoopPD(buffer - 1);
}

void CPDScript::Function_23(CScriptState* pState)
{
	DebugTrace(pState, L"Function_23");

	// byte
	//text += $"??? {GetInt(data, offset, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_24(CScriptState* pState)
{
	DebugTrace(pState, L"Function_24");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_25(CScriptState* pState)
{
	DebugTrace(pState, L"Function_25 - Set Embedded Video Mode");
	videoMode = VideoMode::Embedded;
}

void CPDScript::Function_26(CScriptState* pState)
{
	DebugTrace(pState, L"Function_26 - Add Cash");

	// word
	int cashToAdd = pState->Read16();
	int currentCash = CGameController::GetWord(PD_SAVE_CASH);
	CGameController::SetWord(PD_SAVE_CASH, currentCash + cashToAdd);
}

void CPDScript::Function_27(CScriptState* pState)
{
	DebugTrace(pState, L"Function_27 - Play Video");
	int x = pState->Read16();
	int y = pState->Read16();
	int entry = pState->Read8();
	int rate = pState->Read16();
	PlayVideo(entry, rate);
	pState->WaitingForMediaToFinish = TRUE;
}

void CPDScript::Function_28(CScriptState* pState)
{
	DebugTrace(pState, L"Function_28");
	// byte
	int script = pState->Read8();
	//text += $"Set OnReturnContinueAtScriptID = {GetInt(data, offset++, 1):X2}";
}

void CPDScript::Function_29(CScriptState* pState)
{
	DebugTrace(pState, L"Function_29 - Remove Cash");

	// word
	int cashToRemove = pState->Read16();
	int currentCash = CGameController::GetWord(PD_SAVE_CASH);
	CGameController::SetWord(PD_SAVE_CASH, currentCash - cashToRemove);
}

void CPDScript::Function_2A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2A");
	//text += $"???";
}

void CPDScript::Function_2B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2B");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_2C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2C");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_2D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2D - Travel");
	CModuleController::Push(new CPDTravelModule());
	pState->WaitingForExternalModule = TRUE;
}

void CPDScript::Function_2E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2E - If AskAboutState[x]=y go to z");

	// byte, byte, word
	int ix = pState->Read8();
	int val = pState->Read8();
	int address = pState->Read16();

	if (CGameController::GetAskAboutState(ix) == val)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_2F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2F - Special function");

	int function = pState->Read8();
	int p1 = pState->Read16s();
	int p2 = pState->Read16s();

	switch (function)
	{
		// From 0 to 45
		case 5:
		{
			// Vidphone
			CModuleController::Push(new CPDVidPhoneModule());
			pState->WaitingForInput = TRUE;
			break;
		}
		case 34:
		{
			// Ritz door security keypad
			CModuleController::Push(new CPDRitzSecurityKeypadModule());
			pState->WaitingForInput = TRUE;
			break;
		}
		case 42:
		{
			// Animate elevation
			pElevationModOverlay->SetData(p1, p2);
			pOverlay = pElevationModOverlay;
			pState->WaitingForInput = TRUE;
			break;
		}
		default:
		{
			break;
		}
	}
}

void CPDScript::Function_30(CScriptState* pState)
{
	DebugTrace(pState, L"Function_30");

	// word, word, word
	//text += $"??? {GetInt(data, offset, 2):X4}, {GetInt(data, offset + 2, 2):X4}, {GetInt(data, offset + 4, 2):X4}";
	pState->ExecutionPointer += 6;
}

void CPDScript::Function_31(CScriptState* pState)
{
	DebugTrace(pState, L"Function_31");

	//text += $"???";
	pState->ExecutionPointer++;
}

void CPDScript::Function_32(CScriptState* pState)
{
	DebugTrace(pState, L"Function_32");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_33(CScriptState* pState)
{
	DebugTrace(pState, L"Function_33");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_34(CScriptState* pState)
{
	DebugTrace(pState, L"Function_34 - Display options");

	// word, word, word

	pState->LastDialoguePoint = pState->ExecutionPointer - 1;

	int oldOptionValues[3];
	int askIndex = -1, buyIndex = -1, offerIndex = -1;

	char* pT[3];
	for (int i = 0; i < 3; i++)
	{
		int v = i + 1;
		int stringOffset = pState->Read16();
		pT[i] = (char*)(pState->Script + stringOffset);

		if (pT[i] != NULL && pT[i][0] == '^')
		{
			v = atoi(pT[i] + 1);
			// Get correct string (next in list)
			while (pT[i][0] != 0) pT[i]++;
			pT[i]++;
		}

		oldOptionValues[i] = DialogueOptions[i].GetValue();
		DialogueOptions[i].SetValue(v);

		if (v == 4)
		{
			askIndex = i;
		}
		else if (v == 6)
		{
			buyIndex = i;
		}
		else if (v == 7)
		{
			offerIndex = i;
		}
		/*
				pState->Mode = InteractionMode::AskAbout;
				pState->Mode = InteractionMode::Buy;
		*/
	}

	DialogueOptionsCount = (strlen(pT[2]) == 0) ? (strlen(pT[1]) == 0) ? 1 : 2 : 3;

	float w = 0.0f;
	float h = 0.0f;
	// Make a max size based on number of dialogue options, calculate required size for all items
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

	// TODO: If AskAbout not available should set flag to false
	if (pState->AskAbout && (askIndex < 0 || oldOptionValues[askIndex] != 4))
	{
		pState->AskAbout = FALSE;
	}

	// TODO: If Buy not available should set flag to false
	if (pState->Buy && (buyIndex < 0 || oldOptionValues[buyIndex] != 6))
	{
		pState->Buy = FALSE;
	}

	// TODO: If Offer not available should set flag to false
	if (pState->Offer && (offerIndex < 0 || oldOptionValues[offerIndex] != 7))
	{
		pState->Offer = FALSE;
	}

	// TODO: Should also move any open combobox if required
}

void CPDScript::Function_35(CScriptState* pState)
{
	DebugTrace(pState, L"Function_35");

	// Sleep?
	// word
	//text += $"??? {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_36(CScriptState* pState)
{
	DebugTrace(pState, L"Function_36 - Set Allowed Actions");

	pState->AllowedAction = ActionType::None;
	int actions = pState->Read8();
	if (actions & 1) pState->AllowedAction |= ActionType::Look;
	if (actions & 2) pState->AllowedAction |= ActionType::Get;
	if (actions & 4) pState->AllowedAction |= ActionType::Move;
	if (actions & 8) pState->AllowedAction |= ActionType::Open;
	if (actions & 16) pState->AllowedAction |= ActionType::Talk;
	if (actions & 32) pState->AllowedAction |= ActionType::OnOff;
}

void CPDScript::Function_37(CScriptState* pState)
{
	DebugTrace(pState, L"Function_37 - Check cash");

	// word, word
	int cashTest = pState->Read16();
	int address = pState->Read16();

	int currentCash = CGameController::GetWord(PD_SAVE_CASH);

	if (currentCash >= cashTest)
	{
		pState->ExecutionPointer = address;
	}
	else
	{
		// Display modal window asking to convert points
		pOverlay = pConvertPointsOverlay;
		pState->WaitingForInput = TRUE;
	}
}

void CPDScript::Function_38(CScriptState* pState)
{
	DebugTrace(pState, L"Function_38 - Jump on Selected Option = X");
	int check = pState->Read8();
	int address = pState->Read16();

	if (pState->SelectedOption == check)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_39(CScriptState* pState)
{
	DebugTrace(pState, L"Function_B9 - Return to dialogue/options");
	pState->ExecutionPointer = pState->LastDialoguePoint;
}

void CPDScript::Function_3A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3A - Identify gender of Ask About");

	if (_women.find(pState->SelectedValue) != _women.end())
	{
		// She
		CGameController::SetParameter(0, 0);
	}
	else if (_men.find(pState->SelectedValue) != _men.end())
	{
		// He
		CGameController::SetParameter(0, 1);
	}
	else
	{
		// It
		CGameController::SetParameter(0, 2);
	}
}

void CPDScript::Function_3B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3B - Load Audio Buffer to Cache");

	int entry = pState->Read8();
	int buffer = pState->Read8();

	CAmbientAudio::LoadPD(_mapEntry, entry, buffer - 1);
}

void CPDScript::Function_3C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3C");

	// byte
	//text += $"??? {GetInt(data, offset++, 1):X2}";
}

void CPDScript::Function_3D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3D");
	//pState->ExecutionPointer = -1;
	// Travel?
}

void CPDScript::Function_3E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3E - Jump if ask about/offer/use item");

	// word, word, word
	// Ask about/offer
	int type = pState->Read16();
	int index = pState->Read16();
	int address = pState->Read16();

	if (pState->SelectedValue == index)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_3F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3F - Jump if Item not acquired");

	// word, word

	int itemIndex = pState->Read16();
	int address = pState->Read16();
	if (CGameController::GetItemState(itemIndex) == 0)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_40(CScriptState* pState)
{
	DebugTrace(pState, L"Function_40");
	//byte
	//text += $"??? {GetInt(data, offset++, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_41(CScriptState* pState)
{
	DebugTrace(pState, L"Function_41 - Gosub");
	//pState->ExecutionPointer = -1;
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_42(CScriptState* pState)
{
	DebugTrace(pState, L"Function_42");

	// byte
	//text += $"If inventory[{GetInt(data, offset, 1):X2}] = {GetInt(data, offset + 1, 1):X2} jump to {GetInt(data, offset + 2, 2):X4}";
	pState->ExecutionPointer += 4;
}

void CPDScript::Function_43(CScriptState* pState)
{
	DebugTrace(pState, L"Function_43");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_44(CScriptState* pState)
{
	DebugTrace(pState, L"Function_44 - Hide location object");

	int object = pState->Read8();
	if (_pLoc != NULL)
	{
		_pLoc->SetObjectVisibility(object, FALSE);
	}
}

void CPDScript::Function_45(CScriptState* pState)
{
	DebugTrace(pState, L"Function_45 - Show location object");

	int object = pState->Read8();
	if (_pLoc != NULL)
	{
		_pLoc->SetObjectVisibility(object, TRUE);
	}
}

void CPDScript::Function_46(CScriptState* pState)
{
	DebugTrace(pState, L"Function_46");

	// byte (indirect)
	//text += $"??? {GetInt(data, offset, 1):X2} via Script_3C (possibly game over)";

	pState->ExecutionPointer++;
}

void CPDScript::Function_47(CScriptState* pState)
{
	DebugTrace(pState, L"Function_47");

	// byte (indirect)
	//text += $"??? {GetInt(data, offset, 1):X2} via Script_3C (possibly game over)";
	pState->ExecutionPointer++;
}

void CPDScript::Function_48(CScriptState* pState)
{
	DebugTrace(pState, L"Function_48 - Jump on A[X] != Y");
	// word, byte, word

	int parameter = pState->Read16();
	int value = pState->Read8();
	int address = pState->Read16();

	if (CGameController::GetParameter(parameter) != value)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_49(CScriptState* pState)
{
	DebugTrace(pState, L"Function_49 - Add Score");
	// byte

	int score = pState->Read8s();
	CGameController::AddScore(score);
}

void CPDScript::Function_4A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4A - Set Hint Category State");

	int ix = pState->Read8();
	int state = pState->Read8();
	if (CGameController::GetHintCategoryState(ix) != 2) CGameController::SetHintCategoryState(ix, state);
}

void CPDScript::Function_4B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4B");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_4C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4C - Set hint state, conditional score increment");

	// word
	//text += $"??? {GetInt(data, offset, 2):X4}";
	//offset += 2;

	/*
	DebugTrace(pState, L"Function_CC - Conditional Score Increment");
	int val = pState->GetInt(pState->ExecutionPointer, 2);
	pState->ExecutionPointer += 2;

	if (val >= 1000)
	{
		val -= 1000;
		//Set dword_2198D8 bit val * 2, score not affected
		CGameController::SetHintState(val, 1, 0);
	}
	else
	{
		// If dword_2198D8 bit val * 2 is set, add 1 to score
		// Set dword_2198D8 bit val * 2
		int currentState = CGameController::GetHintState(val);
		CGameController::SetHintState(val, currentState | 1, 0);
		if (currentState == 0)
		{
			CGameController::AddScore(1);
		}
	}
	*/

	pState->ExecutionPointer += 2;
}

void CPDScript::Function_4D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4D - Jump if animation between frames");

	pState->Read8();
	int animIndex = pState->Read8();
	int f1 = pState->Read16();
	int f2 = pState->Read16();
	int address = pState->Read16();
	int frame = _pLoc->GetIndexedAnimationFrame(animIndex);
	if ((f1 <= f2 && frame >= f1 && frame <= f2) || (f1 > f2 && (frame >= f1 || frame < f2)))
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_4E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4E");
	// byte, word, word

	BOOL TexTalk = (pState->Read8() == 0);
	int frame = pState->Read16();
	int stringOffset = pState->Read16();

	pAddCaptions->push_back(new CCaption(frame, (char*)(pState->Script + stringOffset), TexTalk));
}

void CPDScript::Function_4F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4F - Jump if player inside rectangle");

	float f1 = pState->Read12_4();
	float f2 = pState->Read12_4();
	float f3 = pState->Read12_4();
	float f4 = pState->Read12_4();
	int address = pState->Read16();

	float x1 = min(f1, f3);
	float x2 = max(f1, f3);
	float z1 = min(f2, f4);
	float z2 = max(f2, f4);

	if (_pLoc != NULL)
	{
		Point p = _pLoc->GetPlayerPosition();
		if (p.X >= x1 && p.X <= x2 && p.Z >= z1 && p.Z <= z2)
		{
			pState->ExecutionPointer = address;
		}
	}
}

void CPDScript::Function_50(CScriptState* pState)
{
	DebugTrace(pState, L"Function_50");

	// byte, word
	//text += $"If Function({GetInt(data, offset, 1):X2}) <> 0 jump to {GetInt(data, offset + 1, 2):X4}";
	pState->ExecutionPointer += 3;
}

void CPDScript::Function_51(CScriptState* pState)
{
	// This will wait for media completion
	DebugTrace(pState, L"Function_51 - Play Audio");
	int audio = pState->Read8();
	PlayAudio(audio);
	pState->WaitingForMediaToFinish = TRUE;
}

void CPDScript::Function_52(CScriptState* pState)
{
	DebugTrace(pState, L"Function_52");

	//text += $"???";
}

void CPDScript::Function_53(CScriptState* pState)
{
	DebugTrace(pState, L"Function_53");

	//text += $"???";
}

void CPDScript::Function_54(CScriptState* pState)
{
	DebugTrace(pState, L"Function_54");

	// byte
	//text += $"??? {GetInt(data, offset++, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_55(CScriptState* pState)
{
	DebugTrace(pState, L"Function_55 - Load MIDI");

	int file = pState->Read16();
	int entry = pState->Read16();
	std::wstring fileName = CGameController::GetFileName(file);
	BinaryData bd = LoadEntry(fileName.c_str(), entry);
	pMIDI->Init(bd);
}

void CPDScript::Function_56(CScriptState* pState)
{
	DebugTrace(pState, L"Function_56");

	//text += $"Start MIDI?";
}

void CPDScript::Function_57(CScriptState* pState)
{
	DebugTrace(pState, L"Function_57");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_58(CScriptState* pState)
{
	DebugTrace(pState, L"Function_58");
	//pState->ExecutionPointer = -1;
}

void CPDScript::Function_59(CScriptState* pState)
{
	DebugTrace(pState, L"Function_59 - Stop MIDI");

	pMIDI->Stop();
}

void CPDScript::Function_5A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5A - Jump on player close to point");

	float f1 = pState->Read12_4();
	float f2 = pState->Read12_4();
	float f3 = pState->Read12_4();
	int address = pState->Read16();

	double distance = (_pLoc != NULL) ? _pLoc->GetPlayerDistanceFromPoint(f1, f2) : 0.0;
	f3 *= f3;

	if (distance <= f3)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_5B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5B");

	//text += $"word_2A867A = -1";
}

void CPDScript::Function_5C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5C");
}

void CPDScript::Function_5D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5D");
}

void CPDScript::Function_5E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5E");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_5F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5F");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_60(CScriptState* pState)
{
	DebugTrace(pState, L"Function_60");

	// byte
	//text += $"??? {GetInt(data, offset++, 1):X2} Ask About related?";
	pState->ExecutionPointer++;
}

void CPDScript::Function_61(CScriptState* pState)
{
	DebugTrace(pState, L"Function_61");

	// byte
	//text += $"??? {GetInt(data, offset++, 1):X2} Ask About related?";
	pState->ExecutionPointer++;
}

void CPDScript::Function_62(CScriptState* pState)
{
	DebugTrace(pState, L"Function_62");

	// byte
	//text += $"??? {GetInt(data, offset++, 1):X2} Ask About related?";
	pState->ExecutionPointer++;
}

void CPDScript::Function_63(CScriptState* pState)
{
	DebugTrace(pState, L"Function_63");

	// word
	//text += $"If MIDI enabled? jump to {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer = pState->Read16();
}

void CPDScript::Function_64(CScriptState* pState)
{
	DebugTrace(pState, L"Function_64 - SetPlayer Position Min Y");

	float minY = pState->Read12_4();
	//CLocation::SetMinY(minY);
}

void CPDScript::Function_65(CScriptState* pState)
{
	DebugTrace(pState, L"Function_65 - SetPlayer Position Max Y");

	float maxY = pState->Read12_4();
	//CLocation::SetMaxY(maxY);
}

void CPDScript::Function_66(CScriptState* pState)
{
	DebugTrace(pState, L"Function_66 - Set parameter A");

	// word, byte
	int parameter = pState->Read16();
	int value = pState->Read8();
	CGameController::SetParameter(parameter, value);
}

void CPDScript::Function_67(CScriptState* pState)
{
	DebugTrace(pState, L"Function_67");
}

void CPDScript::Function_68(CScriptState* pState)
{
	DebugTrace(pState, L"Function_68 - Clear Cash");

	CGameController::SetWord(PD_SAVE_CASH, 0);
}

void CPDScript::Function_69(CScriptState* pState)
{
	DebugTrace(pState, L"Function_69 - Adjust Sound Volume by Player Distance to Point");

	// byte, word, word, word
	int sound = pState->Read8();
	float x = pState->Read12_4();
	float z = pState->Read12_4();
	double v = pState->Read16();
	v *= v;

	Point player = _pLoc->GetPlayerPosition();
	double dx = player.X - x;
	double dz = player.Z - z;
	double distance = sqrt(dx * dx + dz * dz);

	CAmbientAudio::SetVolume(sound - 1, max(0.0f, min(1.0f, 1.2f - (float)(distance / v))));

	// TODO: Work out what the pan should be
	float pan = 0.0f;
	CAmbientAudio::SetPan(sound - 1, pan);
}

void CPDScript::Function_6A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6A");

	// byte, word, word
	pState->ExecutionPointer += 5;
}

void CPDScript::Function_6B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6B - Set AskAbouts to Buyables");

	pState->AskingAboutBuyables = TRUE;
}

void CPDScript::Function_6C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6C");

	//text += $"???";
}

void CPDScript::Function_6D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6D - Set Buyable Item State");
	// byte, byte

	int item = pState->Read8();
	int state = pState->Read8();
	CGameController::SetBuyableItemState(item, state);
}

void CPDScript::Function_6E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6E");
}

void CPDScript::Function_6F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6F");

	// word, word, word, byte, word
	pState->ExecutionPointer += 9;
}

void CPDScript::Function_70(CScriptState* pState)
{
	DebugTrace(pState, L"Function_70");

	// word
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_71(CScriptState* pState)
{
	DebugTrace(pState, L"Function_71");

	// byte, word, word
	pState->ExecutionPointer += 5;
}

void CPDScript::Function_72(CScriptState* pState)
{
	DebugTrace(pState, L"Function_72");

	// word
	//text += $"Something about MIDI jump to {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_73(CScriptState* pState)
{
	DebugTrace(pState, L"Function_73");
}

void CPDScript::Function_74(CScriptState* pState)
{
	DebugTrace(pState, L"Function_74");
}

void CPDScript::Function_75(CScriptState* pState)
{
	DebugTrace(pState, L"Function_75 - Jump if Parameter[X] > Y");
	// word, byte, word

	int parameter = pState->Read16();
	int value = pState->Read8();
	int address = pState->Read16();
	if (CGameController::GetParameter(parameter) > value)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_76(CScriptState* pState)
{
	DebugTrace(pState, L"Function_76 - Jump if Parameter[X] < Y");
	// word, byte, word

	int parameter = pState->Read16();
	int value = pState->Read8();
	int address = pState->Read16();
	if (CGameController::GetParameter(parameter) < value)
	{
		pState->ExecutionPointer = address;
	}
}

void CPDScript::Function_77(CScriptState* pState)
{
	DebugTrace(pState, L"Function_77");

	// byte, word, word, word, word, word, word, word
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 2):X4}, {GetInt(data, offset + 3, 2):X4}, {GetInt(data, offset + 5, 2):X4}, {GetInt(data, offset + 7, 2):X4}, {GetInt(data, offset + 9, 2):X4}, {GetInt(data, offset + 11, 2):X4} jump to {GetInt(data, offset + 13, 2):X4}";
	pState->ExecutionPointer += 15;
}

void CPDScript::Function_78(CScriptState* pState)
{
	DebugTrace(pState, L"Function_78");

	// word, word, word
	//text += $"??? {GetInt(data, offset, 2):X4}, {GetInt(data, offset + 2, 2):X4}, {GetInt(data, offset + 4, 2):X4}";
	pState->ExecutionPointer += 6;
}

void CPDScript::Function_79(CScriptState* pState)
{
	DebugTrace(pState, L"Function_79");

	// word, word, word
	//text += $"Set resume point and display options\r\n\t\tA - {stringList[GetInt(data, offset, 2)]}\r\n\t\tB - {stringList[GetInt(data, offset + 2, 2)]}\r\n\t\tC - {stringList[GetInt(data, offset + 4, 2)]}";
	pState->ExecutionPointer += 6;
}

void CPDScript::Function_7A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_7A - Ask to climb ladder");

	pOverlay = pClimbLadderOverlay;
	pState->Parameter = pState->Read16();
	pState->WaitingForInput = TRUE;
}

void CPDScript::Function_7B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_7B");
}

void CPDScript::Function_7C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_7C");

	//text += $"???";
}

void CPDScript::Function_7D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_7D");
}

void CPDScript::Function_7E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_7E");

	//text += $"???";
}

void CPDScript::Function_7F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_7F - Allow or deny skipping media");
	CGameController::CanCancelVideo(pState->Read8() == 0);
}

void CPDScript::Function_80(CScriptState* pState)
{
	DebugTrace(pState, L"Function_80 - Set fullscreen video mode?");
	videoMode = VideoMode::FullScreen;
}

void CPDScript::Function_81(CScriptState* pState)
{
	DebugTrace(pState, L"Function_81 - Set embedded video mode?");
	videoMode = VideoMode::Embedded;
}

void CPDScript::Function_82(CScriptState* pState)
{
	DebugTrace(pState, L"Function_82");

	// word, word
	//text += $"??? A[{GetInt(data, offset, 2):X4}] = A[{GetInt(data, offset + 2, 2):X4}]";
	pState->ExecutionPointer += 4;
}

void CPDScript::Function_83(CScriptState* pState)
{
	DebugTrace(pState, L"Function_83");

	// byte, byte
	//text += $"U[{GetInt(data, offset, 1):X2}] += {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_84(CScriptState* pState)
{
	DebugTrace(pState, L"Function_84");

	// byte, byte, word
	//text += $"If U[{GetInt(data, offset, 1):X2}] <> {GetInt(data, offset + 1, 1):X2} jump to {GetInt(data, offset + 2, 2):X4}";
	pState->ExecutionPointer += 4;
}

void CPDScript::Function_85(CScriptState* pState)
{
	DebugTrace(pState, L"Function_85");
}

void CPDScript::Function_86(CScriptState* pState)
{
	DebugTrace(pState, L"Function_86");

	//text += $"??? Save Player Data ???";
}

void CPDScript::Function_87(CScriptState* pState)
{
	DebugTrace(pState, L"Function_87");

	// word
	//text += $"??? {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_88(CScriptState* pState)
{
	DebugTrace(pState, L"Function_88");

	// byte, word, word, word, word, word
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 2):X4}, {GetInt(data, offset + 3, 2):X4}, {GetInt(data, offset + 5, 2):X4}, {GetInt(data, offset + 7, 2):X4}, {GetInt(data, offset + 9, 2):X4}";

	int index = pState->Read8();
	float f1 = pState->Read12_4();
	float f2 = pState->Read12_4();
	float f3 = pState->Read12_4();
	float f4 = pState->Read12_4();
	int u = pState->Read8();
	int v = pState->Read8();

	// New table, 5 * 10 bytes
	// min(p1, p3)
	// min(p2, p4)
	// max(p1, p3)
	// max(p2, p4)
	// p5
	// p6 | 0x80
}

void CPDScript::Function_89(CScriptState* pState)
{
	DebugTrace(pState, L"Function_89");

	// byte, word
	//text += $"If word_2A8512 = {GetInt(data, offset, 1):X2} jump to {GetInt(data, offset + 1, 2):X4}";
	pState->ExecutionPointer += 3;
}

void CPDScript::Function_8A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8A");

	//text += $"??? byte_279CC6 = 0";
}

void CPDScript::Function_8B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8B");

	// byte
	//text += $"byte_2A88B1 = {GetInt(data, offset++, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_8C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8C - Add Score if Game Player Level");
	// byte

	int score = pState->Read8();
	if (CGameController::GetParameter(PD_SAVE_PARAMETERS_GAME_LEVEL) == 1)
	{
		CGameController::AddScore(score);
	}
}

void CPDScript::Function_8D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8D - Set Parameter Bits");
	// word, byte

	int parameter = pState->Read16();
	int bits = pState->Read8();
	CGameController::SetParameter(parameter, CGameController::GetParameter(parameter) | bits);
}

void CPDScript::Function_8E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8E");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_8F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8F");
}

void CPDScript::Function_90(CScriptState* pState)
{
	DebugTrace(pState, L"Function_90");

	//text += $"???";
}

void CPDScript::Function_91(CScriptState* pState)
{
	DebugTrace(pState, L"Function_91");

	// byte
	//text += $"??? {GetInt(data, offset++, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_92(CScriptState* pState)
{
	DebugTrace(pState, L"Function_92");

	// AskAbout base index?
	// byte
	//text += $"??? byte_2A89BB = 0 or 100+{GetInt(data, offset++, 1)}";
	pState->ExecutionPointer++;
}

ActionType CPDScript::GetCurrentActions(CScriptState* pState, int currentObjectIndex)
{
	if (currentObjectIndex >= 0)
	{
		// Add 2 to the object index, as scripts 0 and 1 are reserved
		currentObjectIndex += 2;

		// Find script with id same as current object index
		pState->ExecutionPointer = pState->GetScript(currentObjectIndex);
		if (pState->ExecutionPointer >= 0)
		{
			pState->AllowedAction = ActionType::None;
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

void CPDScript::Show(CScriptState* pState, int index)
{
	if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		FileMap fm = _mapEntry->ImageMap.at(index);
		pState->WaitingForMediaToFinish = TRUE;	// Set to break out of the script loop
		CModuleController::Push(new CPictureModule(fm.File, fm.Entry, this, pState));
	}
}

void CPDScript::PlayAudio(int index)
{
	SwapCaptions();

	FileMap fm = _mapEntry->AudioMap.at(index);
	std::wstring fn = CGameController::GetFileName(fm.File);
	if (fn != L"")
	{
		CAnimationController::Load(fn.c_str(), fm.Entry);
	}
}

void CPDScript::PlaySound(int index)
{
	CAmbientAudio::Play(_mapEntry, index, FALSE);
}

void CPDScript::PlayVideo(int index, int rate)
{
	SwapCaptions();

	FileMap fm = _mapEntry->VideoMap.at(index);
	std::wstring fn = CGameController::GetFileName(fm.File);
	if (fn != L"")
	{
		CAnimationController::Load(fn.c_str(), fm.Entry);
	}
}

void CPDScript::SelectDialogueOption(CScriptState* pState, int option)
{
	pState->SelectedOption = option;

	if (option >= 1 && option <= 3)
	{
		if (pState->WaitingForInput)
		{
			//pState->OfferMode = FALSE;
			pState->Mode = InteractionMode::None;
			Resume(pState, TRUE);
		}
	}
	else if (option == 4)
	{
		pState->AskAbout = !pState->AskAbout;
		pState->Buy = FALSE;
		pState->Offer = FALSE;
		pState->Mode = InteractionMode::AskAbout;
	}
	else if (option == 6)
	{
		pState->AskAbout = FALSE;
		pState->Buy = !pState->Buy;
		pState->Offer = FALSE;
		pState->Mode = InteractionMode::Buy;
	}
	else if (option == 7)
	{
		pState->AskAbout = FALSE;
		pState->Buy = FALSE;
		pState->Offer = !pState->Offer;
		pState->Mode = InteractionMode::Offer;
	}
}
