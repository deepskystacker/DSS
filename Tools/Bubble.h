#if !defined(AFX_BUBBLE_H__BC6DC093_229B_11D1_8F0A_00A0C93A70EC__INCLUDED_)
#define AFX_BUBBLE_H__BC6DC093_229B_11D1_8F0A_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Bubble.h : header file
//

#ifndef __AFXCMN_H__
	#include <afxcmn.h>
#endif // __AFXCMN_H__

/////////////////////////////////////////////////////////////////////////////
// CBubble window

class CBubble : public CWnd
{
// Construction
public:
	CBubble();

// Attributes
private:  
	static CString	m_strClassName;	// bubble window class name
	CStringList	m_strTextRows;		// text rows
	CString		m_strLastText;
	CPoint		m_ptLastPoint;
	CImageList	m_Images;
	int			m_iRowHeight;
	int			m_iImageWidth;
	int			m_iImageHeight;
	BOOL		m_bShadow;

// Operations
public:
	BOOL Create(CWnd* pWndParent = NULL, BOOL bShadow = FALSE);      // create the bubble window

  // request the bubble window to track with the specified text and string resource ID
	void Track(CPoint pt, const CString& string);
	void Hide();        // hide the bubble window

	void SetTextFormat (int iFormat)
	{
		m_iTextFormat = iFormat;
	}

	int GetTextFormat () const
	{
		return m_iTextFormat;
	}

	BOOL SetImageList (UINT uiId, int iImageWidth = 15, COLORREF cltTransparent = RGB (255, 0, 255));
  
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBubble)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBubble();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBubble)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LONG OnSetText(UINT, LONG lParam);
	DECLARE_MESSAGE_MAP()

	int m_iTextFormat;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUBBLE_H__BC6DC093_229B_11D1_8F0A_00A0C93A70EC__INCLUDED_)
