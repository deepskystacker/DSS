/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLFormObjectABC.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "QHTM_Types.h"
#include "HTMLFormObjectABC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTMLFormObjectABC::CHTMLFormObjectABC()
	: CHTMLParagraphObject( CHTMLParagraphObject::knNone )
	, m_pForm( NULL )
	, m_bChecked( false )
	, m_bDisabled( false )
	, m_uObjectID( 0 )
{

}

CHTMLFormObjectABC::~CHTMLFormObjectABC()
{

}
