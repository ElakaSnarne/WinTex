#pragma once

#include <Windows.h>

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
