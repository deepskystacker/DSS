/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GuiTools.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GUITOOLS_H
#define GUITOOLS_H

#ifndef DRAWCONTEXT_H
	#include "DrawContext.h"
#endif	//	DRAWCONTEXT_H

#ifndef GRADIENTFILL_H
	#include "GradientFill.h"
#endif	//	GRADIENTFILL_H


#ifndef PALETTE_H
	#include "Palette.h"
#endif	//	PALETTE_H

#ifndef DIB_H
	#include "dib.h"
#endif	//	DIB_H

namespace GS
{
	enum RoundRectCorners {
							knGFCNone
							, knGFCRoundLeftTop = 1
							, knGFCRoundRightTop = 2
							, knGFCRoundLeftBottom = 4
							, knGFCRoundRightBottom = 8
							, knGFCMax
							};


	HRGN CreateRoundCornerRegion( const WinHelper::CRect &rc, UINT uRoundCorners, UINT uRoundness );

	void RadialGradient( CDIB &dib, const WinHelper::CRect &rc, GS::COLORREFA crFrom, GS::COLORREFA crTo, int nGradientX, int nGradientY, int nGradientRadius );
};


#endif //GUITOOLS_H