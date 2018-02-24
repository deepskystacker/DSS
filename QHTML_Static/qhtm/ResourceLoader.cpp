/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	ResourceLoader.cpp
Owner:	russf@gipsysoft.com
Purpose:	class to load resources of any type.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ResourceLoader.h"


CResourceLoader::CResourceLoader( HINSTANCE hInst )
	: m_pData( NULL )
	, m_nSize( 0 )
	, m_hInst( hInst )
{

}

CResourceLoader::~CResourceLoader()
{

}


bool CResourceLoader::Load( LPCTSTR pcszName, LPCTSTR pcszType )
{
	HRSRC hRes = FindResource( m_hInst, pcszName, pcszType );
	if( !hRes )
		return false;

	HGLOBAL hGlobal = LoadResource( m_hInst, hRes );
	if( !hGlobal ) 
		return false;

	m_pData = LockResource( hGlobal );
	if( !m_pData )
		return false;

	m_nSize = SizeofResource( m_hInst, hRes );

	return true;
}

