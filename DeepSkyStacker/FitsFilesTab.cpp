// FitsFilesTab.cpp : implementation file
//

#include "stdafx.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "FitsFilesTab.h"
#include "Registry.h"
#include "Workspace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* ------------------------------------------------------------------- */

static void	FillDSLRList(std::vector<CDSLR> & vDSLRs)
{
	ZFUNCTRACE_RUNTIME();
	// Fill the four generic DSLR bayer pattern
	CString				strValue;

	strValue.LoadString(IDS_GENERIC_RGGB);
	vDSLRs.push_back(CDSLR(strValue, CFATYPE_RGGB));
	strValue.LoadString(IDS_GENERIC_BGGR);
	vDSLRs.push_back(CDSLR(strValue, CFATYPE_BGGR));
	strValue.LoadString(IDS_GENERIC_GBRG);
	vDSLRs.push_back(CDSLR(strValue, CFATYPE_GBRG));
	strValue.LoadString(IDS_GENERIC_GRBG);
	vDSLRs.push_back(CDSLR(strValue, CFATYPE_GRBG));

	strValue.LoadString(IDS_GENERIC);
	vDSLRs.push_back(CDSLR(strValue+" CYMG (01)",CFATYPE_CYGMCYMG));
	vDSLRs.push_back(CDSLR(strValue+" CYMG (02)",CFATYPE_GMCYMGCY));
	vDSLRs.push_back(CDSLR(strValue+" CYMG (03)",CFATYPE_CYMGCYGM));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (04)",CFATYPE_MGCYGMCY));	//*
	vDSLRs.push_back(CDSLR(strValue+" CYMG (05)",CFATYPE_GMYCGMCY));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (06)",CFATYPE_YCGMCYGM));	//*
	vDSLRs.push_back(CDSLR(strValue+" CYMG (07)",CFATYPE_GMCYGMYC));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (08)",CFATYPE_CYGMYCGM));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (09)",CFATYPE_YCGMYCMG));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (10)",CFATYPE_GMYCMGYC));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (11)",CFATYPE_YCMGYCGM));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (12)",CFATYPE_MGYCGMYC));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (13)",CFATYPE_MGYCMGCY));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (14)",CFATYPE_YCMGCYMG));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (15)",CFATYPE_MGCYMGYC));	
	vDSLRs.push_back(CDSLR(strValue+" CYMG (16)",CFATYPE_CYMGYCMG));	

	vDSLRs.push_back(CDSLR(_T("AVT F-145C"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("AVT F-201C"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("AVT F-510C"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("AVT F-810C"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO EX-P505"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO EX-P600"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO EX-P700"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO EX-S100"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO EX-Z50"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO EX-Z55"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO QV-2000UX"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO QV-3*00EX"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO QV-4000"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO QV-5700"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("CASIO QV-R51"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 10D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 20D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 300D DIGITAL"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 30D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 350D DIGITAL"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 40D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 400D DIGITAL"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 50D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 5D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS 5D Mk II"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS D2000C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Canon EOS D30"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS D60"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS DIGITAL REBEL"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS DIGITAL REBEL XT"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS DIGITAL REBEL XTi"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS Kiss Digital"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS Kiss Digital N"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS-1D"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Canon EOS-1D Mark II"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS-1D Mark II N"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon EOS-1DS"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Canon EOS-1Ds Mark II"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot G2"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot G3"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot G5"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot G6"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot Pro1"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot S30"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot S40"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot S45"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot S50"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot S60"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Canon PowerShot S70"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Contax N Digital"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Creative PC-CAM 600"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("EPSON R-D1"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("EPSON R-D1s"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix E550"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix E900"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix F700"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix F710"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S20Pro"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S2Pro"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S3Pro"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S5000"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S5100"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S5200"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S5600"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S7000"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("FUJIFILM FinePix S9500"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Foculus 531C"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("ISG 2020x1520"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Imacon Ixpress 16-Mp"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Imacon Ixpress 22-Mp"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Imacon Ixpress 22-Mp"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Imacon Ixpress 39-Mp"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("KODAK DCS420"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("KODAK DCS460"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("KODAK EOSDCS1"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("KODAK EOSDCS3B"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("KODAK NC2000F"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("KODAK NC2000G"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("KODAK P850 ZOOM"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("KODAK P880 ZOOM"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Kodak DC120"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DC40"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DC50"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS Pro 14N"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS Pro 14n"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS Pro 14nx"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS Pro SLR/c"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS Pro SLR/n"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS315C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS330C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS520C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS560C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS620C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS620X"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS660C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS720X"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak DCS760C"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak KAI-0340"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Kodak ProBack"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Kodak ProBack645"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Konica KD-400Z"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Konica KD-510Z"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("LEICA D-LUX2"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("LEICA DIGILUX 2"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Leaf Aptus 17"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Leaf Aptus 22"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Leaf Aptus 65"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Leaf Aptus 75"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Leaf Aptus 75"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Leaf CMost"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Leaf Cantare"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 11"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 11"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 11"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 11"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 17"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 22"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 22"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 22"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 6"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Leaf Valeo 6"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Leica Camera AG R8 - Digital Back DMR DNG"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Logitech Fotoman Pixtura"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Meade DSI Color"),CFATYPE_CYGMCYMG));
	vDSLRs.push_back(CDSLR(_T("Micron 2010"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("MINOLTA ALPHA-7 DIGITAL"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("MINOLTA DYNAX 5D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("MINOLTA DYNAX 7D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("MINOLTA DiMAGE A200"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("MINOLTA DiMAGE G400"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("MINOLTA DiMAGE G530"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE 5"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE 7"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE 7Hi"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE A1"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE A2"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE G500"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE G600"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE Z2 DNG"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Minolta DiMAGE Z2"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Minolta RD175"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("NIKON D1"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON D100"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("NIKON D1H"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON D1X"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON D200"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("NIKON D2H"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("NIKON D2Hs"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("NIKON D2X"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("NIKON D2Xs"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("NIKON D50"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON D70"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON D70s"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON D80"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("NIKON E2100"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("NIKON E3700"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("NIKON E4300"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON E5400"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON E8400"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON E8700"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("NIKON E8800"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Nucore BMQ"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Nucore BMQ"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Nucore RAW"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C5050Z"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C5060WZ"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C7070WZ"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C70Z,C7000Z"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C740UZ"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C770UZ"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS C8080WZ"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-1"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-10"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-20,E-20N,E-20P"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-300"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-330"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-400"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS E-500"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS SP350"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS SP500UZ"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS SP500UZ"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS SP500UZ"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("OLYMPUS SP500UZ"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Orion StarShoot Deep-Space"),CFATYPE_CYGMCYMG));
	vDSLRs.push_back(CDSLR(_T("PENTAX *ist D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX *ist DL"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX *ist DS"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX K100D DNG"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX K100D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX K10D"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX Optio 33WR"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("PENTAX Optio S"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX Optio S4"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("PENTAX Optio S4i"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-FZ30"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-FZ50"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-FZ50"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-LC1"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-LX1"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-LX2"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("Panasonic DMC-LX2"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One H 10"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One H 20"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One H 25"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One LightPhase"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One P 20"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One P 25"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One P 30"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Phase One P 45"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Pixelink A782"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("QHY8 CCD Camera"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("RICOH GR Digital DNG"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Rollei d530flex"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("RoverShot 3320AF"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("SAMSUNG GX-1S"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SAMSUNG Pro 815 DNG"),CFATYPE_GBRG));
	vDSLRs.push_back(CDSLR(_T("SBIG ST-2000XCM"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("SBIG ST-2001XCM"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("SI0 SI1 DNG"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("SI0 SI1 DNG"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("SI0 SI1 DNG"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v6 1288x1024"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v6 643x482"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v9 1284x864"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v9 1288x864"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v9 1632x1220"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v9 2032x1520"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SMaL v9 816x612"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SONY DSC-R1"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("SONY DSC-V3"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("SONY DSLR-A100"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("ST Micro STV680 VGA"),CFATYPE_BGGR));
	vDSLRs.push_back(CDSLR(_T("Sarnoff 4096x5440"),CFATYPE_RGGB));
	vDSLRs.push_back(CDSLR(_T("Sinar 3072x2048"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Sinar 4080x4080"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Sinar 4080x5440"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Sinar Sinarback 54 M, Hasselblad"),CFATYPE_GRBG));
	vDSLRs.push_back(CDSLR(_T("Sony XCD-SX910CR"),CFATYPE_GBRG));
};

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CFitsFilesTab property page

IMPLEMENT_DYNCREATE(CFitsFilesTab, CPropertyPage)

CFitsFilesTab::CFitsFilesTab() : CPropertyPage(CFitsFilesTab::IDD)
{
	//{{AFX_DATA_INIT(CFitsFilesTab)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_PREMATURE;
	m_bFirstActivation = TRUE;
}

/* ------------------------------------------------------------------- */

CFitsFilesTab::~CFitsFilesTab()
{
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFitsFilesTab)
	DDX_Control(pDX, IDC_REDSCALE, m_RedScale);
	DDX_Control(pDX, IDC_BRIGHTNESS, m_Brightness);
	DDX_Control(pDX, IDC_BLUESCALE, m_BlueScale);
	DDX_Control(pDX, IDC_BILINEAR, m_Bilinear);
	DDX_Control(pDX, IDC_SUPERPIXELS, m_SuperPixels);
	DDX_Control(pDX, IDC_RAWBAYER, m_RawBayer);
	DDX_Control(pDX, IDC_AHD, m_AHD);
	DDX_Control(pDX, IDC_DSLR, m_DSLR);
	DDX_Control(pDX, IDC_FITSISRAW, m_FITSisRAW);
	DDX_Control(pDX, IDC_BAYERPATTERN, m_BayerPattern);
	DDX_Control(pDX, IDC_FORCEUNSIGNED, m_ForceUnsigned);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CFitsFilesTab, CPropertyPage)
	//{{AFX_MSG_MAP(CFitsFilesTab)
	ON_BN_CLICKED(IDC_FITSISRAW, OnFITSIsRAW)
	ON_EN_CHANGE(IDC_BLUESCALE, OnChangeBluescale)
	ON_EN_CHANGE(IDC_BRIGHTNESS, OnChangeBrightness)
	ON_EN_CHANGE(IDC_REDSCALE, OnChangeRedscale)
	ON_BN_CLICKED(IDC_BILINEAR, OnBilinear)
	ON_BN_CLICKED(IDC_SUPERPIXELS, &OnBnClickedSuperpixels)
	ON_BN_CLICKED(IDC_AHD, &OnAHD)
	ON_BN_CLICKED(IDC_RAWBAYER, &OnBnClickedRawbayer)
	ON_CBN_SELCHANGE(IDC_DSLR, &OnDSLRChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */

void CFitsFilesTab::UpdateControls()
{
	CString				strValue;
	BOOL				bOk = TRUE;
	BOOL				bInterpolation = TRUE;
	double				fValue;

	BOOL				bAllEnabled;

	bAllEnabled = m_FITSisRAW.GetCheck();

	m_Brightness.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = FALSE;

	m_RedScale.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = FALSE;

	m_BlueScale.GetWindowText(strValue);
	fValue = _ttof(strValue);
	if (fValue <= 0)
		bOk = FALSE;

	LONG			lIndice = m_DSLR.GetCurSel();
	if (lIndice >= 0 && lIndice < m_vDSLRs.size())
	{
		BOOL		bCYMG;
		
		bCYMG = ::IsCYMGType(m_vDSLRs[lIndice].m_CFAType);

		m_Brightness.EnableWindow(!bCYMG);
		m_RedScale.EnableWindow(!bCYMG && bAllEnabled);
		m_BlueScale.EnableWindow(!bCYMG && bAllEnabled);

		m_RawBayer.EnableWindow(!bCYMG && bAllEnabled);
		m_SuperPixels.EnableWindow(!bCYMG && bAllEnabled);
		m_AHD.EnableWindow(!bCYMG && bAllEnabled);

		GetDlgItem(IDC_TEXT2)->EnableWindow(!bCYMG);
		GetDlgItem(IDC_TEXT3)->EnableWindow(!bCYMG && bAllEnabled);
		GetDlgItem(IDC_TEXT4)->EnableWindow(!bCYMG && bAllEnabled);
		GetDlgItem(IDC_TEXT6)->EnableWindow(!bCYMG && bAllEnabled);
		GetDlgItem(IDC_TEXT7)->EnableWindow(!bCYMG && bAllEnabled);
		if (bCYMG)
		{
			if (m_RawBayer.GetCheck() || m_SuperPixels.GetCheck() || m_AHD.GetCheck())
			{
				m_RawBayer.SetCheck(FALSE);
				m_SuperPixels.SetCheck(FALSE);
				m_AHD.SetCheck(FALSE);
				m_Bilinear.SetCheck(TRUE);
			};
		};
	}
	else
	{
		m_Brightness.EnableWindow(TRUE);
		m_RedScale.EnableWindow(bAllEnabled);
		m_BlueScale.EnableWindow(bAllEnabled);
		m_RawBayer.EnableWindow(bAllEnabled);
		m_SuperPixels.EnableWindow(bAllEnabled);
		m_AHD.EnableWindow(bAllEnabled);
		GetDlgItem(IDC_TEXT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_TEXT3)->EnableWindow(bAllEnabled);
		GetDlgItem(IDC_TEXT4)->EnableWindow(bAllEnabled);
		GetDlgItem(IDC_TEXT6)->EnableWindow(bAllEnabled);
		GetDlgItem(IDC_TEXT7)->EnableWindow(bAllEnabled);
	};

	m_DSLR.EnableWindow(bAllEnabled);
	m_Bilinear.EnableWindow(bAllEnabled);
	GetDlgItem(IDC_TEXT1)->EnableWindow(bAllEnabled);
	GetDlgItem(IDC_TEXT5)->EnableWindow(bAllEnabled);

	m_BayerPattern.ShowWindow(bAllEnabled ? SW_SHOW : SW_HIDE);

//	m_OK.EnableWindow(bOk);
};

/* ------------------------------------------------------------------- */

void CFitsFilesTab::UpdateBayerPattern()
{
	ZFUNCTRACE_RUNTIME();
	LONG			lIndice;

	lIndice = m_DSLR.GetCurSel();
	if (lIndice >= 0 && lIndice < m_vDSLRs.size())
	{
		switch (m_vDSLRs[lIndice].m_CFAType)
		{
		case CFATYPE_BGGR :
			m_BayerPattern.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PATTERN_BGGR)));
			break;
		case CFATYPE_GBRG :
			m_BayerPattern.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PATTERN_GBRG)));
			break;
		case CFATYPE_GRBG :
			m_BayerPattern.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PATTERN_GRBG)));
			break;
		case CFATYPE_RGGB :
			m_BayerPattern.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PATTERN_RGGB)));
			break;
		case CFATYPE_CYGMCYMG :
			m_BayerPattern.SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PATTERN_CYGMCYMG)));
			break;
		};
	};
};

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CFitsFilesTab message handlers

BOOL CFitsFilesTab::OnSetActive() 
{
	if (m_bFirstActivation)
	{
		CWorkspace			workspace;
		CString				strValue;
		DWORD				bValue;

		bValue = FALSE;

		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("ForceUnsigned"), bValue);
		m_ForceUnsigned.SetCheck(bValue);

		bValue = FALSE;
		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("FITSisRAW"), bValue);
		m_FITSisRAW.SetCheck(bValue);

		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Brighness"), strValue);
		if (!strValue.GetLength())
			strValue = _T("1.0");
		m_Brightness.SetWindowText(strValue);
		
		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("RedScale"), strValue);
		if (!strValue.GetLength())
			strValue = _T("1.0");
		m_RedScale.SetWindowText(strValue);

		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("BlueScale"), strValue);
		if (!strValue.GetLength())
			strValue = _T("1.0");
		m_BlueScale.SetWindowText(strValue);

		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), strValue);
		if (!strValue.GetLength())
			strValue = _T("Bilinear");

		if (strValue == _T("Bilinear"))
			m_Bilinear.SetCheck(TRUE);
		else if (strValue == _T("RawBayer"))
			m_RawBayer.SetCheck(TRUE);
		else if (strValue == _T("AHD"))
			m_AHD.SetCheck(TRUE);
		else
			m_SuperPixels.SetCheck(TRUE);

		FillDSLRList(m_vDSLRs);
		for (LONG i = 0;i<m_vDSLRs.size();i++)
		{
			m_DSLR.AddString(m_vDSLRs[i].m_strName);
		};

		strValue.Empty();
		workspace.GetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("DSLR"), strValue);
		if (!strValue.GetLength())
			m_DSLR.SetCurSel(0);
		else
		{
			m_DSLR.SelectString(-1, strValue);
			if (m_DSLR.GetCurSel() < 0)
				m_DSLR.SetCurSel(0);
		};

		UpdateBayerPattern();
		UpdateControls();
		m_bFirstActivation = FALSE;
	};

	return CPropertyPage::OnSetActive();
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::SaveValues()
{
	if (!m_bFirstActivation)
	{
		CWorkspace			workspace;
		CString				strValue;

		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("FITSisRAW"), m_FITSisRAW.GetCheck() ? true : false);
		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("ForceUnsigned"), m_ForceUnsigned.GetCheck() ? true : false);

		m_Brightness.GetWindowText(strValue);
		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Brighness"), strValue);
		
		m_RedScale.GetWindowText(strValue);
		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("RedScale"), strValue);

		m_BlueScale.GetWindowText(strValue);
		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("BlueScale"), strValue);

		strValue.Empty();
		if (m_Bilinear.GetCheck())
			strValue = _T("Bilinear");
		else if (m_SuperPixels.GetCheck())
			strValue = _T("SuperPixels");
		else if (m_RawBayer.GetCheck())
			strValue = _T("RawBayer");
		else
			strValue = _T("AHD");

		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), strValue);

		m_DSLR.GetLBText(m_DSLR.GetCurSel(), strValue);
		workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("DSLR"), strValue);

		if (m_DSLR.GetCurSel() >= 0)
		{
			DWORD		dwPattern;

			dwPattern = m_vDSLRs[m_DSLR.GetCurSel()].m_CFAType;
			workspace.SetValue(REGENTRY_BASEKEY_FITSSETTINGS, _T("BayerPattern"), dwPattern);
		};

		workspace.SaveToRegistry();
	};
};

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnChangeBluescale() 
{
	UpdateControls();	
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnChangeBrightness() 
{
	UpdateControls();	
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnChangeRedscale() 
{
	UpdateControls();	
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnBilinear() 
{
	m_RawBayer.SetCheck(FALSE);
	m_SuperPixels.SetCheck(FALSE);
	m_AHD.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnBnClickedSuperpixels()
{
	m_RawBayer.SetCheck(FALSE);
	m_Bilinear.SetCheck(FALSE);
	m_AHD.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnBnClickedRawbayer()
{
	m_SuperPixels.SetCheck(FALSE);
	m_Bilinear.SetCheck(FALSE);
	m_AHD.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnAHD()
{
	m_SuperPixels.SetCheck(FALSE);
	m_Bilinear.SetCheck(FALSE);
	m_RawBayer.SetCheck(FALSE);
}

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnDSLRChange( )
{
	UpdateBayerPattern();
	UpdateControls();
};

/* ------------------------------------------------------------------- */

void CFitsFilesTab::OnFITSIsRAW( )
{
	UpdateControls();
};

/* ------------------------------------------------------------------- */




