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

File:	Trace.cpp
Owner:	russf@gipsysoft.com
Purpose:	TRACE handling function.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <reuse/WinHelper.h>
#include "DebugHlp.h"
#include <stdlib.h>

#pragma warning( disable: 4073 )
#pragma init_seg(lib)


static WinHelper::CCriticalSection g_sect;

class CTraceLock: private WinHelper::CCriticalSection::CLock
//
//	Simple lock class for the critcal section
{
public:
	inline CTraceLock()
		: WinHelper::CCriticalSection::CLock( g_sect )
		{
		}

private:
	CTraceLock( const CTraceLock &);
	CTraceLock& operator =( const CTraceLock &);
};



static char g_szLogFileName[ MAX_PATH + MAX_PATH ] = {0};
static wchar_t g_szwLogFileName[ MAX_PATH + MAX_PATH ] = {0};


#ifndef UNDER_CE
void DebugTraceToFileA( LPCSTR pcszFilename )
{
	lstrcpyA( g_szLogFileName, pcszFilename );
}
#endif	//	UNDER_CE


void DebugTraceToFileW( LPCWSTR pcszFilename )
{
	lstrcpyW( g_szwLogFileName, pcszFilename );
	//WideCharToMultiByte( CP_ACP, 0, pcszFilename, -1, g_szLogFileName, MAX_PATH + MAX_PATH, NULL, NULL );
}


#ifndef UNDER_CE
void WriteToLogFileA( LPCSTR pcsz, bool bIncludeDateTime, bool bIncludeCRLF )
{
	if( g_szLogFileName[ 0 ] )
	{
		int nRetries = 40;
		HANDLE h = INVALID_HANDLE_VALUE;

		while( ( h = CreateFileA( g_szLogFileName, GENERIC_WRITE | GENERIC_READ ,FILE_SHARE_READ ,NULL, OPEN_ALWAYS,  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL ) ) == INVALID_HANDLE_VALUE && nRetries-- )
		{
			Sleep( 50 );
		}

		if( h == INVALID_HANDLE_VALUE )
			return;

		SetFilePointer( h, 0, 0, FILE_END );


		DWORD dwWrite;
		if( bIncludeDateTime )
		{
			SYSTEMTIME st;
			GetSystemTime( &st );

			char szDateBuffer[ 256 ];
			(void)WriteFile( h, szDateBuffer, sprintf( szDateBuffer, "%02d-%02d-%04d\t%02d:%02d:%02d\t", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond ), &dwWrite, NULL );
		}

		const DWORD dwSize = (DWORD)strlen( pcsz );
		(void)WriteFile( h, pcsz, dwSize, &dwWrite, NULL );

		if( bIncludeCRLF )
		{
			(void)WriteFile( h, "\r\n", 2, &dwWrite, NULL );
		}

		CloseHandle( h );
	}
}


void _cdecl DebugTraceLineAndFileA( LPCSTR pcszFilename, int nLine )
{
	CTraceLock lock;

	OutputDebugStringA( pcszFilename );

	char szLineNumber[30] = "(";
	_ltoa( nLine, szLineNumber + 1, 10 );
	strcat( szLineNumber , ") : " );
	OutputDebugStringA( szLineNumber );
}


void _cdecl DebugTraceA( LPCSTR pcszFormat, ... )
{
	CTraceLock lock;
 	va_list marker;
	va_start( marker, pcszFormat );

	LPSTR pszBuffer = DBGH_vmalprintfA( pcszFormat, marker );
	OutputDebugStringA( pszBuffer );
	if( g_szLogFileName[0] )
	{
		WriteToLogFileA( pszBuffer, true, false );
	}

	free( pszBuffer );

	va_end( marker );
}
#endif	//	UNDER_CE


void WriteToLogFileW( LPCSTR pcsz, bool bIncludeDateTime, bool bIncludeCRLF )
{
	if( g_szLogFileName[ 0 ] )
	{
		int nRetries = 40;
		HANDLE h = INVALID_HANDLE_VALUE;

		while( ( h = CreateFileW( g_szwLogFileName, GENERIC_WRITE | GENERIC_READ ,FILE_SHARE_READ ,NULL, OPEN_ALWAYS,  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL ) ) == INVALID_HANDLE_VALUE && nRetries-- )
		{
			Sleep( 50 );
		}

		if( h == INVALID_HANDLE_VALUE )
			return;

		SetFilePointer( h, 0, 0, FILE_END );


		DWORD dwWrite;
		if( bIncludeDateTime )
		{
			SYSTEMTIME st;
			GetSystemTime( &st );

			char szDateBuffer[ 256 ];
			(void)WriteFile( h, szDateBuffer, sprintf( szDateBuffer, "%02d-%02d-%04d\t%02d:%02d:%02d\t", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond ), &dwWrite, NULL );
		}

		const DWORD dwSize = (DWORD)strlen( pcsz );
		(void)WriteFile( h, pcsz, dwSize, &dwWrite, NULL );

		if( bIncludeCRLF )
		{
			(void)WriteFile( h, "\r\n", 2, &dwWrite, NULL );
		}

		CloseHandle( h );
	}
}


void _cdecl DebugTraceLineAndFileW( LPCWSTR pcszFilename, int nLine )
{
	CTraceLock lock;
	OutputDebugStringW( pcszFilename );

	WCHAR szLineNumber[30] = L"(";
	_ltow( nLine, szLineNumber + 1, 10 );
	wcscat( szLineNumber , L") : " );
	OutputDebugStringW( szLineNumber );
}


void _cdecl DebugTraceW( LPCWSTR pcszFormat, ... )
{
	CTraceLock lock;

 	va_list marker;
	va_start( marker, pcszFormat );

	LPWSTR pszBuffer = DBGH_vmalprintfW( pcszFormat, marker );

	OutputDebugStringW( pszBuffer );
	if( g_szLogFileName[0] )
	{
		WriteToLogFileW( g_szLogFileName, true, false );
	}

	free( pszBuffer );

	va_end( marker );

}

void _cdecl DebugTraceMemory( LPVOID lp, UINT uSize )
{
	if( ::IsBadReadPtr( lp, uSize ) )
	{
		TRACE( _T("Can't read memory for DebugTraceMemory\n") );
	}
	else
	{
		char *pChar = (char *)lp;
		TCHAR szBufferText[ 256 ] = _T("");
		TCHAR szBufferHex[ 256 ] = _T("");
		TCHAR szHexChar[ 4 ];
		
		wsprintf( szBufferText, _T("Memory dump of %d bytes at 0x%08x\r\n"), uSize, lp );
		OutputDebugString( szBufferText );
		szBufferText[ 0 ] = '\000';

		CTraceLock lock;
		int nChars = 0;
		for( UINT u = 0; u < uSize; u++ )
		{
			if( *pChar < 32 )
			{
				_tcscat( szBufferText, _T(".") );
			}
			else
			{
				TCHAR sz[ 2 ] = {(TCHAR)*pChar, 0 };
				_tcscat( szBufferText, sz );
			}
			_tcscat( szBufferText, _T("  ") );
			wsprintf( szHexChar, _T("%02x"), (unsigned char)(*pChar) );
			_tcscat( szBufferHex, szHexChar );
			_tcscat( szBufferHex, _T(" ") );
			nChars++;

			if( nChars == 16 )
			{
				nChars = 0;
				_tcscat( szBufferText, _T("\r\n") );
				_tcscat( szBufferHex, _T("\r\n") );
				OutputDebugString( szBufferText );
				OutputDebugString( szBufferHex );
				szBufferText[ 0 ] = szBufferHex[ 0 ] = 0;
			}
			pChar++;
		}

		if( nChars  )
		{
			_tcscat( szBufferText, _T("\r\n") );
			_tcscat( szBufferHex, _T("\r\n") );
			OutputDebugString( szBufferText );
			OutputDebugString( szBufferHex );
			szBufferText[ 0 ] = szBufferHex[ 0 ] = 0;
		}
		
	}
}