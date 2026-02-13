#pragma once

#include "Platform.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "Win32Compat.h"
#endif

class CPointList
{
public:
	CPointList();
	CPointList(int first, int count);
	virtual ~CPointList();

	void Add(int first, int count);
	void Remove(int first, int count);
	void Clear();

	int First;
	int Count;

	CPointList* Next;
	CPointList* Prev;
};
