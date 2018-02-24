/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	StaticString.h
Owner:	russf@gipsysoft.com
Purpose:	String class tied to some static text, so it can be used when
					passed around as a TextABC.
----------------------------------------------------------------------*/
#ifndef STATICSTRING_H
#define STATICSTRING_H

#ifndef TEXTABC_H
#include <reuse/TextABC.h>
#endif	//	TEXTABC_H

class CStaticString: public CTextABC
{
public:
	CStaticString()
		: m_pcszText( NULL )
		, m_uLength( 0 )
	{
	}
	/*lint -e1931 */
	CStaticString( LPCTSTR pcszText )
		: m_pcszText( pcszText )
		, m_uLength( _tcslen( pcszText ) )
	{
	}
	/*lint +e1931 */
	CStaticString( LPCTSTR pcszText, size_t uLength )
		: m_pcszText( pcszText )
		, m_uLength( uLength )
	{
	}
	CStaticString( const CStaticString &str )
		: m_pcszText( str.m_pcszText )
		, m_uLength( str.m_uLength )
	{
	}

	void Set( LPCTSTR pcszText, size_t uLength )
	{
		m_pcszText = pcszText;
		m_uLength = uLength;
	}

	LPCTSTR Find( TCHAR ch ) const
	{
		LPCTSTR pcszEnd = GetEndPointer();
		LPCTSTR p = GetData();
		while( p < pcszEnd )
		{
			if( *p == ch )
				return p;
			p++;
		}
		return NULL;
	}

	void TrimBoth() const
	{
		if( m_pcszText )
		{
			while( isspace( *m_pcszText ) )
			{
				m_pcszText++;
				m_uLength--;
			}
			while( isspace( *(m_pcszText + m_uLength) ) )
			{
				m_uLength--;
			}
		}
	}


	size_t GetLength() const
	{
		return m_uLength;
	}

	LPCTSTR GetData() const
	{
		return m_pcszText;
	}

	/*lint -e613 */
	LPCTSTR GetEndPointer() const
	{
		return m_pcszText + m_uLength;
	}
	/*lint -e613 */

private:
	mutable LPCTSTR m_pcszText;
	mutable size_t m_uLength;
};


namespace Container
{
	inline UINT HashIt( const CStaticString& s)
	{
		size_t uHash = 0;
		LPCTSTR pcszText = s.GetData();
		size_t uLength = s.GetLength();
		if (!uLength)
			return 0;
		while( uLength-- )
		{
			uHash = uHash << 1 ^ toupper( *pcszText++ );
		}
		return static_cast< UINT >( uHash );
	}

	inline bool ElementsTheSame( const CStaticString& lhs, const CStaticString& rhs )
	{
		return rhs.GetLength() == lhs.GetLength() && !_tcsnicmp( lhs.GetData(), rhs.GetData(), lhs.GetLength() );
	}

}


#endif //STATICSTRING_H