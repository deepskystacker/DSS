// cdxCRot90DC.h: interface for the cdxCRot90DC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CDXCROT90DC_H__BB122324_D85E_11D2_BF3F_000000000000__INCLUDED_)
#define AFX_CDXCROT90DC_H__BB122324_D85E_11D2_BF3F_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//
// cdxCRot90DC.h : header file
// -----------------------------------------------------------------------
// Author:  Hans Bühler (hans.buehler@student.hu-berlin.de)
//          codex design (http://www-pool.mathematik.hu-berlin.de/~codex
// Version: 1.0
// Release: 1 (Mar 1999 to www.codeguru.com)
// Notes:   This is an advanced version of the CMemDC class that has
//				been published by Keith Rule (even at codeguru's).
// -----------------------------------------------------------------------
// Comments welcome.
//

/*
 * cdxCRot90DC
 * ===========
 * A memory device context that allows to rotate output.
 * Based on CMemDC by Keith Rule - you can find this DC on codeguru as well.
 *
 * The documentation for usage information.
 *
 * (w)Mar.1999 Hans Bühler, codex design
 *    mailto:hans.buehler@student.hu-berlin.de
 */

class cdxCRot90DC : public CDC
{
	DECLARE_DYNAMIC(cdxCRot90DC);

public:
	enum Angle
	{
		a0		=	0,
	//	a360	=	a0,
		a90	=	1,
		a180	=	2,
		a270	=	3,
		a_num	=	4
	};

	enum DrawControlFlags
	{
		DRC_DISABLED			=	0x01,		// draw disabled
		DRC_NO_ICON_ROTATION	=	0x02		// don't rotate icon
	};

	enum
	{
		DRAWTEXT_HPOS			=	DT_LEFT|DT_RIGHT|DT_CENTER,
		DRAWTEXT_VPOS			=	DT_TOP|DT_BOTTOM|DT_VCENTER
	};

protected:
	typedef POINT	PLGBLTPNTS[3];

private:
	struct Rotator
	{
		int	m_iSin,m_iCos;

	public:
		long X(long x, long y) const { return x*m_iCos + y*m_iSin; }
		long Y(long x, long y) const { return y*m_iCos - x*m_iSin; }
		CSize Size(const CSize & sz) const { if(!m_iCos) return sz; return CSize(sz.cy,sz.cx); }
	};

private:
	CDC		*m_pDC;
	CRect		m_rectMine,m_rectMineClipped,
				m_rectDC,m_rectDCClipped;
	Angle		m_Angle;
	CBitmap	m_Bitmap;
	CSize		m_szBitmap;
	bool		m_bFinished;

	// device context storings
	int		m_iOldBkColor,m_iOldBkMode,m_iOldTextColor;
	CFont		*m_pOldFont;
	CBitmap	*m_pOldBitmap;

    void Init()
    {
        m_pDC = nullptr;
        m_Angle = a0;
        m_bFinished = false;
        m_iOldBkColor = 0;
        m_iOldBkMode = 0;
        m_iOldTextColor = 0;
        m_pOldBitmap = nullptr;
        m_pOldFont = nullptr;
    }

public:
    cdxCRot90DC()
    {
        Init();
    }
    
	cdxCRot90DC(CDC & destDC, const CRect & rectDC, int iAngle, bool bCopy = false)
    {
        Init();
        Create(destDC,rectDC,iAngle,bCopy);
    }
	cdxCRot90DC(CDC *pDC, const CRect & rectDC, int iAngle, bool bCopy = false)
    {
        Init();
        Create(pDC,rectDC,iAngle,bCopy);
    }
	cdxCRot90DC(CDC & destDC, int iAngle, bool bCopy = false)
    {
        Init();
        VERIFY( Create(destDC,iAngle,bCopy) );
    }
	cdxCRot90DC(CDC *pDC, int iAngle, bool bCopy = false)
    {
        Init();
        VERIFY( Create(pDC,iAngle,bCopy) );
    }
	virtual ~cdxCRot90DC() { if(!m_bFinished) Finish(); Destroy(); }

	//
	// create & destroy
	//

	bool IsCreated() const { return m_pDC != nullptr; }
	const CRect & GetRotRect() const { return m_rectMine; }

	bool Create(CDC & destDC, const CRect & rectDC, int iAngle, bool bCopy = false);
	bool Create(CDC * pDC, const CRect & rectDC, int iAngle, bool bCopy = false);
	bool Create(CDC & destDC, int iAngle, bool bCopy = false);
	bool Create(CDC * pDC, int iAngle, bool bCopy = false);
	bool Finish();
	void Invalidate() { m_bFinished = true; if(m_pDC && m_hDC) IntersectClipRect(CRect(0,0,0,0)); }
	void Destroy();

	//
	// copy from one DC to the other
	//

	void CopyTo(const CRect & rectDC) { CopyRotTo(rotate(rectDC)); }
	void CopyFrom(const CRect & rectDC);
	void CopyRotTo(const CRect & rectRotDC);
	void CopyRotFrom(const CRect & rectRotDC) { CopyFrom(rotateBack(rectRotDC)); }

	//
	// used to rotate my stuff
	// Forward rotation: Original DC -> Rot90 DC
	// Backward          Rot90 DC -> Original DC
	//

	CRect rotate(const CRect & r) const { return rotate(r,m_Angle); }
	CPoint rotate(const CPoint & p) const { return rotate(p,m_Angle); }
	CSize rotate(const CSize & s) const { return rotate(s,m_Angle); }
	void rotate(POINT *pPnts, UINT nCnt) const { rotate(pPnts,nCnt,m_Angle); }

	CRect rotateBack(const CRect & r) const { return rotate(r,inv(m_Angle)); }
	CPoint rotateBack(const CPoint & p) const { return rotate(p,inv(m_Angle)); }
	CSize rotateBack(const CSize & s) const { return rotate(s,inv(m_Angle)); }
	void rotateBack(POINT *pPnts, UINT nCnt) const { rotate(pPnts,nCnt,inv(m_Angle)); }

	//
	// some operators to use this DC as a pointer
	//

	operator cdxCRot90DC * () { return m_pDC ? this : nullptr; }
	operator const CRect & () const { return GetRotRect(); }
	cdxCRot90DC *operator->() { ASSERT(IsCreated()); return this; }
	const cdxCRot90DC *operator->() const { ASSERT(IsCreated()); return this; }

	//
	// simple rotation
	//

	CRect operator()(const CRect & r, bool bFwd = true) const { return bFwd ? rotate(r) : rotateBack(r); }
	CPoint operator()(const CPoint & p, bool bFwd = true) const { return bFwd ? rotate(p) : rotateBack(p); }
	CSize operator()(const CSize & sz, bool bFwd = true) const { return bFwd ? rotate(sz) : rotateBack(sz); }
	void operator()(POINT *pPnts, UINT nCnt, bool bFwd = true) const { if(bFwd) rotate(pPnts,nCnt); else rotateBack(pPnts,nCnt); }

	//
	// static members
	//

	static CRect rotate(const CRect & r, int iAngle) { return rotate(r,mkAngle(iAngle)); }
	static CPoint rotate(const CPoint & p, int iAngle) { return rotate(p,mkAngle(iAngle)); }
	static CSize rotate(const CSize & s, int iAngle) { return rotate(s,mkAngle(iAngle)); }
	static void rotate(POINT *pPnts, UINT nCnt, int iAngle) { rotate(pPnts,nCnt,mkAngle(iAngle)); }

	static CRect rotate(const CRect & r, Angle iAngle);
	static CPoint rotate(const CPoint & p, Angle iAngle);
	static CSize rotate(const CSize & s, Angle iAngle);
	static void rotate(POINT *pPnts, UINT nCnt, Angle iAngle);

	static Angle inv(Angle a) { return a ? (Angle)(a_num - a) : a; }
	static Angle mkAngle(int iAngle);
	static int mkAngle(Angle a) { return a * 90; }

	//
	// further utility functions
	//

	void DrawControl(CString strText, HICON hIcon, UINT nDrawTextFlags = DT_CENTER|DT_VCENTER, UINT nMyFlags = 0, UINT nIconDist = 2) { DrawControl(m_rectMine,strText,hIcon,nDrawTextFlags,nMyFlags,nIconDist); }
	void DrawControl(CString strText, UINT nDrawTextFlags = DT_CENTER|DT_VCENTER, UINT nMyFlags = 0, UINT nIconDist = 2)  { DrawControl(m_rectMine,strText,nullptr,nDrawTextFlags,nMyFlags,nIconDist); }
	void DrawControl(CRect rect, CString strText, HICON hIcon, UINT nDrawTextFlags = DT_CENTER|DT_VCENTER, UINT nMyFlags = 0, UINT nIconDist = 2);
	void DrawControl(CRect rect, CString strText, UINT nDrawTextFlags = DT_CENTER|DT_VCENTER, UINT nMyFlags = 0, UINT nIconDist = 2) { DrawControl(rect,strText,nullptr,nDrawTextFlags,nMyFlags,nIconDist); }
	static CSize GetBitmapSize(HBITMAP hbmp);
	static CSize GetIconSize(HICON hIcon);

	//
	// protected members
	//
protected:
	static void PrePlgBlt(const CRect & rectSrc, PLGBLTPNTS & pnts, Angle iAngle);

	//
	// static member variables
	//
public:
	static const Rotator M_Rotator[a_num];
};

//////////////////////////////////////////////////////////////////////
// inlines
//////////////////////////////////////////////////////////////////////

/*
 * creation
 */

inline bool cdxCRot90DC::Create(CDC * pDC, const CRect & rectDC, int iAngle, bool bCopy)
{
	if(!pDC)
	{
		m_rectMine.SetRectEmpty();
		return false;
	}
	return Create(*pDC,rectDC,iAngle,bCopy);
}

/*
 * creation with current clip rect
 */

inline bool cdxCRot90DC::Create(CDC * pDC, int iAngle, bool bCopy)
{
	if(!pDC)
	{
		m_rectMine.SetRectEmpty();
		return false;
	}
	return Create(*pDC,iAngle,bCopy);
}

inline bool cdxCRot90DC::Create(CDC & destDC, int iAngle, bool bCopy)
{
	CRect	rect;destDC.GetClipBox(rect);
	return Create(destDC,rect,iAngle,bCopy);
}

/*
 * convert rectangle coords to rotated coords
 */

inline CRect cdxCRot90DC::rotate(const CRect & r, cdxCRot90DC::Angle iAngle)
{
	const Rotator	& rot	=	M_Rotator[iAngle];
	CRect	rect;

	rect.left	=	rot.X(r.left,r.top);
	rect.top		=	rot.Y(r.left,r.top);
	rect.right	=	rot.X(r.right,r.bottom);
	rect.bottom	=	rot.Y(r.right,r.bottom);

	long	l;
	if(rect.left > rect.right)
	{
		l	=	rect.left;
		rect.left	=	rect.right;
		rect.right	=	l;
	}
	if(rect.top > rect.bottom)
	{
		l	=	rect.top;
		rect.top		=	rect.bottom;
		rect.bottom	=	l;
	}
	return rect;
}

inline CPoint cdxCRot90DC::rotate(const CPoint & p, cdxCRot90DC::Angle iAngle)
{
	CPoint	pnt;
	pnt.x			=	M_Rotator[iAngle].X(p.x,p.y);
	pnt.y			=	M_Rotator[iAngle].Y(p.x,p.y);
	return pnt;
}

inline CSize cdxCRot90DC::rotate(const CSize & sz, cdxCRot90DC::Angle iAngle)
{
	return M_Rotator[iAngle].Size(sz);
}

inline void cdxCRot90DC::rotate(POINT *pPnts, UINT nCnt, cdxCRot90DC::Angle iAngle)
{
	CPoint			p;
	const Rotator	& r	=	M_Rotator[iAngle];

	for(;nCnt>0; --nCnt, ++pPnts)
	{
		p			=	*pPnts;
		pPnts->x	=	r.X(p.x,p.y);
		pPnts->y	=	r.Y(p.x,p.y);
	}
}

//////////////////////////////////////////////////////////////////////

/*
 * convert 90° angle to Angle code
 */

inline cdxCRot90DC::Angle cdxCRot90DC::mkAngle(int iAngle)
{
	iAngle	/=	90;
	iAngle	%=	a_num;
	if(iAngle < 0)
		iAngle	+=	a_num;
	return (Angle)iAngle;
}

//////////////////////////////////////////////////////////////////////

/*
 * calculate stuff for PlgBlt
 */

inline void cdxCRot90DC::PrePlgBlt(const CRect & rectSrc, PLGBLTPNTS & pnts, Angle iAngle)
{
	pnts[0].x	=	rectSrc.left;
	pnts[0].y	=	rectSrc.top;
	pnts[1].x	=	rectSrc.right;
	pnts[1].y	=	rectSrc.top;
	pnts[2].x	=	rectSrc.left;
	pnts[2].y	=	rectSrc.bottom;

	rotate((POINT *)pnts,3,iAngle);
}

//////////////////////////////////////////////////////////////////////

/*
 * two nice helpers
 */

inline CSize cdxCRot90DC::GetBitmapSize(HBITMAP hbmp)
{
	BITMAP	bmp;
	bmp.bmWidth	=	bmp.bmHeight	=	0;

	VERIFY( ::GetObject(hbmp,sizeof(BITMAP),&bmp) );
	return CSize(bmp.bmWidth,bmp.bmHeight);
}

inline CSize cdxCRot90DC::GetIconSize(HICON hIcon)
{
	ICONINFO	ii;

	VERIFY( ::GetIconInfo(hIcon,&ii) );
	return GetBitmapSize(ii.hbmColor);
}

//////////////////////////////////////////////////////////////////////
// Blitting stuff around
//////////////////////////////////////////////////////////////////////

/*
 * copy rectRotDC of this context to target context
 */

inline void cdxCRot90DC::CopyRotTo(const CRect & rectRotDC)
{
	if(!m_pDC || rectRotDC.IsRectEmpty())
		return;

	if(m_Angle == a0)
	{
		m_pDC->BitBlt(	rectRotDC.left,
							rectRotDC.top,
							rectRotDC.Width(),
							rectRotDC.Height(),
							this,
							rectRotDC.left,
							rectRotDC.top,
							SRCCOPY);
	}
	else
	{
		PLGBLTPNTS	pnts;
		PrePlgBlt(rectRotDC,pnts,inv(m_Angle));

		::PlgBlt(	m_pDC->m_hDC,
						(const POINT *)&pnts,
						m_hDC,
						rectRotDC.left,
						rectRotDC.top,
						rectRotDC.Width(),
						rectRotDC.Height(),
						nullptr,
						0,
						0);
	}
}

/*
 * fill this context with rectDC from target context
 */

inline void cdxCRot90DC::CopyFrom(const CRect & rectDC)
{
	if(!m_pDC || rectDC.IsRectEmpty())
		return;

	if(m_Angle == a0)
	{
		BitBlt(	rectDC.left,
					rectDC.top,
					rectDC.Width(),
					rectDC.Height(),
					m_pDC,
					rectDC.left,
					rectDC.top,
					SRCCOPY);
	}
	else
	{
		PLGBLTPNTS	pnts;
		PrePlgBlt(rectDC,pnts,m_Angle);

		::PlgBlt(m_hDC,
					(const POINT *)&pnts,
					m_pDC->m_hDC,
					rectDC.left,
					rectDC.top,
					rectDC.Width(),
					rectDC.Height(),
					nullptr,
					0,
					0);
	}
}


#endif // !defined(AFX_CDXCROT90DC_H__BB122324_D85E_11D2_BF3F_000000000000__INCLUDED_)
