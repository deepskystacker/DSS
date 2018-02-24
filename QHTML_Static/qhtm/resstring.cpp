/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	resstring.cpp
Owner:	russf@gipsysoft.com
Purpose:	Simple string resource loader
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ResString.h"
#ifndef _INC_TCHAR
	#include <TCHAR.H>
#endif	//	_INC_TCHAR

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HINSTANCE CResString::g_instStrings = GetModuleHandle( NULL );
HANDLE CResString::g_hHeap = GetProcessHeap();

CResString::CResString( UINT uStringID )
	: m_uStringID( uStringID )
	, m_pcszString( NULL )
	, m_nLength( 0 )
{

}


CResString::~CResString()
{
	if( m_pcszString )
	{
		VERIFY( HeapFree( g_hHeap, HEAP_NO_SERIALIZE, m_pcszString ) );
		m_pcszString = NULL;
	}
}


CResString::operator LPTSTR ()
{
	if( !m_pcszString )
		(void)operator LPCTSTR ();
	return m_pcszString;
}


CResString::operator LPCTSTR () const
//
//	Retrun a pointer to the loaded string, if the string is not already
//	loaded then we will load it.
//
//	Can return NULL if the string is not found.
{
	if( !m_pcszString )
	{
		//
		//	Generally, find the string in teh resourcse, determine the size of the entire resource
		//	table and allocate enough memory for that. Then load the string. If all is well we will
		//	resize the string memory block so as not to waste space and then return the newly allocated memory.

		ASSERT( g_instStrings );
		HRSRC hRes = FindResource( g_instStrings, MAKEINTRESOURCE( m_uStringID / 16 + 1 ), RT_STRING );
		if( hRes )
		{
			DWORD dwSize = SizeofResource( g_instStrings, hRes );
			LPTSTR pcszString = static_cast<LPTSTR>( HeapAlloc( g_hHeap, HEAP_ZERO_MEMORY, dwSize * sizeof( TCHAR ) ) );
			if( !LoadString( g_instStrings, m_uStringID, pcszString, dwSize ) )
			{
				//TRACE( _T("Resource not found as string or not loaded\n" ));
				//	Resource not found???
				VERIFY( HeapFree( g_hHeap, HEAP_NO_SERIALIZE, pcszString ) );
				return NULL;
			}
			m_nLength = lstrlen( pcszString );
			m_pcszString = static_cast<LPTSTR>( HeapReAlloc( g_hHeap, HEAP_ZERO_MEMORY, pcszString, ( static_cast<DWORD>( m_nLength + 1 ) * sizeof(TCHAR) ) ) );
			if( !m_pcszString )
			{
				m_pcszString = pcszString;
			}
		}
		else
		{
			//TRACE(_T("Failed to load string with ID %d\n"), m_uStringID );
		}
	}
	return m_pcszString;
}


int CResString::GetLength() const
//
//	return the length of the string in characters.
{
	if( !m_pcszString )
		(void)operator LPCTSTR ();

	return m_nLength;
}
