////////////////////////////////////////////////////////////////////////////
// File:	RichToolTipCtrl.cpp
// Version:	1.1
// Created:	22-Mar-2005
//
// Author:	Paul S. Vickery
// E-mail:	developer@vickeryhome.freeserve.co.uk
//
// Class to provide a means of using rich text in a tool-tip, without using 
// the newer styles only available in later systems. Based on CToolTipCtrl.
//
// You are free to use or modify this code, with no restrictions, other than
// you continue to acknowledge me as the original author in this source code,
// or any code derived from it.
//
// If you use this code, or use it as a base for your own code, it would be 
// nice to hear from you simply so I know it's not been a waste of time!
//
// Copyright (c) 2005 Paul S. Vickery
//
////////////////////////////////////////////////////////////////////////////
// Version History:
//
// Version 1.1 - 22-Mar-2005
// =========================
// - Added static method for escaping plain text to make it RTF safe
// - Modified the positioning to work on multi-monitor systems
// - Removed dependency on RichToolTipCtrlDemo.h
// 
// Version 1.0 - 14-Mar-2005
// =========================
// Initial version
// 
////////////////////////////////////////////////////////////////////////////
// PLEASE LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RichToolTipCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef __AFXDISP_H__
#error You need to #include <afxdisp.h> here or in your stdafx.h
#endif
#include "atlconv.h"    // for Unicode conversion - requires #include <afxdisp.h> // MFC OLE automation classes

#include <afxole.h> // for COleClientItem
#define COMPILE_MULTIMON_STUBS	// for late-binding to multi-monitor functions
#include <multimon.h> // for multi-monitor support

/////////////////////////////////////////////////////////////////////////////
// CRichToolTipCtrl::CRichToolTipRichEditCtrl

CRichToolTipCtrl::CRichToolTipRichEditCtrl::CRichToolTipRichEditCtrl()
{
}

CRichToolTipCtrl::CRichToolTipRichEditCtrl::~CRichToolTipRichEditCtrl()
{
}

BEGIN_MESSAGE_MAP(CRichToolTipCtrl::CRichToolTipRichEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichToolTipCtrl::CRichToolTipRichEditCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichToolTipCtrl::CRichToolTipRichEditCtrl message handlers

// cookie class to assist with streaming a CString into the richedit control
class _RichToolTipCtrlCookie
{
public:
  _RichToolTipCtrlCookie(CString sText) : m_sText(sText) { m_dwCount = 0; m_dwLength = m_sText.GetLength(); }

  DWORD Read(LPSTR lpszBuffer, DWORD dwCount);
  void Reset() { m_dwCount = 0; m_dwLength = m_sText.GetLength(); }

protected:
  CString m_sText;
  DWORD m_dwLength;
  DWORD m_dwCount;
};

// read dwCount bytes into lpszBuffer, and return number read
// stop if source is empty, or when end of string reached
DWORD _RichToolTipCtrlCookie::Read(LPSTR lpszBuffer, DWORD dwCount)
{
  if (lpszBuffer == NULL)
    return -1;

  // have we already had it all?
  DWORD dwLeft = m_dwLength - m_dwCount;
  if (dwLeft <= 0)  // all done
    return 0;

  // start the source string from where we left off
  USES_CONVERSION;
  LPCSTR lpszText = (LPCSTR)T2A(m_sText.GetBuffer(0)) + m_dwCount;

  // only copy what we've got left
  if (dwLeft < dwCount)
    dwCount = dwLeft;

  // copy the text
  strncpy(lpszBuffer, lpszText, dwCount);

  // keep where we got to
  m_dwCount += dwCount;

  // return how many we copied
  return dwCount;
}

static DWORD CALLBACK RichTextCtrlCallbackIn(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
  // the cookie is a pointer to the text data struct
  _RichToolTipCtrlCookie* pBuf = (_RichToolTipCtrlCookie*)dwCookie;
  if (pBuf == NULL)
    return 1;
  *pcb = pBuf->Read((LPSTR)pbBuff, cb);
  return 0;
}

int CRichToolTipCtrl::CRichToolTipRichEditCtrl::StreamTextIn(LPCTSTR lpszText)
{
  EDITSTREAM es;
  _RichToolTipCtrlCookie data(lpszText);
  es.dwCookie = (DWORD)&data;
  es.pfnCallback = RichTextCtrlCallbackIn;
  int n = StreamIn(SF_RTF, es);
  if (n <= 0)
  {
    data.Reset();
    n = StreamIn(SF_TEXT, es);
  }
  return n;
}

/////////////////////////////////////////////////////////////////////////////
// CRichToolTipCtrl::XRichEditOleCallback

BEGIN_INTERFACE_MAP(CRichToolTipCtrl, CToolTipCtrl)
	// we use IID_IUnknown because richedit doesn't define an IID
	INTERFACE_PART(CRichToolTipCtrl, IID_IUnknown, RichEditOleCallback)
END_INTERFACE_MAP()

STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::GetNewStorage(LPSTORAGE* ppstg)
{
	// Create a flat storage and steal it from the client item
	// the client item is only used for creating the storage
	COleClientItem item;
	item.GetItemStorageFlat();
	*ppstg = item.m_lpStorage;
	HRESULT hRes = E_OUTOFMEMORY;
	if (item.m_lpStorage != NULL)
	{
		item.m_lpStorage = NULL;
		hRes = S_OK;
	}
	return hRes;
}

/////////////////////////////////////////////////////////////////////////////
// CRichToolTipCtrl

IMPLEMENT_DYNAMIC(CRichToolTipCtrl, CToolTipCtrl)

CRichToolTipCtrl::CRichToolTipCtrl()
{
  AfxInitRichEdit();
}

CRichToolTipCtrl::~CRichToolTipCtrl()
{
}

BEGIN_MESSAGE_MAP(CRichToolTipCtrl, CToolTipCtrl)
	//{{AFX_MSG_MAP(CRichToolTipCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(TTN_SHOW, OnShow)
END_MESSAGE_MAP()

/*static*/
CString CRichToolTipCtrl::MakeTextRTFSafe(LPCTSTR lpszText)
{
  // modify the specified text to make it safe for use in a rich-edit 
  // control, by escaping special RTF characters '\', '{' and '}'
  CString sRTF;
  if (lpszText != NULL)
  {
    while (*lpszText != '\0')
    {
      if (*lpszText == _T('\\') || *lpszText == _T('{') || *lpszText == _T('}'))
	sRTF += _T('\\');
      else if (*lpszText == _T('\r') && *(lpszText+1) == _T('\n') || 
	      (*lpszText == _T('\n') && *(lpszText+1) == _T('\r')))
      {
	sRTF += _T("{\\par}");
	sRTF += *lpszText;
	lpszText++;
      }
      else if (*(lpszText) == _T('\n') || *(lpszText) == _T('\r'))
	sRTF += _T("{\\par}");
      sRTF += *lpszText;
      lpszText++;
    }
  }
  return sRTF;
}

/////////////////////////////////////////////////////////////////////////////
// CRichToolTipCtrl message handlers

int CRichToolTipCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CToolTipCtrl::OnCreate(lpCreateStruct) == -1)
    return -1;

  // set the max width to half of the screen width, and 
  // force the ability to use CRLFs to make multi-line tips
  SetMaxTipWidth(::GetSystemMetrics(SM_CXSCREEN) / 2);

  // create a hidden child richedit control
  m_edit.Create(WS_CHILD | ES_MULTILINE | ES_READONLY, CRect(0, 0, 0, 0), this, 1);

  // set the edit control's ole callback handler
  VERIFY(m_edit.SetOLECallback(&m_xRichEditOleCallback));

  return 0;
}

BOOL CRichToolTipCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
  NMHDR* pnmhdr = (NMHDR*)lParam;
  if (pnmhdr != NULL)
  {
    if (pnmhdr->code == EN_REQUESTRESIZE)
    {
      REQRESIZE* prr = (REQRESIZE*)pnmhdr;
      m_sizeEditMin.cx = prr->rc.right - prr->rc.left;
      m_sizeEditMin.cy = prr->rc.bottom - prr->rc.top;
    }
  }
  return CToolTipCtrl::OnNotify(wParam, lParam, pResult);
}

// this code gets the minimium size for a rich edit control, and is 
// taken from http://www.codeproject.com/richedit/richeditsize.asp
CSize CRichToolTipCtrl::CalculateMinimiumRichEditSize()
{
  m_edit.SetEventMask(ENM_REQUESTRESIZE);

  // m_sizeEditMin is a CSize object that stores m_edit required size
  m_sizeEditMin.cx = m_sizeEditMin.cy = 0;

  // Performing the binary search for the best dimension
  int cxFirst = 0;
  int cxLast = GetMaxTipWidth();
  if (cxLast < 0)
    cxLast = ::GetSystemMetrics(SM_CXSCREEN);
  int cyMin = 0;

  cxLast *= 2;	// cos the first thing we do it divide it by two

  do
  {
    // Taking a guess
    int cx = (cxFirst + cxLast) / 2;

    // Testing this guess
    CRect rc(0, 0, cx, 1);
    m_edit.MoveWindow(rc);
    m_edit.SetRect(rc);
    m_edit.RequestResize();

    // If it's the first time, take the result anyway.
    // This is the minimum height the control needs
    if (cyMin == 0)
       cyMin = m_sizeEditMin.cy;

    // Iterating
    if (m_sizeEditMin.cy > cyMin)
    {
       // If the control required a larger height, then
       // it's too narrow.
       cxFirst = cx + 1;
    }
    else
    {
       // If the control didn't required a larger height,
       // then it's too wide.
       cxLast = cx - 1;
    }
  }
  while (cxFirst < cxLast);

  if (m_sizeEditMin.cy > cyMin)
  {
    // it's gone wrong. so add one to the last width, and do it again
    // it does this sometimes, depending on the exact figures
    CRect rc(0, 0, cxLast + 1, 1);
    m_edit.MoveWindow(rc);
    m_edit.SetRect(rc);
    m_edit.RequestResize();
    _ASSERTE(m_sizeEditMin.cy == cyMin);
    // if the above assert fails, then remove it: we're probably not going to get it any better
  }

  // Giving it a few pixels extra width for safety
  m_sizeEditMin.cx += 2;
  m_sizeEditMin.cy += 2;

  // Moving the control
  m_edit.MoveWindow(0, 0, m_sizeEditMin.cx, m_sizeEditMin.cy);

  return m_sizeEditMin;
}

void CRichToolTipCtrl::OnShow(NMHDR* pNMHDR, LRESULT* pResult)
{
  *pResult = 0;

  // the control is about to be displayed
  // we can update the edit control, and re-set the text

  // make sure the edit control is using the correct font and colours
  // and make sure we don't have any formatting effects set
  m_edit.SetBackgroundColor(FALSE, GetTipBkColor());
  m_edit.SetWindowText(_T(""));
  CHARFORMAT cf;
  ZeroMemory(&cf, sizeof(CHARFORMAT));
  cf.cbSize = sizeof(CHARFORMAT);
  cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_ITALIC | CFM_PROTECTED | CFM_STRIKEOUT | CFM_UNDERLINE;
  cf.crTextColor = GetTipTextColor();
  m_edit.SetDefaultCharFormat(cf);
  m_edit.SetFont(GetFont());

  CString sText;
  GetWindowText(sText);
  m_edit.StreamTextIn(sText);

  SetWindowText(_T(""));

  // find out how big the tip window should be
  CSize size = CalculateMinimiumRichEditSize();

  // set the tool-tip's drawing rect
  size.cx += 5; // add a few more on, cos we'll use them for a margin later
  size.cy += 5;

  // it seems that if any text is right-aligned, or centered, then
  // it just creeps over the right-hand edge
  // the best we can do to see if it has any right or centre text
  // is to see if the RTF contains "\qr" or "\qc" tags, and add a bit on
  // (we don't cater for text having "\qr" or "\qc" somewhere in it; 
  // in this case we add on the extra anyway. If we cared we could look for 
  // a "\qr" which is not really a "\\qr", but that could be quite expensive)
  int nPos = sText.Find(_T("\\q"));
  if (nPos >= 0 && (sText[nPos + 2] == _T('r') || sText[nPos + 2] == _T('c')))
    size.cx += 5;

  // calc the new tip window size and position, and move it
  CRect rc;
  GetWindowRect(rc);
  // position the tip with its left edge at the mouse's x
  CPoint ptCursor;
  GetCursorPos(&ptCursor);
  rc.left = ptCursor.x;
  rc.top = ptCursor.y + 16;
  // move it to the left if it's going to go off the edge of the screen
  // but don't move it off the left of the current desktop
  int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
  int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
  CRect rcDesktop(0, 0, cxScreen, cyScreen);
  // adjust the desktop rect for the monitor that contains the cursor
  if (! rcDesktop.PtInRect(ptCursor))
  {
    HMONITOR hMonitor = MonitorFromPoint(ptCursor, MONITOR_DEFAULTTONEAREST);
    if (hMonitor != NULL)
    {
      MONITORINFO mi;
      mi.cbSize = sizeof(MONITORINFO);
      if (GetMonitorInfo(hMonitor, &mi))
	rcDesktop = mi.rcMonitor;
    }
  }
  if ((size.cx + rc.left) > rcDesktop.Width())
    rc.left = max(rcDesktop.left, rc.left - ((rc.left + size.cx) - rcDesktop.Width()));
  // position it above the mouse if it would go off the bottom of the screen
  // but don't move it off the top of the screen
  if ((size.cy + rc.top) > rcDesktop.Height())
    rc.top = max(rcDesktop.top, (ptCursor.y - 16) - size.cy);
  rc.right = rc.left + (size.cx);
  rc.bottom = rc.top + (size.cy);
  MoveWindow(&rc);
  *pResult = TRUE;  // we've moved it
}

void CRichToolTipCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
  LPNMTTCUSTOMDRAW lpttcd = (LPNMTTCUSTOMDRAW)pNMHDR;
  *pResult = CDRF_DODEFAULT;

  if (! ::IsWindow(m_edit.m_hWnd))
    return; // can't do it!

  switch (lpttcd->nmcd.dwDrawStage)
  {
  case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYPOSTPAINT;	// we want post-paint notifications

      // set the text and back colours of the default text
      // so it becomes invisible
      COLORREF crTipBk = GetTipBkColor();
      ::SetTextColor(lpttcd->nmcd.hdc, crTipBk);
      ::SetBkColor(lpttcd->nmcd.hdc, crTipBk);
    }
    break;

  case CDDS_POSTPAINT:
    {
      // FormatRange needs its rect in twips
      // a twip is 1/20 of a printer's point (1,440 twips equal one inch)
      int nLogPixelsX = ::GetDeviceCaps(lpttcd->nmcd.hdc, LOGPIXELSX);
      int nLogPixelsY = ::GetDeviceCaps(lpttcd->nmcd.hdc, LOGPIXELSY);

      // get the drawing rect, and convert to twips
      CRect rc(lpttcd->nmcd.rc);
      rc.DeflateRect(1, 1, 1, 1); // give it a small margin
      rc.left = MulDiv(rc.left, 1440, nLogPixelsX);
      rc.right = MulDiv(rc.right, 1440, nLogPixelsX);
      rc.top = MulDiv(rc.top, 1440, nLogPixelsY);
      rc.bottom = MulDiv(rc.bottom, 1440, nLogPixelsY);

      // use the rich edit control to draw to our device context
      FORMATRANGE fr;
      fr.hdc = lpttcd->nmcd.hdc;
      fr.hdcTarget = lpttcd->nmcd.hdc;
      fr.chrg.cpMin = 0;
      fr.chrg.cpMax = -1;
      fr.rc = rc; // in twips
      fr.rcPage = fr.rc;
      m_edit.FormatRange(&fr, TRUE);
      m_edit.FormatRange(NULL, FALSE);	// get the control to free its cached info
      *pResult = CDRF_SKIPDEFAULT;  // we don't want the default drawing
    }
    break;
  }
}
