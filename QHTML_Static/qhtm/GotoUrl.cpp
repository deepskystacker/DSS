/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GotoUrl.cpp
Owner:	russf@gipsysoft.com
Purpose:	Goto a URL, code taken form various articles and suggestions.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <SHELLAPI.H>	//	For ShellExecute

#if !defined (_WIN32_WCE)

static LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS) {
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }

    return retval;
}
#endif	//	#if !defined (_WIN32_WCE)

extern bool striremove( LPTSTR pszString, LPCTSTR pcszSub );

bool GotoURL( LPCTSTR url, int showcmd )
{
#if !defined (_WIN32_WCE)

	bool bRetVal = false;
  TCHAR key[ MAX_PATH + MAX_PATH + MAX_PATH ];

	if( _tcsnicmp( url, _T("mailto:"), 7 ) && GetRegKey( HKEY_CLASSES_ROOT, _T(".htm"), key ) == ERROR_SUCCESS )
	{
		lstrcat( key, _T("\\shell\\open\\command") );

		if( GetRegKey( HKEY_CLASSES_ROOT, key, key ) == ERROR_SUCCESS )
		{
			TCHAR *pos;
			pos = _tcsstr(key, _T("\"%1\""));
			if( pos == NULL )
			{
				pos = _tcsstr(key, _T("%1"));       // Check for %1, without quotes 
				if( pos == NULL )                   // No parameter at all...
					pos = key+lstrlen(key)-1;
				else
					*pos = '\0';                   // Remove the parameter
			}
			else
			{
				*pos = '\0';                       // Remove the parameter
			}

			if( _tcsstr( key, _T("iexplore") ) )
			{
				_tcscat( key, _T("-new") );
				striremove( key, _T("-nohome") );
			}
			lstrcat( key, _T(" ") );
			lstrcat( key, url );
			STARTUPINFO         si   = {0};
			PROCESS_INFORMATION pi   = {0};
			si.cb = sizeof(si);
			si.dwFlags    = STARTF_USESHOWWINDOW | STARTF_FORCEONFEEDBACK;
			si.wShowWindow = SW_SHOW;
			bRetVal = CreateProcess (NULL, key,
														NULL, NULL,
														TRUE, 0,
														NULL, NULL,
														&si, &pi
														) ? true : false;

		}
	}
	else
	{
		// If it failed, get the .htm regkey and lookup the program
		HINSTANCE result = ShellExecute( NULL, _T("open"), url, NULL, NULL, showcmd );
		if( result > (HINSTANCE)HINSTANCE_ERROR )
		{
			bRetVal = true;
		}
	}

	return bRetVal;
#else
	UNREF( url );
	UNREF( showcmd );
	return false;
#endif	//	#if !defined (_WIN32_WCE)
}

