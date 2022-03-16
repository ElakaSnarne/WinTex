#pragma once

#include <list>

struct ObjectMap
{
	int id;
	int ObjectIndex;
	std::list<int> SubObjectIndices;
	std::list<float*> VisibilityFloatPointers;
};

struct ObjectVisibilityMapping
{
	int ObjectIndex;
	int SubObjectIndex;
	int SubObjectId;
};
