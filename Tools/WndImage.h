// =======================================================
//
//                      CWndImage.h
//              WndImage class declarations
//
//           Copyright (C) 2000 Peter Hauptmann
//
//  Can be used and distributed freely
//          as long as the source copyright stays intact
//
//  updated version can be found under
//          http://buerger.metropolis.de/bitbucket/
//
// -------------------------------------------------------
//
//  2000-05-16: Initial Revision
//



#if !defined(AFX_WNDIMAGE_H__18E060C2_DE37_4419_B87A_09ACF6963828__INCLUDED_)
#define AFX_WNDIMAGE_H__18E060C2_DE37_4419_B87A_09ACF6963828__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <windows.h>

#include "ButtonToolbar.h"

// ==================================================================
//  CWndImage
// ------------------------------------------------------------------

#define NM_ZOOM					1
#define NM_NOTIFYMODECHANGE		2

class CWndImage;

// ------------------------------------------------------------------

class CWndImageSink
{
protected :
	CWndImage *			m_pImage;

public :
	CWndImageSink() : m_pImage(nullptr) {};
	virtual ~CWndImageSink() {};

	void	SetImage(CWndImage * pImage)
	{
		m_pImage = pImage;
	};

	virtual bool	Image_OnMouseLeave() { return false; };
	virtual bool	Image_OnMouseMove([[maybe_unused]] LONG lX, [[maybe_unused]] LONG lY) { return false; };
	virtual bool	Image_OnLButtonDown([[maybe_unused]] LONG lX,[[maybe_unused]] LONG lY) { return false; };
	virtual bool	Image_OnLButtonUp([[maybe_unused]] LONG lX, [[maybe_unused]] LONG lY) { return false; };
	virtual bool	Image_OnRButtonDown([[maybe_unused]] LONG lX, [[maybe_unused]] LONG lY) { return false; };

	virtual Image *	GetOverlayImage([[maybe_unused]] CRect & rcClient) { return nullptr; };
};

// ------------------------------------------------------------------

class CWndImage : public CWnd
{
  public:
	virtual    ~CWndImage();
	            CWndImage(bool bDarkMode = false);

    BOOL        Create(RECT const & r, CWnd * parent, UINT id,
                       DWORD dwStyle = WS_CHILD | WS_VISIBLE);

	BOOL        CreateFromStatic(CWnd * st);

    void        SetBltMode(int mode);
    void        SetAlign(int alignX, int alignY); // pass zero to keep value
    void        SetSourceRect(RECT const & r);
    void        SetSourceRect();                        // use entire image

    void        SetZoom(double zoomX, double zoomY);
    void        SetZoom(double zoom);                       // zoomx=zoomy

    void        SetOrigin(int origX, int origY);
    void        SetOriginX(int origX);
    void        SetOriginY(int origY);

    void        SetImg(HBITMAP bmp, bool shared = false);
    void        SetImg(CBitmap * bmp);
    bool        SetImg(LPCTSTR resID, HINSTANCE instance = 0);
    bool        SetImg(UINT resID, HINSTANCE instance = 0);
    bool        SetImgFile(LPCTSTR fileName);

    int         GetImgSizeX() const      { return m_bmpSize.cx;  }
    int         GetImgSizeY() const      { return m_bmpSize.cy;  }
    int         GetBltMode()  const      { return m_bltMode;     }
    double      GetZoomX()    const      { return m_zoomX;       }
    double      GetZoomY()    const      { return m_zoomY;       }
    int         GetAlignX()   const      { return m_alignX;      }
    int         GetAlignY()   const      { return m_alignY;      }
    int         GetOriginX()  const      { return m_origin.x;    }
    int         GetOriginY()  const      { return m_origin.y;    }
    HBITMAP     GetBitmap(bool detach = false);

	void		Invalidate(BOOL bEraseBackground = FALSE)
	{
		if (bEraseBackground)
			m_bInvalidateInternalBitmap = TRUE;
		CWnd::Invalidate(bEraseBackground);
	};

	void		EnableZoom(BOOL bEnable)
	{
		m_bEnableZoom = bEnable;
	};

	void	SetButtonToolbar(CButtonToolbar * pButtonToolbar)
	{
		m_pButtonToolbar = pButtonToolbar;
	};

	void	SetImageSink(CWndImageSink * pImageSink)
	{
		if (m_pImageSink)
			m_pImageSink->SetImage(nullptr);
		m_pImageSink = pImageSink;
		if (m_pImageSink)
			m_pImageSink->SetImage(this);
		m_bInvalidateOverlayBitmap = TRUE;
		Invalidate(FALSE);
	};

	void	Set4CornersMode(bool bMode)
	{
		if (bMode != m_4Corners)
		{
			m_4Corners = bMode;
			m_bInvalidateOverlayBitmap = TRUE;
			Invalidate(FALSE);
		};
	};

	bool	Get4CornersMode()
	{
		return m_4Corners;
	};


  protected:

    void        Recalc(bool invalidateWnd = true);


    CBitmap     m_bmp;              // the bitmap we wanna blit

	BOOL		m_bEnableZoom;		// Zoom is enabled?
	CRect		m_rcZoom;			// Rectangle where the zoom is drawn
	BOOL		m_bTrackTimerSet;	// Is the tracking timer active

	CSize       m_bmpSize;          // size of the bitmap
    bool        m_shared;           // the bitmap is shared (don't delete upon destruction/replacement)
	bool		m_4Corners;			// The 4 corners and the center are shown

	bool		m_bDarkMode;

	// Selection rectangle
	/*
	BOOL		m_bAllowSelection;
	BOOL		m_bSelecting;
	CRect		m_rcSelect;
	CRect		m_rcNewSelect;
	COLORREF	m_crSelection;
	CPoint		m_ptStart,
				m_ptEnd;*/
	CPoint		m_ptZoomPos;

	CWndImageSink *		m_pImageSink;

    // paint style:
    int         m_bltMode;          // blit mode
    int         m_alignX;           // y alignment
    int         m_alignY;           // y alignment

    CRect       m_srcRect;          // source 'window' rect (as selected by user)

    // custom parameters
    double      m_zoomX;            // custom X zoom factor
    double      m_zoomY;            // custom Y zoom factor
    CPoint      m_origin;           // custom origin (in WndImage coordinates, may be  <0


        // internal vars - for "quick" recalc & change
    CRect				m_dstRect;          // true destination rect
	Bitmap *			m_pBufferImage;

	// Double buffering internal variables
	BOOL				m_bInvalidateInternalBitmap;
	Bitmap *			m_pBaseImage;

	BOOL				m_bCaptured;
	BOOL				m_bInvalidateOverlayBitmap;
	Image *				m_pOverlayImage;

	BOOL				m_bInvalidateZoomBitmap;
	Image *				m_pZoomImage;

	// Toolbar related stuff
	BOOL				m_bInvalidateToolbarBitmap;
	CButtonToolbar *	m_pButtonToolbar;
	BOOL				m_bToolbarTop;
	Image *				m_pToolbarImage;
	BOOL				m_bOnToolbar;

  public:
    enum __bltmodes
    {
      bltCustom     =       1,      // custom source, zoom
      bltNormal     =       2,      // left upper corner
      bltStretch    =       3,      // blit stretched to fit entire frame
      bltFitX       =       4,      // stretch to fit X coordinate
      bltFitY       =       5,      // stretch to fit Y cooddinate
      bltFitXY      =       6,      // stretch to fit, but keep aspect ratio
      bltFitSm      =       7,      // stretch to fit smaller (larger clipped, keeps a/r)
      bltTile       =       8,      // blit tiled (origin included)
      blt_MaxMode   =       8,      // blit tiled (origin included)

      bltNoModify   =       0,      // pass a 0 param to not modify current setting
//      bltCustom     =       1,      // custom alignment
      bltLeft       =       2,      // align to left/top
      bltTop        =       2,
      bltCenter     =       3,      // align to center
      bltRight      =       4,      // align to right / bottom
      bltBottom     =       4,
      blt_MaxAlign  =       4,
    };

	//{{AFX_VIRTUAL(CWndImage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWndImage)

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
 	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public :
	void	BitmapToScreen(CPoint & pt)
	{
		pt.x = pt.x * m_zoomX + m_origin.x;
		pt.y = pt.y * m_zoomY + m_origin.y;
	};

	void	BitmapToScreen(PointF & pt)
	{
		pt.X = pt.X * m_zoomX + m_origin.x;
		pt.Y = pt.Y * m_zoomY + m_origin.y;
	};

	void	BitmapToScreen(double & fX, double & fY)
	{
		fX = fX * (double)m_zoomX + m_origin.x;
		fY = fY * (double)m_zoomY + m_origin.y;
	};

	void	ScreenToBitmap(CPoint & pt)
	{
		pt.x = (pt.x - m_origin.x)/m_zoomX;
		pt.y = (pt.y - m_origin.y)/m_zoomY;
	};

	void	ScreenToBitmap(double & fX, double & fY)
	{
		fX = (fX - (double)m_origin.x)/(double)m_zoomX;
		fY = (fY - (double)m_origin.y)/(double)m_zoomY;
	};

	void	BitmapToScreen(CRect& rc)
	{
		rc.left		= (double)rc.left * m_zoomX + m_origin.x;
		rc.right	= (double)rc.right * m_zoomX + m_origin.x;
		rc.top		= (double)rc.top * m_zoomY + m_origin.y;
		rc.bottom	= (double)rc.bottom * m_zoomY + m_origin.y;
	};

	void	ScreenToBitmap(CRect & rc)
	{
		rc.left		= (rc.left - m_origin.x)/m_zoomX;
		rc.right	= (rc.right - m_origin.x)/m_zoomX;
		rc.top		= (rc.top - m_origin.y)/m_zoomY;
		rc.bottom	= (rc.bottom - m_origin.y)/m_zoomY;
	};

	void	NotifyModeChange()
	{
		CWnd* pwndParent = GetParent();

		if (pwndParent)
		{
			NMHDR		hdr;

			hdr.hwndFrom = GetSafeHwnd();
			hdr.idFrom   = GetDlgCtrlID();
			hdr.code	 = NM_NOTIFYMODECHANGE;
			pwndParent->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&hdr);
		}
	};

private :
	BOOL		CreateInternalBitmap();
	BOOL		CreateToolbarBitmap();
	BOOL		CreateZoomBitmap();
	BOOL		CreateBufferBitmap();
	BOOL		Draw4Corners(Graphics * pGraphics);

	BOOL		GetToolbarRect(CRect & rcToolbar)
	{
		BOOL	bResult = FALSE;

		if (m_pButtonToolbar)
		{
			CRect		rcClient;
			GetClientRect(&rcClient);
			m_pButtonToolbar->GetRect(rcToolbar);

			if (m_bToolbarTop)
				rcToolbar.OffsetRect(rcClient.Width()-rcToolbar.Width(), 0);
			else
				rcToolbar.OffsetRect(rcClient.Width()-rcToolbar.Width(), rcClient.Height()-rcToolbar.Height());

			bResult = TRUE;
		};

		return bResult;
	};

	void		ReleaseZoom();
	void		RefreshZoom();
	void		RefreshToolbar();
	void		RefreshOverlay();
	void		UpdateZoomPosition(const CPoint & pt)
	{
		m_ptZoomPos = pt;
		RefreshZoom();
	};

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDIMAGE_H__18E060C2_DE37_4419_B87A_09ACF6963828__INCLUDED_)
