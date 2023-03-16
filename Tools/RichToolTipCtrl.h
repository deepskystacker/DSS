
////////////////////////////////////////////////////////////////////////////
// File:	RichToolTipCtrl.h
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
#pragma warning (disable:4100)
#if !defined(AFX_RICHTOOLTIPCTRL_H__EFAAE1BA_CDFD_4C7B_B778_146FAC4E3309__INCLUDED_)
#define AFX_RICHTOOLTIPCTRL_H__EFAAE1BA_CDFD_4C7B_B778_146FAC4E3309__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RichToolTipCtrl.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CRichToolTipCtrl window

class CRichToolTipCtrl : public CToolTipCtrl
{
	DECLARE_DYNAMIC(CRichToolTipCtrl);

// Construction
public:
	CRichToolTipCtrl();

// Attributes
public:

// Operations
public:
	static CString MakeTextRTFSafe(LPCTSTR lpszText);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichToolTipCtrl)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRichToolTipCtrl();

protected:
  /////////////////////////////////////////////////////////////////////////////
  // CRichToolTipRichEditCtrl window

  class CRichToolTipRichEditCtrl : public CRichEditCtrl
  {
  // Construction
  protected:
	  CRichToolTipRichEditCtrl();

  // Attributes
  protected:

  // Operations
  protected:
	  int StreamTextIn(LPCTSTR lpszText);

  // Overrides
	  // ClassWizard generated virtual function overrides
	  //{{AFX_VIRTUAL(CRichToolTipRichEditCtrl)
	  //}}AFX_VIRTUAL

  // Implementation
  protected:
	  virtual ~CRichToolTipRichEditCtrl();

	  // Generated message map functions
  protected:
	  //{{AFX_MSG(CRichToolTipRichEditCtrl)
	  //}}AFX_MSG

	  DECLARE_MESSAGE_MAP()

	  friend class CRichToolTipCtrl;
  };

	// Generated message map functions
protected:
	CRichToolTipRichEditCtrl m_edit;
	CSize m_sizeEditMin;

	CSize CalculateMinimiumRichEditSize();

	//{{AFX_MSG(CRichToolTipCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShow(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

// Interface Map
public:
	BEGIN_INTERFACE_PART(RichEditOleCallback, IRichEditOleCallback)
		INIT_INTERFACE_PART(CRichToolTipCtrl, RichEditOleCallback)
		STDMETHOD(GetNewStorage) (LPSTORAGE*);
		STDMETHOD(GetInPlaceContext) (LPOLEINPLACEFRAME*, LPOLEINPLACEUIWINDOW*, LPOLEINPLACEFRAMEINFO);
		STDMETHOD(ShowContainerUI) (BOOL);
		STDMETHOD(QueryInsertObject) (LPCLSID, LPSTORAGE, LONG);
		STDMETHOD(DeleteObject) (LPOLEOBJECT);
		STDMETHOD(QueryAcceptData) (LPDATAOBJECT, CLIPFORMAT*, DWORD,BOOL, HGLOBAL);
		STDMETHOD(ContextSensitiveHelp) (BOOL);
		STDMETHOD(GetClipboardData) (CHARRANGE*, DWORD, LPDATAOBJECT*);
		STDMETHOD(GetDragDropEffect) (BOOL, DWORD, LPDWORD);
		STDMETHOD(GetContextMenu) (WORD, LPOLEOBJECT, CHARRANGE*, HMENU*);
	END_INTERFACE_PART(RichEditOleCallback)

	DECLARE_INTERFACE_MAP()
};

inline STDMETHODIMP_(ULONG) CRichToolTipCtrl::XRichEditOleCallback::AddRef()
  { METHOD_PROLOGUE_EX_(CRichToolTipCtrl, RichEditOleCallback)
    return (ULONG)pThis->InternalAddRef(); }
inline STDMETHODIMP_(ULONG) CRichToolTipCtrl::XRichEditOleCallback::Release()
  { METHOD_PROLOGUE_EX_(CRichToolTipCtrl, RichEditOleCallback)
    return (ULONG)pThis->InternalRelease(); }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
  { METHOD_PROLOGUE_EX_(CRichToolTipCtrl, RichEditOleCallback)
    return (HRESULT)pThis->InternalQueryInterface(&iid, ppvObj); }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::GetInPlaceContext(
	LPOLEINPLACEFRAME* lplpFrame, LPOLEINPLACEUIWINDOW* lplpDoc,
	LPOLEINPLACEFRAMEINFO lpFrameInfo)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::ShowContainerUI(BOOL fShow)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::QueryInsertObject(
	LPCLSID /*lpclsid*/, LPSTORAGE /*pstg*/, LONG /*cp*/)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::DeleteObject(LPOLEOBJECT /*lpoleobj*/)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::QueryAcceptData(
	LPDATAOBJECT lpdataobj, CLIPFORMAT* lpcfFormat, DWORD reco,
	BOOL fReally, HGLOBAL hMetaPict)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::ContextSensitiveHelp(BOOL /*fEnterMode*/)
  { return E_NOTIMPL; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::GetClipboardData(
	CHARRANGE* lpchrg, DWORD reco, LPDATAOBJECT* lplpdataobj)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::GetDragDropEffect(
	BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
  { return S_OK; }
inline STDMETHODIMP CRichToolTipCtrl::XRichEditOleCallback::GetContextMenu(
	WORD seltype, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg,
	HMENU* lphmenu)
  { return S_OK; }

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHTOOLTIPCTRL_H__EFAAE1BA_CDFD_4C7B_B778_146FAC4E3309__INCLUDED_)
