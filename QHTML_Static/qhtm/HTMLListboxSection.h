/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLListboxSection.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLLISTBOXSECTION_H
#define HTMLLISTBOXSECTION_H


#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef FORMDISPLAYELEMENTABC_H
	#include "FormDisplayElementABC.h"
#endif	//	FORMDISPLAYELEMENTABC_H

class CHTMLSectionCreator;
class CHTMLSection;
class CHTMLSelect;

class CHTMLListboxSection : public CHTMLSectionABC, public CFormDisplayElementABC
{
public:
	CHTMLListboxSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLSelect *pFormObject, class CHTMLSectionCreator *psc );
	virtual ~CHTMLListboxSection();

	void FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size, int &nBaseline );
	virtual void OnLayout( const WinHelper::CRect &rc );
	virtual void MoveY( int nOffsetY );
	virtual void MoveXY( int nOffsetX, int nOffsetY );
	virtual void OnDraw( GS::CDrawContext &dc );

	virtual void SetFocus( bool bHasFocus );
	virtual bool IsFocused() const;


private:
	virtual void ResetContent();
	virtual void UpdateFormFromControls();

	CHTMLSelect *m_pFormObject;

	GS::FontDef m_fdef;

private:
	CHTMLListboxSection();
	CHTMLListboxSection( const CHTMLListboxSection & );
	CHTMLListboxSection& operator =( const CHTMLListboxSection & );
};


#endif //HTMLLISTBOXSECTION_H