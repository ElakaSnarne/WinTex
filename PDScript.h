#pragma once

#include "ScriptBase.h"
#include <set>

class CPDScript : public CScriptBase
{
public:
	CPDScript();
	virtual ~CPDScript();

	virtual void Execute(CScriptState* pState, int id);
	virtual void Resume(CScriptState* pState, BOOL breakWait = FALSE);
	//virtual void Init(LPBYTE script, int length, bool owner);
	virtual ActionType GetCurrentActions(CScriptState* pState, int currentObjectIndex);
	virtual void PermformAction(CScriptState* pState, int id, ActionType action, int item);

	virtual void SelectDialogueOption(CScriptState* pState, int option);

protected:
	void Function_00(CScriptState* pState);
	void Function_01(CScriptState* pState);
	void Function_02(CScriptState* pState);
	void Function_03(CScriptState* pState);
	void Function_04(CScriptState* pState);
	void Function_05(CScriptState* pState);
	void Function_06(CScriptState* pState);
	void Function_07(CScriptState* pState);
	void Function_08(CScriptState* pState);
	void Function_09(CScriptState* pState);
	void Function_0A(CScriptState* pState);
	void Function_0B(CScriptState* pState);
	void Function_0C(CScriptState* pState);
	void Function_0D(CScriptState* pState);
	void Function_0E(CScriptState* pState);
	void Function_0F(CScriptState* pState);
	void Function_10(CScriptState* pState);
	void Function_11(CScriptState* pState);
	void Function_12(CScriptState* pState);
	void Function_13(CScriptState* pState);
	void Function_14(CScriptState* pState);
	void Function_15(CScriptState* pState);
	void Function_16(CScriptState* pState);
	void Function_17(CScriptState* pState);
	void Function_18(CScriptState* pState);
	void Function_19(CScriptState* pState);
	void Function_1A(CScriptState* pState);
	void Function_1B(CScriptState* pState);
	void Function_1C(CScriptState* pState);
	void Function_1D(CScriptState* pState);
	void Function_1E(CScriptState* pState);
	void Function_1F(CScriptState* pState);
	void Function_20(CScriptState* pState);
	void Function_21(CScriptState* pState);
	void Function_22(CScriptState* pState);
	void Function_23(CScriptState* pState);
	void Function_24(CScriptState* pState);
	void Function_25(CScriptState* pState);
	void Function_26(CScriptState* pState);
	void Function_27(CScriptState* pState);
	void Function_28(CScriptState* pState);
	void Function_29(CScriptState* pState);
	void Function_2A(CScriptState* pState);
	void Function_2B(CScriptState* pState);
	void Function_2C(CScriptState* pState);
	void Function_2D(CScriptState* pState);
	void Function_2E(CScriptState* pState);
	void Function_2F(CScriptState* pState);
	void Function_30(CScriptState* pState);
	void Function_31(CScriptState* pState);
	void Function_32(CScriptState* pState);
	void Function_33(CScriptState* pState);
	void Function_34(CScriptState* pState);
	void Function_35(CScriptState* pState);
	void Function_36(CScriptState* pState);
	void Function_37(CScriptState* pState);
	void Function_38(CScriptState* pState);
	void Function_39(CScriptState* pState);
	void Function_3A(CScriptState* pState);
	void Function_3B(CScriptState* pState);
	void Function_3C(CScriptState* pState);
	void Function_3D(CScriptState* pState);
	void Function_3E(CScriptState* pState);
	void Function_3F(CScriptState* pState);
	void Function_40(CScriptState* pState);
	void Function_41(CScriptState* pState);
	void Function_42(CScriptState* pState);
	void Function_43(CScriptState* pState);
	void Function_44(CScriptState* pState);
	void Function_45(CScriptState* pState);
	void Function_46(CScriptState* pState);
	void Function_47(CScriptState* pState);
	void Function_48(CScriptState* pState);
	void Function_49(CScriptState* pState);
	void Function_4A(CScriptState* pState);
	void Function_4B(CScriptState* pState);
	void Function_4C(CScriptState* pState);
	void Function_4D(CScriptState* pState);
	void Function_4E(CScriptState* pState);
	void Function_4F(CScriptState* pState);
	void Function_50(CScriptState* pState);
	void Function_51(CScriptState* pState);
	void Function_52(CScriptState* pState);
	void Function_53(CScriptState* pState);
	void Function_54(CScriptState* pState);
	void Function_55(CScriptState* pState);
	void Function_56(CScriptState* pState);
	void Function_57(CScriptState* pState);
	void Function_58(CScriptState* pState);
	void Function_59(CScriptState* pState);
	void Function_5A(CScriptState* pState);
	void Function_5B(CScriptState* pState);
	void Function_5C(CScriptState* pState);
	void Function_5D(CScriptState* pState);
	void Function_5E(CScriptState* pState);
	void Function_5F(CScriptState* pState);
	void Function_60(CScriptState* pState);
	void Function_61(CScriptState* pState);
	void Function_62(CScriptState* pState);
	void Function_63(CScriptState* pState);
	void Function_64(CScriptState* pState);
	void Function_65(CScriptState* pState);
	void Function_66(CScriptState* pState);
	void Function_67(CScriptState* pState);
	void Function_68(CScriptState* pState);
	void Function_69(CScriptState* pState);
	void Function_6A(CScriptState* pState);
	void Function_6B(CScriptState* pState);
	void Function_6C(CScriptState* pState);
	void Function_6D(CScriptState* pState);
	void Function_6E(CScriptState* pState);
	void Function_6F(CScriptState* pState);
	void Function_70(CScriptState* pState);
	void Function_71(CScriptState* pState);
	void Function_72(CScriptState* pState);
	void Function_73(CScriptState* pState);
	void Function_74(CScriptState* pState);
	void Function_75(CScriptState* pState);
	void Function_76(CScriptState* pState);
	void Function_77(CScriptState* pState);
	void Function_78(CScriptState* pState);
	void Function_79(CScriptState* pState);
	void Function_7A(CScriptState* pState);
	void Function_7B(CScriptState* pState);
	void Function_7C(CScriptState* pState);
	void Function_7D(CScriptState* pState);
	void Function_7E(CScriptState* pState);
	void Function_7F(CScriptState* pState);
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

	void(CPDScript::* _functions[147])(CScriptState* pState);

	void PlayAudio(int index);
	void PlaySound(int index);
	void PlayVideo(int index, int rate);
	void Show(CScriptState* pState, int index);

	void AddCaption(CScriptState* pState, BOOL TexTalk);

	int _alternatingIndex;

	std::set<int> _women{ 1,8,13,14,20,25,57,61,62 };
	std::set<int> _men{ 0,2,3,4,5,6,7,9,11,16,17,18,21,23,26,28,33,34,40,41,50 };
};
