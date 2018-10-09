#if !defined(AFX_PICTURELISTCTRL_H__D2ACDE68_6141_4B93_BD30_6B71D7497D34__INCLUDED_)
#define AFX_PICTURELISTCTRL_H__D2ACDE68_6141_4B93_BD30_6B71D7497D34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PictureListCtrl.h : header file
//

#include "ListViewCtrlEx.h"
#include "DSSProgress.h"
#include "FrameList.h"
#include "StackingTasks.h"

/////////////////////////////////////////////////////////////////////////////
// CPictureListCtrl window

const DWORD				WM_CHECKITEM = (WM_USER + 1);
const DWORD				WM_LISTITEMCHANGED = (WM_USER + 2);
const DWORD				WM_SELECTITEM = (WM_USER + 3);

class CPictureListCtrl : public CListCtrlEx,
					     public CFrameList
{
private :
	LONG				m_lSortColumn;
	BOOL				m_bAscending;
	CDSSProgress *		m_pProgress;
	CImageList			m_ImageList;
	DWORD				m_dwCurrentGroupID;
	GUID				m_dwCurrentJobID;
	std::vector<LONG>	m_vVisibles;
	BOOL				m_bRefreshNeeded;
	CBitmap				m_bmpGrayScale;
	CBitmap				m_bmpCFACYMG;
	CBitmap				m_bmpCFARGB;
	CBitmap				m_bmpColor;
	CBitmap				m_bmpMask;

// Construction
public:
	CPictureListCtrl();

// Attributes
public:
	void				RefreshList();

// Operations
public:
	void	Initialize();
	void	AddFileToList(LPCTSTR szFile, DWORD dwGroupID, GUID dwJobID, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, BOOL bCheck = FALSE, int nItem = -1);
	virtual BOOL AddFile(LPCTSTR szFile, DWORD dwGroupID, GUID dwJobID, PICTURETYPE PictureType = PICTURETYPE_LIGHTFRAME, BOOL bCheck = FALSE)
	{
		AddFileToList(szFile, dwGroupID, dwJobID, PictureType, bCheck);
		return TRUE;
	};

	void	SetCurrentGroupID(DWORD dwGroupID)
	{
		if (m_dwCurrentGroupID != dwGroupID)
		{
			m_dwCurrentGroupID = dwGroupID;
			SetItemCount(0);
			RefreshList();
		};
	};

	void	SetCurrentJobID(GUID dwJobID)
	{
		if (m_dwCurrentJobID != dwJobID)
		{
			m_dwCurrentJobID = dwJobID;
			SetItemCount(0);
			RefreshList();
		};
	};

	DWORD	GetCurrentGroupID()
	{
		return m_dwCurrentGroupID;
	};

	GUID	GetCurrentJobID()
	{
		return m_dwCurrentJobID;
	};

	DWORD	GetLastGroupID()
	{
		DWORD			dwResult = 0;

		for (LONG i = 0;i<m_vFiles.size();i++)
		{
			if (!m_vFiles[i].m_bRemoved)
				dwResult = max(dwResult, m_vFiles[i].m_dwGroupID);
		};

		return dwResult;
	};

	int		CompareItems(LONG lItem1, LONG lItem2);
	BOOL	GetSelectedFileName(CString & strFileName);
	BOOL	GetItemFileName(int nItem, CString & strFileName);
	BOOL	GetFirstCheckedLightFrame(CString & strFileName);
	BOOL	GetItemISOSpeedAndExposure(int nItem, LONG & lISOSpeed, double & fExposure);
	void	UpdateOffset(LPCTSTR szFileName, double fdX, double fdY, double fAngle, const CBilinearParameters & Transformation, const VOTINGPAIRVECTOR & vVotedPairs);
	void	ClearOffset(LPCTSTR szFileName);
	int		FindIndice(LPCTSTR szFileName);
	BOOL	IsLightFrame(LPCTSTR szFileName)
	{
		BOOL		bResult = FALSE;
		int			nIndice = FindIndice(szFileName);

		if (nIndice>=0)
			bResult = m_vFiles[nIndice].IsLightFrame() && !m_vFiles[nIndice].m_bRemoved;

		return bResult;
	};

	void	FillJobs(CAllStackingJobs & jobs);

	void	CheckBest(double fPercent);
	void	UnCheckNonStackable();

	BOOL	AreCheckedPictureCompatible();
	BOOL	GetPictureSizes(int nItem, LONG & lWidth, LONG & lHeight, LONG & lNrChannels);

	BOOL	SaveState();

	LONG	GetNrCheckedFrames(LONG lGroupID = -1);
	LONG	GetNrCheckedDarks(LONG lGroupID = -1);
	LONG	GetNrCheckedDarkFlats(LONG lGroupID = -1);
	LONG	GetNrCheckedFlats(LONG lGroupID = -1);
	LONG	GetNrCheckedOffsets(LONG lGroupID = -1);
	LONG	GetNrFrames(LONG lGroupID = -1);
	BOOL	IsChecked(int nItem)
	{
		LONG		lIndice = m_vVisibles[nItem];

		return m_vFiles[lIndice].m_bChecked;
	};

	BOOL	IsChecked(LPCTSTR szFileName)
	{
		BOOL		bResult = FALSE;
		int			nIndice;

		nIndice = FindIndice(szFileName);
		if (nIndice>=0)
			bResult = m_vFiles[nIndice].m_bChecked;

		return bResult;
	};

	void	CheckAbove(double fThreshold);

	void	SortListQualityDesc();

	void	CheckAll(BOOL bCheck);
	void	CheckAllLights(BOOL bCheck);
	void	CheckAllDarks(BOOL bCheck);
	void	CheckAllFlats(BOOL bCheck);
	void	CheckAllOffsets(BOOL bCheck);

	void	CheckImage(LPCTSTR szImage, BOOL bCheck);

	void	SetUseAsStarting(int nItem, BOOL bUse);
	BOOL	GetTransformation(LPCTSTR szFile, CBilinearParameters & Transformation, VOTINGPAIRVECTOR & vVotedPairs);

	void	SetProgress(CDSSProgress * pProgress)
	{
		m_pProgress = pProgress;
	};

	void	Clear()
	{
		DeleteAllItems();
		m_vFiles.clear();
	};

	void	CopyToClipboard();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureListCtrl)
	//}}AFX_VIRTUAL

// Implementation
private :
	//void	UpdateOffset(int nItem);
	void	ChangePictureType(int nItem, PICTURETYPE PictureType);

	void	PostItemChanged()
	{
		PostMessage(WM_LISTITEMCHANGED);
	};

	void	ToggleCheckBox(int nItem)
	{
		//Change check box
		LONG			lIndice = m_vVisibles[nItem];

		m_vFiles[lIndice].m_bChecked = !m_vFiles[lIndice].m_bChecked;

		//And redraw
		RedrawItems(nItem, nItem);
		m_bRefreshNeeded = TRUE;
		PostItemChanged();
	}

public:
	virtual ~CPictureListCtrl();

	void	SortList(int nSubItem);
	void	UpdateItemScores(LPCTSTR szFileName);
	void	UpdateCheckedItemScores();
	void	BlankCheckedItemScores();
	void	ClearOffsets();

	void	SaveList(CMRUList & MRUList, CString & strFileList);
	void	LoadList(CMRUList & MRUList, CString & strFileList);

	const CListBitmap & GetItem(int nIndice);

	// Generated message map functions
protected:
	//{{AFX_MSG(CPictureListCtrl)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown( UINT, CPoint );
	afx_msg void OnRButtonDown( UINT, CPoint );
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnListItemChanged(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PICTURELISTCTRL_H__D2ACDE68_6141_4B93_BD30_6B71D7497D34__INCLUDED_)
