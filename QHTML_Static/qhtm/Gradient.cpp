/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Gradient.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <guitools/guitools.h>
#include "Gradient.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGradient::CGradient()
{

}

CGradient::~CGradient()
{

}


bool CGradient::Draw( GS::CDrawContext &dc, const WinHelper::CRect &rc )
{
	bool bRetVal = false;
	if( m_crLeft.IsSet() && m_crRight.IsSet() )
	{
		GS::GradientFillLeftToRight( dc, rc, m_crLeft, m_crRight );
		bRetVal = true;
	}
	else if( m_crTop.IsSet() && m_crBottom.IsSet() )
	{
		GS::GradientFillTopToBottom( dc, rc, m_crTop, m_crBottom );
		bRetVal = true;
	}
	else if( m_crBack.IsSet() )
	{
		dc.FillRect( rc, m_crBack );
		bRetVal = true;
	}
	return bRetVal;
}
