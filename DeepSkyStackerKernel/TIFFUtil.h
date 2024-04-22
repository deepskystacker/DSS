#pragma once

/* ------------------------------------------------------------------- */
#include "cfa.h"
#include "DSSCommon.h"

constexpr int TIFFTAG_DSS_BASE             = 50000;
constexpr int TIFFTAG_DSS_NRFRAMES         = (TIFFTAG_DSS_BASE + 0);
constexpr int TIFFTAG_DSS_TOTALEXPOSUREOLD = (TIFFTAG_DSS_BASE + 1);
constexpr int TIFFTAG_DSS_ISO              = (TIFFTAG_DSS_BASE + 2);
constexpr int TIFFTAG_DSS_SETTINGSAPPLIED  = (TIFFTAG_DSS_BASE + 3);
constexpr int TIFFTAG_DSS_BEZIERSETTINGS   = (TIFFTAG_DSS_BASE + 4);
constexpr int TIFFTAG_DSS_ADJUSTSETTINGS   = (TIFFTAG_DSS_BASE + 5);
constexpr int TIFFTAG_DSS_CFA              = (TIFFTAG_DSS_BASE + 6);
constexpr int TIFFTAG_DSS_MASTER           = (TIFFTAG_DSS_BASE + 7);
constexpr int TIFFTAG_DSS_TOTALEXPOSURE    = (TIFFTAG_DSS_BASE + 8);
constexpr int TIFFTAG_DSS_CFATYPE          = (TIFFTAG_DSS_BASE + 9);
constexpr int TIFFTAG_DSS_APERTURE         = (TIFFTAG_DSS_BASE + 10);
constexpr int TIFFTAG_DSS_GAIN             = (TIFFTAG_DSS_BASE + 11);

void DSSTIFFInitialize();

class CTIFFHeader
{
protected:
	uint32_t	w;			// TIFF_LONG
	uint32_t	h;
    uint16_t	spp,		// TIFF_SHORT
				bps,
				photo,
				compression,
				planarconfig,
				sampleformat;
	bool		cfa;
	uint32_t	cfatype;	// TIFF_LONG
	uint32_t	master;		// TIFF_LONG
	QString		strMakeModel;
	double		samplemin;	// TIFF_DOUBLE
	double		samplemax;
	float		exposureTime;	// TIFF_FLOAT
	float		aperture;	// TIFF_FLOAT
	uint32_t	isospeed;	// TIFF_LONG
	int32_t		gain;		// TIFF_LONG
	uint32_t	nrframes;	// TIFF_LONG
	QDateTime	m_DateTime;

public:
	CTIFFHeader();
	virtual ~CTIFFHeader() = default;
	CTIFFHeader(const CTIFFHeader&) = delete;
	CTIFFHeader& operator=(const CTIFFHeader&) = delete;
	CTIFFHeader(CTIFFHeader&&) = delete;

public:
	bool IsCFA() const
	{
		return cfa;
	}

	CFATYPE GetCFAType() const
	{
		return static_cast<CFATYPE>(cfatype);
	}

	bool IsMaster() const
	{
		return master;
	}

	bool IsFloat() const;

	int Height() const
	{
		return h;
	}

	int GetISOSpeed() const
	{
		return isospeed;
	}

	void SetISOSpeed(int lISOSpeed)
	{
		isospeed = lISOSpeed;
	}

	int GetGain() const
	{
		return gain;
	}

	void SetGain(int lGain)
	{
		gain = lGain;
	}

	double GetExposureTime() const
	{
		return exposureTime;
	}

	void SetExposureTime(double fExposureTime)
	{
		exposureTime = fExposureTime;
	}

	double GetAperture() const
	{
		return aperture;
	}

	void SetAperture(double fAperture)
	{
		aperture = fAperture;
	}

	void SetNrFrames(int lNrFrames)
	{
		nrframes = lNrFrames;
	}

	int Width() const
	{
		return w;
	}

	bool IsGray() const
	{
		return (spp == 1);
	}

	bool Is8Bits() const
	{
		return (bps == 8);
	}

	bool Is16Bits() const
	{
		return (bps == 16);
	}

	bool Is32Bits() const
	{
		return (bps == 32);
	}

	int BitPerChannels() const
	{
		return bps;
	}

	int NrChannels() const
	{
		return spp == 1 ? 1 : 3;
	}

	QString	getMakeModel() const
	{
		return strMakeModel;
	}

	QDateTime GetDateTime() const
	{
		return m_DateTime;
	}
};

namespace DSS { class ProgressBase; }
using ProgressBase = DSS::ProgressBase;
using TIFF = struct tiff;

class CTIFFReader : public CTIFFHeader
{
public:
	TIFF* m_tiff;
protected:
	fs::path file;
	ProgressBase* m_pProgress;

public:
	CTIFFReader(const fs::path& p, ProgressBase* pProgress);

	virtual ~CTIFFReader()
	{
		Close();
	}

	bool Open();
	bool Read();
	bool Close();

private:
	// bool getInfo();

	virtual bool OnOpen() { return true; }
	virtual bool OnRead(int, int, double, double, double) { return true; }
	virtual bool OnClose() { return true; };

private:
	void decodeCfaDimPat(int patternSize, const char* tagName);
};

/* ------------------------------------------------------------------- */

class CTIFFWriter : public CTIFFHeader
{
public:
	TIFF* m_tiff;
protected:
	fs::path file;
	ProgressBase* m_pProgress;
	QString m_strDescription;
	TIFFFORMAT m_Format;

protected:
	void SetFormat(int lWidth, int lHeight, TIFFFORMAT TiffFormat, CFATYPE CFAType, bool bMaster);
	void SetCompression(TIFFCOMPRESSION tiffcomp);

public:
	CTIFFWriter(const fs::path& szFileName, ProgressBase* pProgress);
	CTIFFWriter(const CTIFFWriter&) = delete;
	CTIFFWriter& operator=(const CTIFFWriter&) = delete;
	CTIFFWriter(CTIFFWriter&&) = delete;

	virtual ~CTIFFWriter()
	{
		Close();
	}

	void SetDescription(const QString& szDescription)
	{
		m_strDescription = szDescription;
	}

	void SetFormatAndCompression(TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression);

	bool Open();
	bool Write();

private:
	bool Close();

	virtual bool OnOpen() { return true; };
	virtual bool OnWrite(int lX, int lY, double& fRed, double& fGreen, double& fBlue) = 0;
	virtual bool OnClose() { return true; };
};

/* ------------------------------------------------------------------- */
class CBitmapInfo;
class CMemoryBitmap;

bool GetTIFFInfo(const fs::path& szFileName, CBitmapInfo & BitmapInfo);
bool ReadTIFF(const fs::path& szFileName, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase *	pProgress);
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase * pProgress, const QString& szDescription, int lISOSpeed, int lGain, double fExposure, double fAperture);
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress);
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, const QString& szDescription);
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase * pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, const QString& szDescription, int lISOSpeed, int lGain, double fExposure, double fAperture);
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase * pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression, const QString& szDescription);
bool WriteTIFF(const fs::path& szFileName, CMemoryBitmap* pBitmap, ProgressBase* pProgress, TIFFFORMAT TIFFFormat, TIFFCOMPRESSION TIFFCompression);

bool IsTIFFPicture(const fs::path& szFileName, CBitmapInfo & BitmapInfo);
int LoadTIFFPicture(const fs::path& szFileName, CBitmapInfo& BitmapInfo, std::shared_ptr<CMemoryBitmap>& rpBitmap, ProgressBase* pProgress);

/* ------------------------------------------------------------------- */

