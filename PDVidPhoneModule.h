#pragma once

#include "ModuleBase.h"
#include "Image.h"
#include "FullScreenModule.h"
#include "RawFont.h"
#include <unordered_map>
#include "ScriptBase.h"
#include "DXListBox.h"
#include "PDScriptState.h"
#include <dsound.h>
#include <xaudio2.h>

class CPDVidPhoneModule : public CFullScreenModule, public IXAudio2VoiceCallback
{
public:
	CPDVidPhoneModule();
	~CPDVidPhoneModule();

	virtual void Resize(int width, int height);
	virtual void Render();

	// For sound playback (dialling)
	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) { }
	STDMETHOD_(void, OnVoiceProcessingPassEnd)() { }
	STDMETHOD_(void, OnStreamEnd)() { }
	STDMETHOD_(void, OnBufferStart)(void*) { }
	STDMETHOD_(void, OnBufferEnd)(void*)
	{
		_nextToneTime = GetTickCount64() + 100;
		_readyForNextTone = TRUE;
	}
	STDMETHOD_(void, OnLoopEnd)(void*) { }
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT) { }

protected:
	virtual void Initialize();

	CRawFont _pdRawFont;
	CRawFont _uakmRawFont;

	// Input related
	virtual void Cursor(float x, float y, BOOL relative);
	virtual void BeginAction();
	virtual void Back();

	void UpdatePhonebook();
	void RenderPhonebook();
	int _topCallIndex;
	std::unordered_map<int, int> _callMap;
	void RenderEnterPhoneNumber();

	struct PDVidPhoneMessage
	{
		int DMapEntry;
		int DMapScript;
	};

	std::list<PDVidPhoneMessage> _messages;

	class Phonebook
	{
	public:
		Phonebook() { Index = 0; IsSelected = FALSE; Box = { 0,0,0,0 }; }
		int Index;
		RECT Box;
		BOOL IsSelected;
	};

	std::list<Phonebook*> _phonebook;
	int _mode;
	int _callerIndex;
	int _diallingIndex;

	void LoadVideo(int caller);
	void RenderScreen();
	LPBYTE _screenResetData;

	// Copied from VideoModule
	CScriptBase* _scriptEngine;
	CPDScriptState _scriptState;
	CDXListBox _listBox;

	int _askAboutBase;

	static void DialogueOptionA(LPVOID data);
	static void DialogueOptionB(LPVOID data);
	static void DialogueOptionC(LPVOID data);

	static void SelectOption(int option);
	void SelectDialogueOption(int option);

	IXAudio2SourceVoice* _sourceVoice;
	void PlayNextTone();
	BOOL _readyForNextTone;
	ULONGLONG _nextToneTime;

	Phonebook* _highlighted;

	BOOL _flashMessages;
	BOOL _flashDial;
};
