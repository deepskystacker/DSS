// MultiSlider.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WusDV_Util.h"
#include "MultiSlider.h"
#include "EditInterval.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiSlider


/*! \fn CMultiSlider::CMultiSlider(int LayoutMask)
	\brief Class costructor: initializes class variables anc layout; if none is passed a default layout
			is used.*/
CMultiSlider::CMultiSlider(int LayoutMask)
{
	m_hCursorDelete = ::LoadCursor (AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_DELETE));
	m_hCursorArrow = ::LoadCursor (NULL, IDC_ARROW);

	LOGFONT lf;
 	memset(&lf, 0, sizeof(LOGFONT));
 	_tcscpy(lf.lfFaceName, _T("Arial"));
 	lf.lfHeight = 12;
 	m_labelFont.CreateFontIndirect(&lf); 
	fFocus = false;
	m_nLabelWidth = 0;

	m_MarkersNum = 1;
	m_Colors.InsertAt(0, (UINT)RGB(255, 255, 255));

	m_TTRect.SetRectEmpty();

	fInit = false;

	m_LayoutMask = LayoutMask;
	m_RangeDir = 1;
	iToolTipEventDelay = 1000;
	uiToolTipEventId = 2;

	m_bDragBoth = false;
	m_pdraggedMarker = NULL;
	m_DraggedMarkerIsBlocked = 0;
	m_pnewMarker = NULL;
	m_pSelectedMarker = NULL;
	m_pTTMarker = NULL;
	m_DragStartingPos = 0.0;

	m_wndInfo.Create (this, TRUE /* Shadow */);
	m_wndInfo.SetOwner (this);
	m_wndInfo.Hide ();
}

/*! \fn CMultiSlider::~CMultiSlider()
	\brief Class destructor: resets class variables (using CleanUp()) and releases resources.*/
CMultiSlider::~CMultiSlider()
{
    m_wndInfo.DestroyWindow();
	CleanUp ();
	DestroyCursor (m_hCursorDelete);
}


#ifndef DOXYGEN_SHOULD_SKIP_THIS
BEGIN_MESSAGE_MAP(CMultiSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CMultiSlider)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif

/////////////////////////////////////////////////////////////////////////////
// CMultiSlider message handlers

/*! \fn void CMultiSlider::OnPaint() 
	\brief Draws the control components.
	
	Manages all possible orientations of the control; at the present moment the codew is not very elegant and 
	concise... sorry!*/
void CMultiSlider::OnPaint() 
{
 	CPaintDC dc(this); // device context for painting
	if(!fInit)
		Initialize();
 	
	CRect mainRect, rightRect, leftRect;
	CRect labelRect, intervalRect;
 	CString text;
	float SliderMin, SliderWidth;
 	BOOL m_bMemDC = FALSE;
   	CDC* pDC = &dc;
   	CDC dcMem;
   	CBitmap bmp;
   	CBitmap* pOldBmp = NULL;
   	float i;
   	CPen* pOldPen;
   	CPen penDark(PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW));
	int width;
	float x;
	CString str;
	CPoint pt, prevPt;
	POSITION pos;
	CRect rect;
   
   	//Try to create memory dc and bitmap
   	if(dcMem.CreateCompatibleDC(&dc) && bmp.CreateCompatibleBitmap(&dc, clientRect.Width(), 
		clientRect.Height()))
   	{
   		m_bMemDC = TRUE;
   		pOldBmp = dcMem.SelectObject(&bmp);
   		pDC = &dcMem;
   	}
   
   	pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
   
   	// Fill background, draw border and thumb
   	pDC->FillSolidRect(&clientRect, GetSysColor(COLOR_BTNFACE));
   	pDC->FillSolidRect(&channelRect, GetSysColor(COLOR_WINDOW));
   	pDC->DrawEdge(&channelRect, BDR_SUNKENOUTER|BDR_SUNKENINNER, BF_RECT);

  	if(fFocus && (m_LayoutMask & MLTSLD_FOCUS_RECT))
   	{
		pDC->DrawFocusRect(&clientRect);
   	}
   
	//Set text properties
 	TEXTMETRIC tm;
   	pDC->SetBkMode(TRANSPARENT);
   	pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
   	CFont* pOldFont = (CFont*)pDC->SelectObject(&m_labelFont);
 	pDC->GetTextMetrics (&tm);

	SetSliderValues(&SliderMin, &SliderWidth);
	//Set min/max text
	if(m_nLabelWidth == 0)//Calculate maximum label width
	{
 		for(i=GetMinRange();i<=GetMaxRange();i+=1.0f)
 		{
			if(m_LayoutMask & MLTSLD_INTEGERS)
				text.Format("%.0lf",i);
			else
				text.Format("%.1lf",i);

 			width = dc.GetTextExtent(text).cx;
 			if(width > m_nLabelWidth)
 			{
 				m_nLabelWidth = width;
 			}
 		}
	}

	// ILIC: Huge Switch for painting everything (apart from Thumb)
   	pDC->SelectObject(&penDark);
	int index;

	if(m_LayoutMask & MLTSLD_DISPLAY_COLORS)	// Color Interval
	{
		index = 0;
		intervalRect = channelRect;
		intervalRect.DeflateRect(2,2);
	}
	DWORD TicCount = GetNumTics();
	float SliderStep = SliderWidth/(TicCount-1);
	double RangeStep = (m_RangeMax-m_RangeMin)/(TicCount-1);
	int Help1, Help2;
	if (m_bStyleBoth)
	{
		if (m_StyleIdx & SLIDER_VERT)
		{
			Help1 = channelRect.right + (int)RoundTo((float)channelRect.Width()/2.0f, 0);
			Help2 = channelRect.left - (int)RoundTo((float)channelRect.Width()/2.0f, 0);			

			// Tickmarks
			// Razionalizziamo un po'..				for(i=m_RangeMin;i<=m_RangeMax;i+=(float)m_nAlignment)
			for (i=0; i<TicCount; i++)
			{
				//Calculate zero-based index of this tickmark
				x = SliderMin + i*SliderStep;

				x = RoundTo(x, 0);
   				pDC->MoveTo(channelRect.right, (int) x);
   				pDC->LineTo(Help1, (int) x);

				pDC->MoveTo(channelRect.left, (int) x);
   				pDC->LineTo(Help2, (int) x);

				if((!i || i == TicCount-1) && (m_LayoutMask & MLTSLD_DISPLAY_VALUES))
				{
					if(m_LayoutMask & MLTSLD_INTEGERS)
						str.Format("%.0lf",i*RangeStep + m_RangeMin);
					else
						str.Format("%.1lf",i*RangeStep + m_RangeMin);

					pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
					pDC->TextOut(Help1 + 1, (int) x, str);

					pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
					pDC->TextOut(Help2, (int) x, str);
				}
			}

			// Mark intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_VALUES)
			{
				for(pos=m_Markers.GetHeadPosition();pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					if(pMarker != m_pdraggedMarker || m_bDragBoth)
					{
						rect = pMarker->m_mainRect;
						x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
						x = RoundTo(x, 0);
	   					pDC->MoveTo(channelRect.right, (int) x);
   						pDC->LineTo(Help1, (int) x);

						pDC->MoveTo(channelRect.left, (int) x);
   						pDC->LineTo(Help2, (int) x);

						if(m_LayoutMask & MLTSLD_INTEGERS)
							str.Format("%.0lf",pMarker->m_Pos);
						else
							str.Format("%.1lf",pMarker->m_Pos);
						
						pDC->SetTextAlign(TA_LEFT|TA_BASELINE);
						pDC->TextOut(Help1 + 1, (int) x, str);

						pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);
						pDC->TextOut(Help2, (int) x, str);

					}
				}
			}

			// Color Intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_COLORS)
			{
				x = SliderMin;
				intervalRect.top = (int)RoundTo(x, 0) + ((m_LayoutMask & MLTSLD_INVERT_RANGES) ? 2 : -1);
				for(pos=m_Markers.GetHeadPosition(); pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					intervalRect.bottom = pMarker->m_mainRect.bottom - (int) RoundTo((float) pMarker->m_mainRect.Height()/2.0f, 0);
					pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
					intervalRect.top = intervalRect.bottom;
					index++;
				}
				//Last interval
				intervalRect.bottom = (long) (SliderMin + SliderWidth) + (m_LayoutMask & MLTSLD_INVERT_RANGES ? -2 : +2);
				pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
			}
		}
		else	// TBS_HORZ | TBS_BOTH
		{
			Help1 = channelRect.bottom + (int)RoundTo((float)channelRect.Height()/2.0f, 0);
			Help2 = channelRect.top - (int)RoundTo((float)channelRect.Height()/2.0f, 0);
			// Tickmarks
			// Razionalizziamo un po'..				for(i=m_RangeMin;i<=m_RangeMax;i+=(float)m_nAlignment)
			for (i=0; i<TicCount; i++)
			{
				//Calculate zero-based index of this tickmark
				x = SliderMin + i*SliderStep;
				x = RoundTo(x, 0);
   				pDC->MoveTo((int)x, channelRect.bottom);
   				pDC->LineTo((int)x, Help1);

				pDC->MoveTo((int)x, channelRect.top);
   				pDC->LineTo((int)x, Help2);

				if((!i || i == TicCount-1) && (m_LayoutMask & MLTSLD_DISPLAY_VALUES))
				{
					if(m_LayoutMask & MLTSLD_INTEGERS)
						str.Format("%.0lf",i*RangeStep + m_RangeMin);
					else
						str.Format("%.1lf",i*RangeStep + m_RangeMin);

					pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
					pDC->TextOut((int)x, clientRect.bottom, str);
					pDC->SetTextAlign(TA_CENTER|TA_BASELINE);
					pDC->TextOut((int)x, Help2, str);
				}
			}

			// Mark intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_VALUES)
			{
				for(pos=m_Markers.GetHeadPosition();pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					if(pMarker != m_pdraggedMarker || m_bDragBoth)
					{
						rect = pMarker->m_mainRect;
						x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
						x = RoundTo(x, 0);
   						pDC->MoveTo((int)x, channelRect.bottom);
   						pDC->LineTo((int)x, Help1);

						pDC->MoveTo((int)x, channelRect.top);
   						pDC->LineTo((int)x, Help2);

						if(m_LayoutMask & MLTSLD_INTEGERS)
							str.Format("%.0lf",pMarker->m_Pos);
						else
							str.Format("%.1lf",pMarker->m_Pos);

						pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
						pDC->TextOut((int)x, clientRect.bottom, str);
						pDC->SetTextAlign(TA_CENTER|TA_BASELINE);
						pDC->TextOut((int)x, Help2, str);
					}
				}
			}

			// Color Intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_COLORS)
			{
				x = SliderMin;
				intervalRect.left = (int)RoundTo(x, 0);
				for(pos=m_Markers.GetHeadPosition(); pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					intervalRect.right = pMarker->m_mainRect.right - (int) RoundTo((float) pMarker->m_mainRect.Width()/2.0f, 0);
					pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
					intervalRect.left = intervalRect.right;
					index++;
				}
				//Last interval
				intervalRect.right = (long) (SliderMin + SliderWidth);
				pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
			}
		}

	}
	else
	{
		switch(m_StyleIdx)
		{
		case 0:	// *** POINTING DOWN ***
			pDC->SetTextAlign(TA_CENTER|TA_BOTTOM);
			Help1 = channelRect.bottom + (int)RoundTo((float)channelRect.Height()/2.0f, 0);
			// Tickmarks
			for (i=0; i<TicCount; i++)
			{
	
				//Calculate zero-based index of this tickmark
				x = SliderMin + i*SliderStep;
				x = RoundTo(x, 0);
   				pDC->MoveTo((int)x, channelRect.bottom);
   				pDC->LineTo((int)x, Help1);
				if((!i || i == TicCount-1) && (m_LayoutMask & MLTSLD_DISPLAY_VALUES))
				{
					if(m_LayoutMask & MLTSLD_INTEGERS)
						str.Format("%.0lf",i*RangeStep + m_RangeMin);
					else
						str.Format("%.1lf",i*RangeStep + m_RangeMin);

					pDC->TextOut((int)x, clientRect.bottom, str);
				}
			}

			// Mark intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_VALUES)
			{
				for(pos=m_Markers.GetHeadPosition();pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					if(pMarker != m_pdraggedMarker || m_bDragBoth)
					{
						rect = pMarker->m_mainRect;
						x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
						x = RoundTo(x, 0);
   						pDC->MoveTo((int)x, channelRect.bottom);
   						pDC->LineTo((int)x, Help1);

						if(m_LayoutMask & MLTSLD_INTEGERS)
							str.Format("%.0lf",pMarker->m_Pos);
						else
							str.Format("%.1lf",pMarker->m_Pos);

						pDC->TextOut((int)x, clientRect.bottom, str);
					}
				}
			}

			// Color Intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_COLORS)
			{
				x = SliderMin;
				intervalRect.left = (int)RoundTo(x, 0);
				for(pos=m_Markers.GetHeadPosition(); pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					intervalRect.right = (int)RoundTo(pMarker->m_leftRect.left + 6.0f, 0);
					pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
					intervalRect.left = pMarker->m_leftRect.right;
					index++;
				}
				//Last interval
				intervalRect.right = (long) (SliderMin + SliderWidth);
				pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
			}

			break;
		case 1:	// *** POINTING RIGHT ***
			Help1 = channelRect.right + (int)RoundTo((float)channelRect.right/3.0f, 0);
			pDC->SetTextAlign(TA_LEFT|TA_BASELINE);

			// Tickmarks
			for (i=0; i<TicCount; i++)
			{
				//Calculate zero-based index of this tickmark
				x = SliderMin + i*SliderStep;
				x = RoundTo(x, 0);
   				pDC->MoveTo(channelRect.right, (int) x);
   				pDC->LineTo(Help1, (int) x);
				if((!i || i == TicCount-1) && (m_LayoutMask & MLTSLD_DISPLAY_VALUES))
				{
					if(m_LayoutMask & MLTSLD_INTEGERS)
						str.Format("%.0lf",i*RangeStep + m_RangeMin);
					else
						str.Format("%.1lf",i*RangeStep + m_RangeMin);

					pDC->TextOut(Help1 + 1, (int) x, str);
				}
			}

			// Mark intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_VALUES)
			{
				for(pos=m_Markers.GetHeadPosition();pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					if(pMarker != m_pdraggedMarker || m_bDragBoth)
					{
						rect = pMarker->m_mainRect;
						x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
						x = RoundTo(x, 0);
	   					pDC->MoveTo(channelRect.right, (int) x);
   						pDC->LineTo(Help1, (int) x);
						
						if(m_LayoutMask & MLTSLD_INTEGERS)
							str.Format("%.0lf",pMarker->m_Pos);
						else
							str.Format("%.1lf",pMarker->m_Pos);

						pDC->TextOut(Help1 + 1, (int) x, str);
					}
				}
			}

			// Color Intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_COLORS)
			{
				x = SliderMin;
				intervalRect.top = (int)RoundTo(x, 0) + ((m_LayoutMask & MLTSLD_INVERT_RANGES) ? 2 : -1);
				for(pos=m_Markers.GetHeadPosition(); pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					intervalRect.bottom = pMarker->m_leftRect.bottom;
					pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
					intervalRect.top = pMarker->m_leftRect.bottom;
					index++;
				}
				//Last interval
				intervalRect.bottom = (long) (SliderMin + SliderWidth) + (m_LayoutMask & MLTSLD_INVERT_RANGES ? -2 : +2);
				pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
			}

			break;
		case 2:	// *** POINTING UP ***
			Help1 = channelRect.top - (int)RoundTo((float)channelRect.Height()/2.0f, 0);			
			pDC->SetTextAlign(TA_CENTER|TA_BASELINE);

			// Tickmarks
			for (i=0; i<TicCount; i++)
			{
				//Calculate zero-based index of this tickmark
				x = SliderMin + i*SliderStep;
				x = RoundTo(x, 0);
   				pDC->MoveTo((int)x, channelRect.top);
   				pDC->LineTo((int)x, Help1);
				if((!i || i == TicCount-1) && (m_LayoutMask & MLTSLD_DISPLAY_VALUES))
				{
					if(m_LayoutMask & MLTSLD_INTEGERS)
						str.Format("%.0lf",i*RangeStep + m_RangeMin);
					else
						str.Format("%.1lf",i*RangeStep + m_RangeMin);

					pDC->TextOut((int)x, Help1, str);
				}
			}

			// Mark intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_VALUES)
			{
				for(pos=m_Markers.GetHeadPosition();pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					if(pMarker != m_pdraggedMarker || m_bDragBoth)
					{
						rect = pMarker->m_mainRect;
						x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
						x = RoundTo(x, 0);
   						pDC->MoveTo((int)x, channelRect.top);
   						pDC->LineTo((int)x, Help1);
						
						if(m_LayoutMask & MLTSLD_INTEGERS)
							str.Format("%.0lf",pMarker->m_Pos);
						else
							str.Format("%.1lf",pMarker->m_Pos);

						pDC->TextOut((int)x, Help1, str);
					}
				}
			}

			// Color Intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_COLORS)
			{
				x = SliderMin;
				intervalRect.left = (int)RoundTo(x, 0);
				for(pos=m_Markers.GetHeadPosition(); pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					intervalRect.right = (int)RoundTo(pMarker->m_rightRect.left + 6.0f, 0);
					pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
					intervalRect.left = pMarker->m_rightRect.right;
					index++;
				}
				//Last interval
				intervalRect.right = (long) (SliderMin + SliderWidth);
				pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
			}

			break;
		case 3:	// *** POINTING LEFT ***
			Help1 = channelRect.left - (int)RoundTo((float)channelRect.Width()/2.0f, 0);
			pDC->SetTextAlign(TA_RIGHT|TA_BASELINE);

			// Tickmarks
			for (i=0; i<TicCount; i++)
			{
				//Calculate zero-based index of this tickmark
				x = SliderMin + i*SliderStep;
				x = RoundTo(x, 0);
   				pDC->MoveTo(channelRect.left, (int) x);
   				pDC->LineTo(Help1, (int) x);
				if((!i || i == TicCount-1) && (m_LayoutMask & MLTSLD_DISPLAY_VALUES))
				{
					if(m_LayoutMask & MLTSLD_INTEGERS)
						str.Format("%.0lf",i*RangeStep + m_RangeMin);
					else
						str.Format("%.1lf",i*RangeStep + m_RangeMin);

					pDC->TextOut(Help1, (int) x, str);
				}
			}


			// Mark intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_VALUES)
			{
				for(pos=m_Markers.GetHeadPosition();pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					if(pMarker != m_pdraggedMarker || m_bDragBoth)
					{
						rect = pMarker->m_mainRect;
						x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
						x = RoundTo(x, 0);
   						pDC->MoveTo(channelRect.left, (int) x);
   						pDC->LineTo(Help1, (int) x);
						
						if(m_LayoutMask & MLTSLD_INTEGERS)
							str.Format("%.0lf",pMarker->m_Pos);
						else
							str.Format("%.1lf",pMarker->m_Pos);

						pDC->TextOut(Help1, (int) x, str);
					}
				}
			}

			// Color Intervals
			if (m_LayoutMask & MLTSLD_DISPLAY_COLORS)
			{
				x = SliderMin;
				intervalRect.top = (int)RoundTo(x, 0) + ((m_LayoutMask & MLTSLD_INVERT_RANGES) ? 2 : -1);
				for(pos=m_Markers.GetHeadPosition(); pos != NULL;)
				{
					CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
					ASSERT(pMarker != NULL);
					intervalRect.bottom = pMarker->m_rightRect.bottom;
					pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
					intervalRect.top = pMarker->m_rightRect.bottom;
					index++;
				}
				//Last interval
				intervalRect.bottom = (long) (SliderMin + SliderWidth) + (m_LayoutMask & MLTSLD_INVERT_RANGES ? -2 : +2);
				pDC->FillSolidRect(intervalRect, m_Colors.GetAt(index));
			}

			break;
		}
	}
	// ILIC: Huge Switch for painting everything (apart from Thumb) (END)

	// Draw thumb in the specific way depending on style (ILIC)
	for(pos=m_Markers.GetHeadPosition();pos != NULL;)
	{
		CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
		ASSERT(pMarker != NULL);
		mainRect = pMarker->m_mainRect;

		if (m_bStyleBoth)
	  		pDC->DrawEdge(&mainRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_MIDDLE|BF_LEFT|BF_TOP|BF_RIGHT|BF_BOTTOM);
		else
		{
			rightRect = pMarker->m_rightRect;
			leftRect = pMarker->m_leftRect;
	  		pDC->DrawEdge(&mainRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_MIDDLE|BF_LEFT|BF_TOP|BF_RIGHT);

			switch (m_StyleIdx)
			{
			case 0:	// Pointing down
				pDC->DrawEdge(&leftRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDTOPLEFT|BF_MIDDLE);
				pDC->DrawEdge(&rightRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDBOTTOMLEFT|BF_MIDDLE);
				pDC->DrawEdge(&mainRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_MIDDLE|BF_LEFT|BF_TOP|BF_RIGHT);
				break;
			case 1: // Pointing right
				pDC->DrawEdge(&leftRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDBOTTOMLEFT|BF_MIDDLE);
				pDC->DrawEdge(&rightRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDBOTTOMRIGHT|BF_MIDDLE);
				pDC->DrawEdge(&mainRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_MIDDLE|BF_LEFT|BF_TOP|BF_BOTTOM);
				break;
			case 2: // Pointing up
				pDC->DrawEdge(&leftRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDBOTTOMRIGHT|BF_MIDDLE);
				pDC->DrawEdge(&rightRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDTOPRIGHT|BF_MIDDLE);
				pDC->DrawEdge(&mainRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_MIDDLE|BF_LEFT|BF_BOTTOM|BF_RIGHT);
				break;
			case 3:	// Pointing left
				pDC->DrawEdge(&leftRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDTOPRIGHT|BF_MIDDLE);
				pDC->DrawEdge(&rightRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_DIAGONAL_ENDTOPLEFT|BF_MIDDLE);
				pDC->DrawEdge(&mainRect, BDR_RAISEDOUTER|BDR_RAISEDINNER, BF_MIDDLE|BF_BOTTOM|BF_TOP|BF_RIGHT);
				break;
			}
		}
	}
	// Draw thumb in the specific way depending on style (ILIC)

	//Restore DC settings
	pDC->SelectObject(pOldPen);

	if(m_bMemDC)
	{
		CRect clipRect;
		dc.GetClipBox(&clipRect);
		dc.BitBlt(clipRect.left, clipRect.top, clipRect.Width(), clipRect.Height(),
   			&dcMem, clipRect.left, clipRect.top, SRCCOPY);
		dcMem.SelectObject(pOldBmp);
	}	
	// Do not call CSliderCtrl::OnPaint() for painting messages
}

/*! \fn void CMultiSlider::OnSetFocus(CWnd* pOldWnd) 
	\brief Sets fFocus to true for eventually drawing Focus Rect.*/
void CMultiSlider::OnSetFocus(CWnd* pOldWnd) 
{
	CSliderCtrl::OnSetFocus(pOldWnd);
	
	fFocus = true;
	
}

/*! \fn void CMultiSlider::OnSetFocus(CWnd* pOldWnd) 
	\brief Sets fFocus to false for eventually (not) drawing Focus Rect.*/
void CMultiSlider::OnKillFocus(CWnd* pNewWnd) 
{
	CSliderCtrl::OnKillFocus(pNewWnd);
	
	fFocus = false;
	
}

/*! \fn void CMultiSlider::OnLButtonDown(UINT nFlags, CPoint point) 
	\brief Depending on Layout configuration, manages different possible actions.

	Clicking on a marker allows the user to start dragging it; if <Ctrl> is pressed and there are only 
	two markers, both will be dragged contemporarily. If one clicks out of the markers area, a new
	markers is created - but only if the total number of markers [set with Init()] has not been reached yet.*/
void CMultiSlider::OnLButtonDown(UINT nFlags, CPoint point) 
{
	float SliderMin, SliderWidth;
	CRect tempRect;
	CString strText;
	CPoint pt;
	float x;

	SetSliderValues(&SliderMin, &SliderWidth);

	m_wndInfo.Hide ();
	m_TTRect.SetRectEmpty ();

	if(sliderRect.PtInRect(point))
	{
		m_pdraggedMarker = MarkerHitTest(point);
		int ActualMarkersNum = m_Markers.GetCount();
		if(m_pdraggedMarker == NULL && ActualMarkersNum < m_MarkersNum)	//Adding interval
		{
			// ILIC Just slightly different... :-)
			if (m_StyleIdx & SLIDER_VERT)
				x = ((float)(point.y - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;
			else
				x = ((float)(point.x - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;

			m_pnewMarker = new CMarker/*(UpdateIndexes(x))*/;	// MLTSLD_PREVENT:CROSSING SUPPORT...

			x = RoundTo(x, 1);
			m_pnewMarker->m_Pos = x;
			m_DragStartingPos = x;
			SetRectangles(m_pnewMarker);
		}
		else
		{
			// Multiple dragging feature (at the moment only for two markers)
			if ((nFlags & MK_CONTROL) && /*ActualMarkersNum > 1*/ ActualMarkersNum == 2)
				m_bDragBoth	= true;

			if (m_pdraggedMarker)
				m_DragStartingPos = m_pdraggedMarker->m_Pos;

			GetMinMax(point);
		}
		
		if (m_LayoutMask & MLTSLD_DISPLAY_TOOLTIPS)
		{
			BuildTooltipText(m_pdraggedMarker, strText);

			::GetCursorPos (&pt);
			pt += CPoint (
						GetSystemMetrics (SM_CXCURSOR) / 2, 
						GetSystemMetrics (SM_CYCURSOR) / 2);
			m_wndInfo.Track (pt, strText);
		}
	}
	
	CSliderCtrl::OnLButtonDown(nFlags, point);
}

/*! \fn void CMultiSlider::OnLButtonUp(UINT nFlags, CPoint point) 
	\brief Manages the end of an (eventual) marker dragging.

	Checks whether a Marker is being dragged; if so, following actions can be taken depending on layout
	and marker position:

	\arg If marker has beeen dragged out of the control and MLTSLD_REMOVE_OUTGOING is included in the layout, the
		 marker is deleted, otherwise it is simply fixed to the correct extreme of the ChannelRect.
	\arg If the dragged marker has position equal to the minimum one and that marker is not the first, dragging
		is considered invalid and marker is set back to its original position (otherwise it would be impossible to 
		get control of it later, due to precedence with which markers are "captured").

	Other features (such as preventing markers crossing) are also considered, even if the greatest of the management
	is done in OnMouseMove().
	*/
void CMultiSlider::OnLButtonUp(UINT nFlags, CPoint point) 
{
	float x;
	float SliderMin, SliderWidth;

	SetFocus ();
	m_wndInfo.Hide ();
	::SetCursor (m_hCursorArrow);

	if (m_pdraggedMarker != NULL)
	{
		ReleaseCapture();
	}

	// Disable multiple dragging
	m_bDragBoth = false;

	SetSliderValues(&SliderMin, &SliderWidth);

	if (m_pdraggedMarker && (m_LayoutMask & MLTSLD_PREVENT_CROSSING) && m_DraggedMarkerIsBlocked)
	{
		POSITION MyPos = m_Markers.Find((CMarker *) m_pdraggedMarker);
		if (m_DraggedMarkerIsBlocked == MLTSLD_BLOCKED_BY_PREV) 
			x = ((CMarker *) GetPrevMarker(MyPos))->m_Pos;
		else
			x = ((CMarker *) GetNextMarker(MyPos))->m_Pos;
	}
	else
	{
		// ILIC Just slightly different... :-)
		if (m_StyleIdx & SLIDER_VERT)
			x = ((float)(point.y - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;
		else
			x = ((float)(point.x - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;
		x = RoundTo(x, 1);
	}
	
	if(m_pnewMarker != NULL && channelRect.PtInRect(point))
	{
		if(x <= m_RangeMax && x >= m_RangeMin)
		{
			InsertMarker(m_pnewMarker);
			Invalidate();
		}
		m_pnewMarker = NULL;
	}
	else if(m_pdraggedMarker != NULL)
	{
		if (m_LayoutMask & MLTSLD_REMOVE_OUTGOING)
		{
			if(!clientRect.PtInRect(point))
			{	
				RemoveLevelMarker(m_pdraggedMarker);
				Invalidate();
				x = m_RangeMax + 1; // So that it won't get into final if ;-)
			}
		}
		else
		{
			if (x < m_RangeMin)
				x = m_RangeMin;
					
			if (x > m_RangeMax)
				x = m_RangeMax;

			// We need to avoid superposition with first marker in the origin
			POSITION MyPos = m_Markers.Find(m_pdraggedMarker);
			if (m_Markers.GetCount() > 1)
			{
				if (MyPos != m_Markers.GetHeadPosition())
					if (x == m_RangeMin)
						x = m_DragStartingPos;
			}
			
		}

		if(x <= m_RangeMax && x >= m_RangeMin)
		{
			CString strText;
			CPoint pt;
			CMarker* pMarker = new CMarker();
			pMarker->m_Pos = x;
			SetRectangles(pMarker);
			RemoveLevelMarker(m_pdraggedMarker);
			InsertMarker(pMarker);
			Invalidate();
			
			if (m_LayoutMask & MLTSLD_DISPLAY_TOOLTIPS)
			{
				BuildTooltipText(pMarker, strText);

				::GetCursorPos (&pt);
				pt += CPoint (
							GetSystemMetrics (SM_CXCURSOR) / 2, 
							GetSystemMetrics (SM_CYCURSOR) / 2);
				m_wndInfo.Track (pt, strText);
			}
			NMHDR nmhdr;
			nmhdr.hwndFrom = m_hWnd;
			nmhdr.idFrom = GetDlgCtrlID();
			nmhdr.code = TB_THUMBPOSITION;
			GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM) &nmhdr);
		}
		m_pdraggedMarker = NULL;
	}
	
	CSliderCtrl::OnLButtonUp(nFlags, point);
}

/*! \fn CMarker* CMultiSlider::MarkerHitTest(CPoint pt)
	\brief Checks if the input point is inside the area of one of the markers and returns a pointer to it (or NULL
	if no matching marker is found).*/
CMarker* CMultiSlider::MarkerHitTest(CPoint pt)
{
	float x;
	CRect mainRect;
	float SliderMin, SliderWidth;
	POSITION pos;

	SetSliderValues(&SliderMin, &SliderWidth);

	if(m_Markers.GetCount() == 0)
		return NULL;

	for(pos = m_Markers.GetHeadPosition();pos!=NULL;)
	{
		CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
		x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
		mainRect = pMarker->m_rightRect;
		mainRect += pMarker->m_mainRect;
		mainRect += pMarker->m_leftRect;
		if(pMarker->m_rightRect.PtInRect(pt)||pMarker->m_leftRect.PtInRect(pt)
			||pMarker->m_mainRect.PtInRect(pt))
		{
			return pMarker;
		}
	}

	return NULL;
}

/*! \fn void CMultiSlider::OnMouseMove(UINT nFlags, CPoint point) 
	\brief Manages the markers dragging.

	Changes the cursor if a marker ig going to be dragged away, blocks the dragged marker if MLTSLD_PREVENT_CROSSING
	is active and there is superposition of two objects... and finally sets new position of the marker (or othe the TWO
	markers in case of multiple dragging.*/
void CMultiSlider::OnMouseMove(UINT nFlags, CPoint point) 
{
	CSliderCtrl::OnMouseMove(nFlags, point);
	float x;
	float SliderMin, SliderWidth;
	CRect invalidRect;

	if(m_pdraggedMarker == NULL)
	{
		SetInfoWindow (point);
		return;
	}

	if(!clientRect.PtInRect(point) && (m_LayoutMask & MLTSLD_REMOVE_OUTGOING))
	{
		m_wndInfo.Hide ();
		::SetCursor (m_hCursorDelete);
		return;
	}
	else
		::SetCursor (m_hCursorArrow);

	SetSliderValues(&SliderMin, &SliderWidth);

	CRect oldRect, newRect;

	if(m_pdraggedMarker != NULL)
	{
		if(sliderRect.PtInRect(point) || !(m_LayoutMask & MLTSLD_REMOVE_OUTGOING))
		{
			if (m_StyleIdx & SLIDER_VERT)
				x = ((float)(point.y - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + (float)m_RangeMin;
			else
				x = ((float)(point.x - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + (float)m_RangeMin;

			x = RoundTo(x, 1);

			if ((m_LayoutMask & MLTSLD_PREVENT_CROSSING) && GetMarkersNum()>1)
				AvoidCrossing(m_pdraggedMarker, &x);

			bool bInRange = true;
			float DeltaPos = x - m_pdraggedMarker->m_Pos;	// eventually useless

			if (m_bDragBoth)
			{
				for (POSITION pos = m_Markers.GetHeadPosition (); pos != NULL;)
				{
					CMarker *pTmpMarker = (CMarker*) m_Markers.GetNext (pos);
						
					float TmpNewPos = pTmpMarker->m_Pos + DeltaPos;

					if (TmpNewPos < m_RangeMin || TmpNewPos > m_RangeMax)
					{
						bInRange = false;
						break;
					}
				}

			}
			else
				bInRange = (x >= m_RangeMin && x <= m_RangeMax);
		
			if (bInRange)
			{
				CMarker *pToMove = m_pdraggedMarker;

				do
				{
					CMarker newMarker/*(pToMove->m_Index)*/;
					oldRect = pToMove->m_mainRect;
					newMarker.m_Pos = pToMove->m_Pos + DeltaPos;
					SetRectangles(&newMarker);
					newRect = newMarker.m_mainRect;
					
					pToMove->m_leftRect = newMarker.m_leftRect;
					pToMove->m_rightRect = newMarker.m_rightRect;
					pToMove->m_mainRect = newMarker.m_mainRect;
					pToMove->m_Pos = newMarker.m_Pos;
					if(oldRect != newRect)
					{
						invalidRect.top = min(oldRect.top, newRect.top) -1;	// Text could be a little bit higher...
						invalidRect.bottom = max(oldRect.bottom, newRect.bottom);
						invalidRect.left = min(oldRect.left, newRect.left) - 6;
						invalidRect.right = max(oldRect.right, newRect.right) + 6;
					
						if (m_StyleIdx & SLIDER_VERT)
						{
							invalidRect.left = clientRect.left;
							invalidRect.right = clientRect.right;
						}
						else
						{
							invalidRect.top = clientRect.top;
							invalidRect.bottom = clientRect.bottom;
						}
					
						InvalidateRect(invalidRect);
						UpdateWindow();
						SetInfoWindow (point);
					}

					// We are not using multiple dragging, or we are over with it
					if (!m_bDragBoth || pToMove != m_pdraggedMarker)
						pToMove = NULL;
					else
					{
						for (POSITION pos = m_Markers.GetHeadPosition (); pos != NULL;)
						{
							pToMove = (CMarker*) m_Markers.GetNext (pos);
							
							// We are looking fo the other one...
							if (pToMove != m_pdraggedMarker)
								break;
						}
					}
				} while (pToMove);
			}
		}
	}
}

/*! \fn void CMultiSlider::BuildTooltipText(CMarker* pMI, CString &text)
	\brief Builds tooltip text - quite straightforward, isn't it?!? ;-).*/
void CMultiSlider::BuildTooltipText(CMarker* pMI, CString &text)
{
	if(pMI == NULL)
		return;

	text = _T("");
 
	if(m_LayoutMask & MLTSLD_INTEGERS)
		text.Format(_T("%.0f"), pMI->m_Pos);
	else
		text.Format(_T("%.1f"), pMI->m_Pos);
}

/*! \fn void CMultiSlider::SetInfoWindow(CPoint &point)
	\brief Builds the window for the tooltip, if this feature is included in the layout.*/
void CMultiSlider::SetInfoWindow(CPoint &point)
{
	CRect rectI;
	BOOL bShowInfo = TRUE;
	CMarker* pMI;

	if (m_pdraggedMarker != NULL && (m_LayoutMask & MLTSLD_DISPLAY_TOOLTIPS) && !m_bDragBoth)
	{
		CString strText;
		BuildTooltipText (m_pdraggedMarker, strText);
		m_wndInfo.SetWindowText (strText);
		m_wndInfo.ShowWindow(SW_SHOWNOACTIVATE);
		m_wndInfo.Invalidate ();
		m_wndInfo.UpdateWindow ();
		return;
	}

	if (m_TTRect.PtInRect (point))
		return;

	m_TTRect.SetRectEmpty();
	if ((pMI = MarkerHitTest(point)) == NULL)
		bShowInfo = FALSE;
	else
	{
		rectI = GetMarkerRect(pMI);
		bShowInfo = rectI.PtInRect (point);
	}

	if (!bShowInfo)
	{
		ReleaseCapture ();
		m_wndInfo.Hide ();

		KillTimer (uiToolTipEventId);
		rectI.SetRectEmpty ();
	}
	else
		SetTimer (uiToolTipEventId, iToolTipEventDelay, NULL);

	m_TTRect = rectI;
	m_pTTMarker = pMI;
}


/*! \fn void CMultiSlider::OnTimer(UINT nIDEvent) 
	\brief For displaying Tooltips with a reasonable frequency.*/
void CMultiSlider::OnTimer(UINT nIDEvent) 
{
 	CPaintDC dc(this);
	CPoint pt;

	if (m_pTTMarker != NULL && (m_LayoutMask & MLTSLD_DISPLAY_TOOLTIPS) && !m_bDragBoth)
	{
		CString strText;
		TEXTMETRIC tm;
		CSize length;
		int width;

		dc.GetTextMetrics(&tm);
		int nTextHeight = tm.tmHeight + 2;
		int nTextWidth = tm.tmMaxCharWidth + 2;

		if(m_LayoutMask & MLTSLD_INTEGERS)
			strText.Format("%.0f", m_pTTMarker->m_Pos);
		else
			strText.Format("%.1f", m_pTTMarker->m_Pos);

		length = dc.GetTextExtent(strText);
		width = length.cx;
		BuildTooltipText(m_pTTMarker, strText);

		CPoint ptCursor;
		::GetCursorPos (&ptCursor);
		ptCursor += CPoint (
					GetSystemMetrics (SM_CXCURSOR) / 2, 
					GetSystemMetrics (SM_CYCURSOR) / 2);
		m_wndInfo.Track (ptCursor, strText);

		SetCapture ();
	}
	else
	{
		m_wndInfo.Hide ();
		m_TTRect.SetRectEmpty ();
	}

	KillTimer (uiToolTipEventId);	

	CSliderCtrl::OnTimer(nIDEvent);
}

/*! \fn void CMultiSlider::Initialize()
	\brief Sets various rects (overwriting base class configuration and checks for markers intial positions.*/
void CMultiSlider::Initialize()
{
	float max, min, width;
	CPoint pt;
	POSITION pos;
	
	CPaintDC dc(this);
	dc.SetMapMode(MM_TEXT);

	//Get control rect, channel rect and thumb rect
   	GetClientRect(&clientRect);
   	GetChannelRect(&channelRect);
   	GetThumbRect(&thumbRect);

	// ILIC: Retrieve control style
	DWORD Style = GetStyle();

	m_StyleIdx = 0; 
	long Correction;	// To make it look perfect ;-)

	if (Style & TBS_VERT)
	{
		m_StyleIdx += 1;
		
		// ACHTUNG: Don't know why, channelRect is not turned automatically... (always horizontal) :-(
		if (Style & TBS_BOTH)
			Correction = (long) RoundTo((float)clientRect.Width()/2.0f,0) - (long) RoundTo((float)channelRect.Height()/2.0f,0) - channelRect.top;
		else if (Style & TBS_LEFT)
			Correction = (clientRect.right-4) - channelRect.bottom;
		else 
			Correction = -2;
	
		CRect TmpRect;
		TmpRect.top = channelRect.left;
		TmpRect.bottom = channelRect.right;
		TmpRect.left = channelRect.top + Correction;
		TmpRect.right = channelRect.bottom + Correction;

		thumbRect.left += Correction;
		thumbRect.right += Correction;

		channelRect = TmpRect;
	}
	
	if (Style & TBS_BOTH)
	{
		m_bStyleBoth = true;	// if TBS_BOTH is enabled we don't need any more info about style
		
		if (!(Style & TBS_VERT))	// Also TBS_HORZ | TBS_BOTH needs its own correction...
		{
			Correction = (long) RoundTo((float)clientRect.Height()/2.0f,0) - (long) RoundTo((float)channelRect.Height()/2.0f,0) - channelRect.top;

			channelRect.top += Correction;
			channelRect.bottom += Correction;

			thumbRect.top += Correction;
			thumbRect.bottom += Correction;
		}
	}
	else
	{
		m_bStyleBoth = false;

		if (Style & TBS_TOP)	
		{
			m_StyleIdx +=2;

			// TBS_TOP and TBS_LEFT are the same; it is a pro when we don't need to
			// distinguish between them (like for the previous line) but here we have 
			// to check again to administer suitable correction
			if (!(Style & TBS_VERT))	// TBS_HORZ is 0...
			{
				channelRect.bottom += 4;	// Correction for TBS_TOP
				channelRect.top += 4;

				thumbRect.bottom += 4;
				thumbRect.top += 4;
			}
		}
	}
	// ILIC: Retrieve control style (END)

	min = m_RangeMin;
	max = m_RangeMax;

	//Get tickmark information
 	m_numTicks = GetNumTics();
   	m_nAlignment = (int)((max - min) / (GetNumTics()-1) + .5);
	width = max-min + 1;

	sliderRect = channelRect;
	sliderRect.DeflateRect(2, 0);

	for(pos = m_Markers.GetHeadPosition();pos!=NULL;)
	{
		CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);

		//Remove any out of range (here no test on MLTSLD in needed as we are at the beginning...)
		if((pMarker->m_Pos > m_RangeMax || pMarker->m_Pos < m_RangeMin))
			RemoveLevelMarker(pMarker);
		else
			SetRectangles(pMarker);	//Reset rectangles
	}
		
	fInit = true;
}

// Moved out of the class (sorry!)	float CMultiSlider::RoundTo(float value, short decimal)
// Moved out of the class (sorry!)	{
// Moved out of the class (sorry!)		value *= (float)pow(10.0, decimal);
// Moved out of the class (sorry!)		value += .5f;
// Moved out of the class (sorry!)		value = (float)floor(value);
// Moved out of the class (sorry!)		value /= (float)pow(10.0, decimal);
// Moved out of the class (sorry!)		return value;
// Moved out of the class (sorry!)	}

/*! \fn bool CMultiSlider::SetRanges(float min, float max, int MarkersNum, float *NewPos)
	\brief Updates the controls after a on-the-fly change of its extremes and/or number of markers.
	
	 \param min New lower extreme of the control;
	 \param max New higher extreme of the control;
	 \param MarkersNum new total number of markers in the control;
	 \param NewPos vector containing starting positions for markers.

	This function resets the control, sets the new extremes and inserts new markers if a list of
	positions is passed. Consistency between MarkersNum and lenght of the vector NewPos relies on caller's 
	resposibility. \b WARNING: Color list for intervals must be eventually updated with correct functions.
	 */
bool CMultiSlider::SetRanges(float min, float max, int MarkersNum, float *NewPos)
{
	m_RangeMax = max;
	m_RangeMin = min;

	m_RangeWidth = m_RangeMax - m_RangeMin;
	int i=0;
	// Beware: if you change number of Markers, you should eventually update the list of colors!
	if (NewPos && MarkersNum)
	{
		CMarker *pNewMarker;
		ResetMarkers();
		for (i=0; i<MarkersNum; i++)
		{
			pNewMarker = new CMarker;
			if (!pNewMarker)
				break;

			pNewMarker->m_Pos = NewPos[i];
			InsertMarker(pNewMarker);
		}
		if (i != MarkersNum)
		{
			ResetMarkers();
			return false;
		}

	}
	fInit = false;
	Invalidate();
	UpdateWindow();
	return true;
}

/*! \fn bool CMultiSlider::GetRanges(float &min, float &max)
	\brief Returns lower and higher extremes of the control.*/
bool CMultiSlider::GetRanges(float &min, float &max)
{
	min = m_RangeMin;
	max = m_RangeMax;

	return true;
}

/*! \fn float CMultiSlider::GetMinRange()
	\brief Try to guess... ;-).*/
float CMultiSlider::GetMinRange()
{
	return m_RangeMin;
}

/*! \fn float CMultiSlider::GetMaxRange()
	\brief Try to guess... ;-).*/
float CMultiSlider::GetMaxRange()
{
	return m_RangeMax;
}

/*! \fn bool CMultiSlider::RemoveLevelMarker(CMarker *pRemoved)
	\brief Removes a marker, usually because it has been dragged out of the control.*/
bool CMultiSlider::RemoveLevelMarker(CMarker *pRemoved)
{
	ASSERT (pRemoved != NULL);
	bool AllIsOk = false;
	CRect rectTI;
	rectTI = pRemoved->m_mainRect;
	rectTI += pRemoved->m_leftRect;
	rectTI += pRemoved->m_rightRect;

	for (POSITION pos = m_Markers.GetHeadPosition (); pos != NULL;)
	{
		POSITION posSave = pos;

		CMarker* pTI = (CMarker*) m_Markers.GetNext (pos);
		ASSERT (pTI != NULL);

		if (pTI == pRemoved)
		{
			m_Markers.RemoveAt (posSave);
			delete pRemoved;

			InvalidateRect(rectTI);
			AllIsOk = true;
		}
	}

	return AllIsOk;
}

/*! \fn int CMultiSlider::InsertMarker(CMarker *pNewMarker)
	\brief Inserts a new marker into the control; starting position must me already sored in the input pointer.*/
int CMultiSlider::InsertMarker(CMarker *pNewMarker)
{
	bool bInserted = false;
	int i = 0;
	int iIndex =  -1;

	for(POSITION pos = m_Markers.GetHeadPosition(); pos != NULL && !bInserted; i++)
	{
		POSITION posSave = pos;
		CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
		ASSERT(pMarker != NULL);

		if(pMarker->m_Pos >= pNewMarker->m_Pos)
		{
			m_Markers.InsertBefore(posSave, pNewMarker);
			iIndex = i;
			bInserted = true;
		}
	}
	
	if(!bInserted)
	{
		m_Markers.AddTail(pNewMarker);
		iIndex = m_Markers.GetCount() - 1;
	}

	return iIndex;
}

/*! \fn void CMultiSlider::CleanUp()
	\brief Resets Markers - calling ResetMarkers() - and colors - calling m_Colors.RemoveAll().*/
void CMultiSlider::CleanUp()
{
	ResetMarkers();
	m_Colors.RemoveAll();
}

/*! \fn void CMultiSlider::ResetMarkers()
	\brief Removes all markers and sets auxiliar pointers to NULL.*/
void CMultiSlider::ResetMarkers()
{
	while (!m_Markers.IsEmpty ())
	{
		delete m_Markers.RemoveHead();
	}

	m_pnewMarker = NULL;
	m_pdraggedMarker = NULL;
	m_pSelectedMarker = NULL;
	m_pTTMarker = NULL;
}

/*! \fn void CMultiSlider::SetRectangles(CMarker *pMarker)
	\brief Builds the three rectangles necessary to draw a Marker (...unfortunately some "emprical" corrections
	were necessary for different configurations...).*/
void CMultiSlider::SetRectangles(CMarker *pMarker)
{
	float SliderMin, SliderWidth;
	CRect top, left, right;
	float x;

	m_RangeMin = m_RangeMin;
	m_RangeMax = m_RangeMax;
	m_RangeWidth = m_RangeMax - m_RangeMin;

	SetSliderValues(&SliderMin, &SliderWidth);

	x = (((pMarker->m_Pos - (float)m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
	x = RoundTo(x, 0);

	// ILIC Rectangle must now be prepared in specific way depending on style...
	right = left = top = thumbRect;

	if (m_StyleIdx & SLIDER_VERT)
	{
		top.top = (int)(x - 6.0f);
		top.bottom = (int)(x + 6.0f);

		if (!m_bStyleBoth)	// m_bStyleBoth doesn't need anything more
		{
			if (m_StyleIdx == 1)	// Pointing right
			{
				top.right = channelRect.right;

				right.left = left.left = top.right;
				right.bottom = left.top = (int) x;	
				
				left.bottom = top.bottom ;
				right.top = top.top;

				right.right +=2; // Don't know why it is necessary :-(
				left.right +=2;
			}
			else	// Pointing left
			{
				top.left = channelRect.left;
				right.right = left.right = top.left;
				right.top = left.bottom = (int) x;
				
				left.top = top.top;
				right.bottom = top.bottom;

				right.left -= 2;
				left.left -= 2;
			}
		}
	}
	else
	{
		top.left = (int)(x - 6.0f);
		top.right = (int)(x + 6.0f);

		if (!m_bStyleBoth)	// m_bStyleBoth doesn't need anything more
		{
			if (m_StyleIdx)	// Pointing up
			{
				top.top = channelRect.top;
				right.bottom = left.bottom = channelRect.top;
				right.right = left.left = (int) x;

				left.right = top.right;
				right.left = top.left;

				left.bottom += 1;
				right.bottom += 1;
				top.top += 1;
				top.bottom += 1;
			}
			else	// Pointing down
			{
				top.bottom = channelRect.bottom;
				right.top = left.top = channelRect.bottom;			
				right.left = left.right = (int) x;

				left.left = top.left;
				right.right = top.right;
			}
		}
	}
	// ILIC Rectangle must now be prepared in specific way depending on style... (END)

	pMarker->m_leftRect = left;
	pMarker->m_mainRect = top;
	pMarker->m_rightRect = right;
}

/*! \fn int CMultiSlider::GetMarkersNum()
	\brief Returns number of markers actually present in the control.*/
int CMultiSlider::GetMarkersNum()
{
	return m_Markers.GetCount();
}

/*! \fn int CMultiSlider::GetMarkers(float *value)
	\brief Returns in input pointer \b value a list of the markers' actual positions.*/
int CMultiSlider::GetMarkers(float *value)
{
	int count = 0;
	POSITION pos;

	for(pos=m_Markers.GetHeadPosition();pos != NULL;)
	{
		CMarker* pMarker = (CMarker*)m_Markers.GetNext(pos);
		ASSERT(pMarker != NULL);
		value[count] = pMarker->m_Pos;
		count++;
	}

	return count;
}

/*! \fn CRect CMultiSlider::GetMarkerRect(CMarker *pMI)
	\brief Returns the rect occupied by the input marker.*/
CRect CMultiSlider::GetMarkerRect(CMarker *pMI)
{
	CRect rect;

	rect = pMI->m_mainRect;
	rect.bottom = clientRect.bottom;

	return rect;
}

/*! \fn void CMultiSlider::OnRButtonUp(UINT nFlags, CPoint point) 
	\brief Allows to set marker position and/or intervals' colors; necessary tests (e.g. MLTSLD_PREVENT_CROSSING) are done.*/
void CMultiSlider::OnRButtonUp(UINT nFlags, CPoint point) 
{
	float SliderMin, SliderWidth;
	CRect tempRect;
	CString strText;
	CPoint pt;
	CEditInterval dlg;
	float x;
	
	SetSliderValues(&SliderMin, &SliderWidth);

	m_wndInfo.Hide ();
	m_TTRect.SetRectEmpty ();

	if(sliderRect.PtInRect(point))	//Verify in window
	{
		m_pSelectedMarker = MarkerHitTest(point);

		if(m_pSelectedMarker != NULL && (m_LayoutMask & MLTSLD_ALLOW_SET_POS))//Editing
		{
			dlg.m_Value = m_pSelectedMarker->m_Pos;
			if (m_LayoutMask & MLTSLD_PREVENT_CROSSING)
			{
				POSITION MyPos = m_Markers.Find(m_pSelectedMarker);
				if (MyPos != m_Markers.GetHeadPosition())
					dlg.m_Min = ((CMarker *) GetPrevMarker(MyPos))->m_Pos;
				else
					dlg.m_Min = m_RangeMin;

				if (MyPos != m_Markers.GetTailPosition())
					dlg.m_Max = ((CMarker *) GetNextMarker(MyPos))->m_Pos;
				else
					dlg.m_Max = m_RangeMax;
			}
			else
			{
				dlg.m_Min = m_RangeMin;
				dlg.m_Max = m_RangeMax;
			}
			if(dlg.DoModal() == IDOK)
			{
				m_pSelectedMarker->m_Pos = dlg.m_Value;
				SetRectangles(m_pSelectedMarker);
				Invalidate();
				x = (((float)(m_pSelectedMarker->m_Pos - m_RangeMin) / (float)m_RangeWidth) * (float)SliderWidth) + SliderMin;
				x = RoundTo(x, 0);

				if (m_LayoutMask & MLTSLD_DISPLAY_TOOLTIPS)
				{
					BuildTooltipText(m_pSelectedMarker, strText);
					::GetCursorPos (&pt);
					pt += CPoint (
								GetSystemMetrics (SM_CXCURSOR) / 2, 
								GetSystemMetrics (SM_CYCURSOR) / 2);
					m_wndInfo.Track (pt, strText);
				}
				NMHDR nmhdr;
				nmhdr.hwndFrom = m_hWnd;
				nmhdr.idFrom = GetDlgCtrlID();
				nmhdr.code = TB_THUMBPOSITION;
				GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM) &nmhdr);
			}
			m_pSelectedMarker = NULL;
		}
		else if ((m_LayoutMask & MLTSLD_DISPLAY_COLORS)	&& (m_LayoutMask & MLTSLD_ALLOW_CHANGE_COLORS))//Changing color of interval
		{
			int i, count, set;
			float pt;
			float *values;

			values = (float*)malloc(sizeof(float) * m_MarkersNum);

			// ILIC Just slightly different... :-)
			if (m_StyleIdx & SLIDER_VERT)
				pt = ((float)(point.y - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;
			else
				pt = ((float)(point.x - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;
			
			pt = RoundTo(pt, 1);
			count = GetMarkers(values);
			set = -1;
			if(pt <= values[0])
				set = 0;
			else if(pt >= values[count-1])
				set = count;
			else
			{
				for(i=0;i<=count;i++)
				{
					if(pt >= values[i] && pt <= values[i+1])
					{
						set = i+1;
						break;
					}
				}
			}
			if(set != -1)
			{
				CColorDialog dlg(m_Colors.GetAt(set), 0, NULL);
				if(dlg.DoModal() == IDOK)
				{
					m_Colors.SetAt(set, dlg.GetColor());
					Invalidate();
				}
			}
			free(values);
		}
	}	
	CSliderCtrl::OnRButtonUp(nFlags, point);
}

/*! \fn void CMultiSlider::GetMinMax(CPoint point)
	\brief Returns extremal positions of markers actually present in the control.*/
void CMultiSlider::GetMinMax(CPoint point)
{
	int i, count;
	float x;
	float SliderMin, SliderWidth;
	float *values;

	values = (float*)malloc(sizeof(float) * m_MarkersNum);

	SetSliderValues(&SliderMin, &SliderWidth);

	// ILIC Just slightly different... :-)
	if (m_StyleIdx & SLIDER_VERT)
		x = ((float)(point.y - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;
	else
		x = ((float)(point.x - SliderMin)/(float)SliderWidth * (float)m_RangeWidth) + m_RangeMin;

	x = RoundTo(x, 1);

	prevMin = m_RangeMin;
	prevMax = m_RangeMax;

	count = GetMarkers(values);
	for(i=0;i<count;i++)
	{
		if(values[i] < x)
			prevMin = values[i];
	}
	for(i=count-1;i>=0;i--)
	{
		if(values[i] > x)
			prevMax = values[i];
	}

	free(values);
	return;
}

/*! \fn void CMultiSlider::AvoidCrossing(CMarker *pMarker, float *Position)
	\brief Checks for marker crossing and avoids it.*/
void CMultiSlider::AvoidCrossing(CMarker *pMarker, float *Position)
{
	POSITION MyPos = m_Markers.Find(pMarker);

	if (MyPos != m_Markers.GetHeadPosition())	// Not the first one
	{
		float OtherPos = ((CMarker *) GetPrevMarker(MyPos))->m_Pos;
		if (OtherPos >= *Position)
		{
			*Position = OtherPos;
			m_DraggedMarkerIsBlocked = MLTSLD_BLOCKED_BY_PREV;
			return;
		}		
	}

	if (MyPos != m_Markers.GetTailPosition())	// Not the last one
	{
		float OtherPos = ((CMarker *) GetNextMarker(MyPos))->m_Pos;
		if (OtherPos <= *Position)
		{
			*Position = OtherPos;
			m_DraggedMarkerIsBlocked = MLTSLD_BLOCKED_BY_NEXT;
			return;
		}		
	}

	m_DraggedMarkerIsBlocked = 0;
}

/*! \fn void CMultiSlider::Init(float Min, float Max, int TicFreq, int MarkersNum, float *InitPos, COLORREF* ColorsList, int LayoutMask)
	\brief Initializes the control with various parameters.

	\param Min Lower extreme of the marker;
	\param Max Higher extreme of the marker;
	\param TicFreq Number of tickmarks (... I'm noto sure about that in this moment!);
	\param MarkersNum Maximum number of markers which will be inserted in the control;
	\param InitPos List of initial positions of markers;
	\param ColorsList List of colors to be used for intervals (obviously you need MarkersNum-1 colors...);
	\param LayoutMask Mask with options for the layout.

	This function allows you to initialize the control, inserting automatically all the markers at the specified conditions. 
	The pointers MUST point to list whose length is coherent with MarkersNum (you can't set MarkersNum to 5 and then initialize 
	and insert only three markers... For details about layout mask, see documentation in header file.*/
void CMultiSlider::Init(float Min, float Max, int TicFreq, int MarkersNum, float *InitPos, COLORREF* ColorsList, int LayoutMask)
{
	fInit = false;

	m_RangeMin = Min;
	m_RangeMax = Max;
	m_RangeWidth = m_RangeMax - m_RangeMin;
	if (LayoutMask != -1)
		m_LayoutMask = LayoutMask;
	m_MarkersNum = MarkersNum;
	SetTicFreq(TicFreq);
	
	m_Colors.RemoveAll();

	for (int i=0; i<MarkersNum; i++)
	{
		if (InitPos)	// Adds Markers immediately 
		{
			CMarker *pMarker = new CMarker/*(i)*/;
			pMarker->m_Pos = InitPos[i];
			InsertMarker(pMarker);
		}
		m_Colors.Add(ColorsList[i]);
	}

	// Intervals are MarkersNum + 1
	m_Colors.Add(ColorsList[i]);

	Invalidate();
	UpdateWindow();
}

/*! \fn bool CMultiSlider::SetColors(COLORREF *ColorsList)
	\brief Sets a new list of colors for intervals.*/
bool CMultiSlider::SetColors(COLORREF *ColorsList)
{
	if (!m_MarkersNum)
		return false;

	m_Colors.RemoveAll();

	for (int i=0; i<=m_MarkersNum; i++)
		m_Colors.Add(ColorsList[i]);
	
	return true;
}

/*! \fn void CMultiSlider::SetSliderValues(float *SliderMin, float *SliderWidth)
	\brief Returns data for drawing the control.*/
void CMultiSlider::SetSliderValues(float *SliderMin, float *SliderWidth)
{
	float SliderMax;
	if (m_StyleIdx & SLIDER_VERT)
	{
		if (m_LayoutMask & MLTSLD_INVERT_RANGES)
		{
			SliderMax = (float) sliderRect.bottom;
			*SliderMin = (float) sliderRect.top;
		}
		else
		{
			SliderMax = (float) sliderRect.top;
			*SliderMin = (float) sliderRect.bottom;
		}
	}
	else
	{
		if (m_LayoutMask & MLTSLD_INVERT_RANGES)
		{
			SliderMax = (float) sliderRect.left;
			*SliderMin = (float) sliderRect.right;
		}
		else
		{
			SliderMax = (float) sliderRect.right;
			*SliderMin = (float) sliderRect.left;
		}
	}
	*SliderWidth = (float) (SliderMax - *SliderMin);	// in order to compensate possible sign change.
}

/*! \fn CMarker *CMultiSlider::GetNextMarker(POSITION &Pos) 
	\brief Returns pointer to next marker in the list.

	Markers are ordered in the list by thier position in the control (from lower to higher), so this function
	can be used both for ciclying over the list and for determining marker following in the slider.*/
CMarker *CMultiSlider::GetNextMarker(POSITION &Pos) 
{
	POSITION OrgPos = Pos;
	m_Markers.GetNext(Pos); 
	CMarker *pTheMarker = (CMarker *) m_Markers.GetNext(Pos);

	// restore Pos
	Pos = OrgPos;

	return pTheMarker;
}

/*! \fn CMarker *CMultiSlider::GetPrevMarker(POSITION &Pos)
	\brief Returns pointer to next marker in the list.

	Markers are ordered in the list by thier position in the control (from lower to higher), so this function
	can be used both for ciclying over the list and for determining marker following in the slider.*/
CMarker *CMultiSlider::GetPrevMarker(POSITION &Pos)
{
	POSITION OrgPos = Pos;
	m_Markers.GetPrev(Pos); 
	CMarker *pTheMarker  = (CMarker *) m_Markers.GetPrev(Pos);

	// restore Pos
	Pos = OrgPos;

	return pTheMarker;
}
