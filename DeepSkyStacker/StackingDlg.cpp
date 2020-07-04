// StackingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "StackingDlg.h"
#include "DeepStackerDlg.h"
#include "DeepStack.h"
#include "ProgressDlg.h"
#include "CheckAbove.h"

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

#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QSettings>
#include <QUrl>

#include "qmfcapp.h"
#include "qwinwidget.h"

#define _USE_MATH_DEFINES
#include <cmath>

const		DWORD					IDC_EDIT_SELECT = 1;
const		DWORD					IDC_EDIT_STAR   = 2;
const		DWORD					IDC_EDIT_COMET  = 3;
const		DWORD					IDC_EDIT_SAVE   = 4;

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackingDlg dialog


CStackingDlg::CStackingDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CStackingDlg::IDD, pParent),
	m_cCtrlCache(this),
	networkManager(nullptr)
{
	//{{AFX_DATA_INIT(CStackingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_MRUList.readSettings();
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

	m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	m_ListInfo.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	m_Picture.CreateFromStatic(&m_PictureStatic);

	{
		CRect				rc;

		GetDlgItem(IDC_SPLITTER)->GetWindowRect(rc);
		ScreenToClient(rc);
		m_Splitter.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER);
	};

	// Add controls to the control cache - this is just a container for helping calcualte sizes and
	// positions when resizing the dialog.
	m_cCtrlCache.AddToCtrlCache(IDC_INFOS);
	m_cCtrlCache.AddToCtrlCache(IDC_4CORNERS);
	m_cCtrlCache.AddToCtrlCache(IDC_GAMMA);
	m_cCtrlCache.AddToCtrlCache(IDC_PICTURE);
	m_cCtrlCache.AddToCtrlCache(IDC_SPLITTER);
	m_cCtrlCache.AddToCtrlCache(IDC_LISTINFO);
	m_cCtrlCache.AddToCtrlCache(IDC_PICTURES);
	m_cCtrlCache.AddToCtrlCache(IDC_GROUPTAB);

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
	m_ButtonToolbar.Enable(IDC_EDIT_SAVE, false);

	m_Picture.EnableZoom(true);
	m_Picture.SetButtonToolbar(&m_ButtonToolbar);
	m_SelectRectSink.ShowDrizzleRectangles();
	m_Picture.SetImageSink(&m_SelectRectSink);
	m_ButtonToolbar.SetSink(this);

	m_Gamma.SetBackgroundColor(GetSysColor(COLOR_3DFACE));
	m_Gamma.ShowTooltips(false);
	m_Gamma.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_Gamma.SetPegSide(true, false);
	m_Gamma.SetPegSide(false, true);
	m_Gamma.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_Gamma.GetGradient().AddPeg(RGB(0, 0, 0), 0.0, 0);
	m_Gamma.GetGradient().AddPeg(RGB(128, 128, 128), sqrt(0.5), 1);
	m_Gamma.GetGradient().AddPeg(RGB(255, 255, 255), 1.0, 2);
	m_Gamma.GetGradient().SetEndPegColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetInterpolationMethod(CGradient::Linear);


	m_4Corners.SetBitmaps(IDB_4CORNERS, RGB(255,0, 255));
	m_4Corners.SetFlat(true);
	//m_4Corners.DrawTransparent(true);

	QSettings			settings;
	bool checkVersion = settings.value("InternetCheck", false).toBool();
	if (checkVersion)
		retrieveLatestVersionInfo();   // will update ui asynchronously

	{
		m_GroupTab.ModifyStyle(0, CTCS_AUTOHIDEBUTTONS | CTCS_TOOLTIPS, 0);

		UpdateGroupTabs();
	};

/*
	m_ShowHideJobs.SetLink(true, true);
	m_ShowHideJobs.SetTransparent(true);
	m_ShowHideJobs.SetLinkCursor(LoadCursor(nullptr,MAKEINTRESOURCE(IDC_HAND)));
	m_ShowHideJobs.SetFont3D(false);
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

	return true;  // return true unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return false
}

/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateLayout()
{
	// No controls present, nothing to do!
	if (GetDlgItem(IDC_PICTURE) == nullptr)
		return;

	// Update the cache so all the sizes and positions are correct.
	m_cCtrlCache.UpdateCtrlCache();

	CRect rcCurrentDlgSize;
	GetClientRect(rcCurrentDlgSize);

	// Cache the controls that we can scale to make things fit.
	// Work out vertical space change.
	int nCtrlHeightSum = 0;
	const int nTopSpacing = min(m_cCtrlCache.GetCtrlOffset(IDC_LISTINFO).y, (min(m_cCtrlCache.GetCtrlOffset(IDC_GAMMA).y, m_cCtrlCache.GetCtrlOffset(IDC_4CORNERS).y)));
	nCtrlHeightSum += nTopSpacing;
	nCtrlHeightSum += max(m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Height(), (max(m_cCtrlCache.GetCtrlSize(IDC_GAMMA).Height(), m_cCtrlCache.GetCtrlSize(IDC_4CORNERS).Height())));

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_SPLITTER).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_SPLITTER).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_LISTINFO).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_PICTURES).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_GROUPTAB).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_GROUPTAB).Height();

	nCtrlHeightSum += nTopSpacing;

	// Preferentially scale the picture first, then the list afterwards (if possible)
	int nDiffPictureY = rcCurrentDlgSize.Height() - nCtrlHeightSum;
	int nDiffListY = 0;

	// Handle if there isn't enough space to handle the picture resizing alone.
	if (m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height() + nDiffPictureY <= sm_nMinImageHeight)
	{
		int nMaxMovement = m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height() - sm_nMinImageHeight;
		if (nMaxMovement <= 0)
		{
			nDiffListY = nDiffPictureY;
			nDiffPictureY = 0;
		}
		else
		{
			nDiffListY = nDiffPictureY + nMaxMovement;
			nDiffPictureY = -nMaxMovement;
		}
		// Handle if there isn't enough space to handle the list resizing as well.
		if (m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height() + nDiffListY <= sm_nMinListHeight)
		{
			int nMaxMovement = m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height() - sm_nMinListHeight;
			if (nMaxMovement <= 0)
				nDiffListY = 0;
			else
				nDiffListY = -nMaxMovement;
		}
	}

	// Perform the resizing and moving of the controls.
	if (nDiffPictureY != 0)
	{
		m_cCtrlCache.SizeCtrlVert(IDC_PICTURE, m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height() + nDiffPictureY);
		m_Splitter.ChangePos(&m_Splitter, 0, nDiffPictureY);
		m_cCtrlCache.MoveCtrlVert(IDC_LISTINFO, nDiffPictureY);
		m_cCtrlCache.MoveCtrlVert(IDC_PICTURES, nDiffPictureY);
		m_cCtrlCache.MoveCtrlVert(IDC_GROUPTAB, nDiffPictureY);
	}
	if (nDiffListY != 0)
	{
		m_cCtrlCache.SizeCtrlVert(IDC_PICTURES, m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height() + nDiffListY);
		m_cCtrlCache.MoveCtrlVert(IDC_GROUPTAB, nDiffListY);
	}

	// Now look at the widths
	int nCtrlWidthSum = m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).x + m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() + m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).x; // Assume same padding at either end.
	int nDiffX = rcCurrentDlgSize.Width() - nCtrlWidthSum;
	if (m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() + nDiffX <= sm_nMinListWidth)
	{
		int nMaxMovement = m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() - sm_nMinListWidth;
		if (nMaxMovement <= 0)
			nDiffX = 0;
		else
			nDiffX = -nMaxMovement;
	}
	if (nDiffX)
	{
		m_cCtrlCache.SizeCtrlHoriz(IDC_INFOS, m_cCtrlCache.GetCtrlSize(IDC_INFOS).Width() + nDiffX);
		m_cCtrlCache.MoveCtrlHoriz(IDC_4CORNERS, nDiffX);
		m_cCtrlCache.MoveCtrlHoriz(IDC_GAMMA, nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_PICTURE, m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() + nDiffX);
		m_Splitter.ChangeWidth(&m_Splitter, nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_LISTINFO, m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Width() + nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_PICTURES, m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Width() + nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_GROUPTAB, m_cCtrlCache.GetCtrlSize(IDC_GROUPTAB).Width() + nDiffX);
	}

	// Because we've resized things, we need to update the max splitter range accordingly.
	// This is not quite right - couple of pixels out - but not sure why.
	int nMinY = m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).y + sm_nMinImageHeight;
	int nMaxY = max(nMinY + m_cCtrlCache.GetCtrlSize(IDC_SPLITTER).Height(), rcCurrentDlgSize.Height() - (sm_nMinListHeight + m_cCtrlCache.GetCtrlSize(IDC_GROUPTAB).Height() + m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Height() + m_cCtrlCache.GetCtrlSize(IDC_SPLITTER).Height()));
	m_Splitter.SetRange(nMinY, nMaxY);

	// Update everything.
	Invalidate();
	UpdateWindow();
	m_cCtrlCache.InvalidateCtrls();
}

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
		bool				bAdjust = false;

		switch (pPegNMHDR->peg.id)
		{
		case 0 :
			// Black point moving
			if (fBlackPoint>fWhitePoint-0.02)
			{
				fBlackPoint = fWhitePoint-0.02;
				bAdjust = true;
			};
			if (fBlackPoint>fGrayPoint-0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = true;
			};
			break;
		case 1 :
			// Gray point moving
			if (fGrayPoint<fBlackPoint+0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = true;
			};
			if (fGrayPoint>fWhitePoint-0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = true;
			};
			break;
		case 2 :
			// White point moving
			if (fWhitePoint<fBlackPoint+0.02)
			{
				fWhitePoint = fBlackPoint+0.02;
				bAdjust = true;
			};
			if (fWhitePoint < fGrayPoint+0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = true;
			};
			break;
		};
		if (bAdjust)
		{
			Gradient.SetPeg(Gradient.IndexFromId(0), (float)fBlackPoint);
			Gradient.SetPeg(Gradient.IndexFromId(1), (float)fGrayPoint);
			Gradient.SetPeg(Gradient.IndexFromId(2), (float)fWhitePoint);
			m_Gamma.InvalidateRect(nullptr);
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
			m_Picture.Invalidate(true);
		};
	};
};

/* ------------------------------------------------------------------- */

bool CStackingDlg::CheckDiskSpace(CAllStackingTasks & tasks)
{
	bool				bResult = false;
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
			ulOffsetSpace = max(ulOffsetSpace, static_cast<__int64>(ulSpace * tasks.m_vStacks[i].m_pOffsetTask->m_vBitmaps.size()));

		if (tasks.m_vStacks[i].m_pDarkTask)
			ulDarkSpace = max(ulDarkSpace, static_cast<__int64>(ulSpace * tasks.m_vStacks[i].m_pDarkTask->m_vBitmaps.size()));

		if (tasks.m_vStacks[i].m_pFlatTask)
			ulFlatSpace = max(ulFlatSpace, static_cast<__int64>(ulSpace * tasks.m_vStacks[i].m_pFlatTask->m_vBitmaps.size()));
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
			bResult = true;
		else
			bResult = false;
	}
	else
		bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CStackingDlg::CheckReadOnlyFolders(CAllStackingTasks & tasks)
{
	bool					bResult = true;
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
		bResult = false;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAdddarks()
{
	QSettings			settings;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENDARKFRAMES);

	strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddDarkFolder").toString().utf16());
	if (!strBaseDirectory.GetLength())
		strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddPictureFolder").toString().utf16());

	strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddDarkExtension").toString().utf16());
	if (!strBaseExtension.GetLength())
		strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddPictureExtension").toString().utf16());

	dwFilterIndex = settings.value("Folders/AddDarkIndex", uint(0)).toUInt();
	if (!dwFilterIndex)
		dwFilterIndex = settings.value("Folders/AddPictureIndex", uint(0)).toUInt();

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(true,
								strBaseExtension,
								nullptr,
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

		BeginWaitCursor();
		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			strFile = dlgOpen.GetNextPathName(pos);
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_DARKFRAME, true);
			_tsplitpath(strFile, szDrive, szDir, nullptr, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		settings.setValue("Folders/AddDarkFolder", QString::fromWCharArray(strBaseDirectory.GetString()));
		settings.setValue("Folders/AddDarkExtension", QString::fromWCharArray(strBaseExtension.GetString()));
		settings.setValue("Folders/AddDarkIndex", (uint)dwFilterIndex);

		UpdateGroupTabs();
	};
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddDarkFlats()
{
	QSettings			settings;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENDARKFLATFRAMES);

	strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddDarkFlatFolder").toString().utf16());
	if (!strBaseDirectory.GetLength())
		strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddPictureFolder").toString().utf16());

	strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddDarkFlatExtension").toString().utf16());
	if (!strBaseExtension.GetLength())
		strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddPictureExtension").toString().utf16());

	dwFilterIndex = settings.value("Folders/AddDarkFlatIndex", uint(0)).toUInt();
	if (!dwFilterIndex)
		dwFilterIndex = settings.value("Folders/AddPictureIndex", uint(0)).toUInt();

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(true,
								strBaseExtension,
								nullptr,
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

		BeginWaitCursor();
		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			strFile = dlgOpen.GetNextPathName(pos);
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_DARKFLATFRAME, true);
			_tsplitpath(strFile, szDrive, szDir, nullptr, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		settings.setValue("Folders/AddDarkFlatFolder", QString::fromWCharArray(strBaseDirectory.GetString()));
		settings.setValue("Folders/AddDarkFlatExtension", QString::fromWCharArray(strBaseExtension.GetString()));
		settings.setValue("Folders/AddDarkFlatIndex", (uint)dwFilterIndex);

		UpdateGroupTabs();
	};
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddFlats()
{
	QSettings			settings;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENFLATFRAMES);

	strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddFlatFolder").toString().utf16());
	if (!strBaseDirectory.GetLength())
		strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddPictureFolder").toString().utf16());

	strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddFlatExtension").toString().utf16());
	if (!strBaseExtension.GetLength())
		strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddPictureExtension").toString().utf16());

	dwFilterIndex = settings.value("Folders/AddFlatIndex", uint(0)).toUInt();
	if (!dwFilterIndex)
		dwFilterIndex = settings.value("Folders/AddPictureIndex", uint(0)).toUInt();

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(true,
								strBaseExtension,
								nullptr,
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

		BeginWaitCursor();

		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			strFile = dlgOpen.GetNextPathName(pos);
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_FLATFRAME, true);
			_tsplitpath(strFile, szDrive, szDir, nullptr, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		settings.setValue("Folders/AddFlatFolder", QString::fromWCharArray(strBaseDirectory.GetString()));
		settings.setValue("Folders/AddFlatExtension", QString::fromWCharArray(strBaseExtension.GetString()));
		settings.setValue("Folders/AddFlatIndex", (uint)dwFilterIndex);

		UpdateGroupTabs();
	};
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddOffsets()
{
	QSettings			settings;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENBIASFRAMES);

	strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddOffsetFolder").toString().utf16());
	if (!strBaseDirectory.GetLength())
		strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddPictureFolder").toString().utf16());

	strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddOffsetExtension").toString().utf16());
	if (!strBaseExtension.GetLength())
		strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddPictureExtension").toString().utf16());

	dwFilterIndex = settings.value("Folders/AddOffsetIndex", uint(0)).toUInt();
	if (!dwFilterIndex)
		dwFilterIndex = settings.value("Folders/AddPictureIndex", uint(0)).toUInt();

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(true,
								strBaseExtension,
								nullptr,
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

		BeginWaitCursor();
		pos = dlgOpen.GetStartPosition();
		while (pos)
		{
			CString		strFile;
			TCHAR		szDir[1+_MAX_DIR];
			TCHAR		szDrive[1+_MAX_DRIVE];
			TCHAR		szExt[1+_MAX_EXT];

			strFile = dlgOpen.GetNextPathName(pos);
			m_Pictures.AddFile(strFile, m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), PICTURETYPE_OFFSETFRAME, true);
			_tsplitpath(strFile, szDrive, szDir, nullptr, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		settings.setValue("Folders/AddOffsetFolder", QString::fromWCharArray(strBaseDirectory.GetString()));
		settings.setValue("Folders/AddOffsetExtension", QString::fromWCharArray(strBaseExtension.GetString()));
		settings.setValue("Folders/AddOffsetIndex", (uint)dwFilterIndex);

		UpdateGroupTabs();
	};
	UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddpictures()
{
	ZFUNCTRACE_RUNTIME();
	QSettings			settings;
	CString				strBaseDirectory;
	CString				strBaseExtension;
	DWORD				dwFilterIndex = 0;
	CString				strTitle;

	strTitle.LoadString(IDS_TITLE_OPENLIGHTFRAMES);

	strBaseDirectory = CString((LPCTSTR)settings.value("Folders/AddPictureFolder").toString().utf16());

	strBaseExtension = CString((LPCTSTR)settings.value("Folders/AddPictureExtension").toString().utf16());

	dwFilterIndex = settings.value("Folders/AddPictureIndex", uint(0)).toUInt();

	if (!strBaseExtension.GetLength())
		strBaseExtension = _T(".bmp");

	CFileDialog			dlgOpen(true,
								strBaseExtension,
								nullptr,
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
			_tsplitpath(strFile, szDrive, szDir, nullptr, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
			strBaseExtension = szExt;
		};
		EndWaitCursor();
		m_Pictures.RefreshList();

		dwFilterIndex = dlgOpen.m_ofn.nFilterIndex;
		settings.setValue("Folders/AddPictureFolder", QString::fromWCharArray(strBaseDirectory.GetString()));
		settings.setValue("Folders/AddPictureExtension", QString::fromWCharArray(strBaseExtension.GetString()));
		settings.setValue("Folders/AddPictureIndex", (uint)dwFilterIndex);

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
			m_Pictures.AddFile(vFiles[i], m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), dlg.GetDropType(), true);
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
		SetCursor(::LoadCursor(nullptr, IDC_WAIT));
		m_Pictures.LoadFilesFromList(strList);
		m_Pictures.RefreshList();
		m_MRUList.Add(strList);
		UpdateGroupTabs();
		UpdateListInfo();
		m_strCurrentFileList = szFileList;
		SetCurrentFileInTitle(m_strCurrentFileList);
		SetCursor(::LoadCursor(nullptr, IDC_ARROW));
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::LoadList()
{
	if (CheckWorkspaceChanges())
	{
		bool			bOpenAnother = true;

		if (m_MRUList.m_vLists.size())
		{
			CPoint				pt;
			CMenu				menu;
			CMenu *				popup;
			int					nResult;
			UINT				lStartID;

			bOpenAnother = false;

			menu.LoadMenu(IDR_FILELISTS);
			popup = menu.GetSubMenu(0);

			QPoint point = QCursor::pos();
			pt.x = point.x();
			pt.y = point.y();

			lStartID = ID_FILELIST_FIRSTMRU+1;
			for (LONG i = 0;i<m_MRUList.m_vLists.size();i++)
			{
				TCHAR				szDrive[1+_MAX_DRIVE];
				TCHAR				szDir[1+_MAX_DIR];
				TCHAR				szName[1+_MAX_FNAME];
				CString				strItem;

				_tsplitpath((LPCTSTR)m_MRUList.m_vLists[i], szDrive, szDir, szName, nullptr);
				strItem.Format(_T("%s%s%s"), szDrive, szDir, szName);

				popup->InsertMenu(ID_FILELIST_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
				lStartID++;
			};

			popup->DeleteMenu(ID_FILELIST_FIRSTMRU, MF_BYCOMMAND);

			nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, nullptr);;

			if (nResult == ID_FILELIST_OPENANOTHERFILELIST)
				bOpenAnother = true;
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
		m_ButtonToolbar.Enable(IDC_EDIT_SAVE, true);
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

	return nullptr;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAskRegister()
{
	// Check that the current light frame is registered (or not)
	// and ask accordingly
	CLightFrameInfo			lfi;

	lfi.SetBitmap(m_strShowFile, false, false);
	if (!lfi.IsRegistered())
	{
		CAskRegistering		dlg;

		if (dlg.DoModal()==IDOK)
		{
			if (dlg.GetAction()==ARA_ONE)
			{
				// Register only this light frame
				m_Pictures.CheckAllLights(false);
				m_Pictures.CheckImage(m_strShowFile, true);
				RegisterCheckedImage();
			}
			else if (dlg.GetAction()==ARA_ALL)
			{
				// Register all the checked light frames
				m_Pictures.CheckImage(m_strShowFile, true);
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
			pButtonToolbar->Check(IDC_EDIT_STAR, false);
			pButtonToolbar->Check(IDC_EDIT_SELECT, false);
			m_EditStarSink.SetCometMode(true);
			m_Picture.SetImageSink(&m_EditStarSink);
		};
		break;
	case IDC_EDIT_STAR :
		if (pButtonToolbar->IsChecked(dwID))
		{
			CheckAskRegister();
			pButtonToolbar->Check(IDC_EDIT_COMET, false);
			pButtonToolbar->Check(IDC_EDIT_SELECT, false);
			m_EditStarSink.SetCometMode(false);
			m_Picture.SetImageSink(&m_EditStarSink);
		};
		break;
	case IDC_EDIT_SELECT :
		if (pButtonToolbar->IsChecked(dwID))
		{
			pButtonToolbar->Check(IDC_EDIT_COMET, false);
			pButtonToolbar->Check(IDC_EDIT_STAR, false);
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
		m_ButtonToolbar.Enable(IDC_EDIT_SAVE, false);
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
		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, this, nullptr);

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

bool CStackingDlg::CheckWorkspaceChanges()
{
	bool						bResult = false;

	if (m_strCurrentFileList.GetLength() || m_Pictures.GetNrFrames())
	{
		CWorkspace				workspace;

		if (m_Pictures.IsDirty() || workspace.isDirty())
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
			bResult = true;
	}
	else
		bResult = true;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CStackingDlg::CheckEditChanges()
{
	bool						bResult = false;

	if (m_EditStarSink.IsDirty())
	{
		int			nResult;

		nResult = AskSaveEditChangesMode();

		if (nResult == IDYES)
		{
			// Save the changes
			bResult = true;
			m_EditStarSink.SaveRegisterSettings();
			m_ButtonToolbar.Enable(IDC_EDIT_SAVE, false);
			// Update the list with the new info
			m_Pictures.UpdateItemScores(m_strShowFile);
		}
		else if (nResult == IDNO)
			bResult = true;
	}
	else
		bResult = true;

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
			m_Picture.SetImageSink(nullptr);
			m_Picture.SetButtonToolbar(nullptr);
			m_EditStarSink.SetBitmap(nullptr);
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
		m_Picture.SetImageSink(nullptr);
		m_Picture.SetButtonToolbar(nullptr);
		m_EditStarSink.SetBitmap(nullptr);
	}
	else
	{
		m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
		m_Infos.SetText("");
		m_Picture.SetImageSink(nullptr);
		m_Picture.SetButtonToolbar(nullptr);
		m_EditStarSink.SetBitmap(nullptr);
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
				m_Infos.SetLink(false, false);
				m_strShowFile = strFileName;
				OnBackgroundImageLoaded(0, 0);
			};
		};
	}
	else
	{
		m_Infos.SetTextColor(RGB(0, 0, 0));
		m_Infos.SetLink(false, false);
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

	m_Pictures.Invalidate(false);
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

	OnClickPictures(nullptr, &lResult);

	return 0;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (!(cx == 0 && cy == 0))
		UpdateLayout();
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

bool CStackingDlg::ShowRecap(CAllStackingTasks & tasks)
{
	QWinWidget	widget(this->GetParent());
	widget.showCentered();
	StackRecap	dlg;

	dlg.setStackingTasks(&tasks);
	return dlg.exec();
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
	bool				bContinue = true;
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

			iff = (INTERMEDIATEFILEFORMAT)workspace.value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();

			if (StackingEngine.GetDefaultOutputFileName(strFileName, m_strCurrentFileList, (iff==IFF_TIFF)))
			{
				StackingEngine.WriteDescription(tasks, strFileName);

				strText.Format(IDS_SAVINGFINAL, strFileName);
				dlg.Start2(strText, 0);

				if (iff==IFF_TIFF)
				{
					if (pBitmap->IsMonochrome())
						WriteTIFF(strFileName, pBitmap, &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE, nullptr);
					else
						WriteTIFF(strFileName, pBitmap, &dlg, TF_32BITRGBFLOAT, TC_DEFLATE, nullptr);
				}
				else
				{
					if (pBitmap->IsMonochrome())
						WriteFITS(strFileName, pBitmap, &dlg, FF_32BITGRAYFLOAT, nullptr);
					else
						WriteFITS(strFileName, pBitmap, &dlg, FF_32BITRGBFLOAT, nullptr);
				};

				dlg.End2();
                dlg.Close();

				GetProcessingDlg(this).LoadFile(strFileName);

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

bool CStackingDlg::CheckStacking(CAllStackingTasks & tasks)
{
	bool				bResult = false;

	if (!m_Pictures.AreCheckedPictureCompatible())
		AfxMessageBox(IDS_ERROR_NOTCOMPATIBLE, MB_OK | MB_ICONSTOP);
	else if (!m_Pictures.GetNrCheckedFrames())
		AfxMessageBox(IDS_ERROR_NOTLIGHTCHECKED, MB_OK | MB_ICONSTOP);
	else
		bResult = true;

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
		bool				bContinue;
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
                GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT);

				m_BackgroundLoading.ClearList();
				if (m_Pictures.GetNrUnregisteredCheckedLightFrames())
				{
					CRegisterEngine	RegisterEngine;
					CDSSProgressDlg	dlg;

					m_Pictures.BlankCheckedItemScores();
					bContinue = RegisterEngine.RegisterLightFrames(tasks, false, &dlg);
					m_Pictures.UpdateCheckedItemScores();
					dlg.Close();
				};

                if (bContinue)
                    DoStacking(tasks);

                GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_STOP);
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
	m_Pictures.CheckAll(true);
	m_Pictures.CheckAllDarks(true);
	UpdateListInfo();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UncheckAll()
{
	m_Pictures.CheckAll(false);
	m_Pictures.CheckAllDarks(false);
	UpdateListInfo();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ClearList()
{
	if (CheckEditChanges() && CheckWorkspaceChanges())
	{
		m_Pictures.Clear();
		m_Picture.SetImg((CBitmap*)nullptr);
		m_Picture.SetImageSink(nullptr);
		m_Picture.SetButtonToolbar(nullptr);
		m_EditStarSink.SetBitmap(nullptr);
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
		bool					bContinue = true;
		CAllStackingTasks		tasks;

		m_Pictures.FillTasks(tasks);
		tasks.ResolveTasks();

		if (m_Pictures.GetNrUnregisteredCheckedLightFrames())
		{
			CRegisterEngine	RegisterEngine;
			CDSSProgressDlg	dlg;

			m_Pictures.BlankCheckedItemScores();
			bContinue = RegisterEngine.RegisterLightFrames(tasks, false, &dlg);
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

bool CStackingDlg::SaveOnClose()
{
	m_Pictures.SaveState();
	m_MRUList.saveSettings();

	return CheckEditChanges() && CheckWorkspaceChanges();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::RegisterCheckedImage()
{
	CDSSProgressDlg			dlg;
	RegisterSettings		dlgSettings;
	bool					bContinue = true;

	bool					bFound = false;

	if (m_Pictures.GetNrCheckedFrames())
	{
		//CString				strFirstLightFrame;

		//m_Pictures.GetFirstCheckedLightFrame(strFirstLightFrame);

		//dlgSettings.SetForceRegister(!m_Pictures.GetNrUnregisteredCheckedLightFrames());
		//dlgSettings.SetNoDark(!m_Pictures.GetNrCheckedDarks());
		//dlgSettings.SetNoFlat(!m_Pictures.GetNrCheckedFlats());
		//dlgSettings.SetNoOffset(!m_Pictures.GetNrCheckedOffsets());
		//dlgSettings.SetFirstLightFrame(strFirstLightFrame);

		CAllStackingTasks	tasks;
		CRect				rcSelect;

		m_Pictures.FillTasks(tasks);
		tasks.ResolveTasks();
		if (m_SelectRectSink.GetSelectRect(rcSelect))
			tasks.SetCustomRectangle(rcSelect);

		dlgSettings.setStackingTasks(&tasks);

		if (dlgSettings.exec())
		{
			double				fMinLuminancy = 0.10;
			bool				bForceRegister = false;
			LONG				lCount = 0;
			LONG				lMaxCount = 0;
			double				fPercent = 20.0;
			bool				bStackAfter = false;

			bForceRegister = dlgSettings.isForceRegister();
			
			bStackAfter = dlgSettings.isStackAfter(fPercent);

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
                    GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT);

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
						m_Picture.Invalidate(true);
					};

					dlg.Close();
				};

				if (bContinue && bStackAfter)
				{
					DoStacking(tasks, fPercent);
					dwEndTime = GetTickCount();
				};

                GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_STOP);
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

void CStackingDlg::versionInfoReceived(QNetworkReply * reply)
{
	QNetworkReply::NetworkError error = reply->error();
	if (QNetworkReply::NoError == error)
	{
		QString string(reply->read(reply->bytesAvailable()));

		if (string.startsWith("DeepSkyStackerVersion="))
		{
			QString verStr = string.section('=', 1, 1);
			int version = verStr.section('.', 0, 0).toInt();
			int release = verStr.section('.', 1, 1).toInt();
			int mod = verStr.section('.', 2, 2).toInt();

			if ((version > DSSVER_MAJOR) ||
				(version == DSSVER_MAJOR && release > DSSVER_MINOR) ||
				(version == DSSVER_MAJOR && release == DSSVER_MINOR && mod > DSSVER_SUB)
				)
			{
				CString	strNewVersion;

				strNewVersion.Format(IDS_VERSIONAVAILABLE, CString((wchar_t *)verStr.utf16()));
				m_Infos.SetTextColor(RGB(255, 0, 0));
				m_Infos.SetText(strNewVersion);
				m_Infos.SetLink(true, false);
				m_Infos.SetHyperLink("https://github.com/deepskystacker/DSS/releases/latest");
			};
		}
	}
	else
	{
		CDeepStackerDlg *	pDlg = GetDeepStackerDlg(nullptr);
		CString title;
		pDlg->GetWindowText(title);
		QMessageBox::warning(nullptr, QString::fromWCharArray(title.GetString()),
			QCoreApplication::translate("StackingDlg", "Internet version check error:\n%1")
			.arg(reply->errorString()), QMessageBox::Ok );

	}
	reply->deleteLater();
	networkManager->deleteLater();
};

void CStackingDlg::retrieveLatestVersionInfo()
{
#ifndef DSSBETA
	ZFUNCTRACE_RUNTIME();

	QSettings			settings;

	bool checkVersion = settings.value("InternetCheck", false).toBool();
	if (checkVersion)
	{
		networkManager = new QNetworkAccessManager();

		QObject::connect(networkManager, &QNetworkAccessManager::finished,
			[this](QNetworkReply * reply) { this->versionInfoReceived(reply); });

		QNetworkRequest req(QUrl("https://github.com/deepskystacker/DSS/raw/master/CurrentVersion.txt"));
		req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
		networkManager->get(req);
	}
#endif
}