#pragma once

#include <Windows.h>

#pragma pack(1)
typedef struct PDExamStruct
{
	short Id;
	short AddItemId;
	short ParameterAIndex;
	BYTE ParameterAValue;
	BYTE AskAbout1;
	BYTE AskAbout2;
	BYTE Travel1;
	BYTE Travel2;
	BYTE Unknown1;
	BYTE ExamFileNumber;
	BYTE ExamEntryNumber;
	int DescriptionOffset;
	BYTE Flags;
	BYTE Rate;
	BYTE Unknown2;
	short HintState;
};
#pragma pack(8)
