#pragma once

#include <windows.h>
#include <mmreg.h>
#include "AnimBase.h"
#include <mfplay.h>
#include <mfapi.h>
#include <mferror.h>

class MyObject : public IMFMediaEventGenerator
{
private:
	long                m_nRefCount;    // Reference count.
	CRITICAL_SECTION    m_critSec;      // Critical section.
	IMFMediaEventQueue* m_pQueue;       // Event queue.
	BOOL                m_bShutdown;    // Is the object shut down?

	// CheckShutdown: Returns MF_E_SHUTDOWN if the object was shut down.
	HRESULT CheckShutdown() const
	{
		return (m_bShutdown ? MF_E_SHUTDOWN : S_OK);
	}

public:
	MyObject(HRESULT& hr) : m_nRefCount(0), m_pQueue(NULL), m_bShutdown(FALSE)
	{
		InitializeCriticalSection(&m_critSec);

		// Create the event queue.
		hr = MFCreateEventQueue(&m_pQueue);
	}

	// Shutdown: Shuts down this object.
	HRESULT Shutdown()
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();
		if (SUCCEEDED(hr))
		{
			// Shut down the event queue.
			if (m_pQueue)
			{
				hr = m_pQueue->Shutdown();
			}

			// Release the event queue.
			m_pQueue->Release();
			// Release any other objects owned by the class (not shown).

			// Set the shutdown flag.
			m_bShutdown = TRUE;
		}

		LeaveCriticalSection(&m_critSec);
		return hr;
	}

	// TODO: Implement IUnknown (not shown).
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);

	// IMFMediaEventGenerator::BeginGetEvent
	STDMETHODIMP BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* pState)
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();
		if (SUCCEEDED(hr))
		{
			hr = m_pQueue->BeginGetEvent(pCallback, pState);
		}

		LeaveCriticalSection(&m_critSec);
		return hr;
	}

	// IMFMediaEventGenerator::EndGetEvent
	STDMETHODIMP EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();
		if (SUCCEEDED(hr))
		{
			hr = m_pQueue->EndGetEvent(pResult, ppEvent);
		}

		LeaveCriticalSection(&m_critSec);
		return hr;
	}

	// IMFMediaEventGenerator::GetEvent
	STDMETHODIMP GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
	{
		// Because GetEvent can block indefinitely, it requires
		// a slightly different locking strategy.
		IMFMediaEventQueue* pQueue = NULL;

		// Hold lock.
		EnterCriticalSection(&m_critSec);

		// Check shutdown
		HRESULT hr = CheckShutdown();

		// Store the pointer in a local variable, so that another thread
		// does not release it after we leave the critical section.
		if (SUCCEEDED(hr))
		{
			pQueue = m_pQueue;
			pQueue->AddRef();
		}

		// Release the lock.
		LeaveCriticalSection(&m_critSec);

		if (SUCCEEDED(hr))
		{
			hr = pQueue->GetEvent(dwFlags, ppEvent);
		}

		pQueue->Release();
		return hr;
	}

	// IMFMediaEventGenerator::QueueEvent
	STDMETHODIMP QueueEvent(
		MediaEventType met, REFGUID extendedType,
		HRESULT hrStatus, const PROPVARIANT* pvValue)
	{
		EnterCriticalSection(&m_critSec);

		HRESULT hr = CheckShutdown();
		if (SUCCEEDED(hr))
		{
			hr = m_pQueue->QueueEventParamVar(
				met, extendedType, hrStatus, pvValue);
		}

		LeaveCriticalSection(&m_critSec);
		return hr;
	}

private:

	// The destructor is private. The caller must call Release.
	virtual ~MyObject()
	{
		assert(m_bShutdown);
		assert(m_nRefCount == 0);
	}
};

class MediaPlayerCallback : public IMFPMediaPlayerCallback
{
	long m_cRef; // Reference count

public:

	MediaPlayerCallback() : m_cRef(1)
	{
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = { QITABENT(MediaPlayerCallback, IMFPMediaPlayerCallback), { 0 }, };
		return QISearch(this, qit, riid, ppv);
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG count = InterlockedDecrement(&m_cRef);
		if (count == 0)
		{
			delete this;
		}

		return count;
	}

	// IMFPMediaPlayerCallback methods
	void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER* pEventHeader);
};

class CVideoPlayer : CAnimBase, public IMFAsyncCallback
{
	long m_cRef; // Reference count

public:
	CVideoPlayer();
	~CVideoPlayer();

	void Init(HWND hWnd, LPCTSTR fileName);
	virtual void Update();

	// MFPlay event handler functions
	void OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT* pEvent);
	void OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT* pEvent);

	virtual BOOL ShouldClearDXBuffer() { return FALSE; }
	virtual void Skip();

private:
	IMFPMediaPlayer* _pPlayer;
	MediaPlayerCallback* _pPlayerCB;
	BOOL _bHasVideo;
	IMFMediaSession* _pSession;

	// Inherited via IMFAsyncCallback
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = { QITABENT(MediaPlayerCallback, IMFPMediaPlayerCallback), { 0 }, };
		return QISearch(this, qit, riid, ppv);
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ULONG count = InterlockedDecrement(&m_cRef);
		if (count == 0)
		{
			delete this;
		}

		return count;
	}

	virtual HRESULT __stdcall GetParameters(DWORD* pdwFlags, DWORD* pdwQueue) override;
	virtual HRESULT __stdcall Invoke(IMFAsyncResult* pAsyncResult) override;
};
