/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	QHTM_Includes.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef QHTM_INLCUDES_H
#define QHTM_INLCUDES_H

#ifndef WINHELPER_H
	#include <reuse/WinHelper.h>
#endif	//	WINHELPER_H

#ifndef DEBUGHLP_H
	#include <debughlp/debughlp.h>
#endif	//	DEBUGHLP_H


#include "config.h"

extern HINSTANCE g_hQHTMInstance;

extern UINT g_uHandCursorID;
extern UINT g_uNoImageBitmapID;
extern HINSTANCE g_hResourceInstance;

#ifndef QHTM_TRACE_H
	#include "QHTM_Trace.h"
#endif	//	QHTM_TRACE_H

#endif //QHTM_INLCUDES_H