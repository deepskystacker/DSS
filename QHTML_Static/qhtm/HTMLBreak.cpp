/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLBreak.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"

#ifdef _DEBUG
void CHTMLBreak::Dump() const
{
	TRACENL( _T("Break\n") );
}
#endif	//	_DEBUG


void CHTMLBreak::AddDisplayElements( class CHTMLSectionCreator *psc )
{
	switch( m_clear )
	{
	case knAll:
		psc->SkipLeftMargin();
		psc->SkipRightMargin();
		break;

	case knRight:
		psc->SkipRightMargin();
		break;

	case knLeft:
		psc->SkipLeftMargin();
		break;

	default:
		{
			int nYPos = psc->GetCurrentYPos();
			psc->CarriageReturn( true );
			if( nYPos == psc->GetCurrentYPos() )
			{
				psc->AddYPos( psc->GetDC().GetCurrentFontHeight() );
			}
		}
	}
	
}