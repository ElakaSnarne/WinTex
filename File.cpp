#include "File.h"

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
	// Enumerate files, return if file is found
	// Enumerate folders, search each one
	std::wstring foundFile;

	WIN32_FIND_DATA fd;
	HANDLE hFF = FindFirstFile((path + L"*").c_str(), &fd);
	do
	{
		std::wstring enumeratedFile = fd.cFileName;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((enumeratedFile != L".") && (enumeratedFile != L".."))
			{
				foundFile = Find(path + enumeratedFile + L"\\", file);

				if (foundFile.size() > 0)
				{
					break;
				}
			}
		}
		else
		{
			if (enumeratedFile == file)
			{
				foundFile = path + file;
				break;
			}
		}
	} while (FindNextFile(hFF, &fd));

	FindClose(hFF);

	return foundFile;
}

BOOL CFile::Open(LPCWSTR fileName, Mode mode, Sharing share)
{
	// Check map to get real file path
	std::wstring realFile = fileName;
	if (mode == Mode::Read)
	{
		std::unordered_map<std::wstring, std::wstring>::iterator it = FileMap.find(fileName);
		if (it != FileMap.end())
		{
			realFile = it->second;
		}
		else
		{
			if (Exists(fileName))
			{
				// File exists, add to map
				FileMap[fileName] = fileName;
			}
			else
			{
				// File does not exist in expected path, perform a recursive search
				std::wstring path = L"";
				std::wstring file = L"";
				size_t lastSeparator = realFile.find_last_of('\\');
				if (lastSeparator != -1)
				{
					path = realFile.substr(0, lastSeparator + 1);
					file = realFile.substr(lastSeparator + 1);
				}
				else
				{
					path = L".\\";
					file = realFile;
				}

				realFile = Find(path, file);

				FileMap[fileName] = realFile;
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

BOOL CFile::Exists(LPCWSTR fileName)
{
	WIN32_FIND_DATA findFileData;
	HANDLE handle = FindFirstFile(fileName, &findFileData);
	BOOL found = (handle != INVALID_HANDLE_VALUE);
	if (found)
	{
		FindClose(handle);
	}

	return found;
}
