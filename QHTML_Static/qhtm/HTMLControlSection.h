/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLControlSection.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLCONTROLSECTION_H
#define HTMLCONTROLSECTION_H

#ifdef QHTM_WINDOWS_CONTROL

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef FORMDISPLAYELEMENTABC_H
	#include "FormDisplayElementABC.h"
#endif	//	FORMDISPLAYELEMENTABC_H

class CHTMLSectionCreator;
class CHTMLSection;
class CHTMLFormInput;

class CHTMLControlSection : public CHTMLSectionABC, public CFormDisplayElementABC
{
public:
	CHTMLControlSection( CHTMLSection *pSectParent, GS::FontDef &fdef, const StringClass &strClass, UINT uStyle, UINT uStyleEx, UINT uWidth, UINT uHeight );
	virtual ~CHTMLControlSection();

	void CHTMLControlSection::SetFont( CHTMLSectionCreator *psc );

	virtual void OnLayout( const WinHelper::CRect &rc );
	virtual void MoveY( int nOffsetY );
	virtual void MoveXY( int nOffsetX, int nOffsetY );
	virtual void OnDraw( GS::CDrawContext &dc );

	virtual void SetFocus( bool bHasFocus );
	virtual bool IsFocused() const;

private:
	virtual void ResetContent();
	virtual void UpdateFormFromControls();

	GS::FontDef m_fdef;
private:
	CHTMLControlSection();
	CHTMLControlSection( const CHTMLControlSection & );
	CHTMLControlSection& operator =( const CHTMLControlSection & );
};

#endif	//	QHTM_WINDOWS_CONTROL

#endif //HTMLCONTROLSECTION_H