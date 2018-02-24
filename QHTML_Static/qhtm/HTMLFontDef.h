/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLFontDef.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLFONTDEF_H
#define HTMLFONTDEF_H

#define MAX_FONT_NAMES	128

struct HTMLFontDef
{
	HTMLFontDef()
		: m_nSize( 0 )
		, m_nWeight( FW_NORMAL )
		, m_bUnderline( false )
		, m_bItalic( false )
		, m_bStrike( false )
		, m_nSub( 0 )
		, m_nSup( 0 )
		, m_bFixedPitchFont( false )
		, m_uHash( 0 )
	{
	}
	HTMLFontDef( LPCTSTR pcszFont, int nSize, bool bBold, bool bItalic, bool bUnderline, bool bStrikeout, int nSup, int nSub, bool bFixedPitchFont )
		: m_strFont( pcszFont )
		, m_nSize( nSize )
		, m_nWeight( bBold ? FW_BOLD : FW_NORMAL )
		, m_bUnderline( bUnderline )
		, m_bItalic( bItalic )
		, m_bStrike( bStrikeout )
		, m_nSub( nSub )
		, m_nSup( nSup )
		, m_bFixedPitchFont( bFixedPitchFont )
		, m_uHash( 0 )
	{
	}

	UINT GetHash() const
	{
		if( !m_uHash )
		{
			LPCTSTR p = m_strFont;
			while( *p )
			{
				m_uHash = m_uHash << 1 ^ _totupper( *p++ );
			}
			m_uHash+=(m_bItalic<<1);
			m_uHash+=(m_bUnderline<<2);
			m_uHash+=(m_nSize<<3);
			m_uHash+=(m_nWeight<<4);
			m_uHash+=(m_bStrike<<5);
			m_uHash+=(m_nSup<<6);
			m_uHash+=(m_nSub<<7);		
		}
		return m_uHash;
	}

	StringClass m_strFont;
	int m_nSize;
	int m_nWeight;
	bool m_bUnderline;
	bool m_bItalic;
	bool m_bStrike;
	int m_nSub;
	int m_nSup;
	bool m_bFixedPitchFont;

	mutable UINT m_uHash;
};

namespace Container {
	inline BOOL ElementsTheSame( const HTMLFontDef &n1, const HTMLFontDef &n2 )
	{
		//
		//	Don't optimise this unless you have empirical evidence to proove that you can
		//	increase it' speed.
		return n1.m_bItalic == n2.m_bItalic
			&& n1.m_nSize == n2.m_nSize
			&& n1.m_nWeight == n2.m_nWeight
			&& n1.m_bUnderline == n2.m_bUnderline
			&& n1.m_bStrike == n2.m_bStrike
			&& n1.m_nSub == n2.m_nSub
			&& n1.m_nSup == n2.m_nSup
			&& !_tcscmp( n1.m_strFont, n2.m_strFont );
	}

	inline UINT HashIt( const HTMLFontDef &keyConst )
	{
		return keyConst.GetHash();
	}
}


#endif //HTMLFONTDEF_H