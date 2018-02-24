/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLFormInput.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLFORMINPUT_H
#define HTMLFORMINPUT_H

#ifndef HTMLFORMOBJECTABC_H
	#include "HTMLFormObjectABC.h"
#endif //HTMLFORMOBJECTABC_H

class CHTMLFormInput : public CHTMLFormObjectABC
//
//	Text block.
//	Has some text.
{
public:
	explicit CHTMLFormInput( const HTMLFontDef * pFont );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG
	virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	StringClass m_strValue;
	StringClass m_strID;

	int m_nCols;
	UINT m_uRows;
	UINT m_uMaxLength;
	bool m_bReadonly;
	StringClass m_strImageSrc;
	const HTMLFontDef * m_pFont;

	enum FormInputType { knNone, knText, knPassword, knCheckbox, knRadio, knSubmit, knReset, knFile, knHidden, knImage, knButton, knTextArea };
	void SetFormType( const CStaticString &strType );
	FormInputType	GetFormType() const { return m_nType; }

	bool GetFormFields( Container::CArray< CHTMLFormField > &arrField );


protected:
	void OnAddButton( class CHTMLSectionCreator *psc );

	void OnAddEdit( class CHTMLSectionCreator *psc );

	FormInputType	m_nType;

private:
	CHTMLFormInput();
	CHTMLFormInput( const CHTMLFormInput &);
	CHTMLFormInput& operator =( const CHTMLFormInput &);
};

#endif //HTMLFORMINPUT_H