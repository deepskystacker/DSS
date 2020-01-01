// ResultParameters.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "ResultParameters.h"
#include "StackSettings.h"
#include "DSSTools.h"
#include "DSSProgress.h"

// CResultParameters dialog

IMPLEMENT_DYNAMIC(CResultParameters, CChildPropertyPage)

/* ------------------------------------------------------------------- */

CResultParameters::CResultParameters()
	: CChildPropertyPage(CResultParameters::IDD)
{
	m_bFirstActivation = TRUE;
	m_ResultMode = SM_NORMAL;
	m_lDrizzle = 1;
	m_bAlignChannels = FALSE;
    m_bEnableCustom = FALSE;
    m_bUseCustom = FALSE;
}

/* ------------------------------------------------------------------- */

CResultParameters::~CResultParameters()
{
}

/* ------------------------------------------------------------------- */

void CResultParameters::DoDataExchange(CDataExchange* pDX)
{
	CChildPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NORMALRESULT, m_Normal);
	DDX_Control(pDX, IDC_MOSAICRESULT, m_Mosaic);
	DDX_Control(pDX, IDC_INTERSECTIONMODE, m_Intersection);
	DDX_Control(pDX, IDC_CUSTOMRECTANGLE, m_Custom);
	DDX_Control(pDX, IDC_MODEPREVIEW, m_Preview);
	DDX_Control(pDX, IDC_ENABLE2XDRIZZLE, m_Drizzlex2);
	DDX_Control(pDX, IDC_ENABLE3XDRIZZLE, m_Drizzlex3);
	DDX_Control(pDX, IDC_ALIGNCHANNELS, m_AlignChannels);
	DDX_Control(pDX, IDC_MODE_TEXT, m_ModeText);
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CResultParameters, CChildPropertyPage)
	ON_BN_CLICKED(IDC_NORMALRESULT, &CResultParameters::OnBnClickedNormal)
	ON_BN_CLICKED(IDC_INTERSECTIONMODE, &CResultParameters::OnBnClickedIntersection)
	ON_BN_CLICKED(IDC_MOSAICRESULT, &CResultParameters::OnBnClickedMosaic)
	ON_BN_CLICKED(IDC_CUSTOMRECTANGLE, &CResultParameters::OnBnClickedCustom)
	ON_BN_CLICKED(IDC_ENABLE2XDRIZZLE, &CResultParameters::OnBnClicked2xDrizzle)
	ON_BN_CLICKED(IDC_ENABLE3XDRIZZLE, &CResultParameters::OnBnClicked3xDrizzle)
	ON_BN_CLICKED(IDC_ALIGNCHANNELS, &CResultParameters::OnBnClickedAlignchannels)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CResultParameters::UpdateControls()
{
	CStackSettings *	pDialog = dynamic_cast<CStackSettings *>(GetParent()->GetParent());
	CString				strText;

	if (m_bUseCustom)
	{
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CUSTOMMODE)));
		m_ModeText.SetWindowText(_T(""));
	}
	else if (m_ResultMode == SM_MOSAIC)
	{
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_MOSAICMODE)));
		strText.LoadString(IDS_STACKINGMODE_MOSAIC);
		m_ModeText.SetWindowText(strText);
	}
	else if (m_ResultMode == SM_INTERSECTION)
	{
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_INTERSECTIONMODE)));
		strText.LoadString(IDS_STACKINGMODE_INTERSECTION);
		m_ModeText.SetWindowText(strText);
	}
	else
	{
		m_Preview.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_NORMALMODE)));
		strText.LoadString(IDS_STACKINGMODE_NORMAL);
		m_ModeText.SetWindowText(strText);
	};

	m_Drizzlex2.SetCheck(m_lDrizzle==2);
	m_Drizzlex3.SetCheck(m_lDrizzle>2);

	m_AlignChannels.SetCheck(m_bAlignChannels);

	if (pDialog)
		pDialog->UpdateControls();
};

/* ------------------------------------------------------------------- */

BOOL CResultParameters::OnSetActive()
{
	if (m_bFirstActivation)
	{
		m_Custom.EnableWindow(FALSE);
		m_Normal.SetCheck(m_ResultMode==SM_NORMAL);
		m_Mosaic.SetCheck(m_ResultMode==SM_MOSAIC);
		m_Intersection.SetCheck(m_ResultMode==SM_INTERSECTION);
		if (m_lDrizzle == 2)
			m_Drizzlex2.SetCheck(TRUE);
		else if (m_lDrizzle > 2)
		{
			m_Drizzlex3.SetCheck(TRUE);
			m_lDrizzle = 3;
		}
		else
			m_lDrizzle = 1;
		UpdateControls();
		m_bFirstActivation = FALSE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */
// CResultParameters message handlers

void CResultParameters::OnBnClickedNormal()
{
	if (m_Normal.GetCheck())
	{
		m_Mosaic.SetCheck(FALSE);
		m_Custom.SetCheck(FALSE);
		m_Intersection.SetCheck(FALSE);
		m_ResultMode	= SM_NORMAL;
		m_bUseCustom	= FALSE;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CResultParameters::OnBnClickedMosaic()
{
	if (m_Mosaic.GetCheck())
	{
		m_Normal.SetCheck(FALSE);
		m_Custom.SetCheck(FALSE);
		m_Intersection.SetCheck(FALSE);
		m_ResultMode    = SM_MOSAIC;
		m_bUseCustom	= FALSE;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CResultParameters::OnBnClickedCustom()
{
	if (m_Custom.GetCheck())
	{
		m_Normal.SetCheck(FALSE);
		m_Mosaic.SetCheck(FALSE);
		m_Intersection.SetCheck(FALSE);
		m_ResultMode    = SM_NORMAL;
		m_bUseCustom	= TRUE;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CResultParameters::OnBnClickedIntersection()
{
	if (m_Intersection.GetCheck())
	{
		m_Normal.SetCheck(FALSE);
		m_Mosaic.SetCheck(FALSE);
		m_Custom.SetCheck(FALSE);
		m_ResultMode    = SM_INTERSECTION;
		m_bUseCustom	= FALSE;
		UpdateControls();
	};
}

/* ------------------------------------------------------------------- */

void CResultParameters::OnBnClicked2xDrizzle()
{
	if (m_Drizzlex2.GetCheck())
	{
		m_Drizzlex3.SetCheck(FALSE);
		m_lDrizzle = 2;
	}
	else
		m_lDrizzle = 1;
};

/* ------------------------------------------------------------------- */

void CResultParameters::OnBnClicked3xDrizzle()
{
	if (m_Drizzlex3.GetCheck())
	{
		m_Drizzlex2.SetCheck(FALSE);
		m_lDrizzle = 3;
	}
	else
		m_lDrizzle = 1;
};

/* ------------------------------------------------------------------- */

void CResultParameters::OnBnClickedAlignchannels()
{
	m_bAlignChannels = m_AlignChannels.GetCheck();
}

/* ------------------------------------------------------------------- */
