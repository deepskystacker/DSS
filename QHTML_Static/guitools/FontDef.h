/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	FontDef.h
Owner:	russf@gipsysoft.com
Purpose:	Font desciptor used throughout.
----------------------------------------------------------------------*/
#ifndef FONTDEF_H
#define FONTDEF_H

#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
	#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif	//	_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES

#ifndef WINHELPER_H
	#include <reuse/winhelper.h>
#endif	//	WINHELPER_H

#define MAX_FONT_NAMES	128
namespace GS
{

	struct FontDef
	{
		FontDef() {};
		FontDef( LPCTSTR pcszFont, int nSize, int nWeight, bool bItalic, bool bUnderline, bool bStrikeout, BYTE cCharSet, bool bFixedPitchFont )
			: m_nSizePixels( nSize )
			, m_nWeight( nWeight )
			, m_bUnderline( bUnderline )
			, m_bItalic( bItalic )
			, m_bStrike( bStrikeout )
			, m_cCharSet( cCharSet )
			, m_bFixedPitchFont( bFixedPitchFont )
			, m_nHasValue( 0 )
			, m_nRef( 0 )
		{
			_tcsncpy( m_szFontName, pcszFont, countof( m_szFontName ) );

			LPCTSTR p = m_szFontName;
			m_nHasValue = 0;
			while( *p )
			{
				m_nHasValue = m_nHasValue << 1 ^ _totupper( *p++ );
			}
			m_nHasValue+=(m_bItalic<<1);
			m_nHasValue+=(m_bUnderline<<2);
			m_nHasValue+=(m_nSizePixels<<3);
			m_nHasValue+=(m_nWeight<<4);
			m_nHasValue+=(m_bStrike<<5);
			m_nHasValue+=(m_cCharSet<<6);
			m_nHasValue+=(m_bFixedPitchFont<<7);		
		}

		void AddRef() const
		{
			m_nRef++;
		}

		void Release()
		{
			m_nRef--;
			if( !m_nRef )
			{
				delete this;
			}
		}

		TCHAR m_szFontName[ MAX_FONT_NAMES ];
		int m_nSizePixels;
		int m_nWeight;
		bool m_bUnderline;
		bool m_bItalic;
		bool m_bStrike;
		BYTE m_cCharSet;
		bool m_bFixedPitchFont;
		UINT m_nHasValue;
		mutable int m_nRef;
	};

};

#endif //FONTDEF_H