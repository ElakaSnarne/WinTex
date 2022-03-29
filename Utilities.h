#pragma once

#ifndef __UTILITIES__
#define __UTILITIES__

#include <Windows.h>
#include "AnimBase.h"
#include "BIC.h"
#include "PTF.h"
#include "Wave.h"
#include "H2O.h"
#include "LZ.h"
#include "File.h"
#include "DoubleData.h"

#define TIMER_SCALE	16.66666

int GetInt(LPBYTE pData, int offset, int length);
void SetInt(LPBYTE pData, int offset, int value, int length);

BinaryData LoadEntry(LPCWSTR fileName, int itemIndex);
DoubleData LoadDoubleEntry(LPCWSTR fileName, int itemIndex);
CCaption* GetFrameCaption(int frame);

void SetGamePath(LPWSTR path);
void Trace(LPCWSTR text);
void Trace(float val, int dc = 2);
void Trace(int val, int rad = 10);
void TraceLine(LPWSTR text);
void TraceLine(float val, int dc = 2);
void TraceLine(int val, int rad = 10);

PBYTE GetResource(int resource, LPWSTR type, PDWORD pSize);

void ClearCaptions(std::list<CCaption*>* pCap);

#endif __UTILITIES__

int GetRegistryInt(HKEY key, LPCWSTR valueName, int defaultValue);
void SetRegistryInt(HKEY key, LPCWSTR valueName, int value);
float GetRegistryFloat(HKEY key, LPCWSTR valueName, float defaultValue);
void SetRegistryFloat(HKEY key, LPCWSTR valueName, float value);

void DebugTrace(CScriptState* pState, LPWSTR text);

std::string ToString(LPCWSTR str);

void SwapCaptions();
