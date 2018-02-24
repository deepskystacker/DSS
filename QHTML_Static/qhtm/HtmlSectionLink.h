/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLSectionLink.h
Owner:	russf@gipsysoft.com
Author: rich@woodbridgeinternalmed.com
Purpose:	Hyperlink 'link' object, links all of teh hyperlink sections
					together. Ensure they all highlight at the same time if the
					hyperlink is made up of multiple sections etc.
----------------------------------------------------------------------*/
#ifndef HTMLSECTIONLINK_H
#define HTMLSECTIONLINK_H

#ifndef SECTIONABC_H
	#include "sectionABC.h"
#endif	// SECTIONABC_H

#ifndef CFOCUSCONTROL_H
	#include "FocusControl.h"
#endif //FOCUSCONTROL_H

class CHTMLSection;

//	This struct is used to store link information for a section
class CHTMLSectionLink: public CFocusControlABC
{
public:
	CHTMLSectionLink(LPCTSTR pcszLinkTarget, LPCTSTR pcszLinkTitle, LPCTSTR pcszID, COLORREF crLink, COLORREF crHover, bool bTitleSet, CHTMLSection *pParent )
		: m_strLinkTarget( pcszLinkTarget )
		, m_strLinkTitle( pcszLinkTitle )
		, m_strLinkID( pcszID )
		, m_crLink( crLink )
		, m_crHover( crHover )
		, m_bFocused( false )
		, m_pParent( pParent )
		, m_bTitleSet( bTitleSet )
	{}

	StringClass m_strLinkTarget;
	bool m_bTitleSet;
	StringClass m_strLinkTitle;
	StringClass m_strLinkID;
	COLORREF	m_crLink;
	COLORREF	m_crHover;
	ArrayClass<CSectionABC*> m_arrSections;

	void OnMouseEnter();
	void OnMouseLeave();
	void AddSection(CSectionABC* psect) { m_arrSections.Add( psect ); }

	void DrawFocus( GS::CDrawContext &dc );
	
	virtual void SetFocus( bool bHasFocus );
	virtual void Activate();
	virtual void Update();
	virtual bool IsFocused() const { return m_bFocused; }
	virtual void GetObjectRect( WinHelper::CRect &rcBounds ) const;

	void GetPointDistance( const POINT &pt, int &nX, int &nY ) const;

	bool GetRegion( HRGN &rgn ) const;

	virtual bool IsSameName( LPCTSTR ) const { return false; }

private:

	bool m_bFocused;

	CHTMLSection *m_pParent;
};


#endif
