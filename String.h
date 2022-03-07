#pragma once

#include <Windows.h>

class CString
{
public:
												CString();
												CString(const CString&);
												CString(const WCHAR*);
												CString(const WCHAR);
												CString(const unsigned int);
	virtual										~CString();

	//	Operators
	CString&									operator=(const CString&);
	CString&									operator=(const WCHAR*);
	CString										operator+(const CString&);
	CString										operator+(const WCHAR*);
	CString&									operator+=(const CString&);
	CString&									operator+=(const WCHAR*);
	BOOL										operator==(const CString&);
	BOOL										operator==(const WCHAR*);
	BOOL										operator!=(const CString&);
	BOOL										operator!=(const WCHAR*);
	BOOL										operator>(const CString&);
	BOOL										operator<(const CString&);

												operator WCHAR*();
												operator void*();

	WCHAR&										operator[](int);

	int											Length() const;

	CString										Left(int);
	CString										Right(int);
	CString										Mid(int, int);

	void										MakeLower();
	void										MakeUpper();

	int											Compare(const CString&);
	int											Compare(const WCHAR*);
	int											CompareNoCase(const CString&);
	int											CompareNoCase(const WCHAR*);

	WCHAR*										m_pString;
	int											m_iLength;

private:
	void										Clear();
	void										Copy(const WCHAR*);
	void										Append(const WCHAR*);
};
