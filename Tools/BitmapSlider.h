#if !defined(AFX_BITMAPSLIDER_H__BED36788_B60C_4C9E_AC56_FE430B93A0FD__INCLUDED_)
#define AFX_BITMAPSLIDER_H__BED36788_B60C_4C9E_AC56_FE430B93A0FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//
// CBitmapSlider v1.5
//
// It's free for everywhere - 16/September/2003 - Joon-ho Ryu
//
/////////////////////////////////////////////////////////////////////////////

#include "memdc.h" // "Flicker Free Drawing In MFC" by Keith Rule

#define WM_BITMAPSLIDER_MOVING		WM_USER + 9425
#define WM_BITMAPSLIDER_MOVED		WM_USER + 9426

class CBitmapSlider : public CStatic
{
// Construction
public:
	CBitmapSlider();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapSlider)
	//}}AFX_VIRTUAL

// Implementation
public:

	void GetRange( int &nMin, int &nMax ) { nMin = m_nMin; nMax = m_nMax; };
	int GetRangeMax() { return m_nMax; };
	int GetRangeMin() { return m_nMin; };

	int GetPos() { return m_nPos; };

	void SetRange( int nMin, int nMax, BOOL bRedraw=FALSE );
	void SetRangeMin(int nMin, BOOL bRedraw = FALSE);
	void SetRangeMax( int nMax, BOOL bRedraw = FALSE );

	void SetPos( int nPos );
	int SetPageSize( int nSize );

	BOOL SetBitmapChannel(
		UINT nChannelID, UINT nActiveID=NULL, BOOL bTransparent=FALSE,
		COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );

	BOOL SetBitmapThumb(
		UINT nThumbID, UINT nActiveID=NULL, BOOL bTransparent=FALSE,
		COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );

	void SetMargin( int nLeft, int nTop, int nRight, int nBottom );
	void SetMarginTop( int nMargin ) { m_nMarginTop = nMargin; };
	void SetMarginLeft( int nMargin ) { m_nMarginLeft = nMargin; };
	void SetMarginRight( int nMargin ) { m_nMarginRight = nMargin; };
	void SetMarginBottom( int nMargin ) { m_nMarginBottom = nMargin; };

	void SetVertical( BOOL bVertical=TRUE ) { m_bVertical = bVertical; };

	void Enable( BOOL bEnable = TRUE );

	void DrawFocusRect( BOOL bDraw = TRUE, BOOL bRedraw = FALSE );

	virtual ~CBitmapSlider();

	// Generated message map functions
protected:
	void RestoreBackground(
		CDC *pDC, int nXDst, int nYDst, int nWidth, int nHeight, CBitmap *pBmSrc);

	void CopyBackground(
		CDC *pDC, int nXSrc, int nYSrc, int nWidth, int nHeight, CBitmap *pBmDst );

	void DrawBitmap(
		CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,
		CDC* pTmpDC, int xSource, int ySource,
		CBitmap *bmMask = NULL, BOOL bTransparent = FALSE );


	void DrawTransparentBitmap(
		CDC* pDC, int xStart, int yStart, int wWidth, int wHeight,
		CDC* pTmpDC, int xSource, int ySource, CBitmap *bmMask );

	void PrepareMask(
		CBitmap* pBmpSource, CBitmap* pBmpMask,
		COLORREF clrpTransColor=0xFF000000, int iTransPixelX=0, int iTransPixelY=0 );

	int Pixel2Pos( int nPixel );
	int Pos2Pixel( int nPos );

	int m_nMax, m_nMin, m_nPos, m_nPage;

	CRect m_rect;
	int m_nWidth, m_nHeight;
	int m_nThumbWidth, m_nThumbHeight;
	int m_nMarginLeft, m_nMarginRight, m_nMarginTop, m_nMarginBottom;
	int m_nThumbBgX, m_nThumbBgY;
	int m_nMouseOffset;

	BOOL m_bVertical;
	BOOL m_bChannelActive, m_bThumbActive;
	BOOL m_bTransparentChannel, m_bTransparentThumb, m_bThumb, m_bChannel;
	BOOL m_bLButtonDown, m_bFocus, m_bFocusRect, m_bDrawFocusRect;
	BOOL m_bEnable;

	CBitmap m_bmChannel, m_bmChannelMask, m_bmChannelActive, m_bmChannelActiveMask;
	CBitmap m_bmThumb, m_bmThumbMask, m_bmThumbActive, m_bmThumbActiveMask, m_bmThumbBg;

	//{{AFX_MSG(CBitmapSlider)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	// This is CStatic method
	void SetBitmap( HBITMAP hBitmap ) {};
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPSLIDER_H__BED36788_B60C_4C9E_AC56_FE430B93A0FD__INCLUDED_)
