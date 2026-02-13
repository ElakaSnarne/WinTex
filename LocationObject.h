#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

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
