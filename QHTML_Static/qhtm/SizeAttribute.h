/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	SizeAttribute.h
Owner:	russf@gipsysoft.com
Purpose:

	A measurement unit

----------------------------------------------------------------------*/
#ifndef SIZEATTRIBUTE_H
#define SIZEATTRIBUTE_H

#include <guitools/DrawContext.h>

class CSizeAttribute
{
public:
	enum Type {
		knNone				//	Can't be this
		, knLine		

		, knPoint
		, knPixel
		, knPC
		, knMM
		, knCM
		, knInch			//	Inches are stored in 100th's of an inch
		, knEM
		, knPercent
		, knHTML
		, knHTMLPlus
		, knHTMLMinus

		, knMax				//	Can't be this
		};

	CSizeAttribute( Type type, int nSize)
		: m_Type( type )
		, m_nSize( nSize )
	{
	}

	CSizeAttribute()
		: m_Type( knNone )
	{
	}

	int ConvertToPixels( class CHTMLSectionCreator *psc ) const;
	int CSizeAttribute::ConvertToPixels( GS::CDrawContext &dc ) const;

	Type m_Type;
	int m_nSize;
};

#endif //SIZEATTRIBUTE_H