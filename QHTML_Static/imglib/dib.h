/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	dib.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef DIB_H
#define DIB_H

#ifndef ARRAY_CONTAINER_H
	#include <array.h>
#endif	//	ARRAY_CONTAINER_H

#define DIB_PIXEL	RGBQUAD

class CDib
{
public:

	typedef Container::CArray< DIB_PIXEL * > CLineArray;

	//
	//	Create an empty DIB
	CDib( HDC hdc, size_t cx, size_t cy );

	//
	//	Create a DIB and copy the contents of the DC given
	CDib( HDC hdc, int x, int y, size_t cx, size_t cy );

	//
	//	Create a DIB from a bitmap
	CDib( HBITMAP hbm );

	//
	//	Image dimensions
	inline size_t GetWidth() const { return m_nWidth; }
	inline size_t GetHeight() const { return m_nHeight; }
	DIB_PIXEL *GetBits() { return m_pBits;}

	//
	//	Set this image as drawing transparently
	void SetTransparentColour( COLORREF cr );
	bool GetTransparentColour( COLORREF &cr ) const;

	void SetHasAlphaChannel( bool bHasAlphaChannel );

	bool Draw( HDC hdc, int nX, int nY );
	bool Draw( HDC hdc, int nX, int nY, int nRight, int nBottom );
	bool DrawTransparent( HDC hdc, int nX, int nY, COLORREF crTransparent );
	bool StretchDrawTransparent( HDC hdc, int nX, int nY, size_t nCX, size_t nCY, COLORREF crTransparent );
	bool AlphaBlend( HDC hdc, int nX, int nY, unsigned char nAlpha );
	bool AlphaBlendChannel( HDC hdc, int nX, int nY );
	bool StretchAlphaBlend( HDC hdc, int nX, int nY, size_t nCX, size_t nCY, unsigned char nAlpha );

	void AddImage( size_t nLeft, size_t nTop, CDib *pDib );

	CDib *CreateCopy( size_t nCX, size_t nCY ) const;

	inline const CLineArray &GetLineArray() { return m_arrLine; }

	HBITMAP GetBitmap() { return m_hBitmap; }

	virtual ~CDib();

private:
	void Initialise();

	HBITMAP m_hBitmap;
	DIB_PIXEL *m_pBits;
	BITMAPINFO m_bi;
	CLineArray m_arrLine;
	size_t m_nWidth, m_nHeight;


	bool m_bHasAlphaChannel;
	bool m_bTransparent;
	COLORREF m_crTransparent;
};


#endif //DIB_H