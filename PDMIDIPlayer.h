#pragma once
#include "MIDIPlayer.h"

class CPDMIDIPlayer : public CMIDIPlayer
{
public:
	virtual void Init(BinaryData data);

protected:
	virtual DWORD Player();
};
