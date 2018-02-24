/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SimpleString.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "DebugHlp/DebugHlp.h"
#include "SimpleString.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSimpleString & CSimpleString::operator = ( const CSimpleString &rhs )
{
	if( &rhs != this )
	{
		const size_t uLength = rhs.GetLength();
		if( uLength )
		{
			m_arrText.SetSize( uLength + 1 );
			_tcscpy( m_arrText.GetData(), rhs );
			m_arrText[ uLength ] = '\000';
		}
		else
		{
			Empty();
		}
		
	}
	return *this;
}

CSimpleString::CSimpleString( const CSimpleString &rhs )
{
	operator = ( rhs );
}


CSimpleString::CSimpleString( LPCTSTR pcszText, size_t uLength )
{
	m_arrText.SetSize( uLength + 1 );
	memcpy( m_arrText.GetData(), pcszText, uLength * sizeof( TCHAR ) );
	m_arrText[ uLength ] = '\000';
}


CSimpleString::CSimpleString( LPCTSTR pcszText )
{
	if( pcszText )
	{
		const size_t uLength = static_cast<UINT>( _tcslen( pcszText ) );
		m_arrText.SetSize( uLength + 1 );
		memcpy( m_arrText.GetData(), pcszText, uLength * sizeof( TCHAR ) );
	}
}

CSimpleString::CSimpleString()
{

}


CSimpleString::~CSimpleString()
{

}

size_t CSimpleString::GetLength() const
{
	if( m_arrText.GetSize() )
		return m_arrText.GetSize() - 1;
	return 0;
}


LPCTSTR CSimpleString::GetData() const
{
	return m_arrText.GetData();
}


void CSimpleString::Empty()
{
	m_arrText.SetSize( 0 );	
}


CSimpleString & CSimpleString::operator += ( const CSimpleString &rhs )
{
	const size_t u = GetLength();
	const size_t urhs = rhs.GetLength();
	m_arrText.SetSize( GetLength() + rhs.GetLength() + 1 );
	memcpy( m_arrText.GetData() + u, rhs.GetData(), urhs * sizeof( TCHAR ) );
	return *this;
}


CSimpleString & CSimpleString::operator += ( LPCTSTR pcszText )
{
	if( pcszText )
	{
		const size_t uLength = _tcslen( pcszText );
		m_arrText.SetSize( GetLength() + uLength + 1 );
		_tcscat( m_arrText.GetData(), pcszText );
	}
	return *this;
}


void CSimpleString::Add( LPCTSTR pcszText, size_t uLength )
{
	const size_t u = GetLength();
	m_arrText.SetSize( GetLength() + uLength + 1 );
	memcpy( m_arrText.GetData() + u, pcszText, uLength * sizeof( TCHAR ) );
}

void CSimpleString::Set( LPCTSTR pcszText, size_t uLength )
{
	m_arrText.SetSize( uLength + 1 );
	memcpy( m_arrText.GetData(), pcszText, uLength * sizeof( TCHAR ) );
	m_arrText[ uLength ] = '\000';
}



void CSimpleString::Delete( size_t uIndex, size_t uCount )
{
	m_arrText.RemoveAt( uIndex, uCount );
}

int CSimpleString::Compare( const CTextABC &txt ) const
{
	return _tcscmp( m_arrText.GetData(), txt );
}

int CSimpleString::CompareNoCase( const CTextABC &txt ) const
{
	return _tcsicmp( m_arrText.GetData(), txt );
}


void CSimpleString::TrimBoth()
{
	LPCTSTR pcszText = m_arrText.GetData();
	size_t uLength = m_arrText.GetSize() - 1;

	while( isspace( *pcszText ) )
	{
		pcszText++;
		uLength--;
	}

	while( isspace( *(pcszText + uLength) ) )
	{
		uLength--;
	}
	
	memcpy( m_arrText.GetData(), pcszText, uLength * sizeof( TCHAR ) );
	m_arrText.SetSize( uLength + 1 );

	m_arrText[ uLength ] = '\000';
}


void CSimpleString::Replace( UINT uFind, UINT uReplace )
{
	const size_t uLength = m_arrText.GetSize();
	for( size_t u = 0; u < uLength; u++ )
	{
		if( (UINT)m_arrText[ u ] == uFind )
		{
			m_arrText[ u ] = (TCHAR)uReplace;
		}
	}
}