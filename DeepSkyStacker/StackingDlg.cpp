// StackingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "StackingDlg.h"
#include "DeepStackerDlg.h"
#include "DeepStack.h"
#include "ProgressDlg.h"
#include "CheckAbove.h"
#include "Registry.h"
#include "RegisterSettings.h"
#include "StackRecap.h"
#include "TIFFUtil.h"
#include "RegisterEngine.h"
#include "StackingEngine.h"
#include "DropFilesDlg.h"
#include "SaveEditChanges.h"
#include "AskRegistering.h"
#include "BatchStacking.h"
#include "DSSVersion.h"

#define _USE_MATH_DEFINES
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const		DWORD					IDC_EDIT_SELECT = 1;
const		DWORD					IDC_EDIT_STAR   = 2;
const		DWORD					IDC_EDIT_COMET  = 3;
const		DWORD					IDC_EDIT_SAVE   = 4;

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackingDlg dialog


CStackingDlg::CStackingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStackingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStackingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_MRUList.InitFromRegistry();
}


/* ------------------------------------------------------------------- */

void CStackingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStackingDlg)
	DDX_Control(pDX, IDC_PICTURES, m_Pictures);
	DDX_Control(pDX, IDC_PICTURE, m_PictureStatic);
	DDX_Control(pDX, IDC_INFOS, m_Infos);
	DDX_Control(pDX, IDC_LISTINFO, m_ListInfo);
	DDX_Control(pDX, IDC_GAMMA, m_Gamma);
	DDX_Control(pDX, IDC_GROUPTAB, m_GroupTab);
	DDX_Control(pDX, IDC_JOBTAB, m_JobTab);
	DDX_Control(pDX, IDC_SHOWHIDEJOBS, m_ShowHideJobs);
	DDX_Control(pDX, IDC_4CORNERS, m_4Corners);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CStackingDlg, CDialog)
	//{{AFX_MSG_MAP(CStackingDlg)
	ON_NOTIFY(NM_CLICK, IDC_PICTURES, OnClickPictures)
	ON_NOTIFY(NM_NOTIFYMODECHANGE, IDC_PICTURE, OnPictureChange)
	ON_MESSAGE(WM_CHECKITEM, OnCheckItem)
	ON_MESSAGE(WM_SELECTITEM, OnSelectItem)
	ON_WM_SIZE()
	ON_NOTIFY(GC_PEGMOVE, IDC_GAMMA, OnChangeGamma)
	ON_NOTIFY(GC_PEGMOVED, IDC_GAMMA, OnChangeGamma)
	ON_MESSAGE(WM_BACKGROUNDIMAGELOADED, OnBackgroundImageLoaded)
	ON_NOTIFY(CTCN_SELCHANGE, IDC_GROUPTAB, OnSelChangeGroup)	
	ON_NOTIFY(CTCN_SELCHANGE, IDC_JOBTAB, OnSelChangeJob)	
	ON_NOTIFY(NM_LINKCLICK, IDC_SHOWHIDEJOBS, OnShowHideJobs)
	ON_NOTIFY(SPN_SIZED, IDC_SPLITTER, OnSplitter)
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_4CORNERS, &CStackingDlg::OnBnClicked4corners)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackingDlg message handlers

BOOL CStackingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ControlPos.SetParent(this);

	m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	m_ListInfo.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	m_Picture.CreateFromStatic(&m_PictureStatic);

	{
		CRect				rc;

		GetDlgItem(IDC_SPLITTER)->GetWindowRect(rc);
		ScreenToClient(rc);
		m_Splitter.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER);
		UpdateSplitter();
	};

	m_ControlPos.AddControl(IDC_INFOS, CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_PICTURES, CP_MOVE_VERTICAL | CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_LISTINFO, CP_MOVE_VERTICAL | CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_PICTURE, CP_RESIZE_VERTICAL | CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_GAMMA, CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_4CORNERS, CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_GROUPTAB, CP_MOVE_VERTICAL | CP_RESIZE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_SHOWHIDEJOBS, CP_MOVE_VERTICAL | CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(IDC_JOBTAB, CP_MOVE_VERTICAL | CP_MOVE_HORIZONTAL);
	m_ControlPos.AddControl(&m_Splitter, CP_MOVE_VERTICAL | CP_RESIZE_HORIZONTAL);

	m_Pictures.Initialize();
	m_Picture.SetBltMode(CWndImage::bltFitXY);
	m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);
	CString				strTooltip;

	strTooltip.LoadString(IDS_TOOLTIP_SELECTRECT);
	m_ButtonToolbar.AddCheck(IDC_EDIT_SELECT,	MBI(SELECT), IDB_BUTTONBASE_MASK, strTooltip);
	strTooltip.LoadString(IDS_TOOLTIP_STAR);
	m_ButtonToolbar.AddCheck(IDC_EDIT_STAR,	MBI(STAR), IDB_BUTTONBASE_MASK, strTooltip);
	strTooltip.LoadString(IDS_TOOLTIP_COMET);
	m_ButtonToolbar.AddCheck(IDC_EDIT_COMET,	MBI(COMET), IDB_BUTTONBASE_MASK, strTooltip);
	strTooltip.LoadString(IDS_TOOLTIP_SAVE);
	m_ButtonToolbar.AddButton(IDC_EDIT_SAVE,	MBI(SAVE), IDB_BUTTONBASE_MASK, strTooltip);

	m_ButtonToolbar.Check(IDC_EDIT_SELECT);
	m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);

	m_Picture.EnableZoom(TRUE);
	m_Picture.SetButtonToolbar(&m_ButtonToolbar);
	m_SelectRectSink.ShowDrizzleRectangles();
	m_Picture.SetImageSink(&m_SelectRectSink);
	m_ButtonToolbar.SetSink(this);

	m_Gamma.SetBackgroundColor(GetSysColor(COLOR_3DFACE));
	m_Gamma.ShowTooltips(FALSE);
	m_Gamma.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_Gamma.SetPegSide(TRUE, FALSE);
	m_Gamma.SetPegSide(FALSE, TRUE);
	m_Gamma.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_Gamma.GetGradient().AddPeg(RGB(0, 0, 0), 0.0, 0);
	m_Gamma.GetGradient().AddPeg(RGB(128, 128, 128), sqrt(0.5), 1);
	m_Gamma.GetGradient().AddPeg(RGB(255, 255, 255), 1.0, 2);
	m_Gamma.GetGradient().SetEndPegColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetInterpolationMethod(CGradient::Linear);


	m_4Corners.SetBitmaps(IDB_4CORNERS, RGB(255,0, 255));
	m_4Corners.SetFlat(TRUE);
	//m_4Corners.DrawTransparent(TRUE);

	CRegistry			reg;
	DWORD				bCheckVersion = 0;

	reg.LoadKey(REGENTRY_BASEKEY, _T("InternetCheck"), bCheckVersion);
	if (bCheckVersion == 2)
	{
		CString			strVersion;

		if (CheckVersion(strVersion))
		{
			CString newMajorStr, newMinorStr, newSubStr;

			int curPos = 0;

			newMajorStr = strVersion.Tokenize(_T("."), curPos);
			if (-1 != curPos) newMinorStr = strVersion.Tokenize(_T("."), curPos);
			if (-1 != curPos) newSubStr   = strVersion.Tokenize(_T("."), curPos);

			int newMajor = _ttoi(newMajorStr), newMinor = _ttoi(newMinorStr), newSub = _ttoi(newSubStr);

			if ((newMajor > DSSVER_MAJOR) ||
				(newMajor == DSSVER_MAJOR && newMinor > DSSVER_MINOR) ||
				(newMajor == DSSVER_MAJOR && newMinor == DSSVER_MINOR && newSub > DSSVER_SUB)
			   )
			{
				CString	strNewVersion;

				strNewVersion.Format(IDS_VERSIONAVAILABLE, strVersion);
				m_Infos.SetTextColor(RGB(255, 0, 0));
				m_Infos.SetText(strNewVersion);
				m_Infos.SetLink(TRUE, FALSE);
				m_Infos.SetHyperLink("http://deepskystacker.free.fr");
			};
		};
	};

	{
		m_GroupTab.ModifyStyle(0, CTCS_AUTOHIDEBUTTONS | CTCS_TOOLTIPS, 0);
		
		UpdateGroupTabs();
	};

/*
	m_ShowHideJobs.SetLink(TRUE, TRUE);
	m_ShowHideJobs.SetTransparent(TRUE);
	m_ShowHideJobs.SetLinkCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_HAND)));
	m_ShowHideJobs.SetFont3D(FALSE);
	m_ShowHideJobs.SetTextColor(RGB(0, 0, 192));
	m_ShowHideJobs.SetWindowText("Show/Hide Jobs");
	{
		m_JobTab.ModifyStyle(0, CTCS_RIGHT | CTCS_AUTOHIDEBUTTONS | CTCS_TOOLTIPS, 0);
		m_JobTab.InsertItem(0, "Main Job");
		m_JobTab.InsertItem(1, "Red");
		m_JobTab.InsertItem(2, "Green");
		m_JobTab.InsertItem(3, "Blue");
		m_JobTab.InsertItem(4, "Lum");
	};*/

	if (m_strStartingFileList.GetLength())
		OpenFileList(m_strStartingFileList);

	m_BackgroundLoading.SetWindow(m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateSplitter()
{
	CRect				rcSplitter;
	CRect				rcDialog;
	if (m_Splitter.GetSafeHwnd())
	{
		m_Splitter.GetWindowRect(&rcSplitter);
		ScreenToClient(&rcSplitter);
		GetClientRect(&rcDialog);

		// The list cannot be smaller than 120 pixels
		// The image cannot be smaller than 200 pixels

		int					nBaseY = (rcSplitter.top + rcSplitter.bottom) / 2;
		int					nMinY,
			nMaxY;

		if (nBaseY < 200)
		{
			// Move the splitter back to the minimum position
			int				nDelta = 200 - nBaseY;

			CSplitterControl::ChangeHeight(&m_Picture, nDelta);
			CSplitterControl::ChangeHeight(&m_Pictures, -nDelta, CW_BOTTOMALIGN);
			CSplitterControl::ChangePos(&m_ListInfo, 0, nDelta);
			CSplitterControl::ChangePos(&m_Splitter, 0, nDelta);
			Invalidate();
			UpdateWindow();
			m_Splitter.Invalidate();
			m_Picture.Invalidate();
			m_Pictures.Invalidate();
			m_ListInfo.Invalidate();
		};

		nMinY = min(200, nBaseY);
		nMaxY = max(rcDialog.Height() - 120, nBaseY);

		m_Splitter.SetRange(nMinY, nMaxY);
	}
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateGroupTabs()
{
	DWORD			dwLastGroupID;
	LONG			lNrFrames;
	LONG			lCurrentGroup;

	dwLastGroupID = m_Pictures.GetLastGroupID();

	lNrFrames = m_Pictures.GetNrFrames(dwLastGroupID);
	if (lNrFrames)
		dwLastGroupID++;

	lCurrentGroup = max(0, m_GroupTab.GetCurSel());

	m_GroupTab.DeleteAllItems();

	CString				strGroup;

	strGroup.LoadString(IDS_MAINGROUP);

	m_GroupTab.InsertItem(0, strGroup);

	strGroup.LoadString(IDS_GROUPIDMASK);

	for (LONG i = 1;i<=dwLastGroupID;i++)
	{
		CString			strName;

		strName.Format(strGroup, i);
		m_GroupTab.InsertItem(i, strName);
	};

	if (lCurrentGroup > dwLastGroupID)
	{
		m_GroupTab.SetCurSel(0);
		m_Pictures.SetCurrentGroupID(0);
	}
	else
		m_GroupTab.SetCurSel(lCurrentGroup);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnChangeGamma(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
		*pResult = 1;

	PegNMHDR *			pPegNMHDR = (PegNMHDR*)pNMHDR;
	double				fBlackPoint,
						fGrayPoint,
						fWhitePoint;

	if ((pPegNMHDR->nmhdr.code == GC_PEGMOVE) ||
		(pPegNMHDR->nmhdr.code == GC_PEGMOVED))
	{
		// Adjust 
		CGradient &			Gradient = m_Gamma.GetGradient();
		fBlackPoint = Gradient.GetPeg(Gradient.IndexFromId(0)).position;
		fGrayPoint  = Gradient.GetPeg(Gradient.IndexFromId(1)).position;
		fWhitePoint = Gradient.GetPeg(Gradient.IndexFromId(2)).position;
		BOOL				bAdjust = FALSE;

		switch (pPegNMHDR->peg.id)
		{
		case 0 :
			// Black point moving
			if (fBlackPoint>fWhitePoint-0.02)
			{
				fBlackPoint = fWhitePoint-0.02;
				bAdjust = TRUE;
			};
			if (fBlackPoint>fGrayPoint-0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = TRUE;
			};
			break;
		case 1 :
			// Gray point moving
			if (fGrayPoint<fBlackPoint+0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = TRUE;
			};
			if (fGrayPoint>fWhitePoint-0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = TRUE;
			};
			break;
		case 2 :
			// White point moving
			if (fWhitePoint<fBlackPoint+0.02)
			{
				fWhitePoint = fBlackPoint+0.02;
				bAdjust = TRUE;
			};
			if (fWhitePoint < fGrayPoint+0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = TRUE;
			};
			break;
		};
		if (bAdjust)
		{
			Gradient.SetPeg(Gradient.IndexFromId(0), (float)fBlackPoint);
			Gradient.SetPeg(Gradient.IndexFromId(1), (float)fGrayPoint);
			Gradient.SetPeg(Gradient.IndexFromId(2), (float)fWhitePoint);
			m_Gamma.InvalidateRect(NULL);
		};
	};

	if (pPegNMHDR->nmhdr.code == GC_PEGMOVED)
	{
		// Adjust Gamma
		m_GammaTransformation.InitTransformation(fBlackPoint*fBlackPoint, fGrayPoint*fGrayPoint, fWhitePoint*fWhitePoint);

		if (m_LoadedImage.m_hBitmap)
		{
			ApplyGammaTransformation(m_LoadedImage.m_hBitmap, m_LoadedImage.m_pBitmap, m_GammaTransformation);
			// Refresh
			m_Picture.Invalidate(TRUE);
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckDiskSpace(CAllStackingTasks & tasks)
{
	BOOL				bResult = FALSE;
	__int64				ulFlatSpace = 0,
						ulDarkSpace = 0,
						ulOffsetSpace = 0;
	__int64				ulNeededSpace = 0;

	for (LONG i = 0;i<tasks.m_vStacks.size();i++)
	{
		LONG			lWidth,
						lHeight,
						lNrChannels,
						lNrBytesPerChannel;
		__int64			ulSpace;

		lWidth		= tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lWidth;
		lHeight		= tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lHeight;
		lNrChannels = tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lNrChannels;
		lNrBytesPerChannel = tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lBitPerChannels/8;
		
		ulSpace		= lWidth * lHeight * lNrBytesPerChannel * lNrChannels;

		if (tasks.m_vStacks[i].m_pOffsetTask)
			ulOffsetSpace = max(ulOffsetSpace, ulSpace * tasks.m_vStacks[i].m_pOffsetTask->m_vBitmaps.size());

		if (tasks.m_vStacks[i].m_pDarkTask)
			ulDarkSpace = max(ulDarkSpace, ulSpace * tasks.m_vStacks[i].m_pDarkTask->m_vBitmaps.size());	

		if (tasks.m_vStacks[i].m_pFlatTask)
			ulFlatSpace = max(ulFlatSpace, ulSpace * tasks.m_vStacks[i].m_pFlatTask->m_vBitmaps.size());	
	};

	ulNeededSpace = max(ulFlatSpace, max(ulOffsetSpace, ulDarkSpace));
	ulNeededSpace *= 1.10;

	// Get available space from drive
	TCHAR			szTempPath[1+_MAX_PATH];

	szTempPath[0] = 0;
	GetTempPath(sizeof(szTempPath)/sizeof(szTempPath[0]), szTempPath);

	ULARGE_INTEGER			ulFreeSpace;
	ULARGE_INTEGER			ulTotal;
	ULARGE_INTEGER			ulTotalFree;

	GetDiskFreeSpaceEx(szTempPath, &ulFreeSpace, &ulTotal, &ulTotalFree);

	if (ulFreeSpace.QuadPart < ulNeededSpace)
	{
		ulFreeSpace.QuadPart /= 1024;
		ulNeededSpace /= 1024;

		LONG			lNeededSpace = ulNeededSpace;
		LONG			lFreeSpace = ulFreeSpace.LowPart;
		CString			strDrive;

		strDrive = szTempPath;
		strDrive = strDrive.Left(2);
		
		CString			strMessage;

		strMessage.Format(IDS_ERROR_NOTENOUGHFREESPACE, lNeededSpace, lFreeSpace, strDrive);
		if (AfxMessageBox(strMessage, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)==IDYES)
			bResult = TRUE;
		else
			bResult = FALSE;
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckReadOnlyFolders(CAllStackingTasks & tasks)
{
	BOOL					bResult = TRUE;
	std::vector<CString>	vFolders;

	if (!tasks.CheckReadOnlyStatus(vFolders))
	{
		CString				strText;
		CString				strFolders;

		for (LONG i = 0;i<vFolders.size();i++)
		{
			strFolders += vFolders[i];
			strFolders += "\n";
		};

		strText.Format(IDS_WARNINGREADONLY, strFolders);

		AfxMessageBox(strText, MB_OK | MB_ICONSTOP);
		bResult = FALSE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAdddarks() 
{
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENDARKFRAMES);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFolder"), strBaseDirectory);
	if (!strBaseDirectory.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureFolder"), strBaseDirectory);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkExtension"), strBaseExtension);
	if (!strBaseExtension.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureExtension"), strBaseExtension);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkIndex"), dwFilterIndex);
	if (!dwFilterIndex)
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureIndex"), dwFilterIndex);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST,
								INPUTFILE_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

	TCHAR				szBigBuffer[20000] = _T("");

	dlgOpen.m_ofn.lpstrFile = szBigBuffer;
	dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer)/sizeof(szBigBuffer[0]);

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
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_DARKFRAME, TRUE);
			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFolder"), strBaseDirectory);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkExtension"), strBaseExtension);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkIndex"), dwFilterIndex);

		UpdateGroupTabs();
	};	
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddDarkFlats() 
{
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENDARKFLATFRAMES);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFlatFolder"), strBaseDirectory);
	if (!strBaseDirectory.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureFolder"), strBaseDirectory);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFlatExtension"), strBaseExtension);
	if (!strBaseExtension.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureExtension"), strBaseExtension);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFlatIndex"), dwFilterIndex);
	if (!dwFilterIndex)
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureIndex"), dwFilterIndex);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST,
								INPUTFILE_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

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
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_DARKFLATFRAME, TRUE);
			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFlatFolder"), strBaseDirectory);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFlatExtension"), strBaseExtension);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddDarkFlatIndex"), dwFilterIndex);

		UpdateGroupTabs();
	};	
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddFlats() 
{
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENFLATFRAMES);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddFlatFolder"), strBaseDirectory);
	if (!strBaseDirectory.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureFolder"), strBaseDirectory);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddFlatExtension"), strBaseExtension);
	if (!strBaseExtension.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureExtension"), strBaseExtension);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddFlatIndex"), dwFilterIndex);
	if (!dwFilterIndex)
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureIndex"), dwFilterIndex);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST,
								INPUTFILE_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

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
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_FLATFRAME, TRUE);
			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddFlatFolder"), strBaseDirectory);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddFlatExtension"), strBaseExtension);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddFlatIndex"), dwFilterIndex);

		UpdateGroupTabs();
	};	
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddOffsets() 
{
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENBIASFRAMES);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddOffsetFolder"), strBaseDirectory);
	if (!strBaseDirectory.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureFolder"), strBaseDirectory);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddOffsetExtension"), strBaseExtension);
	if (!strBaseExtension.GetLength())
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureExtension"), strBaseExtension);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddOffsetIndex"), dwFilterIndex);
	if (!dwFilterIndex)
		reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureIndex"), dwFilterIndex);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST,
								INPUTFILE_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

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
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_OFFSETFRAME, TRUE);
			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddOffsetFolder"), strBaseDirectory);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddOffsetExtension"), strBaseExtension);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddOffsetIndex"), dwFilterIndex);

		UpdateGroupTabs();
	};	
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddpictures() 
{
	ZFUNCTRACE_RUNTIME();
	CRegistry			reg;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENLIGHTFRAMES);

	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureFolder"), strBaseDirectory);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureExtension"), strBaseExtension);
	reg.LoadKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureIndex"), dwFilterIndex);

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(TRUE, 
								strBaseExtension,
								NULL,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST,
								INPUTFILE_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);
	dlgOpen.m_ofn.nFilterIndex = dwFilterIndex;
	dlgOpen.m_ofn.lpstrTitle = strTitle.GetBuffer(200);

	TCHAR				szBigBuffer[20000] = _T("");

	dlgOpen.m_ofn.lpstrFile = szBigBuffer;
	dlgOpen.m_ofn.nMaxFile  = sizeof(szBigBuffer) / sizeof(szBigBuffer[0]);

	ZTRACE_RUNTIME("About to show file open dlg");
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
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID());
			_tsplitpath(strFile, szDrive, szDir, NULL, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureFolder"), strBaseDirectory);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureExtension"), strBaseExtension);
		reg.SaveKey(REGENTRY_BASEKEY_FOLDERS, _T("AddPictureIndex"), dwFilterIndex);

		UpdateGroupTabs();
	};	
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::DropFiles(HDROP hDropInfo)
{
	CDropFilesDlg			dlg;

	dlg.SetDropInfo(hDropInfo);
	if (dlg.DoModal() == IDOK)
	{
		std::vector<CString>	vFiles;

		BeginWaitCursor();
		dlg.GetDroppedFiles(vFiles);

		for (LONG i = 0;i<vFiles.size();i++)
			m_Pictures.AddFile(vFiles[i], m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), dlg.GetDropType(), TRUE);
		m_Pictures.RefreshList();
		UpdateGroupTabs();
		UpdateListInfo();
		EndWaitCursor();
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OpenFileList(LPCTSTR szFileList)
{
	CString					strList = szFileList;

	// Check that the file can be opened
	FILE *					hFile;

	hFile = _tfopen(strList, _T("rt"));
	if  (hFile)
	{
		fclose(hFile);
		SetCursor(::LoadCursor(NULL, IDC_WAIT));
		m_Pictures.LoadFilesFromList(strList);
		m_Pictures.RefreshList();
		m_MRUList.Add(strList);
		UpdateGroupTabs();
		UpdateListInfo();
		m_strCurrentFileList = szFileList;
		SetCurrentFileInTitle(m_strCurrentFileList);
		SetCursor(::LoadCursor(NULL, IDC_ARROW));
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::LoadList()
{
	if (CheckWorkspaceChanges())
	{
		BOOL			bOpenAnother = TRUE;

		if (m_MRUList.m_vLists.size())
		{
			CPoint				pt;
			CMenu				menu;
			CMenu *				popup;
			int					nResult;
			UINT				lStartID;

			bOpenAnother = FALSE;

			menu.LoadMenu(IDR_FILELISTS);
			popup = menu.GetSubMenu(0);

			CRect				rc;

			GetDeepStackerDlg(this)->GetExplorerBar().m_Stacking_LoadList.GetWindowRect(&rc);
			pt.x = rc.left;
			pt.y = rc.bottom;

			lStartID = ID_FILELIST_FIRSTMRU+1;
			for (LONG i = 0;i<m_MRUList.m_vLists.size();i++)
			{
				TCHAR				szDrive[1+_MAX_DRIVE];
				TCHAR				szDir[1+_MAX_DIR];
				TCHAR				szName[1+_MAX_FNAME];
				CString				strItem;

				_tsplitpath((LPCTSTR)m_MRUList.m_vLists[i], szDrive, szDir, szName, NULL);
				strItem.Format(_T("%s%s%s"), szDrive, szDir, szName);

				popup->InsertMenu(ID_FILELIST_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
				lStartID++;
			};

			popup->DeleteMenu(ID_FILELIST_FIRSTMRU, MF_BYCOMMAND);

			nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, NULL);;

			if (nResult == ID_FILELIST_OPENANOTHERFILELIST)
				bOpenAnother = TRUE;
			else if (nResult > ID_FILELIST_FIRSTMRU)
			{
				CString			strList;

				strList = m_MRUList.m_vLists[nResult-ID_FILELIST_FIRSTMRU-1];

				m_Pictures.LoadFilesFromList(strList);
				m_Pictures.RefreshList();
				m_MRUList.Add(strList);
				m_strCurrentFileList = strList;
				SetCurrentFileInTitle(m_strCurrentFileList);
			};
		};

		if (bOpenAnother)
		{
			m_Pictures.LoadList(m_MRUList, m_strCurrentFileList);
			SetCurrentFileInTitle(m_strCurrentFileList);
		};
		UpdateGroupTabs();
		UpdateListInfo();
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::SaveList()
{
	m_Pictures.SaveList(m_MRUList, m_strCurrentFileList);
	SetCurrentFileInTitle(m_strCurrentFileList);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnPictureChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Here check if the new image is dirty
	if (m_EditStarSink.IsDirty())
		m_ButtonToolbar.Enable(IDC_EDIT_SAVE, TRUE);
};

/* ------------------------------------------------------------------- */

CWndImageSink *	CStackingDlg::GetCurrentSink()
{
	if (m_ButtonToolbar.IsChecked(IDC_EDIT_STAR))
		return &m_EditStarSink;
	else if (m_ButtonToolbar.IsChecked(IDC_EDIT_COMET))
		return &m_EditStarSink;
	else if (m_ButtonToolbar.IsChecked(IDC_EDIT_SELECT))
		return &m_SelectRectSink;

	return NULL;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAskRegister()
{
	// Check that the current light frame is registered (or not)
	// and ask accordingly
	CLightFrameInfo			lfi;

	lfi.SetBitmap(m_strShowFile, FALSE, FALSE);
	if (!lfi.IsRegistered())
	{
		CAskRegistering		dlg;

		if (dlg.DoModal()==IDOK)
		{
			if (dlg.GetAction()==ARA_ONE)
			{
				// Register only this light frame
				m_Pictures.CheckAllLights(FALSE);
				m_Pictures.CheckImage(m_strShowFile, TRUE);
				RegisterCheckedImage();
			}
			else if (dlg.GetAction()==ARA_ALL)
			{
				// Register all the checked light frames
				m_Pictures.CheckImage(m_strShowFile, TRUE);
				RegisterCheckedImage();
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ButtonToolbar_OnCheck(DWORD dwID, CButtonToolbar * pButtonToolbar)
{
	switch (dwID)
	{
	case IDC_EDIT_COMET :
		if (pButtonToolbar->IsChecked(dwID))
		{
			CheckAskRegister();
			pButtonToolbar->Check(IDC_EDIT_STAR, FALSE);
			pButtonToolbar->Check(IDC_EDIT_SELECT, FALSE);
			m_EditStarSink.SetCometMode(TRUE);
			m_Picture.SetImageSink(&m_EditStarSink);
		};
		break;
	case IDC_EDIT_STAR :
		if (pButtonToolbar->IsChecked(dwID))
		{
			CheckAskRegister();
			pButtonToolbar->Check(IDC_EDIT_COMET, FALSE);
			pButtonToolbar->Check(IDC_EDIT_SELECT, FALSE);
			m_EditStarSink.SetCometMode(FALSE);
			m_Picture.SetImageSink(&m_EditStarSink);
		};
		break;
	case IDC_EDIT_SELECT :
		if (pButtonToolbar->IsChecked(dwID))
		{
			pButtonToolbar->Check(IDC_EDIT_COMET, FALSE);
			pButtonToolbar->Check(IDC_EDIT_STAR, FALSE);
			m_Picture.SetImageSink(&m_SelectRectSink);
		};
		break;
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ButtonToolbar_OnClick(DWORD dwID, CButtonToolbar * pButtonToolbar)
{
	if (dwID == IDC_EDIT_SAVE)
	{
		m_EditStarSink.SaveRegisterSettings();
		m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);
		// Update the list with the new info
		m_Pictures.UpdateItemScores(m_strShowFile);
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ButtonToolbar_OnRClick(DWORD dwID, CButtonToolbar * pButtonToolbar)
{
	if (dwID == IDC_EDIT_SAVE)
	{
		// Open the popup menu to maybe change the mode
		CPoint				pt;
		CMenu				menu;
		CMenu *				popup;
		int					nResult;

		GetCursorPos(&pt);
		menu.LoadMenu(IDR_SAVECONTEXT);
		popup = menu.GetSubMenu(0);

		switch (GetSaveEditMode())
		{
		case SECM_SAVEDONTASK :
			popup->CheckMenuItem(ID_SAVECONTEXT_SAVEWITHOUTASKING, MF_BYCOMMAND | MF_CHECKED);
			break;
		case SECM_DONTSAVEDONTASK :
			popup->CheckMenuItem(ID_SAVECONTEXT_DONTSAVEWITHOUTASKING, MF_BYCOMMAND | MF_CHECKED);
			break;
		case SECM_ASKALWAYS :
			popup->CheckMenuItem(ID_SAVECONTEXT_ASKALWAYS, MF_BYCOMMAND | MF_CHECKED);
			break;
		};
		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, this, NULL);

		if (nResult == ID_SAVECONTEXT_SAVEWITHOUTASKING)
			SetSaveEditMode(SECM_SAVEDONTASK);
		else if (nResult == ID_SAVECONTEXT_DONTSAVEWITHOUTASKING)
			SetSaveEditMode(SECM_DONTSAVEDONTASK);
		else if (nResult == ID_SAVECONTEXT_ASKALWAYS)
			SetSaveEditMode(SECM_ASKALWAYS);
	};
};

/* ------------------------------------------------------------------- */

void	CStackingDlg::UpdateListInfo()
{
	CString					strText;

	strText.Format(IDS_LISTINFO, 
						m_Pictures.GetNrCheckedFrames(), 
						m_Pictures.GetNrCheckedDarks(), 
						m_Pictures.GetNrCheckedFlats(), 
						m_Pictures.GetNrCheckedDarkFlats(),
						m_Pictures.GetNrCheckedOffsets());

	m_ListInfo.SetText(strText);

	for (LONG i = 0;i<m_GroupTab.GetItemCount();i++)
	{
		strText.Format(IDS_LISTINFO2,
					   m_Pictures.GetNrCheckedFrames(i),
					   m_Pictures.GetNrCheckedDarks(i),
					   m_Pictures.GetNrCheckedFlats(i),
					   m_Pictures.GetNrCheckedDarkFlats(i),
					   m_Pictures.GetNrCheckedOffsets(i));
		m_GroupTab.SetItemTooltipText(i, strText);
	};

};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckWorkspaceChanges()
{
	BOOL						bResult = FALSE;

	if (m_strCurrentFileList.GetLength() || m_Pictures.GetNrFrames())
	{
		CWorkspace				workspace;

		if (m_Pictures.IsDirty() || workspace.IsDirty())
		{
			// Ask for saving the current list.
			CString				strText;
			int					nResult;

			strText.Format(IDS_WARNING_SAVECHANGES, (LPCTSTR)m_strCurrentFileList);
			nResult = AfxMessageBox(strText, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);

			if (nResult == IDYES)
			{
				if (m_strCurrentFileList.GetLength())
					m_Pictures.SaveListToFile(m_strCurrentFileList);
				else
					m_Pictures.SaveList(m_MRUList, m_strCurrentFileList);
				SetCurrentFileInTitle(m_strCurrentFileList);
			};
			
			bResult = (nResult != IDCANCEL);
		}
		else
			bResult = TRUE;
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckEditChanges()
{
	BOOL						bResult = FALSE;

	if (m_EditStarSink.IsDirty())
	{
		int			nResult;

		nResult = AskSaveEditChangesMode();

		if (nResult == IDYES)
		{
			// Save the changes
			bResult = TRUE;
			m_EditStarSink.SaveRegisterSettings();
			m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);
			// Update the list with the new info
			m_Pictures.UpdateItemScores(m_strShowFile);
		}
		else if (nResult == IDNO)
			bResult = TRUE;
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

LRESULT CStackingDlg::OnBackgroundImageLoaded(WPARAM wParam, LPARAM lParam)
{
	CSmartPtr<CMemoryBitmap>	pBitmap;
	CSmartPtr<C32BitsBitmap>	phBitmap;

	if (m_strShowFile.GetLength() && m_BackgroundLoading.LoadImage(m_strShowFile, &pBitmap, &phBitmap))
	{
		m_LoadedImage.m_hBitmap = phBitmap;
		m_LoadedImage.m_pBitmap = pBitmap;
		if (m_GammaTransformation.IsInitialized())
			ApplyGammaTransformation(m_LoadedImage.m_hBitmap, m_LoadedImage.m_pBitmap, m_GammaTransformation);
		m_Picture.SetImg(phBitmap->GetHBITMAP(), true);

		if (m_Pictures.IsLightFrame(m_strShowFile))
		{
			m_Picture.SetButtonToolbar(&m_ButtonToolbar);
			m_EditStarSink.SetLightFrame(m_strShowFile);
			m_EditStarSink.SetBitmap(pBitmap);
			m_Picture.SetImageSink(GetCurrentSink());
		}
		else
		{
			m_Picture.SetImageSink(NULL);
			m_Picture.SetButtonToolbar(NULL);
			m_EditStarSink.SetBitmap(NULL);
		};
		m_Picture.SetBltMode(CWndImage::bltFitXY);
		m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);

		CBilinearParameters		Transformation;
		VOTINGPAIRVECTOR		vVotedPairs;

		if (m_Pictures.GetTransformation(m_strShowFile, Transformation, vVotedPairs))
			m_EditStarSink.SetTransformation(Transformation, vVotedPairs);
		m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
		m_Infos.SetText(m_strShowFile);
	}
	else if (m_strShowFile.GetLength())
	{
		CString				strText;

		strText.Format(IDS_LOADPICTURE, (LPCTSTR)m_strShowFile);
		m_Infos.SetBkColor(RGB(252, 251, 222), RGB(255, 151, 154), CLabel::Gradient);
		m_Infos.SetText(strText);
		m_Picture.SetImageSink(NULL);
		m_Picture.SetButtonToolbar(NULL);
		m_EditStarSink.SetBitmap(NULL);
	}
	else
	{
		m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
		m_Infos.SetText("");
		m_Picture.SetImageSink(NULL);
		m_Picture.SetButtonToolbar(NULL);
		m_EditStarSink.SetBitmap(NULL);
	};
	return 1;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnClickPictures(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString				strFileName;

	UpdateListInfo();
	if (m_Pictures.GetSelectedFileName(strFileName))
	{
		if (strFileName.CompareNoCase(m_strShowFile))
		{
			if (CheckEditChanges())
			{
				BeginWaitCursor();
				m_Infos.SetTextColor(RGB(0, 0, 0));
				m_Infos.SetText(strFileName);
				m_Infos.SetLink(FALSE, FALSE);
				m_strShowFile = strFileName;
				OnBackgroundImageLoaded(0, 0);
			};
		};
	}
	else
	{
		m_Infos.SetTextColor(RGB(0, 0, 0));
		m_Infos.SetLink(FALSE, FALSE);
		m_Infos.SetText("");
	};

	*pResult = 0;
}

/* ------------------------------------------------------------------- */

void CStackingDlg::ReloadCurrentImage()
{
	if (m_strShowFile.GetLength())
	{
		BeginWaitCursor();
		m_BackgroundLoading.ClearList();
		OnBackgroundImageLoaded(0, 0);
		EndWaitCursor();
	};

	m_Pictures.Invalidate(FALSE);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSelChangeGroup(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_Pictures.SetCurrentGroupID(m_GroupTab.GetCurSel());
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSelChangeJob(NMHDR* pNMHDR, LRESULT* pResult)
{
	//m_Pictures.SetCurrentGroupID(m_GroupTab.GetCurSel());
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnShowHideJobs( NMHDR * pNotifyStruct, LRESULT * result )
{
	//
};

/* ------------------------------------------------------------------- */

LRESULT CStackingDlg::OnCheckItem(WPARAM, LPARAM)
{
	UpdateListInfo();

	return 0;
};

/* ------------------------------------------------------------------- */

LRESULT CStackingDlg::OnSelectItem(WPARAM, LPARAM)
{
	LRESULT				lResult;

	OnClickPictures(NULL, &lResult);

	return 0;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	m_ControlPos.MoveControls();
	UpdateSplitter();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSplitter(NMHDR* pNMHDR, LRESULT* pResult)
{
	SPC_NMHDR* pHdr = reinterpret_cast<SPC_NMHDR*>(pNMHDR);
	
	CSplitterControl::ChangeHeight(&m_Picture, pHdr->delta);
	CSplitterControl::ChangeHeight(&m_Pictures, -pHdr->delta, CW_BOTTOMALIGN);
	CSplitterControl::ChangePos(&m_ListInfo, 0, pHdr->delta);
	Invalidate();
	UpdateWindow();
	m_Picture.Invalidate();
	m_Pictures.Invalidate();
	m_ListInfo.Invalidate();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UncheckNonStackablePictures()
{
	m_Pictures.UnCheckNonStackable();
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::ShowRecap(CAllStackingTasks & tasks)
{
	BOOL				bResult = FALSE;
	CStackRecap			dlg;

	dlg.SetStackingTasks(&tasks);
	bResult = (dlg.DoModal() == IDOK);

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateCheckedAndOffsets(CStackingEngine & StackingEngine)
{
	LIGHTFRAMEINFOVECTOR &	vBitmaps = StackingEngine.LightFrames();

	m_Pictures.ClearOffsets();
	for (LONG i = 0;i<vBitmaps.size();i++)
	{
		if (vBitmaps[i].m_bDisabled)
			m_Pictures.ClearOffset(vBitmaps[i].m_strFileName);
		else
			m_Pictures.UpdateOffset(vBitmaps[i].m_strFileName, vBitmaps[i].m_fXOffset, vBitmaps[i].m_fYOffset, vBitmaps[i].m_fAngle, vBitmaps[i].m_BilinearParameters, vBitmaps[i].m_vVotedPairs);
	};

	if (vBitmaps.size())
		m_EditStarSink.SetRefStars(vBitmaps[0].m_vStars);
	else
		m_EditStarSink.ClearRefStars();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::DoStacking(CAllStackingTasks & tasks, double fPercent)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bContinue = TRUE;
	CDSSProgressDlg		dlg;
	DWORD				dwStartTime = GetTickCount();
	DWORD				dwElapsedTime;

	if (!tasks.m_vStacks.size())
	{
		m_Pictures.FillTasks(tasks);
		tasks.ResolveTasks();
	};
	if (tasks.m_vStacks.size() &&
		tasks.m_vStacks[0].m_pLightTask && 
		tasks.m_vStacks[0].m_pLightTask->m_vBitmaps.size() &&
		tasks.m_vStacks[0].m_pLightTask->m_vBitmaps[0].m_strFileName.GetLength())
	{
		ZTRACE_RUNTIME("Start stacking process");
	};

	{
		// Stack registered light frames
		CStackingEngine				StackingEngine;
		CSmartPtr<CMemoryBitmap>	pBitmap;
		CString						strReferenceFrame;

		if (m_Pictures.GetReferenceFrame(strReferenceFrame))
			StackingEngine.SetReferenceFrame(strReferenceFrame);

		StackingEngine.SetKeptPercentage(fPercent);
		bContinue = StackingEngine.StackLightFrames(tasks, &dlg, &pBitmap);
		dwElapsedTime = GetTickCount()-dwStartTime;
		UpdateCheckedAndOffsets(StackingEngine);
		if (bContinue)
		{
			CString					strFileName;
			CString					strText;
			DWORD					iff;
			CWorkspace				workspace;

			workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("IntermediateFileFormat"), iff);

			if (StackingEngine.GetDefaultOutputFileName(strFileName, m_strCurrentFileList, (iff==IFF_TIFF)))
			{
				StackingEngine.WriteDescription(tasks, strFileName);

				strText.Format(IDS_SAVINGFINAL, strFileName);
				dlg.Start2(strText, 0);

				if (iff==IFF_TIFF)
				{
					if (pBitmap->IsMonochrome())
						WriteTIFF(strFileName, pBitmap, &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE);
					else
						WriteTIFF(strFileName, pBitmap, &dlg, TF_32BITRGBFLOAT, TC_DEFLATE);
				}
				else
				{
					if (pBitmap->IsMonochrome())
						WriteFITS(strFileName, pBitmap, &dlg, FF_32BITGRAYFLOAT);
					else
						WriteFITS(strFileName, pBitmap, &dlg, FF_32BITRGBFLOAT);
				};

				dlg.End2();
				GetProcessingDlg(this).LoadFile(strFileName);
				dlg.Close();

				// Change tab to processing
				CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

				if (pDlg)
					pDlg->ChangeTab(IDD_PROCESSING);
			};

			// Total elapsed time
			/*#ifdef DSSBETA
			{
				CString			strElapsed;
				CString			strText;

				ExposureToString((double)dwElapsedTime/1000.0, strElapsed);
				strText.Format(_T("Total process time: %s"), (LPCTSTR)strElapsed);
				AfxMessageBox(strText, MB_OK | MB_ICONINFORMATION);
			};
			#endif*/
		};
	};

	ZTRACE_RUNTIME("------------------------------\nEnd of stacking process");

	EndWaitCursor();
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckStacking(CAllStackingTasks & tasks)
{
	BOOL				bResult = FALSE;

	if (!m_Pictures.AreCheckedPictureCompatible())
		AfxMessageBox(IDS_ERROR_NOTCOMPATIBLE, MB_OK | MB_ICONSTOP);
	else if (!m_Pictures.GetNrCheckedFrames())
		AfxMessageBox(IDS_ERROR_NOTLIGHTCHECKED, MB_OK | MB_ICONSTOP);
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::FillTasks(CAllStackingTasks & tasks)
{
	m_Pictures.FillTasks(tasks);
	tasks.ResolveTasks();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::StackCheckedImage()
{
	if (CheckEditChanges())
	{
		BOOL				bContinue;
		CAllStackingTasks	tasks;
		CRect				rcSelect;

		m_Pictures.FillTasks(tasks);
		tasks.ResolveTasks();
		if (m_SelectRectSink.GetSelectRect(rcSelect))
			tasks.SetCustomRectangle(rcSelect);

		if (CheckReadOnlyFolders(tasks))
		{
			bContinue = CheckStacking(tasks);
			if (bContinue)
				bContinue = ShowRecap(tasks);
			if (bContinue)
			{
				m_BackgroundLoading.ClearList();
				if (m_Pictures.GetNrUnregisteredCheckedLightFrames())
				{
					CRegisterEngine	RegisterEngine;
					CDSSProgressDlg	dlg;

					m_Pictures.BlankCheckedItemScores();
					bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
					m_Pictures.UpdateCheckedItemScores();
					dlg.Close();
				};

				if (bContinue)
					DoStacking(tasks);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::BatchStack()
{
	CBatchStacking			dlg;

	dlg.SetMRUList(m_MRUList);
	dlg.DoModal();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAbove() 
{
	if (CheckEditChanges())
	{
		CCheckAbove		dlg;
		double			fThreshold;

		if (dlg.DoModal() == IDOK)
		{
			fThreshold = dlg.GetThreshold();
			if (dlg.IsPercent())
				m_Pictures.CheckBest(fThreshold);
			else
				m_Pictures.CheckAbove(fThreshold);
		};
		UpdateListInfo();
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAll()
{
	m_Pictures.CheckAll(TRUE);
	m_Pictures.CheckAllDarks(TRUE);
	UpdateListInfo();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UncheckAll()
{
	m_Pictures.CheckAll(FALSE);
	m_Pictures.CheckAllDarks(FALSE);
	UpdateListInfo();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ClearList()
{
	if (CheckEditChanges() && CheckWorkspaceChanges())
	{
		m_Pictures.Clear();
		m_Picture.SetImg((CBitmap*)NULL);
		m_Picture.SetImageSink(NULL);
		m_Picture.SetButtonToolbar(NULL);
		m_EditStarSink.SetBitmap(NULL);
		m_strShowFile.Empty();
		m_Infos.SetText(m_strShowFile);
		m_BackgroundLoading.ClearList();
		m_LoadedImage.Clear();
		UpdateGroupTabs();
		UpdateListInfo();
		m_strCurrentFileList.Empty();
		SetCurrentFileInTitle(m_strCurrentFileList);
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckBests(double fPercent)
{
	if (CheckEditChanges())
		m_Pictures.CheckBest(fPercent);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ComputeOffsets() 
{

	if (CheckEditChanges() && (m_Pictures.GetNrCheckedFrames() > 0))
	{
		BOOL					bContinue = TRUE;
		CAllStackingTasks		tasks;

		m_Pictures.FillTasks(tasks);
		tasks.ResolveTasks();

		if (m_Pictures.GetNrUnregisteredCheckedLightFrames())
		{
			CRegisterEngine	RegisterEngine;
			CDSSProgressDlg	dlg;

			m_Pictures.BlankCheckedItemScores();
			bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
			m_Pictures.UpdateCheckedItemScores();
			dlg.Close();
		};

		if (bContinue)
		{
			CDSSProgressDlg			dlg;
			CStackingEngine			StackingEngine;
			CString					strReferenceFrame;

			if (m_Pictures.GetReferenceFrame(strReferenceFrame))
				StackingEngine.SetReferenceFrame(strReferenceFrame);

			BeginWaitCursor();
			StackingEngine.ComputeOffsets(tasks, &dlg);

			// For each light frame - update the offset in the list
			UpdateCheckedAndOffsets(StackingEngine);

			EndWaitCursor();
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::SaveOnClose()
{
	m_Pictures.SaveState();
	m_MRUList.SaveToRegistry();

	return CheckEditChanges() && CheckWorkspaceChanges();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::RegisterCheckedImage()
{
	CDSSProgressDlg			dlg;
	CString					strText;
	CRegisterSettings		dlgSettings;
	BOOL					bContinue = TRUE;

	BOOL					bFound = FALSE;

	if (m_Pictures.GetNrCheckedFrames())
	{
		CString				strFirstLightFrame;

		m_Pictures.GetFirstCheckedLightFrame(strFirstLightFrame);

		dlgSettings.SetForceRegister(!m_Pictures.GetNrUnregisteredCheckedLightFrames());
		dlgSettings.SetNoDark(!m_Pictures.GetNrCheckedDarks());
		dlgSettings.SetNoFlat(!m_Pictures.GetNrCheckedFlats());
		dlgSettings.SetNoOffset(!m_Pictures.GetNrCheckedOffsets());
		dlgSettings.SetFirstLightFrame(strFirstLightFrame);

		CAllStackingTasks	tasks;
		CRect				rcSelect;

		m_Pictures.FillTasks(tasks);
		tasks.ResolveTasks();
		if (m_SelectRectSink.GetSelectRect(rcSelect))
			tasks.SetCustomRectangle(rcSelect);

		dlgSettings.SetStackingTasks(&tasks);

		if (dlgSettings.DoModal() == IDOK)
		{
			double				fMinLuminancy = 0.10;
			BOOL				bForceRegister;
			LONG				lCount = 0;
			LONG				lMaxCount = 0;
			double				fPercent;
			BOOL				bStackAfter;

			bForceRegister = dlgSettings.GetForceRegister();

			bStackAfter = dlgSettings.IsStackAfter(fPercent);

			if (CheckReadOnlyFolders(tasks))
			{
				if (bStackAfter)
				{
					bContinue = CheckStacking(tasks);
					if (bContinue)
						bContinue = ShowRecap(tasks);
				}
				else
				{
					bContinue = CheckStacking(tasks);
				};

				DWORD				dwStartTime = GetTickCount();
				DWORD				dwEndTime;

				if (bContinue)
				{
					CRegisterEngine	RegisterEngine;

					m_BackgroundLoading.ClearList();
					m_Pictures.BlankCheckedItemScores();

					bContinue = RegisterEngine.RegisterLightFrames(tasks, bForceRegister, &dlg);

					m_Pictures.UpdateCheckedItemScores();
					// Update the current image score if necessary
					if (m_strShowFile.GetLength() 
						&& m_Pictures.IsLightFrame(m_strShowFile) 
						&& m_Pictures.IsChecked(m_strShowFile))
					{
						// Update the registering info
						m_EditStarSink.SetLightFrame(m_strShowFile);
						m_Picture.Invalidate(TRUE);
					};

					dlg.Close();
				};

				if (bContinue && bStackAfter)
				{
					DoStacking(tasks, fPercent);
					dwEndTime = GetTickCount();
				};
			};
		};
	}
	else
	{
		AfxMessageBox(IDS_ERROR_NOTLIGHTCHECKED2, MB_OK | MB_ICONSTOP);
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnBnClicked4corners()
{
	m_Picture.Set4CornersMode(!m_Picture.Get4CornersMode());
}

/* ------------------------------------------------------------------- */
