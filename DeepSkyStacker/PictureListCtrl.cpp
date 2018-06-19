// PictureListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "PictureListCtrl.h"
#include "FileProperty.h"
#include "DeepStack.h"
#include "Registry.h"

#define _USE_MATH_DEFINES
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Change the list version each time a new column is added
const DWORD		LISTVERSION	    = 1;

const DWORD		COLUMN_CHECKED	= 0;
const DWORD 	COLUMN_PATH		= 1;
const DWORD 	COLUMN_FILE		= 2;
const DWORD 	COLUMN_FTYPE	= 3;
const DWORD 	COLUMN_SCORE	= 4;
const DWORD 	COLUMN_DX		= 5;
const DWORD 	COLUMN_DY		= 6;
const DWORD 	COLUMN_ANGLE	= 7;
const DWORD		COLUMN_FILETIME = 8;
const DWORD		COLUMN_SIZES	= 9;
const DWORD		COLUMN_CFA		= 10;
const DWORD		COLUMN_DEPTH	= 11;
const DWORD		COLUMN_INFO		= 12;
const DWORD		COLUMN_ISO		= 13;
const DWORD		COLUMN_EXPOSURE = 14;
const DWORD		COLUMN_FWHM		= 15;
const DWORD		COLUMN_STARS	= 16;
const DWORD		COLUMN_SKYBACKGROUND = 17;

#define M_PI			3.141592654


/* ------------------------------------------------------------------- */

static LONG	CompareDate(const SYSTEMTIME & FileTime1, const SYSTEMTIME & FileTime2)
{
	LONG			lResult = 0;

	if (FileTime1.wYear < FileTime2.wYear)
		lResult = -1;
	else if (FileTime1.wYear > FileTime2.wYear)
		lResult = 1;
	else if (FileTime1.wMonth < FileTime2.wMonth)
		lResult = -1;
	else if (FileTime1.wMonth > FileTime2.wMonth)
		lResult = 1;
	else if (FileTime1.wDay < FileTime2.wDay)
		lResult = -1;
	else if (FileTime1.wDay > FileTime2.wDay)
		lResult = 1;
	else if (FileTime1.wHour < FileTime2.wHour)
		lResult = -1;
	else if (FileTime1.wHour > FileTime2.wHour)
		lResult = 1;
	else if (FileTime1.wMinute < FileTime2.wMinute)
		lResult = -1;
	else if (FileTime1.wMinute > FileTime2.wMinute)
		lResult = 1;
	else if (FileTime1.wSecond < FileTime2.wSecond)
		lResult = -1;
	else if (FileTime1.wSecond > FileTime2.wSecond)
		lResult = 1;
	else if (FileTime1.wMilliseconds < FileTime2.wMilliseconds)
		lResult = -1;
	else if (FileTime1.wMilliseconds > FileTime2.wMilliseconds)
		lResult = 1;

	return lResult;
};

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CPictureListCtrl

CPictureListCtrl::CPictureListCtrl()
{
	m_lSortColumn		= -1;
	m_bAscending		= FALSE;
	m_pProgress			= NULL;
	m_dwCurrentGroupID	= 0;
	m_bRefreshNeeded	= FALSE;

	m_dwCurrentJobID	= MAINJOBID;
}

CPictureListCtrl::~CPictureListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPictureListCtrl, CListCtrlEx)
	//{{AFX_MSG_MAP(CPictureListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydownList)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_MESSAGE(WM_LISTITEMCHANGED, OnListItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CPictureListCtrl message handlers

static LONG	GetListVersion()
{
	CRegistry			reg;
	DWORD				dwVersion = 0;

	reg.LoadKey(REGENTRY_BASEKEY_DEEPSKYSTACKER, _T("ListVersion"), dwVersion);

	return dwVersion;
};

static void SetListVersion(LONG lVersion = LISTVERSION)
{
	CRegistry			reg;

	reg.SaveKey(REGENTRY_BASEKEY_DEEPSKYSTACKER, _T("ListVersion"), lVersion);
};

void CPictureListCtrl::Initialize()
{
	LONG			lVersion = GetListVersion();

	m_ImageList.Create(36, 18, ILC_COLOR32 | ILC_MASK, 0, 5);

	m_bmpGrayScale.Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_IMAGELIST_GRAYSCALE)));
	m_bmpCFACYMG.Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_IMAGELIST_CFA_CYMG)));
	m_bmpCFARGB.Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_IMAGELIST_CFA_RGB)));
	m_bmpColor.Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_IMAGELIST_COLOR)));
	m_bmpMask.Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_IMAGELIST_MASK)));

	m_ImageList.Add(&m_bmpGrayScale, &m_bmpMask);
	m_ImageList.Add(&m_bmpCFACYMG, &m_bmpMask);
	m_ImageList.Add(&m_bmpCFARGB, &m_bmpMask);
	m_ImageList.Add(&m_bmpColor, &m_bmpMask);

	SetImageList(&m_ImageList, LVSIL_SMALL);

	EnableColumnHiding(FALSE);
	EnableSortIcon(TRUE);
	ColorSortColumn(TRUE);

	SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	InsertColumn(0, _T(""), LVCFMT_LEFT, 46);
	CString				strColumn;

	strColumn.LoadString(IDS_COLUMN_PATH);
	InsertColumn(COLUMN_PATH, strColumn, LVCFMT_LEFT, 200);
	strColumn.LoadString(IDS_COLUMN_FILE);
	InsertColumn(COLUMN_FILE, strColumn, LVCFMT_LEFT, 200);
	strColumn.LoadString(IDS_COLUMN_TYPE);
	InsertColumn(COLUMN_FTYPE, strColumn, LVCFMT_LEFT, 60);
	strColumn.LoadString(IDS_COLUMN_SCORE);
	InsertColumn(COLUMN_SCORE, strColumn, LVCFMT_RIGHT, 80);
	strColumn.LoadString(IDS_COLUMN_DX);
	InsertColumn(COLUMN_DX, strColumn, LVCFMT_RIGHT, 60);
	strColumn.LoadString(IDS_COLUMN_DY);
	InsertColumn(COLUMN_DY, strColumn, LVCFMT_RIGHT, 60);
	strColumn.LoadString(IDS_COLUMN_ANGLE);
	InsertColumn(COLUMN_ANGLE, strColumn, LVCFMT_RIGHT, 60);
	strColumn.LoadString(IDS_COLUMN_DATETIME);
	InsertColumn(COLUMN_FILETIME, strColumn, LVCFMT_RIGHT, 120);
	strColumn.LoadString(IDS_COLUMN_SIZES);
	InsertColumn(COLUMN_SIZES, strColumn, LVCFMT_RIGHT, 100);
	strColumn.LoadString(IDS_COLUMN_CFA);
	InsertColumn(COLUMN_CFA, strColumn, LVCFMT_CENTER, 20);
	strColumn.LoadString(IDS_COLUMN_DEPTH);
	InsertColumn(COLUMN_DEPTH, strColumn, LVCFMT_LEFT, 50);
	strColumn.LoadString(IDS_COLUMN_INFOS);
	InsertColumn(COLUMN_INFO, strColumn, LVCFMT_LEFT, 50);
	strColumn.LoadString(IDS_COLUMN_ISO);
	InsertColumn(COLUMN_ISO, strColumn, LVCFMT_RIGHT, 50);
	strColumn.LoadString(IDS_COLUMN_EXPOSURE);
	InsertColumn(COLUMN_EXPOSURE, strColumn, LVCFMT_RIGHT, 50);

	InsertColumn(COLUMN_FWHM, _T("FWHM"), LVCFMT_RIGHT, 50);

	if (!lVersion)
		RestoreState(_T("PictureList"), _T("Settings"));

	strColumn.LoadString(IDS_COLUMN_STARS);
	InsertColumn(COLUMN_STARS, strColumn, LVCFMT_RIGHT, 50);
	strColumn.LoadString(IDS_COLUMN_SKYBACKGROUND);
	InsertColumn(COLUMN_SKYBACKGROUND, strColumn, LVCFMT_RIGHT, 50);
	if (!lVersion)
	{
		// Move the column before the score column
		std::vector<INT>			vColumns;
		std::vector<INT>::iterator	it;
		BOOL						bFound = FALSE;

		vColumns.resize(GetColumnCount());
		GetColumnOrderArray(&vColumns[0]);
		// Move the column
		for (it = vColumns.begin();it!=vColumns.end() && !bFound;it++)
		{
			if ((*it) == COLUMN_SCORE)
			{
				vColumns.insert(it, COLUMN_STARS);
				bFound = TRUE;
			};
		};
		if (bFound)
		{
			// Remove the last column
			vColumns.resize(vColumns.size()-1);
		};

		SetColumnOrderArray((LONG)vColumns.size(), &vColumns[0]);
	};

	if (lVersion == LISTVERSION)
		RestoreState(_T("PictureList"), _T("Settings"));

	SetColumnWidth(COLUMN_CHECKED, 85);

	m_lSortColumn = GetSortColumn();
	if (m_lSortColumn < 0)
	{
		m_bAscending = FALSE;
		m_lSortColumn = -m_lSortColumn;
		m_lSortColumn--;
	}
	else if (m_lSortColumn > 0)
	{
		m_bAscending = TRUE;
		m_lSortColumn--;
	};
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::SaveState()
{
	CListCtrlEx::SaveState(_T("PictureList"), _T("Settings"));
	SetListVersion();
	return TRUE;
};

/* ------------------------------------------------------------------- */

int CALLBACK PictureListCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CPictureListCtrl *		pList = (CPictureListCtrl *)lParamSort;

	if (pList)
		return pList->CompareItems(lParam1, lParam2);
	else
		return 0;
};

/* ------------------------------------------------------------------- */

int	CPictureListCtrl::CompareItems(LONG lItem1, LONG lItem2)
{
	LONG					lResult = 0;

	if (m_vFiles[lItem1].m_dwGroupID == m_vFiles[lItem2].m_dwGroupID)
	{
		switch (m_lSortColumn)
		{
		case 0 :
			break;
		case COLUMN_PATH :
			lResult = m_vFiles[lItem1].m_strPath.CompareNoCase(m_vFiles[lItem2].m_strPath);
			break;
		case COLUMN_FILE :
			lResult = m_vFiles[lItem1].m_strFileName.CompareNoCase(m_vFiles[lItem2].m_strFileName);
			break;
		case COLUMN_FTYPE :
			lResult = m_vFiles[lItem1].m_strType.CompareNoCase(m_vFiles[lItem2].m_strType);
			break;
		case COLUMN_INFO :
			lResult = m_vFiles[lItem1].m_strInfos.CompareNoCase(m_vFiles[lItem2].m_strInfos);
			break;
		case COLUMN_DEPTH :
			lResult = m_vFiles[lItem1].m_strDepth.CompareNoCase(m_vFiles[lItem2].m_strDepth);
			break;
		case COLUMN_SIZES :
			lResult = m_vFiles[lItem1].m_strSizes.CompareNoCase(m_vFiles[lItem2].m_strSizes);
			break;
		case COLUMN_CFA :
			if (m_vFiles[lItem1].GetCFAType() < m_vFiles[lItem2].GetCFAType())
				lResult = -1;
			else if (m_vFiles[lItem1].GetCFAType() > m_vFiles[lItem2].GetCFAType())
				lResult = 1;
			else
				lResult = 0;
			break;
		case COLUMN_SCORE :
			if (!m_vFiles[lItem1].IsLightFrame())
				lResult = -1;
			else if (!m_vFiles[lItem2].IsLightFrame())
				lResult = 1;
			else if (m_vFiles[lItem1].m_fOverallQuality < m_vFiles[lItem2].m_fOverallQuality)
				lResult= -1;
			else if (m_vFiles[lItem1].m_fOverallQuality > m_vFiles[lItem2].m_fOverallQuality)
				lResult = 1;
			break;
		case COLUMN_DX :
			if (!m_vFiles[lItem1].IsLightFrame())
				lResult = -1;
			else if (!m_vFiles[lItem2].IsLightFrame())
				lResult = 1;
			else if (m_vFiles[lItem1].m_dX< m_vFiles[lItem2].m_dX)
				lResult= -1;
			else if (m_vFiles[lItem1].m_dX > m_vFiles[lItem2].m_dX)
				lResult = 1;
			break;
		case COLUMN_DY :
			if (!m_vFiles[lItem1].IsLightFrame())
				lResult = -1;
			else if (!m_vFiles[lItem2].IsLightFrame())
				lResult = 1;
			else if (m_vFiles[lItem1].m_dY< m_vFiles[lItem2].m_dY)
				lResult= -1;
			else if (m_vFiles[lItem1].m_dY > m_vFiles[lItem2].m_dY)
				lResult = 1;
			break;
		case COLUMN_ANGLE :
			if (!m_vFiles[lItem1].IsLightFrame())
				lResult = -1;
			else if (!m_vFiles[lItem2].IsLightFrame())
				lResult = 1;
			else if (m_vFiles[lItem1].m_fAngle< m_vFiles[lItem2].m_fAngle)
				lResult= -1;
			else if (m_vFiles[lItem1].m_fAngle > m_vFiles[lItem2].m_fAngle)
				lResult = 1;
			break;
		case COLUMN_FILETIME :
			lResult = CompareDate(m_vFiles[lItem1].m_DateTime, m_vFiles[lItem2].m_DateTime);
			break;
		case COLUMN_ISO :
			if (m_vFiles[lItem1].m_lISOSpeed < m_vFiles[lItem2].m_lISOSpeed)
				lResult = -1;
			else
				lResult = 1;
			break;
		case COLUMN_EXPOSURE :
			if (m_vFiles[lItem1].m_fExposure< m_vFiles[lItem2].m_fExposure)
				lResult = -1;
			else
				lResult = 1;
			break;
		case COLUMN_FWHM :
			if (m_vFiles[lItem1].m_fFWHM< m_vFiles[lItem2].m_fFWHM)
				lResult = -1;
			else
				lResult = 1;
			break;
		case COLUMN_STARS :
			if (m_vFiles[lItem1].m_lNrStars< m_vFiles[lItem2].m_lNrStars)
				lResult = -1;
			else
				lResult = 1;
			break;
		case COLUMN_SKYBACKGROUND :
			if (m_vFiles[lItem1].m_SkyBackground.m_fLight < m_vFiles[lItem2].m_SkyBackground.m_fLight)
				lResult = -1;
			else
				lResult = 1;
			break;
		};
	}
	else if (m_vFiles[lItem1].m_dwGroupID < m_vFiles[lItem2].m_dwGroupID)
		lResult = -1;
	else
		lResult = 1;

	if (!m_bAscending)
		lResult = -lResult;

	return lResult;
};

/* ------------------------------------------------------------------- */

static	CPictureListCtrl *		g_pCompareList = NULL;

bool CompareVisibleItems(LONG lItem1, LONG lItem2) 
{
	if (g_pCompareList)
		return g_pCompareList->CompareItems(lItem1, lItem2) < 0;
	else
		return false;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::SortList(int nSubItem)
{
	if (nSubItem == m_lSortColumn)
		m_bAscending = !m_bAscending;
	else
		m_bAscending = TRUE;

	m_lSortColumn = nSubItem;
	if (m_bAscending)
		SetSortColumn(m_lSortColumn+1);
	else
		SetSortColumn(-(m_lSortColumn+1));

	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::RefreshList()
{
	LONG					lItemCount = 0;
	std::vector<CString>	vSelected;
	CString					strFocus;
	POSITION				pos;
	LONG					i;
	int						nItem;
	LONG					lIndice;
	BOOL					bFound = FALSE;

	pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		nItem = GetNextSelectedItem(pos);
		lIndice = m_vVisibles[nItem];

		vSelected.push_back(m_vFiles[lIndice].m_strFileName);

		if (!strFocus.GetLength())
			strFocus = m_vFiles[lIndice].m_strFileName;
	};

	// Retrieve the item having the focus
	for (i = 0;i<GetItemCount() && !bFound;i++)
	{
		if (GetItemState(i, LVIF_STATE) & LVIS_FOCUSED)
		{
			strFocus = m_vFiles[m_vVisibles[i]].m_strFileName;
			bFound = TRUE;
		};
	};

	m_vVisibles.clear();
	// Get Item count for current group
	for (i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && (m_vFiles[i].m_dwGroupID == m_dwCurrentGroupID))
		{
			lItemCount++;
			m_vVisibles.push_back(i);
		};
	};

	if (m_lSortColumn>0)
	{
		// Sort the list of visible items
		g_pCompareList = this;
		std::sort(m_vVisibles.begin(), m_vVisibles.end(), CompareVisibleItems);
		g_pCompareList = NULL;
	};

	SetItemCount(lItemCount);

	// Clear selection and put selection back
	LONG				lMustBeVisible = -1;

	for (i = 0;i<GetItemCount();i++)
	{
		lIndice = m_vVisibles[i];
		BOOL			bFound = FALSE;

		for (LONG j = 0;j<vSelected.size() && !bFound;j++)
		{
			if (m_vFiles[lIndice].m_strFileName == vSelected[j])
				bFound = TRUE;
		};

		SetItemState(i, bFound ? LVIS_SELECTED : 0, LVIS_SELECTED);
		if (m_vFiles[lIndice].m_strFileName == strFocus)
		{
			lMustBeVisible = i;
			SetItemState(i, LVIS_FOCUSED, LVIS_FOCUSED);
		};
	};

	if (lMustBeVisible>=0)
		EnsureVisible(lMustBeVisible, FALSE);

	if (!m_bRefreshNeeded)
	{
		m_bRefreshNeeded = TRUE;
		PostMessage(WM_LISTITEMCHANGED);
	};
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::AddFileToList(LPCTSTR szFile, DWORD dwGroupID, GUID dwJobID, PICTURETYPE PictureType, BOOL bCheck, int nItem)
{
	CString				strFile = szFile;
	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szFileName[1+_MAX_FNAME];
	TCHAR				szExt[1+_MAX_EXT];
	LONG				lIndice = -1;
	BOOL				bFound = FALSE;
	BOOL				bAdd = FALSE,
						bUpdate = FALSE;

	// First check that the file is not already in the list
	for (LONG i = 0;i<m_vFiles.size() && !bFound;i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			!strFile.CompareNoCase(m_vFiles[i].m_strFileName))
		{
			bFound = TRUE;
			lIndice = i;
		};
	};

	if (bFound)
		bUpdate = (nItem != -1);
	else
		bAdd = TRUE;

	if (bAdd || bUpdate)
	{
		CListBitmap			lb;

		lb.m_dwGroupID = dwGroupID;
		lb.m_JobID   = dwJobID;

		if (lb.InitFromFile(szFile, PictureType))
		{
			if (PictureType == PICTURETYPE_DARKFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_DARK);
				lb.m_bUseAsStarting = FALSE;
			}
			else if (PictureType == PICTURETYPE_DARKFLATFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_DARKFLAT);
				lb.m_bUseAsStarting = FALSE;
			}
			else if (PictureType == PICTURETYPE_FLATFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_FLAT);
				lb.m_bUseAsStarting = FALSE;
			}
			else if (PictureType == PICTURETYPE_OFFSETFRAME)
			{
				lb.m_strType.LoadString(IDS_TYPE_OFFSET);
				lb.m_bUseAsStarting = FALSE;
			}
			else
			{
				lb.m_strType.LoadString(IDS_TYPE_LIGHT);
			};
			_tsplitpath(strFile, szDrive, szDir, szFileName, szExt);

			if (bAdd)
			{
				//nItem = InsertItem(GetItemCount(), "", nImage);
				if (bCheck)
					lb.m_bChecked = TRUE;
					//SetCheck(nItem, TRUE);
			}
			else
			{
				// SetItem(nItem, 0, LVIF_IMAGE, "", nImage, 0, 0, 0);
			};

			lb.m_strPath     = szDrive;
			lb.m_strPath    += szDir;

			CString				strFileName;
			strFileName = szFileName;
			strFileName += szExt;

			lb.m_strFile = strFileName;

			if (lb.m_PictureType != PICTURETYPE_LIGHTFRAME)
			{
			}
			else
			{
				CLightFrameInfo			bmpInfo;

				bmpInfo.SetBitmap(szFile, FALSE);
				if (bmpInfo.m_bInfoOk)
				{
					lb.m_bRegistered = TRUE;
					lb.m_fOverallQuality = bmpInfo.m_fOverallQuality;
					lb.m_fFWHM			 = bmpInfo.m_fFWHM;
					lb.m_lNrStars		 = (LONG)bmpInfo.m_vStars.size();
					lb.m_bComet			 = bmpInfo.m_bComet;
					lb.m_SkyBackground	 = bmpInfo.m_SkyBackground;
					lb.m_bUseAsStarting	 = (PictureType == PICTURETYPE_REFLIGHTFRAME);
				}
				else
				{
				};
			};

			{
				CString				strSizes;
				CString				strDepth;
				CString				strInfos;

				strSizes.Format(_T("%ld x %ld"), lb.m_lWidth, lb.m_lHeight);
				lb.m_strSizes = strSizes;

				if (lb.m_lNrChannels == 3)
					strDepth.Format(IDS_FORMAT_RGB, lb.m_lBitPerChannels);
				else
					strDepth.Format(IDS_FORMAT_GRAY, lb.m_lBitPerChannels);
				lb.m_strDepth = strDepth;

				CString				strText;

				if (!bAdd)
				{
					// Keep ISO Speed and Exposure time as set by the user
					lb.m_lISOSpeed = m_vFiles[lIndice].m_lISOSpeed;
					lb.m_fExposure = m_vFiles[lIndice].m_fExposure;
				};

				if (lb.IsMasterFrame())
				{
					if (PictureType == PICTURETYPE_DARKFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTERDARK);
					else if (PictureType == PICTURETYPE_DARKFLATFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTERDARKFLAT);
					else if (PictureType == PICTURETYPE_FLATFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTERFLAT);
					else if (PictureType == PICTURETYPE_OFFSETFRAME)
						lb.m_strType.LoadString(IDS_TYPE_MASTEROFFSET);
				};

				if (lb.GetCFAType() != CFATYPE_NONE)
					strText.LoadString(IDS_YES);
				else
					strText.LoadString(IDS_NO);

				lb.m_strCFA = strText;
			};

			if (bAdd)
			{
				m_vFiles.push_back(lb);
			}
			else
				m_vFiles[lIndice] = lb;
		};
	};

	m_bDirty = TRUE;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::ChangePictureType(int nItem, PICTURETYPE PictureType)
{
	LONG			lIndice;
	CString			strFileName;

	lIndice = m_vVisibles[nItem];
	strFileName = m_vFiles[lIndice].m_strFileName;

	AddFileToList(strFileName, m_dwCurrentGroupID, m_dwCurrentJobID, PictureType, FALSE, nItem);
};

/* ------------------------------------------------------------------- */

LRESULT CPictureListCtrl::OnListItemChanged(WPARAM, LPARAM)
{
	if (m_bRefreshNeeded)
	{
		GetParent()->SendMessage(WM_CHECKITEM);
		m_bRefreshNeeded = FALSE;
	};

	return 0;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (pNMListView->iSubItem)
	{
		SortList(pNMListView->iSubItem);
	};

	*pResult = 0;
}

/* ------------------------------------------------------------------- */

void CPictureListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (LVIF_STATE == pNMListView->uChanged)
	{
		GetParent()->SendMessage(WM_CHECKITEM);

		if ((pNMListView->uNewState & LVIS_SELECTED) &&
			!(pNMListView->uOldState & LVIS_SELECTED))
		{
			// Change of selected state - Click equivalent
			GetParent()->SendMessage(WM_SELECTITEM);
		};
	};

	*pResult = 0;
}

/* ------------------------------------------------------------------- */

void CPictureListCtrl::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_KEYDOWN* pLVKeyDown = (LV_KEYDOWN*)pNMHDR;

    //If user press space, toggle flag on selected item
    if( pLVKeyDown->wVKey == VK_SPACE )
    {
        //Toggle if some item is selected
        if(GetSelectionMark() != -1)
		{
            ToggleCheckBox( GetSelectionMark() );
			m_bDirty = TRUE;
		};
    }

    *pResult = 0;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::OnLButtonDown( UINT nFlags, CPoint pt)
{
	BOOL				bPass = TRUE;
	CListCtrlEx::OnLButtonDown(nFlags, pt);

    LVHITTESTINFO hitinfo;
    hitinfo.pt = pt;
	int		item = HitTest(&hitinfo);

    if(item != -1)
    {
        //We hit one item... did we hit state image (check box)?
        //This test only works if we are in list or report mode.
		if( !hitinfo.iSubItem && hitinfo.flags & LVHT_ONITEMSTATEICON)
        {
			bPass = FALSE;
            ToggleCheckBox(item);
			m_bDirty = TRUE;
        }
    }
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;

	int iItemIndex= pItem->iItem;

	if(pItem->mask & LVIF_TEXT)
	{
		int					lIndice = m_vVisibles[iItemIndex];
		CString				strValue;

		switch (pItem->iSubItem)
		{
		case COLUMN_PATH :
			strValue = m_vFiles[lIndice].m_strPath;
			break;
		case COLUMN_FILE :
			strValue = m_vFiles[lIndice].m_strFile;
			break;
		case COLUMN_FTYPE :
			strValue = m_vFiles[lIndice].m_strType;
			break;
		case COLUMN_SCORE :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = "N/A";
			else
			{
				if (m_vFiles[lIndice].m_bRegistered)
				{
					if (m_vFiles[lIndice].m_bUseAsStarting)
						strValue.Format(_T("(*) %.2f"), m_vFiles[lIndice].m_fOverallQuality);
					else
						strValue.Format(_T("%.2f"), m_vFiles[lIndice].m_fOverallQuality);
				}
				else
					strValue = _T("NC");
			};
			break;
		case COLUMN_FWHM :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = _T("N/A");
			else
			{
				if (m_vFiles[lIndice].m_bRegistered)
					strValue.Format(_T("%.2f"), m_vFiles[lIndice].m_fFWHM);
				else
					strValue = "NC";
			};
			break;
		case COLUMN_STARS :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = "N/A";
			else
			{
				if (m_vFiles[lIndice].m_bRegistered)
				{
					if (m_vFiles[lIndice].m_bComet)
						strValue.Format(_T("%ld+(C)"), m_vFiles[lIndice].m_lNrStars);
					else
						strValue.Format(_T("%ld"), m_vFiles[lIndice].m_lNrStars);
				}
				else
					strValue = _T("NC");
			};
			break;
		case COLUMN_DX :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = _T("N/A");
			else
			{
				if (m_vFiles[lIndice].m_bDeltaComputed)
					strValue.Format(_T("%.2f"), m_vFiles[lIndice].m_dX);
				else
					strValue = _T("NC");
			};
			break;
		case COLUMN_DY :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = _T("N/A");
			else
			{
				if (m_vFiles[lIndice].m_bDeltaComputed)
					strValue.Format(_T("%.2f"), m_vFiles[lIndice].m_dY);
				else
					strValue = _T("NC");
			};
			break;
		case COLUMN_ANGLE :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = _T("N/A");
			else
			{
				if (m_vFiles[lIndice].m_bDeltaComputed)
					strValue.Format(_T("%.2f °"), m_vFiles[lIndice].m_fAngle*180.0/M_PI);
				else
					strValue = _T("NC");
			};
			break;
		case COLUMN_SKYBACKGROUND :
			if (m_vFiles[lIndice].m_PictureType != PICTURETYPE_LIGHTFRAME)
				strValue = _T("N/A");
			else
			{
				if (m_vFiles[lIndice].m_SkyBackground.m_fLight)
					strValue.Format(_T("%.2f %%"), m_vFiles[lIndice].m_SkyBackground.m_fLight*100.0);
				else
					strValue = _T("NC");
			};
			break;
		case COLUMN_FILETIME :
			strValue = m_vFiles[lIndice].m_strDateTime;
			break;
		case COLUMN_SIZES :
			strValue = m_vFiles[lIndice].m_strSizes;
			break;
		case COLUMN_CFA	:
			strValue.LoadString((m_vFiles[lIndice].GetCFAType() != CFATYPE_NONE) ? IDS_YES : IDS_NO);
			break;
		case COLUMN_DEPTH :
			strValue = m_vFiles[lIndice].m_strDepth;
			break;
		case COLUMN_INFO :
			strValue = m_vFiles[lIndice].m_strInfos;
			break;
		case COLUMN_ISO	:
			ISOToString(m_vFiles[lIndice].m_lISOSpeed, strValue);
			break;
		case COLUMN_EXPOSURE :
			ExposureToString(m_vFiles[lIndice].m_fExposure, strValue);
			break;
		};

		if (strValue.GetLength())
			lstrcpyn(pItem->pszText, (LPCTSTR)strValue, pItem->cchTextMax);
	}
	if (pItem->mask & LVIF_IMAGE)
	{
		int					lIndice = m_vVisibles[iItemIndex];
		int					nImage = 1;

		switch (m_vFiles[lIndice].m_PictureType)
		{
		case PICTURETYPE_LIGHTFRAME :
			nImage = 1;
			break;
		case PICTURETYPE_DARKFRAME :
		case PICTURETYPE_DARKFLATFRAME :
			nImage = 2;
			break;
		case PICTURETYPE_FLATFRAME :
			nImage = 3;
			break;
		case PICTURETYPE_OFFSETFRAME :
			nImage = 4;
			break;
		};

		if (IsCYMGType(m_vFiles[lIndice].GetCFAType()))
			nImage += 5;
		else if (m_vFiles[lIndice].GetCFAType() != CFATYPE_NONE)
			nImage += 10;
		else if (m_vFiles[lIndice].m_lNrChannels==3)
			nImage += 15;
		
		pItem->iImage = nImage;

        //To enable check box, we have to enable state mask...
        pItem->mask |= LVIF_STATE;
        pItem->stateMask = LVIS_STATEIMAGEMASK;

		if(m_vFiles[lIndice].m_bChecked)
        {
            //Turn check box on
            pItem->state = INDEXTOSTATEIMAGEMASK(2);
        }
        else
        {
            //Turn check box off
            pItem->state = INDEXTOSTATEIMAGEMASK(1);
        }
	};

	*pResult = 0;
}

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::GetTransformation(LPCTSTR szFile, CBilinearParameters & Transformation, VOTINGPAIRVECTOR & vVotedPairs)
{
	BOOL				bResult = FALSE;

	for (LONG i = 0;i<m_vFiles.size() && !bResult;i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsLightFrame() &&
			!m_vFiles[i].m_strFileName.CompareNoCase(szFile))
		{
			Transformation = m_vFiles[i].m_Transformation;
			vVotedPairs    = m_vFiles[i].m_vVotedPairs;
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::FillJobs(CAllStackingJobs & jobs)
{
	for (LONG i = 0;i<m_Jobs.m_vJobs.size();i++)
	{
		CAllStackingTasks 		tasks;

		FillTasks(tasks, m_Jobs.m_vJobs[i].m_ID);

		jobs.m_vStackingTasks.push_back(tasks);
	};
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckBest(double fPercent)
{
	LONG							lLast;
	std::vector<CScoredLightFrame>	vLightFrames;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsLightFrame())
			vLightFrames.push_back(CScoredLightFrame(i, m_vFiles[i].m_fOverallQuality));
	};

	lLast = (LONG)(fPercent * vLightFrames.size()/100.0);
	std::sort(vLightFrames.begin(), vLightFrames.end());

	for (LONG i = 0;i<vLightFrames.size();i++)
		m_vFiles[vLightFrames[i].m_dwIndice].m_bChecked = (i<=lLast);

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::UnCheckNonStackable()
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].m_bChecked &&
			m_vFiles[i].IsLightFrame())
		{
			if (!m_vFiles[i].IsDeltaComputed())
				m_vFiles[i].m_bChecked = FALSE;
		};
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CopyToClipboard()
{
	CString			strClipboard;
	LONG			i, j;

	for (j = 0;j<GetColumnCount();j++)
	{
		HDITEM		hdrItem;
		TCHAR		szItem[200];
		CString		strItem;

		if (j)
			strClipboard += "\t";
		hdrItem.mask = HDI_TEXT;
		hdrItem.cchTextMax = sizeof(szItem)/sizeof(TCHAR);
		hdrItem.pszText    = szItem;
		GetHeaderCtrl()->GetItem(j, &hdrItem);
		strItem = szItem;
		strClipboard += strItem;
	};
	for (i = 0;i<GetItemCount();i++)
	{
		strClipboard += "\n";
		for (j = 0;j<GetColumnCount();j++)
		{
			CString			strSubItem;

			strSubItem = GetItemText(i, j);
			if (j)
				strClipboard+="\t";
			strClipboard += strSubItem;
		};
	};

	if (strClipboard.GetLength())
	{
		::OpenClipboard(NULL);
		
		HGLOBAL				hMem;
		LPVOID				lpMem;

		hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, strClipboard.GetLength()+1);
		lpMem = (LPVOID)GlobalLock(hMem);
		memcpy(lpMem, (LPCTSTR)strClipboard, strClipboard.GetLength()+1);
		GlobalUnlock(hMem);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	};
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::OnRButtonDown( UINT nFlags, CPoint pt)
{
	CListCtrlEx::OnRButtonDown(nFlags, pt);

	CMenu			menu;
	CMenu *			popup;
	int				nResult;
	CFileProperty	dlgProperties;
	POSITION		pos;
	BOOL			bEnableUseAsStarting = FALSE;
	BOOL			bStartingChecked = FALSE;

	ClientToScreen(&pt);
	menu.LoadMenu(IDR_LISTCONTEXT);
	popup = menu.GetSubMenu(0);

	pos = GetFirstSelectedItemPosition();
	if (pos)
	{
		int			nItem;
		LONG		lIndice;

		nItem = GetNextSelectedItem(pos);
		lIndice = m_vVisibles[nItem];

		if (!pos)
		{
			if (m_vFiles[lIndice].IsLightFrame())
			{
				bEnableUseAsStarting = TRUE;
				bStartingChecked     = m_vFiles[lIndice].IsUseAsStarting();
			};
		};
	};

	popup->EnableMenuItem(IDM_USEASSTARTING, MF_BYCOMMAND | (bEnableUseAsStarting ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
	popup->CheckMenuItem(IDM_USEASSTARTING, MF_BYCOMMAND | (bStartingChecked ? MF_CHECKED : MF_UNCHECKED));
	nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);

	std::vector<int>	vToRemove;
	std::vector<int>	vToRefresh;
	BOOL				bContinue = TRUE;

	if (nResult == IDM_ERASEFROMDISK)
	{
		int			nErase;
		CString		strText;
		LONG		lNrFiles = 0;

		pos = GetFirstSelectedItemPosition();
		while (pos)
		{
			lNrFiles++;
			GetNextSelectedItem(pos);
		};

		strText.Format(IDS_WARNING_ERASEFILES, lNrFiles);

		nErase = AfxMessageBox(strText, MB_YESNO | MB_DEFBUTTON2 | MB_ICONSTOP);
		bContinue = (nErase == IDYES);
		m_bDirty = TRUE;
	};

	if (nResult == IDM_COPYTOCLIPBOARD)
	{
		switch (nResult)
		{
			case IDM_COPYTOCLIPBOARD :
				CopyToClipboard();
				break;
		};
	}
	else if (bContinue)
	{
		pos = GetFirstSelectedItemPosition();
		while (pos)
		{
			int			nItem;
			LONG		lIndice;
			
			nItem = GetNextSelectedItem(pos);
			lIndice = m_vVisibles[nItem];

			switch (nResult)
			{
			case IDM_USEASSTARTING :
				SetUseAsStarting(nItem, !bStartingChecked);
				m_bDirty = TRUE;
				break;
			case IDM_CHECK :
				m_vFiles[lIndice].m_bChecked = TRUE;
				m_bDirty = TRUE;
				break;
			case IDM_UNCHECK :
				m_vFiles[lIndice].m_bChecked = FALSE;
				m_bDirty = TRUE;
				break;
			case IDM_CHANGETOFRAME :
				ChangePictureType(nItem, PICTURETYPE_LIGHTFRAME);
				m_bDirty = TRUE;
				break;
			case IDM_CHANGETODARK :
				ChangePictureType(nItem, PICTURETYPE_DARKFRAME);
				m_bDirty = TRUE;
				break;
			case IDM_CHANGETODARKFLAT :
				ChangePictureType(nItem, PICTURETYPE_DARKFLATFRAME);
				m_bDirty = TRUE;
				break;
			case IDM_CHANGETOOFFSET :
				ChangePictureType(nItem, PICTURETYPE_OFFSETFRAME);
				m_bDirty = TRUE;
				break;
			case IDM_CHANGETOFLAT :
				ChangePictureType(nItem, PICTURETYPE_FLATFRAME);
				m_bDirty = TRUE;
				break;
			case IDM_ERASEFROMDISK :
				m_vFiles[lIndice].m_bRemoved = TRUE;
				m_vFiles[lIndice].EraseFile();
				m_bDirty = TRUE;
				break;
			case IDM_REMOVEFROMLIST :
				m_vFiles[lIndice].m_bRemoved = TRUE;
				m_bDirty = TRUE;
				break;
			case IDM_PROPERTIES :
				dlgProperties.AddBitmap(&(m_vFiles[lIndice]));
				break;
			};
		};
	};

	if (nResult == IDM_PROPERTIES)
	{
		if (dlgProperties.DoModal() == IDOK)
			m_bDirty = TRUE;
	};
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::SortListQualityDesc()
{
	m_bAscending = FALSE;
	m_lSortColumn = COLUMN_SCORE;		/* Score column */

	SortItems(PictureListCompareFunc, (DWORD_PTR)this);
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::GetSelectedFileName(CString & strFileName)
{
	BOOL			bResult = FALSE;
	POSITION		pos;

	if (GetSelectedCount() == 1)
	{
		pos = GetFirstSelectedItemPosition();
		if (pos)
		{
			int			nItem;
			LONG		lIndice;
			
			nItem = GetNextSelectedItem(pos);
			lIndice = m_vVisibles[nItem];

			strFileName = m_vFiles[lIndice].m_strFileName;

			bResult = TRUE;
		};
	};
	
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::GetItemISOSpeedAndExposure(int nItem, LONG & lISOSpeed, double & fExposure)
{
	BOOL			bResult = TRUE;
	LONG			lItem;

	lItem = m_vVisibles[nItem];

	lISOSpeed = m_vFiles[lItem].m_lISOSpeed;
	fExposure = m_vFiles[lItem].m_fExposure;
	
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::GetItemFileName(int nItem, CString & strFileName)
{
	BOOL			bResult = TRUE;
	LONG			lItem;

	lItem = m_vVisibles[nItem];

	strFileName = m_vFiles[lItem].m_strFileName;
	
	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::GetFirstCheckedLightFrame(CString & strFileName)
{
	BOOL			bResult = FALSE;

	for (LONG i = 0;i<m_vFiles.size() && !bResult;i++)
	{
		if (!m_vFiles[i].m_bRemoved && 
			m_vFiles[i].IsLightFrame() &&
			m_vFiles[i].m_bChecked)
		{
			strFileName = m_vFiles[i].m_strFileName;
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::GetPictureSizes(int nItem, LONG & lWidth, LONG & lHeight, LONG & lNrChannels)
{
	BOOL			bResult = TRUE;
	LONG			lItem;

	lItem = m_vVisibles[nItem];

	lWidth = m_vFiles[lItem].m_lWidth;
	lHeight= m_vFiles[lItem].m_lHeight;
	lNrChannels = m_vFiles[lItem].m_lNrChannels;

	return bResult;
};

/* ------------------------------------------------------------------- */

int	CPictureListCtrl::FindIndice(LPCTSTR szFileName)
{
	int				nResult = -1;
	CString			strFileName = szFileName;

	for (LONG i = 0;(i<m_vFiles.size()) && (nResult == -1);i++)
	{
		if (!m_vFiles[i].m_bRemoved && 
			!strFileName.CompareNoCase(m_vFiles[i].m_strFileName))
			nResult = i;
	};

	return nResult;
};

/* ------------------------------------------------------------------- */

const CListBitmap & CPictureListCtrl::GetItem(int nIndice)
{
	LONG			lItem;

	lItem = m_vVisibles[nIndice];

	return m_vFiles[lItem];
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::SetUseAsStarting(int nItem, BOOL bUse)
{
	m_vFiles[m_vVisibles[nItem]].m_bUseAsStarting = bUse;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (i != m_vVisibles[nItem])
		{
			if (m_vFiles[i].IsLightFrame() && m_vFiles[i].m_bUseAsStarting)
				m_vFiles[i].m_bUseAsStarting = FALSE;
		};
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckAbove(double fThreshold)
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && m_vFiles[i].IsLightFrame())
			m_vFiles[i].m_bChecked = (m_vFiles[i].m_fOverallQuality >= fThreshold);
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckAll(BOOL bCheck)
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved)
			m_vFiles[i].m_bChecked = bCheck;
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckAllDarks(BOOL bCheck)
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && m_vFiles[i].IsDarkFrame())
			m_vFiles[i].m_bChecked = bCheck;
	};
	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckAllFlats(BOOL bCheck)
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && m_vFiles[i].IsFlatFrame())
			m_vFiles[i].m_bChecked = bCheck;
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckAllOffsets(BOOL bCheck)
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && m_vFiles[i].IsOffsetFrame())
			m_vFiles[i].m_bChecked = bCheck;
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckAllLights(BOOL bCheck)
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && m_vFiles[i].IsLightFrame())
			m_vFiles[i].m_bChecked = bCheck;
	};

	m_bDirty = TRUE;
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::CheckImage(LPCTSTR szImage, BOOL bCheck)
{
	LONG		lIndice = FindIndice(szImage);

	if (lIndice>=0)
	{
		m_vFiles[lIndice].m_bChecked = bCheck;
		RefreshList();
	};
	m_bDirty = TRUE;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::ClearOffset(LPCTSTR szFileName)
{
	LONG		lIndice = FindIndice(szFileName);

	if (lIndice >= 0)
	{
		m_vFiles[lIndice].m_bDeltaComputed = FALSE;
		RefreshList();
	};
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::UpdateOffset(LPCTSTR szFileName, double fdX, double fdY, double fAngle, const CBilinearParameters & Transformation, const VOTINGPAIRVECTOR & vVotedPairs)
{
	LONG		lIndice = FindIndice(szFileName);

	if (lIndice >= 0)
	{
		m_vFiles[lIndice].m_bDeltaComputed = TRUE;
		m_vFiles[lIndice].m_dX = fdX;
		m_vFiles[lIndice].m_dY = fdY;
		m_vFiles[lIndice].m_fAngle = fAngle;
		m_vFiles[lIndice].m_Transformation = Transformation;
		m_vFiles[lIndice].m_vVotedPairs = vVotedPairs;

		RefreshList();
	};
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::ClearOffsets()
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (m_vFiles[i].IsLightFrame())
			m_vFiles[i].m_bDeltaComputed = FALSE;
	};
	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::UpdateItemScores(LPCTSTR szFileName)
{
	LONG		lIndice = FindIndice(szFileName);

	if (lIndice >= 0)
	{
		if (!m_vFiles[lIndice].m_bRemoved &&
			m_vFiles[lIndice].IsLightFrame())
		{
			CLightFrameInfo		bmpInfo;

			bmpInfo.SetBitmap(m_vFiles[lIndice].m_strFileName, FALSE, FALSE);

			// Update list info
			if (bmpInfo.m_bInfoOk)
			{
				m_vFiles[lIndice].m_bRegistered = TRUE;
				m_vFiles[lIndice].m_fOverallQuality	= bmpInfo.m_fOverallQuality;
				m_vFiles[lIndice].m_fFWHM			= bmpInfo.m_fFWHM;
				m_vFiles[lIndice].m_lNrStars		= (LONG)bmpInfo.m_vStars.size();
				m_vFiles[lIndice].m_bComet			= bmpInfo.m_bComet;
				m_vFiles[lIndice].m_SkyBackground	= bmpInfo.m_SkyBackground;
			}
			else
			{
				m_vFiles[lIndice].m_bRegistered = FALSE;
			};
		};
	};

	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::UpdateCheckedItemScores()
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].m_bChecked &&
			m_vFiles[i].IsLightFrame())
		{
			CLightFrameInfo		bmpInfo;

			bmpInfo.SetBitmap(m_vFiles[i].m_strFileName, FALSE, FALSE);

			// Update list info
			if (bmpInfo.m_bInfoOk)
			{
				m_vFiles[i].m_bRegistered = TRUE;
				m_vFiles[i].m_fOverallQuality	= bmpInfo.m_fOverallQuality;
				m_vFiles[i].m_fFWHM				= bmpInfo.m_fFWHM;
				m_vFiles[i].m_lNrStars			= (LONG)bmpInfo.m_vStars.size();
				m_vFiles[i].m_bComet			= bmpInfo.m_bComet;
				m_vFiles[i].m_SkyBackground		= bmpInfo.m_SkyBackground;
			}
			else
			{
				m_vFiles[i].m_bRegistered = FALSE;
			};
		};
	};

	RefreshList();
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::BlankCheckedItemScores()
{
	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].m_bChecked &&
			m_vFiles[i].IsLightFrame())
			m_vFiles[i].m_bRegistered = FALSE;
	};

	RefreshList();
};

/* ------------------------------------------------------------------- */

LONG CPictureListCtrl::GetNrCheckedFrames(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsLightFrame() && 
			m_vFiles[i].m_bChecked)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};
	};

	return lResult;
};

/* ------------------------------------------------------------------- */

LONG CPictureListCtrl::GetNrCheckedDarks(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && 
			m_vFiles[i].IsDarkFrame() && 
			m_vFiles[i].m_bChecked)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};
	};

	return lResult;
};

/* ------------------------------------------------------------------- */

LONG CPictureListCtrl::GetNrCheckedFlats(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsFlatFrame() && 
			m_vFiles[i].m_bChecked)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};

	};

	return lResult;
};

/* ------------------------------------------------------------------- */

LONG CPictureListCtrl::GetNrCheckedDarkFlats(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved &&
			m_vFiles[i].IsDarkFlatFrame() && 
			m_vFiles[i].m_bChecked)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};

	};

	return lResult;
};

/* ------------------------------------------------------------------- */

LONG CPictureListCtrl::GetNrCheckedOffsets(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved && 
			m_vFiles[i].IsOffsetFrame() && 
			m_vFiles[i].m_bChecked)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};
	};

	return lResult;
};

/* ------------------------------------------------------------------- */

LONG	CPictureListCtrl::GetNrFrames(LONG lGroupID)
{
	LONG				lResult = 0;

	for (LONG i = 0;i<m_vFiles.size();i++)
	{
		if (!m_vFiles[i].m_bRemoved)
		{
			if ((lGroupID < 0) || (lGroupID == m_vFiles[i].m_dwGroupID))
				lResult++;
		};
	};

	return lResult;
};

/* ------------------------------------------------------------------- */

BOOL CPictureListCtrl::AreCheckedPictureCompatible()
{
	BOOL				bResult = TRUE;
	BOOL				bFirst = TRUE;
	CListBitmap			lb;
	LONG				lNrDarks = 0;
	LONG				lNrDarkFlats = 0;
	LONG				lNrFlats = 0;
	LONG				lNrOffsets = 0;
	BOOL				bMasterDark = FALSE;
	BOOL				bMasterFlat = FALSE;
	BOOL				bMasterDarkFlat = FALSE;
	BOOL				bMasterOffset = FALSE;

	for (LONG i = 0;(i<m_vFiles.size()) && bResult;i++)
	{
		if (!m_vFiles[i].m_bRemoved && m_vFiles[i].m_bChecked)
		{
/*			if (m_vFiles[lItem].IsDarkFrame())
			{
				lNrDarks++;
				bMasterDark = bMasterDark || m_vFiles[lItem].IsMasterFrame();
			}
			else if (m_vFiles[lItem].IsFlatFrame())
			{
				lNrFlats++;
				bMasterFlat = bMasterFlat || m_vFiles[lItem].IsMasterFrame();
			}
			else if (m_vFiles[lItem].IsOffsetFrame())
			{
				lNrOffsets++;
				bMasterOffset = bMasterOffset || m_vFiles[lItem].IsMasterFrame();
			}*/
			if (bFirst)
			{
				lb = m_vFiles[i];
				bFirst = FALSE;
			}
			else
				bResult = lb.IsCompatible(m_vFiles[i]);
		};
	};

	if (bResult)
	{
		if (bMasterDark && lNrDarks>1)
			bResult = FALSE;
		if (bMasterDarkFlat && lNrDarkFlats>1)
			bResult = FALSE;
		if (bMasterFlat && lNrFlats>1)
			bResult = FALSE;
		if (bMasterOffset && lNrOffsets>1)
			bResult = FALSE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::SaveList(CMRUList & MRUList, CString & strFileList)
{
	CRegistry					reg;
	CString						strBaseDirectory;
	DWORD						dwFilterIndex = 0;
	CString						strBaseExtension;

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListFolder"), strBaseDirectory);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListIndex"), dwFilterIndex);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListExtension"), strBaseExtension);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".dssfilelist");

	CFileDialog					dlgSave(FALSE, 
								strBaseExtension,
								(LPCTSTR)strFileList,
								OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
								OUTPUTLIST_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgSave.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgSave.m_ofn.nFilterIndex = dwFilterIndex;

	if (dlgSave.DoModal() == IDOK)
	{
		POSITION		pos;

		pos = dlgSave.GetStartPosition();
		if (pos)
		{
			CString		strFile;

			strFile = dlgSave.GetNextPathName(pos);

			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;

			dwFilterIndex = dlgSave.m_ofn.nFilterIndex;
			reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("ListFolder"), strBaseDirectory);
			reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("ListIndex"), dwFilterIndex);
			reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListExtension"), strBaseExtension);

			SaveListToFile(strFile);
			strFileList = strFile;
			MRUList.Add((LPCTSTR)strFile);
		};
	};
};

/* ------------------------------------------------------------------- */

void CPictureListCtrl::LoadList(CMRUList & MRUList, CString & strFileList)
{
	CRegistry					reg;
	CString						strBaseDirectory;
	DWORD						dwFilterIndex = 0;
	CString						strBaseExtension;

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListFolder"), strBaseDirectory);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListIndex"), dwFilterIndex);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListExtension"), strBaseExtension);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".dssfilelist");

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
								OUTPUTLIST_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;

	TCHAR				szBigBuffer[20000] = _T("");

	dlgOpen.m_ofn.lpstrFile = szBigBuffer;
	dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

	if (dlgOpen.DoModal() == IDOK)
	{
		POSITION		pos;
		CString			strDrive;
		CString			strDir;

		BeginWaitCursor();
		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			strFile = dlgOpen.GetNextPathName(pos);

			LoadFilesFromList(strFile);
			strFileList = strFile;
			MRUList.Add((LPCTSTR)strFile);

			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;

		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("ListFolder"), strBaseDirectory);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("ListIndex"), dwFilterIndex);
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("ListExtension"), strBaseExtension);
	};	
}

/* ------------------------------------------------------------------- */
