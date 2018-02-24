/*----------------------------------------------------------------------
Copyright (c) 1998,1999 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Frame.cpp
Owner:	russf@gipsysoft.com
Purpose:	A single frame.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "ImgLibInt.h"
//#include <ImgLib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrame::CFrame( GS::CDIB *pDib, int nTimeMilliseconds )
	: m_pDib( pDib )
	, m_nTimeMilliseconds( nTimeMilliseconds )
{

}

CFrame::~CFrame()
{
	delete m_pDib;
}
