#include "File.h"
#include "Platform.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <cstring>

namespace fs = std::filesystem;

// Helper to convert wstring to string
static std::string WStrToStr(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    try {
        return converter.to_bytes(wstr);
    } catch (...) {
        return "";
    }
}

static std::wstring StrToWStr(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    try {
        return converter.from_bytes(str);
    } catch (...) {
        return L"";
    }
}

std::unordered_map<std::wstring, std::wstring> CFile::FileMap;

CFile::CFile()
{
	_handle = INVALID_HANDLE_VALUE;
}

CFile::~CFile()
{
	Close();
}

std::wstring CFile::Find(std::wstring path, std::wstring file)
{
    std::string sPath = ResolvePath(WStrToStr(path));
    std::string sFile = WStrToStr(file);
    
    if (sPath.empty()) sPath = ".";

    try {
        if (!fs::exists(sPath)) {
             return L"";
        }

        for (const auto& entry : fs::recursive_directory_iterator(sPath, fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (strcasecmp(filename.c_str(), sFile.c_str()) == 0) {
                     std::string foundPath = entry.path().string();
                     return StrToWStr(foundPath);
                }
            }
        }
    } catch (...) {}
    return L"";
}

BOOL CFile::Open(LPCWSTR fileName, Mode mode, Sharing share)
{
	std::wstring realFile = fileName;
	if (mode == Mode::Read)
	{
		auto it = FileMap.find(fileName);
		if (it != FileMap.end())
		{
			realFile = it->second;
		}
		else
		{
			if (Exists(fileName))
			{
				FileMap[fileName] = fileName;
			}
			else
			{
				std::wstring path = L".";
				std::wstring file = fileName;
                
                std::string sFileName = WStrToStr(fileName);
                std::replace(sFileName.begin(), sFileName.end(), '\\', '/');
                size_t lastSlash = sFileName.find_last_of('/');
                if (lastSlash != std::string::npos) {
                    file = StrToWStr(sFileName.substr(lastSlash + 1));
                    path = StrToWStr(sFileName.substr(0, lastSlash));
                }

				realFile = Find(path, file);
                if (!realFile.empty())
				    FileMap[fileName] = realFile;
                else 
                    realFile = fileName;
			}
		}
	}

	Creation c = (mode == Mode::Read) ? Creation::OpenExisting : (mode == Mode::Write) ? Creation::CreateAlways : Creation::OpenAlways;
	_handle = CreateFile(realFile.c_str(), (DWORD)mode, (DWORD)share, NULL, (DWORD)c, (DWORD)Flags::Normal, NULL);
	return (_handle != INVALID_HANDLE_VALUE);
}

void CFile::Close()
{
	if (_handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(_handle);
		_handle = INVALID_HANDLE_VALUE;
	}
}

DWORD CFile::Seek(DWORD distance, SeekMethod method)
{
	return SetFilePointer(_handle, distance, NULL, (DWORD)method);
}

int CFile::Read(LPBYTE pBuffer, int length)
{
	int read = 0;
	ReadFile(_handle, pBuffer, length, (LPDWORD)&read, NULL);
	return read;
}

int CFile::Write(LPBYTE pBuffer, int length)
{
	int written = 0;
	WriteFile(_handle, pBuffer, length, (LPDWORD)&written, NULL);
	return written;
}

DWORD CFile::Size()
{
	return GetFileSize(_handle, NULL);
}

BOOL CFile::Exists(LPCWSTR fileName)
{
    std::string path = ResolvePath(WStrToStr(fileName));
    return fs::exists(path);
}
