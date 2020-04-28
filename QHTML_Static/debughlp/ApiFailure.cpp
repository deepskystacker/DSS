/*----------------------------------------------------------------------
Copyright (c) 1998-present Russ Freeman. All Rights Reserved.
Web site: http://www.gipsysoft.com/

This software is provided 'as-is', without any express or implied warranty.

In no event will the author be held liable for any damages arising from the
use of this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject
to the following restrictions:

1) This source code may not be distributed as part of a commercial library
   without written permission from the author
2) The origin of this software must not be misrepresented; you must not claim
   that you wrote the original software. If you use this software in a product,
	 an acknowledgment in the product documentation is requested but not required.
3) Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
4) Altered source is encouraged to be submitted back to the original author so
   it can be shared with the community. Please share your changes. Don't make me
	 use GPL!
5) This notice may not be removed or altered from any source distribution.
6) If you have any confusion then ask

File:	ApiFailure.cpp
Owner:	russf@gipsysoft.com
Purpose:	API Verify Failure message box, if an API function fails then
					this will display a message box with the filename of the source
					file, the line number, the expression that failed and the result of
					GetLastError(...)
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DebugHlp.h"

#ifndef countof
	#define countof( t )	(sizeof( (t) ) / sizeof( (t)[0] ) )
#endif	//	countof

BOOL _cdecl ApiFailure( LPCSTR pcszFilename, int nLine, LPCSTR pcszExpression, int nGetLastError )
{
	LPCTSTR lpMsgBuf;
	(void)::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, nGetLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL );

	char szExeName[ MAX_PATH ];
  if( !GetModuleFileNameA( NULL, szExeName, countof( szExeName ) ) )
		strcpy( szExeName, "<No Program Name>" );


	char szMessage[ 1024 ];
	_snprintf(szMessage, countof(szMessage)
		, "API VERIFY Failure!"
		"\nProgram: %s"
		"\n"
		"\nFile %s"
		"\nLine %d"
		"\n"
		"\nExpression %s"
		"\n"
		"\nLast Error %d"
#if defined(_UNICODE)
							"\n           %ws"	
#else
							"\n           %s"
#endif
							"\n\nPress Retry to debug the application"
						, szExeName
						, pcszFilename
						, nLine
						, pcszExpression
						, nGetLastError
						, lpMsgBuf
						);

#ifndef UNDER_CE
	(void)LocalFree( (LPVOID)lpMsgBuf );
	HWND hwndParent = ::GetActiveWindow();
	hwndParent = ::GetLastActivePopup( hwndParent );
  int nCode = ::MessageBoxA( hwndParent, szMessage, "Debug Helper", MB_TASKMODAL | MB_ICONHAND | MB_ABORTRETRYIGNORE | MB_SETFOREGROUND );

  if( nCode == IDABORT )
  {
		exit( nCode );
  }

  if( nCode == IDRETRY )
		return 1;
#endif	UNDER_CE
	return FALSE;
}
