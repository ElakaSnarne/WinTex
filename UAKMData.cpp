#include "UAKMData.h"

CUAKMData::CUAKMData()
{
	_score = 0;

	ZeroMemory(_A, sizeof(_A));
	ZeroMemory(_B, sizeof(_B));
	ZeroMemory(_AskAbout, sizeof(_AskAbout));
}

CUAKMData::~CUAKMData()
{
}

void CUAKMData::Init()
{
	_dmap.Init();
	_map.Init();
}

void CUAKMData::SetParameterA(int index, BYTE value)
{
	if (index >= 0 && index < 256)
	{
		_A[index] = value;
	}
}

BYTE CUAKMData::GetParameterA(int index)
{
	return (index >= 0 && index < 256) ? _A[index] : 0;
}

void CUAKMData::SetParameterB(int index, BYTE value)
{
	if (index >= 0 && index < 256)	// B limit is probably higher...
	{
		_B[index] = value;
	}
}

BYTE CUAKMData::GetParameterB(int index)
{
	return (index >= 0 && index < 256) ? _B[index] : 0;
}

void CUAKMData::SetAskAboutState(int index, BYTE state)
{
	if (index >= 0 && index < 256)
	{
		_AskAbout[index] = state;
	}
}

BYTE CUAKMData::GetAskAboutState(int index)
{
	return (index >= 0 && index < 256) ? _AskAbout[index] : 0;
}

void CUAKMData::SetHintState(int index, BYTE state)
{
	if (index >= 0 && index < 256)
	{
		_HintState[index] = state;
	}
}

BYTE CUAKMData::GetHintState(int index)
{
	return (index >= 0 && index < 256) ? _HintState[index] : 0;
}
