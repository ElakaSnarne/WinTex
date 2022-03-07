#pragma once

#include <Windows.h>

typedef struct UAKMExamStruct
{
	BYTE Id;
	BYTE AddItemId;
	BYTE ParameterAIndex;
	BYTE ParameterAValue;
	BYTE AskAbout1;
	BYTE AskAbout2;
	BYTE Travel1;
	BYTE Travel2;
	BYTE ExamFileNumber;
	BYTE ExamEntryNumber;
	int DescriptionOffset;
	BYTE Flags;
	short Rate;
	short HintState;
};
