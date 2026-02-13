#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

#pragma pack(1)
struct PDExamStruct
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
