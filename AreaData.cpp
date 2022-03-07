#include "AreaData.h"

CAreaData::CAreaData()
{
	CategoryOptionOffsets = NULL;
	CategoryOptionCounts = NULL;
	CategoryAreas = NULL;
	Table2 = NULL;
	Table3 = NULL;
	Table4 = NULL;
}

CAreaData::~CAreaData()
{
}

void CAreaData::Init(LPBYTE data)
{
	CategoryOptionOffsets = data;
	CategoryOptionCounts = data + 6;
	CategoryAreas = (AreaData_Table1b*)(data + 12);
	data += 61;

	// Table 2
	Table2 = data + 2;
	data += (2 + data[0] + (data[1] << 8));

	// Table 3
	Table3 = (AreaData_Table3*)(data + 2);
	data += (2 + data[0] + (data[1] << 8));

	// Table 4
	Table4 = (AreaData_Table4*)(data + 2);
}
