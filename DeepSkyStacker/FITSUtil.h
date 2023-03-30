#pragma once

/* ------------------------------------------------------------------- */
#include "cfa.h"
#include "DSSCommon.h"
#include "BitmapExtraInfo.h"
#include "fitsio.h"

#pragma comment(lib, "cfitsio.lib")

class CFITSHeader
{
public:
	int				m_lWidth;
	int				m_lHeight;
	int				m_lBitsPerPixel;
	int				m_lNrChannels;
	double			m_fExposureTime;
	int				m_lISOSpeed;
	int				m_lGain;
	bool			m_bFloat;
	bool			m_bSigned;
	CString			m_strMake;
	CFATYPE			m_CFAType;
	FITSFORMAT		m_Format;
	CBitmapExtraInfo	m_ExtraInfo;
	SYSTEMTIME		m_DateTime;
	int				m_xBayerOffset;
	int				m_yBayerOffset;
	int				m_bitPix;
	QString			m_filterName;

	CFITSHeader();
	virtual ~CFITSHeader();

	inline int	Height() noexcept
	{
		return m_lHeight;
	};

	inline int	Width() noexcept
	{
		return m_lWidth;
	};

	inline int	BitPerChannels() noexcept
	{
		return m_lBitsPerPixel;
	};

	inline int	NrChannels() noexcept
	{
		return m_lNrChannels;
	};

	inline bool	IsFloat() noexcept
	{
		return m_bFloat;
	};

	inline bool	IsCFA() noexcept
	{
		return (m_CFAType != CFATYPE_NONE);
	};

	inline CFATYPE GetCFAType() noexcept
	{
		return m_CFAType;
	};

	inline bool	IsMaster() noexcept
	{
		return false;
	};

	inline double	GetExposureTime() noexcept
	{
		return m_fExposureTime;
	};

	inline int	GetISOSpeed() noexcept
	{
		return m_lISOSpeed;
	};

	inline int	GetGain() noexcept
	{
		return m_lGain;
	};

	inline SYSTEMTIME	GetDateTime() noexcept
	{
		return m_DateTime;
	};

	inline int	getXOffset() noexcept
	{
		return m_xBayerOffset;
	}

	inline int	getYOffset() noexcept
	{
		return m_yBayerOffset;
	}
};

/* ------------------------------------------------------------------- */
namespace DSS { class ProgressBase; }
using namespace DSS;
class CFITSReader : public CFITSHeader
{
public:
	fitsfile*				m_fits;
	CString					m_strFileName;
	ProgressBase*			m_pProgress;

protected:
	double					m_fGreenRatio;
	double					m_fRedRatio;
	double					m_fBlueRatio;
	double					m_fBrightnessRatio;
	bool					m_bDSI;

private :
	bool	ReadKey(LPCSTR szKey, double & fValue, CString & strComment);
	bool	ReadKey(LPCSTR szKey, double & fValue);
	bool	ReadKey(LPCSTR szKey, int& lValue);
	bool	ReadKey(LPCSTR szKey, CString & strValue);
	void	ReadAllKeys();

public:
	CFITSReader(LPCTSTR szFileName, ProgressBase *	pProgress) :
		CFITSHeader(),
		m_fits{ nullptr },
		m_strFileName{ szFileName },
		m_pProgress{ pProgress },
		m_fGreenRatio{ 1.0 },
		m_fRedRatio{ 1.0 },
		m_fBlueRatio{ 1.0 },
		m_bDSI{ false },
		m_fBrightnessRatio{ 0.0 }
	{}

	virtual ~CFITSReader()
	{
		Close();
	}

	bool Open();
	bool Read();
	virtual bool Close();

	virtual bool OnOpen() { return true; }
	virtual bool OnRead(int, int, double, double, double) { return false; }
	virtual bool OnClose() { return true; }
};

/* ------------------------------------------------------------------- */

class CFITSWriter : public CFITSHeader
{
public:
	fitsfile*				m_fits;
	CString					m_strFileName;
	ProgressBase*			m_pProgress;
	CString					m_strDescription;

private :
	bool	WriteKey(LPCSTR szKey, double fValue, LPCSTR szComment = nullptr);
	bool	WriteKey(LPCSTR szKey, int lValue, LPCSTR szComment = nullptr);
	bool	WriteKey(LPCSTR szKey, LPCTSTR szValue, LPCSTR szComment = nullptr);
	void	WriteAllKeys();

protected:
	void SetFormat(int lWidth, int lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType);

public:
	CFITSWriter(LPCTSTR szFileName, ProgressBase*	pProgress) :
		CFITSHeader(),
		m_fits{ nullptr },
		m_strFileName{ szFileName },
		m_pProgress{ pProgress }
	{
		m_Format = FF_UNKNOWN;
	}

	virtual ~CFITSWriter()
	{
		Close();
	};

	void SetDescription(LPCTSTR szDescription)
	{
		m_strDescription = szDescription;
	};

	void SetFormat(FITSFORMAT FITSFormat)
	{
		m_Format = FITSFormat;
	};

	bool Open();
	bool Write();
	bool Close();

	virtual bool OnOpen() { return true; };
	virtual bool OnWrite(int lX, int lY, double & fRed, double & fGreen, double & fBlue) = 0;
	virtual bool OnClose() { return true; };
};

/* ------------------------------------------------------------------- */
class CBitmapInfo;
class CMemoryBitmap;

CFATYPE GetFITSCFATYPE();
bool GetFITSInfo(LPCTSTR szFileName, CBitmapInfo& BitmapInfo);
bool ReadFITS(LPCTSTR szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress);
bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription,
			   int lISOSpeed, int lGain, double fExposure);
bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, LPCTSTR szDescription);
bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, LPCTSTR szDescriptionL, int lISOSpeed, int lGain, double fExposure);
bool WriteFITS(LPCTSTR szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, LPCTSTR szDescription);
bool IsFITSPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo);
int	LoadFITSPicture(LPCTSTR szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress);
void GetFITSExtension(LPCTSTR szFileName, CString& strExtension);
void GetFITSExtension(fs::path path, CString& strExtension);

