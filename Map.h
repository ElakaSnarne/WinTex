#pragma once

#include <Windows.h>
#include <vector>
#include "StartupPosition.h"
#include "MapData.h"

class CMap
{
public:
	~CMap();
	virtual BOOL Init() = NULL;

	CMapData* Get(int entry);
	StartupPosition GetStartupPosition(int index, int entry);

protected:
	std::vector<CMapData*> _entries;

	int ReadStartupPositions(CMapData* pMapdata, LPBYTE data, int offset, int numberOfStartupPositions, int positionDataStructSize);
};
