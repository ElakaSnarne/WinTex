#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

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
