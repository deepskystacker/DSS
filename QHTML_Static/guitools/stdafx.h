/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	StdAfx.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef STDAFX_H
#define STDAFX_H

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <WinUser.h>
#include <tchar.h>

#include <debughlp/debughlp.h>
#include <reuse/winhelper.h>

#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

#pragma warning( disable: 4710 )	

#endif //STDAFX_H