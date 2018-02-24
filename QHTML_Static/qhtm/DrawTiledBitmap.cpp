/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DrawTiledBitmap.cpp
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#include "stdafx.h"
#include "../imglib/imglib.h"
#include "QHTM.h"

void DrawTiledBitmap( int x, int y, int cx, int cy, HDC hdc, const CQHTMImageABC *pImage, UINT uFrame )
{
	const SIZE &size = pImage->GetSize();

	//
	//	Figure out when our last images will be drawn.
	int nEndCol = cx / size.cx + 1;
	int nEndRow = cy / size.cy + 1;
	POINT pt;
	for( int nCol = 0; nCol < nEndCol; nCol++ )
	{
		pt.x = x + size.cx * nCol;
		for( int nRow = 0; nRow < nEndRow; nRow++ )
		{
			pt.y = y + size.cy * nRow;
			(void)pImage->DrawFrame( uFrame, hdc, pt.x , pt.y );
		}
	}
}
