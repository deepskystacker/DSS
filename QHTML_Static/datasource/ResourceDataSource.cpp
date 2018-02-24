/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ResourceDataSource.cpp
Owner:	russf@gipsysoft.com
Purpose:	A data source based on a Windows resource.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ResourceDataSource.h"

CResourceDataSource::CResourceDataSource()
	: m_hInst( NULL )
	, m_hRsrc( NULL )
	, m_hGlobal( NULL )
{
}


bool CResourceDataSource::Open( HINSTANCE hInst, LPCTSTR pcszName, LPCTSTR pcszResourceType )
{
	m_hInst = hInst;

	m_hRsrc = FindResource( hInst, pcszName, pcszResourceType );

	if (!m_hRsrc)
	{
		//TRACE( _T("FindResource Failed (Code %i).\n"), GetLastError());
		return false;
	}

	m_hGlobal = LoadResource (m_hInst, m_hRsrc);
	if (!m_hGlobal)
	{
		//TRACE( _T("LoadResource Failed (Code %i).\n"), GetLastError());
		return false;
	}

	BYTE *pData = (BYTE *)LockResource (m_hGlobal);
	if( !pData )
	{
		//TRACE( _T("LockResource failed.\n") );
		return false;
	}

	return CBufferDataSource::Open( pData, ::SizeofResource( m_hInst, m_hRsrc ) );
}


CResourceDataSource::~CResourceDataSource()
{
#ifndef UNDER_CE
	UnlockResource(m_hGlobal);
	FreeResource(m_hGlobal);
#endif	//	UNDER_CE
	m_hGlobal = NULL;
}
