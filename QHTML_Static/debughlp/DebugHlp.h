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

File:	DebugHlp.h
Owner:	russf@gipsysoft.com
Purpose:	Debugging helpers
----------------------------------------------------------------------*/
#ifndef DEBUGHLP_H
#define DEBUGHLP_H	1

#ifndef _WINDOWS_
	#include <windows.h>
#endif	//	_WINDOWS_

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif	//	_INC_TCHAR

#undef ASSERT
#undef VERIFY
#undef TRACE

#pragma warning( disable : 4127 )	//	: conditional expression is constant

#define _DEBUG_HELP_NO_LINK_LIBS 1
#ifndef _DEBUG_HELP_NO_LINK_LIBS
	#define DBGH_BASELIBPATH "\\Astro\\Projects\\QHTML_Static\\bin\\"

	#if defined(_DEBUG)
		#if defined(_UNICODE)
			#define DBGH_VERSIONFOLDER "Debug.Unicode"
		#else
			#define DBGH_VERSIONFOLDER "Debug"
		#endif

	#elif defined(RELEASE_DEBUG)
		#if defined(_UNICODE)
			#error No Release/Debug library defined for Unicode Configuration
		#else
			#define DBGH_VERSIONFOLDER "Release Debug"
		#endif
	#else
		#if defined(_UNICODE)
			#define DBGH_VERSIONFOLDER "Release.Unicode"
		#else
			#define DBGH_VERSIONFOLDER "Release"
		#endif
	#endif // _DEBUG

#ifndef DBGH_VERSIONFOLDER
	#define DBGH_VERSIONFOLDER "Debug"
#endif

	#pragma comment( lib, DBGH_BASELIBPATH DBGH_VERSIONFOLDER "\\debughlp.lib" )

/*
	#ifdef _WIN64
		#define DH_PLATFORM	"64"
	#else		//	 _WIN64
		#define DH_PLATFORM	""
	#endif	//	 _WIN64

	#ifdef _UNICODE
		#define DH_UNICODE	"U"
	#else		//	 _UNICODE
		#define DH_UNICODE	""
	#endif	//	 _UNICODE

	#ifdef _DEBUG
		#define DH_DEBUG	"D"
	#else		//	_DEBUG
		#define DH_DEBUG	""
	#endif	//	 _DEBUG
	
	#define DH_LIB_POST DH_PLATFORM DH_UNICODE DH_DEBUG
	//#pragma message( "**** Linking against DebugHLP DebugHLP" DH_LIB_POST ".lib ****" )
	#pragma comment( lib, "DebugHlp" DH_LIB_POST ".lib" )

	#undef DH_LIB_POST
	#undef DH_PLATFORM
	#undef DH_UNICODE
	#undef DH_DEBUG
*/
#endif	//	_DEBUG_HELP_NO_LINK_LIBS

#ifndef _WINDOWS_
	#include <windows.h>
#endif	//_WINDOWS_

#ifndef _INC_TCHAR
	#include <tchar.h>
#endif	//	_INC_TCHAR

	void _cdecl DebugTraceToFileA( LPCSTR pcszFilename );
	void _cdecl DebugTraceToFileW( LPCWSTR pcszFilename );

	void _cdecl DebugTraceA( LPCSTR pcszFormat, ... );
	void _cdecl DebugTraceW( LPCWSTR pcszFormat, ... );

	void _cdecl DebugTraceLineAndFileA( LPCSTR pcszFilename, int nLine );
	void _cdecl DebugTraceLineAndFileW( LPCWSTR pcszFilename, int nLine );

	void _cdecl DebugTraceMemory( LPVOID lp, UINT uSize );
	void _cdecl DH_DebugBreak();

	//	Return TRUE if the program should retry, use STOPHRE to do the retry.
	BOOL _cdecl AssertFailedA( LPCSTR pcszFilename, int nLine, LPCSTR pcszExpression );
	BOOL _cdecl AssertFailedW( LPCSTR pcszFilename, int nLine, LPCSTR pcszExpression );

	BOOL _cdecl ApiFailure( LPCSTR pcszFilename, int nLine, LPCSTR pcszExpression, int nGetLastError );

	#ifndef DEBUGHLP_BARE_TRACE
		#define TRACEA ::DebugTraceLineAndFileA( __FILE__, __LINE__ ), ::DebugTraceA
		#define TRACEW ::DebugTraceLineAndFileW( _T(__FILE__), __LINE__ ), ::DebugTraceW
		#define TRACENLA ::DebugTraceA
		#define TRACENLW ::DebugTraceW
		#define TRACE_MEMORYA	DebugTraceLineAndFileA( _T(__FILE__), __LINE__ ), ::DebugTraceMemory
		#define TRACE_MEMORYW	DebugTraceLineAndFileA( _T(__FILE__), __LINE__ ), ::DebugTraceMemory

	#else	//	DEBUGHLP_BARE_TRACE
		#define TRACEA ::DebugTraceA
		#define TRACEW ::DebugTraceW
		#define TRACE_MEMORY	::DebugTraceMemory
	#endif	//	DEBUGHLP_BARE_TRACE

#ifdef _DEBUG
	#define SETLOGFILEA( pcszFilename )	::DebugTraceToFileA( pcszFilename )
	#define SETLOGFILEW( pcszFilename )	::DebugTraceToFileW( pcszFilename )
#ifdef _WIN64
	#ifndef _INC_CRTDBG
		#include <CRTDBG.h>
	#endif	//	_INC_CRTDBG
	#define STOPHERE()	_CrtDbgBreak();
#else
	#ifdef _WIN32
		#define STOPHERE()	_asm{ int 3 }
	#endif
#endif

	
	#define VERIFY( exp )	ASSERT( exp )
	#define ASSERT( exp ) \
		/*lint -e717 -e506 -e774*/ \
		do \
		{ \
			if( !( exp ) )\
			{\
				TRACEA( "Assert Failed with expression %s\n", #exp);\
				if( ::AssertFailed( __FILE__, __LINE__, #exp ) ) \
				{\
					STOPHERE();	\
				}\
			}\
		} while (0) \
		/*lint +e717 +e506 +e774*/ \

	#define VAPI(exp) \
		/*lint -e717 -e506 -e774*/ \
		do \
		{ \
			if( !(exp) )\
			{\
				int nGetLastError = GetLastError();\
				TRACEA( "VAPI Failed with expression %s\n", #exp);\
				if( ApiFailure( __FILE__, __LINE__, #exp, nGetLastError ) ) \
				{\
					STOPHERE();	\
				}\
			}\
		} while (0) \
		/*lint +e717 +e506 +e774*/ \

	#define ASSERT_VALID_HWND( hwnd )						ASSERT( ::IsWindow( hwnd ) )
	#define ASSERT_VALID_STR_LEN( str, n )			ASSERT( !::IsBadStringPtr( str, n ) )
	#define ASSERT_VALID_STR( str )							ASSERT_VALID_STR_LEN( str, 0xfffff )
	#define ASSERT_VALID_WRITEPTR( obj, n )			ASSERT( !::IsBadWritePtr( obj, n ) )
	#define ASSERT_VALID_READPTR( obj, n )			ASSERT( !::IsBadReadPtr( obj, n ) )
	#define ASSERT_VALID_WRITEOBJPTR( obj )			ASSERT_VALID_WRITEPTR( obj, sizeof(*obj ) )
	#define ASSERT_VALID_READOBJPTR( obj )			ASSERT_VALID_READPTR( obj, sizeof(*obj ) )

	#ifdef _UNICODE
		#define TRACE TRACEW
		#define TRACENL	TRACENLW
		#define TRACE_MEMORY TRACE_MEMORYW
		#define SETLOGFILE SETLOGFILEW
		#define AssertFailed	AssertFailedW
	#else	//	_UNICODE
		#define TRACE TRACEA
		#define TRACENL	TRACENLA
		#define TRACE_MEMORY TRACE_MEMORYA
		#define SETLOGFILE SETLOGFILEA
		#define AssertFailed	AssertFailedA
	#endif	//	_UNICODE

#else	//	_DEBUG
	#define ASSERT_VALID_HWND( hwnd )						((void)0)
	#define ASSERT_VALID_STR_LEN( str, n )			((void)0)
	#define ASSERT_VALID_STR( str )							((void)0)
	#define ASSERT_VALID_WRITEPTR( obj, n )			((void)0)
	#define ASSERT_VALID_READPTR( obj, n )			((void)0)
	#define ASSERT_VALID_WRITEOBJPTR( obj )			((void)0)
	#define ASSERT_VALID_READOBJPTR( obj )			((void)0)

	#ifdef _UNICODE
		#define TRACE							1 ? (void)0 : ::DebugTraceW
		#define TRACENL						1 ? (void)0 : ::DebugTraceW
	#else	//	_UNICODE
		#define TRACE							1 ? (void)0 : ::DebugTraceA
		#define TRACENL						1 ? (void)0 : ::DebugTraceA
	#endif	//	_UNICODE

	#define ASSERT(exp)					((void)0)
	#define STOPHERE					((void)0)
	#define VERIFY(exp)					((void)(exp))
	#define VAPI(exp)					((void)(exp))
	#define SETLOGFILE				__noop
	#define TRACE_MEMORY				1 ? (void)0 : ::DebugTraceMemory
#endif	//	_DEBUG

#endif //DEBUGHLP_H
