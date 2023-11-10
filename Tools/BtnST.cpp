#include "stdafx.h"
#include "BtnST.h"

#ifdef	BTNST_USE_SOUND
#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonST

// Mask for control's type
#ifndef BS_TYPEMASK
#define BS_TYPEMASK SS_TYPEMASK
#endif

#ifndef	TTM_SETTITLE
#define TTM_SETTITLEA           (WM_USER + 32)  // wParam = TTI_*, lParam = char* szTitle
#define TTM_SETTITLEW           (WM_USER + 33)  // wParam = TTI_*, lParam = wchar* szTitle
#ifdef	UNICODE
#define TTM_SETTITLE            TTM_SETTITLEW
#else
#define TTM_SETTITLE            TTM_SETTITLEA
#endif
#endif

#ifndef	TTS_BALLOON
#define	TTS_BALLOON		0x40
#endif
#pragma warning (disable:4100)

CButtonST::CButtonST()
{
	m_bIsPressed		= FALSE;
	m_bIsFocused		= FALSE;
	m_bIsDisabled		= FALSE;
	m_bMouseOnButton	= FALSE;

	FreeResources(FALSE);

	// Default type is "flat" button
	m_bIsFlat = TRUE;
	// Button will be tracked also if when the window is inactive (like Internet Explorer)
	m_bAlwaysTrack = TRUE;

	// By default draw border in "flat" button
	m_bDrawBorder = TRUE;

	// By default icon is aligned horizontally
	m_byAlign = ST_ALIGN_HORIZ;

	// By default use usual pressed style
	SetPressedStyle(BTNST_PRESSED_LEFTRIGHT, FALSE);

	// By default, for "flat" button, don't draw the focus rect
	m_bDrawFlatFocus = FALSE;

	// By default the button is not the default button
	m_bIsDefault = FALSE;
	// Invalid value, since type still unknown
	m_nTypeStyle = BS_TYPEMASK;

	// By default the button is not a checkbox
	m_bIsCheckBox = FALSE;
	m_nCheck = 0;

	// Set default colors
	SetDefaultColors(FALSE);

	// No tooltip created
	m_ToolTip.m_hWnd = nullptr;
	m_dwToolTipStyle = 0;

	// Do not draw as a transparent button
	m_bDrawTransparent = FALSE;
	m_pbmpOldBk = nullptr;

	// No URL defined
	SetURL(nullptr);

	// No cursor defined
	m_hCursor = nullptr;

	// No associated menu
#ifndef	BTNST_USE_BCMENU
	m_hMenu = nullptr;
#endif
	m_hParentWndMenu = nullptr;
	m_bMenuDisplayed = FALSE;

	m_bShowDisabledBitmap = TRUE;

	m_ptImageOrg.x = 3;
	m_ptImageOrg.y = 3;

	// No defined callbacks
	::ZeroMemory(&m_csCallbacks, sizeof(m_csCallbacks));

#ifdef	BTNST_USE_SOUND
	// No defined sounds
	::ZeroMemory(&m_csSounds, sizeof(m_csSounds));
#endif
} // End of CButtonST

CButtonST::~CButtonST()
{
	// Restore old bitmap (if any)
	if (m_dcBk.m_hDC && m_pbmpOldBk)
	{
		m_dcBk.SelectObject(m_pbmpOldBk);
	} // if

	FreeResources();

	// Destroy the cursor (if any)
	if (m_hCursor) ::DestroyCursor(m_hCursor);

	// Destroy the menu (if any)
#ifdef	BTNST_USE_BCMENU
	if (m_menuPopup.m_hMenu)	m_menuPopup.DestroyMenu();
#else
	if (m_hMenu)	::DestroyMenu(m_hMenu);
#endif
} // End of ~CButtonST

BEGIN_MESSAGE_MAP(CButtonST, CButton)
    //{{AFX_MSG_MAP(CButtonST)
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_ACTIVATE()
	ON_WM_ENABLE()
	ON_WM_CANCELMODE()
	ON_WM_GETDLGCODE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
#ifdef	BTNST_USE_BCMENU
	ON_WM_MENUCHAR()
	ON_WM_MEASUREITEM()
#endif

	ON_MESSAGE(BM_SETSTYLE, OnSetStyle)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(BM_SETCHECK, OnSetCheck)
	ON_MESSAGE(BM_GETCHECK, OnGetCheck)
END_MESSAGE_MAP()

void CButtonST::FreeResources(BOOL bCheckForNULL)
{
	if (bCheckForNULL)
	{
		// Destroy icons
		// Note: the following two lines MUST be here! even if
		// BoundChecker says they are unnecessary!
		if (m_csIcons[0].hIcon)	::DestroyIcon(m_csIcons[0].hIcon);
		if (m_csIcons[1].hIcon)	::DestroyIcon(m_csIcons[1].hIcon);

		// Destroy bitmaps
		if (m_csBitmaps[0].hBitmap)	::DeleteObject(m_csBitmaps[0].hBitmap);
		if (m_csBitmaps[1].hBitmap)	::DeleteObject(m_csBitmaps[1].hBitmap);

		// Destroy mask bitmaps
		if (m_csBitmaps[0].hMask)	::DeleteObject(m_csBitmaps[0].hMask);
		if (m_csBitmaps[1].hMask)	::DeleteObject(m_csBitmaps[1].hMask);
	} // if

	::ZeroMemory(&m_csIcons, sizeof(m_csIcons));
	::ZeroMemory(&m_csBitmaps, sizeof(m_csBitmaps));
} // End of FreeResources

void CButtonST::PreSubclassWindow()
{
	UINT nBS;

	nBS = GetButtonStyle();

	// Set initial control type
	m_nTypeStyle = nBS & BS_TYPEMASK;

	// Check if this is a checkbox
	if (nBS & BS_CHECKBOX) m_bIsCheckBox = TRUE;

	// Set initial default state flag
	if (m_nTypeStyle == BS_DEFPUSHBUTTON)
	{
		// Set default state for a default button
		m_bIsDefault = TRUE;

		// Adjust style for default button
		m_nTypeStyle = BS_PUSHBUTTON;
	} // If

	// You should not set the Owner Draw before this call
	// (don't use the resource editor "Owner Draw" or
	// ModifyStyle(0, BS_OWNERDRAW) before calling PreSubclassWindow() )
	ASSERT(m_nTypeStyle != BS_OWNERDRAW);

	// Switch to owner-draw
	ModifyStyle(BS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
} // End of PreSubclassWindow

UINT CButtonST::OnGetDlgCode()
{
	UINT nCode = CButton::OnGetDlgCode();

	// Tell the system if we want default state handling
	// (losing default state always allowed)
	nCode |= (m_bIsDefault ? DLGC_DEFPUSHBUTTON : DLGC_UNDEFPUSHBUTTON);

	return nCode;
} // End of OnGetDlgCode

void CButtonST::OnMove( int, int )
{
	InvalidateRect(nullptr, TRUE);
};

BOOL CButtonST::PreTranslateMessage(MSG* pMsg)
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	return CButton::PreTranslateMessage(pMsg);
} // End of PreTranslateMessage

HBRUSH CButtonST::CtlColor(CDC* pDC, UINT nCtlColor)
{
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
} // End of CtlColor

void CButtonST::OnSysColorChange()
{
	CButton::OnSysColorChange();

	m_dcBk.DeleteDC();
	m_bmpBk.DeleteObject();
	SetDefaultColors();
} // End of OnSysColorChange

LRESULT CButtonST::OnSetStyle(WPARAM wParam, LPARAM lParam)
{
	UINT nNewType = (wParam & BS_TYPEMASK);

	// Update default state flag
	if (nNewType == BS_DEFPUSHBUTTON)
	{
		m_bIsDefault = TRUE;
	} // if
	else if (nNewType == BS_PUSHBUTTON)
	{
		// Losing default state always allowed
		m_bIsDefault = FALSE;
	} // if

	// Can't change control type after owner-draw is set.
	// Let the system process changes to other style bits
	// and redrawing, while keeping owner-draw style
	return DefWindowProc(BM_SETSTYLE,
		(wParam & ~BS_TYPEMASK) | BS_OWNERDRAW, lParam);
} // End of OnSetStyle

LRESULT CButtonST::OnSetCheck(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_bIsCheckBox);

	switch (wParam)
	{
		case BST_CHECKED:
		case BST_INDETERMINATE:	// Indeterminate state is handled like checked state
			SetCheck(1);
			break;
		default:
			SetCheck(0);
			break;
	} // switch

	return 0;
} // End of OnSetCheck

LRESULT CButtonST::OnGetCheck(WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_bIsCheckBox);
	return GetCheck();
} // End of OnGetCheck

#ifdef	BTNST_USE_BCMENU
LRESULT CButtonST::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
	LRESULT lResult;
	if (BCMenu::IsMenu(pMenu))
		lResult = BCMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	else
		lResult = CButton::OnMenuChar(nChar, nFlags, pMenu);
	return lResult;
} // End of OnMenuChar
#endif

#ifdef	BTNST_USE_BCMENU
void CButtonST::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	BOOL bSetFlag = FALSE;
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		if (IsMenu((HMENU)lpMeasureItemStruct->itemID) && BCMenu::IsMenu((HMENU)lpMeasureItemStruct->itemID))
		{
			m_menuPopup.MeasureItem(lpMeasureItemStruct);
			bSetFlag = TRUE;
		} // if
	} // if
	if (!bSetFlag) CButton::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
} // End of OnMeasureItem
#endif

void CButtonST::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);

	if (bEnable == FALSE)
	{
		CWnd*	pWnd = GetParent()->GetNextDlgTabItem(this);
		if (pWnd)
			pWnd->SetFocus();
		else
			GetParent()->SetFocus();

		CancelHover();
	} // if
} // End of OnEnable

void CButtonST::OnKillFocus(CWnd * pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);
	CancelHover();
} // End of OnKillFocus

void CButtonST::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CButton::OnActivate(nState, pWndOther, bMinimized);
	if (nState == WA_INACTIVE)	CancelHover();
} // End of OnActivate

void CButtonST::OnCancelMode()
{
	CButton::OnCancelMode();
	CancelHover();
} // End of OnCancelMode

BOOL CButtonST::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// If a cursor was specified then use it!
	if (m_hCursor != nullptr)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	} // if

	return CButton::OnSetCursor(pWnd, nHitTest, message);
} // End of OnSetCursor

void CButtonST::CancelHover()
{
	// Only for flat buttons
	if (m_bIsFlat)
	{
		if (m_bMouseOnButton)
		{
			m_bMouseOnButton = FALSE;
			Invalidate();
		} // if
	} // if
} // End of CancelHover

void CButtonST::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd*				wndUnderMouse = nullptr;
	CWnd*				wndActive = this;
	TRACKMOUSEEVENT		csTME;

	CButton::OnMouseMove(nFlags, point);

	ClientToScreen(&point);
	wndUnderMouse = WindowFromPoint(point);

	// If the mouse enter the button with the left button pressed then do nothing
	if (nFlags & MK_LBUTTON && m_bMouseOnButton == FALSE) return;

	// If our button is not flat then do nothing
	if (m_bIsFlat == FALSE) return;

	if (m_bAlwaysTrack == FALSE)	wndActive = GetActiveWindow();

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd && wndActive)
	{
		if (!m_bMouseOnButton)
		{
			m_bMouseOnButton = TRUE;

			Invalidate();

#ifdef	BTNST_USE_SOUND
			// Play sound ?
			if (m_csSounds[0].lpszSound)
				::PlaySound(m_csSounds[0].lpszSound, m_csSounds[0].hMod, m_csSounds[0].dwFlags);
#endif

			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		} // if
	} else CancelHover();
} // End of OnMouseMove

// Handler for WM_MOUSELEAVE
LRESULT CButtonST::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	CancelHover();
	return 0;
} // End of OnMouseLeave

BOOL CButtonST::OnClicked()
{
	SetFocus();

#ifdef	BTNST_USE_SOUND
	// Play sound ?
	if (m_csSounds[1].lpszSound)
		::PlaySound(m_csSounds[1].lpszSound, m_csSounds[1].hMod, m_csSounds[1].dwFlags);
#endif

	if (m_bIsCheckBox)
	{
		m_nCheck = !m_nCheck;
		Invalidate();
	} // if
	else
	{
		// Handle the menu (if any)
#ifdef	BTNST_USE_BCMENU
		if (m_menuPopup.m_hMenu)
#else
		if (m_hMenu)
#endif
		{
			CRect	rWnd;
			GetWindowRect(rWnd);

			m_bMenuDisplayed = TRUE;
			Invalidate();

#ifdef	BTNST_USE_BCMENU
			BCMenu* psub = (BCMenu*)m_menuPopup.GetSubMenu(0);
			if (m_csCallbacks.hWnd)	::SendMessage(m_csCallbacks.hWnd, m_csCallbacks.nMessage, (WPARAM)psub, m_csCallbacks.lParam);
			DWORD dwRetValue = psub->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, rWnd.left, rWnd.bottom, this, nullptr);
#else
			HMENU hSubMenu = ::GetSubMenu(m_hMenu, 0);
			if (m_csCallbacks.hWnd)	::SendMessage(m_csCallbacks.hWnd, m_csCallbacks.nMessage, (WPARAM)hSubMenu, m_csCallbacks.lParam);
			DWORD dwRetValue = ::TrackPopupMenuEx(hSubMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, rWnd.left, rWnd.bottom, m_hParentWndMenu, nullptr);
#endif

			m_bMenuDisplayed = FALSE;
			Invalidate();

			if (dwRetValue)
				::PostMessage(m_hParentWndMenu, WM_COMMAND, MAKEWPARAM(dwRetValue, 0), (LPARAM)nullptr);
		} // if
		else
		{
			// Handle the URL (if any)
			if (_tcslen(m_szURL) > 0)
			{
				SHELLEXECUTEINFO	csSEI;

				memset(&csSEI, 0, sizeof(csSEI));
				csSEI.cbSize = sizeof(SHELLEXECUTEINFO);
				csSEI.fMask = SEE_MASK_FLAG_NO_UI;
				csSEI.lpVerb = _T("open");
				csSEI.lpFile = m_szURL;
				csSEI.nShow = SW_SHOWMAXIMIZED;
				::ShellExecuteEx(&csSEI);
			} // if
		} // else
	} // else

	return FALSE;
} // End of OnClicked

void CButtonST::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC*	pDC = CDC::FromHandle(lpDIS->hDC);

	// Checkbox?
	if (m_bIsCheckBox)
	{
		m_bIsPressed  =  (lpDIS->itemState & ODS_SELECTED) || (m_nCheck != 0);
	} // if
	else	// Normal button OR other button style ...
	{
		m_bIsPressed = (lpDIS->itemState & ODS_SELECTED);

		// If there is a menu and it's displayed, draw the button as pressed
		if (
#ifdef	BTNST_USE_BCMENU
			m_menuPopup.m_hMenu
#else
			m_hMenu
#endif
			&& m_bMenuDisplayed)	m_bIsPressed = TRUE;
	} // else

	m_bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
	m_bIsDisabled = (lpDIS->itemState & ODS_DISABLED);

	CRect itemRect = lpDIS->rcItem;

	pDC->SetBkMode(TRANSPARENT);

	// Prepare draw... paint button background

	// Draw transparent?
	if (m_bDrawTransparent)
		PaintBk(pDC);
	else
		OnDrawBackground(pDC, &itemRect);

	// Draw button border
	OnDrawBorder(pDC, &itemRect);

	// Read the button's title
	CString sTitle;
	GetWindowText(sTitle);

	CRect captionRect = lpDIS->rcItem;

	// Draw the icon
	if (m_csIcons[0].hIcon)
	{
		DrawTheIcon(pDC, !sTitle.IsEmpty(), &lpDIS->rcItem, &captionRect, m_bIsPressed, m_bIsDisabled);
	} // if

	if (m_csBitmaps[0].hBitmap)
	{
		pDC->SetBkColor(RGB(255,255,255));
		DrawTheBitmap(pDC, !sTitle.IsEmpty(), &lpDIS->rcItem, &captionRect, m_bIsPressed, m_bIsDisabled);
	} // if

	// Write the button title (if any)
	if (sTitle.IsEmpty() == FALSE)
	{
		DrawTheText(pDC, (LPCTSTR)sTitle, &lpDIS->rcItem, &captionRect, m_bIsPressed, m_bIsDisabled);
	} // if

	if (m_bIsFlat == FALSE || (m_bIsFlat && m_bDrawFlatFocus))
	{
		// Draw the focus rect
		if (m_bIsFocused)
		{
			CRect focusRect = itemRect;
			focusRect.DeflateRect(3, 3);
			pDC->DrawFocusRect(&focusRect);
		} // if
	} // if
} // End of DrawItem

void CButtonST::PaintBk(CDC* pDC)
{
	CClientDC clDC(GetParent());
	CRect rect;
	CRect rect1;

	GetClientRect(rect);

	GetWindowRect(rect1);
	GetParent()->ScreenToClient(rect1);

	if (m_dcBk.m_hDC == nullptr)
	{
		m_dcBk.CreateCompatibleDC(&clDC);
		m_bmpBk.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
		m_pbmpOldBk = m_dcBk.SelectObject(&m_bmpBk);
		m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
	} // if

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcBk, 0, 0, SRCCOPY);
} // End of PaintBk

HBITMAP CButtonST::CreateBitmapMask(HBITMAP hSourceBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTransColor)
{
	HBITMAP		hMask		= nullptr;
	HDC			hdcSrc		= nullptr;
	HDC			hdcDest		= nullptr;
	HBITMAP		hbmSrcT		= nullptr;
	HBITMAP		hbmDestT	= nullptr;
	COLORREF	crSaveBk;
	COLORREF	crSaveDestText;

	hMask = ::CreateBitmap(dwWidth, dwHeight, 1, 1, nullptr);
	if (hMask == nullptr)	return nullptr;

	hdcSrc	= ::CreateCompatibleDC(nullptr);
	hdcDest	= ::CreateCompatibleDC(nullptr);

	hbmSrcT = (HBITMAP)::SelectObject(hdcSrc, hSourceBitmap);
	hbmDestT = (HBITMAP)::SelectObject(hdcDest, hMask);

	crSaveBk = ::SetBkColor(hdcSrc, crTransColor);

	::BitBlt(hdcDest, 0, 0, dwWidth, dwHeight, hdcSrc, 0, 0, SRCCOPY);

	crSaveDestText = ::SetTextColor(hdcSrc, RGB(255, 255, 255));
	::SetBkColor(hdcSrc,RGB(0, 0, 0));

	::BitBlt(hdcSrc, 0, 0, dwWidth, dwHeight, hdcDest, 0, 0, SRCAND);

	SetTextColor(hdcDest, crSaveDestText);

	::SetBkColor(hdcSrc, crSaveBk);
	::SelectObject(hdcSrc, hbmSrcT);
	::SelectObject(hdcDest, hbmDestT);

	::DeleteDC(hdcSrc);
	::DeleteDC(hdcDest);

	return hMask;
} // End of CreateBitmapMask

//
// Parameters:
//		[IN]	bHasTitle
//				TRUE if the button has a text
//		[IN]	rpItem
//				A pointer to a RECT structure indicating the allowed paint area
//		[IN/OUT]rpTitle
//				A pointer to a CRect object indicating the paint area reserved for the
//				text. This structure will be modified if necessary.
//		[IN]	bIsPressed
//				TRUE if the button is currently pressed
//		[IN]	dwWidth
//				Width of the image (icon or bitmap)
//		[IN]	dwHeight
//				Height of the image (icon or bitmap)
//		[OUT]	rpImage
//				A pointer to a CRect object that will receive the area available to the image
//
void CButtonST::PrepareImageRect(BOOL bHasTitle, RECT* rpItem, CRect* rpTitle, BOOL bIsPressed, DWORD dwWidth, DWORD dwHeight, CRect* rpImage)
{
	CRect rBtn;

	rpImage->CopyRect(rpItem);

	switch (m_byAlign)
	{
		case ST_ALIGN_HORIZ:
			if (bHasTitle == FALSE)
			{
				// Center image horizontally
				rpImage->left += ((rpImage->Width() - (long)dwWidth)/2);
			}
			else
			{
				// Image must be placed just inside the focus rect
				rpImage->left += m_ptImageOrg.x;
				rpTitle->left += dwWidth + m_ptImageOrg.x;
			}
			// Center image vertically
			rpImage->top += ((rpImage->Height() - (long)dwHeight)/2);
			break;

		case ST_ALIGN_HORIZ_RIGHT:
			GetClientRect(&rBtn);
			if (bHasTitle == FALSE)
			{
				// Center image horizontally
				rpImage->left += ((rpImage->Width() - (long)dwWidth)/2);
			}
			else
			{
				// Image must be placed just inside the focus rect
				rpTitle->right = rpTitle->Width() - dwWidth - m_ptImageOrg.x;
				rpTitle->left = m_ptImageOrg.x;
				rpImage->left = rBtn.right - dwWidth - m_ptImageOrg.x;
				// Center image vertically
				rpImage->top += ((rpImage->Height() - (long)dwHeight)/2);
			}
			break;

		case ST_ALIGN_VERT:
			// Center image horizontally
			rpImage->left += ((rpImage->Width() - (long)dwWidth)/2);
			if (bHasTitle == FALSE)
			{
				// Center image vertically
				rpImage->top += ((rpImage->Height() - (long)dwHeight)/2);
			}
			else
			{
				rpImage->top = m_ptImageOrg.y;
				rpTitle->top += dwHeight;
			}
			break;

		case ST_ALIGN_OVERLAP:
			break;
	} // switch

	// If button is pressed then press image also
	if (bIsPressed && m_bIsCheckBox == FALSE)
		rpImage->OffsetRect(m_ptPressedOffset.x, m_ptPressedOffset.y);
} // End of PrepareImageRect

void CButtonST::DrawTheIcon(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	BYTE		byIndex		= 0;

	// Select the icon to use
	if ((m_bIsCheckBox && bIsPressed) || (!m_bIsCheckBox && (bIsPressed || m_bMouseOnButton)))
		byIndex = 0;
	else
		byIndex = (m_csIcons[1].hIcon == nullptr ? 0 : 1);

	CRect	rImage;
	PrepareImageRect(bHasTitle, rpItem, rpCaption, bIsPressed, m_csIcons[byIndex].dwWidth, m_csIcons[byIndex].dwHeight, &rImage);

	// Ole'!
	pDC->DrawState(	rImage.TopLeft(),
					rImage.Size(),
					m_csIcons[byIndex].hIcon,
					(bIsDisabled ? DSS_DISABLED : DSS_NORMAL),
					(CBrush*)nullptr);
} // End of DrawTheIcon

void CButtonST::DrawTheBitmap(CDC* pDC, BOOL bHasTitle, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	HDC			hdcBmpMem	= nullptr;
	HBITMAP		hbmOldBmp	= nullptr;
	HDC			hdcMem		= nullptr;
	HBITMAP		hbmT		= nullptr;

	BYTE		byIndex		= 0;

	// Select the bitmap to use
	if ((m_bIsCheckBox && bIsPressed) || (!m_bIsCheckBox && (bIsPressed || m_bMouseOnButton)))
		byIndex = 0;
	else
		byIndex = (m_csBitmaps[1].hBitmap == nullptr ? 0 : 1);

	CRect	rImage;
	PrepareImageRect(bHasTitle, rpItem, rpCaption, bIsPressed, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, &rImage);

	hdcBmpMem = ::CreateCompatibleDC(pDC->m_hDC);

	hbmOldBmp = (HBITMAP)::SelectObject(hdcBmpMem, m_csBitmaps[byIndex].hBitmap);

	hdcMem = ::CreateCompatibleDC(nullptr);

	hbmT = (HBITMAP)::SelectObject(hdcMem, m_csBitmaps[byIndex].hMask);

	if (bIsDisabled && m_bShowDisabledBitmap)
	{
		HDC		hDC = nullptr;
		HBITMAP	hBitmap = nullptr;

		hDC = ::CreateCompatibleDC(pDC->m_hDC);
		hBitmap = ::CreateCompatibleBitmap(pDC->m_hDC, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight);
		HBITMAP	hOldBmp2 = (HBITMAP)::SelectObject(hDC, hBitmap);

		RECT	rRect;
		rRect.left = 0;
		rRect.top = 0;
		rRect.right = rImage.right + 1;
		rRect.bottom = rImage.bottom + 1;
		::FillRect(hDC, &rRect, (HBRUSH)RGB(255, 255, 255));

		COLORREF crOldColor = ::SetBkColor(hDC, RGB(255,255,255));

		::BitBlt(hDC, 0, 0, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcMem, 0, 0, SRCAND);
		::BitBlt(hDC, 0, 0, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcBmpMem, 0, 0, SRCPAINT);

		::SetBkColor(hDC, crOldColor);
		::SelectObject(hDC, hOldBmp2);
		::DeleteDC(hDC);

		pDC->DrawState(	CPoint(rImage.left/*+1*/, rImage.top),
						CSize(m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight),
						hBitmap, DST_BITMAP | DSS_DISABLED);

		::DeleteObject(hBitmap);
	} // if
	else
	{
		::BitBlt(pDC->m_hDC, rImage.left, rImage.top, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcMem, 0, 0, SRCAND);

		::BitBlt(pDC->m_hDC, rImage.left, rImage.top, m_csBitmaps[byIndex].dwWidth, m_csBitmaps[byIndex].dwHeight, hdcBmpMem, 0, 0, SRCPAINT);
	} // else

	::SelectObject(hdcMem, hbmT);
	::DeleteDC(hdcMem);

	::SelectObject(hdcBmpMem, hbmOldBmp);
	::DeleteDC(hdcBmpMem);
} // End of DrawTheBitmap

void CButtonST::DrawTheText(CDC* pDC, LPCTSTR lpszText, RECT* rpItem, CRect* rpCaption, BOOL bIsPressed, BOOL bIsDisabled)
{
	// Draw the button's title
	// If button is pressed then "press" title also
	if (m_bIsPressed && m_bIsCheckBox == FALSE)
		rpCaption->OffsetRect(m_ptPressedOffset.x, m_ptPressedOffset.y);

	// ONLY FOR DEBUG
	//CBrush brBtnShadow(RGB(255, 0, 0));
	//pDC->FrameRect(rCaption, &brBtnShadow);

	// Center text
	CRect centerRect = rpCaption;
	pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER | DT_CALCRECT);
	rpCaption->OffsetRect((centerRect.Width() - rpCaption->Width())/2, (centerRect.Height() - rpCaption->Height())/2);
	/* RFU
	rpCaption->OffsetRect(0, (centerRect.Height() - rpCaption->Height())/2);
	rpCaption->OffsetRect((centerRect.Width() - rpCaption->Width())-4, (centerRect.Height() - rpCaption->Height())/2);
	*/

	pDC->SetBkMode(TRANSPARENT);
	/*
	pDC->DrawState(rCaption->TopLeft(), rCaption->Size(), (LPCTSTR)sTitle, (bIsDisabled ? DSS_DISABLED : DSS_NORMAL),
					TRUE, 0, (CBrush*)nullptr);
	*/
	if (m_bIsDisabled)
	{
		rpCaption->OffsetRect(1, 1);
		pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
		pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER);
		rpCaption->OffsetRect(-1, -1);
		pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
		pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER);
	} // if
	else
	{
		if (m_bMouseOnButton || m_bIsPressed)
		{
			pDC->SetTextColor(m_crColors[BTNST_COLOR_FG_IN]);
			pDC->SetBkColor(m_crColors[BTNST_COLOR_BK_IN]);
		} // if
		else
		{
			if (m_bIsFocused)
			{
				pDC->SetTextColor(m_crColors[BTNST_COLOR_FG_FOCUS]);
				pDC->SetBkColor(m_crColors[BTNST_COLOR_BK_FOCUS]);
			} // if
			else
			{
				pDC->SetTextColor(m_crColors[BTNST_COLOR_FG_OUT]);
				pDC->SetBkColor(m_crColors[BTNST_COLOR_BK_OUT]);
			} // else
		} // else
		pDC->DrawText(lpszText, -1, rpCaption, DT_WORDBREAK | DT_CENTER);
	} // if
} // End of DrawTheText

// This function creates a grayscale bitmap starting from a given bitmap.
// The resulting bitmap will have the same size of the original one.
//
// Parameters:
//		[IN]	hBitmap
//				Handle to the original bitmap.
//		[IN]	dwWidth
//				Specifies the bitmap width, in pixels.
//		[IN]	dwHeight
//				Specifies the bitmap height, in pixels.
//		[IN]	crTrans
//				Color to be used as transparent color. This color will be left unchanged.
//
// Return value:
//		If the function succeeds, the return value is the handle to the newly created
//		grayscale bitmap.
//		If the function fails, the return value is nullptr.
//
HBITMAP CButtonST::CreateGrayscaleBitmap(HBITMAP hBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTrans)
{
	HBITMAP		hGrayBitmap = nullptr;
	HDC			hMainDC = nullptr, hMemDC1 = nullptr, hMemDC2 = nullptr;
	HBITMAP		hOldBmp1 = nullptr, hOldBmp2 = nullptr;

	hMainDC = ::GetDC(nullptr);
	if (hMainDC == nullptr)	return nullptr;
	hMemDC1 = ::CreateCompatibleDC(hMainDC);
	if (hMemDC1 == nullptr)
	{
		::ReleaseDC(nullptr, hMainDC);
		return nullptr;
	} // if
	hMemDC2 = ::CreateCompatibleDC(hMainDC);
	if (hMemDC2 == nullptr)
	{
		::DeleteDC(hMemDC1);
		::ReleaseDC(nullptr, hMainDC);
		return nullptr;
	} // if

	hGrayBitmap = ::CreateCompatibleBitmap(hMainDC, dwWidth, dwHeight);
	if (hGrayBitmap)
	{
		hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, hGrayBitmap);
		hOldBmp2 = (HBITMAP)::SelectObject(hMemDC2, hBitmap);

		//::BitBlt(hMemDC1, 0, 0, dwWidth, dwHeight, hMemDC2, 0, 0, SRCCOPY);

		DWORD		dwLoopY = 0, dwLoopX = 0;
		COLORREF	crPixel = 0;
		BYTE		byNewPixel = 0;

		for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++)
		{
			for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++)
			{
				crPixel = ::GetPixel(hMemDC2, dwLoopX, dwLoopY);
				byNewPixel = (BYTE)((GetRValue(crPixel) * 0.299) + (GetGValue(crPixel) * 0.587) + (GetBValue(crPixel) * 0.114));

				if (crPixel != crTrans)
					::SetPixel(hMemDC1, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel));
				else
					::SetPixel(hMemDC1, dwLoopX, dwLoopY, crPixel);
			} // for
		} // for

		::SelectObject(hMemDC1, hOldBmp1);
		::SelectObject(hMemDC2, hOldBmp2);
	} // if

	::DeleteDC(hMemDC1);
	::DeleteDC(hMemDC2);
	::ReleaseDC(nullptr, hMainDC);

	return hGrayBitmap;
} // End of CreateGrayscaleBitmap

// This function creates a bitmap that is 25% darker than the original.
// The resulting bitmap will have the same size of the original one.
//
// Parameters:
//		[IN]	hBitmap
//				Handle to the original bitmap.
//		[IN]	dwWidth
//				Specifies the bitmap width, in pixels.
//		[IN]	dwHeight
//				Specifies the bitmap height, in pixels.
//		[IN]	crTrans
//				Color to be used as transparent color. This color will be left unchanged.
//
// Return value:
//		If the function succeeds, the return value is the handle to the newly created
//		darker bitmap.
//		If the function fails, the return value is nullptr.
//
HBITMAP CButtonST::CreateDarkerBitmap(HBITMAP hBitmap, DWORD dwWidth, DWORD dwHeight, COLORREF crTrans)
{
	HBITMAP		hGrayBitmap = nullptr;
	HDC			hMainDC = nullptr, hMemDC1 = nullptr, hMemDC2 = nullptr;
	HBITMAP		hOldBmp1 = nullptr, hOldBmp2 = nullptr;

	hMainDC = ::GetDC(nullptr);
	if (hMainDC == nullptr)	return nullptr;
	hMemDC1 = ::CreateCompatibleDC(hMainDC);
	if (hMemDC1 == nullptr)
	{
		::ReleaseDC(nullptr, hMainDC);
		return nullptr;
	} // if
	hMemDC2 = ::CreateCompatibleDC(hMainDC);
	if (hMemDC2 == nullptr)
	{
		::DeleteDC(hMemDC1);
		::ReleaseDC(nullptr, hMainDC);
		return nullptr;
	} // if

	hGrayBitmap = ::CreateCompatibleBitmap(hMainDC, dwWidth, dwHeight);
	if (hGrayBitmap)
	{
		hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, hGrayBitmap);
		hOldBmp2 = (HBITMAP)::SelectObject(hMemDC2, hBitmap);

		//::BitBlt(hMemDC1, 0, 0, dwWidth, dwHeight, hMemDC2, 0, 0, SRCCOPY);

		DWORD		dwLoopY = 0, dwLoopX = 0;
		COLORREF	crPixel = 0;

		for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++)
		{
			for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++)
			{
				crPixel = ::GetPixel(hMemDC2, dwLoopX, dwLoopY);

				if (crPixel != crTrans)
					::SetPixel(hMemDC1, dwLoopX, dwLoopY, DarkenColor(crPixel, 0.25));
				else
					::SetPixel(hMemDC1, dwLoopX, dwLoopY, crPixel);
			} // for
		} // for

		::SelectObject(hMemDC1, hOldBmp1);
		::SelectObject(hMemDC2, hOldBmp2);
	} // if

	::DeleteDC(hMemDC1);
	::DeleteDC(hMemDC2);
	::ReleaseDC(nullptr, hMainDC);

	return hGrayBitmap;
} // End of CreateDarkerBitmap

// This function creates a grayscale icon starting from a given icon.
// The resulting icon will have the same size of the original one.
//
// Parameters:
//		[IN]	hIcon
//				Handle to the original icon.
//
// Return value:
//		If the function succeeds, the return value is the handle to the newly created
//		grayscale icon.
//		If the function fails, the return value is nullptr.
//
// Updates:
//		26/Nov/2002	Restored 1 BitBlt operation
//		03/May/2002	Removed dependancy from m_hWnd
//					Removed 1 BitBlt operation
//
HICON CButtonST::CreateGrayscaleIcon(HICON hIcon)
{
	HICON		hGrayIcon = nullptr;
	HDC			hMainDC = nullptr, hMemDC1 = nullptr, hMemDC2 = nullptr;
	BITMAP		bmp;
	HBITMAP		hOldBmp1 = nullptr, hOldBmp2 = nullptr;
	ICONINFO	csII, csGrayII;
	BOOL		bRetValue = FALSE;

	bRetValue = ::GetIconInfo(hIcon, &csII);
	if (bRetValue == FALSE)	return nullptr;

	hMainDC = ::GetDC(nullptr);
	hMemDC1 = ::CreateCompatibleDC(hMainDC);
	hMemDC2 = ::CreateCompatibleDC(hMainDC);
	if (hMainDC == nullptr || hMemDC1 == nullptr || hMemDC2 == nullptr)	return nullptr;

	if (::GetObject(csII.hbmColor, sizeof(BITMAP), &bmp))
	{
		DWORD	dwWidth = csII.xHotspot*2;
		DWORD	dwHeight = csII.yHotspot*2;

		csGrayII.hbmColor = ::CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, nullptr);
		if (csGrayII.hbmColor)
		{
			hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, csII.hbmColor);
			hOldBmp2 = (HBITMAP)::SelectObject(hMemDC2, csGrayII.hbmColor);

			//::BitBlt(hMemDC2, 0, 0, dwWidth, dwHeight, hMemDC1, 0, 0, SRCCOPY);

			DWORD		dwLoopY = 0, dwLoopX = 0;
			COLORREF	crPixel = 0;
			BYTE		byNewPixel = 0;

			for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++)
			{
				for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++)
				{
					crPixel = ::GetPixel(hMemDC1, dwLoopX, dwLoopY);
					byNewPixel = (BYTE)((GetRValue(crPixel) * 0.299) + (GetGValue(crPixel) * 0.587) + (GetBValue(crPixel) * 0.114));

					if (crPixel)
						::SetPixel(hMemDC2, dwLoopX, dwLoopY, RGB(byNewPixel, byNewPixel, byNewPixel));
					else
						::SetPixel(hMemDC2, dwLoopX, dwLoopY, crPixel);
				} // for
			} // for

			::SelectObject(hMemDC1, hOldBmp1);
			::SelectObject(hMemDC2, hOldBmp2);

			csGrayII.hbmMask = csII.hbmMask;

			csGrayII.fIcon = TRUE;
			hGrayIcon = ::CreateIconIndirect(&csGrayII);
		} // if

		::DeleteObject(csGrayII.hbmColor);
		//::DeleteObject(csGrayII.hbmMask);
	} // if

	::DeleteObject(csII.hbmColor);
	::DeleteObject(csII.hbmMask);
	::DeleteDC(hMemDC1);
	::DeleteDC(hMemDC2);
	::ReleaseDC(nullptr, hMainDC);

	return hGrayIcon;
} // End of CreateGrayscaleIcon

// This function creates a icon that is 25% darker than the original.
// The resulting icon will have the same size of the original one.
//
// Parameters:
//		[IN]	hIcon
//				Handle to the original icon.
//
// Return value:
//		If the function succeeds, the return value is the handle to the newly created
//		darker icon.
//		If the function fails, the return value is nullptr.
//
HICON CButtonST::CreateDarkerIcon(HICON hIcon)
{
	HICON		hGrayIcon = nullptr;
	HDC			hMainDC = nullptr, hMemDC1 = nullptr, hMemDC2 = nullptr;
	BITMAP		bmp;
	HBITMAP		hOldBmp1 = nullptr, hOldBmp2 = nullptr;
	ICONINFO	csII, csGrayII;
	BOOL		bRetValue = FALSE;

	bRetValue = ::GetIconInfo(hIcon, &csII);
	if (bRetValue == FALSE)	return nullptr;

	hMainDC = ::GetDC(nullptr);
	hMemDC1 = ::CreateCompatibleDC(hMainDC);
	hMemDC2 = ::CreateCompatibleDC(hMainDC);
	if (hMainDC == nullptr || hMemDC1 == nullptr || hMemDC2 == nullptr)	return nullptr;

	if (::GetObject(csII.hbmColor, sizeof(BITMAP), &bmp))
	{
		DWORD	dwWidth = csII.xHotspot*2;
		DWORD	dwHeight = csII.yHotspot*2;

		csGrayII.hbmColor = ::CreateBitmap(dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, nullptr);
		if (csGrayII.hbmColor)
		{
			hOldBmp1 = (HBITMAP)::SelectObject(hMemDC1, csII.hbmColor);
			hOldBmp2 = (HBITMAP)::SelectObject(hMemDC2, csGrayII.hbmColor);

			//::BitBlt(hMemDC2, 0, 0, dwWidth, dwHeight, hMemDC1, 0, 0, SRCCOPY);

			DWORD		dwLoopY = 0, dwLoopX = 0;
			COLORREF	crPixel = 0;

			for (dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++)
			{
				for (dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++)
				{
					crPixel = ::GetPixel(hMemDC1, dwLoopX, dwLoopY);

					if (crPixel)
						::SetPixel(hMemDC2, dwLoopX, dwLoopY, DarkenColor(crPixel, 0.25));
					else
						::SetPixel(hMemDC2, dwLoopX, dwLoopY, crPixel);
				} // for
			} // for

			::SelectObject(hMemDC1, hOldBmp1);
			::SelectObject(hMemDC2, hOldBmp2);

			csGrayII.hbmMask = csII.hbmMask;

			csGrayII.fIcon = TRUE;
			hGrayIcon = ::CreateIconIndirect(&csGrayII);
		} // if

		::DeleteObject(csGrayII.hbmColor);
		//::DeleteObject(csGrayII.hbmMask);
	} // if

	::DeleteObject(csII.hbmColor);
	::DeleteObject(csII.hbmMask);
	::DeleteDC(hMemDC1);
	::DeleteDC(hMemDC2);
	::ReleaseDC(nullptr, hMainDC);

	return hGrayIcon;
} // End of CreateDarkerIcon

COLORREF CButtonST::DarkenColor(COLORREF crColor, double dFactor)
{
	if (dFactor > 0.0 && dFactor <= 1.0)
	{
		BYTE red,green,blue,lightred,lightgreen,lightblue;
		red = GetRValue(crColor);
		green = GetGValue(crColor);
		blue = GetBValue(crColor);
		lightred = (BYTE)(red-(dFactor * red));
		lightgreen = (BYTE)(green-(dFactor * green));
		lightblue = (BYTE)(blue-(dFactor * blue));
		crColor = RGB(lightred,lightgreen,lightblue);
	} // if

	return crColor;
} // End of DarkenColor

// This function assigns icons to the button.
// Any previous icon or bitmap will be removed.
//
// Parameters:
//		[IN]	nIconIn
//				ID number of the icon resource to show when the mouse is over the button.
//				Pass nullptr to remove any icon from the button.
//		[IN]	nCxDesiredIn
//				Specifies the width, in pixels, of the icon to load.
//		[IN]	nCyDesiredIn
//				Specifies the height, in pixels, of the icon to load.
//		[IN]	nIconOut
//				ID number of the icon resource to show when the mouse is outside the button.
//				Can be nullptr.
//				If this parameter is the special value BTNST_AUTO_GRAY (cast to int) the second
//				icon will be automatically created starting from nIconIn and converted to grayscale.
//				If this parameter is the special value BTNST_AUTO_DARKER (cast to int) the second
//				icon will be automatically created 25% darker starting from nIconIn.
//		[IN]	nCxDesiredOut
//				Specifies the width, in pixels, of the icon to load.
//		[IN]	nCyDesiredOut
//				Specifies the height, in pixels, of the icon to load.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//
DWORD CButtonST::SetIcon(int nIconIn, int nCxDesiredIn, int nCyDesiredIn, int nIconOut, int nCxDesiredOut, int nCyDesiredOut)
{
	HICON		hIconIn			= nullptr;
	HICON		hIconOut		= nullptr;
	HINSTANCE	hInstResource	= nullptr;

	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nIconIn), RT_GROUP_ICON);

	// Set icon when the mouse is IN the button
	hIconIn = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(nIconIn), IMAGE_ICON, nCxDesiredIn, nCyDesiredIn, 0);

  	// Set icon when the mouse is OUT the button
	switch (nIconOut)
	{
		case 0:
			break;
		case (int)(size_t)BTNST_AUTO_GRAY:
			hIconOut = BTNST_AUTO_GRAY;
			break;
		case (int)(size_t)BTNST_AUTO_DARKER:
			hIconOut = BTNST_AUTO_DARKER;
			break;
		default:
			hIconOut = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(nIconOut), IMAGE_ICON, nCxDesiredOut, nCyDesiredOut, 0);
			break;
	} // switch

	return SetIcon(hIconIn, hIconOut);
} // End of SetIcon

// This function assigns icons to the button.
// Any previous icon or bitmap will be removed.
//
// Parameters:
//		[IN]	nIconIn
//				ID number of the icon resource to show when the mouse is over the button.
//				Pass nullptr to remove any icon from the button.
//		[IN]	nIconOut
//				ID number of the icon resource to show when the mouse is outside the button.
//				Can be nullptr.
//				If this parameter is the special value BTNST_AUTO_GRAY (cast to int) the second
//				icon will be automatically created starting from nIconIn and converted to grayscale.
//				If this parameter is the special value BTNST_AUTO_DARKER (cast to int) the second
//				icon will be automatically created 25% darker starting from nIconIn.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//
DWORD CButtonST::SetIcon(int nIconIn, int nIconOut)
{
	return SetIcon(nIconIn, 0, 0, nIconOut, 0, 0);
} // End of SetIcon

// This function assigns icons to the button.
// Any previous icon or bitmap will be removed.
//
// Parameters:
//		[IN]	hIconIn
//				Handle fo the icon to show when the mouse is over the button.
//				Pass nullptr to remove any icon from the button.
//		[IN]	hIconOut
//				Handle to the icon to show when the mouse is outside the button.
//				Can be nullptr.
//				If this parameter is the special value BTNST_AUTO_GRAY the second
//				icon will be automatically created starting from hIconIn and converted to grayscale.
//				If this parameter is the special value BTNST_AUTO_DARKER the second
//				icon will be automatically created 25% darker starting from hIconIn.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//
DWORD CButtonST::SetIcon(HICON hIconIn, HICON hIconOut)
{
	BOOL		bRetValue;
	ICONINFO	ii;

	// Free any loaded resource
	FreeResources();

	if (hIconIn)
	{
		// Icon when mouse over button?
		m_csIcons[0].hIcon = hIconIn;
		// Get icon dimension
		::ZeroMemory(&ii, sizeof(ICONINFO));
		bRetValue = ::GetIconInfo(hIconIn, &ii);
		if (bRetValue == FALSE)
		{
			FreeResources();
			return BTNST_INVALIDRESOURCE;
		} // if

		m_csIcons[0].dwWidth	= (DWORD)(ii.xHotspot * 2);
		m_csIcons[0].dwHeight	= (DWORD)(ii.yHotspot * 2);
		::DeleteObject(ii.hbmMask);
		::DeleteObject(ii.hbmColor);

		// Icon when mouse outside button?
		if (hIconOut)
		{
			switch ((int)(size_t)hIconOut)
			{
				case (int)(size_t)BTNST_AUTO_GRAY:
					hIconOut = CreateGrayscaleIcon(hIconIn);
					break;
				case (int)(size_t)BTNST_AUTO_DARKER:
					hIconOut = CreateDarkerIcon(hIconIn);
					break;
			} // switch

			m_csIcons[1].hIcon = hIconOut;
			// Get icon dimension
			::ZeroMemory(&ii, sizeof(ICONINFO));
			bRetValue = ::GetIconInfo(hIconOut, &ii);
			if (bRetValue == FALSE)
			{
				FreeResources();
				return BTNST_INVALIDRESOURCE;
			} // if

			m_csIcons[1].dwWidth	= (DWORD)(ii.xHotspot * 2);
			m_csIcons[1].dwHeight	= (DWORD)(ii.yHotspot * 2);
			::DeleteObject(ii.hbmMask);
			::DeleteObject(ii.hbmColor);
		} // if
	} // if

	Invalidate();

	return BTNST_OK;
} // End of SetIcon

// This function assigns bitmaps to the button.
// Any previous icon or bitmap will be removed.
//
// Parameters:
//		[IN]	nBitmapIn
//				ID number of the bitmap resource to show when the mouse is over the button.
//				Pass nullptr to remove any bitmap from the button.
//		[IN]	crTransColorIn
//				Color (inside nBitmapIn) to be used as transparent color.
//		[IN]	nBitmapOut
//				ID number of the bitmap resource to show when the mouse is outside the button.
//				Can be nullptr.
//		[IN]	crTransColorOut
//				Color (inside nBitmapOut) to be used as transparent color.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//		BTNST_FAILEDMASK
//			Failed creating mask bitmap.
//
DWORD CButtonST::SetBitmaps(int nBitmapIn, COLORREF crTransColorIn, int nBitmapOut, COLORREF crTransColorOut)
{
	HBITMAP		hBitmapIn		= nullptr;
	HBITMAP		hBitmapOut		= nullptr;
	HINSTANCE	hInstResource	= nullptr;

	// Find correct resource handle
	hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nBitmapIn), RT_BITMAP);

	// Load bitmap In
	hBitmapIn = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapIn), IMAGE_BITMAP, 0, 0, 0);

	// Load bitmap Out
	switch (nBitmapOut)
	{
		case 0:
			break;
		case (int)(size_t)BTNST_AUTO_GRAY:
			hBitmapOut = (HBITMAP)BTNST_AUTO_GRAY;
			break;
		case (int)(size_t)BTNST_AUTO_DARKER:
			hBitmapOut = (HBITMAP)BTNST_AUTO_DARKER;
			break;
		default:
			hBitmapOut = (HBITMAP)::LoadImage(hInstResource, MAKEINTRESOURCE(nBitmapOut), IMAGE_BITMAP, 0, 0, 0);
			break;
	} // if

	return SetBitmaps(hBitmapIn, crTransColorIn, hBitmapOut, crTransColorOut);
} // End of SetBitmaps

// This function assigns bitmaps to the button.
// Any previous icon or bitmap will be removed.
//
// Parameters:
//		[IN]	hBitmapIn
//				Handle fo the bitmap to show when the mouse is over the button.
//				Pass nullptr to remove any bitmap from the button.
//		[IN]	crTransColorIn
//				Color (inside hBitmapIn) to be used as transparent color.
//		[IN]	hBitmapOut
//				Handle to the bitmap to show when the mouse is outside the button.
//				Can be nullptr.
//		[IN]	crTransColorOut
//				Color (inside hBitmapOut) to be used as transparent color.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//		BTNST_FAILEDMASK
//			Failed creating mask bitmap.
//
DWORD CButtonST::SetBitmaps(HBITMAP hBitmapIn, COLORREF crTransColorIn, HBITMAP hBitmapOut, COLORREF crTransColorOut)
{
	int		nRetValue = 0;
	BITMAP	csBitmapSize;

	// Free any loaded resource
	FreeResources();

	if (hBitmapIn)
	{
		m_csBitmaps[0].hBitmap = hBitmapIn;
		m_csBitmaps[0].crTransparent = crTransColorIn;
		// Get bitmap size
		nRetValue = ::GetObject(hBitmapIn, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
		{
			FreeResources();
			return BTNST_INVALIDRESOURCE;
		} // if
		m_csBitmaps[0].dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_csBitmaps[0].dwHeight = (DWORD)csBitmapSize.bmHeight;

		// Create grayscale/darker bitmap BEFORE mask (of hBitmapIn)
		switch ((int)(size_t)hBitmapOut)
		{
			case (int)(size_t)BTNST_AUTO_GRAY:
				hBitmapOut = CreateGrayscaleBitmap(hBitmapIn, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, crTransColorIn);
				m_csBitmaps[1].hBitmap = hBitmapOut;
				crTransColorOut = crTransColorIn;
				break;
			case (int)(size_t)BTNST_AUTO_DARKER:
				hBitmapOut = CreateDarkerBitmap(hBitmapIn, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, crTransColorIn);
				m_csBitmaps[1].hBitmap = hBitmapOut;
				crTransColorOut = crTransColorIn;
				break;
		} // switch

		// Create mask for bitmap In
		m_csBitmaps[0].hMask = CreateBitmapMask(hBitmapIn, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight, crTransColorIn);
		if (m_csBitmaps[0].hMask == nullptr)
		{
			FreeResources();
			return BTNST_FAILEDMASK;
		} // if

		if (hBitmapOut)
		{
			m_csBitmaps[1].hBitmap = hBitmapOut;
			m_csBitmaps[1].crTransparent = crTransColorOut;
			// Get bitmap size
			nRetValue = ::GetObject(hBitmapOut, sizeof(csBitmapSize), &csBitmapSize);
			if (nRetValue == 0)
			{
				FreeResources();
				return BTNST_INVALIDRESOURCE;
			} // if
			m_csBitmaps[1].dwWidth = (DWORD)csBitmapSize.bmWidth;
			m_csBitmaps[1].dwHeight = (DWORD)csBitmapSize.bmHeight;

			// Create mask for bitmap Out
			m_csBitmaps[1].hMask = CreateBitmapMask(hBitmapOut, m_csBitmaps[1].dwWidth, m_csBitmaps[1].dwHeight, crTransColorOut);
			if (m_csBitmaps[1].hMask == nullptr)
			{
				FreeResources();
				return BTNST_FAILEDMASK;
			} // if
		} // if
	} // if

	Invalidate();

	return BTNST_OK;
} // End of SetBitmaps

// This functions sets the button to have a standard or flat style.
//
// Parameters:
//		[IN]	bFlat
//				If TRUE the button will have a flat style, else
//				will have a standard style.
//				By default, CButtonST buttons are flat.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::SetFlat(BOOL bFlat, BOOL bRepaint)
{
	m_bIsFlat = bFlat;
	if (bRepaint)	Invalidate();

	return BTNST_OK;
} // End of SetFlat

// This function sets the alignment type between icon/bitmap and text.
//
// Parameters:
//		[IN]	byAlign
//				Alignment type. Can be one of the following values:
//				ST_ALIGN_HORIZ			Icon/bitmap on the left, text on the right
//				ST_ALIGN_VERT			Icon/bitmap on the top, text on the bottom
//				ST_ALIGN_HORIZ_RIGHT	Icon/bitmap on the right, text on the left
//				ST_ALIGN_OVERLAP		Icon/bitmap on the same space as text
//				By default, CButtonST buttons have ST_ALIGN_HORIZ alignment.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDALIGN
//			Alignment type not supported.
//
DWORD CButtonST::SetAlign(BYTE byAlign, BOOL bRepaint)
{
	switch (byAlign)
	{
		case ST_ALIGN_HORIZ:
		case ST_ALIGN_HORIZ_RIGHT:
		case ST_ALIGN_VERT:
		case ST_ALIGN_OVERLAP:
			m_byAlign = byAlign;
			if (bRepaint)	Invalidate();
			return BTNST_OK;
			break;
	} // switch

	return BTNST_INVALIDALIGN;
} // End of SetAlign

// This function sets the pressed style.
//
// Parameters:
//		[IN]	byStyle
//				Pressed style. Can be one of the following values:
//				BTNST_PRESSED_LEFTRIGHT		Pressed style from left to right (as usual)
//				BTNST_PRESSED_TOPBOTTOM		Pressed style from top to bottom
//				By default, CButtonST buttons have BTNST_PRESSED_LEFTRIGHT style.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDPRESSEDSTYLE
//			Pressed style not supported.
//
DWORD CButtonST::SetPressedStyle(BYTE byStyle, BOOL bRepaint)
{
	switch (byStyle)
	{
		case BTNST_PRESSED_LEFTRIGHT:
			m_ptPressedOffset.x = 1;
			m_ptPressedOffset.y = 1;
			break;
		case BTNST_PRESSED_TOPBOTTOM:
			m_ptPressedOffset.x = 0;
			m_ptPressedOffset.y = 2;
			break;
		default:
			return BTNST_INVALIDPRESSEDSTYLE;
	} // switch

	if (bRepaint)	Invalidate();

	return BTNST_OK;
} // End of SetPressedStyle

// This function sets the state of the checkbox.
// If the button is not a checkbox, this function has no meaning.
//
// Parameters:
//		[IN]	nCheck
//				1 to check the checkbox.
//				0 to un-check the checkbox.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::SetCheck(int nCheck, BOOL bRepaint)
{
	if (m_bIsCheckBox)
	{
		if (nCheck == 0) m_nCheck = 0;
		else m_nCheck = 1;

		if (bRepaint) Invalidate();
	} // if

	return BTNST_OK;
} // End of SetCheck

// This function returns the current state of the checkbox.
// If the button is not a checkbox, this function has no meaning.
//
// Return value:
//		The current state of the checkbox.
//			1 if checked.
//			0 if not checked or the button is not a checkbox.
//
int CButtonST::GetCheck()
{
	return m_nCheck;
} // End of GetCheck

// This function sets all colors to a default value.
//
// Parameters:
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::SetDefaultColors(BOOL bRepaint)
{
	m_crColors[BTNST_COLOR_BK_IN]		= ::GetSysColor(COLOR_BTNFACE);
	m_crColors[BTNST_COLOR_FG_IN]		= ::GetSysColor(COLOR_BTNTEXT);
	m_crColors[BTNST_COLOR_BK_OUT]		= ::GetSysColor(COLOR_BTNFACE);
	m_crColors[BTNST_COLOR_FG_OUT]		= ::GetSysColor(COLOR_BTNTEXT);
	m_crColors[BTNST_COLOR_BK_FOCUS]	= ::GetSysColor(COLOR_BTNFACE);
	m_crColors[BTNST_COLOR_FG_FOCUS]	= ::GetSysColor(COLOR_BTNTEXT);

	if (bRepaint)	Invalidate();

	return BTNST_OK;
} // End of SetDefaultColors

// This function sets the color to use for a particular state.
//
// Parameters:
//		[IN]	byColorIndex
//				Index of the color to set. Can be one of the following values:
//				BTNST_COLOR_BK_IN		Background color when mouse is over the button
//				BTNST_COLOR_FG_IN		Text color when mouse is over the button
//				BTNST_COLOR_BK_OUT		Background color when mouse is outside the button
//				BTNST_COLOR_FG_OUT		Text color when mouse is outside the button
//				BTNST_COLOR_BK_FOCUS	Background color when the button is focused
//				BTNST_COLOR_FG_FOCUS	Text color when the button is focused
//		[IN]	crColor
//				New color.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDINDEX
//			Invalid color index.
//
DWORD CButtonST::SetColor(BYTE byColorIndex, COLORREF crColor, BOOL bRepaint)
{
	if (byColorIndex >= BTNST_MAX_COLORS)	return BTNST_INVALIDINDEX;

	// Set new color
	m_crColors[byColorIndex] = crColor;

	if (bRepaint)	Invalidate();

	return BTNST_OK;
} // End of SetColor

// This functions returns the color used for a particular state.
//
// Parameters:
//		[IN]	byColorIndex
//				Index of the color to get.
//				See SetColor for the list of available colors.
//		[OUT]	crpColor
//				A pointer to a COLORREF that will receive the color.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDINDEX
//			Invalid color index.
//
DWORD CButtonST::GetColor(BYTE byColorIndex, COLORREF* crpColor)
{
	if (byColorIndex >= BTNST_MAX_COLORS)	return BTNST_INVALIDINDEX;

	// Get color
	*crpColor = m_crColors[byColorIndex];

	return BTNST_OK;
} // End of GetColor

// This function applies an offset to the RGB components of the specified color.
// This function can be seen as an easy way to make a color darker or lighter than
// its default value.
//
// Parameters:
//		[IN]	byColorIndex
//				Index of the color to set.
//				See SetColor for the list of available colors.
//		[IN]	shOffsetColor
//				A short value indicating the offset to apply to the color.
//				This value must be between -255 and 255.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDINDEX
//			Invalid color index.
//		BTNST_BADPARAM
//			The specified offset is out of range.
//
DWORD CButtonST::OffsetColor(BYTE byColorIndex, short shOffset, BOOL bRepaint)
{
	BYTE	byRed = 0;
	BYTE	byGreen = 0;
	BYTE	byBlue = 0;
	short	shOffsetR = shOffset;
	short	shOffsetG = shOffset;
	short	shOffsetB = shOffset;

	if (byColorIndex >= BTNST_MAX_COLORS)	return BTNST_INVALIDINDEX;
	if (shOffset < -255 || shOffset > 255)	return BTNST_BADPARAM;

	// Get RGB components of specified color
	byRed = GetRValue(m_crColors[byColorIndex]);
	byGreen = GetGValue(m_crColors[byColorIndex]);
	byBlue = GetBValue(m_crColors[byColorIndex]);

	// Calculate max. allowed real offset
	if (shOffset > 0)
	{
		if (byRed + shOffset > 255)		shOffsetR = 255 - byRed;
		if (byGreen + shOffset > 255)	shOffsetG = 255 - byGreen;
		if (byBlue + shOffset > 255)	shOffsetB = 255 - byBlue;

		shOffset = min(min(shOffsetR, shOffsetG), shOffsetB);
	} // if
	else
	{
		if (byRed + shOffset < 0)		shOffsetR = -byRed;
		if (byGreen + shOffset < 0)		shOffsetG = -byGreen;
		if (byBlue + shOffset < 0)		shOffsetB = -byBlue;

		shOffset = max(max(shOffsetR, shOffsetG), shOffsetB);
	} // else

	// Set new color
	m_crColors[byColorIndex] = RGB(byRed + shOffset, byGreen + shOffset, byBlue + shOffset);

	if (bRepaint)	Invalidate();

	return BTNST_OK;
} // End of OffsetColor

// This function sets the hilight logic for the button.
// Applies only to flat buttons.
//
// Parameters:
//		[IN]	bAlwaysTrack
//				If TRUE the button will be hilighted even if the window that owns it, is
//				not the active window.
//				If FALSE the button will be hilighted only if the window that owns it,
//				is the active window.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::SetAlwaysTrack(BOOL bAlwaysTrack)
{
	m_bAlwaysTrack = bAlwaysTrack;
	return BTNST_OK;
} // End of SetAlwaysTrack

// This function sets the cursor to be used when the mouse is over the button.
//
// Parameters:
//		[IN]	nCursorId
//				ID number of the cursor resource.
//				Pass nullptr to remove a previously loaded cursor.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//
DWORD CButtonST::SetBtnCursor(int nCursorId, BOOL bRepaint)
{
	HINSTANCE	hInstResource = nullptr;
	// Destroy any previous cursor
	if (m_hCursor)
	{
		::DestroyCursor(m_hCursor);
		m_hCursor = nullptr;
	} // if

	// Load cursor
	if (nCursorId)
	{
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nCursorId), RT_GROUP_CURSOR);
		// Load cursor resource
		m_hCursor = (HCURSOR)::LoadImage(hInstResource, MAKEINTRESOURCE(nCursorId), IMAGE_CURSOR, 0, 0, 0);
		// Repaint the button
		if (bRepaint) Invalidate();
		// If something wrong
		if (m_hCursor == nullptr) return BTNST_INVALIDRESOURCE;
	} // if

	return BTNST_OK;
} // End of SetBtnCursor

// This function sets if the button border must be drawn.
// Applies only to flat buttons.
//
// Parameters:
//		[IN]	bDrawBorder
//				If TRUE the border will be drawn.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::DrawBorder(BOOL bDrawBorder, BOOL bRepaint)
{
	m_bDrawBorder = bDrawBorder;
	// Repaint the button
	if (bRepaint) Invalidate();

	return BTNST_OK;
} // End of DrawBorder

// This function sets if the focus rectangle must be drawn for flat buttons.
//
// Parameters:
//		[IN]	bDrawFlatFocus
//				If TRUE the focus rectangle will be drawn also for flat buttons.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::DrawFlatFocus(BOOL bDrawFlatFocus, BOOL bRepaint)
{
	m_bDrawFlatFocus = bDrawFlatFocus;
	// Repaint the button
	if (bRepaint) Invalidate();

	return BTNST_OK;
} // End of DrawFlatFocus

void CButtonST::InitToolTip()
{
	if (m_ToolTip.m_hWnd == nullptr)
	{
		// Create ToolTip control
		m_ToolTip.Create(this, m_dwToolTipStyle);
		// Create inactive
		m_ToolTip.Activate(FALSE);
		// Enable multiline
		m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 400);
		//m_ToolTip.SendMessage(TTM_SETTITLE, TTI_INFO, (LPARAM)_T("Title"));
	} // if
} // End of InitToolTip

// This function sets the text to show in the button tooltip.
//
// Parameters:
//		[IN]	nText
//				ID number of the string resource containing the text to show.
//		[IN]	bActivate
//				If TRUE the tooltip will be created active.
//
void CButtonST::SetTooltipText(int nText, BOOL bActivate)
{
	CString sText;

	// Load string resource
	sText.LoadString(nText);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE) SetTooltipText((LPCTSTR)sText, bActivate);
} // End of SetTooltipText

// This function sets the text to show in the button tooltip.
//
// Parameters:
//		[IN]	lpszText
//				Pointer to a null-terminated string containing the text to show.
//		[IN]	bActivate
//				If TRUE the tooltip will be created active.
//
void CButtonST::SetTooltipText(LPCTSTR lpszText, BOOL bActivate)
{
	// We cannot accept nullptr pointer
	if (lpszText == nullptr) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn;
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, lpszText, rectBtn, 1);
	} // if

	// Set text for tooltip
	m_ToolTip.UpdateTipText(lpszText, this, 1);
	m_ToolTip.Activate(bActivate);
} // End of SetTooltipText

// This function enables or disables the button tooltip.
//
// Parameters:
//		[IN]	bActivate
//				If TRUE the tooltip will be activated.
//
void CButtonST::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip

// This function enables the tooltip to be displayed using the balloon style.
// This function must be called before any call to SetTooltipText is made.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::EnableBalloonTooltip()
{
	m_dwToolTipStyle |= TTS_BALLOON;
	return BTNST_OK;
} // End of EnableBalloonTooltip

// This function returns if the button is the default button.
//
// Return value:
//		TRUE
//			The button is the default button.
//		FALSE
//			The button is not the default button.
//
BOOL CButtonST::GetDefault()
{
	return m_bIsDefault;
} // End of GetDefault

// This function enables the transparent mode.
// Note: this operation is not reversible.
// DrawTransparent should be called just after the button is created.
// Do not use trasparent buttons until you really need it (you have a bitmapped
// background) since each transparent button makes a copy in memory of its background.
// This may bring unnecessary memory use and execution overload.
//
// Parameters:
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
void CButtonST::DrawTransparent(BOOL bRepaint)
{
	m_bDrawTransparent = TRUE;

	// Restore old bitmap (if any)
	if (m_dcBk.m_hDC != nullptr && m_pbmpOldBk != nullptr)
	{
		m_dcBk.SelectObject(m_pbmpOldBk);
	} // if

	m_bmpBk.DeleteObject();
	m_dcBk.DeleteDC();

	// Repaint the button
	if (bRepaint) Invalidate();
} // End of DrawTransparent

DWORD CButtonST::SetBk(CDC* pDC)
{
	if (m_bDrawTransparent && pDC)
	{
		// Restore old bitmap (if any)
		if (m_dcBk.m_hDC != nullptr && m_pbmpOldBk != nullptr)
		{
			m_dcBk.SelectObject(m_pbmpOldBk);
		} // if

		m_bmpBk.DeleteObject();
		m_dcBk.DeleteDC();

		CRect rect;
		CRect rect1;

		GetClientRect(rect);

		GetWindowRect(rect1);
		GetParent()->ScreenToClient(rect1);

		m_dcBk.CreateCompatibleDC(pDC);
		m_bmpBk.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		m_pbmpOldBk = m_dcBk.SelectObject(&m_bmpBk);
		m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rect1.left, rect1.top, SRCCOPY);

		return BTNST_OK;
	} // if

	return BTNST_BADPARAM;
} // End of SetBk

// This function sets the URL that will be opened when the button is clicked.
//
// Parameters:
//		[IN]	lpszURL
//				Pointer to a null-terminated string that contains the URL.
//				Pass nullptr to removed any previously specified URL.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::SetURL(LPCTSTR lpszURL)
{
	// Remove any existing URL
	memset(m_szURL, 0, sizeof(m_szURL));

	if (lpszURL)
	{
		// Store the URL
		_tcsncpy(m_szURL, lpszURL, _MAX_PATH);
	} // if

	return BTNST_OK;
} // End of SetURL

// This function associates a menu to the button.
// The menu will be displayed clicking the button.
//
// Parameters:
//		[IN]	nMenu
//				ID number of the menu resource.
//				Pass nullptr to remove any menu from the button.
//		[IN]	hParentWnd
//				Handle to the window that owns the menu.
//				This window receives all messages from the menu.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//
#ifndef	BTNST_USE_BCMENU
DWORD CButtonST::SetMenu(UINT nMenu, HWND hParentWnd, BOOL bRepaint)
{
	HINSTANCE	hInstResource	= nullptr;

	// Destroy any previous menu
	if (m_hMenu)
	{
		::DestroyMenu(m_hMenu);
		m_hMenu = nullptr;
		m_hParentWndMenu = nullptr;
		m_bMenuDisplayed = FALSE;
	} // if

	// Load menu
	if (nMenu)
	{
		// Find correct resource handle
		hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(nMenu), RT_MENU);
		// Load menu resource
		m_hMenu = ::LoadMenu(hInstResource, MAKEINTRESOURCE(nMenu));
		m_hParentWndMenu = hParentWnd;
		// If something wrong
		if (m_hMenu == nullptr) return BTNST_INVALIDRESOURCE;
	} // if

	// Repaint the button
	if (bRepaint) Invalidate();

	return BTNST_OK;
} // End of SetMenu
#endif

// This function associates a menu to the button.
// The menu will be displayed clicking the button.
// The menu will be handled by the BCMenu class.
//
// Parameters:
//		[IN]	nMenu
//				ID number of the menu resource.
//				Pass nullptr to remove any menu from the button.
//		[IN]	hParentWnd
//				Handle to the window that owns the menu.
//				This window receives all messages from the menu.
//		[IN]	bWinXPStyle
//				If TRUE the menu will be displayed using the new Windows XP style.
//				If FALSE the menu will be displayed using the standard style.
//		[IN]	nToolbarID
//				Resource ID of the toolbar to be associated to the menu.
//		[IN]	sizeToolbarIcon
//				A CSize object indicating the size (in pixels) of each icon into the toolbar.
//				All icons into the toolbar must have the same size.
//		[IN]	crToolbarBk
//				A COLORREF value indicating the color to use as background for the icons into the toolbar.
//				This color will be used as the "transparent" color.
//		[IN]	bRepaint
//				If TRUE the control will be repainted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//		BTNST_INVALIDRESOURCE
//			Failed loading the specified resource.
//
#ifdef	BTNST_USE_BCMENU
DWORD CButtonST::SetMenu(UINT nMenu, HWND hParentWnd, BOOL bWinXPStyle, UINT nToolbarID, CSize sizeToolbarIcon, COLORREF crToolbarBk, BOOL bRepaint)
{
	BOOL	bRetValue = FALSE;

	// Destroy any previous menu
	if (m_menuPopup.m_hMenu)
	{
		m_menuPopup.DestroyMenu();
		m_hParentWndMenu = nullptr;
		m_bMenuDisplayed = FALSE;
	} // if

	// Load menu
	if (nMenu)
	{
		m_menuPopup.SetMenuDrawMode(bWinXPStyle);
		// Load menu
		bRetValue = m_menuPopup.LoadMenu(nMenu);
		// If something wrong
		if (bRetValue == FALSE) return BTNST_INVALIDRESOURCE;

		// Load toolbar
		if (nToolbarID)
		{
			m_menuPopup.SetBitmapBackground(crToolbarBk);
			m_menuPopup.SetIconSize(sizeToolbarIcon.cx, sizeToolbarIcon.cy);

			bRetValue = m_menuPopup.LoadToolbar(nToolbarID);
			// If something wrong
			if (bRetValue == FALSE)
			{
				m_menuPopup.DestroyMenu();
				return BTNST_INVALIDRESOURCE;
			} // if
		} // if

		m_hParentWndMenu = hParentWnd;
	} // if

	// Repaint the button
	if (bRepaint) Invalidate();

	return BTNST_OK;
} // End of SetMenu
#endif

// This function sets the callback message that will be sent to the
// specified window just before the menu associated to the button is displayed.
//
// Parameters:
//		[IN]	hWnd
//				Handle of the window that will receive the callback message.
//				Pass nullptr to remove any previously specified callback message.
//		[IN]	nMessage
//				Callback message to send to window.
//		[IN]	lParam
//				A 32 bits user specified value that will be passed to the callback function.
//
// Remarks:
//		the callback function must be in the form:
//		LRESULT On_MenuCallback(WPARAM wParam, LPARAM lParam)
//		Where:
//				[IN]	wParam
//						If support for BCMenu is enabled: a pointer to BCMenu
//						else a HMENU handle to the menu that is being to be displayed.
//				[IN]	lParam
//						The 32 bits user specified value.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::SetMenuCallback(HWND hWnd, UINT nMessage, LPARAM lParam)
{
	m_csCallbacks.hWnd = hWnd;
	m_csCallbacks.nMessage = nMessage;
	m_csCallbacks.lParam = lParam;

	return BTNST_OK;
} // End of SetMenuCallback

// This function resizes the button to the same size of the image.
// To get good results both the IN and OUT images should have the same size.
//
void CButtonST::SizeToContent()
{
	if (m_csIcons[0].hIcon)
	{
		m_ptImageOrg.x = 0;
		m_ptImageOrg.y = 0;
	    SetWindowPos(	nullptr, -1, -1, m_csIcons[0].dwWidth, m_csIcons[0].dwHeight,
						SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	} // if
	else
	if (m_csBitmaps[0].hBitmap)
	{
		m_ptImageOrg.x = 0;
		m_ptImageOrg.y = 0;
	    SetWindowPos(	nullptr, -1, -1, m_csBitmaps[0].dwWidth, m_csBitmaps[0].dwHeight,
						SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	} // if
} // End of SizeToContent

// This function sets the sound that must be played on particular button states.
//
// Parameters:
//		[IN]	lpszSound
//				A string that specifies the sound to play.
//				If hMod is nullptr this string is interpreted as a filename, else it
//				is interpreted as a resource identifier.
//				Pass nullptr to remove any previously specified sound.
//		[IN]	hMod
//				Handle to the executable file that contains the resource to be loaded.
//				This parameter must be nullptr unless lpszSound specifies a resource identifier.
//		[IN]	bPlayOnClick
//				TRUE if the sound must be played when the button is clicked.
//				FALSE if the sound must be played when the mouse is moved over the button.
//		[IN]	bPlayAsync
//				TRUE if the sound must be played asynchronously.
//				FALSE if the sound must be played synchronously. The application takes control
//				when the sound is completely played.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
#ifdef	BTNST_USE_SOUND
DWORD CButtonST::SetSound(LPCTSTR lpszSound, HMODULE hMod, BOOL bPlayOnClick, BOOL bPlayAsync)
{
	BYTE	byIndex = bPlayOnClick ? 1 : 0;

	// Store new sound
	if (lpszSound)
	{
		if (hMod)	// From resource identifier ?
		{
			m_csSounds[byIndex].lpszSound = lpszSound;
		} // if
		else
		{
			_tcscpy(m_csSounds[byIndex].szSound, lpszSound);
			m_csSounds[byIndex].lpszSound = m_csSounds[byIndex].szSound;
		} // else

		m_csSounds[byIndex].hMod = hMod;
		m_csSounds[byIndex].dwFlags = SND_NODEFAULT | SND_NOWAIT;
		m_csSounds[byIndex].dwFlags |= hMod ? SND_RESOURCE : SND_FILENAME;
		m_csSounds[byIndex].dwFlags |= bPlayAsync ? SND_ASYNC : SND_SYNC;
	} // if
	else
	{
		// Or remove any existing
		::ZeroMemory(&m_csSounds[byIndex], sizeof(STRUCT_SOUND));
	} // else

	return BTNST_OK;
} // End of SetSound
#endif

// This function is called every time the button background needs to be painted.
// If the button is in transparent mode this function will NOT be called.
// This is a virtual function that can be rewritten in CButtonST-derived classes
// to produce a whole range of buttons not available by default.
//
// Parameters:
//		[IN]	pDC
//				Pointer to a CDC object that indicates the device context.
//		[IN]	pRect
//				Pointer to a CRect object that indicates the bounds of the
//				area to be painted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::OnDrawBackground(CDC* pDC, CRect* pRect)
{
	COLORREF	crColor;

	if (m_bIsFlat == FALSE)
	{
		if (m_bIsFocused || m_bIsDefault)
		{
			CBrush br(RGB(0,0,0));
			pDC->FrameRect(pRect, &br);
			pRect->DeflateRect(1, 1);
		} // if
	} // if

	if (m_bMouseOnButton || m_bIsPressed)
		crColor = m_crColors[BTNST_COLOR_BK_IN];
	else
	{
		if (m_bIsFocused)
			crColor = m_crColors[BTNST_COLOR_BK_FOCUS];
		else
			crColor = m_crColors[BTNST_COLOR_BK_OUT];
	} // else

	CBrush		brBackground(crColor);

	pDC->FillRect(pRect, &brBackground);

	return BTNST_OK;
} // End of OnDrawBackground

// This function is called every time the button border needs to be painted.
// This is a virtual function that can be rewritten in CButtonST-derived classes
// to produce a whole range of buttons not available by default.
//
// Parameters:
//		[IN]	pDC
//				Pointer to a CDC object that indicates the device context.
//		[IN]	pRect
//				Pointer to a CRect object that indicates the bounds of the
//				area to be painted.
//
// Return value:
//		BTNST_OK
//			Function executed successfully.
//
DWORD CButtonST::OnDrawBorder(CDC* pDC, CRect* pRect)
{
	// Draw pressed button
	if (m_bIsPressed)
	{
		if (m_bIsFlat)
		{
			if (m_bDrawBorder)
				pDC->Draw3dRect(pRect, ::GetSysColor(COLOR_BTNSHADOW), ::GetSysColor(COLOR_BTNHILIGHT));
		}
		else
		{
			CBrush brBtnShadow(GetSysColor(COLOR_BTNSHADOW));
			pDC->FrameRect(pRect, &brBtnShadow);
		}
	}
	else // ...else draw non pressed button
	{
		CPen penBtnHiLight(PS_SOLID, 0, GetSysColor(COLOR_BTNHILIGHT)); // White
		CPen pen3DLight(PS_SOLID, 0, GetSysColor(COLOR_3DLIGHT));       // Light gray
		CPen penBtnShadow(PS_SOLID, 0, GetSysColor(COLOR_BTNSHADOW));   // Dark gray
		CPen pen3DDKShadow(PS_SOLID, 0, GetSysColor(COLOR_3DDKSHADOW)); // Black

		if (m_bIsFlat)
		{
			if (m_bMouseOnButton && m_bDrawBorder)
				pDC->Draw3dRect(pRect, ::GetSysColor(COLOR_BTNHILIGHT), ::GetSysColor(COLOR_BTNSHADOW));
		}
		else
		{
			// Draw top-left borders
			// White line
			CPen* pOldPen = pDC->SelectObject(&penBtnHiLight);
			pDC->MoveTo(pRect->left, pRect->bottom-1);
			pDC->LineTo(pRect->left, pRect->top);
			pDC->LineTo(pRect->right, pRect->top);
			// Light gray line
			pDC->SelectObject(pen3DLight);
			pDC->MoveTo(pRect->left+1, pRect->bottom-1);
			pDC->LineTo(pRect->left+1, pRect->top+1);
			pDC->LineTo(pRect->right, pRect->top+1);
			// Draw bottom-right borders
			// Black line
			pDC->SelectObject(pen3DDKShadow);
			pDC->MoveTo(pRect->left, pRect->bottom-1);
			pDC->LineTo(pRect->right-1, pRect->bottom-1);
			pDC->LineTo(pRect->right-1, pRect->top-1);
			// Dark gray line
			pDC->SelectObject(penBtnShadow);
			pDC->MoveTo(pRect->left+1, pRect->bottom-2);
			pDC->LineTo(pRect->right-2, pRect->bottom-2);
			pDC->LineTo(pRect->right-2, pRect->top);
			//
			pDC->SelectObject(pOldPen);
		} // else
	} // else

	return BTNST_OK;
} // End of OnDrawBorder

#undef BS_TYPEMASK
