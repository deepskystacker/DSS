#include <stdafx.h>
#include "BitmapExt.h"
#include "DSSTools.h"
#include "DSSProgress.h"

#include "RAWUtils.h"
#include <set>
#include <list>
#include <iostream>
#include <locale>
#include <map>
#include <stdexcept>
#include <utility>
#include <float.h>
#include "Multitask.h"
#include "Workspace.h"
#include "zexcept.h"
#include "ztrace.h"

#include "libraw/libraw.h"

LARGE_INTEGER start;
void timerstart(void) { QueryPerformanceCounter(&start); }
float timerend()
{
	LARGE_INTEGER unit, end;
	QueryPerformanceCounter(&end);
	QueryPerformanceFrequency(&unit);
	float msec = (float)(end.QuadPart - start.QuadPart);
	msec /= (float)unit.QuadPart / 1000.0f;
	return msec;
}

class CRAWSettings
{
public :
	bool		m_bRawBayer;
	bool		m_bSuperPixel;

private :
	void	CopyFrom(const CRAWSettings & rs) noexcept
	{
		m_bRawBayer = rs.m_bRawBayer;
		m_bSuperPixel = rs.m_bSuperPixel;
	};

public :
	CRAWSettings() noexcept
	{
		m_bRawBayer		= false;
		m_bSuperPixel	= false;
	};

	CRAWSettings(const CRAWSettings & rs)
	{
		CopyFrom(rs);
	};

	const CRAWSettings & operator = (const CRAWSettings & rs)
	{
		CopyFrom(rs);
		return (*this);
	};
};

typedef std::list<CRAWSettings>		RAWSETTINGSSTACK;

RAWSETTINGSSTACK		g_RawSettingsStack;

/* ------------------------------------------------------------------- */

static bool IsRegistrySuperPixels()
{
	DWORD		bResult = false;
	CWorkspace	workspace;

	bResult = workspace.value("RawDDP/SuperPixels", false).toBool();

	return bResult;
};

/* ------------------------------------------------------------------- */

static bool IsRegistryRawBayer()
{
	bool		bResult = false;
	CWorkspace	workspace;

	bResult = workspace.value("RawDDP/RawBayer", false).toBool();

	return bResult;
};

/* ------------------------------------------------------------------- */

bool IsSuperPixels()
{
	DWORD		bResult = false;

	if (g_RawSettingsStack.size())
		bResult = g_RawSettingsStack.back().m_bSuperPixel;
	else
		bResult = IsRegistrySuperPixels();

	return bResult;
};

/* ------------------------------------------------------------------- */

bool IsRawBayer()
{
	DWORD		bResult = false;

	if (g_RawSettingsStack.size())
		bResult = g_RawSettingsStack.back().m_bRawBayer;
	else
		bResult = IsRegistryRawBayer();

	return bResult;
};

/* ------------------------------------------------------------------- */

bool IsRawBilinear()
{
	CWorkspace	workspace;
	QString		strInterpolation;

	strInterpolation = workspace.value("RawDDP/Interpolation", "").toString();

	return strInterpolation.isEmpty() || (strInterpolation == "Bilinear");
};

/* ------------------------------------------------------------------- */

bool IsRawAHD()
{
	CWorkspace	workspace;
	QString		strInterpolation;

	workspace.value("RawDDP/Interpolation", strInterpolation);

	return (strInterpolation.isEmpty() || (strInterpolation == "AHD"));
};

/* ------------------------------------------------------------------- */

void	PushRAWSettings(bool bSuperPixel, bool bRawBayer)
{
	CRAWSettings		rs;

	if (IsRegistryRawBayer() || IsRegistrySuperPixels())
	{
		rs.m_bRawBayer		= bRawBayer;
		rs.m_bSuperPixel	= bSuperPixel;

		g_RawSettingsStack.push_back(rs);
	};
};

/* ------------------------------------------------------------------- */

void	PopRAWSettings()
{
	if (g_RawSettingsStack.size())
		g_RawSettingsStack.pop_back();
};

/* ------------------------------------------------------------------- */

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <typeinfo>

class BitMapFiller
{

private:
	CDSSProgress *			m_pProgress;
	bool					m_bStarted;
	CFATYPE					m_CFAType;
	CMemoryBitmap *			m_pBitmap;
	DWORD					m_dwPos;
	PixelIterator			m_PixelIt;
	DWORD					m_dwCurrentX,
							m_dwCurrentY;
	BYTE *					m_pBuffer;
	DWORD					m_dwBufferSize;
	DWORD					m_dwBufferReadPos;
	DWORD					m_dwBufferWritePos;
	LONG					m_lWidth,
							m_lHeight,
							m_lMaxColors;
	LONG					m_lBytePerChannel;
	bool					m_bGrey;
	double					m_fRedScale,
							m_fGreenScale,
							m_fBlueScale;

private:
	void	AdjustColor(double & fColor, double fAdjust) noexcept
	{
		const double	fResult = fColor * fAdjust;

		fColor = min(static_cast<double>(MAXWORD - 1), fResult);
	};

	void	AddToBuffer(const void * buffer, DWORD lSize)
	{
		if (!m_bStarted)
			Start();
		ZASSERTSTATE(0 != m_lWidth);
		ZASSERTSTATE(0 != m_lHeight);
		ZASSERTSTATE(0 != m_lMaxColors);

		if (m_lMaxColors > 255)
			m_lBytePerChannel = 2;
		else
			m_lBytePerChannel = 1;

		//
		// Now process the bitmap
		//
		if (!m_pBuffer)
		{
			// Alloc the buffer
			m_dwBufferSize = 2 * lSize;
			ZTRACE_RUNTIME("Allocating image buffer of %d bytes", m_dwBufferSize);
			m_pBuffer = (BYTE*)malloc(m_dwBufferSize);
			if (nullptr == m_pBuffer)
			{
				ZOutOfMemory e("Could not allocate storage for image buffer");
				ZTHROW(e);
			}

			m_dwBufferReadPos = 0;
			m_dwBufferWritePos = 0;
		};
		if (lSize > m_dwBufferSize - m_dwBufferWritePos)
		{
			// realloc the buffer
			ZTRACE_RUNTIME("Re-allocating image buffer to %d bytes", lSize + m_dwBufferWritePos * 2);
			BYTE* temp = (BYTE*)realloc(m_pBuffer, lSize + m_dwBufferWritePos * 2);
			if (nullptr == temp)
			{
				ZOutOfMemory e("Could not re-allocate storage for image buffer");
				ZTHROW(e);
			}

			m_pBuffer = temp;
			m_dwBufferSize = lSize + m_dwBufferWritePos * 2;
		};

		BYTE *				pWrite = m_pBuffer;
		BYTE *				pRead = m_pBuffer;
		LONG				lToRead;
		bool				bEnd = false;

		pWrite += m_dwBufferWritePos;
		pRead += m_dwBufferReadPos;
		memcpy(m_pBuffer, buffer, lSize);
		m_dwBufferWritePos += lSize;

		while (!bEnd)
		{
			bEnd = true;
			lToRead = m_dwBufferWritePos - m_dwBufferReadPos;
			// Read pixels
			{
				if (m_bGrey)
				{
					if (lToRead >= m_lBytePerChannel)
					{
						// Read gray
						double		fGrey;
						if (m_lBytePerChannel == 2)
						{
							fGrey = ((*(pRead + 0)) << 8) + (*(pRead + 1));
							m_dwBufferReadPos += 2;
							pRead += 2;
						}
						else
						{
							fGrey = ((*(pRead + 0)) << 8);
							m_dwBufferReadPos++;
							pRead++;
						};

						if (m_dwCurrentX >= m_lWidth)
						{
							m_dwCurrentY++;
							m_dwCurrentX = 0;
							m_PixelIt->Reset(0, m_dwCurrentY);
							m_dwPos = m_dwCurrentY + 1;
						};

						switch (GetBayerColor(m_dwCurrentX, m_dwCurrentY, m_CFAType))
						{
						case BAYER_RED:
							AdjustColor(fGrey, m_fRedScale);
							break;
						case BAYER_GREEN:
							AdjustColor(fGrey, m_fGreenScale);
							break;
						case BAYER_BLUE:
							AdjustColor(fGrey, m_fBlueScale);
							break;
						};

						m_PixelIt->SetPixel((double)fGrey / 256.0);
						(*m_PixelIt)++;
						m_dwCurrentX++;

						bEnd = false;
					};
				}
				else
				{
					if (lToRead >= m_lBytePerChannel * 3)
					{
						// Read RGB
						double				fRed, fGreen, fBlue;

						if (m_lBytePerChannel == 2)
						{
							fRed = ((*(pRead + 0)) << 8) + (*(pRead + 1));
							fGreen = ((*(pRead + 2)) << 8) + (*(pRead + 3));
							fBlue = ((*(pRead + 4)) << 8) + (*(pRead + 5));
							m_dwBufferReadPos += 6;
							pRead += 6;
						}
						else
						{
							fRed = ((*(pRead + 0)) << 8);
							fGreen = ((*(pRead + 1)) << 8);
							fBlue = ((*(pRead + 2)) << 8);
							m_dwBufferReadPos += 3;
							pRead += 3;
						};

						if (m_dwCurrentX >= m_lWidth)
						{
							m_dwCurrentY++;
							m_dwCurrentX = 0;
							m_PixelIt->Reset(0, m_dwCurrentY);
							m_dwPos = m_dwCurrentY + 1;
						};

						AdjustColor(fRed, m_fRedScale);
						AdjustColor(fGreen, m_fGreenScale);
						AdjustColor(fBlue, m_fBlueScale);

						m_PixelIt->SetPixel(fRed / 256.0, fGreen / 256.0, fBlue / 256.0);
						m_dwCurrentX++;
						(*m_PixelIt)++;

						bEnd = false;
					};
				};
			}

		};

		// Adjust Buffer
		LONG			lToMove = m_dwBufferWritePos - m_dwBufferReadPos;

		if (lToMove)
		{
			memmove(m_pBuffer, pRead, lToMove);
			m_dwBufferWritePos -= m_dwBufferReadPos;
			m_dwBufferReadPos = 0;
		}
		else
			m_dwBufferReadPos = m_dwBufferWritePos = 0;
	};
private:

public:

    BitMapFiller(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
    {
        m_pBitmap = pBitmap;
        m_pProgress = pProgress;
        m_bStarted = false;
        m_fRedScale = 1.0;
        m_fGreenScale = 1.0;
        m_fBlueScale = 1.0;
        m_lBytePerChannel = 2;				// Never going to handle 8 bit data !!
        m_lHeight = 0;
        m_lWidth = 0;
        m_CFAType = CFATYPE_NONE;
        m_dwPos = 0;
        m_dwCurrentX = 0;
        m_dwCurrentY = 0;
        m_pBuffer = nullptr;
        m_dwBufferSize = 0;
        m_dwBufferReadPos = 0;
        m_dwBufferWritePos = 0;
        m_lMaxColors = 0;
        m_bGrey = false;
    }

	virtual ~BitMapFiller()
	{
		if (m_pBuffer)
			free(m_pBuffer);
	};

	void	SetWhiteBalance(double fRedScale, double fGreenScale, double fBlueScale) noexcept
	{
		m_fRedScale = fRedScale;
		m_fGreenScale = fGreenScale;
		m_fBlueScale = fBlueScale;
	};

	void	SetCFAType(CFATYPE CFAType) noexcept
	{
		m_CFAType = CFAType;
	};

	void	setGrey(bool grey) noexcept
	{
		m_bGrey = grey;
	};

	void	setWidth(LONG width) noexcept
	{
		m_lWidth = width;
	};

	void	setHeight(LONG height) noexcept
	{
		m_lHeight = height;
	};

	void	setMaxColors(LONG maxcolors) noexcept
	{
		m_lMaxColors = maxcolors;
	};

	int	Start()
	{
		ZFUNCTRACE_RUNTIME();
		//
		// Do initialisation not done by ctor
		//
		m_bStarted = true;
		m_pBuffer = nullptr;
		m_dwBufferSize = 0;
		m_dwBufferReadPos = 0;
		m_dwBufferWritePos = 0;
		m_dwPos = 0;
		m_dwCurrentX = 0;
		m_dwCurrentY = 0;

		//
		// If the bitmap is a 16-bit grayscale bitmap, then set the CFA type
		//
		C16BitGrayBitmap *			pGray16Bitmap = dynamic_cast<C16BitGrayBitmap *>(m_pBitmap);
		if (pGray16Bitmap)
			pGray16Bitmap->SetCFAType(m_CFAType);
		//
		// Initialise the progress dialog
		//
		if (m_pProgress)
			m_pProgress->Start2(nullptr, m_pBitmap->Height());

		m_pBitmap->GetIterator(&m_PixelIt);

		return 0;
	};

#if (0)
	int	Printf(const char *format, va_list va)
	{
		int				nResult;
		CString			strText;

		strText.FormatV(format, va);
		nResult = strText.GetLength();

		AddToBuffer(strText.GetBuffer(10000), nResult);

		if (m_pProgress)
			m_pProgress->Progress2(nullptr, m_dwPos);

		return nResult;
	};
#endif

	size_t Write(const void *buffer, size_t size, size_t count)
	{
		AddToBuffer(buffer, static_cast<DWORD>(size*count));
		if (m_pProgress)
			m_pProgress->Progress2(nullptr, m_dwPos);

		return count;
	};
};

class DSSLibRaw : public LibRaw
{
public:
	DSSLibRaw() noexcept {};
	~DSSLibRaw() {};
	void setBitMapFiller(BitMapFiller * pFiller) noexcept
	{
		pDSSBitMapFiller = pFiller;
	};

	int			dcraw_ppm_tiff_writer(const char *filename);
	inline unsigned		get_fuji_layout() noexcept
	{
		return libraw_internal_data.unpacker_data.fuji_layout;
	};

	void adjust_bl() noexcept { this->LibRaw::adjust_bl(); }
	virtual int is_phaseone_compressed() noexcept { return this->LibRaw::is_phaseone_compressed(); }

protected:
	void        write_ppm_tiff();

private:
	BitMapFiller * pDSSBitMapFiller = nullptr;

};

#define Thread   __declspec( thread )

// Thread DSSLibRaw	rawProcessor;
DSSLibRaw rawProcessor;

/* ------------------------------------------------------------------- */

//static CString			g_strInputFileName;
static Thread CDSSProgress *	g_Progress;

/* ------------------------------------------------------------------- */

class CRawDecod
{
private :
	CString			m_strFileName;
	CString			m_strModel;
	CString			m_strMake;
	LONG			m_lISOSpeed;
	double			m_fExposureTime;
	double			m_fAperture;
	LONG			m_lHeight,
					m_lWidth;
	bool			m_bColorRAW;
	CFATYPE			m_CFAType;
	SYSTEMTIME		m_DateTime;
    bool            m_isRawFile;

#define P1		rawProcessor.imgdata.idata
#define P2		rawProcessor.imgdata.other

#define mnLens rawProcessor.imgdata.lens.makernotes
#define exifLens rawProcessor.imgdata.lens
#define ShootingInfo rawProcessor.imgdata.shootinginfo

#define S		rawProcessor.imgdata.sizes
#define O		rawProcessor.imgdata.params
#define C		rawProcessor.imgdata.color
#define T		rawProcessor.imgdata.thumbnail

#define Canon	rawProcessor.imgdata.makernotes.canon
#define Fuji	rawProcessor.imgdata.makernotes.fuji
#define Oly		rawProcessor.imgdata.makernotes.olympus

#define RawData	rawProcessor.imgdata.rawdata
#define IOParams	rawProcessor.imgdata.rawdata.ioparams

public :
    CRawDecod(LPCTSTR szFile) noexcept :
		m_strFileName(szFile)
    {
        ZFUNCTRACE_RUNTIME();
        m_bColorRAW = false;
        m_CFAType = CFATYPE_NONE;
        m_DateTime.wYear = 0;
        m_lISOSpeed = 0;
        m_fExposureTime = 0;
        m_fAperture = 0;
        m_lHeight = 0;
        m_lWidth = 0;

        m_isRawFile = rawProcessor.open_file(szFile) == LIBRAW_SUCCESS;

        if (m_isRawFile)
        {
            m_strMake = P1.normalized_make;
            m_strModel = P1.normalized_model;
            m_lHeight = S.iheight;
            m_lWidth = S.iwidth;
            m_lISOSpeed = P2.iso_speed;

            if (_finite(P2.shutter))
                m_fExposureTime = P2.shutter;
            else
                m_fExposureTime = 0;
            if (_finite(P2.aperture))
                m_fAperture = P2.aperture;
            else
                m_fAperture = 0.0;

            // Retrieve the Date/Time
            memset(&m_DateTime, 0, sizeof(m_DateTime));
            tm *		pdatetime;

            if (P2.timestamp)
            {
                pdatetime = localtime(&(P2.timestamp));
                if (pdatetime)
                {
                    m_DateTime.wDayOfWeek = pdatetime->tm_wday;
                    m_DateTime.wDay = pdatetime->tm_mday;
                    m_DateTime.wMonth = pdatetime->tm_mon + 1;
                    m_DateTime.wYear = pdatetime->tm_year + 1900;
                    m_DateTime.wHour = pdatetime->tm_hour;
                    m_DateTime.wMinute = pdatetime->tm_min;
                    m_DateTime.wSecond = pdatetime->tm_sec;
                };
            };

            m_bColorRAW = P1.is_foveon || !(P1.filters);
            if (1 == P1.filters || 9 == P1.filters)
            {
                //
                // This is somewhat of a lie as the only the Foveon sensors
                // create full color raw files.  However by telling this lie
                // we can use libraw to decode and interpolate Fujitsu X-Trans
                // and Leaf Catchlight images
                //
                if (1 == P1.filters) ZTRACE_RUNTIME("Image is from a Leaf Catchlight");
                else ZTRACE_RUNTIME("Image is from a Fujitsu X-Trans Sensor");
                m_bColorRAW = true;
            }

            m_CFAType = GetCurrentCFAType();
        };
    }

	virtual ~CRawDecod()
	{
		ZFUNCTRACE_RUNTIME();
		rawProcessor.recycle();
	};

	bool	IsRawFile() const;
	bool	LoadRawFile(CMemoryBitmap * pBitmap, CDSSProgress * pProgress = nullptr, bool bThumb = false);

	bool	GetModel(CString & strModel)
	{
		strModel = m_strMake + _T(" ") + m_strModel;
		return true;
	};

	void checkCameraSupport(const CString& strModel);

	LONG	GetISOSpeed() noexcept
	{
		return m_lISOSpeed;
	};

	double	GetExposureTime() noexcept
	{
		return m_fExposureTime;
	};

	double	getAperture() noexcept
	{
		return m_fAperture;
	};

	LONG	Width() noexcept
	{
		return m_lWidth;
	};

	LONG	Height() noexcept
	{
		return m_lHeight;
	};

	bool	IsColorRAW() noexcept
	{
		return m_bColorRAW;
	};

	CFATYPE	GetCFAType()
	{
		return m_CFAType;
	};

	CFATYPE GetCurrentCFAType()
	{
		if (P1.colors == 4 && IOParams.mix_green == 0)
		{	// CYMG Bayer pattern
			switch (P1.filters)
			{
			case 0xb4b4b4b4:
				return CFATYPE_GMYC;
				break;
			case 0xe1e4e1e4:
				return CFATYPE_GMCYMGCY;
				break;
			case 0x1e4e1e4e:
				return CFATYPE_CYGMCYMG;
				break;
			};

		}
		else
		{	// RGB Bayer Pattern
			switch (P1.filters)
			{
			case 0x1e1e1e1e:
			case 0x16161616:
				return CFATYPE_BGGR;
				break;
			case 0xe1e1e1e1:
			case 0x61616161:
				return CFATYPE_GRBG;
				break;
			case 0x4b4b4b4b:
			case 0x49494949:
				return CFATYPE_GBRG;
				break;
			case 0xb4b4b4b4:
			case 0x94949494:
				return CFATYPE_RGGB;
				break;
			default:
				return CFATYPE_NONE;
			};
		}

		return CFATYPE_NONE;
	};

	/* ------------------------------------------------------------------- */

	SYSTEMTIME GetDateTime()
	{
		return m_DateTime;
	};
};

void CRawDecod::checkCameraSupport(const CString& strModel)
{
	bool result = false;
	CStringA strModelA(strModel);
	const char * camera = static_cast<LPCSTR>(strModelA);

	static std::set<std::string> checkedCameras;

	//
	// If we've already checked this camera type, then just bail out so
	// complaints about unsupported cameras are only issued once.
	//
	auto it = checkedCameras.find(camera);

	if (it != checkedCameras.end() )
	{
		return;
	}

	static std::vector<std::string> supportedCameras;

	if (0 == supportedCameras.size())
	{
		const char **cameraList = rawProcessor.cameraList();
		const size_t count = rawProcessor.cameraCount();
		supportedCameras.reserve(count);

		//
		// Copy LibRaw's supported camera list
		//
		for (size_t i = 0; i != count; ++i)
		{
			if (nullptr != cameraList[i])
			{
				supportedCameras.emplace_back(cameraList[i]);
			}
		}
		//
		// Sort the camera names using std::sort and a case independent comparison lambda function
		// See: https://stackoverflow.com/questions/33379846/case-insensitive-sorting-of-an-array-of-strings
		//
			sort(supportedCameras.begin(), supportedCameras.end(), 
				[](const auto& lhs, const auto& rhs) 
			{ 
				const auto result = mismatch(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
					[](const auto& lhs, const auto& rhs) {return tolower(lhs) == tolower(rhs); });
				return result.second != rhs.cend() && (result.first == lhs.cend() || tolower(*result.first) < tolower(*result.second));
			});
	}

	//
	// The camera type hasn't already been checked, so search the LibRaw supported camera list
	//
	result = std::binary_search(supportedCameras.begin(), supportedCameras.end(), camera,
		[](const std::string &lhs, const std::string &rhs) noexcept
	{
		const char* pclhs = lhs.c_str();
		const char* pcrhs = rhs.c_str();
		size_t len = strlen(pclhs);
		const size_t szrhs = strlen(pcrhs);
		// choose the shorter length
		len = (len > szrhs) ? szrhs : len;
		const int result = _strnicmp(pclhs, pcrhs, len);
		return (result < 0) ? true : false;
	}
		);

	//
	// Now we know whether this camera is supported or not, remember we've seen it before
	//
	checkedCameras.insert(camera);

	//
	// If the camera isn't supported complain, but only once
	//
	if (false == result)
	{
		CString errorMessage;
		errorMessage.Format(IDS_CAMERA_NOT_SUPPORTED, strModel);
#if defined(_CONSOLE)
		std::cerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONWARNING);
#endif
	}

	return;
};
/* ------------------------------------------------------------------- */

bool CRawDecod::LoadRawFile(CMemoryBitmap * pBitmap, CDSSProgress * pProgress, bool bThumb)
{
	ZFUNCTRACE_RUNTIME();
	bool		bResult = true;
	BitMapFiller *		pFiller = nullptr;
	int			ret = 0;

	pBitmap->Init(m_lWidth, m_lHeight);
	pBitmap->SetISOSpeed(m_lISOSpeed);
	pBitmap->SetExposure(m_fExposureTime);
	pBitmap->SetAperture(m_fAperture);
	pBitmap->m_DateTime = m_DateTime;

	CString			strDescription;
	GetModel(strDescription);

	//
	// If it's a DNG file, we don't need to check for camera support, but if
	// we're processing a true raw file then check that the camera is supported.
	//
	if (0 == P1.dng_version)
	{
		checkCameraSupport(strDescription);
	};

	pBitmap->SetDescription(strDescription);

	const		int maxargs = 50;
	CWorkspace	workspace;
	double		fBrightness = 1.0;
	double		fRedScale = 1.0;
	double		fBlueScale = 1.0;
	double		fGreenScale = 1.0;

	//DWORD		bSuperPixels;
	//DWORD		bRawBayer;
	//bool		bBilinear;
	//bool		bAHD;
	DWORD		bBlackPointTo0 = 0;
	DWORD		bValue = 0;

	do	// Do once!
	{
		fBrightness = workspace.value("RawDDP/Brightness").toDouble();
		fRedScale = workspace.value("RawDDP/RedScale").toDouble();
		fBlueScale = workspace.value("RawDDP/BlueScale").toDouble();

		fGreenScale = fBrightness;
		fRedScale *= fBrightness;
		fBlueScale *= fBrightness;

		//bSuperPixels = IsSuperPixels();
		//bRawBayer    = IsRawBayer();
		//bBilinear	 = !bSuperPixels && !bRawBayer && IsRawBilinear();
		//bAHD		 = !bSuperPixels && !bRawBayer && IsRawAHD();

		// No Auto White Balance
		O.use_auto_wb = false;

		//
		// Do we disable all White Balance processing?
		//
		bValue = workspace.value("RawDDP/NoWB", false).toBool();
		if (bValue)
		{
			// Yes, so set the user white balance multipliers to 1.0
			O.user_mul[0] = O.user_mul[1] = O.user_mul[2] = O.user_mul[3] = 1.0;
		}
		else
		{
			// No, so set the user white balance multipliers to 0.0
			O.user_mul[0] = O.user_mul[1] = O.user_mul[2] = O.user_mul[3] = 0.0;
		}

		bValue = workspace.value("RawDDP/CameraWB", false).toBool();
		O.use_camera_wb = bValue ? 1 : 0;

		// Don't stretch or rotate raw pixels (equivalent to dcraw -j)
		O.use_fuji_rotate = 0;

		// Don't flip the image (equivalent to dcraw -t 0)
		O.user_flip = 0;

		// Output color space : raw-> sRGB (default)
		/*
		argv[argc] = _T("-o");
		argc++;
		argv[argc] = _T("0");
		argc++;*/

		bBlackPointTo0 = workspace.value("RawDDP/BlackPointTo0", false).toBool();
		O.user_black = bBlackPointTo0 ? 0 : -1;

		// Output is 16 bits (equivalent of dcraw flag -4)
		O.gamm[0] = O.gamm[1] = O.no_auto_bright = 1;
		O.output_bps = 16;

		g_Progress = pProgress;

		ZTRACE_RUNTIME("Calling LibRaw::unpack()");
		if ((ret = rawProcessor.unpack()) != LIBRAW_SUCCESS)
		{
			bResult = false;
			ZTRACE_RUNTIME("Cannot unpack %s: %s", m_strFileName, libraw_strerror(ret));
		}
		if (!bResult) break;

		//
		// Create the class that populates the bitmap
		//
		pFiller = new BitMapFiller(pBitmap, pProgress);
		pFiller->SetWhiteBalance(fRedScale, fGreenScale, fBlueScale);
		// Get the Colour Filter Array type and set into the bitmap filler
		m_CFAType = GetCurrentCFAType();
		pFiller->SetCFAType(m_CFAType);

#define RAW(row,col) \
	raw_image[(row)*S.width+(col)]

		//
		// Get our endian-ness so we can swap bytes if needed (always on Windows).
		//
		const bool littleEndian = htons(0x55aa) != 0x55aa;

		unsigned short *raw_image = nullptr;
		void * buffer = nullptr;		// Used for debugging only (memory window)

		if (!m_bColorRAW)
		{
			ZTRACE_RUNTIME("Processing Bayer pattern raw image data");
			//
			// The initial openmp changes were made by David Partridge, but it was
			// Vitali Pelenjow who made it work without the critical sections
			// killing the performance.
			//
			// Set up a temporary image array of unsigned short that will be:
			//
			// 1) Filled in from the Fujitsu Super-CCD image array in
			//    Rawdata.raw_image, or
			//
			// 2) will be copied from the image portion of Rawdata.raw_image
			//    excluding the frame (Top margin, Left Margin).
			//
			raw_image =
				(unsigned short *)calloc(static_cast<size_t>(S.height)*static_cast<size_t>(S.width), sizeof(unsigned short));
			if (nullptr == raw_image)
			{
				ZOutOfMemory e("Could not allocate storage for RAW image");
				ZTHROW(e);
			}

			const int fuji_width = rawProcessor.is_fuji_rotated();
			const unsigned fuji_layout = rawProcessor.get_fuji_layout();
			buffer = raw_image;		// only for memory window debugging

			if (fuji_width)   // Are we processing a Fuji Super-CCD image?
			{
				ZTRACE_RUNTIME("Converting Fujitsu Super-CCD image to regular raw image");
#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
				for (int row = 0; row < S.raw_height - S.top_margin * 2; row++)
				{
					for (int col = 0; col < fuji_width << int(!fuji_layout); col++)
					{
						unsigned r, c;

						if (fuji_layout)
						{
							r = fuji_width - 1 - col + (row >> 1);
							c = col + ((row + 1) >> 1);
						}
						else
						{
							r = fuji_width - 1 + row - (col >> 1);
							c = row + ((col + 1) >> 1);
						}
						if (r < S.height && c < S.width)
							RAW(r, c)
							= RawData.raw_image[(row + S.top_margin)*S.raw_pitch / 2 + (col + S.left_margin)];
					}
				}
			}
			else
			{
				ZTRACE_RUNTIME("Extracting real image data (excluding the frame) from RawData.raw_image");

				//
				// This is a regular RAW file so no Fuji Super-CCD stuff
				//
				// Just copy the "real image" portion of the data excluding
				// the frame
				//
				buffer = raw_image;
#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
				for (int row = 0; row < S.height; row++)
				{
					for (int col = 0; col < S.width; col++)
					{
						RAW(row, col)
							= RawData.raw_image[(row + S.top_margin)*S.raw_pitch / 2 + (col + S.left_margin)];
					}
				}
			}

			//
			// Now process the data that raw_image points to which is either
			//
			// 1) The output of post processing the Fuji Super-CCD raw,
			//    stored in the USHORT array hung off raw_image, or
			//
			// 2) Normal common or garden raw Bayer matrix data that's been
			//    copied from RawData.raw_image to raw_image (less the frame)
			//
			// Either way we should now be processing a regular greyscale 16-bit
			// pixel array which has an associated Bayer Matrix
			//
			pFiller->setGrey(true);
			pFiller->setWidth(S.width);
			pFiller->setHeight(S.height);
			pFiller->setMaxColors((1 << 16) - 1);

			// Report User Black Point over-ride
			if (0 == O.user_black)
				ZTRACE_RUNTIME("User set Black Point to 0");

			//
			// Before doing dark subtraction, normalise C.black / C.cblack[]
			//
			ZTRACE_RUNTIME("Before adjust_bl() C.black = %d.", C.black);
			ZTRACE_RUNTIME("First 10 C.cblack elements\n  %d, %d, %d, %d\n  %d, %d\n  %d, %d, %d, %d",
				C.cblack[0], C.cblack[1], C.cblack[2], C.cblack[3],
				C.cblack[4], C.cblack[5],
				C.cblack[6], C.cblack[7], C.cblack[8], C.cblack[9]);
			rawProcessor.adjust_bl();

			//
			// This code is based on code from LibRaw Version 19.2, specifically method:
			//
			//   int LibRaw::subtract_black_internal()
			//
			// found at line 4532 in source file libraw_cxx.cpp
			//
			// Do dark subtraction on the image.   If a user defined black level has
			// been set (it will be zero) then use that, otherwise just use the black
			// level for the camera.
			//
			// Note that this is only done on real image data, not the frame
			//
			// While doing so collect the largest value in the image data.
			//
			ZTRACE_RUNTIME("Subtracting black level of C.black = %d from raw_image data.", C.black);
			ZTRACE_RUNTIME("First 10 C.cblack elements\n  %d, %d, %d, %d\n  %d, %d\n  %d, %d, %d, %d",
				C.cblack[0], C.cblack[1], C.cblack[2], C.cblack[3],
				C.cblack[4], C.cblack[5],
				C.cblack[6], C.cblack[7], C.cblack[8], C.cblack[9]);

			if (!rawProcessor.is_phaseone_compressed() &&
				(C.cblack[0] || C.cblack[1] || C.cblack[2] || C.cblack[3] || (C.cblack[4] && C.cblack[5])))
			{
				int cblk[4], i;
				for (i = 0; i < 4; i++)
					cblk[i] = C.cblack[i];

				const int size = S.height * S.width;
				int dmax = 0;	// Maximum value of pixels in entire image.
				int lmax = 0;	// Local (or Loop) maximum value found in the 'for' loops below. For OMP.
				if (C.cblack[4] && C.cblack[5])
				{
#if defined(_OPENMP)
#pragma omp parallel default(none) shared(dmax) firstprivate(lmax)
					{
#pragma omp for
#endif
						for (i = 0; i < size; i++)
						{
							int val = raw_image[i];
							val -= C.cblack[6 + i / S.width % C.cblack[4] * C.cblack[5] + i % S.width % C.cblack[5]];
							val -= cblk[i & 3];
							raw_image[i] = max(0, min(val, 65535));
							lmax = val > lmax ? val : lmax;
						}
#if defined(_OPENMP)
#pragma omp critical
#endif
						dmax = lmax > dmax ? lmax : dmax; // For non-OMP case this is equal to dmax = lmax.
#if defined(_OPENMP)
					}
#endif
				}
				else
				{
#if defined(_OPENMP)
#pragma omp parallel default(none) shared(dmax) firstprivate(lmax)
					{
#pragma omp for
#endif
						for (i = 0; i < size; i++)
						{
							int val = raw_image[i];
							val -= cblk[i & 3];
							raw_image[i] = max(0, min(val, 65535)); ;
							lmax = val > lmax ? val : lmax;
						}
#if defined(_OPENMP)
#pragma omp critical
#endif
						dmax = lmax > dmax ? lmax : dmax; // For non-OMP case this is equal to dmax = lmax.
#if defined(_OPENMP)
					}
#endif
				}
				C.data_maximum = dmax & 0xffff;
				C.maximum -= C.black;
				memset(&C.cblack, 0, sizeof(C.cblack)); // Yeah, we used cblack[6+] values too!
				C.black = 0;
			}
			else
			{
				// Nothing to Do, maximum is already calculated, black level is 0, so no change
				// only calculate channel maximum;
				int dmax = 0;	// Maximum value of pixels in entire image.
				int lmax = 0;	// Local (or Loop) maximum value found in the 'for' loop below. For OMP.
#if defined(_OPENMP)
#pragma omp parallel default(none) shared(dmax) firstprivate(lmax)
				{
#pragma omp for
#endif
					for (int i = 0; i < S.height * S.width; i++)
						if (lmax < raw_image[i])
							lmax = raw_image[i];
#if defined(_OPENMP)
#pragma omp critical
#endif
					dmax = lmax > dmax ? lmax : dmax; // For non-OMP case this is equal to dmax = lmax.
#if defined(_OPENMP)
				}
#endif

				C.data_maximum = dmax;
			}

			//
			// The image data needs to be scaled to the "white balance co-efficients"
			// Currently do not handle "Auto White Balance"
			//
			float pre_mul[4] = {0.0, 0.0, 0.0, 0.0 };
			if (1.0 == O.user_mul[0])
			{
				ZTRACE_RUNTIME("No White Balance processing.");
				memcpy(pre_mul, O.user_mul, sizeof pre_mul);
			}
			else if (1 == O.use_camera_wb && -1 != C.cam_mul[0])
			{
				ZTRACE_RUNTIME("Using Camera White Balance (as shot).");
				memcpy(pre_mul, C.cam_mul, sizeof pre_mul);
			}
			else
			{
				ZTRACE_RUNTIME("Using Daylight White Balance.");
				memcpy(pre_mul, C.pre_mul, sizeof pre_mul);
			}
			ZTRACE_RUNTIME("White balance co-efficients being used are %f, %f, %f, %f",
				pre_mul[0], pre_mul[1], pre_mul[2], pre_mul[3]);

			if (0 == pre_mul[3]) pre_mul[3] = P1.colors < 4 ? pre_mul[1] : 1;

			//
			// Now apply a linear stretch to the raw data, scale to the "saturation" level
			// not to the value of the pixel with the greatest value (which may be higher
			// than the saturation level).
			//

			double dmin, dmax; int c = 0;

			for (dmin = DBL_MAX, dmax = c = 0; c < 4; c++)
			{
				if (dmin > pre_mul[c])
					dmin = pre_mul[c];
				if (dmax < pre_mul[c])
					dmax = pre_mul[c];
			}

			float scale_mul[4];

			for (c = 0; c < 4; c++)	scale_mul[c] = (pre_mul[c] /= dmin) * (65535.0 / C.maximum);

			ZTRACE_RUNTIME("Maximum value pixel has value %d", C.data_maximum);
			ZTRACE_RUNTIME("Saturation level is %d", C.maximum);
			ZTRACE_RUNTIME("Applying linear stretch to raw data.  Scale values %f, %f, %f, %f",
				scale_mul[0], scale_mul[1], scale_mul[2], scale_mul[3]);

#if defined(_OPENMP)
#pragma omp parallel for default(none)
#endif
			for (int row = 0; row < S.height; row++)
			{
				for (int col = 0; col < S.width; col++)
				{
					// What colour will this pixel become
					const int colour = rawProcessor.COLOR(row, col);

					const float val = scale_mul[colour] *
						(float)(RAW(row, col));
					RAW(row, col) = max(0, min(int(val), 65535));
				}
			}

			// Convert raw data to big-endian
			if (littleEndian)
#if defined(_OPENMP)
#pragma omp parallel for default(none)
				for (int i = 0; i < S.height * S.width; i++)
				{
					raw_image[i] = _byteswap_ushort(raw_image[i]);
				}
#else
				_swab(
					(char*)(raw_image),
					(char*)(raw_image),
					S.height*S.width*sizeof(unsigned short));		// Use number of rows times row width in BYTES!!
#endif

			for (int row = 0; row < S.height; row++)
			{
				buffer = &RAW(row, 0);
				// Write raw pixel data into our private bitmap format
				pFiller->Write(buffer, sizeof(unsigned short), S.width);
			}
		}
		else
		{
			//
			// This is a "full colour" RAW file, so we can use full libraw
			// processing and capture the PPM file output.
			//
			if (LIBRAW_SUCCESS != (ret = rawProcessor.dcraw_process()))
			{
				ZTRACE_RUNTIME("Cannot do postprocessing on %s: %s", m_strFileName, libraw_strerror(ret));
				if (LIBRAW_FATAL_ERROR(ret))
					bResult = false;
			}
			if (!bResult) break;

			ZTRACE_RUNTIME("Processing Foveon or Fuji X-Trans raw image data");
			//
			// Now capture the output using our over-ridden methods
			//
			// Set up the intercept code to write the image data to our bitmap instead of
			// to an external file, and invoke the overridden dcraw_ppm_tiff_writer()
			//
			rawProcessor.setBitMapFiller(pFiller);
			if (LIBRAW_SUCCESS != (ret = rawProcessor.dcraw_ppm_tiff_writer("")))
			{
				bResult = false;
				ZTRACE_RUNTIME("Cannot write image data to bitmap %s", libraw_strerror(ret));
			}
		}
#undef RAW
		//
		// If we allocated memory for an image, release it.
		if (nullptr != raw_image)
		{
			free(raw_image); raw_image = nullptr;
		}

	} while (0);

	g_Progress = nullptr;

	if (pFiller)
		delete pFiller;
	pFiller = nullptr;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CRawDecod::IsRawFile() const
{
	ZFUNCTRACE_RUNTIME();

    return m_isRawFile;
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

bool	IsRAWPicture(LPCTSTR szFileName, CString & strModel)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

    CRawDecod		dcr(szFileName);

    bResult = dcr.IsRawFile();

    if (bResult)
        dcr.GetModel(strModel);

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	IsRAWPicture(LPCTSTR szFileName, CBitmapInfo & BitmapInfo)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = FALSE;
	TCHAR			szExt[_MAX_EXT];
	CString			strExt;

	//
	// Check the extension - a file with the following extensions
	// are definitely not considered to be a RAW file
	// 
	// .tiff .tif		TIFF files
	// .jpg .jpeg .jpe	JPEG files
	//
	_tsplitpath(szFileName, nullptr, nullptr, nullptr, szExt);
	strExt = szExt;
	strExt.MakeUpper();

	if ((strExt != _T(".TIF")) && (strExt != _T(".TIFF")) && 
		strExt != _T(".JPG") && strExt != _T(".JPEG") && strExt != _T(".JPE"))
	{
		CRawDecod		dcr(szFileName);

		bResult = dcr.IsRawFile();

		if (bResult)
		{
			BitmapInfo.m_strFileName	 = szFileName;
			BitmapInfo.m_strFileType	 = "RAW";
			if (dcr.IsColorRAW())
				BitmapInfo.m_CFAType	 = CFATYPE_NONE;
			else
				BitmapInfo.m_CFAType	 = dcr.GetCFAType();
			BitmapInfo.m_lWidth			 = dcr.Width();
			BitmapInfo.m_lHeight		 = dcr.Height();
			BitmapInfo.m_lBitPerChannel  = 16;
			BitmapInfo.m_lNrChannels	 = dcr.IsColorRAW() ? 3 : 1;
			BitmapInfo.m_bCanLoad		 = true;
			dcr.GetModel(BitmapInfo.m_strModel);
			BitmapInfo.m_lISOSpeed		 = dcr.GetISOSpeed();
			BitmapInfo.m_fExposure		 = dcr.GetExposureTime();
			BitmapInfo.m_fAperture       = dcr.getAperture();
			BitmapInfo.m_DateTime		 = dcr.GetDateTime();
		};
	}

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	LoadRAWPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	bool			bResult = false;

	CRawDecod		dcr(szFileName);

    if (dcr.IsRawFile() && ppBitmap)
    {
        bool						bColorRAW;
        CSmartPtr<CMemoryBitmap>	pBitmap;

        bColorRAW = dcr.IsColorRAW();

        if ((IsSuperPixels() || IsRawBayer() || IsRawBilinear() || IsRawAHD()) && !bColorRAW)
        {
            pBitmap.Attach(new C16BitGrayBitmap);
            ZTRACE_RUNTIME("Creating 16 bit gray memory bitmap %p (%s)", pBitmap.m_p, szFileName);
        }
        else
        {
            pBitmap.Attach(new C48BitColorBitmap);
            ZTRACE_RUNTIME("Creating 16 bit RGB memory bitmap %p (%s)", pBitmap.m_p, szFileName);
        };

        bResult = dcr.LoadRawFile(pBitmap, pProgress);

        if (bResult)
        {
            C16BitGrayBitmap *	pGrayBitmap;

            pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(pBitmap.m_p);
            if (pGrayBitmap)
            {
                if (IsSuperPixels())
                    pGrayBitmap->UseSuperPixels(true);
                else if (IsRawBayer())
                    pGrayBitmap->UseRawBayer(true);
                else if (IsRawBilinear())
                    pGrayBitmap->UseBilinear(true);
                else if (IsRawAHD())
                    pGrayBitmap->UseAHD(true);
            };
            pBitmap.CopyTo(ppBitmap);
        };
    }

	return bResult;
};

#define EXCEPTION_HANDLER(e) do{                        \
    /* fprintf(stderr,"Exception %d caught\n",e);*/     \
    switch(e)                                           \
      {                                                 \
      case LIBRAW_EXCEPTION_ALLOC:                      \
        recycle();                                      \
        return LIBRAW_UNSUFFICIENT_MEMORY;              \
      case LIBRAW_EXCEPTION_DECODE_RAW:                 \
      case LIBRAW_EXCEPTION_DECODE_JPEG:                \
        recycle();                                      \
        return LIBRAW_DATA_ERROR;                       \
      case LIBRAW_EXCEPTION_DECODE_JPEG2000:            \
        recycle();                                      \
        return LIBRAW_DATA_ERROR;                       \
      case LIBRAW_EXCEPTION_IO_EOF:                     \
      case LIBRAW_EXCEPTION_IO_CORRUPT:                 \
        recycle();                                      \
        return LIBRAW_IO_ERROR;                                 \
      case LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK:              \
        recycle();                                              \
        return LIBRAW_CANCELLED_BY_CALLBACK;                    \
      case LIBRAW_EXCEPTION_BAD_CROP:                           \
        recycle();                                              \
        return LIBRAW_BAD_CROP;                                 \
      default:                                                  \
        return LIBRAW_UNSPECIFIED_ERROR;                        \
      }                                                         \
  }while(0)

int DSSLibRaw::dcraw_ppm_tiff_writer(const char *filename)
{
	ZFUNCTRACE_RUNTIME();
	CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

	if (!imgdata.image)
		return LIBRAW_OUT_OF_ORDER_CALL;

	try {
		if (!libraw_internal_data.output_data.histogram)
		{
			libraw_internal_data.output_data.histogram =
				(int(*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram) * 4);
			merror(libraw_internal_data.output_data.histogram, "LibRaw::dcraw_ppm_tiff_writer()");
		}
		write_ppm_tiff();
		SET_PROC_FLAG(LIBRAW_PROGRESS_FLIP);
		return 0;
	}
	catch (LibRaw_exceptions err) {
		EXCEPTION_HANDLER(err);
	}
}

#include "internal/var_defines.h"
#define FORCC for (c=0; c < colors && c < 4; c++)
#define SWAP(a,b) { a=a+b; b=a-b; a=a-b; }

void DSSLibRaw::write_ppm_tiff()
{
	ZFUNCTRACE_RUNTIME();
	uchar *ppm;
	ushort *ppm2;
	int c, row, col, soff, rstep, cstep;
	int perc, val, total, t_white = 0x2000;

#ifdef LIBRAW_LIBRARY_BUILD
	perc = width * height * auto_bright_thr;
#else
	perc = width * height * 0.01;		/* 99th percentile white level */
#endif
	if (fuji_width) perc /= 2;
	if (!((highlight & ~2) || no_auto_bright))
		for (t_white = c = 0; c < colors; c++) {
			for (val = 0x2000, total = 0; --val > 32; )
				if ((total += histogram[c][val]) > perc) break;
			if (t_white < val) t_white = val;
		}
	gamma_curve(gamm[0], gamm[1], 2, (t_white << 3) / bright);
	iheight = height;
	iwidth = width;
	if (flip & 4) SWAP(height, width);
	ppm = (uchar *)calloc(width, colors*output_bps / 8);
	ppm2 = (ushort *)ppm;
	merror(ppm, "write_ppm_tiff()");

	//
	// Neither need nor want to write TIFF/PPM headers
	//
#if (0)
	if (output_tiff) {
		tiff_head(&th, 1);
		fwrite(&th, sizeof th, 1, ofp);
		if (oprof)
			fwrite(oprof, ntohl(oprof[0]), 1, ofp);
	}
	else if (colors > 3)
		fprintf(ofp,
			"P7\nWIDTH %d\nHEIGHT %d\nDEPTH %d\nMAXVAL %d\nTUPLTYPE %s\nENDHDR\n",
			width, height, colors, (1 << output_bps) - 1, cdesc);
	else
		fprintf(ofp, "P%d\n%d %d\n%d\n",
			colors / 2 + 5, width, height, (1 << output_bps) - 1);
#endif
	//
	// Populate some Bitmap loader variables with data that would
	// have been written to the PGM/PPM header
	//
	pDSSBitMapFiller->setGrey(1 == colors);
	pDSSBitMapFiller->setWidth(width);
	pDSSBitMapFiller->setHeight(height);
	pDSSBitMapFiller->setMaxColors((1 << output_bps) - 1);

	soff = flip_index(0, 0);
	cstep = flip_index(0, 1) - soff;
	rstep = flip_index(1, 0) - flip_index(0, width);
	for (row = 0; row < height; row++, soff += rstep) {
		for (col = 0; col < width; col++, soff += cstep)
			if (output_bps == 8)
				FORCC ppm[col*colors + c] = curve[image[soff][c]] >> 8;
			else FORCC ppm2[col*colors + c] = curve[image[soff][c]];
			if (output_bps == 16 && !output_tiff && htons(0x55aa) != 0x55aa)
				_swab((char*)ppm2, (char*)ppm2, width*colors * 2);

			//
			// Instead of writing the bitmap data to an output file
			// send it to our Bitmap loader class
			//
			pDSSBitMapFiller->Write(ppm, colors*output_bps / 8, width);
	}
	free(ppm);
}
