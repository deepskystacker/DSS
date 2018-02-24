/*----------------------------------------------------------------------
Copyright (c) 1998 Russell Freeman. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
Email: russf@gipsysoft.com
Web site: http://www.gipsysoft.com

This code may be used in compiled form in any way you desire. This
file may be redistributed unmodified by any means PROVIDING it is 
not sold for profit without the authors written consent, and 
providing that this notice and the authors name is included. If 
the source code in this file is used in any commercial application 
then a simple email would be nice.

This file is provided 'as is' with no expressed or implied warranty.
The author accepts no liability if it causes any damage to your
computer.

Expect bugs.

Please use and enjoy. Please let me know of any bugs/mods/improvements 
that you have found/implemented and I will fix/incorporate them into this
file.

File:	HandleMouseWheelRegisteredMessage.cpp
Owner:	russf@gipsysoft.com
Purpose:	Functon to handle the regsitered message associated with the
					mouse wheel. This will simply re-package the message and send
					it on to the main window as a WM_MOUSEWHEEL. This code originated
					in from MFC source code, and then I made it better.
----------------------------------------------------------------------*/
#include "stdafx.h"

#if !defined (_WIN32_WCE)

#ifndef MSH_MOUSEWHEEL
	#include <ZMouse.h>
#endif	//	MSH_MOUSEWHEEL

extern LRESULT FASTCALL HandleMouseWheelRegisteredMessage( HWND hwnd, WPARAM wParam, LPARAM lParam );

LRESULT FASTCALL HandleMouseWheelRegisteredMessage( HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	//
	//	Fabricate the key state information for our shiny new WM_MOUSEWHEEL message
	//	we are about to send
	WPARAM wpKeyState = ( wParam << 16 );
	if( WinHelper::IsControlPressed() )
		wpKeyState |= MK_CONTROL;

	if( WinHelper::IsShiftPressed() )
		wpKeyState |= MK_SHIFT;


	//
	//	Get the focus window, if there is no focus window then simply send our fabricated
	//	WM_MOUSEWHEEL message to the window passed.
	//	If there is a focus window then according to the docs we shoudl propagate the
	//	message of through the windows until we get to the desktop(owner of all windows)
	//	or until some window handles the message
	HWND hwFocus = ::GetFocus();
	const HWND hwDesktop = ::GetDesktopWindow();
	LRESULT lResult = 0;
	if( hwFocus == NULL )
	{
		lResult = SendMessage( hwnd, WM_MOUSEWHEEL, wpKeyState, lParam );
	}
	else
	{
		do
		{
			lParam = ::SendMessage( hwFocus, WM_MOUSEWHEEL, wpKeyState, lParam );
			hwFocus = ::GetParent( hwFocus );
		}
		while( lParam == 0 && hwFocus != NULL && hwFocus != hwDesktop );
	}
	return lResult;
}
#endif	//	 !defined (_WIN32_WCE)
