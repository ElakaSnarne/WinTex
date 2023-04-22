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

void CPDScript::PermformAction(CScriptState* pState, int id, int action, int item)
{
	//pState->ExecutionPointer = pState->GetScript(id);
	pState->CurrentAction = action;
	if (item > 0)
	{
		pState->CurrentAction = ActionType::Use;
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
	}

	//WCHAR buffer[40];

	while (CModuleController::CurrentModule == pThisModule && pState->ExecutionPointer > -1 && pState->ExecutionPointer < pState->Length && pState->Script != NULL && !pState->WaitingForMediaToFinish && !pState->WaitingForInput)
	{
		//_itow(pState->ExecutionPointer, buffer, 16);
		//OutputDebugString(buffer);
		//OutputDebugString(L" - ");

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
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Look) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_04(CScriptState* pState)
{
	DebugTrace(pState, L"Function_04");
	//pState->ExecutionPointer = -1;
	pState->ExecutionPointer += 2;	// Jump to address if skip requested (enter pressed)

	//text += $"If byte_2A851E <> 0 jump to {GetInt(data, offset, 2):X4}";
	//offset += 2;
}

void CPDScript::Function_05(CScriptState* pState)
{
	DebugTrace(pState, L"Function_05 - If Action = Get jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Get) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_06(CScriptState* pState)
{
	DebugTrace(pState, L"Function_06 - If Action = Move jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Move) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_07(CScriptState* pState)
{
	DebugTrace(pState, L"Function_07 - If Action = Use jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Use) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_08(CScriptState* pState)
{
	DebugTrace(pState, L"Function_08 - If Action = Talk jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Talk) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_09(CScriptState* pState)
{
	DebugTrace(pState, L"Function_09 - If Action = Open jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::Open) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_0A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0A - Print");
	// TODO: Fix this, it is not correct
	pState->FrameTrigger = 1;
	AddCaption(pState, TRUE);
	pState->ExecutionPointer += 2;
}

void CPDScript::AddCaption(CScriptState* pState, BOOL TexTalk)
{
	// Print at frame trigger (ce = Tex talks, cf = others talk)
	int stringOffset = GetInt(pState->Script, pState->ExecutionPointer, 2);
	pAddCaptions->push_back(new CCaption(pState->FrameTrigger, (char*)(pState->Script + stringOffset), TexTalk));
}

void CPDScript::Function_0B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0B - Start Animation");

	int anim = pState->Script[pState->ExecutionPointer++];
	if (_pLoc != NULL)
	{
		//_pLoc->StartIndexedAnimation(anim);
		_pLoc->StartMappedAnimation(anim);
		//_pLoc->StartIdAnimation(anim);
	}
}

void CPDScript::Function_0C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0C - Set Parameter");

	// Set parameter
	int index = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int value = pState->Script[pState->ExecutionPointer + 2];
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
	pState->ExecutionPointer += 3;
}

void CPDScript::Function_0D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0D - Jump on A[X]=Y");

	int ix = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int cmp = GetInt(pState->Script, pState->ExecutionPointer + 2, 1);

	if (CGameController::GetParameter(ix) == cmp)
	{
		pState->ExecutionPointer = GetInt(pState->Script, pState->ExecutionPointer + 3, 2);
	}
	else
	{
		pState->ExecutionPointer += 5;
	}
}

void CPDScript::Function_0E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0E - Jump to offset");
	pState->ExecutionPointer = GetInt(pState->Script, pState->ExecutionPointer, 2);
}

void CPDScript::Function_0F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_0F");

	//text += $"??? {GetInt(data, offset, 2):X4}, {GetInt(data, offset + 2, 2):X4}, {GetInt(data, offset + 4, 2):X4}, {GetInt(data, offset + 6, 2):X4}";

	pState->ExecutionPointer += 8;
}

void CPDScript::Function_10(CScriptState* pState)
{
	DebugTrace(pState, L"Function_10 - Set item state");

	int itemIndex = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int itemState = GetInt(pState->Script, pState->ExecutionPointer + 2, 1);
	CGameController::SetItemState(itemIndex, itemState);

	pState->ExecutionPointer += 3;
}

void CPDScript::Function_11(CScriptState* pState)
{
	DebugTrace(pState, L"Function_11 - Jump on Item State = X");

	// word, byte, word
	//text += $"If X[{GetInt(data, offset, 2):X4}] = {GetInt(data, offset + 2, 1):X2} jump to {GetInt(data, offset + 3, 2):X4}";

	int item = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int state = pState->Script[pState->ExecutionPointer + 2];

	pState->ExecutionPointer = (CGameController::GetItemState(item) == state) ? GetInt(pState->Script, pState->ExecutionPointer + 3, 2) : pState->ExecutionPointer + 5;
}

void CPDScript::Function_12(CScriptState* pState)
{
	DebugTrace(pState, L"Function_12");
	pState->ExecutionPointer = -1;
}

void CPDScript::Function_13(CScriptState* pState)
{
	DebugTrace(pState, L"Function_13 - Load Location");
	// byte, byte, byte
	//text += $"Load Location {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}, {GetInt(data, offset + 2, 1):X2} (files from from RMAP.AP)";
	//offset += 3;

	CAmbientAudio::Clear();
	//CGameController::SetParameter(252, 0);

	int locationId = pState->Script[pState->ExecutionPointer];
	int startupPosition = pState->Script[pState->ExecutionPointer + 1];
	//CGameController::SetData(UAKM_SAVE_MAP_ENTRY, locationId);
	//CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 0);

	CModuleController::Push(new CPDLocationModule(locationId, startupPosition));
	pState->ExecutionPointer = -1;
}

void CPDScript::Function_14(CScriptState* pState)
{
	DebugTrace(pState, L"Function_14 - Load file");

	// byte, byte
	//text += $"Load File {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2} (from DMAP?)";

	CAmbientAudio::Clear();
	pMIDI->Stop();
	//CGameController::SetParameter(252, 0);

	// Conversations & media
	// Load files from DMAP.LZ
	int ix = pState->Script[pState->ExecutionPointer];
	int unknown = pState->Script[pState->ExecutionPointer + 1];

	//CGameController::SetData(UAKM_SAVE_DMAP_ENTRY, ix);
	//CGameController::SetData(UAKM_SAVE_DMAP_FLAG, 1);

	CModuleController::Push(new CVideoModule(VideoType::Scripted, ix, unknown));

	pState->ExecutionPointer = -1;
	//pState->WaitingForInput = TRUE;
}

void CPDScript::Function_15(CScriptState* pState)
{
	DebugTrace(pState, L"Function_15 - Jump if animation at frame");
	pState->ExecutionPointer = (_pLoc != NULL && _pLoc->GetAnimationFrame(pState->Script[pState->ExecutionPointer]) == GetInt(pState->Script, pState->ExecutionPointer + 1, 2)) ? GetInt(pState->Script, pState->ExecutionPointer + 3, 2) : pState->ExecutionPointer + 5;
}

void CPDScript::Function_16(CScriptState* pState)
{
	DebugTrace(pState, L"Function_16 - Jump if animation ended");
	pState->ExecutionPointer = (_pLoc != NULL && _pLoc->GetAnimationFrame(pState->Script[pState->ExecutionPointer]) == -1) ? GetInt(pState->Script, pState->ExecutionPointer + 1, 2) : pState->ExecutionPointer + 3;
}

void CPDScript::Function_17(CScriptState* pState)
{
	// This will NOT wait for media completion
	DebugTrace(pState, L"Function_17 - Play Sound");
	PlaySound(pState, pState->Script[pState->ExecutionPointer++] - 1);
	pState->WaitingForMediaToFinish = FALSE;
}

void CPDScript::Function_18(CScriptState* pState)
{
	DebugTrace(pState, L"Function_18 - Jump on player Y > value");

	// word, word
	int ytest = GetInt(pState->Script, pState->ExecutionPointer, 2);
	if (ytest & 0x8000)
	{
		ytest |= ~0xffff;
	}

	float ylimit = ((float)ytest) / 16.0f;
	float playerY = -_pLoc->GetPlayerPosition().Y;
	pState->ExecutionPointer = (playerY > ylimit) ? GetInt(pState->Script, pState->ExecutionPointer + 2, 2) : pState->ExecutionPointer + 4;

	//text += $"If word_2A9102 <= {GetInt(data, offset, 2):X4} jump to {GetInt(data, offset + 2, 2):X4}";
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
	DebugTrace(pState, L"Function_1A");
	//pState->ExecutionPointer += 10;

	int p1 = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int p3 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int p4 = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	int address = GetInt(pState->Script, pState->ExecutionPointer + 8, 2);

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

	pState->ExecutionPointer += 10;
	if (_pLoc != NULL)
	{
		Point p = _pLoc->GetPlayerPosition();
		if (p.X >= x1 && p.X <= x2 && p.Z >= z1 && p.Z <= z2)
			//if (p.X < x1 || p.X > x2 || p.Z < z1 || p.Z > z2)
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
	DebugTrace(pState, L"Function_1D");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_1E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1E");

	// byte, word
	//text += $"Set Timer {GetInt(data, offset, 1):X2} to {GetInt(data, offset + 1, 2) * 16.6666666667:0} ms";

	int timer = pState->Script[pState->ExecutionPointer];
	int duration = GetInt(pState->Script, pState->ExecutionPointer + 1, 2);
	CGameController::SetTimer(timer, duration);
	pState->ExecutionPointer += 3;
}

void CPDScript::Function_1F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_1F");

	// byte, word, word

	//text += $"If W[{GetInt(data, offset, 1):X2}] = {GetInt(data, offset + 1, 1):X2} jump to {GetInt(data, offset + 3, 2):X4}";
	int timer = pState->Script[pState->ExecutionPointer];
	int compare = pState->Script[pState->ExecutionPointer + 1];
	if (CGameController::GetTimerState(timer) == compare)
	{
		int address = GetInt(pState->Script, pState->ExecutionPointer + 3, 2);
		pState->ExecutionPointer = address;
	}
	else
	{
		pState->ExecutionPointer += 5;
	}
}

void CPDScript::Function_20(CScriptState* pState)
{
	DebugTrace(pState, L"Function_A0 - Change Travel Location state");

	CGameController::SetData(PD_SAVE_TRAVEL + pState->Script[pState->ExecutionPointer], pState->Script[pState->ExecutionPointer + 1]);

	pState->ExecutionPointer += 2;
}

void CPDScript::Function_21(CScriptState* pState)
{
	DebugTrace(pState, L"Function_21 - If Action = On/Off jump to offset");
	pState->ExecutionPointer = (pState->CurrentAction == ActionType::OnOff) ? GetInt(pState->Script, pState->ExecutionPointer, 2) : pState->ExecutionPointer + 2;
}

void CPDScript::Function_22(CScriptState* pState)
{
	DebugTrace(pState, L"Function_22");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
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
	DebugTrace(pState, L"Function_25");

	//text += $"???";
}

void CPDScript::Function_26(CScriptState* pState)
{
	DebugTrace(pState, L"Function_26 - Add Cash");

	// word
	int cashToAdd = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int currentCash = CGameController::GetWord(PD_SAVE_CASH);
	CGameController::SetWord(PD_SAVE_CASH, currentCash + cashToAdd);

	pState->ExecutionPointer += 2;
}

void CPDScript::Function_27(CScriptState* pState)
{
	DebugTrace(pState, L"Function_27 - Play Video");
	int x = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int y = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int entry = pState->Script[pState->ExecutionPointer + 4];
	int rate = GetInt(pState->Script, pState->ExecutionPointer + 5, 2);
	PlayVideo(pState, entry, rate);
	pState->ExecutionPointer += 7;
	pState->WaitingForMediaToFinish = TRUE;
}

void CPDScript::Function_28(CScriptState* pState)
{
	DebugTrace(pState, L"Function_28");
	// byte
	//text += $"Set OnReturnContinueAtScriptID = {GetInt(data, offset++, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_29(CScriptState* pState)
{
	DebugTrace(pState, L"Function_29");

	// word
	int cashToRemove = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int currentCash = CGameController::GetWord(PD_SAVE_CASH);
	CGameController::SetWord(PD_SAVE_CASH, currentCash - cashToRemove);

	pState->ExecutionPointer += 2;
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
	DebugTrace(pState, L"Function_2D");
	//pState->ExecutionPointer = -1;

	//text += $"???";
}

void CPDScript::Function_2E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2E - If AskAboutState[x]=y go to z");

	// byte, byte, word
	int ix = pState->Script[pState->ExecutionPointer];
	int val = pState->Script[pState->ExecutionPointer + 1];
	int address = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);

	pState->ExecutionPointer = (CGameController::GetAskAboutState(ix) == val) ? address : pState->ExecutionPointer + 4;
}

void CPDScript::Function_2F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_2F - Special function");

	int function = pState->Script[pState->ExecutionPointer];
	int p1 = GetInt(pState->Script, pState->ExecutionPointer + 1, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 3, 2);

	switch (function)
	{
	case 5:
	{
		// Vidphone
		break;
	}
	default:
	{
		break;
	}
	}

	pState->ExecutionPointer += 5;
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
	DebugTrace(pState, L"Function_34");

	// word, word, word
	//text += $"Display options\r\n\t\tA - {stringList[GetInt(data, offset, 2)]}\r\n\t\tB - {stringList[GetInt(data, offset + 2, 2)]}\r\n\t\tC - {stringList[GetInt(data, offset + 4, 2)]}";
	//	pState->LastDialoguePoint = pState->ExecutionPointer - 1;
	//pState->ExecutionPointer += 6;
	//pState->WaitingForInput = TRUE;

	pState->LastDialoguePoint = pState->ExecutionPointer - 1;

	//ClearCaption();

	char* pT[3];
	//float w = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		int v = i + 1;
		int stringOffset = GetInt(pState->Script, pState->ExecutionPointer, 2);
		pState->ExecutionPointer += 2;
		pT[i] = (char*)(pState->Script + stringOffset);

		if (pT[i] != NULL && pT[i][0] == '^')
		{
			v = atoi(pT[i] + 1);
			// Get correct string (next in list)
			while (pT[i][0] != 0) pT[i]++;
			pT[i]++;
		}

		DialogueOptions[i].SetValue(v);

		//w = max(Font.PixelWidth(pT[i]), w);
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

	pState->AskAbout = FALSE;
	pState->Offer = FALSE;
	pState->Buy = FALSE;

	// Should now wait for input
	pState->WaitingForInput = TRUE;
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
	int actions = pState->Script[pState->ExecutionPointer++];
	if (actions & 1) pState->AllowedAction |= ActionType::Look;
	if (actions & 2) pState->AllowedAction |= ActionType::Get;
	if (actions & 4) pState->AllowedAction |= ActionType::Move;
	if (actions & 8) pState->AllowedAction |= ActionType::Open;
	if (actions & 16) pState->AllowedAction |= ActionType::Talk;
	if (actions & 32) pState->AllowedAction |= ActionType::OnOff;
}

void CPDScript::Function_37(CScriptState* pState)
{
	DebugTrace(pState, L"Function_37");

	// word, word
	int cashTest = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int address = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	pState->ExecutionPointer = address;
	//text += $"If Cash >= {GetInt(data, offset, 2)} jump to {GetInt(data, offset + 2, 2):X4} (otherwise ask to convert points)";
	//pState->ExecutionPointer += 4;
}

void CPDScript::Function_38(CScriptState* pState)
{
	DebugTrace(pState, L"Function_38 - Jump on Selected Option = X");
	pState->ExecutionPointer = (pState->SelectedOption == pState->Script[pState->ExecutionPointer]) ? GetInt(pState->Script, pState->ExecutionPointer + 1, 2) : pState->ExecutionPointer + 3;
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
	else if (pState->SelectedValue == 2)
	{
		// It
		CGameController::SetParameter(0, 2);
	}
}

void CPDScript::Function_3B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3B");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
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
	DebugTrace(pState, L"Function_3E");
	//pState->ExecutionPointer = -1;

	// word, word, word
	// Ask about/offer
	int type = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int index = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);

	//string topic = (type == 0) ? PDData.GetAskAboutName(index) : PDData.GetItemName(index);

	if (pState->SelectedValue == index)
	{
		pState->ExecutionPointer = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	}
	else
	{
		pState->ExecutionPointer += 6;
	}

	//text += $"If ??? {type:X4} - {index:X4} - Ask About {PDData.GetAskAboutName(index)} or Offer {PDData.GetItemName(index)} ??? jump to {GetInt(data, offset + 4, 2):X4}";
}

void CPDScript::Function_3F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_3F - Jump if Item not acquired");

	// word, word
	//text += $"If X[{GetInt(data, offset, 2):X4}] = 0 jump to {GetInt(data, offset + 2, 2):X4}";

	int itemIndex = GetInt(pState->Script, pState->ExecutionPointer, 2);
	if (CGameController::GetItemState(itemIndex) == 0)
	{
		pState->ExecutionPointer = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	}
	else
	{
		pState->ExecutionPointer += 4;
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

	if (_pLoc != NULL)
	{
		_pLoc->SetObjectVisibility(pState->Script[pState->ExecutionPointer], FALSE);
	}
	pState->ExecutionPointer++;
}

void CPDScript::Function_45(CScriptState* pState)
{
	DebugTrace(pState, L"Function_45 - Show location object");

	if (_pLoc != NULL)
	{
		_pLoc->SetObjectVisibility(pState->Script[pState->ExecutionPointer], TRUE);
	}
	pState->ExecutionPointer++;
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
	//text += $"If A[{GetInt(data, offset, 2):X4}] <> {GetInt(data, offset + 2, 1):X2} jump to {GetInt(data, offset + 3, 2):X4}";
	pState->ExecutionPointer = (CGameController::GetParameter(GetInt(pState->Script, pState->ExecutionPointer, 2)) != pState->Script[pState->ExecutionPointer + 2]) ? GetInt(pState->Script, pState->ExecutionPointer + 3, 2) : pState->ExecutionPointer + 5;
}

void CPDScript::Function_49(CScriptState* pState)
{
	DebugTrace(pState, L"Function_49");

	// byte
	//text += $"Score += {GetInt(data, offset++, 1):X2}";
	int score = pState->Script[pState->ExecutionPointer];
	if (score & 0x80)
	{
		score |= ~0xff;
	}
	CGameController::AddScore(score);

	pState->ExecutionPointer++;
}

void CPDScript::Function_4A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4A - Set Hint Category State");
	int ix = pState->Script[pState->ExecutionPointer];
	if (CGameController::GetHintCategoryState(ix) != 2) CGameController::SetHintCategoryState(ix, pState->Script[pState->ExecutionPointer + 1]);
	pState->ExecutionPointer += 2;
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
	int val = GetInt(pState->Script, pState->ExecutionPointer, 2);
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
	DebugTrace(pState, L"Function_4D");

	int animIndex = pState->Script[pState->ExecutionPointer + 1];
	int f1 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int f2 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int frame = _pLoc->GetIndexedAnimationFrame(animIndex);
	if ((f1 <= f2 && frame >= f1 && frame <= f2) || (f1 > f2 && (frame >= f1 || frame < f2)))
	{
		pState->ExecutionPointer = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	}
	else
	{
		pState->ExecutionPointer += 8;
	}
}

void CPDScript::Function_4E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4E");

	// byte, word, word
	//string who = GetInt(data, offset, 1) == 0 ? "Tex" : "Other";
	//text += $"At frame {:X4}, print as {who} \"{stringList[GetInt(data, offset + 3, 2)]}\"";
	//AddCaption(state);
	//int frame = GetInt(data, offset + 1, 2)

	//char* pST = (char*)(pState->Script + pState->ExecutionPointer);
	//while (pState->Script[pState->ExecutionPointer++] != 0);
	//pAddCaptions->push_back(new CCaption(pState->FrameTrigger, pST, TexTalk));

	//AddCaption(pState, TRUE);

	BOOL TexTalk = (pState->Script[pState->ExecutionPointer] == 0);
	int frame = GetInt(pState->Script, pState->ExecutionPointer + 1, 2);
	int stringOffset = GetInt(pState->Script, pState->ExecutionPointer + 3, 2);

	pAddCaptions->push_back(new CCaption(frame, (char*)(pState->Script + stringOffset), TexTalk));

	pState->ExecutionPointer += 5;
}

void CPDScript::Function_4F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_4F - Jump on player inside rectangle");

	int p1 = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int p3 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int p4 = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);
	int address = GetInt(pState->Script, pState->ExecutionPointer + 8, 2);

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

	pState->ExecutionPointer += 10;
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
	PlayAudio(pState, pState->Script[pState->ExecutionPointer++]);
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

	// word, word
	//text += $"Load MIDI from file {GetInt(data, offset, 2):X4}, entry {GetInt(data, offset + 2, 2):X4}";

	int file = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int entry = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	std::wstring fileName = CGameController::GetFileName(file);
	BinaryData bd = LoadEntry(fileName.c_str(), entry);
	pMIDI->Init(bd);

	pState->ExecutionPointer += 4;
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

	//text += "??? Do something if byte_283ED1 = 0";
	pMIDI->Stop();
}

void CPDScript::Function_5A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_5A - Jump on player close to point");

	int p1 = GetInt(pState->Script, pState->ExecutionPointer, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 2, 2);
	int p3 = GetInt(pState->Script, pState->ExecutionPointer + 4, 2);
	int p4 = GetInt(pState->Script, pState->ExecutionPointer + 6, 2);

	if ((p1 & 0x8000) != 0) p1 = (int)(p1 | 0xffff0000);
	if ((p2 & 0x8000) != 0) p2 = (int)(p2 | 0xffff0000);

	float f1 = ((float)p1) / 16.0f;
	float f2 = ((float)p2) / 16.0f;
	float f3 = ((float)p3) / 16.0f;

	double distance = (_pLoc != NULL) ? _pLoc->GetPlayerDistanceFromPoint(f1, f2) : 0.0;
	f3 *= f3;

	if (distance <= f3)
	{
		pState->ExecutionPointer = p4;
	}
	else
	{
		pState->ExecutionPointer += 8;
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
	pState->ExecutionPointer = GetInt(pState->Script, pState->ExecutionPointer, 2);
	//pState->ExecutionPointer += 2;
}

void CPDScript::Function_64(CScriptState* pState)
{
	DebugTrace(pState, L"Function_64");

	// word
	//text += $"??? {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_65(CScriptState* pState)
{
	DebugTrace(pState, L"Function_65");

	// word
	//text += $"??? {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_66(CScriptState* pState)
{
	DebugTrace(pState, L"Function_66 - Set parameter A");

	// word, byte
	CGameController::SetParameter(GetInt(pState->Script, pState->ExecutionPointer, 2), pState->Script[pState->ExecutionPointer + 2]);
	//text += $"A[{GetInt(data, offset, 2):X4}] = {GetInt(data, offset + 2, 1):X2}";
	pState->ExecutionPointer += 3;
}

void CPDScript::Function_67(CScriptState* pState)
{
	DebugTrace(pState, L"Function_67");
}

void CPDScript::Function_68(CScriptState* pState)
{
	DebugTrace(pState, L"Function_68");

	//text += $"Clear cash";
}

void CPDScript::Function_69(CScriptState* pState)
{
	DebugTrace(pState, L"Function_69");

	// byte, word, word, word
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 2):X4}, {GetInt(data, offset + 3, 2):X4}, {GetInt(data, offset + 5, 2):X4}";
	int sound = pState->Script[pState->ExecutionPointer];
	int p1 = GetInt(pState->Script, pState->ExecutionPointer + 1, 2);
	int p2 = GetInt(pState->Script, pState->ExecutionPointer + 3, 2);
	int v = GetInt(pState->Script, pState->ExecutionPointer + 5, 2);
	if (p1 & 0x8000) p1 |= ~0xffff;
	if (p2 & 0x8000) p2 |= ~0xffff;

	// TODO: Adjust volume of sound based on distance from point

	pState->ExecutionPointer += 7;
}

void CPDScript::Function_6A(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6A");

	// byte, word, word
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 2):X4} jump to {GetInt(data, offset + 3, 2):X4}";
	pState->ExecutionPointer += 5;
}

void CPDScript::Function_6B(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6B");

	//text += $"???";
}

void CPDScript::Function_6C(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6C");

	//text += $"???";
}

void CPDScript::Function_6D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6D");

	// byte, byte
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 1):X2}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_6E(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6E");
}

void CPDScript::Function_6F(CScriptState* pState)
{
	DebugTrace(pState, L"Function_6F");

	// word, word, word, byte, word
	//text += $"??? {GetInt(data, offset, 2):X4}, {GetInt(data, offset + 2, 2):X4}, {GetInt(data, offset + 4, 2):X4}, {GetInt(data, offset + 6, 1):X2}, {GetInt(data, offset + 7, 2):X4}";
	pState->ExecutionPointer += 9;
}

void CPDScript::Function_70(CScriptState* pState)
{
	DebugTrace(pState, L"Function_70");

	// word
	//text += $"??? {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
}

void CPDScript::Function_71(CScriptState* pState)
{
	DebugTrace(pState, L"Function_71");

	// byte, word, word
	//text += $"??? {GetInt(data, offset, 1):X2}, {GetInt(data, offset + 1, 2):X4}, {GetInt(data, offset + 3, 2):X4}";
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
	DebugTrace(pState, L"Function_75");

	// word, byte, word
	//text += $"If A[{GetInt(data, offset, 2):X4}] > {GetInt(data, offset + 2, 1):X2} jump to {GetInt(data, offset + 3, 2):X4}";
	pState->ExecutionPointer += 5;
}

void CPDScript::Function_76(CScriptState* pState)
{
	DebugTrace(pState, L"Function_76");

	// word, byte, word
	//text += $"If A[{GetInt(data, offset, 2):X4}] < {GetInt(data, offset + 2, 1):X2} jump to {GetInt(data, offset + 3, 2):X4}";
	pState->ExecutionPointer += 5;
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
	DebugTrace(pState, L"Function_7A");

	// word
	//text += $"??? If word_28ED1E = 0 or other condition jump to {GetInt(data, offset, 2):X4}";
	pState->ExecutionPointer += 2;
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
	CGameController::CanCancelVideo(pState->Script[pState->ExecutionPointer++] == 0);
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
	pState->ExecutionPointer += 11;
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
	DebugTrace(pState, L"Function_8C");

	// byte
	//text += $"If byte_2AFC99 = 1 Score += {GetInt(data, offset++, 1):X2}";
	pState->ExecutionPointer++;
}

void CPDScript::Function_8D(CScriptState* pState)
{
	DebugTrace(pState, L"Function_8D");

	// word, byte
	//text += $"A[{GetInt(data, offset, 2):X4}] |= {GetInt(data, offset + 2, 1):X2}";
	pState->ExecutionPointer += 3;
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

	// byte
	//text += $"??? byte_2A89BB = 0 or 100+{GetInt(data, offset++, 1)}";
	pState->ExecutionPointer++;
}

/*
void CPDScript::Play(CScriptState* pState, int index, int bank, int rate)
{
	// TODO: Pass rate on to animation loader

	// Swap caption pointers
	std::list<CCaption*>* pOld = pDisplayCaptions;
	pDisplayCaptions = pAddCaptions;
	pAddCaptions = pOld;

	ClearCaptions(pOld);

	/*
	if (_dmapEntry.ScriptFileEntry != 0 || _dmapEntry.ScriptFileIndex != 0)
	{
		if (bank == 1)
		{
			FileMap fm = _dmapEntry.VideoMap.at(index);
			std::wstring fn = CGameController::GetFileName(fm.File);
			if (fn != L"")
			{
				//CModuleController::Push(new CVideoModule(VideoType::Scripted, fn, mm.FileEntry));
				CAnimationController::Load(fn.c_str(), fm.Entry);
			}
		}
		else
		{
			FileMap fm = _dmapEntry.AudioMap.at(index);
			std::wstring fn = CGameController::GetFileName(fm.File);
			if (fn != L"")
			{
				CAnimationController::Load(fn.c_str(), fm.Entry);
			}
		}
	}
	else if (_mapEntry.ScriptFileEntry != 0 || _mapEntry.ScriptFileIndex != 0)
	{
		FileMap fm;
		if (bank == 0)
		{
			fm = _mapEntry.SoundMap2.at(index);
		}
		else if (bank == 1)
		{
			fm = _mapEntry.VideoMap.at(index);
		}

		std::wstring fn = CGameController::GetFileName(fm.File);
		if (fn != L"")
		{
			CAnimationController::Load(fn.c_str(), fm.Entry);
		}
	}
	* /

	pState->WaitingForMediaToFinish = TRUE;
}
*/

int CPDScript::GetCurrentActions(CScriptState* pState, int currentObjectIndex)
{
	if (currentObjectIndex >= 0)
	{
		// Add 2 to the object index, as scripts 0 and 1 are reserved
		currentObjectIndex += 2;

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

void CPDScript::Show(CScriptState* pState, int index)
{
	if (_mapEntry->ScriptFileEntry != 0 || _mapEntry->ScriptFileIndex != 0)
	{
		FileMap fm = _mapEntry->ImageMap.at(index);
		pState->WaitingForMediaToFinish = TRUE;	// Set to break out of the script loop
		CModuleController::Push(new CPictureModule(fm.File, fm.Entry, this, pState));
	}
}

void CPDScript::PlayAudio(CScriptState* pState, int index)
{
	SwapCaptions();

	FileMap fm = _mapEntry->AudioMap.at(index);
	std::wstring fn = CGameController::GetFileName(fm.File);
	if (fn != L"")
	{
		CAnimationController::Load(fn.c_str(), fm.Entry);
	}
}

void CPDScript::PlaySound(CScriptState* pState, int index)
{
	CAmbientAudio::Play(_mapEntry, index);
}

void CPDScript::PlayVideo(CScriptState* pState, int index, int rate)
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
		pState->AskAbout = TRUE;
		pState->Mode = InteractionMode::AskAbout;
		//pState->TopItemOffset
	}
	else if (option == 6)
	{
		pState->Buy = TRUE;
		pState->Mode = InteractionMode::Buy;
	}
	else if (option == 7)
	{
		pState->Offer = TRUE;
		pState->Mode = InteractionMode::Offer;
	}
}
