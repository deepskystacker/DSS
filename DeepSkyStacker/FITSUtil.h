#ifndef __FITSUTIL_H__
#define __FITSUTIL_H__

/* ------------------------------------------------------------------- */
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "..\CFitsio\fitsio.h"
#pragma comment(lib, "cfitsio.lib")

class CFITSHeader
{
public :
	LONG				m_lWidth,
						m_lHeight;
	LONG				m_lBitsPerPixel;
	LONG				m_lNrChannels;
	double				m_fExposureTime;
	LONG				m_lISOSpeed;
	LONG				m_lGain;
	bool				m_bFloat;
	bool				m_bSigned;
	CString				m_strMake;
	CFATYPE				m_CFAType;
	FITSFORMAT			m_Format;
	CBitmapExtraInfo	m_ExtraInfo;
	SYSTEMTIME			m_DateTime;
	LONG				m_xBayerOffset;
	LONG				m_yBayerOffset;
	long				m_bitPix;
	CString				m_filterName;

public :
	CFITSHeader();
	virtual ~CFITSHeader();

public :
	inline LONG	Height() noexcept
	{
		return m_lHeight;
	};

	inline LONG	Width() noexcept
	{
		return m_lWidth;
	};

	inline LONG	BitPerChannels() noexcept
	{
		return m_lBitsPerPixel;
	};

	inline LONG	NrChannels() noexcept
	{
		return m_lNrChannels;
	};

	inline BOOL	IsFloat() noexcept
	{
		return m_bFloat;
	};

	inline BOOL	IsCFA() noexcept
	{
		return (m_CFAType != CFATYPE_NONE);
	};

	inline CFATYPE GetCFAType() noexcept
	{
		return m_CFAType;
	};

	inline BOOL	IsMaster() noexcept
	{
		return FALSE;
	};

	inline double	GetExposureTime() noexcept
	{
		return m_fExposureTime;
	};

	inline LONG	GetISOSpeed() noexcept
	{
		return m_lISOSpeed;
	};

	inline LONG	GetGain() noexcept
	{
		return m_lGain;
	};

	inline SYSTEMTIME	GetDateTime() noexcept
	{
		return m_DateTime;
	};

	inline LONG	getXOffset() noexcept
	{
		return m_xBayerOffset;
	}

	inline LONG	getYOffset() noexcept
	{
		return m_yBayerOffset;
	}
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
		m_fits		  = nullptr;
		m_strFileName = szFileName;
		m_pProgress   = pProgress;
		m_fGreenRatio = 1.0;
		m_fRedRatio   = 1.0;
		m_fBlueRatio  = 1.0;
		m_bDSI		  = FALSE;
        m_fBrightnessRatio = 0;
	};

	virtual ~CFITSReader()
	{
		Close();
	};

	BOOL	Open();
	BOOL	Read();
	virtual BOOL	Close();

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

private :
	BOOL	WriteKey(LPSTR szKey, double fValue, LPSTR szComment = nullptr);
	BOOL	WriteKey(LPSTR szKey, LONG lValue, LPSTR szComment = nullptr);
	BOOL	WriteKey(LPSTR szKey, LPCTSTR szValue, LPSTR szComment = nullptr);
	void	WriteAllKeys();

protected :
	void	SetFormat(LONG lWidth, LONG lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType);

public :
	CFITSWriter(LPCTSTR szFileName, CDSSProgress *	pProgress)
	{
		m_fits		  = nullptr;
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
BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription,
			LONG lISOSpeed, LONG lGain, double fExposure);
BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription);
BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescriptionL,
			LONG lISOSpeed, LONG lGain, double fExposure);
BOOL	WriteFITS(LPCTSTR szFileName, CMemoryBitmap * pBitmap, CDSSProgress * pProgress, LPCTSTR szDescription);

BOOL	IsFITSPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
int		LoadFITSPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);

void	GetFITSExtension(LPCTSTR szFileName, CString & strExtension);

#endif // __FITSUTIL_H__
