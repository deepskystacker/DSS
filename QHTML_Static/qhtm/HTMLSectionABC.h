/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.

File:	HTMLSectionABC.h
Owner:	russf@gipsysoft.com
Purpose:	Base class used for the HTML display sections.
----------------------------------------------------------------------*/
#ifndef HTMLSECTIONABC_H
#define HTMLSECTIONABC_H

#ifndef HTMLSECTION_H
	#include "HTMLSection.h"
#endif	//	HTMLSECTION_H

#ifndef FOCUSCONTROL_H
	#include "FocusControl.h"
#endif //FOCUSCONTROL_H

//
//	Uncommenting this will force all HTML components to draw a red
//	rectangle around themselves.
//	This helps a lot when debugging drawing problems.
//#define DRAW_DEBUG

class CHTMLSectionLink;
class CHTMLSection;

class CHTMLSectionABC : public CSectionABC, public CFocusControlABC
{
public:
	explicit CHTMLSectionABC( CHTMLSection *psectParent );
	virtual ~CHTMLSectionABC();

	virtual StringClass GetTipText() const;
	virtual void OnMouseLeftUp( const WinHelper::CPoint &pt );
	virtual void OnMouseLeftDown( const WinHelper::CPoint &pt );
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

#ifdef QHTM_ALLOW_FOCUS
	virtual void OnDraw( GS::CDrawContext &dc );
#endif	//	QHTM_ALLOW_FOCUS

	//	Set this section as a link, either a link target or a name
	void SetAsLink(CHTMLSectionLink*);

	//	Switch on/off the ability for the HTML to display tooltips.
	static void EnableTooltips( bool bEnable );
	static bool IsTooltipsEnabled();

	void SetActivationTarget( const StringClass &strActivationTarget )
		{ m_strActivationTarget = strActivationTarget; }

	COLORREF LinkColour(); 
	COLORREF LinkHoverColour();


protected:
	bool inline IsLink() const { return m_pHtmlLink != NULL; }

	virtual void SetFocus( bool bHasFocus );
	virtual void Update() { ForceRedraw(); }
	virtual bool IsFocused() const;
	virtual void GetObjectRect( WinHelper::CRect &rcBounds ) const;
	virtual void Activate();

	CHTMLSection *m_psectHTMLParent;

private:
	CHTMLSectionLink*	m_pHtmlLink;
	StringClass m_strActivationTarget;


private:
	CHTMLSectionABC();
	CHTMLSectionABC( const CHTMLSectionABC & );
	CHTMLSectionABC &operator =( const CHTMLSectionABC & );

	friend class CHTMLSectionCreator;
};

#endif //HTMLSECTIONABC_H
