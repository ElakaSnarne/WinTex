#include "VideoPlayer.h"

#pragma comment(linker, \
    "\"/manifestdependency:type='Win32' "\
    "name='Microsoft.Windows.Common-Controls' "\
    "version='6.0.0.0' "\
    "processorArchitecture='*' "\
    "publicKeyToken='6595b64144ccf1df' "\
    "language='*'\"")

#pragma comment(lib, "Mfplay.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "Mfuuid.lib")
#pragma comment(lib, "Mf.lib")

#include <mfidl.h>

CVideoPlayer* g_pPlayer = NULL;

CVideoPlayer::CVideoPlayer()
{
	_pPlayer = NULL;
	_pPlayerCB = NULL;
	_pSession = NULL;
}

CVideoPlayer::~CVideoPlayer()
{
	// Cleanup
	if (_pPlayer != NULL)
	{
		_pPlayer->Shutdown();
		_pPlayer->Release();
		_pPlayer = NULL;
	}

	if (_pPlayerCB != NULL)
	{
		_pPlayerCB->Release();
		_pPlayerCB = NULL;
	}

	if (_pSession != NULL)
	{
		_pSession->Release();
		_pSession = NULL;
	}
}

void CVideoPlayer::Init(HWND hWnd, LPCTSTR fileName)
{
	_done = FALSE;
	_bHasVideo = FALSE;

	g_pPlayer = this;

	HRESULT hr = MFStartup(MF_VERSION);
	hr = MFCreateMediaSession(NULL, &_pSession);

	hr = _pSession->BeginGetEvent((IMFAsyncCallback*)this, NULL);

	IMFSourceResolver* pSR = NULL;
	hr = MFCreateSourceResolver(&pSR);
	MF_OBJECT_TYPE ObjectType = MF_OBJECT_INVALID;
	IUnknown* pSource = NULL;
	hr = pSR->CreateObjectFromURL(fileName, MF_RESOLUTION_MEDIASOURCE, NULL, &ObjectType, &pSource);
	if (hr == S_OK)
	{
		IMFMediaSource* pMSource = NULL;
		hr = pSource->QueryInterface<IMFMediaSource>(&pMSource);
		IMFPresentationDescriptor* pPD = NULL;
		hr = pMSource->CreatePresentationDescriptor(&pPD);
		IMFTopology* pTopology = NULL;
		hr = MFCreateTopology(&pTopology);

		DWORD cSourceStreams = 0;
		pPD->GetStreamDescriptorCount(&cSourceStreams);

		for (DWORD i = 0; i < cSourceStreams; i++)
		{
			BOOL fSelected = FALSE;
			IMFStreamDescriptor* pSD = NULL;
			hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
			if (fSelected)
			{
				IMFActivate* pSinkActivate = NULL;

				IMFMediaTypeHandler* pHandler = NULL;
				hr = pSD->GetMediaTypeHandler(&pHandler);

				GUID guidMajorType;
				hr = pHandler->GetMajorType(&guidMajorType);
				if (MFMediaType_Audio == guidMajorType)
				{
					hr = MFCreateAudioRendererActivate(&pSinkActivate);
				}
				else if (MFMediaType_Video == guidMajorType)
				{
					hr = MFCreateVideoRendererActivate(hWnd, &pSinkActivate);
				}

				if (pSinkActivate != NULL)
				{
					IMFTopologyNode* pSourceNode = NULL;
					hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode);
					hr = pSourceNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
					hr = pSourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
					hr = pSourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);
					hr = pTopology->AddNode(pSourceNode);

					IMFTopologyNode* pOutputNode = NULL;
					hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode);
					hr = pOutputNode->SetObject(pSinkActivate);
					hr = pTopology->AddNode(pOutputNode);
					hr = pOutputNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, TRUE);

					hr = pSourceNode->ConnectOutput(0, pOutputNode, 0);
				}
			}
		}
		//MFSESSION_SETTOPOLOGY_FLAGS
		hr = _pSession->SetTopology(0, pTopology);

		//IMFMediaEventGenerator* pMEG = NULL;


		PROPVARIANT varStart;
		PropVariantInit(&varStart);
		hr = _pSession->Start(&GUID_NULL, &varStart);
		PropVariantClear(&varStart);
	}
}

void CVideoPlayer::Update()
{
	if (_pPlayer != NULL && _bHasVideo == TRUE)
	{
		_pPlayer->UpdateVideo();
	}
}

void MediaPlayerCallback::OnMediaPlayerEvent(MFP_EVENT_HEADER* pEventHeader)
{
	if (FAILED(pEventHeader->hrEvent))
	{
		//ShowErrorMessage(L"Playback error", pEventHeader->hrEvent);
		return;
	}

	switch (pEventHeader->eEventType)
	{
		case MFP_EVENT_TYPE_MEDIAITEM_CREATED:
			g_pPlayer->OnMediaItemCreated(MFP_GET_MEDIAITEM_CREATED_EVENT(pEventHeader));
			break;

		case MFP_EVENT_TYPE_MEDIAITEM_SET:
			g_pPlayer->OnMediaItemSet(MFP_GET_MEDIAITEM_SET_EVENT(pEventHeader));
			break;
	}
}

//-------------------------------------------------------------------
// OnMediaItemCreated
//
// Called when the IMFPMediaPlayer::CreateMediaItemFromURL method
// completes.
//-------------------------------------------------------------------

void CVideoPlayer::OnMediaItemCreated(MFP_MEDIAITEM_CREATED_EVENT* pEvent)
{
	HRESULT hr = S_OK;

	// The media item was created successfully.

	if (_pPlayer)
	{
		BOOL bHasVideo = FALSE, bIsSelected = FALSE;

		// Check if the media item contains video.
		hr = pEvent->pMediaItem->HasVideo(&bHasVideo, &bIsSelected);

		if (FAILED(hr)) { goto done; }

		_bHasVideo = (bHasVideo && bIsSelected);

		// Set the media item on the player. This method completes asynchronously.
		hr = _pPlayer->SetMediaItem(pEvent->pMediaItem);
	}

done:
	if (FAILED(hr))
	{
		//ShowErrorMessage(L"Error playing this file.", hr);
	}
}

void CVideoPlayer::OnMediaItemSet(MFP_MEDIAITEM_SET_EVENT* /*pEvent*/)
{
	HRESULT hr = S_OK;

	hr = _pPlayer->Play();

	if (FAILED(hr))
	{
		//ShowErrorMessage(L"IMFPMediaPlayer::Play failed.", hr);
	}
}

HRESULT __stdcall CVideoPlayer::GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
{
	return E_NOTIMPL;
}

HRESULT __stdcall CVideoPlayer::Invoke(IMFAsyncResult* pAsyncResult)
{
	MediaEventType meType = MEUnknown;  // Event type

	IMFMediaEvent* pEvent = NULL;

	// Get the event from the event queue.
	HRESULT hr = _pSession->EndGetEvent(pAsyncResult, &pEvent);

	// Get the event type. 
	hr = pEvent->GetType(&meType);

	char buffer[128];
	itoa(meType, buffer, 10);
	OutputDebugStringA(buffer);
	buffer[0] = '\r';
	buffer[1] = '\n';
	buffer[2] = 0;
	OutputDebugStringA(buffer);

	if (meType == MESessionClosed || meType == MESessionStopped || meType == MEEndOfStream || meType == MEStreamStopped || meType == MESourceStopped || meType == MEEndOfPresentation)
	{
		_done = TRUE;
	}
	else
	{
		// For all other events, get the next event in the queue.
		hr = _pSession->BeginGetEvent(this, NULL);
	}

	// Check the application state. 

	// If a call to IMFMediaSession::Close is pending, it means the 
	// application is waiting on the m_hCloseEvent event and
	// the application's message loop is blocked. 

	// Otherwise, post a private window message to the application. 

	/*
	if (m_state != Closing)
	{
		// Leave a reference count on the event.
		pEvent->AddRef();

		PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, (WPARAM)pEvent, (LPARAM)meType);
	}
	*/

	pEvent->Release();

	return S_OK;
}
