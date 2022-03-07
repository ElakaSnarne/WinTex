#include "DirectX.h"
#include "AnimBase.h"
#include <list>"
#include "Caption.h"
#include "GameBase.h"
#include "ModuleBase.h"

#pragma comment( lib, "dxguid.lib")

CDirectX dx;

VideoMode videoMode = VideoMode::FullScreen;	// Play title video in fullscreen
ConversationOption conversationOption = ConversationOption::None;
std::list<CCaption*> captions1;
std::list<CCaption*> captions2;
std::list<CCaption*>* pDisplayCaptions = &captions1;
std::list<CCaption*>* pAddCaptions = &captions2;

PWSTR gamePath = NULL;

float twopi = XM_PI * 2;
HWND _hWnd = NULL;

CConfiguration* pConfig = NULL;

CMutex _lock;

CDXDialogueOption DialogueOptions[3];
int DialogueOptionsCount = 0;
CDXFont TexFont;

CMIDIPlayer* pMIDI = NULL;

BOOL isUAKM = TRUE;
