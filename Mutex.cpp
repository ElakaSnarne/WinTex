#include "Mutex.h"
#include "Utilities.h"

CMutex::CMutex()
{
	_hMutex = CreateMutex(NULL, FALSE, NULL);

	_currentThreadId = 0;
	_lockCount = 0;
}

CMutex::~CMutex()
{
	CloseHandle(_hMutex);
}

BOOL CMutex::Lock(int timeout)
{
	DWORD currentThread = GetCurrentThreadId();
	if (_currentThreadId == currentThread)
	{
		_lockCount++;
		return TRUE;
	}

	//Trace(L"Lock requested by thread ");
	//Trace((int)currentThread);
	//Trace(L"\r\n");

	DWORD dwWaitResult = WaitForSingleObject(_hMutex, timeout);
	if (dwWaitResult == WAIT_OBJECT_0)
	{
		//_currentThreadId = currentThread;
		_lockCount++;

		//Trace(L"Lock acquired by thread ");
		//Trace((int)currentThread);
		//Trace(L"\r\n");

		return TRUE;
	}

	return FALSE;
}

void CMutex::Release()
{
	_lockCount--;
	if (_lockCount == 0)
	{
		_currentThreadId = 0;

		//Trace(L"Lock released by thread ");
		//Trace((int)currentThread);
		//Trace(L"\r\n");
	}

	ReleaseMutex(_hMutex);
}
