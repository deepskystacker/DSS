// CheckAbove.cpp : implementation file
//

#include "stdafx.h"
#include "deepskystacker.h"

#include "FrameList.h"
#include "StackingEngine.h"
#include "ProgressDlg.h"
#include "TIFFUtil.h"
#include "BatchStacking.h"
#include <QSettings>

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CBatchStacking dialog

IMPLEMENT_DYNAMIC(CBatchStacking, CDialog)


CBatchStacking::CBatchStacking(CWnd* pParent /*=nullptr*/)
	: CDialog(CBatchStacking::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchStacking)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/* ------------------------------------------------------------------- */

void CBatchStacking::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchStacking)
	DDX_Control(pDX, IDC_FILELISTS, m_Lists);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CBatchStacking, CDialog)
	//{{AFX_MSG_MAP(CBatchStacking)
	ON_BN_CLICKED(IDC_ADDLISTS, OnBnClickedAddLists)
	ON_BN_CLICKED(IDC_CLEARLIST, OnBnClickedClearList)
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CBatchStacking)
    EASYSIZE(IDC_ADDLISTS,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
    EASYSIZE(IDC_CLEARLIST,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_FILELISTS,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDOK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(IDCANCEL,ES_KEEPSIZE, ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
    EASYSIZE(AFX_IDW_SIZE_BOX,ES_KEEPSIZE,ES_KEEPSIZE, ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

/////////////////////////////////////////////////////////////////////////////
// CBatchStacking message handlers

#define GRIPPIE_SQUARE_SIZE 15

/* ------------------------------------------------------------------- */

BOOL CBatchStacking::OnInitDialog()
{
	ZFUNCTRACE_RUNTIME();
	CDialog::OnInitDialog();

    CRect			rcClient;
    GetClientRect(&rcClient);

    CRect			rcGrip;


    rcGrip.right	= rcClient.right;
    rcGrip.bottom	= rcClient.bottom;
    rcGrip.left		= rcClient.right-GRIPPIE_SQUARE_SIZE;
    rcGrip.top		= rcClient.bottom-GRIPPIE_SQUARE_SIZE;

	m_Gripper.Create(WS_CHILD|WS_VISIBLE|SBS_SIZEGRIP|WS_CLIPSIBLINGS, rcGrip, this, AFX_IDW_SIZE_BOX);

	INIT_EASYSIZE;

	RestoreWindowPosition(this, "Dialogs/Batch/Position", true);

	if (mruPath)
	{
		for (size_t i = 0; i != mruPath->paths.size(); i++)
			m_Lists.AddString(mruPath->paths[i].c_str());		// fs::path::c_str return as a null terminated char* or wchar*
	}

	UpdateListBoxWidth();
	return true;
};

/* ------------------------------------------------------------------- */

void CBatchStacking::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;
};

/* ------------------------------------------------------------------- */

void CBatchStacking::OnSizing(UINT nSide, LPRECT lpRect)
{
	CDialog::OnSizing(nSide, lpRect);

	EASYSIZE_MINSIZE(390,320,nSide,lpRect);
};

/* ------------------------------------------------------------------- */

void CBatchStacking::OnBnClickedAddLists()
{
	ZFUNCTRACE_RUNTIME();
	QSettings			settings;
	CString				strBaseDirectory;

	strBaseDirectory = CString((LPCTSTR)settings.value("Folders/ListFolder", "").toString().utf16());

	CFileDialog			dlgOpen(true,
								_T(".txt"),
								nullptr,
								OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_ENABLESIZING,
								OUTPUTLIST_FILTERS,
								this);

	if (strBaseDirectory.GetLength())
		dlgOpen.m_ofn.lpstrInitialDir = strBaseDirectory.GetBuffer(_MAX_PATH);

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

			// Check that the file is not already in the list
			if (m_Lists.FindStringExact(-1, (LPCTSTR)strFile) < 0)
			{
				int				nIndex;

				nIndex = m_Lists.AddString(strFile);
				m_Lists.SetCheck(nIndex, true);
			};

			_tsplitpath(strFile, szDrive, szDir, nullptr, szExt);
			strBaseDirectory = szDrive;
			strBaseDirectory += szDir;
		};
		EndWaitCursor();

		settings.setValue("ListFolder", QString::fromWCharArray(strBaseDirectory.GetString()));
	};
	UpdateListBoxWidth();
};

/* ------------------------------------------------------------------- */

void CBatchStacking::OnBnClickedClearList()
{
	m_Lists.ResetContent();
	UpdateListBoxWidth();
};

/* ------------------------------------------------------------------- */

void CBatchStacking::UpdateListBoxWidth()
{
	int				lWidth = 0;
    CClientDC			dc(&m_Lists);
	CRect				rcClient;

	m_Lists.GetClientRect(&rcClient);

    CFont * f = m_Lists.GetFont();
    dc.SelectObject(f);

	for (int i = 0; i < m_Lists.GetCount(); i++)
	{
		CString			strText;

		m_Lists.GetText(i, strText);
		CSize			sz = dc.GetTextExtent(strText);
		sz.cx += 3 * ::GetSystemMetrics(SM_CXBORDER)+20;

		lWidth = std::max(lWidth, static_cast<int>(sz.cx));
	};

	m_Lists.SetHorizontalExtent(lWidth);
};

/* ------------------------------------------------------------------- */

bool CBatchStacking::ProcessList(LPCTSTR szList, CString& strOutputFile)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = true;
	Workspace workspace;
	CAllStackingTasks tasks;
	CFrameList list;

	workspace.Push();
	list.LoadFilesFromList(szList);
	list.FillTasks(tasks);
	tasks.ResolveTasks();

	if (!tasks.m_vStacks.empty())
	{
		bool bContinue = true;
		CDSSProgressDlg dlg;
		CStackingEngine StackingEngine;
		CString strReferenceFrame;

		// First check that the images are registered
		if (list.GetNrUnregisteredCheckedLightFrames() != 0)
		{
			CRegisterEngine	RegisterEngine;
			bContinue = RegisterEngine.RegisterLightFrames(tasks, false, &dlg);
		}

		if (bContinue)
		{
			if (list.GetReferenceFrame(strReferenceFrame))
				StackingEngine.SetReferenceFrame(strReferenceFrame);

			std::shared_ptr<CMemoryBitmap> pBitmap;
			bContinue = StackingEngine.StackLightFrames(tasks, &dlg, pBitmap);
			if (bContinue)
			{
				CString strFileName;
				CString strText;
				TCHAR				szFileName[1+_MAX_FNAME];
				_tsplitpath(szList, nullptr, nullptr, szFileName, nullptr);

				strFileName = szFileName;

				const auto iff = workspace.value("Stacking/IntermediateFileFormat").toUInt();

				if (StackingEngine.GetDefaultOutputFileName(strFileName, szList, iff == IFF_TIFF))
				{
					StackingEngine.WriteDescription(tasks, strFileName);

					strText.Format(IDS_SAVINGFINAL, strFileName);
					dlg.Start2(strText, 0);

					if (iff == IFF_TIFF)
					{
						if (pBitmap->IsMonochrome())
							WriteTIFF(strFileName, pBitmap.get(), &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE, nullptr);
						else
							WriteTIFF(strFileName, pBitmap.get(), &dlg, TF_32BITRGBFLOAT, TC_DEFLATE, nullptr);
					}
					else
					{
						if (pBitmap->IsMonochrome())
							WriteFITS(strFileName, pBitmap.get(), &dlg, FF_32BITGRAYFLOAT, nullptr);
						else
							WriteFITS(strFileName, pBitmap.get(), &dlg, FF_32BITRGBFLOAT, nullptr);
					}
					dlg.End2();
				}
				strOutputFile = strFileName;
			}
		}
		dlg.Close();
		bResult = bContinue;
	}
	workspace.Pop();

	return bResult;
}

/* ------------------------------------------------------------------- */

void CBatchStacking::OnOK()
{
	ZFUNCTRACE_RUNTIME();
	bool bContinue  = true;
	int lNrProcessedLists = 0;

	for (int i = 0; i < m_Lists.GetCount() && bContinue; i++)
	{
		CString strFile;

		if (m_Lists.GetCheck(i))
		{
			m_Lists.GetText(i, strFile);
			CString strOutputFile;

			bContinue = ProcessList(strFile, strOutputFile);
			m_Lists.SetCheck(i, false);
			if (bContinue)
			{
				CString strText;
				strText.Format(_T("->%s"), (LPCTSTR)strOutputFile);
				m_Lists.InsertString(i, strText);
				m_Lists.SetCheck(i, false);
				m_Lists.Enable(i, false);
				m_Lists.DeleteString(i+1);
				UpdateListBoxWidth();
			};
			lNrProcessedLists++;
		};
	};

	if (!lNrProcessedLists)
	{
		SaveWindowPosition(this, "Dialogs/Batch/Position");
		CDialog::OnOK();
	};
}

/* ------------------------------------------------------------------- */
