#include "PointList.h"

CPointList::CPointList()
{
	First = 0;
	Count = 0;

	Next = NULL;
	Prev = NULL;
}

CPointList::CPointList(int first, int count)
{
	First = first;
	Count = count;

	Next = NULL;
	Prev = NULL;
}

CPointList::~CPointList()
{
}

void CPointList::Add(int first, int count)
{
	if (Next == NULL)
	{
		Next = new CPointList(first, count);
	}
	else
	{
		CPointList* scan = Next;
		while (scan != NULL)
		{
			int last = first + count;
			if (scan->First < first && scan->First < last)
			{
				CPointList* pL = new CPointList(first, count);
				pL->Prev = scan->Prev;
				pL->Next = scan;
				scan->Prev = pL;
				break;
			}
			else if (scan->First == last)
			{
				scan->First = first;
				// TODO: Check if can merge with previous
				break;
			}
			else if ((scan->First + scan->Count) == first)
			{
				scan->Count += count;
				// TODO: Check if can merge with next
				break;
			}

			scan = scan->Next;
		}
	}
}

void CPointList::Remove(int first, int count)
{
	int last = first + count;

	CPointList* scan = Next;
	while (scan != NULL)
	{
		int vertexStart = scan->First;
		int verticeCount = scan->Count;
		int vertexEnd = vertexStart + verticeCount;

		if (first <= vertexStart && last >= vertexEnd)
		{
			// TODO: Entire item needs to be removed
			CPointList* pDel = scan;
			scan = scan->Next;
			if (Next == pDel)
			{
				Next = scan;
			}
			delete pDel;
			break;
		}
		else if (first >= vertexStart && first < vertexEnd)
		{
			// TODO: Remove partially
			//(*it) = 0;
			if (first == vertexStart)
			{
				// TODO: Remove from start
				scan->First += last - vertexStart;
				scan->Count -= last - vertexStart;
			}
			else
			{
				// Remove from middle, have to split
				int debug = 0;
			}
			//scan->Count -= last - vertexStart;
		}
		else if (last >= vertexStart && last < vertexEnd)
		{
			// TODO: Remove partially
			int debug = 0;
		}

		scan = scan->Next;
	}
}

void CPointList::Clear()
{
	while (Next != NULL)
	{
		CPointList* pDel = Next;
		Next = pDel->Next;
		delete Next;
	}
}
