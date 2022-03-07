#include "String.h"

CString::CString()
{
	m_pString = 0;
	m_iLength = 0;
}

CString::CString(const CString& sSrc)
{
	m_pString = 0;
	m_iLength = 0;

	Copy(sSrc.m_pString);
}

CString::CString(const WCHAR* pSrc)
{
	m_pString = 0;
	m_iLength = 0;

	Copy(pSrc);
}

CString::CString(const WCHAR cSrc)
{
	m_pString = new WCHAR[2];
	m_iLength = 0;
	if (m_pString)
	{
		m_pString[0] = cSrc;
		m_pString[1] = 0;
		m_iLength = 1;
	}
}

CString::CString(const unsigned int length)
{
	m_pString = new WCHAR[length + 1];
	m_iLength = 0;
	if (m_pString)
	{
		memset(m_pString, 0, (length + 1) * 2);
		m_iLength = length;
	}
}

CString::~CString()
{
	Clear();
}

//	Operators
CString& CString::operator=(const CString& sSrc)
{
	Copy(sSrc.m_pString);

	return *this;
}

CString& CString::operator=(const WCHAR* pSrc)
{
	Copy(pSrc);

	return *this;
}

CString CString::operator+(const CString& sSrc)
{
	CString sNew(m_pString);
	sNew.Append(sSrc.m_pString);

	return sNew;
}

CString CString::operator+(const WCHAR* pSrc)
{
	CString sNew(m_pString);
	sNew.Append(pSrc);

	return sNew;
}

CString& CString::operator+=(const CString& sSrc)
{
	Append(sSrc.m_pString);

	return *this;
}

CString& CString::operator+=(const WCHAR* pSrc)
{
	Append(pSrc);

	return *this;
}

BOOL CString::operator==(const CString& sCmp)
{
	return Compare(sCmp.m_pString) ? FALSE : TRUE;
	/*
		if (!pCmp)
		{
			if (!m_pString || !strlen(m_pString)) return true;
			return false;
		}

		if (!m_pString)
		{
			if (!pCmp || !strlen(pCmp)) return true;
			return false;
		}
	*/

	//	return (strcmp(m_pString, sCmp.m_pString)) ? false : true;
}

BOOL CString::operator==(const WCHAR* pCmp)
{
	return Compare(pCmp) ? FALSE : TRUE;

	/*
		if (!pCmp)
		{
			if (!m_pString || !strlen(m_pString)) return true;
			return false;
		}

		if (!m_pString)
		{
			if (!pCmp || !strlen(pCmp)) return true;
			return false;
		}

		return (strcmp(m_pString, pCmp)) ? false : true;
	*/
}

BOOL CString::operator!=(const CString& sCmp)
{
	return (Compare(sCmp.m_pString)) ? TRUE : FALSE;
	//	return (strcmp(m_pString, sCmp.m_pString)) ? true : false;
}

BOOL CString::operator!=(const WCHAR* pCmp)
{
	return (Compare(pCmp)) ? TRUE : FALSE;
	//	return (strcmp(m_pString, pCmp)) ? true : false;
}

BOOL CString::operator>(const CString& sCmp)
{
	return (Compare(sCmp.m_pString) > 0) ? TRUE : FALSE;
	//	return (strcmpi(m_pString, sCmp.m_pString)<0) ? true : false;
}

BOOL CString::operator<(const CString& sCmp)
{
	return (Compare(sCmp.m_pString) < 0) ? TRUE : FALSE;
	//	return (strcmpi(m_pString, sCmp.m_pString)>0) ? true : false;
}

CString::operator WCHAR*()
{
	return m_pString;
}

CString::operator void*()
{
	return (void*)m_pString;
}

void CString::Clear()
{
	if (m_pString != NULL)
	{
		delete[] m_pString;
		m_pString = 0;
		m_iLength = 0;
	}
}

void CString::Copy(const WCHAR* pSrc)
{
	Clear();

	if (pSrc)
	{
		int iLen = wcslen(pSrc) + 1;
		if (iLen > 0 && iLen < 10000)
		{
			WCHAR* pTemp = new WCHAR[iLen];
			if (pTemp)
			{
				for (int i = 0; i < iLen; i++) pTemp[i] = pSrc[i];

				m_pString = pTemp;
				m_iLength = iLen - 1;
			}
		}
		else
		{
			int iDebug = 0;
		}
	}
}

int CString::Length() const
{
	return m_iLength;
}

void CString::Append(const WCHAR* pSrc)
{
	if (pSrc)
	{
		int iLen = wcslen(pSrc);
		if (iLen > 0 && iLen < 10000)
		{
			int iNewLen = m_iLength + iLen + 1;
			if (iNewLen > 0 && iNewLen < 10000)
			{
				WCHAR* pOld = m_pString;
				WCHAR* pNew = new WCHAR[iNewLen];
				if (pNew)
				{
					int i;
					for (i = 0; i < m_iLength; i++) pNew[i] = pOld[i];
					for (i = 0; i <= iLen; i++) pNew[m_iLength + i] = pSrc[i];

					Clear();

					m_pString = pNew;
					m_iLength = iNewLen - 1;
				}
			}
			else
			{
				int iDebug = 0;
			}
		}
		else
		{
			int iDebug = 0;
		}
	}
}

CString operator+(const WCHAR* s1, const CString& s2)
{
	CString sNew(s1);
	sNew += s2;
	return sNew;
}

CString operator+(const CString& s1, const CString& s2)
{
	CString sNew(s1);
	sNew += s2;
	return sNew;
}

CString CString::Left(int iCount)
{
	if (iCount > m_iLength) iCount = m_iLength;

	if (m_pString)
	{
		WCHAR usTemp = m_pString[iCount];
		m_pString[iCount] = 0;
		CString sRet = m_pString;
		m_pString[iCount] = usTemp;
		return sRet;
	}

	return L"";
}

CString CString::Right(int iCount)
{
	if (iCount > m_iLength) iCount = m_iLength;

	return (m_pString && iCount > 0) ? (m_pString + m_iLength - iCount) : L"";
}

CString CString::Mid(int iStart, int iLength)
{
	CString sRet;
	if (iStart < m_iLength && iLength>0)
	{
		if ((iStart + iLength) > m_iLength) iLength = m_iLength - iStart;

		WCHAR cTemp = *(m_pString + iStart + iLength);
		*(m_pString + iStart + iLength) = 0;
		sRet = m_pString + iStart;
		*(m_pString + iStart + iLength) = cTemp;
	}

	return sRet;
}

void CString::MakeLower()
{
	for (int i = 0; i < m_iLength; i++) m_pString[i] = tolower(m_pString[i]);
}

void CString::MakeUpper()
{
	for (int i = 0; i < m_iLength; i++) m_pString[i] = toupper(m_pString[i]);
}

int CString::Compare(const CString& sCmp)
{
	return Compare(sCmp.m_pString);
}

int CString::Compare(const WCHAR* pCmp)
{
	if (!pCmp)
	{
		if (!m_pString || !wcslen(m_pString)) return 0;

		return -1;
	}
	if (!m_pString)
	{
		if (!pCmp || !wcslen(pCmp)) return 0;

		return 1;
	}

	return wcscmp(pCmp, m_pString);
}

int CString::CompareNoCase(const CString& sCmp)
{
	return CompareNoCase(sCmp.m_pString);
}

int CString::CompareNoCase(const WCHAR* pCmp)
{
	if (!pCmp)
	{
		if (!m_pString || !wcslen(m_pString)) return 0;

		return -1;
	}
	if (!m_pString)
	{
		if (!pCmp || !wcslen(pCmp)) return 0;

		return 1;
	}

	return wcsicmp(pCmp, m_pString);
}

WCHAR& CString::operator[](int index)
{
	return *(m_pString + index);
}
