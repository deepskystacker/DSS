/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLComboSection.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLCOMBOSECTION_H
#define HTMLCOMBOSECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef FORMDISPLAYELEMENTABC_H
	#include "FormDisplayElementABC.h"
#endif	//	FORMDISPLAYELEMENTABC_H

class CHTMLSectionCreator;
class CHTMLSection;
class CHTMLSelect;

class CHTMLComboSection : public CHTMLSectionABC, public CFormDisplayElementABC
{
public:
	CHTMLComboSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLSelect *pFormObject, class CHTMLSectionCreator *psc );
	virtual ~CHTMLComboSection();

	void FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size );
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

	size_t m_uComboHeight;
	UINT m_nOriginalSelection;

private:
	CHTMLComboSection();
	CHTMLComboSection( const CHTMLComboSection & );
	CHTMLComboSection& operator =( const CHTMLComboSection & );
};


#endif //HTMLCOMBOSECTION_H