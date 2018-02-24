/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLTextArea.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "defaults.h"

CHTMLTextArea::CHTMLTextArea( const HTMLFontDef * pFont )
	: CHTMLFormInput( pFont )
{
	m_nType = knTextArea;
	m_nCols = 20;
	m_uRows = 2;
}

#ifdef _DEBUG
void CHTMLTextArea::Dump() const
{
	TRACENL( _T("TextArea\n") );
}
#endif	//	_DEBUG
