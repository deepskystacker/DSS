/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	GradientFill.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef GRADIENTFILL_H
#define GRADIENTFILL_H

#ifndef DRAWCONTEXT_H
	#include "DrawContext.h"
#endif	//	DRAWCONTEXT_H

extern void GradientFillTopToBottom( CDrawContext &dc, const WinHelper::CRect &rc, COLORREF crTop, COLORREF crBottom );
extern void GradientFillLeftToRight( CDrawContext &dc, const WinHelper::CRect &rc, COLORREF crTop, COLORREF crBottom );

#endif //GRADIENTFILL_H