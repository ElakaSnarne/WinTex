#pragma once

#include <Windows.h>

class CLocationSubObject
{
public:
	CLocationSubObject();
	virtual ~CLocationSubObject();

	int Id;
	int TextureIndex;
	int VertexIndex;
	int VertexCount;
};

class CLocationObject
{
public:
	CLocationObject();
	virtual ~CLocationObject();

	int SubObjectCount;
	CLocationSubObject* pSubObjects;
};
