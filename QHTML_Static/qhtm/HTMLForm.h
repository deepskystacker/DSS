/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLForm.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef HTMLFORM_H
#define HTMLFORM_H

class CHTMLFormObjectABC;
class CFormDisplayElementABC;

#ifndef HTMLPARAGRAPH_H
	#include "HTMLParagraph.h"
#endif	//	HTMLPARAGRAPH_H

#ifndef HTMLFORMFIELD_H
	#include "HTMLFormField.h"
#endif	//	HTMLFORMFIELD_H

class CHTMLForm
{
public:
	CHTMLForm();
	virtual ~CHTMLForm();

	void AddItem( CHTMLFormObjectABC *pFormInput );

	//virtual void AddDisplayElements( class CHTMLSectionCreator *psc );

	void ResetContent();
	void UpdateFormFromControls();

	StringClass m_strAction;
	StringClass m_strMethod;
	StringClass m_strName;

	void AddDisplayElement( CFormDisplayElementABC *pSect )
	{
		m_arrFormDisplayElements.Add( pSect );
	}
		

	Container::CArray< CFormDisplayElementABC *> m_arrFormDisplayElements;

	void GetFormFields( Container::CArray< CHTMLFormField > &arrField );
	void UncheckAllNamedInputs( LPCTSTR pcszName );

#ifdef _DEBUG
	virtual void Dump() const;
#endif	//	_DEBUG

private:
	Container::CArray< CHTMLFormObjectABC * > m_arrFormItems;

	CHTMLForm( const CHTMLForm &);
	CHTMLForm& operator =( const CHTMLForm &);
};

#endif //HTMLFORM_H