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
#include "PDMIDIPlayer.h"
#include "Enums.h"
#include "Overlay.h"

extern float twopi;

class CAnimBase;

extern CDirectX dx;
extern VideoMode videoMode;
extern ConversationOption conversationOption;
extern std::list<CCaption*> captions1;
extern std::list<CCaption*> captions2;
extern std::list<CCaption*>* pDisplayCaptions;
extern std::list<CCaption*>* pAddCaptions;
extern PWSTR gamePath;

extern CMutex _lock;

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

extern COverlay* pOverlay;
extern COverlay* pClimbLadderOverlay;
extern COverlay* pConvertPointsOverlay;
extern COverlay* pElevationModOverlay;

extern int DefaultCaptionColour1;
extern int DefaultCaptionColour2;
extern int DefaultCaptionColour3;
extern int DefaultCaptionColour4;
