/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	AquireImage.cpp
Owner:	russf@gipsysoft.com
Purpose:	Function for handling default image acquisition
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM.h"
#include "QHTMImage.h"
#include "AquireImage.h"
#include "QHTM_Types.h"
#include "stdlib.h"
#include <datasource/ResourceDataSource.h>
#include <datasource/FileDataSource.h>

#ifdef QHTM_BUILD_INTERNAL_IMAGING
static CImage * LoadFromResource( HINSTANCE hInstance, LPCTSTR pcszResourceName )
{
	CImage *				pResult = NULL;
	CImage *				pImage = new CImage;
	bool					bResult = false;
	CResourceDataSource		ds;

	// Look for brackets in the Name
	LPCTSTR					szResource = pcszResourceName;
	LONG					lImage = -1;

	TCHAR					szResourceName[2000];
	LPCTSTR					szOpen = NULL;
	LPCTSTR					szClose = NULL;
	LPCTSTR					szSharp = NULL;
	LONG					lNumeral = 0;

	if (szResource)
	{
		// First character is # means that it is a numeric value (MAKEINTRESOURCE)
		szSharp = _tcschr(szResource, _T('#'));
		if (szSharp && (szSharp != szResource))
			szSharp = NULL;

		szOpen = _tcschr(szResource, _T('['));
		szClose = _tcschr(szResource, _T(']'));
	};

	if (szOpen && szClose && (szOpen < szClose))
	{
		// Extract the image number from within the bracket
		TCHAR				szImage[100];

		memset(szImage, 0, sizeof(szImage));
		memset(szResourceName, 0, sizeof(szResourceName));
		_tcsncpy(szImage, szOpen+1, (szClose-szOpen-1));
		lImage = _tstol(szImage);
		_tcsncpy(szResourceName, pcszResourceName, (szOpen-pcszResourceName));
		szResource = szResourceName;
		if (szSharp)
		{
			szResource++;
			lNumeral = _tstol(szResource);
			szResource = MAKEINTRESOURCE(lNumeral);
		};
	}
	else if (szSharp)
	{
		szResource++;
		lNumeral = _tstol(szResource);
		szResource = MAKEINTRESOURCE(lNumeral);
	};

	//
	//	We try icon group first because it's one of the top level items in the resources
	if (ds.Open( hInstance, szResource, RT_GROUP_ICON ))
	{
		(void)ds.Reset();
		bResult = pImage->Load( ds );
	}
	else if( ds.Open( hInstance, szResource, _T("PNG") ) )
	{
		bResult = pImage->Load( ds );
	}
	else if( ds.Open( hInstance, szResource, RT_RCDATA ) )
	{
		bResult =  pImage->Load( ds );
	}
	else if (ds.Open( hInstance, szResource, RT_BITMAP ))
	{
		(void)ds.Reset();
		bResult = pImage->Load( ds );
	}
	else if (ds.Open( hInstance, szResource, RT_ICON ))
	{
		(void)ds.Reset();
		bResult = pImage->Load( ds );
	}

	if (bResult && pImage && (lImage>=0))
	{
		SIZE				sz;
		LONG				lNrImages;

		sz = pImage->GetSize();

		lNrImages = sz.cx/max(1, sz.cy);
		if (lImage < lNrImages)
			pResult = pImage->CreateSubImage(0, lImage * sz.cy, 0, sz.cy, sz.cy);
	}
	else if (bResult)
	{
		pResult = pImage;
		pImage = NULL;
	};

	if (pImage)
		delete pImage;

	return pResult;
}


//
//	Simple class used so I don't need to remember to call FreeLibrary in several places.
class CAutoModule
{
public:
	CAutoModule() : m_hModule( NULL ) {}
	~CAutoModule()	{ if( m_hModule ) VAPI( ::FreeLibrary( m_hModule ) ); m_hModule = NULL;}

	operator HMODULE () { return m_hModule; }
	CAutoModule & operator = ( HMODULE hModule ) { m_hModule = hModule; return *this; }

private:
	CAutoModule( const CAutoModule & );
	CAutoModule & operator = ( const CAutoModule & );

	HMODULE m_hModule;
};


CImage *AquireImage( HINSTANCE hInstance, LPCTSTR pcszFilePath, LPCTSTR pcszFilename, bool bIsTransparent, COLORREF crForceTransparent )
{
	CImage *pImage = NULL;

	//
	//	Check for resource based images
	if( !_tcsnicmp( pcszFilename, _T("RES:"), 4 ) )
	{
		pcszFilename += 4;
		ASSERT( _tcslen( pcszFilename  ) );
		pImage = LoadFromResource( hInstance, pcszFilename );
		if( pImage )
		{
			if( bIsTransparent ) 
				pImage->ForceTransparent( crForceTransparent );
			return pImage;
		}

		LPTSTR endptr;

		pImage = LoadFromResource( hInstance, MAKEINTRESOURCE( _tcstol( pcszFilename, &endptr, 10 ) ) );
		if( pImage )
		{
			if( bIsTransparent ) 
				pImage->ForceTransparent( crForceTransparent );
			return pImage;
		}


		//
		//	Here we attempt to read the resource given the name RES:exeORdll:number
		//	However, despite out valiant efforts it has not worked, at least not for icons

		CAutoModule amOther;
		LPTSTR pszNumber = const_cast< LPTSTR >( _tcsrchr( pcszFilename, ':' ) );
		if( pszNumber )
		{
			*pszNumber = '\000';
			amOther = LoadLibraryEx( pcszFilename, NULL, LOAD_LIBRARY_AS_DATAFILE );
			pcszFilename = pszNumber + 1;
		}
		else
		{
			amOther = LoadLibraryEx( pcszFilename, NULL, LOAD_LIBRARY_AS_DATAFILE );
		}
		
		if( amOther )
		{
			pImage = LoadFromResource( amOther, pcszFilename );

			if( pImage )
			{
				if( bIsTransparent ) 
					pImage->ForceTransparent( crForceTransparent );
				return pImage;
			}

			pImage = LoadFromResource( amOther, MAKEINTRESOURCE( _tcstol( pcszFilename, &endptr, 10 ) ) );

			if( pImage )
			{
				if( bIsTransparent ) 
					pImage->ForceTransparent( crForceTransparent );
				return pImage;
			}
		}
		if (pImage)
			pImage->Destroy();

		return NULL;
	}

	//
	//	Check for file based images
	StringClass strImageName( pcszFilePath );
	strImageName += pcszFilename ;

	CFileDataSource file;
	if( file.Open( pcszFilename ) || file.Open( strImageName ) )
	{
		pImage = new CQHTMImage;

		if( pImage->Load( file ) )
		{
			if( bIsTransparent ) 
				pImage->ForceTransparent( crForceTransparent );
			return pImage;
		}
	}

	pImage->Destroy();
	return NULL;
}

#endif	//	QHTM_BUILD_INTERNAL_IMAGING