#include "Utilities.h"
#include "Globals.h"
#include <stdio.h>
#include <stdlib.h>
#include "LZ.h"
#include <iomanip>
#include <sstream>
#include <string>
#include <locale>
#include <codecvt>

int GetInt(LPBYTE pData, int offset, int length)
{
	int ret = 0;
	for (int i = length - 1; i >= 0; i--)
	{
		ret <<= 8;
		ret |= pData[offset + i];
	}

	return ret;
}

void SetInt(LPBYTE pData, int offset, int value, int length)
{
	for (int i = 0; i < length; i++)
	{
		pData[offset + i] = (byte)(value & 0xff);
		value >>= 8;
	}
}

WCHAR _fileName[MAX_PATH];
int _pathLen = 0;
BinaryData LoadEntry(LPCWSTR fileName, int itemIndex)
{
	BinaryData bd;
	ZeroMemory(&bd, sizeof(bd));

	auto nameLen = wcslen(fileName);
	if ((_pathLen + nameLen) < MAX_PATH)
	{
		CopyMemory(_fileName + _pathLen, fileName, (nameLen + 1) * 2);

		CFile file;
		if (file.Open(_fileName))
		{
			int len = 10 + itemIndex * 4;
			LPBYTE header = new BYTE[len];

			if (header != NULL)
			{
				if (file.Read(header, len) == len)
				{
					int count = GetInt(header, 0, 2);
					if (itemIndex >= 0 && itemIndex < (count - 1))
					{
						int offset1 = GetInt(header, 2 + itemIndex * 4, 4);
						int offset2 = GetInt(header, 6 + itemIndex * 4, 4);
						if (offset2 > offset1&& offset1 >= (2 + count * 4))
						{
							int size = offset2 - offset1;
							LPBYTE pBuffer = new BYTE[size];
							if (pBuffer != NULL)
							{
								if (file.Seek(offset1, CFile::SeekMethod::Begin) == offset1)
								{
									if (file.Read(pBuffer, size) == size)
									{
										if (CLZ::IsCompressed(pBuffer, size))
										{
											// Decompress
											BinaryData decompressed = CLZ::Decompress(pBuffer, size);
											bd.Data = decompressed.Data;
											bd.Length = decompressed.Length;
											delete[] pBuffer;
										}
										else
										{
											bd.Data = pBuffer;
											bd.Length = size;
										}
									}
									else
									{
										delete[] pBuffer;
									}
								}
							}
						}
					}
				}

				delete[] header;
			}
		}
		else
		{
			std::wstring err = L"Failed to open file ";
			err += fileName;
			MessageBox(NULL, err.c_str(), L"Load AP Entry", MB_OK);
		}
	}

	return bd;
}

DoubleData LoadDoubleEntry(LPCWSTR fileName, int itemIndex)
{
	DoubleData dd;
	ZeroMemory(&dd, sizeof(dd));

	// Read 2 sequential files (typically palette + image)

	auto nameLen = wcslen(fileName);
	if ((_pathLen + nameLen) < MAX_PATH)
	{
		CopyMemory(_fileName + _pathLen, fileName, (nameLen + 1) * 2);

		CFile file;
		if (file.Open(_fileName))
		{
			int len = 14 + itemIndex * 4;
			LPBYTE header = new BYTE[len];

			if (header != NULL)
			{
				if (file.Read(header, len) == len)
				{
					int count = GetInt(header, 0, 2);
					if (itemIndex >= 0 && itemIndex < (count - 1))
					{
						int offset1 = GetInt(header, 2 + itemIndex * 4, 4);
						int offset2 = GetInt(header, 6 + itemIndex * 4, 4);
						int offset3 = GetInt(header, 10 + itemIndex * 4, 4);
						if (offset2 > offset1&& offset3 > offset2&& offset1 >= (2 + count * 4))
						{
							int size1 = offset2 - offset1;
							LPBYTE pBuffer1 = new BYTE[size1];
							if (pBuffer1 != NULL)
							{
								int size2 = offset3 - offset2;
								LPBYTE pBuffer2 = new BYTE[size2];
								if (pBuffer2 != NULL)
								{
									if (file.Seek(offset1, CFile::SeekMethod::Begin) == offset1)
									{
										if (file.Read(pBuffer1, size1) == size1)
										{
											if (CLZ::IsCompressed(pBuffer1, size1))
											{
												// Decompress
												BinaryData decompressed = CLZ::Decompress(pBuffer1, size1);
												dd.File1.Data = decompressed.Data;
												dd.File1.Length = decompressed.Length;
												delete[] pBuffer1;
											}
											else
											{
												dd.File1.Data = pBuffer1;
												dd.File1.Length = size1;
											}

											if (file.Read(pBuffer2, size2) == size2)
											{
												if (CLZ::IsCompressed(pBuffer2, size2))
												{
													// Decompress
													BinaryData decompressed = CLZ::Decompress(pBuffer2, size2);
													dd.File2.Data = decompressed.Data;
													dd.File2.Length = decompressed.Length;
													delete[] pBuffer2;
												}
												else
												{
													dd.File2.Data = pBuffer2;
													dd.File2.Length = size2;
												}
											}
											else
											{
												delete[] pBuffer2;
											}
										}
										else
										{
											delete[] pBuffer1;
										}
									}
								}
							}
						}
					}
				}

				delete[] header;
			}
		}
		else
		{
			std::wstring err = L"Failed to open file ";
			err += fileName;
			MessageBox(NULL, err.c_str(), L"Load AP Entry", MB_OK);
		}
	}

	return dd;
}

void SetGamePath(LPWSTR path)
{
	_pathLen = static_cast<int>(wcslen(path));
	gamePath = new WCHAR[_pathLen + 1];
	CopyMemory(gamePath, path, (_pathLen + 1) * 2);
	CopyMemory(_fileName, path, _pathLen * 2);
}

CCaption* GetFrameCaption(int frame)
{
	std::list<CCaption*>::iterator it = pDisplayCaptions->begin();
	std::list<CCaption*>::iterator end = pDisplayCaptions->end();
	while (it != end && (*it)->Frame() != frame) it++;
	return (it != end) ? *it : NULL;
}

void Trace(LPCWSTR text)
{
	OutputDebugString(text);
}

void Trace(float val, int dc)
{
	if (val < 0 && ((int)val) == 0)
	{
		Trace(L"-");
	}
	Trace(std::to_wstring(static_cast<int>(val)).c_str());
	Trace(L".");
	if (val < 0.0f) val = -val;
	while (dc-- > 0)
	{
		val -= (int)val;
		val *= 10;
		Trace(std::to_wstring(static_cast<int>(val)).c_str());
	}
}

void Trace(int val, int rad)
{
	std::wstringstream buffer;
	buffer << std::setbase(rad) << val;// std::to_wstring(val);
	Trace(buffer.str().c_str());
}

void TraceLine(LPWSTR text) { Trace(text); Trace(L"\r\n"); }
void TraceLine(LPCWSTR text) { Trace(text); Trace(L"\r\n"); }
void TraceLine(float val, int dc) { Trace(val, dc); Trace(L"\r\n"); }
void TraceLine(int val, int rad) { Trace(val, rad); Trace(L"\r\n"); }

PBYTE GetResource(int resource, LPWSTR type, PDWORD pSize)
{
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(resource), type);
	*pSize = SizeofResource(NULL, hRsrc);
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
	return (PBYTE)LockResource(hGlobal);
}

void ClearCaptions(std::list<CCaption*>* pCap)
{
	std::list<CCaption*>::iterator it = pCap->begin();
	std::list<CCaption*>::iterator end = pCap->end();
	while (it != end)
	{
		delete* (it++);
	}
	pCap->clear();
}

int GetRegistryInt(HKEY key, LPCWSTR valueName, int defaultValue)
{
	int data = 0;
	DWORD size = sizeof(data);
	if (RegGetValue(key, L"", valueName, RRF_RT_REG_DWORD, NULL, (PVOID)&data, &size) == ERROR_SUCCESS)
	{
		return data;
	}

	return defaultValue;
}

void SetRegistryInt(HKEY key, LPCWSTR valueName, int value)
{
	DWORD size = sizeof(value);
	RegSetValueEx(key, valueName, 0, REG_DWORD, (PBYTE)&value, size);
}

float GetRegistryFloat(HKEY key, LPCWSTR valueName, float defaultValue)
{
	DWORD stringLength = 0;
	DWORD size = sizeof(stringLength);
	DWORD keyType = 0;
	if (RegGetValue(key, L"", valueName, RRF_RT_REG_SZ, &keyType, NULL, &stringLength) == ERROR_SUCCESS)
	{
		LPBYTE data = new BYTE[stringLength];
		if (data != NULL)
		{
			if (RegGetValue(key, L"", valueName, RRF_RT_REG_SZ, &keyType, data, &stringLength) == ERROR_SUCCESS)
			{
				float val = wcstof((PCWSTR)data, NULL);
				return val;
			}
		}
	}

	return defaultValue;
}

void SetRegistryFloat(HKEY key, LPCWSTR valueName, float value)
{
	std::wstring data = std::to_wstring(value);
	RegSetValueEx(key, valueName, 0, REG_SZ, (PBYTE)data.c_str(), static_cast<int>(data.length()));
}

void DebugTrace(CScriptState* pState, LPWSTR text)
{
	if (pState->DebugMode)
	{
		Trace(static_cast<int>(reinterpret_cast<uintptr_t>(pState)), 16);
		Trace(L" - ");
		Trace(pState->ExecutionPointer - 1, 16);
		Trace(L" - ");
		TraceLine(text);
	}
}

std::string ToString(LPCWSTR str)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	return converter.to_bytes(str);
}

void SwapCaptions()
{
	std::list<CCaption*>* pOld = pDisplayCaptions;
	pDisplayCaptions = pAddCaptions;
	pAddCaptions = pOld;
	ClearCaptions(pOld);
}

float From12_4(int v)
{
	return ((float)v) / 16.0f;
}

float From16_16(int v)
{
	return ((float)v) / 65536.0f;
}
