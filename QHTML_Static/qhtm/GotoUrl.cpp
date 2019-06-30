/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GotoUrl.cpp
Owner:	russf@gipsysoft.com
Purpose:	Goto a URL, code taken form various articles and suggestions.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <SHELLAPI.H>	//	For ShellExecute

#define DSS_USE_DEFAULT_BROWSER 1

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

#ifdef DSS_USE_DEFAULT_BROWSER
static LONG GetRegSubKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_ENUMERATE_SUB_KEYS, &hkey);

	if (retval == ERROR_SUCCESS)
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		retval = RegEnumKey(hkey, 0, data, datasize);
		if (retval == ERROR_SUCCESS)
		{
			lstrcpy(retdata, data);
		}
		RegCloseKey(hkey);
	}

	return retval;
}
#endif  //  #ifdef DSS_USE_DEFAULT_BROWSER
#endif	//	#if !defined (_WIN32_WCE)

extern bool striremove( LPTSTR pszString, LPCTSTR pcszSub );

bool GotoURL( LPCTSTR url, int showcmd )
{
#if !defined (_WIN32_WCE)
#ifdef DSS_USE_DEFAULT_BROWSER
	bool bRetVal = false;
	TCHAR key[MAX_PATH + MAX_PATH + MAX_PATH];
	if (_tcsnicmp(url, _T("mailto:"), 7) != 0)
	{
		TCHAR subkey[MAX_PATH];
		TCHAR *keyPrefix = _T("Software\\Clients\\StartMenuInternet");
		bool fLocalMachine = false;
		LONG rc = GetRegSubKey(HKEY_CURRENT_USER, keyPrefix, subkey);
		if (rc != ERROR_SUCCESS)
		{
			keyPrefix = _T("SOFTWARE\\Clients\\StartMenuInternet");
			fLocalMachine = true;
			rc = GetRegSubKey(HKEY_LOCAL_MACHINE, keyPrefix, subkey);
		}

		if (rc == ERROR_SUCCESS)
		{
			lstrcpy(key, keyPrefix);
			lstrcat(key, _T("\\"));
			lstrcat(key, subkey);
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(fLocalMachine ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER, key, key) == ERROR_SUCCESS)
			{
				lstrcat(key, _T(" "));
				lstrcat(key, url);

				STARTUPINFO         si = { 0 };
				PROCESS_INFORMATION pi = { 0 };
				si.cb = sizeof(si);
				si.dwFlags = STARTF_USESHOWWINDOW | STARTF_FORCEONFEEDBACK;
				si.wShowWindow = SW_SHOW;
				bRetVal = CreateProcess(NULL, key,
					NULL, NULL,
					TRUE, 0,
					NULL, NULL,
					&si, &pi
				) ? true : false;
			}
		}
	}
	
	if (!bRetVal)
	{
		// If it failed, get the .htm regkey and lookup the program
		HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL, NULL, showcmd);
		if (result > (HINSTANCE)HINSTANCE_ERROR)
		{
			bRetVal = true;
		}
	}

	return bRetVal;
#else

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
#endif
#else
	UNREF( url );
	UNREF( showcmd );
	return false;
#endif	//	#if !defined (_WIN32_WCE)
}

