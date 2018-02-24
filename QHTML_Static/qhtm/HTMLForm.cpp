/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLForm.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "FormDisplayElementABC.h"

CHTMLForm::CHTMLForm( )
	: m_strMethod( _T("GET") )
{

}

CHTMLForm::~CHTMLForm()
{
}

#ifdef _DEBUG
void CHTMLForm::Dump() const
{
	TRACENL( _T("Form\n") );
	TRACENL( _T("  Name %s\n"), (LPCTSTR)m_strName );
	TRACENL( _T("  Method %s\n"), (LPCTSTR)m_strMethod );
	TRACENL( _T("  Action %s\n"), (LPCTSTR)m_strAction );
}
#endif	//	_DEBUG


void CHTMLForm::AddItem( CHTMLFormObjectABC *pFormObject )
{
	m_arrFormItems.Add( pFormObject );

	pFormObject->SetForm( this );
}


void CHTMLForm::ResetContent()
{
	for( UINT u = 0; u < m_arrFormDisplayElements.GetSize(); u++ )
	{
		CFormDisplayElementABC *pItem = m_arrFormDisplayElements[ u ];
		pItem->ResetContent();
		pItem->RedrawContent();
	}
}


void CHTMLForm::UpdateFormFromControls()
{
	for( UINT u = 0; u < m_arrFormDisplayElements.GetSize(); u++ )
	{
		m_arrFormDisplayElements[ u ]->UpdateFormFromControls();
	}
}


void CHTMLForm::GetFormFields( Container::CArray< CHTMLFormField > &arrField )
{
	arrField.RemoveAll();

	for( UINT u = 0; u < m_arrFormItems.GetSize(); u++ )
	{
		(void)m_arrFormItems[ u ]->GetFormFields( arrField );
	}
}


void CHTMLForm::UncheckAllNamedInputs( LPCTSTR pcszName )
{
	for( UINT u = 0; u < m_arrFormDisplayElements.GetSize(); u++ )
	{
		m_arrFormDisplayElements[ u ]->CheckIfNamed( pcszName, false );
	}
}
