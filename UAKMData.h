#pragma once

#include <Windows.h>
#include "DMap.h"
#include "Map.h"

class CUAKMData
{
public:
	CUAKMData();
	~CUAKMData();

	void Init();

	void SetParameterA(int index, BYTE value);
	BYTE GetParameterA(int index);
	void SetParameterB(int index, BYTE value);
	BYTE GetParameterB(int index);
	void SetAskAboutState(int index, BYTE state);
	BYTE GetAskAboutState(int index);
	void SetHintState(int index, BYTE state);
	BYTE GetHintState(int index);

	int GetScore() { return _score; }
	void AddScore(int val) { _score += val; }

protected:
	int _score;

	BYTE _A[256];
	BYTE _B[256];		// Could be only 8E long
	BYTE _AskAbout[256];
	BYTE _HintState[256];

	CDMap _dmap;
	CMap _map;
};
