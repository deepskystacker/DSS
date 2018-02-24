/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Initialize.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "qhtm.h"
#include "TipWIndow.h"
#include "buildno.h"
#include <guitools/guitools.h>

extern bool FASTCALL RegisterWindow( HINSTANCE hInst );
extern void QHTM_StylesStartup();
extern void QHTM_ColoursStartup();
extern void QHTM_ParserStartup();


BOOL WINAPI QHTM_Initialize( HINSTANCE hInst )
{
//
//	If we are a DLL then this gets set in the DLLMain, however, if we are statically linked
//	then we need to set it to the EXE.
#ifndef QHTM_DLL
	//TRACE( _T("QHTM_Initialize - Static Lib - build ") _T( QHTM_BUILD_VERSION ) );
	g_hQHTMInstance = hInst;
#else
	TRACE( _T("QHTM_Initialize - DLL - build ")  _T( QHTM_BUILD_VERSION ) );

	#ifdef UNICODE
		TRACENL( _T(" UNICODE version") );
	#endif	//	UNICODE

#endif	//	QHTM_DLL
	//TRACENL( _T("\n") );

	#ifdef _DEBUG
		//SETLOGFILE( _T("d:\\QHTM.log") );
	#endif	//	_DEBUG

#ifdef QHTM_ALLOW_TOOLTIPS
	QHTM_StylesStartup();
#endif	//	QHTM_ALLOW_TOOLTIPS
	QHTM_ColoursStartup();
	QHTM_ParserStartup();
	GS::CDrawContext::Startup();

	if( RegisterWindow( hInst ) )
	{
#ifdef QHTM_ALLOW_TOOLTIPS
		return CTipWindow::Register( hInst );
#else	//	QHTM_ALLOW_TOOLTIPS
		return TRUE;
#endif	//	QHTM_ALLOW_TOOLTIPS

	}

	return FALSE;
}
