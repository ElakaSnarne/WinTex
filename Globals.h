#pragma once

#include <Windows.h>
#include "DirectX.h"
#include "Caption.h"
#include "ScriptBase.h"
#include <list>
#include <iterator>
#include "ConstantBuffers.h"
#include "Shaders.h"
#include "Configuration.h"
#include "DXDialogueOption.h"
#include "Mutex.h"
#include "ModuleController.h"
#include "DXFont.h"
#include "MIDIPlayer.h"

extern float twopi;

class CAnimBase;

enum class VideoMode
{
	Embedded = 0,
	FullScreen = 1
};

enum class ConversationOption
{
	None = 0,
	A = 1,
	B = 2,
	C = 3,

	AskAbout = 4,
	Offer = 7,
	Abort = 99,
};

extern CDirectX dx;
extern VideoMode videoMode;
extern ConversationOption conversationOption;
extern std::list<CCaption*> captions1;
extern std::list<CCaption*> captions2;
extern std::list<CCaption*>* pDisplayCaptions;
extern std::list<CCaption*>* pAddCaptions;
extern PWSTR gamePath;

extern CMutex _lock;

enum class Alignment
{
	Default = 0x00,
	Left = 0x01,
	Right = 0x02,
	CenterX = 0x04,
	Top = 0x08,
	Bottom = 0x10,
	CenterY = 0x20,
	ScaleX = 0x40,
	ScaleY = 0x80,
	Scale = ScaleX | ScaleY
};

inline Alignment operator|(Alignment a, Alignment b)
{
	return static_cast<Alignment>(static_cast<int>(a) | static_cast<int>(b));
}

inline Alignment operator&(Alignment a, Alignment b)
{
	return static_cast<Alignment>(static_cast<int>(a) & static_cast<int>(b));
}

extern HWND _hWnd;

extern CConfiguration* pConfig;

extern CDXDialogueOption DialogueOptions[3];
extern int DialogueOptionsCount;
extern CDXFont TexFont;

extern CMIDIPlayer* pMIDI;

extern BOOL isUAKM;
