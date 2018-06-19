// About.cpp : implementation file
//

#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "About.h"
#include "Registry.h"


// CAbout dialog

IMPLEMENT_DYNAMIC(CAbout, CDialog)

CAbout::CAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CAbout::IDD, pParent)
{

}

CAbout::~CAbout()
{
}

void CAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CHECKVERSION, m_CheckVersion);
	DDX_Control(pDX, IDC_LANGUAGE, m_Language);
	DDX_Control(pDX, IDC_CREDITS, m_Credits);
	DDX_Control(pDX, IDC_LANGCREDITS, m_LanguageCredits);
}


BEGIN_MESSAGE_MAP(CAbout, CDialog)
END_MESSAGE_MAP()


// CAbout message handlers

/* ------------------------------------------------------------------- */

BOOL CAbout::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString 				strHTML;
	CString 				strText;
	CString 				strMask;

	strHTML.Format(_T("<body link=#0000ff></body>"));

	strHTML += _T("<img valign=top align=right src=\"RES:LOGO.PNG\">");

	strText.Format(IDS_ABOUT_DSS, VERSION_DEEPSKYSTACKER);
	strText.Replace(_T("\n"), _T("<BR>"));
	strText += _T("<BR>");
	strText += _T(DSSVER_COPYRIGHT);
	strHTML += _T("<b>")+strText+_T("</b><br>");
	strText.LoadString(IDS_ABOUT_DSS_LINK);
	strMask.Format(_T("<a href=\"%s\">%s</a><br><br>"), (LPCTSTR)strText, (LPCTSTR)strText);
	strHTML += strMask;


	strText.Format(IDS_ABOUT_DCRAW, VERSION_DCRAW);
	strText.Replace(_T("\n"), _T("<BR>"));
	strHTML += strText+_T("<br>");
	strText.LoadString(IDS_ABOUT_DCRAW_LINK);
	strMask.Format(_T("<a href=\"%s\">%s</a><br><br>"), (LPCTSTR)strText, (LPCTSTR)strText);
	strHTML += strMask;

	strText.Format(IDS_ABOUT_TIFF, VERSION_LIBTIFF);
	strText.Replace(_T("\n"), _T("<BR>"));
	strHTML += strText+_T("<br>");
	strText.LoadString(IDS_ABOUT_TIFF_LINK);
	strMask.Format(_T("<a href=\"%s\">%s</a><br><br>"), (LPCTSTR)strText, (LPCTSTR)strText);
	strHTML += strMask;

	strText.Format(IDS_ABOUT_FITS, VERSION_CFITSIO);
	strText.Replace(_T("\n"), _T("<BR>"));
	strHTML += strText+_T("<br>");
	strText.LoadString(IDS_ABOUT_FITS_LINK);
	strMask.Format(_T("<a href=\"%s\">%s</a><br><br>"), (LPCTSTR)strText, (LPCTSTR)strText);
	strHTML += strMask;

	strText.Format(IDS_ABOUT_QHTM);
	strText.Replace(_T("\n"), _T("<BR>"));
	strHTML += strText+_T("<br>");
	strText.LoadString(IDS_ABOUT_QHTM_LINK);
	strMask.Format(_T("<a href=\"%s\">%s</a>"), (LPCTSTR)strText, (LPCTSTR)strText);
	strHTML += strMask;

	m_Credits.SetWindowText(strHTML);

	strHTML.Format(_T("<body link=#0000ff></body>"));
	strHTML += _T("<img valign=center src=\"RES:SPANISH.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_SPANISH);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_SPANISH_LINK);
	strMask.Format(_T("<a href=\"http://espacioprofundo.com.ar/\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:CZECH.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_CZECH);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_CZECH_LINK);
	strMask.Format(_T("<a href=\"mailto:lukavsky.premysl@tiscali.cz\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:ITALIAN.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_ITALIAN);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_ITALIAN_LINK);
	strMask.Format(_T("<a href=\"http://www.astrofilipc.it\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:CATALAN.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_CATALAN);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_CATALAN_LINK);
	strMask.Format(_T("<a href=\"http://astrosurf.com/pruneda/\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:GERMAN.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_GERMAN);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_GERMAN_LINK);
	strMask.Format(_T("<a href=\"mailto:kasakow@arcor.de\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:DUTCH.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_DUTCH);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_DUTCH_LINK1);
	strMask.Format(_T("<a href=\"http://www.geocities.com/me_gielis/\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T(" ");
	strText.LoadString(IDS_AND);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_DUTCH_LINK2);
	strHTML += strText + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:TAIWAN.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_CHINESET);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_CHINESET_LINK);
	strMask.Format(_T("<a href=\"http://www.aoc.nrao.edu/~whwang/\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:PORTUGAL.PNG\">&nbsp;<img valign=center src=\"RES:BRAZIL.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_PORTUGUESE);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_PORTUGUESE_LINK);
	strMask.Format(_T("<a href=\"http://www.astrofotografia.biz\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:ROMANIAN.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_ROMANIAN);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_ROMANIAN_LINK);
	strMask.Format(_T("<a href=\"http://www.astronomy.ro\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:RUSSIAN.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_RUSSIAN);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_RUSSIAN_LINK);
	strMask.Format(_T("<a href=\"http://oleg.milantiev.com\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	strHTML += _T("<img valign=center src=\"RES:TURKISH.PNG\">&nbsp;&nbsp;");
	strText.LoadString(IDS_ABOUT_LANG_TURKISH);
	strHTML += strText + _T(" ");
	strText.LoadString(IDS_ABOUT_LANG_TURKISH_LINK);
	strMask.Format(_T("<a href=\"http://www.physics.metu.edu.tr\">%s</a>"), (LPCTSTR)strText);
	strHTML += strMask + _T("<br>");

	m_LanguageCredits.SetWindowText(strHTML);

	CRegistry			reg;
	DWORD				bCheckVersion = 0;

	reg.LoadKey(REGENTRY_BASEKEY, _T("InternetCheck"), bCheckVersion);

	m_CheckVersion.SetCheck(bCheckVersion == 2);

	CString				strLanguage;
	reg.LoadKey(REGENTRY_BASEKEY, _T("Language"), strLanguage);

	if (!strLanguage.CompareNoCase(_T("FR")))
		m_Language.SetCurSel(2);
	else if (!strLanguage.CompareNoCase(_T("EN")))
		m_Language.SetCurSel(1);
	else if (!strLanguage.CompareNoCase(_T("ES")))
		m_Language.SetCurSel(3);
	else if (!strLanguage.CompareNoCase(_T("CZ")))
		m_Language.SetCurSel(5);
	else if (!strLanguage.CompareNoCase(_T("IT")))
		m_Language.SetCurSel(4);
	else if (!strLanguage.CompareNoCase(_T("CAT")))
		m_Language.SetCurSel(6);
	else if (!strLanguage.CompareNoCase(_T("DE")))
		m_Language.SetCurSel(7);
	else if (!strLanguage.CompareNoCase(_T("NL")))
		m_Language.SetCurSel(8);
	else if (!strLanguage.CompareNoCase(_T("CN")))
		m_Language.SetCurSel(9);
	else if (!strLanguage.CompareNoCase(_T("PTB")))
		m_Language.SetCurSel(10);
	else if (!strLanguage.CompareNoCase(_T("RO")))
		m_Language.SetCurSel(11);
	else if (!strLanguage.CompareNoCase(_T("RU")))
		m_Language.SetCurSel(12);
	else if (!strLanguage.CompareNoCase(_T("TR")))
		m_Language.SetCurSel(13);
	else
		m_Language.SetCurSel(0);

	return TRUE;  
}

/* ------------------------------------------------------------------- */

void CAbout::OnOK()
{
	DWORD				bCheckVersion;
	CRegistry			reg;

	if (m_CheckVersion.GetCheck())
		bCheckVersion = 2;
	else
		bCheckVersion = 1;

	reg.SaveKey(REGENTRY_BASEKEY, _T("InternetCheck"), bCheckVersion);

	switch (m_Language.GetCurSel())
	{
	case 0 :
		reg.DeleteKey(REGENTRY_BASEKEY, _T("Language"));
		break;
	case 1 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("EN"));
		break;
	case 2 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("FR"));
		break;
	case 3 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("ES"));
		break;
	case 4 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("IT"));
		break;
	case 5 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("CZ"));
		break;
	case 6 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("CAT"));
		break;
	case 7 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("DE"));
		break;
	case 8 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("NL"));
		break;
	case 9 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("CN"));
		break;
	case 10 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("PTB"));
		break;
	case 11 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("RO"));
		break;
	case 12 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("RU"));
		break;
	case 13 :
		reg.SaveKey(REGENTRY_BASEKEY, _T("Language"), _T("TR"));
		break;
	};

	CDialog::OnOK();
}

/* ------------------------------------------------------------------- */
