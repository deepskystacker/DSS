#ifndef __COMMON_H__
#define __COMMON_H__

#if defined(PCL_PROJECT) || defined(_CONSOLE)
// Redefine WIN32 types and important classes

#if !defined(_WINDOWS_)
#define bool				bool
#define true				true
#define false				false
#define LONG				int32
#define DWORD				uint32
#define WORD				uint16
#define INT					int
#define BYTE				char

typedef struct tagPOINT {
   LONG x;
   LONG y;
} POINT;

typedef struct tagRECT {
  LONG left;
  LONG top;
  LONG right;
  LONG bottom;
} RECT, *PRECT, *LPRECT;

typedef const RECT *		LPCRECT;

typedef DWORD				COLORREF;
typedef DWORD *				LPCOLORREF;

#define LOBYTE(w)           ((BYTE)((DWORD)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD)(w) >> 8))

#define GetRValue(rgb)      (LOBYTE(rgb))
#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))

#endif

//#ifndef min
//#define min(a, b)	((a)<(b) ? (a) : (b))
//#endif
//
//#ifndef max
//#define max(a, b)	((a)>(b) ? (a) : (b))
//#endif

class CPoint
{
public :
	LONG	x;
	LONG	y;

public :
	CPoint(LONG x0 = 0, LONG y0 = 0)
	{
		x = x0;
		y = y0;
	}

	CPoint(const CPoint & pt)
	{
		x = pt.x;
		y = pt.y;
	};

	CPoint(const POINT & pt)
	{
		x = pt.x;
		y = pt.y;
	};

	const CPoint & operator = (const CPoint & pt)
	{
		x = pt.x;
		y = pt.y;
		return (*this);
	};

	virtual ~CPoint()
	{
	};
};

class CRect : public tagRECT
{
#ifndef _WINDOWS_
public :
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
#endif

public :
	CRect(LONG l = 0, LONG t = 0, LONG r = 0, LONG b = 0)
	{
		left	= min(l, r);
		top		= min(t, b);
		right	= max(l, r);
		bottom	= max(t, b);
	};

	CRect(const CRect & rc)
	{
		left	= rc.left;
		right	= rc.right;
		top		= rc.top;
		bottom	= rc.bottom;
	};

	const CRect & operator = (const CRect & rc)
	{
		left	= rc.left;
		right	= rc.right;
		top		= rc.top;
		bottom	= rc.bottom;

		return (*this);
	};

	operator LPRECT ()
	{
		return this;
	};

	operator LPCRECT ()
	{
		return this;
	};

	LONG	Width() const
	{
		return right-left;
	};

	LONG	Height() const
	{
		return bottom-top;
	};

	virtual ~CRect()
	{
	};

	bool	PtInRect(const CPoint & pt) const
	{
		return (pt.x >= left) && (pt.x <= right) &&
			   (pt.y >= top) && (pt.y <= bottom);
	};

	void	SetRectEmpty()
	{
		left = right = top = bottom = 0;
	};

	bool	IsRectEmpty() const
	{
		return (left==right) || (top == bottom);
	};

	bool	IntersectRect(LPCRECT pRect1, LPCRECT pRect2)
	{
		bool		bResult = false;

		if (pRect1 && pRect2)
		{
			if (! (pRect2->left   > pRect1->right ||
				   pRect2->right  < pRect1->left ||
				   pRect2->top    > pRect1->bottom ||
				   pRect2->bottom < pRect1->top ))
			{
				left	= max(pRect1->left, pRect2->left);
				right	= min(pRect1->right, pRect2->right);
				top		= max(pRect1->top, pRect2->top);
				bottom	= min(pRect1->bottom, pRect2->bottom);
				bResult = true;
			}
		};
		if (!bResult)
			left = right = top = bottom = 0;

		return bResult;
	};

	void	OffsetRect(int dx, int dy)
	{
		left	+= dx;
		right	+= dx;
		top		+= dy;
		bottom	+= dy;
	};

	void	SetRect(int x1, int y1, int x2, int y2)
	{
		left = x1;		right = x2;
		top	 = y1;		bottom = y2;
	};
};

#endif // PCL_PROJECT || _CONSOLE

#endif // __COMMON_H__