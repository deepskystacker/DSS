/**********************************************************************
**
**	ThemeUtil.cpp : implementation file of CThemeUtil class
**
**	by Andrzej Markowski June 2004
**
**********************************************************************/

#include "stdafx.h"
#include "themeutil.h"

CThemeUtil::CThemeUtil()
{
	m_hUxThemeDll = NULL;
	m_hRcDll = NULL;
	m_hTheme = NULL;
	FreeLibrary();
	if(IsWinXP())
		m_hUxThemeDll = LoadLibrary(CString(_T("UxTheme.dll")));
}

CThemeUtil::~CThemeUtil()
{
	FreeLibrary();
}

void CThemeUtil::FreeLibrary()
{
	CloseThemeData();
	if(m_hUxThemeDll!=NULL)
		::FreeLibrary(m_hUxThemeDll);
	m_hUxThemeDll = NULL;
}

BOOL CThemeUtil::IsWinXP(void)
{
	OSVERSIONINFO osvi;
	::ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
	return MAKELONG(osvi.dwMajorVersion,osvi.dwMinorVersion) >= MAKELONG(5,1);
}

BOOL CThemeUtil::OpenThemeData(HWND hWnd, LPCWSTR pszClassList)
{
	if(m_hUxThemeDll==NULL || hWnd==NULL || m_hTheme) 
		return FALSE;
	UINT (PASCAL* pfnIsThemeActive)();	// IsThemeActive
	(FARPROC&)pfnIsThemeActive=GetProcAddress(m_hUxThemeDll,"IsThemeActive");
	if(pfnIsThemeActive && pfnIsThemeActive())
	{
		UINT (PASCAL* pfnOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
		(FARPROC&)pfnOpenThemeData=GetProcAddress(m_hUxThemeDll,"OpenThemeData");
		if(pfnOpenThemeData)
			m_hTheme=pfnOpenThemeData(hWnd, pszClassList);
		if(m_hTheme)
		{
			WCHAR szThemeFileName[MAX_PATH];
			WCHAR szColorBuff[MAX_PATH];
			WCHAR szSizeBuff[MAX_PATH];
			
			if(GetCurrentThemeName(szThemeFileName,MAX_PATH,szColorBuff,MAX_PATH,szSizeBuff,MAX_PATH))
			{
				if((m_hRcDll = LoadLibrary(CString(szThemeFileName))))
					return TRUE;				
			}
			CloseThemeData();
			return FALSE;
		}
	}
	return FALSE;
}

void CThemeUtil::CloseThemeData()
{
	if(m_hTheme)
	{
		UINT (PASCAL* pfnCloseThemeData)(HANDLE hTheme);
		(FARPROC&)pfnCloseThemeData=GetProcAddress(m_hUxThemeDll,"CloseThemeData");
		if(pfnCloseThemeData)
			pfnCloseThemeData((HANDLE)m_hTheme);
	}
	m_hTheme = NULL;
	if(m_hRcDll!=NULL)
		::FreeLibrary(m_hRcDll);
	m_hRcDll = NULL;
}

BOOL CThemeUtil::DrawThemePart(HDC hdc, int iPartId, int iStateId, const RECT *pRect)
{
	if(m_hTheme==NULL)
		return FALSE;
	UINT (PASCAL* pfnDrawThemeBackground)(UINT hTheme, HDC hdc, int iPartId, int iStateId,
										const RECT *pRect, const RECT* pClipRect);
	(FARPROC&)pfnDrawThemeBackground=GetProcAddress(m_hUxThemeDll,"DrawThemeBackground");
	if(pfnDrawThemeBackground)
	{
		pfnDrawThemeBackground(m_hTheme, hdc, iPartId, iStateId, pRect, NULL);
		return TRUE;
	}
	return FALSE;
}

BOOL CThemeUtil::GetThemeColor(int iPartId, int iStateId, int iPropId, const COLORREF *pColor)
{
	if(m_hTheme==NULL)
		return FALSE;
	UINT (PASCAL* pfnGetThemeColor)(UINT hTheme, int iPartId, int iStateId, int iPropId, 
										const COLORREF *pColor);
	(FARPROC&)pfnGetThemeColor=GetProcAddress(m_hUxThemeDll,"GetThemeColor");
	if(pfnGetThemeColor)
	{
		pfnGetThemeColor(m_hTheme, iPartId, iStateId, iPropId, pColor);
		return TRUE;
	}
	return FALSE;
}

BOOL CThemeUtil::GetThemeEnumValue(int iPartId, int iStateId, int iPropId, const int *piVal)
{
	if(m_hTheme==NULL)
		return FALSE;

	UINT (PASCAL* pfnGetThemeEnumValue)(UINT hTheme, int iPartId, int iStateId, int iPropId, 
										const int *piVal);
	(FARPROC&)pfnGetThemeEnumValue=GetProcAddress(m_hUxThemeDll,"GetThemeEnumValue");
	if(pfnGetThemeEnumValue)
	{
		pfnGetThemeEnumValue(m_hTheme, iPartId, iStateId, iPropId, piVal);
		return TRUE;
	}
	return FALSE;
}

BOOL CThemeUtil::GetThemeInt(int iPartId, int iStateId, int iPropId, const int *piVal)
{
	if(m_hTheme==NULL)
		return FALSE;

	UINT (PASCAL* pfnGetThemeInt)(UINT hTheme, int iPartId, int iStateId, int iPropId, 
										const int *piVal);
	(FARPROC&)pfnGetThemeInt=GetProcAddress(m_hUxThemeDll,"GetThemeInt");
	if(pfnGetThemeInt)
	{
		pfnGetThemeInt(m_hTheme, iPartId, iStateId, iPropId, piVal);
		return TRUE;
	}
	return FALSE;
}

BOOL CThemeUtil::GetThemeMargins(int iPartId, int iStateId, int iPropId, const MY_MARGINS *pMargins)
{
	if(m_hTheme==NULL)
		return FALSE;

	UINT (PASCAL* pfnGetThemeMargins)(UINT hTheme, OPTIONAL HDC hdc, int iPartId, 
				int iStateId, int iPropId, OPTIONAL RECT *prc, const MY_MARGINS *pMargins);
	(FARPROC&)pfnGetThemeMargins=GetProcAddress(m_hUxThemeDll,"GetThemeMargins");
	if(pfnGetThemeMargins)
	{
		pfnGetThemeMargins(m_hTheme, NULL, iPartId, iStateId, iPropId, NULL, pMargins);
		return TRUE;
	}
	return FALSE;
}

BOOL CThemeUtil::GetThemeFilename(int iPartId, int iStateId, int iPropId, 
								  OUT LPWSTR pszThemeFileName, int cchMaxBuffChars)
{
	if(m_hTheme==NULL)
		return FALSE;
	UINT (PASCAL* pfnGetThemeFilename)(UINT hTheme, int iPartId, int iStateId, int iPropId, 
										OUT LPWSTR pszThemeFileName, int cchMaxBuffChars);
	(FARPROC&)pfnGetThemeFilename=GetProcAddress(m_hUxThemeDll,"GetThemeFilename");
	if(pfnGetThemeFilename)
	{
		pfnGetThemeFilename(m_hTheme, iPartId, iStateId, iPropId, pszThemeFileName, cchMaxBuffChars);
		return TRUE;
	}
	return FALSE;
}

BOOL CThemeUtil::GetCurrentThemeName(OUT LPWSTR pszThemeFileName, int cchMaxNameChars, 
							OUT OPTIONAL LPWSTR pszColorBuff, int cchMaxColorChars,
							OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars)
{
	if(m_hUxThemeDll==NULL)
		return FALSE;
	UINT (PASCAL* pfnGetCurrentThemeName)(OUT LPWSTR pszThemeFileName, 
										int cchMaxNameChars, 
										OUT OPTIONAL LPWSTR pszColorBuff, 
										int cchMaxColorChars,
										OUT OPTIONAL LPWSTR pszSizeBuff, 
										int cchMaxSizeChars);
	(FARPROC&)pfnGetCurrentThemeName=GetProcAddress(m_hUxThemeDll,"GetCurrentThemeName");
	if(pfnGetCurrentThemeName)
	{
		pfnGetCurrentThemeName(pszThemeFileName,cchMaxNameChars,pszColorBuff,cchMaxColorChars,pszSizeBuff,cchMaxSizeChars);
		return TRUE;
	}
	return FALSE;
}

HBITMAP CThemeUtil::LoadBitmap(LPWSTR pszBitmapName)
{
	if(m_hRcDll==NULL)
		return NULL;
	try
	{
		CString sBitmapName(pszBitmapName);
		sBitmapName.Replace('\\','_');
		sBitmapName.Replace('.','_');
		return ::LoadBitmap(m_hRcDll,sBitmapName);
	}
	catch(CMemoryException* e)
	{
		e->Delete();
	}
	return NULL;
}