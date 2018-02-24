/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLButtonSection.h
Owner:	russf@gipsysoft.com
Purpose:	A FORM button
----------------------------------------------------------------------*/
#ifndef HTMLBUTTONSECTION_H
#define HTMLBUTTONSECTION_H

#ifndef HTMLSECTIONABC_H
	#include "HTMLSectionABC.h"
#endif	//	HTMLSECTIONABC_H

#ifndef MESSAGEREFLECTOR_H
	#include "MessageReflector.h"
#endif	//	MESSAGEREFLECTOR_H

#ifndef FORMDISPLAYELEMENTABC_H
	#include "FormDisplayElementABC.h"
#endif	//	FORMDISPLAYELEMENTABC_H

class CHTMLSectionCreator;
class CHTMLSection;
class CHTMLFormInput;

class CHTMLButtonSection : public CHTMLSectionABC, CMessageReflector, public CFormDisplayElementABC
{
public:
	CHTMLButtonSection( CHTMLSection *pSectParent, GS::FontDef &fdef, CHTMLFormInput *pFormObject, class CHTMLSectionCreator *psc );
	virtual ~CHTMLButtonSection();
	virtual void OnLayout( const WinHelper::CRect &rc );

	void FigureOutSize( CHTMLSectionCreator *psc, WinHelper::CSize &size, int &nBaseline );

private:
	virtual void ResetContent();
	virtual void UpdateFormFromControls();
	virtual LRESULT OnWindowMessage( UINT uMessage, WPARAM wParam, LPARAM lParam );

	virtual void MoveY( int nOffsetY );
	virtual void MoveXY( int nOffsetX, int nOffsetY );
	virtual void OnDraw( GS::CDrawContext &dc );
	virtual void SetFocus( bool bHasFocus );
	virtual bool IsFocused() const;
	virtual void Activate();

	virtual bool IsSameName( LPCTSTR pcszName ) const;
	virtual LPCTSTR GetName() const;
	virtual bool IsSelected() const;
	
	void Check( bool bCheck );
	void CheckIfNamed( LPCTSTR /*pcszName*/, bool /*bCheck*/ );

	GS::FontDef m_fdef;

	CHTMLFormInput *m_pFormObject;

	bool m_bOriginalCheckState;

private:
	CHTMLButtonSection();
	CHTMLButtonSection( const CHTMLButtonSection & );
	CHTMLButtonSection& operator =( const CHTMLButtonSection & );
};


#endif //HTMLBUTTONSECTION_H