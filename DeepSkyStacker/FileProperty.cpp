// FileProperty.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "PictureListCtrl.h"
#include "FileProperty.h"

constexpr int				MINISOSPEED = 25;
constexpr int				MINEXPOSURE = 1;
constexpr int				MAXISOSPEED = 10000;
constexpr int				MAXEXPOSURE = 5000;

/* ------------------------------------------------------------------- */
// CFileProperty dialog

IMPLEMENT_DYNAMIC(CFileProperty, CDialog)

/* ------------------------------------------------------------------- */

CFileProperty::CFileProperty(CWnd* pParent /*=nullptr*/)
	: CDialog(CFileProperty::IDD, pParent)
{
    m_bChangeType = false;
    m_bChangeISOSpeed = false;
    m_bChangeExposure = false;
    m_imageList = nullptr;
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

	return true;
};

/* ------------------------------------------------------------------- */

void CFileProperty::InitControls()
{
	bool				bFirst = true;
	CString				strFileName;
	CString				strDateTime;
	CString				strSizes;
	CString				strDepth;
	CString				strInfo;
	CString				strCFA;
	int				lISOSpeed = -1;
	CString				strISOSpeed;
	double				fExposure = -1;
	CString				strExposure;
	PICTURETYPE			PictureType = PICTURETYPE_UNKNOWN;

	for (auto const& pair : m_bitmaps)
	{
		if (ListBitMap* bitmap = pair.second)
		{
			if (bFirst)
			{
				strFileName = bitmap->m_strFileName;
				PictureType = bitmap->m_PictureType;
				strDateTime = bitmap->m_strDateTime;
				strSizes    = bitmap->m_strSizes;
				strDepth    = bitmap->m_strDepth;
				strInfo		= bitmap->m_strInfos;
				strCFA		= bitmap->m_strCFA;
				lISOSpeed	= bitmap->m_lISOSpeed;
				fExposure	= bitmap->m_fExposure;
				bFirst = false;
			}
			else
			{
				if (strFileName != bitmap->m_strFileName)
					strFileName.Format(IDS_MULTIPLEFILESELECTED, m_bitmaps.size());
				if (PictureType != bitmap->m_PictureType)
					PictureType = PICTURETYPE_UNKNOWN;
				if (strDateTime != bitmap->m_strDateTime)
					strDateTime = "-";
				if (strSizes != bitmap->m_strSizes)
					strSizes = "-";
				if (strDepth != bitmap->m_strDepth)
					strDepth = "-";
				if (strInfo != bitmap->m_strInfos)
					strInfo = "-";
				if (strCFA != bitmap->m_strCFA)
					strCFA = "-";
				if (lISOSpeed != bitmap->m_lISOSpeed)
					lISOSpeed = 0;
				if (fExposure != bitmap->m_fExposure)
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
		auto lExposure = static_cast<int>(fExposure);
		strExposure.Format(_T("%ld"), lExposure);
		m_Exposure.SetWindowText(strExposure);
	};

	m_bChangeType	  = false;
	m_bChangeISOSpeed = false;
	m_bChangeExposure = false;
};

/* ------------------------------------------------------------------- */
// CFileProperty message handlers

void CFileProperty::OnCbnSelchangeType()
{
	m_bChangeType	  = true;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::OnCbnSelchangeIsospeed()
{
	m_bChangeISOSpeed = true;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::OnCbnEditupdateIsospeed()
{
	m_bChangeISOSpeed = true;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::OnEnChangeExposure()
{
	m_bChangeExposure = true;
	UpdateControls();
}

/* ------------------------------------------------------------------- */

void CFileProperty::UpdateControls()
{
	bool			bOk = true;

	if (m_bChangeISOSpeed)
	{
		CString		strISOSpeed;

		m_ISOSpeed.GetWindowText(strISOSpeed);
		if (strISOSpeed.GetLength())
		{
			auto	lISOSpeed = _ttol(strISOSpeed);

			if ((lISOSpeed < MINISOSPEED) || (lISOSpeed > MAXISOSPEED))
				bOk = false;
		};
	};

	if (m_bChangeExposure)
	{
		CString		strExposure;

		m_Exposure.GetWindowText(strExposure);
		if (strExposure.GetLength())
		{
			auto		lExposure = _ttol(strExposure);

			if ((lExposure < MINEXPOSURE) || (lExposure > MAXEXPOSURE))
				bOk = false;
		};
	};

	m_OK.EnableWindow(bOk);
};

/* ------------------------------------------------------------------- */

void CFileProperty::ApplyChanges()
{
	if (m_bChangeType)
	{
		PICTURETYPE PictureType = PICTURETYPE_UNKNOWN;

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
            for (auto const& pair : m_bitmaps)
                m_imageList->ChangePictureType(pair.first, PictureType);
		};
	};

	if (m_bChangeISOSpeed)
	{
		CString		strISOSpeed;

		m_ISOSpeed.GetWindowText(strISOSpeed);
		if (strISOSpeed.GetLength())
		{
			auto	lISOSpeed = _ttol(strISOSpeed);

			if ((lISOSpeed >= MINISOSPEED) && (lISOSpeed <= MAXISOSPEED))
			{
                for (auto const& pair : m_bitmaps)
                    pair.second->m_lISOSpeed = lISOSpeed;
			};
		};
	};

	if (m_bChangeExposure)
	{
		CString		strExposure;

		m_Exposure.GetWindowText(strExposure);
		if (strExposure.GetLength())
		{
			auto		lExposure = _ttol(strExposure);

			if ((lExposure >= MINEXPOSURE) && (lExposure <= MAXEXPOSURE))
			{
                for (auto const& pair : m_bitmaps)
                    pair.second->m_fExposure = lExposure;
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
