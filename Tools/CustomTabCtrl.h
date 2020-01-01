#pragma once

/**********************************************************************
**
**	CustomTabCtrl.h : include file
**
**	by Andrzej Markowski June 2004
**
**********************************************************************/

#include <Afxtempl.h>
#include <afxcmn.h>

#include "themeutil.h"
#include "RichToolTipCtrl.h"

#ifndef WS_EX_LAYOUTRTL
#define WS_EX_LAYOUTRTL		0x400000
#endif

// CustomTabCtrlItem

#define TAB_SHAPE1		0		//  Invisible

#define TAB_SHAPE2		1		//	 __
								//	| /
								//	|/

#define TAB_SHAPE3		2		//	|\
								//	|/

#define TAB_SHAPE4		3		//	____________
								//	\          /
								//   \________/

#define TAB_SHAPE5		4		//	___________
								//	\          \
								//	  \________/

#define RECALC_PREV_PRESSED			0
#define RECALC_NEXT_PRESSED			1
#define RECALC_ITEM_SELECTED		2
#define RECALC_RESIZED				3
#define RECALC_FIRST_PRESSED		4
#define RECALC_LAST_PRESSED			5
#define RECALC_EDIT_RESIZED			6
#define RECALC_CLOSE_PRESSED		7

#define MAX_LABEL_TEXT				30

typedef struct _CTC_NMHDR
{
    NMHDR hdr;
	int	nItem;
	TCHAR pszText[MAX_LABEL_TEXT];
	LPARAM lParam;
	RECT rItem;
	POINT ptHitTest;
	BOOL fSelected;
	BOOL fHighlighted;
} CTC_NMHDR;

class CCustomTabCtrlItem
{
	friend class CCustomTabCtrl;
private:
								CCustomTabCtrlItem(CString sText, LPARAM lParam);
	void						ComputeRgn(BOOL fOnTop);
	void						Draw(CDC& dc, CFont& font, BOOL fOnTop, BOOL fRTL);
	BOOL						HitTest(CPoint pt)			{ return (m_bShape && m_rgn.PtInRegion(pt)) ? TRUE : FALSE; }
	void						GetRegionPoints(const CRect& rc, CPoint* pts, BOOL fOnTop) const;
	void						GetDrawPoints(const CRect& rc, CPoint* pts, BOOL fOnTop) const;
	void						operator=(const CCustomTabCtrlItem &other);
private:
	CString						m_sText;
	LPARAM						m_lParam;
	CRect						m_rect;
	CRect						m_rectText;
	CRgn						m_rgn;
	BYTE						m_bShape;
	BOOL						m_fSelected;
	BOOL						m_fHighlighted;
	BOOL						m_fHighlightChanged;
	COLORREF					m_crColor;
};

// CCustomTabCtrl

// styles
#define CTCS_FIXEDWIDTH			1		// Makes all tabs the same width.
#define CTCS_FOURBUTTONS		2		// Four buttons (First, Prev, Next, Last)
#define CTCS_AUTOHIDEBUTTONS	4		// Auto hide buttons
#define CTCS_TOOLTIPS			8		// Tooltips
#define CTCS_MULTIHIGHLIGHT		16		// Multi highlighted items
#define CTCS_EDITLABELS			32		// Allows item text to be edited in place
#define CTCS_DRAGMOVE			64		// Allows move items
#define CTCS_DRAGCOPY			128		// Allows copy items
#define CTCS_CLOSEBUTTON		256     // Close button
#define CTCS_BUTTONSAFTER		512		// Button after items
#define CTCS_TOP				1024    // Location on top
#define CTCS_RIGHT				2048    // Location on right
#define CTCS_LEFT				3072	// Location on left

// hit test
#define CTCHT_ONFIRSTBUTTON		-1
#define CTCHT_ONPREVBUTTON		-2
#define CTCHT_ONNEXTBUTTON		-3
#define CTCHT_ONLASTBUTTON		-4
#define CTCHT_ONCLOSEBUTTON		-5
#define CTCHT_NOWHERE			-6

// notification messages
#define CTCN_CLICK				NM_CLICK
#define CTCN_RCLICK				NM_RCLICK
#define CTCN_DBLCLK				NM_DBLCLK
#define CTCN_RDBLCLK			NM_RDBLCLK
#define CTCN_OUTOFMEMORY		NM_OUTOFMEMORY

#define CTCN_SELCHANGE			NM_FIRST
#define CTCN_HIGHLIGHTCHANGE	NM_FIRST + 1
#define CTCN_ITEMMOVE			NM_FIRST + 2
#define CTCN_ITEMCOPY			NM_FIRST + 3
#define CTCN_LABELUPDATE		NM_FIRST + 4

#define CTCID_FIRSTBUTTON		-1
#define CTCID_PREVBUTTON		-2
#define CTCID_NEXTBUTTON		-3
#define CTCID_LASTBUTTON		-4
#define CTCID_CLOSEBUTTON		-5
#define CTCID_NOBUTTON			-6

#define CTCID_EDITCTRL			1

#define REPEAT_TIMEOUT			250

// error codes
#define CTCERR_NOERROR					0
#define CTCERR_OUTOFMEMORY				-1
#define CTCERR_INDEXOUTOFRANGE			-2
#define CTCERR_NOEDITLABELSTYLE			-3
#define CTCERR_NOMULTIHIGHLIGHTSTYLE	-4
#define CTCERR_ITEMNOTSELECTED			-5
#define CTCERR_ALREADYINEDITMODE		-6
#define CTCERR_TEXTTOOLONG				-7
#define CTCERR_NOTOOLTIPSSTYLE			-8
#define CTCERR_CREATETOOLTIPFAILED		-9
#define CTCERR_EDITNOTSUPPORTED			-10

// button states
#define BNST_INVISIBLE			0
#define BNST_NORMAL				DNHZS_NORMAL
#define BNST_HOT				DNHZS_HOT
#define BNST_PRESSED			DNHZS_PRESSED

#define CustomTabCtrl_CLASSNAME    _T("CCustomTabCtrl")  // Window class name

class CCustomTabCtrl : public CWnd
{
public:

	// Construction

	CCustomTabCtrl();
	virtual						~CCustomTabCtrl();
	BOOL						Create(UINT dwStyle, const CRect & rect, CWnd * pParentWnd, UINT nID);

	// Attributes

	int							GetItemCount() {return m_aItems.GetSize();}
	int							GetCurSel() { return m_nItemSelected; }
	int							SetCurSel(int nItem);
	int							IsItemHighlighted(int nItem);
	int							HighlightItem(int nItem, BOOL fHighlight);
	int							GetItemData(int nItem, DWORD& dwData);
	int							SetItemData(int nItem, DWORD dwData);
	int							GetItemText(int nItem, CString& sText);
	int							SetItemText(int nItem, CString sText);
	int							GetItemRect(int nItem, CRect& rect) const;
	int							SetItemTooltipText(int nItem, CString sText);
	void						SetDragCursors(HCURSOR hCursorMove, HCURSOR hCursorCopy);
	BOOL						ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags=0);
	BOOL						ModifyStyleEx(DWORD dwRemove, DWORD dwAdd, UINT nFlags=0);
	void						SetControlFont(const LOGFONT& lf, BOOL fRedraw=FALSE);
	static const LOGFONT&		GetDefaultFont() {return lf_default;}
	BOOL						IsVertical() { return (GetStyle()&CTCS_TOP && GetStyle()&CTCS_RIGHT) || GetStyle()&CTCS_RIGHT;}

	// Operations

	int							InsertItem(int nItem, CString sText, LPARAM lParam=0);
	int							DeleteItem(int nItem);
	void						DeleteAllItems();
	int							MoveItem(int nItemSrc, int nItemDst);
	int							CopyItem(int nItemSrc, int nItemDst);
	int							HitTest(CPoint pt);
	int							EditLabel(int nItem);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomTabCtrl)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CCustomTabCtrl)
	afx_msg BOOL				OnEraseBkgnd(CDC* pDC);
	afx_msg void				OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void				OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT				OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT				OnThemeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void				OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void				OnPaint();
	afx_msg void				OnMove(int x, int y);
	afx_msg void				OnSize(UINT nType, int cx, int cy);
	afx_msg void				OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void				OnTimer(UINT_PTR nIDEvent);
	afx_msg void				OnUpdateEdit();
	afx_msg void				OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT				OnSizeParent(WPARAM, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	void						RecalcLayout(int nRecalcType,int nItem);
	void						RecalcEditResized(int nOffset, int nItem);
	void						RecalcOffset(int nOffset);
	int							RecalcRectangles();
	BOOL						RegisterWindowClass();
	int							ProcessLButtonDown(int nHitTest, UINT nFlags, CPoint point);
	int							MoveItem(int nItemSrc, int nItemDst, BOOL fMouseSel);
	int							CopyItem(int nItemSrc, int nItemDst, BOOL fMouseSel);
	int							SetCurSel(int nItem, BOOL fMouseSel, BOOL fCtrlPressed);
	int							HighlightItem(int nItem, BOOL fMouseSel, BOOL fCtrlPressed);
	void						DrawGlyph(CDC& dc, CPoint& pt, int nImageNdx, int nColorNdx);
	void						DrawBk(CDC& dc, CRect& r, HBITMAP hBmp, BOOL fIsImageHorLayout, MY_MARGINS& mrgn, int nImageNdx);
	BOOL						NotifyParent(UINT code, int nItem, CPoint pt);
	int							EditLabel(int nItem, BOOL fMouseSel);

private:
	static LOGFONT				lf_default;
	static BYTE					m_bBitsGlyphs[];
	HCURSOR						m_hCursorMove;
	HCURSOR						m_hCursorCopy;
	CFont						m_Font;
	CFont						m_FontSelected;
	int							m_nItemSelected;
	int							m_nItemNdxOffset;
	int							m_nItemDragDest;
	int							m_nPrevState;
	int							m_nNextState;
	int							m_nFirstState;
	int							m_nLastState;
	int							m_nCloseState;
	int							m_nButtonIDDown;
	DWORD						m_dwLastRepeatTime;
	COLORREF					m_rgbGlyph[4];
	CBitmap						m_bmpGlyphsMono;
	HBITMAP						m_hBmpBkLeftSpin;
	HBITMAP						m_hBmpBkRightSpin;
	BOOL						m_fIsLeftImageHorLayout;
	BOOL						m_fIsRightImageHorLayout;
	MY_MARGINS					m_mrgnLeft;
	MY_MARGINS					m_mrgnRight;
	CRichToolTipCtrl			m_ctrlToolTip;
	CEdit						m_ctrlEdit;
	CArray <CCustomTabCtrlItem*,CCustomTabCtrlItem*>	m_aItems;
};
