/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	Gradient.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GRADIENT_H
#define GRADIENT_H

#ifndef COLOR_H
	#include "Color.h"
#endif	//	COLOR_H

namespace GS
{
	class CDrawContext;
};

class CGradient
{
public:
	CGradient();
	virtual ~CGradient();

	bool IsSet() const
	{
		return m_crBack.IsSet() || ( m_crLeft.IsSet() && m_crRight.IsSet() ) || ( m_crTop.IsSet() && m_crBottom.IsSet() );
	}

	CColor m_crBack;
	CColor m_crLeft;
	CColor m_crRight;
	CColor m_crTop;
	CColor m_crBottom;
	bool Draw( GS::CDrawContext &dc, const WinHelper::CRect &rc );

};

#endif //GRADIENT_H