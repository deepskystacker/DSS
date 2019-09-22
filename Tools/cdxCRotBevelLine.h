#if !defined(AFX_CDXCROTBEVELLINE_H__5B60B8C5_DA3B_11D2_BF41_000000000000__INCLUDED_)
#define AFX_CDXCROTBEVELLINE_H__5B60B8C5_DA3B_11D2_BF41_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// cdxCRotBevelLine.h : header file
//

/*
 * cdxCRotBevelLine
 * ================
 * A bevel line plus text (even rotated).
 * To use, create a static control and assign an object of this
 * class to it.
 * If the width of the control is larger than its height,
 * a horizontal bevelline will be drawn, a vertical one otherwise.
 *
 * Use the resource editor's left/center/right options to place
 * the text.
 *
 * Put a ~ in front of your text to make vertical text be rotated by
 * 270°, not by 90°.
 *
 * This is an update on the cdxCBevelLine class.
 * Posted to www.codeguru.com March 1999.
 *
 * (w)Mar.1999 Hans Bühler, codex design
 *    mailto:hans.buehler@student.hu-berlin.de
 *    V1.0
 */

class cdxCRotBevelLine : public CStatic
{
	DECLARE_DYNAMIC(cdxCRotBevelLine);

public:
	bool	m_bSunken;

public:
	cdxCRotBevelLine() : m_bSunken(true) {}
	virtual ~cdxCRotBevelLine() {}

	BOOL Create(const RECT& rect, CWnd* pParentWnd) { return CStatic::Create(nullptr,WS_CHILD|WS_VISIBLE,rect,pParentWnd); }

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(cdxCRotBevelLine)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(cdxCRotBevelLine)
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CDXCROTBEVELLINE_H__5B60B8C5_DA3B_11D2_BF41_000000000000__INCLUDED_)
