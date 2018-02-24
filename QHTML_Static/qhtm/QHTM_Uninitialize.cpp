/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Uninitialize.cpp
Owner:	russf@gipsysoft.com
Purpose:	Uninitialise QHTM
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "qhtm.h"
#include <guitools/guitools.h>

extern void QHTM_StylesShutdown();
extern void QHTM_ColoursShutdown();
extern void StopSubclassing();
extern void QHTM_FontInfoShutdown();
extern void QHTM_ParserShutdown();

BOOL WINAPI QHTM_Uninitialize()
{
	StopSubclassing();

#ifdef QHTM_ALLOW_TOOLTIPS
	QHTM_StylesShutdown();
#endif	//	QHTM_ALLOW_TOOLTIPS

	QHTM_ColoursShutdown();
	QHTM_FontInfoShutdown();
	QHTM_ParserShutdown();

	GS::CDrawContext::Shutdown();

	return TRUE;
}
