#pragma once
#include "Gradient.h"

#define GCW_AUTO -1

#define GC_SELCHANGE			1
#define GC_PEGMOVE				2
#define GC_PEGMOVED				3
#define GC_PEGREMOVED			4
#define GC_CREATEPEG			5
#define GC_EDITPEG				6
#define GC_CHANGE				7

struct PegNMHDR
{
	NMHDR nmhdr;
	CPeg peg;
	int index;
};

struct PegCreateNMHDR
{
	NMHDR nmhdr;
	float position;
	COLORREF colour;
};

class CGradientCtrlImpl;

/////////////////////////////////////////////////////////////////////////////
// CGradientCtrl window

class CGradientCtrl : public CWnd
{
// Construction
public:
	CGradientCtrl();
	BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID);

	enum Orientation
	{
		ForceHorizontal,
		ForceVertical,
		Auto
	};

// Attributes
public:
	int		GetGradientWidth() const {return m_Width;};
	void	SetGradientWidth(int iWidth) {ASSERT(iWidth < -1); m_Width = iWidth;};
	int		GetSelIndex() const {return m_Selected;};
	int		SetSelIndex(int iSel);
	const CPeg GetSelPeg() const;
	CGradient& GetGradient() {return m_Gradient;};
	void	SetGradient(CGradient const& src) {m_Gradient = src;}
	void	ShowTooltips(BOOL bShow = true);
	Orientation GetOrientation() const {return m_Orientation;};
	void	SetOrientation(Orientation orientation) {m_Orientation = orientation;};
	BOOL	GetPegSide(BOOL rightup) const;
	void	SetPegSide(BOOL setrightup, BOOL enable);
	void	SetTooltipFormat(const CString format);
	CString GetTooltipFormat() const;

	void	SetBackgroundColor(COLORREF crColor)
	{
		m_crBackground = crColor;
	};

// Operations
public:
	void DeleteSelected(BOOL bUpdate);
	int MoveSelected(float newpos, BOOL bUpdate);
	COLORREF SetColourSelected(COLORREF crNewColour, BOOL bUpdate);

// Internals
protected:
	BOOL RegisterWindowClass();
	void GetPegRgn(CRgn *rgn);
	void SendBasicNotification(UINT code, CPeg const& peg, int index);

	CGradient	m_Gradient;
	int			m_Width;

	int			m_Selected;
	int			m_LastPos;
	CPoint		m_MouseDown;

	BOOL		m_bShowToolTip;
	CString		m_ToolTipFormat;
	COLORREF	m_crBackground;

	enum Orientation m_Orientation;

	CGradientCtrlImpl *m_Impl;

	friend class CGradientCtrlImpl;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGradientCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGradientCtrl();

	// Generated message map functions
protected:

	//{{AFX_MSG(CGradientCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
