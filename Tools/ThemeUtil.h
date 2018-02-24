#pragma once

/**********************************************************************
**
**	ThemeUtil.h : include file
**
**	by Andrzej Markowski June 2004
**
**********************************************************************/

#include "TmSchema.h"

#define THM_WM_THEMECHANGED     0x031A

typedef struct _MY_MARGINS
{
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} MY_MARGINS;

class CThemeUtil
{
public:
	CThemeUtil();
	virtual ~CThemeUtil();
	BOOL OpenThemeData(HWND hWnd, LPCWSTR pszClassList);
	void CloseThemeData();
	BOOL DrawThemePart(HDC hdc, int iPartId, int iStateId, const RECT *pRect);
	BOOL GetThemeColor(int iPartId, int iStateId, int iPropId, const COLORREF *pColor);
	BOOL GetThemeInt(int iPartId, int iStateId, int iPropId, const int *piVal);
	BOOL GetThemeMargins(int iPartId, int iStateId, int iPropId, const MY_MARGINS *pMargins);
	BOOL GetThemeEnumValue(int iPartId, int iStateId, int iPropId, const int *piVal);
	BOOL GetThemeFilename(int iPartId, int iStateId, int iPropId, 
							OUT LPWSTR pszThemeFileName, int cchMaxBuffChars);
	BOOL GetCurrentThemeName(OUT LPWSTR pszThemeFileName, int cchMaxNameChars, 
							OUT OPTIONAL LPWSTR pszColorBuff, int cchMaxColorChars,
							OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);
	HBITMAP LoadBitmap(LPWSTR pszBitmapName);
private:
	BOOL IsWinXP(void);
	void FreeLibrary();
private:
	HINSTANCE m_hUxThemeDll;
	HINSTANCE m_hRcDll;
	UINT m_hTheme;
};
