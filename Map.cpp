#include "Map.h"
#include "LZ.h"
#include "Utilities.h"

CMap::~CMap()
{
	// Delete map entries
	std::vector<CMapData*>::iterator it = _entries.begin();
	std::vector<CMapData*>::iterator end = _entries.begin();
	while (it != end)
	{
		delete* (it++);
	}
}

CMapData* CMap::Get(int entry)
{
	if (_entries.size() > entry)
	{
		return _entries.at(entry);
	}

	return NULL;
}

StartupPosition CMap::GetStartupPosition(int index, int entry)
{
	StartupPosition sp;
	ZeroMemory(&sp, sizeof(sp));

	if (_entries.size() > index)
	{
		CMapData* pMap = _entries.at(index);
		if (pMap != NULL)
		{
			if (pMap->StartupPositions.size() > entry)
			{
				sp = pMap->StartupPositions.at(entry);
			}
		}
	}

	return sp;
}
