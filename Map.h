#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif
#include <vector>
#include "StartupPosition.h"
#include "MapData.h"

class CMap
{
public:
	~CMap();
	virtual BOOL Init() = 0;

	CMapData* Get(int entry);
	StartupPosition GetStartupPosition(int index, int entry);

protected:
	std::vector<CMapData*> _entries;

	int ReadStartupPositions(CMapData* pMapdata, LPBYTE data, int offset, int numberOfStartupPositions, int positionDataStructSize);
};
