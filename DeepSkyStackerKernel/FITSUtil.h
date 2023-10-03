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
	QString			m_strMake;
	CFATYPE			m_CFAType;
	FITSFORMAT		m_Format;
	CBitmapExtraInfo	m_ExtraInfo;
	QDateTime		m_DateTime;
	int				m_xBayerOffset;
	int				m_yBayerOffset;
	int				m_bitPix;
	QString			m_filterName;
	int				m_nrframes;

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

	inline bool	IsFloat() const noexcept
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

	inline QDateTime GetDateTime() noexcept
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
	fitsfile* m_fits;
	fs::path file;
	ProgressBase* m_pProgress;

protected:
	double					m_fGreenRatio;
	double					m_fRedRatio;
	double					m_fBlueRatio;
	double					m_fBrightnessRatio;
	bool					m_bDSI;

private :
	bool	ReadKey(LPCSTR szKey, double & fValue, QString & strComment);
	bool	ReadKey(LPCSTR szKey, double & fValue);
	bool	ReadKey(LPCSTR szKey, int& lValue);
	bool	ReadKey(LPCSTR szKey, QString & strValue);
	void	ReadAllKeys();

public:
	CFITSReader(const fs::path& path, ProgressBase *	pProgress) :
		CFITSHeader(),
		m_fits{ nullptr },
		file { path },
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
	virtual bool OnRead(int, int, double, double, double) { return true; }
	virtual bool OnClose() { return true; }
};

/* ------------------------------------------------------------------- */

class CFITSWriter : public CFITSHeader
{
public:
	fitsfile* m_fits;
	fs::path file;
	ProgressBase* m_pProgress;
	QString m_strDescription;

private :
	bool	WriteKey(LPCSTR szKey, double fValue, LPCSTR szComment = nullptr);
	bool	WriteKey(LPCSTR szKey, int lValue, LPCSTR szComment = nullptr);
	bool	WriteKey(LPCSTR szKey, const QString& szValue, LPCSTR szComment = nullptr);
	void	WriteAllKeys();

protected:
	void SetFormat(int lWidth, int lHeight, FITSFORMAT FITSFormat, CFATYPE CFAType);

public:
	CFITSWriter(const fs::path& path, ProgressBase*	pProgress) :
		CFITSHeader(),
		m_fits{ nullptr },
		file {path},
		m_pProgress{ pProgress }
	{
		m_Format = FF_UNKNOWN;
	}

	virtual ~CFITSWriter()
	{
		Close();
	};

	void SetDescription(const QString& szDescription)
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
bool GetFITSInfo(const fs::path& szFileName, CBitmapInfo& BitmapInfo);
bool ReadFITS(const fs::path& szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress);
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, const QString& szDescription, int lISOSpeed, int lGain, double fExposure);
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat, const QString& szDescription);
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, FITSFORMAT FITSFormat);
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, const QString& szDescriptionL, int lISOSpeed, int lGain, double fExposure);
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, const QString& szDescription);
bool WriteFITS(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress);
bool IsFITSPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo);
int	LoadFITSPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, const bool ignoreBrightness, ProgressBase* pProgress);
void GetFITSExtension(const fs::path& szFileName, QString& strExtension);
void GetFITSExtension(const fs::path& path, QString& strExtension);
void GetFITSExtension(const QString& path, QString& strExtension);

