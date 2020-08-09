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
						bps,
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
		TIFFSetWarningHandler(nullptr);
		TIFFSetWarningHandlerExt(nullptr);
		//TIFFSetErrorHandler(nullptr);
		//TIFFSetErrorHandlerExt(nullptr);
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
		m_DateTime = { 0, 0, 0, 0, 0, 0, 0, 0};
        w = 0;
        h = 0;
        spp = 0;
        bps = 0;
        photo = 0;
        compression = 0;
        planarconfig = 0;
        sampleformat = 0;
        master = 0;
	};
	virtual ~CTIFFHeader() {};

public :
	bool	IsCFA()
	{
		return cfa;
	};

	CFATYPE GetCFAType()
	{
		return (CFATYPE)cfatype;
	};

	bool	IsMaster()
	{
		return master;
	};

	bool	IsFloat()
	{
		return (sampleformat == SAMPLEFORMAT_IEEEFP) && (bps == 32);
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

	bool	IsGray()
	{
		return (spp == 1);
	};

	bool	Is8Bits()
	{
		return (bps == 8);
	};

	bool	Is16Bits()
	{
		return (bps == 16);
	};

	bool	Is32Bits()
	{
		return (bps == 32);
	};

	LONG	BitPerChannels()
	{
		return bps;
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
		m_tiff		  = nullptr;
		m_strFileName = szFileName;
		m_pProgress   = pProgress;
	};

	virtual ~CTIFFReader()
	{
		Close();
	};

	bool	Open();
	bool	Read();
	bool	Close();

	// bool getInfo();

	virtual bool	OnOpen() { return true; };
	virtual void	OnRead(LONG lX, LONG lY, double fRed, double fGreen, double fBlue) { return;};
	virtual bool	OnClose() { return true; };
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
	void	SetFormat(LONG lWidth, LONG lHeight, TIFFFORMAT TiffFormat, CFATYPE CFAType, bool bMaster);
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
		m_tiff		  = nullptr;
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

	bool	Open();
	bool	Write();
	bool	Close();

	virtual bool	OnOpen() { return true; };
	virtual void	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue) = 0;
	virtual bool	OnClose() { return true; };
};

/* ------------------------------------------------------------------- */

bool	GetTIFFInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
bool	ReadTIFF(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress);
bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure, double fAperture);
bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription);
bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress,
			TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure, double fAperture);
bool	WriteTIFF(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress,
			TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, LPCTSTR szDescription);

bool	IsTIFFPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
int		LoadTIFFPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);

/* ------------------------------------------------------------------- */

#endif // _TIFFUTIL_H__
