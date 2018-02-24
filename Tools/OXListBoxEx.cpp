// OXListBoxEx.cpp : implementation file
//
// Version: 9.3


#include "stdafx.h"
//#include "OXMainRes.h"
#include "OXListBoxEx.h"

//#include "UTBStrOp.h"
//#include "UTB64Bit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL COXListBoxEx::m_bWindowsNTRunning=COXListBoxEx::IsWindowsNTRunning();


IMPLEMENT_DYNAMIC(COXListBoxEx, CListBox)

/////////////////////////////////////////////////////////////////////////////
// COXListBoxEx

COXListBoxEx::COXListBoxEx() :
	m_pImageList(NULL),
	m_nMaxItemsBeforeSeparator(0),
	m_nLastItemBeforeSeparator(-1),
	m_nWidestItemIndex(-1),
	m_nLastTopIndex(-1),
	m_clrBackground(::GetSysColor(COLOR_WINDOW)),
	m_bHighlightOnlyText(FALSE),
	m_sTooltipText(_T("")),
	m_bSaveRestoreMRUState(FALSE)
{
}

COXListBoxEx::~COXListBoxEx()
{
}


BEGIN_MESSAGE_MAP(COXListBoxEx, CListBox)
	//{{AFX_MSG_MAP(COXListBoxEx)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_MOUSEWHEEL()
    ON_WM_CTLCOLOR_REFLECT()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COXListBoxEx message handlers

int COXListBoxEx::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
{	
	// TODO: Add your code to determine the sorting order of the specified items
	// return -1 = item 1 sorts before item 2
	// return 0 = item 1 and item 2 sort the same
	// return 1 = item 1 sorts after item 2
	OXLISTBOXITEM* pLBI1=GetItemInfo(lpCompareItemStruct->itemID1);
	if(pLBI1==NULL)
		return 0;
	OXLISTBOXITEM* pLBI2=GetItemInfo(lpCompareItemStruct->itemID2);
	if(pLBI2==NULL)
		return 0;

	ASSERT(pLBI1->m_nMRUIndex!=pLBI2->m_nMRUIndex || 
		(pLBI1->m_nMRUIndex==-1 && pLBI2->m_nMRUIndex==-1));
	if(pLBI1->m_nMRUIndex!=pLBI2->m_nMRUIndex)
	{
		if(pLBI1->m_nMRUIndex==-1)
		{
			return 1;
		}
		else if(pLBI2->m_nMRUIndex==-1)
		{
			return -1;
		}
		else
		{
			return ((pLBI1->m_nMRUIndex<pLBI2->m_nMRUIndex ? -1 : 1));
		}
			
	}
	else
	{
		return 0;
	}

}

void COXListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	UNREFERENCED_PARAMETER(lpMeasureItemStruct);
}

void COXListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect rect(lpDrawItemStruct->rcItem);

	if(GetCount()==0)
	{
		return;
	}

	OXLISTBOXITEM* pLBI=NULL;
	BOOL bHaveToDelete=FALSE;
	if(lpDrawItemStruct->itemID==-1)
	{
		pLBI=new OXLISTBOXITEM;
		bHaveToDelete=TRUE;
	}
	else
	{
		pLBI=GetItemInfo(lpDrawItemStruct->itemID);
	}
	ASSERT(pLBI!=NULL);
	CRect rectItem=pLBI->m_rectItem;
	CRect rectImage=pLBI->m_rectImage;
	CRect rectText=pLBI->m_rectText;

	if((int)lpDrawItemStruct->itemID==GetLastItemBeforeSeparator())
	{
		if((lpDrawItemStruct->itemAction & ODA_DRAWENTIRE))
		{
			CRect rectSeparator=rect;
			rectSeparator.top=rectSeparator.bottom-OXLB_SEPARATOR_HEIGHT;

			DrawSeparator(pDC,rectSeparator,pLBI);
		}
		rect.bottom-=OXLB_SEPARATOR_HEIGHT;
	}

	rectItem+=rect.TopLeft();
	rectImage+=rect.TopLeft();
	rectText+=rect.TopLeft();

	if((lpDrawItemStruct->itemAction & (ODA_SELECT|ODA_DRAWENTIRE)))
	{
		CBrush brush(lpDrawItemStruct->itemState & ODS_SELECTED ?
			pLBI->m_clrBackgroundHighlight : pLBI->m_clrBackground);
		pDC->FillRect((GetHighlightOnlyText() ? rectText : rect),&brush);
	}
	else if((lpDrawItemStruct->itemAction & (ODA_FOCUS|ODA_DRAWENTIRE)) &&
		!(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		CBrush brush(lpDrawItemStruct->itemState & ODS_SELECTED ?
			pLBI->m_clrBackgroundHighlight : pLBI->m_clrBackground);
		pDC->FrameRect((GetHighlightOnlyText() ? rectText : rect),&brush);
	}


	if(lpDrawItemStruct->itemAction&(ODA_SELECT|ODA_DRAWENTIRE))
	{
		// draw image
		if(!rectImage.IsRectEmpty() && pLBI->m_nImageIndex!=-1)
		{
			UINT nStyle=(((lpDrawItemStruct->itemState & ODS_SELECTED) && 
				GetHighlightOnlyText()) ? ILD_SELECTED : 0) | ILD_TRANSPARENT;
			DrawImage(pDC,rectImage,nStyle,pLBI);
		}

		if(GetStyle()&LBS_HASSTRINGS)
		{
			// draw text
			CString sText;
			if(lpDrawItemStruct->itemID==-1)
				sText.Empty();
			else
				GetText(lpDrawItemStruct->itemID,sText);

			COLORREF clrText=pLBI->m_clrText;
			if(lpDrawItemStruct->itemState & ODS_SELECTED)
				clrText=pLBI->m_clrTextHighlight;

			DrawText(pDC,sText,rectText,clrText,pLBI->m_pFont);
		}
	}

	if((lpDrawItemStruct->itemAction & (ODA_FOCUS|ODA_DRAWENTIRE)) &&
		(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		pDC->DrawFocusRect((GetHighlightOnlyText() ? rectText : rect));
	}

	if(bHaveToDelete)
		delete pLBI;
}


void COXListBoxEx::DrawSeparator(CDC* pDC, CRect rectSeparator, OXLISTBOXITEM* pLBI)
{
	ASSERT(pDC!=NULL);
	ASSERT(pLBI!=NULL);

	CBrush brush(pLBI->m_clrBackground);
	pDC->FillRect(rectSeparator,&brush);

	rectSeparator.top+=OXLB_SEPARATOR_HEIGHT/2-1;
	rectSeparator.bottom=rectSeparator.top+2;
    ::FillRect(pDC->GetSafeHdc(),rectSeparator,
		::GetSysColorBrush(COLOR_3DSHADOW));
}


void COXListBoxEx::DrawImage(CDC* pDC, CRect& rectImage, 
							 UINT nStyle, OXLISTBOXITEM* pLBI)
{
	ASSERT(pDC!=NULL);
	ASSERT(pLBI!=NULL);

	ASSERT(GetImageList()!=NULL);
	GetImageList()->Draw(pDC,pLBI->m_nImageIndex,rectImage.TopLeft(),nStyle);
}


void COXListBoxEx::DrawText(CDC* pDC, LPCTSTR lpszText, CRect& rectText,
							COLORREF clrText, CFont* pFont)
{
	ASSERT(pDC!=NULL);

	int nOldBkMode=pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldText=pDC->SetTextColor(clrText);
	CFont* pOldFont=NULL;
	if(pFont!=NULL)
		pOldFont=pDC->SelectObject(pFont);

	pDC->DrawText(lpszText,rectText,DT_LEFT|DT_SINGLELINE|DT_VCENTER);

	if(pOldFont!=NULL)
		pDC->SelectObject(pOldFont);
	pDC->SetTextColor(clrOldText);
	pDC->SetBkMode(nOldBkMode);
}


HBRUSH COXListBoxEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
	UNREFERENCED_PARAMETER(pDC);
	UNREFERENCED_PARAMETER(nCtlColor);

    if(GetCount()==0)
    {
		static CBrush brush;
		if((HBRUSH)brush!=NULL)
			brush.DeleteObject();
		brush.CreateSolidBrush(m_clrBackground);
		return brush;
    } 
    return NULL;
}


void COXListBoxEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	DWORD dwStyle=GetStyle();
	// make sure LBS_OWNERDRAWVARIABLE style is specified
	ASSERT(dwStyle&LBS_OWNERDRAWVARIABLE);

	_AFX_THREAD_STATE* pThreadState=AfxGetThreadState();
	// hook not already in progress
	if(pThreadState->m_pWndInit==NULL)
	{
		if(!InitializeListBox())
		{
			TRACE(_T("COXListBoxEx::PreSubclassWindow: failed to initialize the control\n"));
		}
	}

	CListBox::PreSubclassWindow();
}

int COXListBoxEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if(!InitializeListBox())
	{
		TRACE(_T("COXListBoxEx::OnCreate: failed to initialize the control\n"));
		return -1;
	}
	
	
	return 0;
}

BOOL COXListBoxEx::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	// make sure LBS_OWNERDRAWVARIABLE style is specified
	cs.style&=~LBS_OWNERDRAWFIXED;
	cs.style|=LBS_OWNERDRAWVARIABLE;

	return CListBox::PreCreateWindow(cs);
}

int COXListBoxEx::DeleteString(UINT nIndex)
{
	if(!m_bWindowsNTRunning)
	{
		int nItemIndex=nIndex;
		OXLISTBOXITEM* pLBI=GetItemInfo(nItemIndex);
		if(pLBI!=NULL)
		{
			VERIFY(m_mapItems.RemoveKey(nItemIndex));
			delete pLBI;
			int nItemCount= PtrToInt(m_mapItems.GetCount());
			if(nItemIndex<nItemCount)
			{
				for(int nIndex=nItemIndex+1; nIndex<=(int)nItemCount; nIndex++)
				{
					pLBI=GetItemInfo(nIndex);
					ASSERT(pLBI!=NULL);
					m_mapItems.SetAt(nIndex-1,pLBI);
				}
				VERIFY(m_mapItems.RemoveKey(nItemCount));
				ASSERT(m_mapItems.GetCount()==(int)nItemCount);
			}
		
			BOOL bRecalc=(nItemIndex==m_nWidestItemIndex);
			if(nItemIndex<=GetLastItemBeforeSeparator())
			{
				// update MRU list
				for(int nIndex=nItemIndex; nIndex<GetLastItemBeforeSeparator(); nIndex++)
				{
					OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
					ASSERT(pLBI!=NULL);
					ASSERT(pLBI->m_nMRUIndex==nIndex+1);
					pLBI->m_nMRUIndex--;
				}
				m_nLastItemBeforeSeparator--;

				bRecalc=TRUE;
			}

			if(bRecalc)
				RecalcItemsExtent();

			UpdateTooltipTools();
		}
		else
		{
			ASSERT(!m_mapItems.Lookup(nItemIndex,pLBI));
		}
	}

	return CListBox::DeleteString(nIndex);
}

void COXListBoxEx::ResetContent()
{
	if(!m_bWindowsNTRunning)
	{
		POSITION pos=m_mapItems.GetStartPosition();
		while(pos!=NULL)
		{
			int nIndex=-1;
			OXLISTBOXITEM* pLBI=NULL;
			m_mapItems.GetNextAssoc(pos,nIndex,pLBI);
			ASSERT(nIndex!=-1);
			ASSERT(pLBI!=NULL);
			delete pLBI;
		}
		m_mapItems.RemoveAll();

		m_nLastItemBeforeSeparator=-1;
		m_nWidestItemIndex=-1;
		m_nLastTopIndex=-1;
		RecalcItemsExtent();
		UpdateTooltipTools();
	}

	CListBox::ResetContent();
}


void COXListBoxEx::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT(lpDeleteItemStruct!=NULL);

	if(m_bWindowsNTRunning)
	{
		OXLISTBOXITEM* pLBI=GetItemInfo(lpDeleteItemStruct->itemID);
		if(pLBI!=NULL)
		{
			VERIFY(m_mapItems.RemoveKey(lpDeleteItemStruct->itemID));
			delete pLBI;
			UINT nItemCount=(UINT)m_mapItems.GetCount();
			if(lpDeleteItemStruct->itemID<nItemCount)
			{
				for(int nIndex=lpDeleteItemStruct->itemID+1; 
					nIndex<=(int)nItemCount; nIndex++)
				{
					pLBI=GetItemInfo(nIndex);
					ASSERT(pLBI!=NULL);
					m_mapItems.SetAt(nIndex-1,pLBI);
				}
				VERIFY(m_mapItems.RemoveKey(nItemCount));
				ASSERT(m_mapItems.GetCount()==(int)nItemCount);
			}
			
			CListBox::DeleteItem(lpDeleteItemStruct);

			BOOL bRecalc=(lpDeleteItemStruct->itemID==(UINT)m_nWidestItemIndex);
			if((int)lpDeleteItemStruct->itemID<=GetLastItemBeforeSeparator())
			{
				// update MRU list
				for(int nIndex=lpDeleteItemStruct->itemID; 
					nIndex<GetLastItemBeforeSeparator(); nIndex++)
				{
					OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
					ASSERT(pLBI!=NULL);
					ASSERT(pLBI->m_nMRUIndex==nIndex+1);
					pLBI->m_nMRUIndex--;
				}
				m_nLastItemBeforeSeparator--;

				bRecalc=TRUE;
			}

			if(bRecalc)
				RecalcItemsExtent();

			UpdateTooltipTools();
		}
		else
		{
			ASSERT(!m_mapItems.Lookup(lpDeleteItemStruct->itemID,pLBI));
			CListBox::DeleteItem(lpDeleteItemStruct);
		}
	}
	else
	{
		CListBox::DeleteItem(lpDeleteItemStruct);
	}
}


BOOL COXListBoxEx::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class

	int nTopIndex=GetTopIndex();
	if(nTopIndex!=LB_ERR && nTopIndex!=m_nLastTopIndex)
	{
		m_nLastTopIndex=nTopIndex;
		UpdateTooltipTools();
	}

	CToolTipCtrl* pTooTipCtrl=GetToolTipCtrl();
	ASSERT(pTooTipCtrl!=NULL);
	ASSERT(::IsWindow(pTooTipCtrl->GetSafeHwnd()));
	pTooTipCtrl->Activate(TRUE);
	pTooTipCtrl->RelayEvent(pMsg);
	
	return CListBox::PreTranslateMessage(pMsg);
}


void COXListBoxEx::OnSize(UINT nType, int cx, int cy) 
{
	CListBox::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CToolTipCtrl* pTooTipCtrl=GetToolTipCtrl();
	ASSERT(pTooTipCtrl!=NULL);
	if(::IsWindow(pTooTipCtrl->GetSafeHwnd()))
	{
		CRect rect;
		GetClientRect(rect);
		if(rect.IsRectEmpty())
		{
			rect.right=rect.left+1;
			rect.bottom=rect.top+1;
		}

		CToolInfo toolInfo;
		toolInfo.cbSize=sizeof(TOOLINFO);
		VERIFY(pTooTipCtrl->GetToolInfo(toolInfo,this,ID_OXLB_TOOLTIP));
		toolInfo.rect=rect;
		pTooTipCtrl->SetToolInfo(&toolInfo);
	}

    UpdateTooltipTools();
}


void COXListBoxEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CListBox::OnVScroll(nSBCode, nPos, pScrollBar);

	if(nSBCode==SB_ENDSCROLL)
		UpdateTooltipTools();
}


BOOL COXListBoxEx::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRect rect;
	GetClientRect(rect);
	if(GetHighlightOnlyText() || GetCount()==0)
	{
		CBrush brush(m_clrBackground);
		pDC->FillRect(rect,&brush);
	}
	else 
	{
		CRect rectItem;
		VERIFY(GetItemRect(GetCount()-1,rectItem)!=LB_ERR);
		if(rectItem.bottom<rect.bottom)
		{
			rect.top=rectItem.bottom;
			CBrush brush(m_clrBackground);
			pDC->FillRect(rect,&brush);
		}
	}

	return TRUE;
}


void COXListBoxEx::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CListBox::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if(GetExStyle()&WS_EX_TOPMOST)
	{
		CToolTipCtrl* pTooTipCtrl=GetToolTipCtrl();
		ASSERT(pTooTipCtrl!=NULL);
		if(::IsWindow(pTooTipCtrl->GetSafeHwnd()))
		{
			pTooTipCtrl->SetWindowPos(&wndTopMost,0,0,0,0,
				SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
		}
	}

	UpdateTooltipTools();
}


void COXListBoxEx::OnDestroy() 
{
	if(GetSaveRestoreMRUState())
	{
		SaveMRUState();
	}

	ResetContent();
	if(::IsWindow(m_tooltip.GetSafeHwnd()))
	{
		VERIFY(m_tooltip.DestroyWindow());
	}

	CListBox::OnDestroy();
}


OXLISTBOXITEM* COXListBoxEx::GetItemInfo(int nIndex) const
{
	OXLISTBOXITEM* pLBI=NULL;
	if(!m_mapItems.Lookup(nIndex,pLBI))
	{
		return NULL;
	}
	else
	{
		ASSERT(pLBI!=NULL);
		return pLBI;
	}
}

CFont* COXListBoxEx::GetItemFont(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return NULL;
	else
		return pLBI->m_pFont;
}

COLORREF COXListBoxEx::GetItemTextColor(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return CLR_NONE;
	else
		return pLBI->m_clrText;
}

COLORREF COXListBoxEx::GetItemTextColorHighlight(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return CLR_NONE;
	else
		return pLBI->m_clrTextHighlight;
}

COLORREF COXListBoxEx::GetItemBkColor(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return CLR_NONE;
	else
		return pLBI->m_clrBackground;
}

COLORREF COXListBoxEx::GetItemBkColorHighlight(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return CLR_NONE;
	else
		return pLBI->m_clrBackgroundHighlight;
}

int COXListBoxEx::GetItemImageIndex(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return -1;
	else
		return pLBI->m_nImageIndex;
}

int COXListBoxEx::GetItemIndent(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return 0;
	else
		return pLBI->m_nIndent;
}

CString COXListBoxEx::GetItemTooltipText(int nIndex) const
{
	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
		return _T("");
	else
		return pLBI->m_sTooltipText;
}


BOOL COXListBoxEx::SetItemInfo(int nIndex, int nMask, OXLISTBOXITEM* pLBI)
{
	ASSERT(pLBI!=NULL);
	return SetItemInfo(nIndex,nMask,pLBI->m_pFont,pLBI->m_clrText,
		pLBI->m_clrBackground,pLBI->m_sTooltipText,pLBI->m_nImageIndex,
		pLBI->m_nIndent,pLBI->m_clrTextHighlight,pLBI->m_clrBackgroundHighlight);
}


BOOL COXListBoxEx::SetItemInfo(int nIndex, int nMask/*=0*/, CFont* pFont/*=NULL*/, 
							   COLORREF clrText/*=::GetSysColor(COLOR_WINDOWTEXT)*/,
							   COLORREF clrBackground/*=::GetSysColor(COLOR_WINDOW)*/,
							   CString sTooltipText/*=_T("")*/, 
							   int nImageIndex/*=-1*/, int nIndent/*=0*/,
							   COLORREF clrTextHighlight/*=::GetSysColor(COLOR_HIGHLIGHTTEXT)*/,
							   COLORREF clrBackgroundHighlight/*=::GetSysColor(COLOR_HIGHLIGHT)*/)
{
	if(nIndex<-1 && nIndex>=GetCount())
		return FALSE;

	if(nIndex==-1)
	{
		for(int nItemIndex=0; nItemIndex<GetCount(); nItemIndex++)
		{
			VERIFY(SetItemInfo(nItemIndex,nMask,pFont,clrText,
				clrBackground,sTooltipText,nImageIndex,nIndent,
				clrTextHighlight,clrBackgroundHighlight));
		}
		return TRUE;
	}

	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	if(pLBI==NULL)
	{
		pLBI=new OXLISTBOXITEM(pFont,clrText,clrBackground,sTooltipText,
			nImageIndex,nIndent,clrTextHighlight,clrBackgroundHighlight);
		m_mapItems.SetAt(nIndex,pLBI);

		RecalcItemsExtent(nIndex);
		UpdateTooltipTools();
	}
	else
	{
		if(nMask&OXLBI_FONT)
			pLBI->m_pFont=pFont;
		if(nMask&OXLBI_TEXTCOLOR)
			pLBI->m_clrText=clrText;
		if(nMask&OXLBI_BKCOLOR)
			pLBI->m_clrBackground=clrBackground;
		if(nMask&OXLBI_TOOLTIPTEXT)
			pLBI->m_sTooltipText=sTooltipText;
		if(nMask&OXLBI_IMAGEINDEX)
			pLBI->m_nImageIndex=nImageIndex;
		if(nMask&OXLBI_INDENT)
			pLBI->m_nIndent=nIndent;
		if(nMask&OXLBI_TEXTCOLORHIGHLIGHT)
			pLBI->m_clrTextHighlight=clrTextHighlight;
		if(nMask&OXLBI_BKCOLORHIGHLIGHT)
			pLBI->m_clrBackgroundHighlight=clrBackgroundHighlight;
	
		if(nMask&OXLBI_FONT || nMask&OXLBI_INDENT)
		{
			RecalcItemsExtent(nIndex);
		}

		if(nMask&OXLBI_FONT || nMask&OXLBI_INDENT || nMask&OXLBI_TOOLTIPTEXT)
		{
			UpdateTooltipTools();
		}
	}

	CRect rect;
	GetClientRect(rect);
	CRect rectItem;
	VERIFY(GetItemRect(nIndex,rectItem)!=LB_ERR);
	if(rectItem.IntersectRect(rectItem,rect))
		RedrawWindow();

	return TRUE;
}


BOOL COXListBoxEx::SetItemFont(int nIndex, CFont* pFont)
{
	return SetItemInfo(nIndex,OXLBI_FONT,pFont);
}

BOOL COXListBoxEx::SetItemTextColor(int nIndex, COLORREF clrText)
{
	return SetItemInfo(nIndex,OXLBI_TEXTCOLOR,NULL,clrText);
}

BOOL COXListBoxEx::SetItemTextColorHighlight(int nIndex, COLORREF clrTextHighlight)
{
	OXLISTBOXITEM lbi;
	lbi.m_clrTextHighlight=clrTextHighlight;
	return SetItemInfo(nIndex,OXLBI_TEXTCOLORHIGHLIGHT,&lbi);
}

BOOL COXListBoxEx::SetItemBkColor(int nIndex, COLORREF clrBackground)
{
	return SetItemInfo(nIndex,OXLBI_BKCOLOR,NULL,::GetSysColor(COLOR_WINDOWTEXT),
		clrBackground);
}

BOOL COXListBoxEx::SetItemBkColorHighlight(int nIndex, COLORREF clrBackgroundHighlight)
{
	OXLISTBOXITEM lbi;
	lbi.m_clrBackgroundHighlight=clrBackgroundHighlight;
	return SetItemInfo(nIndex,OXLBI_BKCOLORHIGHLIGHT,&lbi);
}

BOOL COXListBoxEx::SetItemImageIndex(int nIndex, int nImageIndex)
{
	return SetItemInfo(nIndex,OXLBI_IMAGEINDEX,NULL,::GetSysColor(COLOR_WINDOWTEXT),
		::GetSysColor(COLOR_WINDOW),_T(""),nImageIndex);
}

BOOL COXListBoxEx::SetItemIndent(int nIndex, int nIndent)
{
	return SetItemInfo(nIndex,OXLBI_INDENT,NULL,::GetSysColor(COLOR_WINDOWTEXT),
		::GetSysColor(COLOR_WINDOW),_T(""),-1,nIndent);
}

BOOL COXListBoxEx::SetItemTooltipText(int nIndex, CString sTooltipText)
{
	return SetItemInfo(nIndex,OXLBI_TOOLTIPTEXT,NULL,::GetSysColor(COLOR_WINDOWTEXT),
		::GetSysColor(COLOR_WINDOW),sTooltipText);
}


int COXListBoxEx::AddString(LPCTSTR lpszItem, int nMask, OXLISTBOXITEM* pLBI)
{
	ASSERT(pLBI!=NULL);
	return AddString(lpszItem,nMask,pLBI->m_pFont,pLBI->m_clrText,
		pLBI->m_clrBackground,pLBI->m_sTooltipText,pLBI->m_nImageIndex,
		pLBI->m_nIndent,pLBI->m_clrTextHighlight,pLBI->m_clrBackgroundHighlight);
}


int COXListBoxEx::AddString(LPCTSTR lpszItem, int nMask/*=0*/, CFont* pFont/*=NULL*/, 
							COLORREF clrText/*=::GetSysColor(COLOR_WINDOWTEXT)*/,
							COLORREF clrBackground/*=::GetSysColor(COLOR_WINDOW)*/,
							CString sTooltipText/*=_T("")*/, 
							int nImageIndex/*=-1*/, int nIndent/*=0*/,
							COLORREF clrTextHighlight/*=::GetSysColor(COLOR_HIGHLIGHTTEXT)*/,
							COLORREF clrBackgroundHighlight/*=::GetSysColor(COLOR_HIGHLIGHT)*/)
{
	int nResult=CListBox::AddString(lpszItem);
	if(nResult>=0)
	{
		const int nItemCount = PtrToInt(m_mapItems.GetCount());

		// If we didn't insert at the end, then the box is sorted, and we need to
		// move the item info objects to line up with the new order.
		if (nResult != nItemCount)
		{
			for(int nIndexUpdate=nItemCount-1; 
				nIndexUpdate>=nResult; nIndexUpdate--)
			{
				OXLISTBOXITEM* pLBI=GetItemInfo(nIndexUpdate);
				ASSERT(pLBI!=NULL);
				m_mapItems.SetAt(nIndexUpdate+1,pLBI);
			}

			m_mapItems.RemoveKey(nResult);
		}

		ASSERT(nItemCount == m_mapItems.GetCount());

		SetItemInfo(nResult,nMask,pFont,clrText,clrBackground,sTooltipText,
			nImageIndex,nIndent,clrTextHighlight,clrBackgroundHighlight);

	}
	return nResult;
}


int COXListBoxEx::InsertString(int nIndex, LPCTSTR lpszItem, int nMask, 
							   OXLISTBOXITEM* pLBI)
{
	ASSERT(pLBI!=NULL);
	return InsertString(nIndex,lpszItem,nMask,pLBI->m_pFont,pLBI->m_clrText,
		pLBI->m_clrBackground,pLBI->m_sTooltipText,pLBI->m_nImageIndex,
		pLBI->m_nIndent,pLBI->m_clrTextHighlight,pLBI->m_clrBackgroundHighlight);
}


int COXListBoxEx::InsertString(int nIndex, LPCTSTR lpszItem, 
							   int nMask/*=0*/, CFont* pFont/*=NULL*/, 
							   COLORREF clrText/*=::GetSysColor(COLOR_WINDOWTEXT)*/,
							   COLORREF clrBackground/*=::GetSysColor(COLOR_WINDOW)*/,
							   CString sTooltipText/*=_T("")*/, 
							   int nImageIndex/*=-1*/, int nIndent/*=0*/,
							   COLORREF clrTextHighlight/*=::GetSysColor(COLOR_HIGHLIGHTTEXT)*/,
							   COLORREF clrBackgroundHighlight/*=::GetSysColor(COLOR_HIGHLIGHT)*/)
{
	if(nIndex==-1)
	{
		return AddString(lpszItem,nMask,pFont,clrText,clrBackground,
			sTooltipText,nImageIndex,nIndent,clrTextHighlight,clrBackgroundHighlight);
	}

	int nResult=CListBox::InsertString(nIndex,lpszItem);
	if(nResult==nIndex)
	{
		int nItemCount= PtrToInt(m_mapItems.GetCount());
		if(nIndex<nItemCount)
		{
			for(int nIndexUpdate=nItemCount-1; 
				nIndexUpdate>=nIndex; nIndexUpdate--)
			{
				OXLISTBOXITEM* pLBI=GetItemInfo(nIndexUpdate);
				ASSERT(pLBI!=NULL);
				m_mapItems.SetAt(nIndexUpdate+1,pLBI);
			}
			m_mapItems.RemoveKey(nIndex);
		}
		ASSERT(nItemCount==m_mapItems.GetCount());
		SetItemInfo(nIndex,nMask,pFont,clrText,clrBackground,sTooltipText,
			nImageIndex,nIndent,clrTextHighlight,clrBackgroundHighlight);
	}
	return nResult;
}


void COXListBoxEx::SetMaxItemsBeforeSeparator(int nMaxItemsBeforeSeparator)
{
	ASSERT(nMaxItemsBeforeSeparator>=-1);

	m_nMaxItemsBeforeSeparator=nMaxItemsBeforeSeparator;
	if(m_nMaxItemsBeforeSeparator!=-1 &&
		m_nMaxItemsBeforeSeparator<=GetLastItemBeforeSeparator())
	{
		for(int nIndex=GetLastItemBeforeSeparator(); 
			nIndex>=m_nMaxItemsBeforeSeparator;	nIndex--)
		{
			VERIFY(MRUDelete(nIndex)!=LB_ERR);
		}
		m_nLastItemBeforeSeparator=m_nMaxItemsBeforeSeparator-1;
		if(::IsWindow(GetSafeHwnd()))
		{
			RecalcItemsExtent();
			UpdateTooltipTools();
			RedrawWindow();
		}
	}
}



void COXListBoxEx::RecalcItemsExtent(int nItemIndex/*=-1*/)
{
	if(nItemIndex==-1)
	{
		for(int nIndex=0; nIndex<m_mapItems.GetCount(); nIndex++)
		{
			RecalcItemsExtent(nIndex);
		}
	}
	else
	{
		int nMaxLength=GetHorizontalExtent();

		CRect rectItem;
		CRect rectItemVisible;
		CRect rectImage;
		CRect rectText;
		CalcItemRects(nItemIndex,rectItem,rectItemVisible,rectImage,rectText);

   		VERIFY(SetItemHeight(nItemIndex,rectItem.Height())!=LB_ERR);

		if(rectItem.Width()>nMaxLength)
		{
			m_nWidestItemIndex=nItemIndex;
			nMaxLength=rectItem.Width();
			SetHorizontalExtent(nMaxLength);
		}
	}
}


void COXListBoxEx::CalcItemRects(int nIndex, CRect& rectItem,
								 CRect& rectItemVisible, CRect& rectImage,
								 CRect& rectText)
{
	ASSERT(nIndex>=0 && nIndex<GetCount());

	rectItem.SetRectEmpty();
	rectItemVisible.SetRectEmpty();
	rectImage.SetRectEmpty();
	rectText.SetRectEmpty();

	OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
	ASSERT(pLBI!=NULL);

	int nWidthText=0;
	int nHeightText=0;
	int nWidthImage=0;
	int nHeightImage=0;

	// calculating image size
	CalcImageSize(nIndex,nWidthImage,nHeightImage);

	// calculating text size
	CalcTextSize(nIndex,nWidthText,nHeightText);

	// combining two rects
	rectItem.right=pLBI->m_nIndent+(nWidthImage>0 ? OXLB_IMAGEOFFSET_X : 0)+
		nWidthImage+2*OXLB_TEXTOFFSET_X+nWidthText;
	if(nHeightImage+2*OXLB_IMAGEOFFSET_Y>nHeightText+2*OXLB_TEXTOFFSET_Y)
	{
		rectItem.bottom=nHeightImage+2*OXLB_IMAGEOFFSET_Y;
	}
	else
	{
		rectItem.bottom=nHeightText+2*OXLB_TEXTOFFSET_Y;
	}

	if(nWidthImage>0)
	{
		rectImage.left=pLBI->m_nIndent+OXLB_IMAGEOFFSET_X;
		rectImage.top=(rectItem.Height()-nHeightImage)/2;
		rectImage.right=rectImage.left+nWidthImage;
		rectImage.bottom=rectImage.top+nHeightImage;
	}

	rectText.left=pLBI->m_nIndent+(nWidthImage>0 ? OXLB_IMAGEOFFSET_X : 0)+
		nWidthImage+OXLB_TEXTOFFSET_X;
	rectText.top=(rectItem.Height()-nHeightText)/2;
	rectText.right=rectText.left+nWidthText;
	rectText.bottom=rectText.top+nHeightText;

	if(nIndex==GetLastItemBeforeSeparator())
		rectItem.bottom+=OXLB_SEPARATOR_HEIGHT;

	rectItemVisible=rectItem;
	CRect rect;
	GetClientRect(rect);
	if(rect.Width()<rectItemVisible.Width())
		rectItemVisible.right=rect.Width();

	pLBI->m_rectItem=rectItem;
	pLBI->m_rectImage=rectImage;
	pLBI->m_rectText=rectText;
}


void COXListBoxEx::CalcImageSize(int nIndex, int& nWidthImage, int& nHeightImage)
{
	UNREFERENCED_PARAMETER(nIndex);

	if(GetImageList()!=NULL && GetImageList()->GetImageCount()>0)
	{	
		IMAGEINFO imageInfo;
		VERIFY(GetImageList()->GetImageInfo(0,&imageInfo));
		CRect rectImage=imageInfo.rcImage;
		nWidthImage=rectImage.Width();
		nHeightImage=rectImage.Height();
	}
	else
	{
		nWidthImage=0;
		nHeightImage=0;
	}
}


void COXListBoxEx::CalcTextSize(int nIndex, int& nWidthText, int& nHeightText)
{
	if(GetStyle()&LBS_HASSTRINGS)
	{
		OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
		ASSERT(pLBI!=NULL);

		CString sText;
		GetText(nIndex,sText);

		CClientDC dc(this);
		CFont* pOldFont=NULL;
		if(pLBI->m_pFont!=NULL)
			pOldFont=dc.SelectObject(pLBI->m_pFont);

		CRect rectText(0, 0, 0, 0);
		dc.DrawText(sText,rectText,DT_CALCRECT|DT_LEFT|DT_SINGLELINE);

		if(pOldFont!=NULL)
			dc.SelectObject(pOldFont);

		nWidthText=rectText.Width();
		nHeightText=rectText.Height();
	}
	else
	{
		nWidthText=0;
		nHeightText=0;
	}
}


void COXListBoxEx::RecalcLayout()
{
	if(::IsWindow(GetSafeHwnd()))
	{
		RecalcItemsExtent();
		UpdateTooltipTools();
	}
}


BOOL COXListBoxEx::InitializeListBox()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	ASSERT(GetCount()==0);

	CToolTipCtrl* pTooTipCtrl=GetToolTipCtrl();
	ASSERT(pTooTipCtrl!=NULL);
	ASSERT(!::IsWindow(pTooTipCtrl->GetSafeHwnd()));
	BOOL bIsTopMost=(GetExStyle()&WS_EX_TOPMOST);
	pTooTipCtrl->Create(this,(bIsTopMost ? TTS_ALWAYSTIP : 0));
	// make sure tooltip window is displayed even in the case when 
	// list box is popup window
	if(bIsTopMost)
	{
		pTooTipCtrl->SetWindowPos(&wndTopMost,0,0,0,0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	}

	CRect rect;
	GetClientRect(rect);
	if(rect.IsRectEmpty())
	{
		rect.right=rect.left+1;
		rect.bottom=rect.top+1;
	}
	pTooTipCtrl->AddTool(this,GetTooltipText(),rect,ID_OXLB_TOOLTIP);

	UpdateTooltipTools();

	return TRUE;
}


void COXListBoxEx::UpdateTooltipTools()
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// set up tool areas
	CToolTipCtrl* pTooTipCtrl=GetToolTipCtrl();
	ASSERT(pTooTipCtrl!=NULL);
	if(::IsWindow(pTooTipCtrl->GetSafeHwnd()))
    {
        // delete all existing tools
        int nToolCount=pTooTipCtrl->GetToolCount();
        for(int nIndex=1; nIndex<nToolCount; nIndex++)
        {
            pTooTipCtrl->DelTool(this,nIndex);
        }

        ASSERT(pTooTipCtrl->GetToolCount()==1);

        // now add new tools
        CRect rect;
        GetClientRect(rect);
        int nTopItemIndex=GetTopIndex();

        if(nTopItemIndex>=0)
        {
			CRect lineRect=rect;
            lineRect.bottom=GetItemHeight(nTopItemIndex);

			int nTooltipIndex=1;
			for(int nIndex=nTopItemIndex; nIndex<m_mapItems.GetCount(); nIndex++)
            {
				OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
				ASSERT(pLBI!=NULL);

				if(!pLBI->m_sTooltipText.IsEmpty())
				{
					if(GetHighlightOnlyText())
					{
						if(lineRect.left<pLBI->m_rectImage.left)
							lineRect.left=pLBI->m_rectImage.left;
						else if(lineRect.left<pLBI->m_rectText.left)
							lineRect.left=pLBI->m_rectText.left;
						if(lineRect.right>pLBI->m_rectText.right)
							lineRect.right=pLBI->m_rectText.right;
					}
					VERIFY(pTooTipCtrl->AddTool(this,pLBI->m_sTooltipText,
						lineRect,nTooltipIndex));
					nTooltipIndex++;
				}

                lineRect.top=lineRect.bottom;
				if(lineRect.top>=rect.bottom)
					break;
                lineRect.bottom=lineRect.top+GetItemHeight(nIndex+1);
            }
        }
    }
}


int COXListBoxEx::MRUInsert(int nItemIndex, int nMRUIndex, 
							BOOL bForceToInsert/*=TRUE*/)
{
	ASSERT(GetMaxItemsBeforeSeparator()==-1 ||
		GetLastItemBeforeSeparator()+1<=GetMaxItemsBeforeSeparator());

	if(nItemIndex<0 || nItemIndex>=GetCount())
	{
		TRACE(_T("COXListBoxEx::MRUInsert: failed to move to MRU list - the item is not found\n"));
		return LB_ERR;
	}

	if(nMRUIndex<0 || nMRUIndex>GetLastItemBeforeSeparator()+1 ||
		(GetMaxItemsBeforeSeparator()!=-1 && nMRUIndex>GetMaxItemsBeforeSeparator()))
	{
		TRACE(_T("COXListBoxEx::MRUInsert: failed to move to MRU list - invalid MRU index has been specified\n"));
		return LB_ERR;
	}

	if(!bForceToInsert && GetMaxItemsBeforeSeparator()!=-1 &&
		GetLastItemBeforeSeparator()+1==GetMaxItemsBeforeSeparator() && 
		nItemIndex>GetLastItemBeforeSeparator())
	{
		TRACE(_T("COXListBoxEx::MRUInsert: failed to move to MRU list - the max number of MRU items has been reached\n"));
		return LB_ERR;
	}

	if(GetLastItemBeforeSeparator()+1==GetMaxItemsBeforeSeparator() &&
		nItemIndex>GetLastItemBeforeSeparator())
	{
		ASSERT(bForceToInsert);

		int nNewIndex=MRUDelete(GetLastItemBeforeSeparator());
		if(nNewIndex==LB_ERR)
			return LB_ERR;
		if(nItemIndex<=nNewIndex && nItemIndex>GetLastItemBeforeSeparator())
			nItemIndex--;
	}

	// preserve the data associated with the item
	OXLISTBOXITEM* pLBI=GetItemInfo(nItemIndex);
	ASSERT(pLBI!=NULL);

	if(nItemIndex==nMRUIndex)
	{
		if(pLBI->m_nMRUIndex>=0)
		{
			ASSERT(pLBI->m_nMRUIndex==nItemIndex);
		}
		else
		{
			ASSERT(GetLastItemBeforeSeparator()+1==nItemIndex);
			pLBI->m_nMRUIndex=nItemIndex;
			m_nLastItemBeforeSeparator++;
			RecalcItemsExtent();
			UpdateTooltipTools();
		}
		return nMRUIndex;
	}

	OXLISTBOXITEM lbiCopy=*pLBI;
	CString sText;
	GetText(nItemIndex,sText);

	DWORD_PTR dwData=GetItemData(nItemIndex);

	BOOL bIsSelected=FALSE;
	BOOL bIsFocus=FALSE;
	BOOL bIsMultipleSel=(GetStyle()&LBS_MULTIPLESEL);
	if(bIsMultipleSel)
	{
		bIsSelected=GetSel(nItemIndex);
		bIsFocus=(GetCaretIndex()==nItemIndex);
	}
	else
	{
		bIsSelected=(GetCurSel()==nItemIndex);
		bIsFocus=bIsSelected;
	}

	// delete item
	VERIFY(DeleteString(nItemIndex)!=LB_ERR);

	// update MRU list
	for(int nIndex=nMRUIndex; nIndex<=GetLastItemBeforeSeparator(); nIndex++)
	{
		OXLISTBOXITEM* pLBI=GetItemInfo(nIndex);
		ASSERT(pLBI!=NULL);
		ASSERT(pLBI->m_nMRUIndex==nIndex);
		pLBI->m_nMRUIndex++;
	}
	m_nLastItemBeforeSeparator++;

	// insert item
	VERIFY(InsertString(nMRUIndex,sText,OXLBI_ALL,&lbiCopy)==nMRUIndex);
	pLBI=GetItemInfo(nMRUIndex);
	ASSERT(pLBI!=NULL);
	pLBI->m_nMRUIndex=nMRUIndex;

	// restore data
	VERIFY(SetItemData(nMRUIndex,dwData)!=LB_ERR);
	if(bIsMultipleSel)
	{
		if(bIsSelected)
			VERIFY(SetSel(nMRUIndex,TRUE)!=LB_ERR);
		if(bIsFocus)
			VERIFY(SetCaretIndex(nMRUIndex,TRUE)!=LB_ERR);
	}
	else
	{
		if(bIsSelected)
			VERIFY(SetCurSel(nMRUIndex)!=LB_ERR);
	}

	RecalcItemsExtent();
	UpdateTooltipTools();

	return nMRUIndex;
}


int COXListBoxEx::MRUDelete(int nMRUIndex)
{
	if(nMRUIndex<0 || nMRUIndex>GetLastItemBeforeSeparator())
	{
		TRACE(_T("COXListBoxEx::MRUDelete: failed to delete from MRU list - invalid MRU index has been specified\n"));
		return LB_ERR;
	}

	// preserve the data associated with the item
	OXLISTBOXITEM* pLBI=GetItemInfo(nMRUIndex);
	ASSERT(pLBI!=NULL);
	OXLISTBOXITEM lbiCopy=*pLBI;
	ASSERT(lbiCopy.m_nMRUIndex==nMRUIndex);

	CString sText;
	GetText(nMRUIndex,sText);
	
	DWORD_PTR dwData=GetItemData(nMRUIndex);

	BOOL bIsSelected=FALSE;
	BOOL bIsFocus=FALSE;
	BOOL bIsMultipleSel=(GetStyle()&LBS_MULTIPLESEL);
	if(bIsMultipleSel)
	{
		bIsSelected=GetSel(nMRUIndex);
		bIsFocus=(GetCaretIndex()==nMRUIndex);
	}
	else
	{
		bIsSelected=(GetCurSel()==nMRUIndex);
		bIsFocus=bIsSelected;
	}

	// delete item
	VERIFY(DeleteString(nMRUIndex)!=LB_ERR);

	// insert item
	int nItemIndex=AddFormerMRUString(sText,OXLBI_ALL,&lbiCopy);
	ASSERT(nItemIndex!=LB_ERR);
	pLBI=GetItemInfo(nItemIndex);
	ASSERT(pLBI!=NULL);
	pLBI->m_nMRUIndex=-1;

	// restore data
	VERIFY(SetItemData(nItemIndex,dwData)!=LB_ERR);
	if(bIsMultipleSel)
	{
		if(bIsSelected)
			VERIFY(SetSel(nItemIndex,TRUE)!=LB_ERR);
		if(bIsFocus)
			VERIFY(SetCaretIndex(nItemIndex,TRUE)!=LB_ERR);
	}
	else
	{
		if(bIsSelected)
			VERIFY(SetCurSel(nItemIndex)!=LB_ERR);
	}

	return nItemIndex;

}


BOOL COXListBoxEx::RestoreMRUState(LPCTSTR lpszSectionName/*=_T("OXLISTBOXEX_MRU")*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	CString sSectionName=lpszSectionName;
	ASSERT(!sSectionName.IsEmpty());

	// restore MRU list

	CWinApp* pApp=AfxGetApp();
	ASSERT(pApp!=NULL);

	int nMaxItemsBeforeSeparator=pApp->GetProfileInt(sSectionName,
		_T("MRU_MaxNumber"),GetMaxItemsBeforeSeparator());
	SetMaxItemsBeforeSeparator(nMaxItemsBeforeSeparator);
	int nCount=pApp->GetProfileInt(sSectionName,_T("MRU_Count"),
		nMaxItemsBeforeSeparator);
	ASSERT(nCount<=nMaxItemsBeforeSeparator || nMaxItemsBeforeSeparator==-1);

	int nIndex=0;
	for(nIndex=GetLastItemBeforeSeparator(); nIndex>=0;	nIndex--)
	{
		VERIFY(MRUDelete(nIndex)!=LB_ERR);
	}

	for(nIndex=0; nIndex<nCount; nIndex++)
	{
		CString sEntryName;
		sEntryName.Format(_T("MRU_Entry_%d"),nIndex);
		CString sText=pApp->GetProfileString(sSectionName,sEntryName,_T(""));
		if(!sText.IsEmpty())
		{
			int nFoundIndex=FindStringExact(-1,sText);
			if(nFoundIndex!=LB_ERR)
			{
				VERIFY(MRUAdd(nFoundIndex)!=LB_ERR);
			}
		}
		else
		{
			break;
		}
	}

	return TRUE;
}


BOOL COXListBoxEx::SaveMRUState(LPCTSTR lpszSectionName/*=_T("OXLISTBOXEX_MRU")*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	CString sSectionName=lpszSectionName;
	ASSERT(!sSectionName.IsEmpty());

	// save MRU list

	CWinApp* pApp=AfxGetApp();
	ASSERT(pApp!=NULL);

	pApp->WriteProfileInt(sSectionName,_T("MRU_MaxNumber"),
		GetMaxItemsBeforeSeparator());
	pApp->WriteProfileInt(sSectionName,_T("MRU_Count"),
		GetLastItemBeforeSeparator()+1);

	for(int nIndex=0; nIndex<=GetLastItemBeforeSeparator(); nIndex++)
	{
		CString sEntryName;
		sEntryName.Format(_T("MRU_Entry_%d"),nIndex);
		CString sText;
		GetText(nIndex,sText);
		ASSERT(!sText.IsEmpty());
		pApp->WriteProfileString(sSectionName,sEntryName,sText);
	}

	return TRUE;
}


BOOL COXListBoxEx::IsWindowsNTRunning()
{
	BOOL bResult=FALSE;

	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	if (::GetVersionEx(&verInfo))
	{
		if (verInfo.dwPlatformId==VER_PLATFORM_WIN32_NT && 
			verInfo.dwMajorVersion>=4)
		{
			bResult=TRUE;
		}
	}

	return bResult;
}

BOOL COXListBoxEx::OnMouseWheel( UINT /*nFlags*/, short zDelta, CPoint /*pt*/ )
{
	if ( zDelta > 0 )
		SetTopIndex( GetTopIndex() - 1 );
	else
		SetTopIndex( GetTopIndex() + 1 );

	return TRUE;
}

///////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
int FontEntrySortFunc(const void* pFont1, const void* pFont2)
{
	ASSERT(pFont1!=NULL);
	ASSERT(pFont2!=NULL);

	OXLBFONTINFO* pFontInfo1=(OXLBFONTINFO*)pFont1;
	OXLBFONTINFO* pFontInfo2=(OXLBFONTINFO*)pFont2;

    return _tcsicmp(pFontInfo1->m_lf.lfFaceName,pFontInfo2->m_lf.lfFaceName);
}
