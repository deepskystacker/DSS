#if !defined(AFX_MULTISLIDER_H__AC2B03B4_ABE0_11D3_9121_006008682811__INCLUDED_)
#define AFX_MULTISLIDER_H__AC2B03B4_ABE0_11D3_9121_006008682811__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultiSlider.h : header file
//

#ifndef __AFXTEMPL_H__
	#include "afxtempl.h"
#endif
#include "bubble.h"
/////////////////////////////////////////////////////////////////////////////
// CMultiSlider window
class CMarker : public CObject
{
public:
	CMarker (/*int Idx*/) 
	{
		m_Pos = -1;
		m_mainRect.SetRectEmpty();
		m_rightRect.SetRectEmpty();
		m_leftRect.SetRectEmpty();
	}

	CMarker (const CMarker& src)
	{
		m_Pos = src.m_Pos;
		m_mainRect = src.m_mainRect;
		m_rightRect = src.m_rightRect;
		m_leftRect = src.m_leftRect;
	}

	float	m_Pos;
	CRect m_mainRect;
	CRect m_rightRect;
	CRect m_leftRect;

	const BOOL operator == (const CMarker& other)
	{
		return other.m_Pos == m_Pos && other.m_mainRect == m_mainRect
			&& other.m_rightRect == m_rightRect && other.m_leftRect == m_leftRect;
	}

	const BOOL operator != (const CMarker& other)
	{
		return !(*this == other);
	}

	const CMarker& operator=(const CMarker& src)
	{
		m_Pos = src.m_Pos;
		m_mainRect = src.m_mainRect;
		m_rightRect = src.m_rightRect;
		m_leftRect = src.m_leftRect;

		return *this;
	}
};


/*! \brief To recognize quicly if slider is vertical or not.*/
#define SLIDER_VERT		1

/*! \brief Layout mask element to decide whether displaying coloured intervals or not.*/
#define MLTSLD_DISPLAY_COLORS		0x001
/*! \brief Layout mask element to decide whether displaying numbers or not.*/
#define MLTSLD_DISPLAY_VALUES		0x002
/*! \brief Layout mask element to decide whether to use integer or float (one digit after comma) format for numbers.*/
#define MLTSLD_INTEGERS				0x004
/*! \brief Layout mask element to decide whether displaying tooltips or not.*/
#define MLTSLD_DISPLAY_TOOLTIPS		0x008
/*! \brief Layout mask element to decide whether to remove markers which are being dragged away.*/
#define MLTSLD_REMOVE_OUTGOING		0x010
/*! \brief Layout mask element to decide whether markers can exchange their order or not.*/
#define MLTSLD_PREVENT_CROSSING     0x020
/*! \brief Layout mask element to decide whether you can use right button to change interval colours.*/
#define MLTSLD_ALLOW_CHANGE_COLORS  0x040
/*! \brief Layout mask element to decide whether you can use right button to set marker positions.*/
#define MLTSLD_ALLOW_SET_POS        0x080
/*! \brief Layout mask element to set orientation of ranges (Right-to-Left and downwards or viceversa); useful for vertical controls.*/
#define MLTSLD_INVERT_RANGES        0x100
/*! \brief Layout mask element to decide whether to draw the focus rect.*/
#define MLTSLD_FOCUS_RECT	        0x200

#define MLTSLD_BLOCKED_BY_PREV  1
#define MLTSLD_BLOCKED_BY_NEXT  2

/*! \class AFX_EXT_CLASS CMultiSlider : public CSliderCtrl
	\brief Class for a slider with multiple markers.

	This object is the excellent class by Terri Braxton, enhanced (I hope!) with some more features:
	- Control now supports all possible orientations and configurations
	- You can set a \b Layout \b mask defining several features of the control, included the possibility of 
	  enabling (disabling) markers removal and marker crossing, the possibility of changing intervals run-time colours
	  (always possible in original control). You can also decide whether to use tooltips and focus rect, which sometimes
	  may be disturbing.
	- Multiple dragging is introduced: if you have two markers in your control and you click on one of them holding
	  down CTRL button, both will be dragged, keeping their distance constant.
	- A notification from the control is added, so that Parent window may consequentely update itself. 
	- The code has been slightly documented in standard Doxygen format.

	LIMITS: I haven't tested it on all possible operating systems; it is possible that layout is not perfect in all possible
	configurations. In addition to that the onPaint code is horribly huge and not modular - sorry for that, they don't pay me
	for taking care only of this control!!! ;-)*/
class /*AFX_EXT_CLASS*/ CMultiSlider : public CSliderCtrl
{
// Construction
public:
	CMultiSlider(int LayoutMask = MLTSLD_DISPLAY_VALUES | MLTSLD_DISPLAY_TOOLTIPS | MLTSLD_PREVENT_CROSSING | MLTSLD_ALLOW_SET_POS);

// Attributes
public:
	
	CObList m_Markers;
	CMarker* m_pnewMarker;
	CMarker* m_pdraggedMarker;
	CMarker* m_pSelectedMarker;
	CMarker* m_pTTMarker;
	CRect m_TTRect;
	float prevMin, prevMax;
	
	UINT uiToolTipEventId;
	int iToolTipEventDelay;
	
	int m_numTicks;
	bool fInit;
	CRect m_TTWindow;
	bool fFocus;

	float m_nAlignment;
	int m_nLabelWidth;
	CFont m_labelFont;

protected:
	/*! \brief Set of flags concerning control appearance and behaviour.*/
	int m_LayoutMask;
	/*! \brief Number of markers.*/
	int m_MarkersNum;	
	/*! \brief Flag indicating corresponding to TBS_BOTH style status.*/
	bool m_bStyleBoth;
	/*! \brief Orientation of the control, starting from TBS_BOTTOM (Idx=0) and proceeding Anticlockwise up to TBS_LEFT (Idx=3).*/
	char m_StyleIdx;
	/*! \brief Support to MLTSLD_PREVENT_CROSSING.*/
	char m_DraggedMarkerIsBlocked;
	/*! \brief Cursor used when removing markers.*/
	HCURSOR	m_hCursorDelete;
	/*! \brief Standard Windows cursor.*/
	HCURSOR	m_hCursorArrow;
	/*! \brief Quite nice tooltip.*/
	CBubble	m_wndInfo;
	/*! \brief Rectangle  */
	CRect sliderRect; 
	/*! \brief Rectangle of the area where range selection are displayed (if MLTSLD_DISPLAY_COLORS is on).*/
	CRect channelRect; 
	/*! \brief Control client area.*/
	CRect clientRect; 
	/*! \brief Dimensions of a marker.*/
	CRect thumbRect;
	/*! \brief Colors used for showing intevals.*/
	CUIntArray m_Colors;
	/*! \brief Flag allowing to invert the "direction" of the control (1 = right-to-left/downwards; -1 = left-to-right/upwards.*/
	char m_RangeDir;
	float m_RangeMax;
	float m_RangeMin;
	float m_RangeWidth;
	bool m_bDragBoth;
	float m_DragStartingPos;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiSlider)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	int GetMarkers(float *value);
	int GetMarkersNum();
	void SetMarkersNum(int MarkersNum) {m_MarkersNum = MarkersNum;}
	float GetMaxRange();
	float GetMinRange();
	bool GetRanges(float &min, float &max);
	bool SetRanges(float min, float max, int MarkersNum = 0, float *NewPos = NULL);
	bool SetColors(COLORREF *ColorsList);
	virtual ~CMultiSlider();
	void SetLayout(int LayoutMask) {m_LayoutMask = LayoutMask;}
	bool GetLayoutFeature(int Feature) {return (m_LayoutMask & Feature) ? true : false;}
	void ResetColors() {m_Colors.RemoveAll();}
	/*! \brief Added just to make the "Update slider" a bit safer ;-).*/
	void ResetAll() {m_Colors.RemoveAll(); CleanUp();}
	void Init(float Min, float Max, int TicFreq, int MarkersNum, float *InitPos, COLORREF* ColorsList, int LayoutMask = -1);
	// Generated message map functions
protected:
	
	void GetMinMax(CPoint pt);
	CRect GetMarkerRect(CMarker* pMI);
	void SetRectangles(CMarker* pMarker);
	void CleanUp();
	void ResetMarkers();

	int InsertMarker(CMarker *pMarker);
	bool RemoveLevelMarker(CMarker *pRemoved);
// Moved out of the class (sorry!)		float RoundTo(float value, short decimal);
	void Initialize();
	void SetInfoWindow(CPoint &point);
	void BuildTooltipText(CMarker* pMI, CString &text);
	CMarker* MarkerHitTest(CPoint pt);	//returns index of interval or -1 if not an interval
		
	CMarker* GetNextMarker(POSITION &Pos);
	CMarker* GetPrevMarker(POSITION &Pos);
	void AvoidCrossing(CMarker *pMarker, float *Position);
	void SetSliderValues(float *SliderMin, float *SliderWidth);
	//{{AFX_MSG(CMultiSlider)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTISLIDER_H__AC2B03B4_ABE0_11D3_9121_006008682811__INCLUDED_)
