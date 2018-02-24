/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	stdafx.h
Owner:	russf@gipsysoft.com
Purpose:	Precompiled header file.
----------------------------------------------------------------------*/
#ifndef STDAFX_H
#define STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_IE 0x0500

#pragma warning( disable : 4355 )	//	'this' : used in base member initializer list
#pragma warning( disable : 4710 )	//	'function' : function not inlined - specifically for the ArrayClass

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_DEPRECATE	1
#define _CRT_NON_CONFORMING_SWPRINTFS	1

#include <windows.h>
#include <TCHAR.h>
#include <stdlib.h>
#include <commctrl.h>
#ifndef _WIN32_WCE
	#include <ZMouse.h>
#endif	//	_WIN32_WCE
#include <Windowsx.h>

#include "QHTM_Includes.h"

#ifndef GET_WHEEL_DELTA_WPARAM
	#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif	//	GET_WHEEL_DELTA_WPARAM

//#define QHTM_TRACE_ENABLED	1

#endif //STDAFX_H