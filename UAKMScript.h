#pragma once

#include "ScriptBase.h"
#include <unordered_map>

class CUAKMScript : public CScriptBase
{
public:
	CUAKMScript();
	~CUAKMScript();

	virtual void Execute(CScriptState* pState, int id);
	virtual void Resume(CScriptState* pState, BOOL breakWait = FALSE);
	virtual ActionType GetCurrentActions(CScriptState* pState, int currentObjectIndex);
	virtual void PermformAction(CScriptState* pState, int id, ActionType action, int item);

	virtual void SelectDialogueOption(CScriptState* pState, int option);

protected:
	void Function_80(CScriptState* pState);
	void Function_81(CScriptState* pState);
	void Function_82(CScriptState* pState);
	void Function_83(CScriptState* pState);
	void Function_84(CScriptState* pState);
	void Function_85(CScriptState* pState);
	void Function_86(CScriptState* pState);
	void Function_87(CScriptState* pState);
	void Function_88(CScriptState* pState);
	void Function_89(CScriptState* pState);
	void Function_8A(CScriptState* pState);
	void Function_8B(CScriptState* pState);
	void Function_8C(CScriptState* pState);
	void Function_8D(CScriptState* pState);
	void Function_8E(CScriptState* pState);
	void Function_8F(CScriptState* pState);
	void Function_90(CScriptState* pState);
	void Function_91(CScriptState* pState);
	void Function_92(CScriptState* pState);
	void Function_93(CScriptState* pState);
	void Function_94(CScriptState* pState);
	void Function_95(CScriptState* pState);
	void Function_96(CScriptState* pState);
	void Function_97(CScriptState* pState);
	void Function_98(CScriptState* pState);
	void Function_99(CScriptState* pState);
	void Function_9A(CScriptState* pState);
	void Function_9B(CScriptState* pState);
	void Function_9C(CScriptState* pState);
	void Function_9D(CScriptState* pState);
	void Function_9E(CScriptState* pState);
	void Function_9F(CScriptState* pState);
	void Function_A0(CScriptState* pState);
	void Function_A1(CScriptState* pState);
	void Function_A2(CScriptState* pState);
	void Function_A3(CScriptState* pState);
	void Function_A4(CScriptState* pState);
	void Function_A5(CScriptState* pState);
	void Function_A6(CScriptState* pState);
	void Function_A7(CScriptState* pState);
	void Function_A8(CScriptState* pState);
	void Function_A9(CScriptState* pState);
	void Function_AA(CScriptState* pState);
	void Function_AB(CScriptState* pState);
	void Function_AC(CScriptState* pState);
	void Function_AD(CScriptState* pState);
	void Function_AE(CScriptState* pState);
	void Function_AF(CScriptState* pState);
	void Function_B0(CScriptState* pState);
	void Function_B1(CScriptState* pState);
	void Function_B2(CScriptState* pState);
	void Function_B3(CScriptState* pState);
	void Function_B4(CScriptState* pState);
	void Function_B5(CScriptState* pState);
	void Function_B6(CScriptState* pState);
	void Function_B7(CScriptState* pState);
	void Function_B8(CScriptState* pState);
	void Function_B9(CScriptState* pState);
	void Function_BA(CScriptState* pState);
	void Function_BB(CScriptState* pState);
	void Function_BC(CScriptState* pState);
	void Function_BD(CScriptState* pState);
	void Function_BE(CScriptState* pState);
	void Function_BF(CScriptState* pState);
	void Function_C0(CScriptState* pState);
	void Function_C1(CScriptState* pState);
	void Function_C2(CScriptState* pState);
	void Function_C3(CScriptState* pState);
	void Function_C4(CScriptState* pState);
	void Function_C5(CScriptState* pState);
	void Function_C6(CScriptState* pState);
	void Function_C7(CScriptState* pState);
	void Function_C8(CScriptState* pState);
	void Function_C9(CScriptState* pState);
	void Function_CA(CScriptState* pState);
	void Function_CB(CScriptState* pState);
	void Function_CC(CScriptState* pState);
	void Function_CD(CScriptState* pState);
	void Function_CE(CScriptState* pState);
	void Function_CF(CScriptState* pState);
	void Function_D0(CScriptState* pState);
	void Function_D1(CScriptState* pState);
	void Function_D2(CScriptState* pState);
	void Function_D3(CScriptState* pState);
	void Function_D4(CScriptState* pState);
	void Function_D5(CScriptState* pState);
	void Function_D6(CScriptState* pState);
	void Function_D7(CScriptState* pState);
	void Function_D8(CScriptState* pState);
	void Function_D9(CScriptState* pState);
	void Function_DA(CScriptState* pState);
	void Function_DB(CScriptState* pState);
	void Function_DC(CScriptState* pState);
	void Function_DD(CScriptState* pState);
	void Function_DE(CScriptState* pState);
	void Function_DF(CScriptState* pState);
	void Function_E0(CScriptState* pState);

	void(CUAKMScript::*_functions[97])(CScriptState* pState);

	void AddCaption(CScriptState* pState, BOOL TexTalk);

	void Play(CScriptState* pState, int index, int bank, int rate);
	void PlayAudio(CScriptState* pState, int index);
	void Show(CScriptState* pState, int index);
};
