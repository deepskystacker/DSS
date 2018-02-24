/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	HTMLSectionCreator.cpp
Owner:	russf@gipsysoft.com
Purpose:	Section creator for the HTML display.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include <math.h>
#include "qhtm.h"
#include "HTMLParse.h"
#include "HTMLSectionCreator.h"
#include "HTMLTextSection.h"
#include "HTMLSection.h"
#include "defaults.h"
#include <reuse/mapiter.h>


CHTMLSectionCreator::~CHTMLSectionCreator()
{
	for( Container::CMapIter< HTMLFontDef, GS::FontDef * > iter( m_mapFonts ); !iter.EOL(); iter.Next() )
	{
		GS::FontDef *pDef = iter.GetValue();
		pDef->Release();
	}
}
