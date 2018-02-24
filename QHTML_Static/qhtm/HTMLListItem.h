/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLListItem.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLLISTITEM_H
#define HTMLLISTITEM_H

class CHTMLListItem : public CHTMLDocument
//
//	List Item
//	Is a document.
{
public:
	CHTMLListItem( CDefaults *pDefaults, bool bBullet, LPCTSTR pcszFont, int nSize, bool bBold, bool bItalic, bool bUnderline, bool bStrikeout, CColor crFore, BYTE cCharSet );

	bool IsEmpty() const;

	//
	//	Set this items index value
	void SetValue( UINT uValue );
	UINT GetValue( UINT uDefaultValue ) const { if( m_bHasValue ) return m_uValue; return uDefaultValue; }
	
	StringClass m_strFont;
	int m_nSize;
	bool m_bBold;
	bool m_bItalic;
	bool m_bUnderline;
	bool m_bStrikeout;
	CColor m_crFore;

	bool m_bBullet;
	
#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:	
	bool m_bHasValue;
	UINT m_uValue;
private:
	CHTMLListItem();
	CHTMLListItem( const CHTMLListItem &);
	CHTMLListItem& operator =( const CHTMLListItem &);
};

#endif //HTMLLISTITEM_H