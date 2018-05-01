/******************************************************************************

$Author$
  
$Modtime$
$Revision$

Description: Interfaces of the classes "CListCtrlEx" and "CListViewEx"
             (list control and list view with sort icons and
              colored sort column)

$Log$

******************************************************************************/

#pragma once

#ifndef LPUINT
typedef UINT *			LPUINT;
#endif

/*** Declaration of "workhorse" class "CListBase" ****************************/
class CListCtrlHelper;

class CListBase
{
  public:
   CListBase();
  ~CListBase();

  void         ColorSortColumn(BOOL bEnable = TRUE, int nSortColumn = 0);
  virtual void DrawSmallIcon  (CDC* pDC, LVITEM* pItem, LPRECT pRect);
  virtual void DrawStateIcon  (CDC* pDC, LVITEM* pItem, LPRECT pRect);
  virtual void DrawSubItemText(CDC* pDC, LVITEM* pItem, LVCOLUMN* pColumn,
                               LPRECT pRect);
  void EnableColumn      (int nColumn, BOOL bEnableIt = TRUE);
  void EnableColumnHiding(int nColumn, BOOL bEnableIt = TRUE);
  void EnableSortIcon    (BOOL bEnable = TRUE, int nSortColumn = 0);
  void EnableSubItemTips (BOOL bEnable = TRUE) {m_bSubItemTips = bEnable;};

  int GetColumnCount() const
  {
    return static_cast<int>(m_aColumnData.GetSize());
  }

  POSITION GetFirstCheckedItemPosition() const;
  int      GetNextCheckedItem         (POSITION& pos) const;
  int      GetSortColumn              () const {return m_iSortColumn;}
  BOOL                  GetState  (LPBYTE* ppState, LPUINT pnStateLen) const;
  virtual const CString GetToolTip(int /*nItem*/) {return _T("");}

  virtual const CString GetToolTip(int /*nItem*/, int /*nSubItem*/,
                                   UINT /*nFlags*/, BOOL& /*bAsLabel*/)
  {
    return _T("");
  }

  BOOL IsColumnEnabled(int nColumn) const
  {
    return m_aColumnData[nColumn]->m_bEnabled;
  }

  BOOL IsSubItemWidthSufficient(int nItem, int nSubItem, LPCTSTR pszText);
  BOOL KeepLabelLeft           (BOOL bKeepLeft = true);
  BOOL RestoreState            (LPCTSTR pszSection, LPCTSTR pszEntry);
  BOOL SaveState               (LPCTSTR pszSection, LPCTSTR pszEntry) const;
  void SetSortColumn           (int nColumn);
  BOOL SetState                (LPBYTE pState, UINT nStateLen);
  void ShowColumn              (int nColumn, BOOL bShowIt = TRUE);

	private:
  enum VISUAL_STYLE {Unknown, NotPresent, Present};

  friend class CListCtrlEx;
  friend class CListViewEx;
  friend class CLabelTipCtrl;

  struct COLUMN_DATA
  {
    COLUMN_DATA(): m_bEnabled      (true),
                   m_bHidingAllowed(false),
                   m_bVisible      (true),
                   m_nWidth        (0),
                   m_nOrder        (0),
                   m_pLVColumn     (0)
    {}

    ~COLUMN_DATA();

    bool      m_bEnabled;
    bool      m_bHidingAllowed;
    bool      m_bVisible;
    int       m_nWidth;
    int       m_nOrder;
    LVCOLUMN* m_pLVColumn;
  };

  struct ITEM_DATA
  {
     ITEM_DATA(): m_lParam(0) {}
    ~ITEM_DATA();

    LPARAM                   m_lParam;
    CArray<LVITEM*, LVITEM*> m_apLVItem;
  };

  static int CALLBACK CompareFunc          (LPARAM lParam1, LPARAM lParam2,
                                            LPARAM lParamSort);
  void                CreateSortIcons      ();
  void                DrawItem             (LPDRAWITEMSTRUCT lpDrawItemStruct);
  LVCOLUMN*           DupLVColumn          (LVCOLUMN* pLVColumn) const;
  LVITEM*             DupLVItem            (LVITEM* pLVItem) const;
  void                EraseRect            (CDC* pDC, LPRECT pRect);
  bool                GetLabelRect         (int nItem, int nSubItem, LPRECT pRect);
  LVITEM*             GetLVITEM            (int nItem, int nSubItem = 0) const;
  int                 GetLogicalIndex      (int nPhysicalColumn) const;
  int                 GetLogicalOrder      (int nPhysicalOrder) const;
  int                 GetPhysicalIndex     (int nColumnIndex) const;
  int                 GetPhysicalOrder     (int nColumnOrder) const;
  bool                GetStateIconRect     (int nItem, LPRECT pRect);
  void                HideColumn           (int nColumn);
  int                 IndexToOrder         (int nIndex);
  void                InitializeTipCtrls   ();
  void                InvalidateNonItemArea();
  void                JustifyFirstColumn   (int nFormat);
	BOOL                OnColumnclick        (NMHDR* pNMHDR, LRESULT* pResult);
  BOOL                OnCommand            (WPARAM wParam);
	void                OnContextMenu        (CWnd* pWnd, CPoint point);
	void                OnCustomDraw         (NMHDR* pNMHDR, LRESULT* pResult);
  LRESULT             OnDeleteAllItems     ();
  LRESULT             OnDeleteColumn       (WPARAM wParam);
  LRESULT             OnDeleteItem         (WPARAM wParam);
	void                OnDestroy            ();
	BOOL                OnEraseBkgnd         (CDC* pDC);
  LRESULT             OnFindItem           (WPARAM wParam, LPARAM lParam);  
  LRESULT             OnGetColumn          (WPARAM wParam, LPARAM lParam);
  LRESULT             OnGetColumnWidth     (WPARAM wParam);
  LRESULT             OnGetColumnOrderArray(WPARAM wParam, LPARAM lParam);
  BOOL                OnGetdispinfo        (NMHDR* pNMHDR);
  LRESULT             OnGetItem            (LPARAM lParam);
  LRESULT             OnGetItemRect        (WPARAM wParam, LPARAM lParam);
  LRESULT             OnGetItemText        (WPARAM wParam, LPARAM lParam);
  LRESULT             OnGetSubItemRect     (WPARAM wParam, LPARAM lParam);
  int                 OnHitTest            (LPARAM lParam);
  void                OnHScroll            ();
  LRESULT             OnInsertColumn       (WPARAM wParam, LPARAM lParam);
  LRESULT             OnInsertItem         (LPARAM lParam);
	void                OnKeyDown            (UINT nChar);
	void                OnKeyUp              (UINT nChar);
	void                OnKillFocus          ();
	void                OnLButtonDblClk      (CPoint point);
	void                OnLButtonDown        (CPoint point);
  void                OnLButtonUp          ();
  LRESULT             OnMouseLeave         ();
  void                OnMouseMove          (CPoint point) ;
  BOOL                OnMouseWheel         (CPoint point);
	BOOL                OnNotify             (LPARAM lParam);
  BOOL                OnODCacheHint        (NMHDR* pNMHDR);
  LRESULT             OnSetBkColor         ();
  LRESULT             OnSetColumn          (WPARAM wParam, LPARAM lParam);
  LRESULT             OnSetColumnOrderArray(WPARAM wParam, LPARAM lParam);
  LRESULT             OnSetColumnWidth     (WPARAM wParam, LPARAM lParam);
  LRESULT             OnSetExtendedStyle   (WPARAM wParam, LPARAM lParam);
	void                OnSetFocus           ();
  LRESULT             OnSetImageList       (WPARAM wParam);
  LRESULT             OnSetItem            (LPARAM lParam);
  LRESULT             OnSetItemText        (WPARAM wParam, LPARAM lParam);
  LRESULT             OnSortItems          (WPARAM wParam, LPARAM lParam);
	void                OnSysColorChange     ();
  int                 OrderToIndex         (int nOrder);
  void                PrepareHotUnderlining();
  void                RedisplayColumn      (int nColumn);
  void                RefreshToolTips      ();
  void                SetHotCursor         (LPLVHITTESTINFO pInfo);
  void                SetSortIcon          ();

  static UINT                        m_winver;
  static VISUAL_STYLE                m_visualStyle;
  static const int                   m_iFirstColXOff;
  static const int                   m_iNextColXOff;
  int                                m_iIconXOff; // offset of icon may vary of
                                                  // unknown reason
  CListCtrlHelper*                   m_pListCtrl;
  bool                               m_bMouseInClientArea;
  bool                               m_bTopMost;
  bool                               m_bFocusSet;
  CArray<COLUMN_DATA*, COLUMN_DATA*> m_aColumnData;
  int                                m_iColumnHidingAllowed;
  BOOL                               m_bSortIconEnabled;
  BOOL                               m_bColorSortColumn;
  CImageList                         m_imglstSortIcons;
  CBitmap                            m_bmpUpArrow;
  CBitmap                            m_bmpDownArrow;
  int                                m_iUpArrow;
  int                                m_iDownArrow;
  DWORD                              m_dwColSortColor;
  int                                m_iSortColumn;
  int                                m_iForm_ttofSubItem0;
  BOOL                               m_bKeepLabelLeft;
  bool                               m_bLocked;
  bool                               m_bControl;
  bool                               m_bIconXOffCalculated;
  DWORD                              m_dwExtendedStyle;
  int                                m_iHotItem;
  COLORREF                           m_dwHotLite;
  HCURSOR                            m_hcursorCustom;
  HCURSOR                            m_hcursorArrow;
  HCURSOR                            m_hcursorHand;
  PFNLVCOMPARE                       m_pfnLVCompare;
  LPARAM                             m_lParamSort;
  int                                m_iFirstCachedItem;
  int                                m_iLastCachedItem;
  bool                               m_bOnGetDispinfo;
  BOOL                               m_bSubItemTips;
  bool                               m_bUpdateToolTips;
  CToolTipCtrl*                      m_pToolTip;
  CLabelTipCtrl*                     m_pLabelTip;
  CString                            m_strCurrentToolTip;
  CString                            m_strCurrentLabelTip;
  bool                               m_bToolTips;
  bool                               m_bUnfoldLabel;
  BOOL                               m_bLabelUnfolded;
  bool                               m_bOnPaint;

  int                                m_iCurrentItem;
  int                                m_iCurrentSubItem;
  UINT                               m_fCurrentFlags;
};


/*** Declaration of class "CListCtrlEx" **************************************/
class CListCtrlEx: public CListCtrl, public CListBase
{
  DECLARE_DYNCREATE(CListCtrlEx);

  public:
  CListCtrlEx() {m_pListCtrl = reinterpret_cast<CListCtrlHelper*>(this);}

  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
  {
    CListBase::DrawItem(lpDrawItemStruct);
  }

  protected:
  // Overrides
	// ClassWizard generated virtual function overrides
	virtual BOOL OnCommand        (WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify         (WPARAM wParam, LPARAM lParam,
                                 LRESULT* pResult);
	virtual void PreSubclassWindow();

	// Generated message map functions
	afx_msg BOOL    OnColumnclick        (NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void    OnContextMenu        (CWnd* pWnd, CPoint point);
	afx_msg void    OnCustomDraw         (NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg LRESULT OnDeleteAllItems     (WPARAM, LPARAM);
  afx_msg LRESULT OnDeleteColumn       (WPARAM wParam, LPARAM);
  afx_msg LRESULT OnDeleteItem         (WPARAM wParam, LPARAM);
	afx_msg void    OnDestroy            ();
	afx_msg BOOL    OnEraseBkgnd         (CDC* pDC);
  afx_msg LRESULT OnFindItem           (WPARAM wParam, LPARAM lParam);  
  afx_msg LRESULT OnGetColumn          (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetColumnOrderArray(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetColumnWidth     (WPARAM wParam, LPARAM);
  afx_msg BOOL    OnGetdispinfo        (NMHDR* pNMHDR, LRESULT*);
  afx_msg LRESULT OnGetExtendedStyle   (WPARAM, LPARAM);
  afx_msg LRESULT OnGetItem            (WPARAM, LPARAM lParam);
  afx_msg LRESULT OnGetItemRect        (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetItemText        (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetSubItemRect     (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnHitTest            (WPARAM, LPARAM lParam);
	afx_msg void    OnHScroll            (UINT nSBCode, UINT nPos,
                                        CScrollBar* pScrollBar);
  afx_msg LRESULT OnInsertColumn       (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnInsertItem         (WPARAM, LPARAM lParam);
	afx_msg void    OnKeyDown            (UINT nChar, UINT nRepCnt,
                                        UINT nFlags);
	afx_msg void    OnKeyUp              (UINT nChar, UINT nRepCnt,
                                        UINT nFlags);
	afx_msg void    OnKillFocus          (CWnd* pNewWnd);
	afx_msg void    OnLButtonDblClk      (UINT nFlags, CPoint point);
	afx_msg void    OnLButtonDown        (UINT nFlags, CPoint point);
	afx_msg void    OnLButtonUp          (UINT nFlags, CPoint point);
  afx_msg LRESULT OnMouseLeave         (WPARAM, LPARAM);
	afx_msg void    OnMouseMove          (UINT, CPoint point);
  afx_msg BOOL    OnMouseWheel         (UINT, short, CPoint pt);
  afx_msg BOOL    OnODCacheHint        (NMHDR* pNMHDR, LRESULT*);
  afx_msg LRESULT OnSetBkColor         (WPARAM, LPARAM);
  afx_msg LRESULT OnSetColumn          (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetColumnOrderArray(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetColumnWidth     (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetExtendedStyle   (WPARAM wParam, LPARAM lParam);
	afx_msg void    OnSetFocus           (CWnd* pOldWnd);
  afx_msg LRESULT OnSetImageList       (WPARAM wParam, LPARAM);
  afx_msg LRESULT OnSetItem            (WPARAM, LPARAM lParam);
  afx_msg LRESULT OnSetItemText        (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSortItems          (WPARAM wParam, LPARAM lParam);
	afx_msg void    OnSysColorChange     ();

	DECLARE_MESSAGE_MAP()
};


/*** Declaration of class "CListViewEx" **************************************/
class CListViewEx: public CListView, public CListBase
{
  DECLARE_DYNCREATE(CListViewEx);

  public:
  CListViewEx()
  {
    m_pListCtrl = reinterpret_cast<CListCtrlHelper*>(&GetListCtrl());
  }

  virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
  {
    CListBase::DrawItem(lpDrawItemStruct);
  }

  protected:
  // Overrides
	// ClassWizard generated virtual function overrides
	virtual BOOL OnCommand      (WPARAM wParam, LPARAM lParam);
  virtual void OnInitialUpdate();
  virtual BOOL OnNotify       (WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	// Generated message map functions
	afx_msg BOOL    OnColumnclick        (NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void    OnContextMenu        (CWnd* pWnd, CPoint point);
	afx_msg void    OnCustomDraw         (NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg LRESULT OnDeleteAllItems     (WPARAM, LPARAM);
  afx_msg LRESULT OnDeleteColumn       (WPARAM wParam, LPARAM);
  afx_msg LRESULT OnDeleteItem         (WPARAM wParam, LPARAM);
	afx_msg void    OnDestroy            ();
	afx_msg BOOL    OnEraseBkgnd         (CDC* pDC);
  afx_msg LRESULT OnFindItem           (WPARAM wParam, LPARAM lParam);  
  afx_msg LRESULT OnGetColumn          (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetColumnOrderArray(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetColumnWidth     (WPARAM wParam, LPARAM);
  afx_msg BOOL    OnGetdispinfo        (NMHDR* pNMHDR, LRESULT*);
  afx_msg LRESULT OnGetExtendedStyle   (WPARAM, LPARAM);
  afx_msg LRESULT OnGetItem            (WPARAM, LPARAM lParam);
  afx_msg LRESULT OnGetItemRect        (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetItemText        (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnGetSubItemRect     (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnHitTest            (WPARAM, LPARAM lParam);
	afx_msg void    OnHScroll            (UINT nSBCode, UINT nPos,
                                        CScrollBar* pScrollBar);
  afx_msg LRESULT OnInsertColumn       (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnInsertItem         (WPARAM, LPARAM lParam);
	afx_msg void    OnKeyDown            (UINT nChar, UINT nRepCnt,
                                        UINT nFlags);
	afx_msg void    OnKeyUp              (UINT nChar, UINT nRepCnt,
                                        UINT nFlags);
	afx_msg void    OnKillFocus          (CWnd* pNewWnd);
	afx_msg void    OnLButtonDblClk      (UINT nFlags, CPoint point);
	afx_msg void    OnLButtonDown        (UINT nFlags, CPoint point);
	afx_msg void    OnLButtonUp          (UINT nFlags, CPoint point);
  afx_msg LRESULT OnMouseLeave         (WPARAM, LPARAM);
	afx_msg void    OnMouseMove          (UINT, CPoint point);
  afx_msg BOOL    OnMouseWheel         (UINT, short, CPoint pt);
  afx_msg BOOL    OnODCacheHint        (NMHDR* pNMHDR, LRESULT*);
  afx_msg LRESULT OnSetBkColor         (WPARAM, LPARAM);
  afx_msg LRESULT OnSetColumn          (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetColumnOrderArray(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetColumnWidth     (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSetExtendedStyle   (WPARAM wParam, LPARAM lParam);
	afx_msg void    OnSetFocus           (CWnd* pOldWnd);
  afx_msg LRESULT OnSetImageList       (WPARAM wParam, LPARAM);
  afx_msg LRESULT OnSetItem            (WPARAM, LPARAM lParam);
  afx_msg LRESULT OnSetItemText        (WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnSortItems          (WPARAM wParam, LPARAM lParam);
	afx_msg void    OnSysColorChange     ();

	DECLARE_MESSAGE_MAP()
};
