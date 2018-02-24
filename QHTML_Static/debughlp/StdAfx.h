// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__18996BB8_5956_11D2_B63D_000000000000__INCLUDED_)
#define AFX_STDAFX_H__18996BB8_5956_11D2_B63D_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <tchar.h> 

LPWSTR DBGH_vmalprintfW( LPCWSTR pcszFormat, va_list list );
LPSTR DBGH_vmalprintfA( LPCSTR pcszFormat, va_list list );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__18996BB8_5956_11D2_B63D_000000000000__INCLUDED_)
