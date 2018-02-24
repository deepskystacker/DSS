/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLFormObjectABC.h
Owner:	russf@gipsysoft.com
Purpose:

	Base class for form handling

----------------------------------------------------------------------*/
#ifndef HTMLFORMOBJECTABC_H
#define HTMLFORMOBJECTABC_H

#ifndef HTMLPARAGRAPHOBJECT_H
	#include "HTMLParagraphObject.h"
#endif //HTMLPARAGRAPHOBJECT_H

#ifndef HTMLFORM_H
	#include "HTMLFormField.h"
#endif	//	HTMLFORM_H

class CHTMLForm;

class CHTMLFormObjectABC : public CHTMLParagraphObject
{
public:
	CHTMLFormObjectABC();
	virtual ~CHTMLFormObjectABC();

	void SetForm( CHTMLForm *pForm ) { m_pForm = pForm; }

	virtual bool GetFormFields( Container::CArray< CHTMLFormField > &arrField ) = 0;
	

	UINT m_uObjectID;
	StringClass m_strName;
	bool m_bChecked;
	bool m_bDisabled;

	CHTMLForm *m_pForm;

private:
	CHTMLFormObjectABC( const CHTMLFormObjectABC &);
	CHTMLFormObjectABC& operator =( const CHTMLFormObjectABC &);
};

#endif //HTMLFORMOBJECTABC_H