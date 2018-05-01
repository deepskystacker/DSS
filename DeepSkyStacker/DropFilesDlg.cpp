// DropFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "DropFilesDlg.h"
#include "BitmapExt.h"


/* ------------------------------------------------------------------- */
// CDropFilesDlg dialog

IMPLEMENT_DYNAMIC(CDropFilesDlg, CDialog)

CDropFilesDlg::CDropFilesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDropFilesDlg::IDD, pParent)
{
	m_hDropInfo = NULL;
	m_DropType  = PICTURETYPE_UNKNOWN;
}

/* ------------------------------------------------------------------- */

CDropFilesDlg::~CDropFilesDlg()
{
}

/* ------------------------------------------------------------------- */

void CDropFilesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DROPFILESTEXT, m_Text);
	DDX_Control(pDX, IDC_LIGHTFRAMES, m_LightFrames);
	DDX_Control(pDX, IDC_DARKFRAMES, m_DarkFrames);
	DDX_Control(pDX, IDC_FLATFRAMES, m_FlatFrames);
	DDX_Control(pDX, IDC_BIASFRAMES, m_BiasFrames);
	DDX_Control(pDX, IDC_DARKFLATFRAMES, m_DarkFlatFrames);
}

/* ------------------------------------------------------------------- */

static void	GetFilesInFolder(LPCTSTR szFolder, std::vector<CString>	& vFiles)
{
	CString					strFolder = szFolder;
	WIN32_FIND_DATA			FindData;
	CString					strFileMask;
	HANDLE					hFindFiles;
	
	strFileMask = strFolder;
	strFileMask += "\\*.*";

	hFindFiles = FindFirstFile(strFileMask, &FindData);
	if (hFindFiles != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString			strFile;

			strFile = strFolder;
			strFile += "\\";
			strFile += FindData.cFileName;

			DWORD			dwAttributes;

			dwAttributes = GetFileAttributes(strFile);
			if (!(dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
				vFiles.push_back(strFile);
		}
		while (FindNextFile(hFindFiles, &FindData));

		FindClose(hFindFiles);
	};
};

/* ------------------------------------------------------------------- */

BOOL CDropFilesDlg::IsMasterFile(LPCTSTR szFile)
{
	BOOL				bResult = FALSE;
	CBitmapInfo			BitmapInfo;

	if (GetPictureInfo(szFile, BitmapInfo))
		bResult = BitmapInfo.IsMaster();

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CDropFilesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString					strText;
	LONG					lNrFiles = 0;
	std::vector<CString>	vMasters;

	m_Text.GetWindowText(strText);

	if (m_hDropInfo)
	{
		lNrFiles = DragQueryFile(m_hDropInfo, 0xFFFFFFFF, NULL, 0);
		for (LONG i = 0;i<lNrFiles;i++)
		{
			TCHAR			szFile[1+_MAX_PATH];
			CString			strFile;

			DragQueryFile(m_hDropInfo, i, szFile, sizeof(szFile)/sizeof(TCHAR));

			// If it's a folder, get all the files in the folder
			DWORD			dwAttributes;
			
			dwAttributes = GetFileAttributes(szFile);

			if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::vector<CString>	vFiles;

				GetFilesInFolder(szFile, vFiles);
				for (LONG j = 0;j<vFiles.size();j++)
				{
					if (IsMasterFile(vFiles[j]))
						vMasters.push_back(vFiles[j]);
					else
						m_vFiles.push_back(vFiles[j]);
				};
			}
			else
			{
				strFile = szFile;
				if (IsMasterFile(strFile))
					vMasters.push_back(strFile);
				else
					m_vFiles.push_back(strFile);
			};
		};

		DragFinish(m_hDropInfo);
		if (!m_vFiles.size())
			m_vFiles = vMasters;
		lNrFiles = (LONG)m_vFiles.size();
	};

	strText.Format(strText, lNrFiles);
	m_Text.SetWindowText(strText);

	m_LightFrames.SetCheck(TRUE);

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnOK()
{
	if (m_LightFrames.GetCheck())
		m_DropType = PICTURETYPE_LIGHTFRAME;
	else if (m_DarkFrames.GetCheck())
		m_DropType = PICTURETYPE_DARKFRAME;
	else if (m_DarkFlatFrames.GetCheck())
		m_DropType = PICTURETYPE_DARKFLATFRAME;
	else if (m_FlatFrames.GetCheck())
		m_DropType = PICTURETYPE_FLATFRAME;
	else
		m_DropType = PICTURETYPE_OFFSETFRAME;

	CDialog::OnOK();
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnLightFrames()
{
	m_DarkFrames.SetCheck(FALSE);
	m_FlatFrames.SetCheck(FALSE);
	m_BiasFrames.SetCheck(FALSE);
	m_DarkFlatFrames.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnDarkFrames()
{
	m_LightFrames.SetCheck(FALSE);
	m_FlatFrames.SetCheck(FALSE);
	m_BiasFrames.SetCheck(FALSE);
	m_DarkFlatFrames.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnFlatFrames()
{
	m_LightFrames.SetCheck(FALSE);
	m_DarkFrames.SetCheck(FALSE);
	m_BiasFrames.SetCheck(FALSE);
	m_DarkFlatFrames.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnDarkFlatFrames()
{
	m_LightFrames.SetCheck(FALSE);
	m_DarkFrames.SetCheck(FALSE);
	m_BiasFrames.SetCheck(FALSE);
	m_FlatFrames.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

void CDropFilesDlg::OnBiasFrames()
{
	m_LightFrames.SetCheck(FALSE);
	m_DarkFrames.SetCheck(FALSE);
	m_FlatFrames.SetCheck(FALSE);
	m_DarkFlatFrames.SetCheck(FALSE);
};

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CDropFilesDlg, CDialog)
	ON_BN_CLICKED(IDC_LIGHTFRAMES, OnLightFrames)
	ON_BN_CLICKED(IDC_DARKFRAMES, OnDarkFrames)
	ON_BN_CLICKED(IDC_FLATFRAMES, OnFlatFrames)
	ON_BN_CLICKED(IDC_BIASFRAMES, OnBiasFrames)
	ON_BN_CLICKED(IDC_DARKFLATFRAMES, OnDarkFlatFrames)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

// CDropFilesDlg message handlers
