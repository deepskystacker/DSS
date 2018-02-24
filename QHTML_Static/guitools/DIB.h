/*----------------------------------------------------------------------
Copyright (c)  Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	DIB.h
Owner:	russf@gipsysoft.com
Purpose:	<Description of module>.
----------------------------------------------------------------------*/
#ifndef DIB_H
#define DIB_H

#ifndef ARRAY_CONTAINER_H
	#include "../reuse/array.h"
#endif	//	ARRAY_CONTAINER_H

namespace GS
{
	#define DIB_PIXEL	RGBQUAD

	typedef DWORD   COLORREFA;
	//
	//	Alpha color types and macros
	#define DIB_RGBA(r,g,b,a)          ((GS::COLORREFA)(r)|( (g)<<8)|((b)<<16)|((a)<<24) )
	#define GetAValue(rgb)      ((BYTE)((rgb)>>24))
	#define COLORREF_ALPHA( cr, a) ((GS::COLORREFA) (cr) | ((a)<<24) )


	class CDIB
	{
	public:

		typedef Container::CArray< DIB_PIXEL * > CLineArray;

		//
		//	Create an empty DIB
		CDIB( HDC hdc, int cx, int cy );

		//
		//	Create a DIB and copy the contents of the DC given
		CDIB( HDC hdc, int x, int y, int cx, int cy );

		//
		//	CReate a DIB and copy the contents of the passed dib into it.
		CDIB( CDIB &dib, int x, int y, int cx, int cy );

		//
		//	Create a DIB from a bitmap
		CDIB( HBITMAP hbm );

		//
		//	Create from an icon
		CDIB( HICON hIcon );

		//
		//	Image dimensions
		inline int GetWidth() const { return m_size.cx; }
		inline int GetHeight() const { return m_size.cy; }
		const SIZE &GetSize() const { return m_size; }

		bool BasicDraw( HDC hdc, int nX, int nY );
		bool BasicStretch( HDC hdc, int nX, int nY, int nWidth, int nHeight );

		bool Draw( HDC hdc, int nX, int nY );
		bool Draw( HDC hdc, int nX, int nY, int nRight, int nBottom );

		bool DrawTransparent( HDC hdc, int nX, int nY, COLORREF crTransparent );
		bool DrawWithAlphaChannel( HDC hdc, int nX, int nY );
		bool StretchDrawTransparent( HDC hdc, int nX, int nY, int nCX, int nCY, COLORREF crTransparent );
		void AlphaBlend( HDC hdc, int nX, int nY, unsigned char nAlpha );
		void StretchAlphaBlend( HDC hdc, int nX, int nY, int nCX, int nCY, unsigned char nAlpha );
		void Blur( int nTimes );
		void DrawBitmapOntoUs( HBITMAP hBitmap, int x, int y, int nSourceX, int nSourceY, int cx, int cy );

		void GradientFillTopToBottom( const RECT &rc, COLORREF crTop, COLORREF crBottom );
		void GradientFillLeftToRight( const RECT &rc, COLORREF crTop, COLORREF crBottom );

		void AddImage( int nLeft, int nTop, CDIB *pDib );

		void ClearToColour( COLORREFA cr );

		CDIB *CreateCopy( int nCX, int nCY ) const;

		void DrawLine( int x1, int y1, int x2, int y2, COLORREFA cr );
		void PutPixelBlend( int x, int y, BYTE r, BYTE g, BYTE b, BYTE a );
		void PutPixel( int x, int y, BYTE r, BYTE g, BYTE b, BYTE a );
		bool GetPixel( int x, int y, BYTE &r, BYTE &g, BYTE &b, BYTE &a );
		void FillRect( const RECT &rc, COLORREFA );
		void Rectangle( const RECT &rc, COLORREFA );

		void SetColourTransparent( COLORREF cr );
		bool GetColourTransparent( COLORREF &cr) const { return m_bTransparentColorSet; cr = m_crTransparent; }

		inline const CLineArray &GetLineArray() { return m_arrLine; }

		HBITMAP GetBitmap()
		{
			#ifndef UNDER_CE
				GdiFlush();
			#endif	//	UNDER_CE
			return m_hBitmap;
		}

		CDIB *CreateResized( int newx, int newy ) const;
		void SetHasAlphaChannel( bool bHasAlpha ) { m_bHasAlpha = bHasAlpha; }

		bool IsValid() const { return m_pBits ? true : false; }

		virtual ~CDIB();

		enum { g_knBPP = 32 };

	private:
		void Initialise();

		HBITMAP m_hBitmap;
		DIB_PIXEL *m_pBits;
		BITMAPINFO m_bi;
		CLineArray m_arrLine;
		SIZE m_size;
		COLORREF m_crTransparent;
		bool m_bTransparentColorSet, m_bHasAlpha;



		#define BYTES_PER_PIXEL 4

		#define ALPHA( src, dest, color, alpha, negalpha ) dest->color = static_cast<unsigned char>( ( dest->color * negalpha + src->color * alpha ) >>8 )

	};

	extern void ConvertToGrayScale( GS::CDIB &dib );
	extern HRGN DIBCreateRegionFromTransparency( GS::CDIB &dib );
};




#endif //DIB_H