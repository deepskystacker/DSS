// FileProperty.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "PictureListCtrl.h"
#include "FileProperty.h"

const LONG				MINISOSPEED = 25;
const LONG				MINEXPOSURE = 1;
const LONG				MAXISOSPEED = 10000;
const LONG				MAXEXPOSURE = 5000;

/* ------------------------------------------------------------------- */
// CFileProperty dialog

IMPLEMENT_DYNAMIC(CFileProperty, CDialog)

/* ------------------------------------------------------------------- */

CFileProperty::CFileProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CFileProperty::IDD, pParent)
{

}

/* ------------------------------------------------------------------- */

CFileProperty::~CFileProperty()
{
}

/* ------------------------------------------------------------------- */

void CFileProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILENAME, m_FileName);
	DDX_Control(pDX, IDC_DATETIME, m_DateTime);
	DDX_Control(pDX, IDC_SIZES, m_Sizes);
	DDX_Control(pDX, IDC_DEPTH, m_Depth);
	DDX_Control(pDX, IDC_INFO, m_Info);
	DDX_Control(pDX, IDC_CFA, m_CFA);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_ISOSPEED, m_ISOSpeed);
	DDX_Control(pDX, IDC_EXPOSURE, m_Exposure);
	DDX_Control(pDX, IDOK, m_OK);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CFileProperty, CDialog)
	ON_CBN_SELCHANGE(IDC_TYPE, &CFileProperty::OnCbnSelchangeType)
	ON_CBN_EDITUPDATE(IDC_ISOSPEED, &CFileProperty::OnCbnEditupdateIsospeed)
	ON_EN_CHANGE(IDC_EXPOSURE, &CFileProperty::OnEnChangeExposure)
	ON_BN_CLICKED(IDOK, &CFileProperty::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_ISOSPEED, &CFileProperty::OnCbnSelchangeIsospeed)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

BOOL CFileProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitControls();

	return TRUE;
};

/* ------------------------------------------------------------------- */

void CFileProperty::InitControls()
{
	BOOL				bFirst = TRUE;
	CString				strFileName;
	CString				strDateTime;
	CString				strSizes;
	CString				strDepth;
	CString				strInfo;
	CString				strCFA;
	LONG				lISOSpeed = -1;
	CString				strISOSpeed;
	double				fExposure = -1;
	CString				strExposure;
	PICTURETYPE			PictureType = PICTURETYPE_UNKNOWN;

	for (LONG i = 0;i<m_vpBitmaps.size();i++)
	{
		if (m_vpBitmaps[i])
		{
			if (bFirst)
			{
				strFileName = m_vpBitmaps[i]->m_strFileName;
				PictureType = m_vpBitmaps[i]->m_PictureType;
				strDateTime = m_vpBitmaps[i]->m_strDateTime;
				strSizes    = m_vpBitmaps[i]->m_strSizes;
				strDepth    = m_vpBitmaps[i]->m_strDepth;
				strInfo		= m_vpBitmaps[i]->m_strInfos;
				strCFA		= m_vpBitmaps[i]->m_strCFA;
				lISOSpeed	= m_vpBitmaps[i]->m_lISOSpeed;
				fExposure	= m_vpBitmaps[i]->m_fExposure;
				bFirst = FALSE;
			}
			else
			{
				if (strFileName != m_vpBitmaps[i]->m_strFileName)
					strFileName.Format(IDS_MULTIPLEFILESELECTED, m_vpBitmaps.size());
				if (PictureType != m_vpBitmaps[i]->m_PictureType)
					PictureType = PICTURETYPE_UNKNOWN;
				if (strDateTime != m_vpBitmaps[i]->m_strDateTime)
					strDateTime = "-";
				if (strSizes != m_vpBitmaps[i]->m_strSizes)
					strSizes = "-";
				if (strDepth != m_vpBitmaps[i]->m_strDepth)
					strDepth = "-";
				if (strInfo != m_vpBitmaps[i]->m_strInfos)
					strInfo = "-";
				if (strCFA != m_vpBitmaps[i]->m_strCFA)
					strCFA = "-";
				if (lISOSpeed != m_vpBitmaps[i]->m_lISOSpeed)
					lISOSpeed = 0;
				if (fExposure != m_vpBitmaps[i]->m_fExposure)
					fExposure = 0;
			};
		};
	};

	m_FileName.SetWindowText(strFileName);
	m_DateTime.SetWindowText(strDateTime);
	m_Sizes.SetWindowText(strSizes);
	m_Depth.SetWindowText(strDepth);
	m_Info.SetWindowText(strInfo);
	m_CFA.SetWindowText(strCFA);

	switch (PictureType)
	{
	case PICTURETYPE_LIGHTFRAME :
		m_Type.SetCurSel(0);
		break;
	case PICTURETYPE_DARKFRAME :
		m_Type.SetCurSel(1);
		break;
	case PICTURETYPE_FLATFRAME :
		m_Type.SetCurSel(2);
		break;
	case PICTURETYPE_DARKFLATFRAME :
		m_Type.SetCurSel(3);
		break;
	case PICTURETYPE_OFFSETFRAME :
		m_Type.SetCurSel(4);
		break;
	default :
		m_Type.SetCurSel(-1);
		break;
	};

	if (lISOSpeed)
		strISOSpeed.Format(_T("%ld"), lISOSpeed);
	m_ISOSpeed.SetWindowText(strISOSpeed);

	if (fExposure>=1)
	{
		LONG			lExposure = fExposure;
		strExposure.Format(_T("%ld"), lExposure);
		m_Exposure.SetWindowText(strExposure);
	};

	m_bChangeType	  = FALSE;
	m_bChangeISOSpeed = FALSE;
	m_bChangeExposure = FALSE;
};

/* ------------------------------------------------------------------- */
// CFileProperty message handlers

void CFileProperty::OnCbnSelchangeType()
{
	m_bChangeType	  = TRUE;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::OnCbnSelchangeIsospeed()
{
	m_bChangeISOSpeed = TRUE;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::OnCbnEditupdateIsospeed()
{
	m_bChangeISOSpeed = TRUE;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::OnEnChangeExposure()
{
	m_bChangeExposure = TRUE;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::UpdateControls()
{
	BOOL			bOk = TRUE;

	if (m_bChangeISOSpeed)
	{
		CString		strISOSpeed;

		m_ISOSpeed.GetWindowText(strISOSpeed);
		if (strISOSpeed.GetLength())
		{
			LONG	lISOSpeed = _ttol(strISOSpeed);

			if ((lISOSpeed < MINISOSPEED) || (lISOSpeed > MAXISOSPEED))
				bOk = FALSE;
		};
	};

	if (m_bChangeExposure)
	{
		CString		strExposure;

		m_Exposure.GetWindowText(strExposure);
		if (strExposure.GetLength())
		{
			LONG		lExposure = _ttol(strExposure);

			if ((lExposure <MINEXPOSURE) || (lExposure > MAXEXPOSURE))
				bOk = FALSE;
		};
	};

	m_OK.EnableWindow(bOk);
};

/* ------------------------------------------------------------------- */

void CFileProperty::ApplyChanges()
{
	LONG				i = 0;

	if (m_bChangeType)
	{
		PICTURETYPE			PictureType = PICTURETYPE_UNKNOWN;

		switch (m_Type.GetCurSel())
		{
		case 0 :
			PictureType = PICTURETYPE_LIGHTFRAME;
			break;
		case 1 :
			PictureType = PICTURETYPE_DARKFRAME;
			break;
		case 2 :
			PictureType = PICTURETYPE_FLATFRAME;
			break;
		case 3 :
			PictureType = PICTURETYPE_DARKFLATFRAME;
			break;
		case 4 :
			PictureType = PICTURETYPE_OFFSETFRAME;
			break;
		};

		if (PictureType != PICTURETYPE_UNKNOWN)
		{
			for (i = 0;i<m_vpBitmaps.size();i++)
				m_vpBitmaps[i]->m_PictureType = PictureType;
		};
	};

	if (m_bChangeISOSpeed)
	{
		CString		strISOSpeed;

		m_ISOSpeed.GetWindowText(strISOSpeed);
		if (strISOSpeed.GetLength())
		{
			LONG	lISOSpeed = _ttol(strISOSpeed);

			if ((lISOSpeed >= MINISOSPEED) && (lISOSpeed <= MAXISOSPEED))
			{
				for (i = 0;i<m_vpBitmaps.size();i++)
					m_vpBitmaps[i]->m_lISOSpeed = lISOSpeed;
			};
		};
	};

	if (m_bChangeExposure)
	{
		CString		strExposure;

		m_Exposure.GetWindowText(strExposure);
		if (strExposure.GetLength())
		{
			LONG		lExposure = _ttol(strExposure);

			if ((lExposure >= MINEXPOSURE) && (lExposure <= MAXEXPOSURE))
			{
				for (i = 0;i<m_vpBitmaps.size();i++)
					m_vpBitmaps[i]->m_fExposure = lExposure;
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CFileProperty::OnBnClickedOk()
{
	ApplyChanges();
	OnOK();
}

/* ------------------------------------------------------------------- */
