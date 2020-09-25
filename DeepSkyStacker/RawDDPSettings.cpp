// RawDDPSettings.cpp : implementation file
//

#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <afx.h>
#include <afxcmn.h>
#include <afxcview.h>
#include <afxwin.h>


//#include <afxdlgs.h>

#include <ZExcept.h>
#include <Ztrace.h>

#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QSettings>
#include <QShowEvent>
#include <QString>

#include "RawDDPSettings.h"
#include "ui/ui_RawDDPSettings.h"

extern bool		g_bShowRefStars;

#include "DSSCommon.h"
#include "commonresource.h"
#include "BitmapExt.h"
#include "DeepStackerDlg.h"
#include "ProgressDlg.h"
#include "RegisterEngine.h"
#include "StackingDlg.h"
#include "StackSettings.h"

#include "Workspace.h"

class CDSLR
{
public:
	QString				m_strName;
	CFATYPE				m_CFAType;

private:
	void	CopyFrom(const CDSLR & cd)
	{
		m_strName = cd.m_strName;
		m_CFAType = cd.m_CFAType;
	};

public:
	CDSLR(QString name, CFATYPE CFAType) :
		m_strName(name),
		m_CFAType(CFAType)
	{
	};

	CDSLR(const CDSLR & cd)
	{
		CopyFrom(cd);
	};

	virtual ~CDSLR()
	{
	};

	const CDSLR & operator = (const CDSLR & cd)
	{
		CopyFrom(cd);
		return *this;
	};
};


void	RawDDPSettings::fillDSLRList(std::vector<CDSLR> & vDSLRs)
{
	ZFUNCTRACE_RUNTIME();
	// Fill the four generic DSLR bayer pattern

	vDSLRs.emplace_back(tr("Generic RGGB"), CFATYPE_RGGB);
	vDSLRs.emplace_back(tr("Generic BGGR"), CFATYPE_BGGR);
	vDSLRs.emplace_back(tr("Generic GBRG"), CFATYPE_GBRG);
	vDSLRs.emplace_back(tr("Generic GRBG"), CFATYPE_GRBG);

	QString string(tr("Generic"));
	vDSLRs.emplace_back(string + " CYMG (01)", CFATYPE_CYGMCYMG);
	vDSLRs.emplace_back(string + " CYMG (02)", CFATYPE_GMCYMGCY);
	vDSLRs.emplace_back(string + " CYMG (03)", CFATYPE_CYMGCYGM);
	vDSLRs.emplace_back(string + " CYMG (04)", CFATYPE_MGCYGMCY);	//*
	vDSLRs.emplace_back(string + " CYMG (05)", CFATYPE_GMYCGMCY);
	vDSLRs.emplace_back(string + " CYMG (06)", CFATYPE_YCGMCYGM);	//*
	vDSLRs.emplace_back(string + " CYMG (07)", CFATYPE_GMCYGMYC);
	vDSLRs.emplace_back(string + " CYMG (08)", CFATYPE_CYGMYCGM);
	vDSLRs.emplace_back(string + " CYMG (09)", CFATYPE_YCGMYCMG);
	vDSLRs.emplace_back(string + " CYMG (10)", CFATYPE_GMYCMGYC);
	vDSLRs.emplace_back(string + " CYMG (11)", CFATYPE_YCMGYCGM);
	vDSLRs.emplace_back(string + " CYMG (12)", CFATYPE_MGYCGMYC);
	vDSLRs.emplace_back(string + " CYMG (13)", CFATYPE_MGYCMGCY);
	vDSLRs.emplace_back(string + " CYMG (14)", CFATYPE_YCMGCYMG);
	vDSLRs.emplace_back(string + " CYMG (15)", CFATYPE_MGCYMGYC);
	vDSLRs.emplace_back(string + " CYMG (16)", CFATYPE_CYMGYCMG);

	vDSLRs.emplace_back("AVT F-145C", CFATYPE_RGGB);
	vDSLRs.emplace_back("AVT F-201C", CFATYPE_RGGB);
	vDSLRs.emplace_back("AVT F-510C", CFATYPE_RGGB);
	vDSLRs.emplace_back("AVT F-810C", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO EX-P505", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO EX-P600", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO EX-P700", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO EX-S100", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO EX-Z50", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO EX-Z55", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO QV-2000UX", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO QV-3*00EX", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO QV-4000", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO QV-5700", CFATYPE_RGGB);
	vDSLRs.emplace_back("CASIO QV-R51", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 10D", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 20D", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 300D DIGITAL", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 30D", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 350D DIGITAL", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 40D", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 400D DIGITAL", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 50D", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 5D", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS 5D Mk II", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS D2000C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Canon EOS D30", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS D60", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS DIGITAL REBEL", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS DIGITAL REBEL XT", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS DIGITAL REBEL XTi", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS Kiss Digital", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS Kiss Digital N", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS-1D", CFATYPE_GRBG);
	vDSLRs.emplace_back("Canon EOS-1D Mark II", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS-1D Mark II N", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon EOS-1DS", CFATYPE_GRBG);
	vDSLRs.emplace_back("Canon EOS-1Ds Mark II", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot G2", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot G3", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot G5", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot G6", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot Pro1", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot S30", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot S40", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot S45", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot S50", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot S60", CFATYPE_RGGB);
	vDSLRs.emplace_back("Canon PowerShot S70", CFATYPE_RGGB);
	vDSLRs.emplace_back("Contax N Digital", CFATYPE_GRBG);
	vDSLRs.emplace_back("Creative PC-CAM 600", CFATYPE_GBRG);
	vDSLRs.emplace_back("EPSON R-D1", CFATYPE_RGGB);
	vDSLRs.emplace_back("EPSON R-D1s", CFATYPE_RGGB);
	vDSLRs.emplace_back("FUJIFILM FinePix E550", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix E900", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix F700", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix F710", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S20Pro", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S2Pro", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S3Pro", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S5000", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S5100", CFATYPE_RGGB);
	vDSLRs.emplace_back("FUJIFILM FinePix S5200", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S5600", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S7000", CFATYPE_GBRG);
	vDSLRs.emplace_back("FUJIFILM FinePix S9500", CFATYPE_GBRG);
	vDSLRs.emplace_back("Foculus 531C", CFATYPE_GBRG);
	vDSLRs.emplace_back("ISG 2020x1520", CFATYPE_GRBG);
	vDSLRs.emplace_back("Imacon Ixpress 16-Mp", CFATYPE_GRBG);
	vDSLRs.emplace_back("Imacon Ixpress 22-Mp", CFATYPE_GRBG);
	vDSLRs.emplace_back("Imacon Ixpress 22-Mp", CFATYPE_RGGB);
	vDSLRs.emplace_back("Imacon Ixpress 39-Mp", CFATYPE_RGGB);
	vDSLRs.emplace_back("KODAK DCS420", CFATYPE_GRBG);
	vDSLRs.emplace_back("KODAK DCS460", CFATYPE_GRBG);
	vDSLRs.emplace_back("KODAK EOSDCS1", CFATYPE_GRBG);
	vDSLRs.emplace_back("KODAK EOSDCS3B", CFATYPE_GRBG);
	vDSLRs.emplace_back("KODAK NC2000F", CFATYPE_GRBG);
	vDSLRs.emplace_back("KODAK NC2000G", CFATYPE_GRBG);
	vDSLRs.emplace_back("KODAK P850 ZOOM", CFATYPE_BGGR);
	vDSLRs.emplace_back("KODAK P880 ZOOM", CFATYPE_BGGR);
	vDSLRs.emplace_back("Kodak DC120", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DC40", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DC50", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS Pro 14N", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS Pro 14n", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS Pro 14nx", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS Pro SLR/c", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS Pro SLR/n", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS315C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS330C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS520C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS560C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS620C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS620X", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS660C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS720X", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak DCS760C", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak KAI-0340", CFATYPE_RGGB);
	vDSLRs.emplace_back("Kodak ProBack", CFATYPE_GRBG);
	vDSLRs.emplace_back("Kodak ProBack645", CFATYPE_GRBG);
	vDSLRs.emplace_back("Konica KD-400Z", CFATYPE_RGGB);
	vDSLRs.emplace_back("Konica KD-510Z", CFATYPE_GRBG);
	vDSLRs.emplace_back("LEICA D-LUX2", CFATYPE_RGGB);
	vDSLRs.emplace_back("LEICA DIGILUX 2", CFATYPE_RGGB);
	vDSLRs.emplace_back("Leaf Aptus 17", CFATYPE_RGGB);
	vDSLRs.emplace_back("Leaf Aptus 22", CFATYPE_GRBG);
	vDSLRs.emplace_back("Leaf Aptus 65", CFATYPE_GBRG);
	vDSLRs.emplace_back("Leaf Aptus 75", CFATYPE_GBRG);
	vDSLRs.emplace_back("Leaf Aptus 75", CFATYPE_RGGB);
	vDSLRs.emplace_back("Leaf CMost", CFATYPE_GBRG);
	vDSLRs.emplace_back("Leaf Cantare", CFATYPE_GRBG);
	vDSLRs.emplace_back("Leaf Valeo 11", CFATYPE_BGGR);
	vDSLRs.emplace_back("Leaf Valeo 11", CFATYPE_GBRG);
	vDSLRs.emplace_back("Leaf Valeo 11", CFATYPE_GRBG);
	vDSLRs.emplace_back("Leaf Valeo 11", CFATYPE_RGGB);
	vDSLRs.emplace_back("Leaf Valeo 17", CFATYPE_GRBG);
	vDSLRs.emplace_back("Leaf Valeo 22", CFATYPE_BGGR);
	vDSLRs.emplace_back("Leaf Valeo 22", CFATYPE_GRBG);
	vDSLRs.emplace_back("Leaf Valeo 22", CFATYPE_RGGB);
	vDSLRs.emplace_back("Leaf Valeo 6", CFATYPE_BGGR);
	vDSLRs.emplace_back("Leaf Valeo 6", CFATYPE_GBRG);
	vDSLRs.emplace_back("Leica Camera AG R8 - Digital Back DMR DNG", CFATYPE_RGGB);
	vDSLRs.emplace_back("Logitech Fotoman Pixtura", CFATYPE_GRBG);
	vDSLRs.emplace_back("Meade DSI Color", CFATYPE_CYGMCYMG);
	vDSLRs.emplace_back("Micron 2010", CFATYPE_BGGR);
	vDSLRs.emplace_back("MINOLTA ALPHA-7 DIGITAL", CFATYPE_RGGB);
	vDSLRs.emplace_back("MINOLTA DYNAX 5D", CFATYPE_RGGB);
	vDSLRs.emplace_back("MINOLTA DYNAX 7D", CFATYPE_RGGB);
	vDSLRs.emplace_back("MINOLTA DiMAGE A200", CFATYPE_GBRG);
	vDSLRs.emplace_back("MINOLTA DiMAGE G400", CFATYPE_GRBG);
	vDSLRs.emplace_back("MINOLTA DiMAGE G530", CFATYPE_GRBG);
	vDSLRs.emplace_back("Minolta DiMAGE 5", CFATYPE_RGGB);
	vDSLRs.emplace_back("Minolta DiMAGE 7", CFATYPE_RGGB);
	vDSLRs.emplace_back("Minolta DiMAGE 7Hi", CFATYPE_RGGB);
	vDSLRs.emplace_back("Minolta DiMAGE A1", CFATYPE_RGGB);
	vDSLRs.emplace_back("Minolta DiMAGE A2", CFATYPE_RGGB);
	vDSLRs.emplace_back("Minolta DiMAGE G500", CFATYPE_GRBG);
	vDSLRs.emplace_back("Minolta DiMAGE G600", CFATYPE_GRBG);
	vDSLRs.emplace_back("Minolta DiMAGE Z2 DNG", CFATYPE_BGGR);
	vDSLRs.emplace_back("Minolta DiMAGE Z2", CFATYPE_BGGR);
	vDSLRs.emplace_back("Minolta RD175", CFATYPE_GRBG);
	vDSLRs.emplace_back("NIKON D1", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON D100", CFATYPE_GRBG);
	vDSLRs.emplace_back("NIKON D1H", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON D1X", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON D200", CFATYPE_RGGB);
	vDSLRs.emplace_back("NIKON D2H", CFATYPE_GBRG);
	vDSLRs.emplace_back("NIKON D2Hs", CFATYPE_GBRG);
	vDSLRs.emplace_back("NIKON D2X", CFATYPE_RGGB);
	vDSLRs.emplace_back("NIKON D2Xs", CFATYPE_RGGB);
	vDSLRs.emplace_back("NIKON D50", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON D70", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON D70s", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON D80", CFATYPE_GBRG);
	vDSLRs.emplace_back("NIKON E2100", CFATYPE_RGGB);
	vDSLRs.emplace_back("NIKON E3700", CFATYPE_RGGB);
	vDSLRs.emplace_back("NIKON E4300", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON E5400", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON E8400", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON E8700", CFATYPE_BGGR);
	vDSLRs.emplace_back("NIKON E8800", CFATYPE_BGGR);
	vDSLRs.emplace_back("Nucore BMQ", CFATYPE_BGGR);
	vDSLRs.emplace_back("Nucore BMQ", CFATYPE_GRBG);
	vDSLRs.emplace_back("Nucore RAW", CFATYPE_GRBG);
	vDSLRs.emplace_back("OLYMPUS C5050Z", CFATYPE_BGGR);
	vDSLRs.emplace_back("OLYMPUS C5060WZ", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS C7070WZ", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS C70Z,C7000Z", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS C740UZ", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS C770UZ", CFATYPE_BGGR);
	vDSLRs.emplace_back("OLYMPUS C8080WZ", CFATYPE_BGGR);
	vDSLRs.emplace_back("OLYMPUS E-1", CFATYPE_GRBG);
	vDSLRs.emplace_back("OLYMPUS E-10", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS E-20,E-20N,E-20P", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS E-300", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS E-330", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS E-400", CFATYPE_GRBG);
	vDSLRs.emplace_back("OLYMPUS E-500", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS SP350", CFATYPE_RGGB);
	vDSLRs.emplace_back("OLYMPUS SP500UZ", CFATYPE_GBRG);
	vDSLRs.emplace_back("OLYMPUS SP500UZ", CFATYPE_GBRG);
	vDSLRs.emplace_back("OLYMPUS SP500UZ", CFATYPE_GBRG);
	vDSLRs.emplace_back("OLYMPUS SP500UZ", CFATYPE_GBRG);
	vDSLRs.emplace_back("Orion StarShoot Deep-Space", CFATYPE_CYGMCYMG);
	vDSLRs.emplace_back("PENTAX *ist D", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX *ist DL", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX *ist DS", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX K100D DNG", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX K100D", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX K10D", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX Optio 33WR", CFATYPE_BGGR);
	vDSLRs.emplace_back("PENTAX Optio S", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX Optio S4", CFATYPE_RGGB);
	vDSLRs.emplace_back("PENTAX Optio S4i", CFATYPE_RGGB);
	vDSLRs.emplace_back("Panasonic DMC-FZ30", CFATYPE_RGGB);
	vDSLRs.emplace_back("Panasonic DMC-FZ50", CFATYPE_GBRG);
	vDSLRs.emplace_back("Panasonic DMC-FZ50", CFATYPE_RGGB);
	vDSLRs.emplace_back("Panasonic DMC-LC1", CFATYPE_RGGB);
	vDSLRs.emplace_back("Panasonic DMC-LX1", CFATYPE_RGGB);
	vDSLRs.emplace_back("Panasonic DMC-LX2", CFATYPE_GBRG);
	vDSLRs.emplace_back("Panasonic DMC-LX2", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One H 10", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One H 20", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One H 25", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One LightPhase", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One P 20", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One P 25", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One P 30", CFATYPE_RGGB);
	vDSLRs.emplace_back("Phase One P 45", CFATYPE_RGGB);
	vDSLRs.emplace_back("Pixelink A782", CFATYPE_GRBG);
	vDSLRs.emplace_back("QHY8 CCD Camera", CFATYPE_RGGB);
	vDSLRs.emplace_back("RICOH GR Digital DNG", CFATYPE_RGGB);
	vDSLRs.emplace_back("Rollei d530flex", CFATYPE_BGGR);
	vDSLRs.emplace_back("RoverShot 3320AF", CFATYPE_GRBG);
	vDSLRs.emplace_back("SAMSUNG GX-1S", CFATYPE_RGGB);
	vDSLRs.emplace_back("SAMSUNG Pro 815 DNG", CFATYPE_GBRG);
	vDSLRs.emplace_back("SBIG ST-2000XCM", CFATYPE_BGGR);
	vDSLRs.emplace_back("SBIG ST-2001XCM", CFATYPE_BGGR);
	vDSLRs.emplace_back("SI0 SI1 DNG", CFATYPE_BGGR);
	vDSLRs.emplace_back("SI0 SI1 DNG", CFATYPE_GRBG);
	vDSLRs.emplace_back("SI0 SI1 DNG", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v6 1288x1024", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v6 643x482", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v9 1284x864", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v9 1288x864", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v9 1632x1220", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v9 2032x1520", CFATYPE_RGGB);
	vDSLRs.emplace_back("SMaL v9 816x612", CFATYPE_RGGB);
	vDSLRs.emplace_back("SONY DSC-R1", CFATYPE_GRBG);
	vDSLRs.emplace_back("SONY DSC-V3", CFATYPE_RGGB);
	vDSLRs.emplace_back("SONY DSLR-A100", CFATYPE_RGGB);
	vDSLRs.emplace_back("ST Micro STV680 VGA", CFATYPE_BGGR);
	vDSLRs.emplace_back("Sarnoff 4096x5440", CFATYPE_RGGB);
	vDSLRs.emplace_back("Sinar 3072x2048", CFATYPE_GRBG);
	vDSLRs.emplace_back("Sinar 4080x4080", CFATYPE_GRBG);
	vDSLRs.emplace_back("Sinar 4080x5440", CFATYPE_GRBG);
	vDSLRs.emplace_back("Sinar Sinarback 54 M, Hasselblad", CFATYPE_GRBG);
	vDSLRs.emplace_back("Sony XCD-SX910CR", CFATYPE_GBRG);
};

RawDDPSettings::RawDDPSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDDPSettings),
	workspace(new CWorkspace()),
	initialised(false)
{
	ui->setupUi(this);

	//
	// Create a validator for the scale values
	//
	scaleValidator = new QDoubleValidator(0, 5, 4, this);
	// Apply it to the RAW Files tab
	ui->brightness->setValidator(scaleValidator);
	ui->redScale->setValidator(scaleValidator);
	ui->blueScale->setValidator(scaleValidator);
	// Apply it to the FITS Files tab
	ui->brightness_2->setValidator(scaleValidator);
	ui->redScale_2->setValidator(scaleValidator);
	ui->blueScale_2->setValidator(scaleValidator);

	//
	// forceUnsigned is no longer used
	//
	ui->forceUnsigned->setEnabled(false);
	ui->forceUnsigned->setVisible(false);

	//
	// Preserve the current settings
	//
	workspace->Push();
}

RawDDPSettings::~RawDDPSettings()
{
	delete ui;
}

void RawDDPSettings::showEvent(QShowEvent *event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}

void RawDDPSettings::onInitDialog()
{
	QSettings settings;
	double value = 0.0;
	QString string;

	//
	// Restore Window position etc..
	//
	QByteArray ba = settings.value("Dialogs/RawDDPSettings/geometry").toByteArray();
	if (!ba.isEmpty())
	{
		restoreGeometry(ba);
	}
	else
	{
		//
		// Get NATIVE windows ultimate parent
		//
		HWND hParent = GetDeepStackerDlg(nullptr)->m_hWnd;
		RECT r;
		GetWindowRect(hParent, &r);

		QSize size = this->size();

		int top = ((r.top + (r.bottom - r.top) / 2) - (size.height() / 2));
		int left = ((r.left + (r.right - r.left) / 2) - (size.width() / 2));
		move(left, top);
	}

	//
	// First initialise the controls on the Raw Files tab
	//
	value = workspace->value("RawDDP/Brightness", "1.0").toDouble();
	ui->brightness->setText(QString("%L1").arg(value, 0, 'f', 4));

	value = workspace->value("RawDDP/RedScale", "1.0").toDouble();
	ui->redScale->setText(QString("%L1").arg(value, 0, 'f', 4));

	value = workspace->value("RawDDP/BlueScale", "1.0").toDouble();
	ui->blueScale->setText(QString("%L1").arg(value, 0, 'f', 4));

	ui->noWB->setChecked(workspace->value("RawDDP/NoWB", false).toBool());

	ui->cameraWB->setChecked(workspace->value("RawDDP/CameraWB", false).toBool());

	bool isSuperPixels;
	bool isRawBayer;

	isSuperPixels = workspace->value("RawDDP/SuperPixels", false).toBool();
	isRawBayer = workspace->value("RawDDP/RawBayer", false).toBool();

	string = workspace->value("RawDDP/Interpolation", "Bilinear").toString();

	if (isRawBayer)
		ui->rawBayer->setChecked(true);
	else if (isSuperPixels)
		ui->superPixels->setChecked(true);
	else if (string == "Bilinear")
		ui->bilinear->setChecked(true);
	else if (string == "AHD")
		ui->AHD->setChecked(true);

	ui->blackPointToZero->setChecked(workspace->value("RawDDP/BlackPointTo0", false).toBool());

	//
	// Now populate the FITS Files tab controls
	//
	ui->isFITSRaw->setChecked(workspace->value("FitsDDP/FITSisRAW", false).toBool());

	value = workspace->value("FitsDDP/Brightness", "1.0").toDouble();
	ui->brightness_2->setText(QString("%L1").arg(value, 0, 'f', 4));

	value = workspace->value("FitsDDP/RedScale", "1.0").toDouble();
	ui->redScale_2->setText(QString("%L1").arg(value, 0, 'f', 4));

	value = workspace->value("FitsDDP/BlueScale", "1.0").toDouble();
	ui->blueScale_2->setText(QString("%L1").arg(value, 0, 'f', 4));

	string = workspace->value("FitsDDP/Interpolation", "Bilinear").toString();

	if (string == "Bilinear")
		ui->bilinear_2->setChecked(true);
	else if (string == "RawBayer")
		ui->rawBayer_2->setChecked(true);
	else if (string == "AHD")
		ui->AHD_2->setChecked(true);
	else
		ui->superPixels_2->setChecked(true);

	fillDSLRList(vector_DSLRs);

	for (std::vector<CDSLR>::size_type i = 0; i < vector_DSLRs.size(); i++)
	{
		ui->DSLRs->addItem(vector_DSLRs[i].m_strName, static_cast<uint>(vector_DSLRs[i].m_CFAType));
	};

	string = workspace->value("FitsDDP/DSLR", "").toString();
	if (string.isEmpty())
		ui->DSLRs->setCurrentIndex(0);
	else
	{
		const int row = ui->DSLRs->findText(string);
		if (row != -1)
		{
			ui->DSLRs->setCurrentIndex(row);
		}
	}

	updateBayerPattern().updateControls();

	ui->tabWidget->setCurrentIndex(0);

}

RawDDPSettings & RawDDPSettings::updateBayerPattern()
{
	ZFUNCTRACE_RUNTIME();

	int index = ui->DSLRs->currentIndex();
	if (-1 != index)
	{
		CFATYPE type = (CFATYPE)ui->DSLRs->currentData().toUInt();
		switch (type)
		{
		case CFATYPE_BGGR:
			if (bggrPix.isNull())
			{
				bggrPix.load(":/rawddpsettings/Pattern_BGGR.bmp");
			}
			ui->bayerPattern->setPixmap(bggrPix);
			break;
		case CFATYPE_GBRG:
			if (gbrgPix.isNull())
			{
				gbrgPix.load(":/rawddpsettings/Pattern_GBRG.bmp");
			}
			ui->bayerPattern->setPixmap(gbrgPix);
			break;
		case CFATYPE_GRBG:
			if (grbgPix.isNull())
			{
				grbgPix.load(":/rawddpsettings/Pattern_GRBG.bmp");
			}
			ui->bayerPattern->setPixmap(grbgPix);
			break;
		case CFATYPE_RGGB:
			if (rggbPix.isNull())
			{
				rggbPix.load(":/rawddpsettings/Pattern_RGGB.bmp");
			}
			ui->bayerPattern->setPixmap(rggbPix);
			break;
		case CFATYPE_CYGMCYMG:
			if (cygmcymgPix.isNull())
			{
				cygmcymgPix.load(":/rawddpsettings/Pattern_CYGMCYMG.bmp");
			}
			ui->bayerPattern->setPixmap(cygmcymgPix);
			break;
		default:
			ui->bayerPattern->clear();
		};
	}
	else ui->bayerPattern->clear();

	return *this;
};

RawDDPSettings & RawDDPSettings::updateControls()
{
	const bool isFITSRaw = ui->isFITSRaw->isChecked();

	const size_t index = ui->DSLRs->currentIndex();
	if (isFITSRaw && index >= 0 && index < vector_DSLRs.size())
	{
		bool		bCYMG;

		bCYMG = ::IsCYMGType(vector_DSLRs[index].m_CFAType);

		ui->colourAdjustment->setEnabled(!bCYMG);

		ui->rawBayer_2->setEnabled(!bCYMG);
		ui->superPixels_2->setEnabled(!bCYMG);
		ui->AHD_2->setEnabled(!bCYMG);

		ui->bilinear_2_desc->setEnabled(!bCYMG);
		ui->superPixels_2_desc->setEnabled(!bCYMG);
		if (bCYMG)
		{
			ui->rawBayer_2->setChecked(false);
			ui->superPixels_2->setChecked(false);
			ui->AHD_2->setChecked(false);
			ui->bilinear_2->setChecked(true);
		};
	}
	else
	{
		ui->rawBayer_2->setEnabled(true);
		ui->superPixels_2->setEnabled(true);
		ui->AHD_2->setEnabled(true);
		ui->bilinear_2_desc->setEnabled(true);
		ui->superPixels_2_desc->setEnabled(true);
	};

	ui->textDSLRs->setEnabled(isFITSRaw);
	ui->DSLRs->setEnabled(isFITSRaw);
	ui->colourAdjustment->setEnabled(true);
	ui->bayerGroup->setEnabled(isFITSRaw);

	return *this;
}


// Slots for RAW Files tab
void RawDDPSettings::on_brightness_textEdited(const QString& string)
{
	bool OK = false;
	double value = string.toDouble(&OK);
	ZASSERTSTATE(OK);
	workspace->setValue("RawDDP/Brightness", value);
}

void RawDDPSettings::on_redScale_textEdited(const QString& string)
{
	bool OK = false;
	double value = string.toDouble(&OK);
	ZASSERTSTATE(OK);
	workspace->setValue("RawDDP/RedScale", value);
}

void RawDDPSettings::on_blueScale_textEdited(const QString& string)
{
	bool OK = false;
	double value = string.toDouble(&OK);
	ZASSERTSTATE(OK);
	workspace->setValue("RawDDP/BlueScale", value);
}

void RawDDPSettings::on_noWB_clicked()
{
	workspace->setValue("RawDDP/NoWB", true);
	workspace->setValue("RawDDP/CameraWB", false);
}
void RawDDPSettings::on_cameraWB_clicked()
{
	workspace->setValue("RawDDP/CameraWB", true);
	workspace->setValue("RawDDP/NoWB", false);
}

void RawDDPSettings::on_bilinear_clicked()
{
	workspace->setValue("RawDDP/Interpolation", "Bilinear");
	workspace->setValue("RawDDP/RawBayer", false);
	workspace->setValue("RawDDP/SuperPixels", false);
}

void RawDDPSettings::on_AHD_clicked()
{
	workspace->setValue("RawDDP/Interpolation", "AHD");
	workspace->setValue("RawDDP/RawBayer", false);
	workspace->setValue("RawDDP/SuperPixels", false);
}
void RawDDPSettings::on_rawBayer_clicked()
{
	workspace->setValue("RawDDP/Interpolation", "");
	workspace->setValue("RawDDP/RawBayer", true);
	workspace->setValue("RawDDP/SuperPixels", false);
}
void RawDDPSettings::on_superPixels_clicked()
{
	workspace->setValue("RawDDP/Interpolation", "");
	workspace->setValue("RawDDP/RawBayer", false);
	workspace->setValue("RawDDP/SuperPixels", true);
}

void RawDDPSettings::on_blackPointToZero_clicked(bool checked)
{
	workspace->setValue("RawDDP/BlackPointTo0", checked);
}

// Slots for FITS Files tab
void RawDDPSettings::on_isFITSRaw_clicked(bool checked)
{
	workspace->setValue("FitsDDP/FITSisRAW", checked);
	updateControls();
}

void RawDDPSettings::on_DSLRs_currentIndexChanged(int)
{
	workspace->setValue("FitsDDP/DSLR", ui->DSLRs->currentText());
	workspace->setValue("FitsDDP/BayerPattern", ui->DSLRs->currentData().toUInt());
	updateBayerPattern().updateControls();
}

void RawDDPSettings::on_brightness_2_textEdited(const QString& string)
{
	bool OK = false;
	double value = string.toDouble(&OK);
	ZASSERTSTATE(OK);
	workspace->setValue("FitsDDP/Brightness", value);

}

void RawDDPSettings::on_redScale_2_textEdited(const QString& string)
{
	bool OK = false;
	double value = string.toDouble(&OK);
	ZASSERTSTATE(OK);
	workspace->setValue("FitsDDP/RedScale", value);
}

void RawDDPSettings::on_blueScale_2_textEdited(const QString& string)
{
	bool OK = false;
	double value = string.toDouble(&OK);
	ZASSERTSTATE(OK);
	workspace->setValue("FitsDDP/BlueScale", value);
}

/**********************************************************************************/
/*                           ***** NOTE WELL *****                                */
/*                                                                                */
/*   The following four slots that process the Bayer Matrix Transformation Radio  */  
/* Buttons of the FITS Files tab all set the value of "RawDDP/SuperPixels" in the */
/* workspace (which is what the original pre-Qt code did).                                                                     */
/*                                                                                */
/*               AS FAR AS I CAN DETERMINE THIS IS NOT A BUG                      */
/*                                                                                */
/**********************************************************************************/
void RawDDPSettings::on_bilinear_2_toggled(bool checked)
{
	if (checked)
	{
		workspace->setValue("FitsDDP/Interpolation", "Bilinear");
		workspace->setValue("RawDDP/SuperPixels", false);
	}
}

void RawDDPSettings::on_AHD_2_toggled(bool checked)
{
	if (checked)
	{
		workspace->setValue("FitsDDP/Interpolation", "AHD");
		workspace->setValue("RawDDP/SuperPixels", false);
	}
}

void RawDDPSettings::on_rawBayer_2_toggled(bool checked)
{
	if (checked)
	{
		workspace->setValue("FitsDDP/Interpolation", "RawBayer");
		workspace->setValue("RawDDP/SuperPixels", false);
	}
}

void RawDDPSettings::on_superPixels_2_toggled(bool checked)
{
	if (checked)
	{
		workspace->setValue("FitsDDP/Interpolation", "SuperPixels");
		workspace->setValue("RawDDP/SuperPixels", true);
	}
}

void RawDDPSettings::on_buttonBox_clicked(QAbstractButton *button)
{
	//
	// If the Apply button was pressed, do the appropiate thing
	//
	if (QDialogButtonBox::Apply == ui->buttonBox->standardButton(button))
	{
		apply();
	}
}

void RawDDPSettings::accept()
{
	QSettings settings;

	settings.setValue("Dialogs/RawDDPSettings/geometry", saveGeometry());

	apply();

	Inherited::accept();
}

void RawDDPSettings::apply()
{
	//
	// Pop the preserved workspace setting and discard the saved values
	//
	workspace->Pop(false);

	//
	// Harden the workspace changes
	//
	workspace->saveSettings();

	//
	// Reload current image with changed settings
	//
	GetDeepStackerDlg(nullptr)->GetStackingDlg().ReloadCurrentImage();
}

void RawDDPSettings::reject()
{
	QSettings settings;

	settings.setValue("Dialogs/RawDDPSettings/geometry", saveGeometry());

	//
	// Pop the preserved workspace setting and restore the status quo ante 
	//
	workspace->Pop(true);

	Inherited::reject();

}