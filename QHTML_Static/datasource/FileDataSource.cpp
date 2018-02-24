/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FileDataSource.cpp
Owner:	russf@gipsysoft.com
Purpose:	Data source object that is based upon a file.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "FileDataSource.h"


CFileDataSource::CFileDataSource()
	: m_pFile( NULL )
{

}

CFileDataSource::~CFileDataSource()
{
	if( m_pFile )
		fclose( m_pFile );
}


bool CFileDataSource::Open( LPCTSTR pcszFilename )
{
	if( pcszFilename && *pcszFilename )
	{
		ASSERT_VALID_STR( pcszFilename );
		m_pFile = _tfopen( pcszFilename, _T("rb") );
		if( m_pFile )
			return true;
	}
	//TRACE( _T("CFileDataSource::Open failed to open file: %s\n"), pcszFilename );
	return false;
}


bool CFileDataSource::ReadBytes( BYTE *pbBuffer, UINT uCount )
{
	if( m_pFile && fread( pbBuffer, uCount, 1, m_pFile ) == 1 )
	{
		return true;
	}
	//TRACE( _T("CFileDataSource::ReadBytes failed to read required size with just %d bytes remaining\n"), GetSize() - GetCurrentPos() );
	return false;
}


bool CFileDataSource::SetRelativePos( int n )
{
	if( m_pFile && fseek( m_pFile, n, SEEK_CUR ) )
	{
		return true;
	}
	return false;
}


bool CFileDataSource::Reset()
{
	if( m_pFile && fseek( m_pFile, 0, SEEK_SET ) )
	{
		return true;
	}
	return false;
}


int CFileDataSource::GetSize() const
{
	fpos_t posSave;
	if( fgetpos( m_pFile, &posSave ) != 0 )
		return 0;

	if( fseek( m_pFile, 0, SEEK_END ) != 0 )
		return 0;

	fpos_t posEnd;
	if( fgetpos( m_pFile, &posEnd ) != 0 )
		return 0;

	if( fsetpos( m_pFile, &posSave ) != 0 )
		return 0;

	return static_cast<UINT>( posEnd );
}

int CFileDataSource::GetCurrentPos() const
{
	return ftell( m_pFile );
}
