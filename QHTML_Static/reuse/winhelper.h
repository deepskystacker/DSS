/*----------------------------------------------------------------------
Copyright (c) 1998 Gipsysoft. All Rights Reserved.
Please see the file "licence.txt" for licencing details.
File:	WinHelper.h
Owner:	russf@gipsysoft.com
Purpose:	Windows helper functions, classes, structures and macros
					that make life a little easier
					These should all be zero impact classes etc. that is they
					should *not* have a cpp file associated with them.
----------------------------------------------------------------------*/
#ifndef WINHELPER_H
#define WINHELPER_H

namespace WinHelper
{
	#define FASTCALL __fastcall


	#define ZeroStructure( t ) memset( &t, 0, sizeof( t ) )
	#define countof( t )	(sizeof( (t) ) / sizeof( (t)[0] ) )
#if ! defined(lint)
	#define UNREF(P)          (P)
#else	//	lint
	#define UNREF(P)          \
    /*lint -save -e527 -e530 */ \
    { \
        (P) = (P); \
    } \
    /*lint -restore */
#endif	//	lint

	inline int MulDiv(int nNumber, int nNumerator, int nDenominator)
	{
#if defined (_WIN32_WCE)
		__int64 x;

		x = (__int64)nNumber * (__int64)nNumerator;
		x /= (__int64)nDenominator;

		return (int)x;
#else	//	(_WIN32_WCE)
		return ::MulDiv( nNumber, nNumerator, nDenominator );
#endif	//	(_WIN32_WCE)
	}


	class CSize : public tagSIZE
	//
	//	Wrapper for the SIZE structure
	{
	public:
		inline CSize() {};
		inline explicit CSize( const SIZE &size ) { cx = size.cx; cy = size.cy; }
		inline explicit CSize( int nSizeX, int nSizeY ) { cx = nSizeX; cy = nSizeY; }
		inline CSize( const CSize& size ) { cx = size.cx; cy = size.cy; };
		inline operator LPSIZE() { return this; };
		inline void Set( long nSizeX, long nSizeY ) { cx = nSizeX; cy = nSizeY; }


		inline bool operator !=( const SIZE &size ) const { return cx != size.cx || cy != size.cy;}
		inline CSize & operator =( const SIZE &size ) { cx = size.cx; cy = size.cy; return *this; }
		inline bool operator >( const SIZE &size ) { return cx > size.cx || cy > size.cy; }
	};


	class CRect : public tagRECT
	//
	//	Wrapper for a RECT structure
	{
	public:
		inline CRect() {}
		//	Initialisation constructor
		inline explicit CRect( const RECT& rhs ) { Set( rhs.left, rhs.top, rhs.right, rhs.bottom );}
		inline CRect(int xLeft, int yTop, int xRight, int yBottom) { Set( xLeft, yTop, xRight, yBottom ); }
		//	Get the width of the rectangle
		inline int Width() const
			{ return right - left; }
		//	Get the height of the rectangle
		inline int Height() const
			{ return bottom - top; }
		//	overloaded operator so you don't have to do &rc anymore
		inline operator LPCRECT() const
			{ return this; };
		inline operator LPRECT()
			{ return this; };
		//	Return the SIZE of the rectangle;
		inline CSize Size() const
			{ CSize s( Width(), Height() ); return s; }
		//	Return the top left of the rectangle
		inline POINT TopLeft() const
			{ POINT pt = { left, top }; return pt; }
		//	Return the bottom right of the rectangle
		inline POINT BottomRight() const
			{ POINT pt = { right, bottom }; return pt; }
		//	Set the rectangles left, top, right and bottom
		inline void Set( int xLeft, int yTop, int xRight, int yBottom)
			{ top = yTop; bottom = yBottom; right = xRight; left = xLeft; }
		inline void Set( const WinHelper::CRect &rhs )
			{
				left = rhs.left; top = rhs.top;
				right = rhs.right; bottom = rhs.bottom;
			}

		//	Return true if the rectangle contains all zeros
		inline bool IsEmpty() const
			{ return left == 0 && right == 0 && top == 0 && bottom == 0 ? true : false; }
		//	Zero out our rectangle
		inline void Empty()
			{ left = right = top = bottom = 0; }
		//	Set the size of the rect but leave the top left position untouched.
		inline void SetSize( const CSize &size )
			{ bottom = top + size.cy; right = left + size.cx; }
		inline void SetSize( const SIZE &size )
			{ bottom = top + size.cy; right = left + size.cx; }
		inline void SetSize( int cx, int cy )
			{ bottom = top + cy; right = left + cx; }
		inline void SetHeight( int cy )
			{ bottom = top + cy; }
		inline void SetWidth( int cx )
			{ right = left + cx; }
		//	Move the rectangle by an offset
		inline void Offset( int cx, int cy )
			{
				top+=cy;
				bottom+=cy;
				right+=cx;
				left+=cx;
			}
		//	Inflate the rectangle by the cx and cy, use negative to shrink the rectangle
		inline void Inflate( int cx, int cy )
			{
				top-=cy;
				bottom+=cy;
				right+=cx;
				left-=cx;
			}
		//	Assignment from a RECT
		inline CRect &operator = ( const RECT&rhs )
			{
				left = rhs.left; top = rhs.top;
				right = rhs.right; bottom = rhs.bottom;
				return *this;
			}

		
		//	Return true if the point passed is within the rectangle
		inline bool PtInRect( const POINT &pt ) const	{	return  ( pt.x >= left && pt.x < right && pt.y >=top && pt.y < bottom ); }
		//	Return true if the rectangle passed overlaps this rectangle
		inline bool Intersect( const RECT &rc ) const {	return ( rc.left < right && rc.right > left && rc.top < bottom && rc.bottom > top ); }

		//	Adjust the rectangle so it would draw normally
		inline void Normalise()
			{
				if( right < left )
				{
					const long temp = left;
					left = right;
					right = temp;
				}
				if( bottom < top )
				{
					const long temp = top;
					top = bottom;
					bottom = temp;
				}
			}
	};

	class CPoint : public tagPOINT
	//
	//	Wrapper for the POINT structure
	{
	public:
		inline CPoint() {};
#ifdef WIN32
		inline CPoint( LPARAM lParam ) { x = ((int)(short)LOWORD(lParam)); y = ((int)(short)HIWORD(lParam)); }
#endif	//	WIN32
		inline CPoint( int nX, int nY ) { x = nX; y = nY; }
		inline CPoint( const POINT &pt ) { x = pt.x; y = pt.y; }
		inline bool operator == ( const CPoint &rhs ) const { return x == rhs.x && y == rhs.y; }
		inline bool operator != ( const CPoint &rhs ) const { return x != rhs.x || y != rhs.y; }
		inline operator LPPOINT () { return this; }
	};


	class CCriticalSection
	//
	//	Simple crtical section handler/wrapper
	{
	public:
		inline CCriticalSection()	{ ::InitializeCriticalSection(&m_sect); }
		inline ~CCriticalSection() { ::DeleteCriticalSection(&m_sect); }

		//	Blocking lock.
		inline void Lock()			{ ::EnterCriticalSection(&m_sect); }
		//	Unlock
		inline void Unlock()		{ ::LeaveCriticalSection(&m_sect); }

		class CLock
		//
		//	Simple lock class for the critcal section
		{
		public:
			inline CLock( CCriticalSection &sect ) : m_sect( sect ) { m_sect.Lock(); }
			inline ~CLock() { m_sect.Unlock(); }
		private:
			CCriticalSection &m_sect;

			CLock();
			CLock( const CLock &);
			CLock& operator =( const CLock &);
		};

	private:
		CRITICAL_SECTION m_sect;

		CCriticalSection( const CCriticalSection & );
		CCriticalSection& operator =( const CCriticalSection & );
	};

#ifdef WIN32
	class CBrush
	//
	//	Wrapper for a brush
	{
	public:
		CBrush( HBRUSH hbr ) : m_hbr( hbr ) {}
		CBrush( COLORREF cr ) : m_hbr( CreateSolidBrush( cr ) ) {}
		
		~CBrush() { DeleteObject( m_hbr ); }
		operator HBRUSH() { return m_hbr; }

		HBRUSH m_hbr;
	};


	class CPen
	//
	//	Wrapper for a brush
	{
	public:
		CPen( HPEN h ) : m_pen( h ) {}
		CPen( COLORREF cr, int nWidth = 1, int nStyle = PS_SOLID ) : m_pen( CreatePen( nStyle, nWidth, cr ) ) {}
		
		~CPen() { DeleteObject( m_pen ); }
		operator HPEN() { return m_pen; }

		HPEN m_pen;
	};

	
	class CWindowDC
	//
	//	Wrapper to get and release a window DC
	{
	public:
		CWindowDC( HWND hwnd )
			: m_hdc( ::GetDC( hwnd ) )
			, m_hwnd( hwnd )
			{}

		~CWindowDC() { ::ReleaseDC( m_hwnd, m_hdc ); }
		operator HDC() const { return m_hdc; }
	private:
		HWND m_hwnd;
		HDC m_hdc;
	};


	class CSaveDCObject
	//
	//	Simple class to 
	{
	public:
		CSaveDCObject( HDC hdc, HGDIOBJ h )
			: m_hOld( SelectObject( hdc, h ) )
			, m_hdc( hdc )
			{}
		~CSaveDCObject()
		{
			SelectObject( m_hdc, m_hOld );
		}

	private:
		HDC m_hdc;
		HGDIOBJ m_hOld;
	};

	class CScrollInfo : public tagSCROLLINFO
	{
	public:
		CScrollInfo( unsigned int fPassedMask ) { cbSize = sizeof( tagSCROLLINFO ); fMask = fPassedMask; }
	};



	inline bool IsShiftPressed()
	{
		return GetKeyState(VK_SHIFT) & 0x8000 ? true : false;
	}

	inline bool IsAltPressed()
	{
		return GetKeyState(VK_MENU) & 0x8000 ? true : false;
	}

	inline bool IsControlPressed()
	{
		return GetKeyState(VK_CONTROL) & 0x8000 ? true : false;
	}

	inline HICON LoadIcon16x16( HINSTANCE hInst, unsigned int uID )
	//
	//	Load a 16x16 icon from the same resource as the other size icons.
	{
		return reinterpret_cast<HICON>( ::LoadImage( hInst, MAKEINTRESOURCE( uID ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR ) );
	}
	inline HICON LoadIcon32x32( HINSTANCE hInst, unsigned int uID )
	//
	//	Load a 16x16 icon from the same resource as the other size icons.
	{
		return reinterpret_cast<HICON>( ::LoadImage( hInst, MAKEINTRESOURCE( uID ), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR ) );
	}


	class CDeferWindowPos
	//
	//	Wrapper for the Begin, Defer and End WindowPos functions. Nothing glamorous.
	{
	public:
		inline CDeferWindowPos( const int nWindows = 1 ) : m_hdlDef( ::BeginDeferWindowPos( nWindows ) ) {}
		inline ~CDeferWindowPos() { (void)::EndDeferWindowPos( m_hdlDef ); }
		inline HDWP DeferWindowPos( HWND hWnd, HWND hWndInsertAfter , int x, int y, int cx, int cy, unsigned int uFlags )
		{
			return ::DeferWindowPos( m_hdlDef, hWnd, hWndInsertAfter, x, y, cx, cy, uFlags );
		}
		inline HDWP DeferWindowPos( HWND hWnd, HWND hWndInsertAfter, const CRect &rc, unsigned int uFlags )
		{
			return ::DeferWindowPos( m_hdlDef, hWnd, hWndInsertAfter, rc.left, rc.top, rc.Width(), rc.Height(), uFlags );
		}

	private:
		HDWP m_hdlDef;
	};
#endif	//	WIN32

	template <class T>
	inline T SkipWhiteSpace( T pcsz )
	//
	//	Skip past any whitespace
	{
		while( *pcsz && isspace( *pcsz ) ) pcsz++;
		return pcsz;
	}


}	//	WinHelper

#define BEGINTHREAD( pfnStartAddr, pvParam, pdwThreadID ) \
		 ((HANDLE) _beginthreadex(                     \
				(void *) (0),                            \
				(unsigned) (0),                      \
				(unsigned (__stdcall *) (void *))pfnStartAddr,            \
				(void *) (pvParam),                        \
				(unsigned) (0),                    \
				(unsigned *) (pdwThreadID))) 

#endif //WINHELPER_H
