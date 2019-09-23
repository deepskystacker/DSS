// GradientCtrlImpl.h: interface for the CGradientCtrlImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRADIENTCTRLIMPL_H__C28028E9_A49A_498F_8BC4_77DE5D266D04__INCLUDED_)
#define AFX_GRADIENTCTRLIMPL_H__C28028E9_A49A_498F_8BC4_77DE5D266D04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Gradient.h"

class CGradientCtrl;

class CGradientCtrlImpl
{
public:
	CGradientCtrlImpl(CGradientCtrl *owner);
protected:
	virtual ~CGradientCtrlImpl();

	void Draw(CDC *dc);
	void DrawGradient(CDC *dc);
	void DrawPegs(CDC *dc);
	void DrawSelPeg(CDC *dc, int peg);
	void DrawSelPeg(CDC *dc, CPoint point, int direction);
	void DrawPeg(CDC *dc, CPoint point, COLORREF colour, int direction);
	void DrawEndPegs(CDC *dc);

	int PointFromPos(float pos);
	float PosFromPoint(int point);
	int GetPegIndent(int index);
	int PtInPeg(CPoint point);

	void GetPegRect(int index, CRect *rect, bool right);

	void ParseToolTipLine(CString &tiptext, CPeg const& peg);
	void ShowTooltip(CPoint point, CString text);
	CString ExtractLine(CString source, int line);
	void SetTooltipText(CString text);
	void DestroyTooltip();
	void SynchronizeTooltips();

	bool IsVertical();
	int GetDrawWidth();

	HWND m_wndToolTip;
	TOOLINFO m_ToolInfo;
	CGradientCtrl *m_Owner;
	CToolTipCtrl m_ToolTipCtrl;
	int m_RectCount;
	BOOL m_LeftDownSide, m_RightUpSide;

	CPeg m_Null;

	friend class CGradientCtrl;
};

#endif // !defined(AFX_GRADIENTCTRLIMPL_H__C28028E9_A49A_498F_8BC4_77DE5D266D04__INCLUDED_)
