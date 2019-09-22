#ifndef _TIFFUTIL_H__
#define _TIFFUTIL_H__

/* ------------------------------------------------------------------- */
#include "DSSTools.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include <tiffio.h>

const LONG	TIFFTAG_DSS_BASE			= 50000;
const LONG	TIFFTAG_DSS_NRFRAMES		= (TIFFTAG_DSS_BASE + 0);
const LONG	TIFFTAG_DSS_TOTALEXPOSUREOLD= (TIFFTAG_DSS_BASE + 1);
const LONG	TIFFTAG_DSS_ISO				= (TIFFTAG_DSS_BASE + 2);
const LONG	TIFFTAG_DSS_SETTINGSAPPLIED	= (TIFFTAG_DSS_BASE + 3);
const LONG	TIFFTAG_DSS_BEZIERSETTINGS	= (TIFFTAG_DSS_BASE + 4);
const LONG	TIFFTAG_DSS_ADJUSTSETTINGS	= (TIFFTAG_DSS_BASE + 5);
const LONG	TIFFTAG_DSS_CFA				= (TIFFTAG_DSS_BASE + 6);
const LONG	TIFFTAG_DSS_MASTER			= (TIFFTAG_DSS_BASE + 7);
const LONG	TIFFTAG_DSS_TOTALEXPOSURE	= (TIFFTAG_DSS_BASE + 8);
const LONG	TIFFTAG_DSS_CFATYPE			= (TIFFTAG_DSS_BASE + 9);
const LONG  TIFFTAG_DSS_APERTURE        = (TIFFTAG_DSS_BASE + 10);
const LONG	TIFFTAG_DSS_GAIN			= (TIFFTAG_DSS_BASE + 11);

void DSSTIFFInitialize();

class CTIFFHeader
{
protected :
	int					w, h;
    uint16				spp,
						bpp,
						photo,
						compression,
						planarconfig,
						sampleformat;
	int					cfa;
	int					cfatype;
	int					master;
	CString				strMakeModel;
	double				samplemin,
						samplemax;
	float				exposureTime;
	float				aperture;
	LONG				isospeed;
	LONG				gain;
	LONG				nrframes;
	SYSTEMTIME			m_DateTime;

public :
	CTIFFHeader()
	{
		TIFFSetWarningHandler(NULL);
		TIFFSetWarningHandlerExt(NULL);
		TIFFSetErrorHandler(NULL);
		TIFFSetErrorHandlerExt(NULL);
		DSSTIFFInitialize();
		samplemax = 1.0;
		samplemin = 0.0;
		exposureTime = 0.0;
		aperture = 0.0;
		isospeed = 0;
		gain = -1;
		cfatype  = 0;
		cfa      = 0;
		nrframes = 0;
		m_DateTime.wYear = 0;
	};
	virtual ~CTIFFHeader() {};

public :
	BOOL	IsCFA()
	{
		return cfa;
	};

	CFATYPE GetCFAType()
	{
		return (CFATYPE)cfatype;
	};

	BOOL	IsMaster()
	{
		return master;
	};

	BOOL	IsFloat()
	{
		return (sampleformat = SAMPLEFORMAT_IEEEFP) && (bpp == 32);
	};

	LONG	Height()
	{
		return h;
	};

	LONG	GetISOSpeed()
	{
		return isospeed;
	};

	void	SetISOSpeed(LONG lISOSpeed)
	{
		isospeed = lISOSpeed;
	};

	LONG	GetGain()
	{
		return gain;
	};

	void	SetGain(LONG lGain)
	{
		gain = lGain;
	};

	double	GetExposureTime()
	{
		return exposureTime;
	};

	void	SetExposureTime(double fExposureTime)
	{
		exposureTime = fExposureTime;
	};

	double	GetAperture()
	{
		return aperture;
	};

	void	SetAperture(double fAperture)
	{
		aperture = fAperture;
	};

	void	SetNrFrames(LONG lNrFrames)
	{
		nrframes = lNrFrames;
	};

	LONG	Width()
	{
		return w;
	};

	BOOL	IsGray()
	{
		return (spp == 1);
	};

	BOOL	Is8Bits()
	{
		return (bpp == 8);
	};

	BOOL	Is16Bits()
	{
		return (bpp == 16);
	};

	BOOL	Is32Bits()
	{
		return (bpp == 32);
	};

	LONG	BitPerChannels()
	{
		return bpp;
	};

	LONG	NrChannels()
	{
		if (spp==1)
			return spp;
		else
			return 3;
	};

	void	GetMakeModel(CString & str)
	{
		str = strMakeModel;
	};

	SYSTEMTIME	GetDateTime()
	{
		return m_DateTime;
	};
};

class CTIFFReader : public CTIFFHeader
{
public :
	TIFF *					m_tiff;
	CString					m_strFileName;
	CDSSProgress *			m_pProgress;

public :
	CTIFFReader(LPCTSTR szFileName, CDSSProgress *	pProgress)
	{
		m_tiff		  = NULL;
		m_strFileName = szFileName;
		m_pProgress   = pProgress;
	};

	virtual ~CTIFFReader()
	{
		Close();
	};

	BOOL	Open();
	BOOL	Read();
	BOOL	Close();

	virtual BOOL	OnOpen() { return TRUE; };
	virtual BOOL	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue) { return FALSE;};
	virtual BOOL	OnClose() { return TRUE; };
};

/* ------------------------------------------------------------------- */

class CTIFFWriter : public CTIFFHeader
{
public :
	TIFF *					m_tiff;
	CString					m_strFileName;
	CDSSProgress *			m_pProgress;
	CString					m_strDescription;
	TIFFFORMAT				m_Format;

protected :
	void	SetFormat(LONG lWidth, LONG lHeight, TIFFFORMAT TiffFormat, CFATYPE CFAType, BOOL bMaster);
	void	SetCompression(TIFFCOMPRESSION tiffcomp)
	{
		compression = COMPRESSION_NONE;
		switch (tiffcomp)
		{
		case TC_LZW :
			compression = COMPRESSION_LZW;
			break;
		case TC_DEFLATE :
			compression = COMPRESSION_DEFLATE;
			break;
		};
	};

public :
	CTIFFWriter(LPCTSTR szFileName, CDSSProgress *	pProgress)
	{
		m_tiff		  = NULL;
		m_strFileName = szFileName;
		m_pProgress   = pProgress;
		compression   = COMPRESSION_NONE;
		m_Format	  = TF_UNKNOWN;
	};

	virtual ~CTIFFWriter()
	{
		Close();
	};

	void	SetDescription(LPCTSTR szDescription)
	{
		m_strDescription = szDescription;
	};

	void	SetFormatAndCompression(TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression)
	{
		m_Format = TIFFFormat;
		SetCompression(TIFFCompression);
	};

	BOOL	Open();
	BOOL	Write();
	BOOL	Close();

	virtual BOOL	OnOpen() { return TRUE; };
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue) = 0;
	virtual BOOL	OnClose() { return TRUE; };
};

/* ------------------------------------------------------------------- */

BOOL	GetTIFFInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
BOOL	ReadTIFF(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress);
BOOL	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure, double fAperture);
BOOL	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription);
BOOL	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress,
			TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure, double fAperture);
BOOL	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress,
			TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription);

BOOL	IsTIFFPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
BOOL	LoadTIFFPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);

/* ------------------------------------------------------------------- */

#endif // _TIFFUTIL_H__