/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1997 by Joerg Koenig
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    J.Koenig@adg.de                 (company site)
//    Joerg.Koenig@rhein-neckar.de    (private site)
/////////////////////////////////////////////////////////////////////////////

// last revised $Date: 15.07.98 0:16 $ $Revision: 4 $


#include <stdafx.h>

#include "Registry.h"


//#include "Registry.cpp"		// load class' core functionality


/////////////////////////////////////////////////////////////////////////////
// MFC extensions to the registry classes

BOOL CRegistry :: LoadKey(
							LPCTSTR pszSubKey_p,
							LPCTSTR pszValueName_p,
							CString & strValue_p
						) const {
	CRegVal Data;
	if( LoadKey(pszSubKey_p, pszValueName_p, Data) )
		if( Data.GetValue(strValue_p) )
			return TRUE;
	return FALSE;
}
#ifdef _AFXDLL
BOOL CRegistry :: LoadKey(
							LPCTSTR pszSubKey_p,
							LPCTSTR pszValueName_p,
							CStringArray & strListValue_p
						) const {
	CRegVal Data;
	if( LoadKey(pszSubKey_p, pszValueName_p, Data) )
		if( Data.GetValue(strListValue_p) )
			return TRUE;
	return FALSE;
}


BOOL CRegistry :: SaveKey(
							LPCTSTR pszSubKey_p,
							LPCTSTR pszValueName_p,
							const CStringArray & arrValue_p
						) const {
	CRegVal Data(arrValue_p);
	return SaveKey(pszSubKey_p, pszValueName_p, Data);
}


/////////////////////////////////////////////////////////////////////////////
// CRegVal extensions to use with MFC

CRegVal :: CRegVal( const CStringArray & arrInitial_p )
	: m_hHeap( HeapCreate( 0, 0, 0) )
	, m_pbyteData( 0 )
	, m_dwDataSize( 0 )
	, m_dwType( REG_NONE ) {
	SetValue( arrInitial_p ) ;
}
#endif // _AFXDLL

BOOL CRegVal :: GetValue( CString & rString_p ) const {
	ASSERT( m_dwDataSize > 0 ) ;
	ASSERT( m_pbyteData ) ;

	if(	m_dwType != REG_SZ
		&& m_dwType != REG_EXPAND_SZ
		&& m_dwType != REG_LINK ) {
		TRACE0("CRegVal::GetValue(CString &): wrong type.\n");
		return FALSE ;
	}

	rString_p = LPCTSTR(m_pbyteData);

	return TRUE ;
}

#ifdef _AFXDLL
BOOL CRegVal :: GetValue( CStringArray & rArray_p ) const {
	ASSERT( m_dwDataSize > 0 ) ;
	ASSERT( m_pbyteData ) ;

	if(	m_dwType != REG_MULTI_SZ ) {
		TRACE0("CRegVal::GetValue(CStringArray &): wrong type.\n");
		return FALSE ;
	}

	rArray_p.RemoveAll();

	for( register LPCTSTR p = LPCTSTR(m_pbyteData); *p; p += _tcslen(p)+1 )
		rArray_p.Add( CString(p) );

	return TRUE ;
}


void CRegVal :: SetValue( const CStringArray & rArray_p ) {
	// first step: calculate size ...
	register int i ;
	DWORD nSize = 0 ;
	const int nElem = rArray_p.GetSize();

	for( i = 0 ; i < nElem ; ++i )
		nSize += (rArray_p[i].GetLength() * sizeof(TCHAR)) + sizeof(TCHAR);
	nSize += sizeof(TCHAR);	// for the trailing '\0' !

	AllocateDataBuffer( nSize + (nElem ? 0 : sizeof(TCHAR))) ;
	m_dwType = REG_MULTI_SZ ;

	register LPTSTR p = LPTSTR(m_pbyteData);
	if( ! nElem )
		*p++ = TEXT('\0');

	// last step: copy the strings together
	for( i = 0 ; i < nElem ; ++i ) {
		CString str = rArray_p[i];
		register LPCTSTR c = LPCTSTR(str);
		while( *p++ = *c++ );
	}
	*p = TEXT('\0');
}
#endif // _AFXDLL
