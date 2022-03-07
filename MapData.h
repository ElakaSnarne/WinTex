#pragma once

#include <vector>
#include "FileMap.h"
#include "StartupPosition.h"

class CMapData
{
public:
	int ScriptFileIndex;
	int ScriptFileEntry;
	int LocationFileIndex;

	std::vector<FileMap> EnvironmentAudioMap;
	std::vector<FileMap> AudioMap;
	std::vector<FileMap> VideoMap;
	std::vector<FileMap> ImageMap;
	std::vector<int> AnimationMap;
	std::vector<int> ObjectMap;

	std::vector<StartupPosition> StartupPositions;
};
