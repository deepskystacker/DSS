/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLEditSection.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLEDITSECTION_H
#define HTMLEDITSECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef FORMDISPLAYELEMENTABC_H
	#include "FormDisplayElementABC.h"
#endif	//	FORMDISPLAYELEMENTABC_H

class CHTMLSectionCreator;
class CHTMLSection;
class CHTMLFormInput;

class CHTMLEditSection : public CHTMLSectionABC, public CFormDisplayElementABC
{
public:
	CHTMLEditSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLFormInput *pFormObject, class CHTMLSectionCreator *psc );
	virtual ~CHTMLEditSection();

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

	CHTMLFormInput *m_pFormObject;

	const StringClass m_strOriginalSelection;

	GS::FontDef m_fdef;

private:
	CHTMLEditSection();
	CHTMLEditSection( const CHTMLEditSection & );
	CHTMLEditSection& operator =( const CHTMLEditSection & );
};


#endif //HTMLEDITSECTION_H