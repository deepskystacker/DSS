#ifndef __FITSUTIL_H__
#define __FITSUTIL_H__

/* ------------------------------------------------------------------- */
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "..\CFitsio\fitsio.h"

class CFITSHeader
{
public :
	LONG				m_lWidth,
						m_lHeight;
	LONG				m_lBitsPerPixel;
	LONG				m_lNrChannels;
	double				m_fExposureTime;
	LONG				m_lISOSpeed;
	BOOL				m_bFloat;
	BOOL				m_bSigned;
	CString				m_strMake;
	CFATYPE				m_CFAType;
	FITSFORMAT			m_Format;
	BOOL				m_bByteSwap;
	CBitmapExtraInfo	m_ExtraInfo;
	SYSTEMTIME			m_DateTime;

public :
	CFITSHeader();
	virtual ~CFITSHeader();

public :
	LONG	Height()
	{
		return m_lHeight;
	};

	LONG	Width()
	{
		return m_lWidth;
	};

	LONG	BitPerChannels()
	{
		return m_lBitsPerPixel;
	};

	LONG	NrChannels()
	{
		return m_lNrChannels;
	};

	BOOL	IsFloat()
	{
		return m_bFloat;
	};

	BOOL	IsCFA()
	{
		return (m_CFAType != CFATYPE_NONE);
	};

	CFATYPE GetCFAType()
	{
		return m_CFAType;
	};


	BOOL	IsMaster()
	{
		return FALSE;
	};

	double	GetExposureTime()
	{
		return m_fExposureTime;
	};

	LONG	GetISOSpeed()
	{
		return m_lISOSpeed;
	};

	SYSTEMTIME	GetDateTime()
	{
		return m_DateTime;
	};
};

/* ------------------------------------------------------------------- */

class CFITSReader : public CFITSHeader
{
public :
	fitsfile *				m_fits;
	CString					m_strFileName;
	CDSSProgress *			m_pProgress;

protected :
	double					m_fGreenRatio;
	double					m_fRedRatio;
	double					m_fBlueRatio;
	double					m_fBrightnessRatio;
	BOOL					m_bDSI;

private :
	BOOL	ReadKey(LPSTR szKey, double & fValue, CString & strComment);
	BOOL	ReadKey(LPSTR szKey, double & fValue);
	BOOL	ReadKey(LPSTR szKey, LONG & lValue);
	BOOL	ReadKey(LPSTR szKey, CString & strValue);
	void	ReadAllKeys();

public :
	CFITSReader(LPCTSTR szFileName, CDSSProgress *	pProgress)
	{
		m_fits		  = NULL;
		m_strFileName = szFileName;
		m_pProgress   = pProgress;
		m_fGreenRatio = 1.0;
		m_fRedRatio   = 1.0;
		m_fBlueRatio  = 1.0;
		m_bDSI		  = FALSE;
	};

	virtual ~CFITSReader() 
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

class CFITSWriter : public CFITSHeader
{
public :
	fitsfile *				m_fits;
	CString					m_strFileName;
	CDSSProgress *			m_pProgress;
	CString					m_strDescription;
	FITSFORMAT				m_Format;

private :
	BOOL	WriteKey(LPSTR szKey, double fValue, LPSTR szComment = NULL);
	BOOL	WriteKey(LPSTR szKey, LONG lValue, LPSTR szComment = NULL);
	BOOL	WriteKey(LPSTR szKey, LPCTSTR szValue, LPSTR szComment = NULL);
	void	WriteAllKeys();

protected :
	void	SetFormat(LONG lWidth, LONG lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType);

public :
	CFITSWriter(LPCTSTR szFileName, CDSSProgress *	pProgress)
	{
		m_fits		  = NULL;
		m_strFileName = szFileName;
		m_pProgress   = pProgress;
		m_Format	  = FF_UNKNOWN;
	};

	virtual ~CFITSWriter() 
	{
		Close();
	};

	void	SetDescription(LPCTSTR szDescription)
	{
		m_strDescription = szDescription;
	};

	void	SetFormat(FITSFORMAT FITSFormat)
	{
		m_Format = FITSFormat;
	};

	BOOL	Open();
	BOOL	Write();
	BOOL	Close();

	virtual BOOL	OnOpen() { return TRUE; };
	virtual BOOL	OnWrite(LONG lX, LONG lY, double & fRed, double & fGreen, double & fBlue) = 0;
	virtual BOOL	OnClose() { return TRUE; };
};

/* ------------------------------------------------------------------- */

CFATYPE GetFITSCFATYPE();
BOOL	GetFITSInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
BOOL	ReadFITS(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress *	pProgress);
BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, FITSFORMAT FITSFormat = FF_UNKNOWN, LPCTSTR szDescription = NULL, LONG lISOSpeed = 0, double fExposure = 0.0);
BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription = NULL, LONG lISOSpeed = 0, double fExposure = 0.0);

BOOL	IsFITSPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
BOOL	LoadFITSPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);

void	GetFITSExtension(LPCTSTR szFileName, CString & strExtension);

#endif // __FITSUTIL_H__