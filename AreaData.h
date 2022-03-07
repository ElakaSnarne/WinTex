#pragma once

#include <Windows.h>
#include "Utilities.h"

struct AreaData_Table1b
{
	short Y1;
	short Y2;
	short X1;
	short X2;
};

struct AreaData_Table3
{
	short X;
	short Y;
	BYTE unk1;	// ID fields?
	BYTE unk2;
	BYTE unk3;
	BYTE unk4;
};

struct AreaData_Table4
{
	BYTE unk1;
	BYTE unk2;
	short Y1;
	short Y2;
	short X1;
	short X2;
	BYTE WidthIndex;
	BYTE unk4;
	BYTE unk5;
	BYTE unk6;
};

class CAreaData
{
public:
	CAreaData();
	~CAreaData();

	void Init(LPBYTE data);

	// Table 1
	LPBYTE CategoryOptionOffsets;
	// Table 1b
	LPBYTE CategoryOptionCounts;
	AreaData_Table1b* CategoryAreas;

	// Table 2
	LPBYTE Table2;

	// Table 3
	AreaData_Table3* Table3;

	// Table 4
	AreaData_Table4* Table4;
};
