#pragma once

#undef max
#undef min

#include <vector>
#include <memory>
#include "RefCount.h"
#include "../Tools/SmartPtr.h"
#include "Multitask.h"
#include "DSSCommon.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "resource.h"


class CMultiBitmap;
class CMedianFilterEngine;
class CBitmapCharacteristics;
template <typename TType> class CGrayMedianFilterEngineT;
template <typename TType> class CGrayBitmapT;
template <typename TType> class CColorBitmapT;


#pragma pack(push, HDCOLORREFT, 1)

template <typename TType>
class COLORREFT
{
public:
	TType		red;
	TType		green;
	TType		blue;

private:
	void	CopyFrom(const COLORREFT<TType>& cr)
	{
		red = cr.red;
		green = cr.green;
		blue = cr.blue;
	};

public:
	COLORREFT(TType r = 0, TType g = 0, TType b = 0)
	{
		red = r;
		green = g;
		blue = b;
	};

	COLORREFT(const COLORREFT<TType>& cr)
	{
		CopyFrom(cr);
	};

	COLORREFT<TType>& operator = (const COLORREFT<TType>& cr)
	{
		CopyFrom(cr);
		return *this;
	};

};

typedef COLORREFT<std::uint8_t> COLORREF8;
typedef COLORREFT<std::uint16_t> COLORREF16;
typedef COLORREFT<std::uint32_t> COLORREF32;
typedef COLORREFT<float> COLORREF32F;

#pragma pack(pop, HDCOLORREFT)


typedef enum
{
	BAYER_UNKNOWN = 0,
	BAYER_RED = 1,
	BAYER_GREEN = 2,
	BAYER_BLUE = 3,
	BAYER_CYAN = 0xA,
	BAYER_GREEN2 = 0xB,
	BAYER_MAGENTA = 0xC,
	BAYER_YELLOW = 0xD,
	BAYER_NRCOLORS = 0xD + 1
} BAYERCOLOR;


typedef enum tagEXTRAINFOTYPE
{
	EIT_UNKNOWN = 0,
	EIT_STRING = 1,
	EIT_LONG = 2,
	EIT_DOUBLE = 3
} EXTRAINFOTYPE;

class CExtraInfo
{
public:
	EXTRAINFOTYPE m_Type;
	CString m_strName;
	CString m_strValue;
	CString m_strComment;
	int m_lValue;
	double m_fValue;
	bool m_bPropagate;

private:
	void	CopyFrom(const CExtraInfo& ei)
	{
		m_Type = ei.m_Type;
		m_strName = ei.m_strName;
		m_strValue = ei.m_strValue;
		m_lValue = ei.m_lValue;
		m_fValue = ei.m_fValue;
		m_strComment = ei.m_strComment;
		m_bPropagate = ei.m_bPropagate;
	};

public:
	CExtraInfo()
	{
		m_bPropagate = false;
		m_Type = EIT_UNKNOWN;
		m_lValue = 0;
		m_fValue = 0;
	};
	CExtraInfo(const CExtraInfo& ei)
	{
		CopyFrom(ei);
	};
	~CExtraInfo() {};

	CExtraInfo& operator = (const CExtraInfo& ei)
	{
		CopyFrom(ei);
		return (*this);
	};
};

typedef std::vector<CExtraInfo> EXTRAINFOVECTOR;


class CBitmapExtraInfo
{
public:
	EXTRAINFOVECTOR			m_vExtras;

public:
	CBitmapExtraInfo() {};
	~CBitmapExtraInfo() {};

	void	AddInfo(const CExtraInfo& ei)
	{
		m_vExtras.push_back(ei);
	};

	void	AddInfo(LPCTSTR szName, LPCTSTR szValue, LPCTSTR szComment = nullptr, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type = EIT_STRING;
		ei.m_strName = szName;
		ei.m_strComment = szComment;
		ei.m_strValue = szValue;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};
	void	AddInfo(LPCTSTR szName, int lValue, LPCTSTR szComment = nullptr, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type = EIT_LONG;
		ei.m_strName = szName;
		ei.m_lValue = lValue;
		ei.m_strComment = szComment;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};
	void	AddInfo(LPCTSTR szName, double fValue, LPCTSTR szComment = nullptr, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type = EIT_DOUBLE;
		ei.m_strName = szName;
		ei.m_fValue = fValue;
		ei.m_strComment = szComment;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};

	void	Clear()
	{
		m_vExtras.clear();
	};
};


class CMemoryBitmap : public CRefCount
{
public:
	class CPixelIterator : public CRefCount
	{
	protected:
		int m_lX, m_lY;
	public:
		CPixelIterator() : m_lX{ -1 }, m_lY{ -1 }
		{}

		~CPixelIterator() {};

		virtual void Reset(int x, int y) = 0;

		virtual void GetPixel(double& fRed, double& fGreen, double& fBlue) = 0;
		virtual void GetPixel(double& fGray) = 0;
		virtual void SetPixel(double fRed, double fGreen, double fBlue) = 0;
		virtual void SetPixel(double fGray) {};

		virtual void operator++(int) = 0;
		virtual void operator+=(int lIncrement)
		{
			while (lIncrement > 0)
			{
				operator++(0);
				lIncrement--;
			}
		};

		bool end()
		{
			return (m_lX == -1) && (m_lY == -1);
		};
	};
public:
	CBitmapExtraInfo	m_ExtraInfo;
	SYSTEMTIME			m_DateTime;

protected:
	bool				m_bTopDown;
	bool				m_bMaster;
	bool				m_bCFA;
	double				m_fExposure;
	double				m_fAperture;
	int					m_lISOSpeed;
	int					m_lGain;
	int					m_lNrFrames;
	CString				m_strDescription;
	CString				m_filterName;

protected:
	void			CopyFrom(const CMemoryBitmap& mb)
	{
		m_ExtraInfo = mb.m_ExtraInfo;
		m_bTopDown = mb.m_bTopDown;
		m_bMaster = mb.m_bMaster;
		m_bCFA = mb.m_bCFA;
		m_fExposure = mb.m_fExposure;
		m_fAperture = mb.m_fAperture;
		m_lISOSpeed = mb.m_lISOSpeed;
		m_lGain = mb.m_lGain;
		m_lNrFrames = mb.m_lNrFrames;
		m_strDescription = mb.m_strDescription;
		m_DateTime = mb.m_DateTime;
		m_filterName = mb.m_filterName;
	};

public:
	CMemoryBitmap() :
		m_bTopDown{ false },
		m_bMaster{ false },
		m_bCFA{ false },
		m_fExposure{ 0.0 },
		m_fAperture{ 0.0 },
		m_lISOSpeed{ 0 },
		m_lGain{ -1 },
		m_lNrFrames{ 0 }
	{
		m_DateTime.wYear = 0;
	}
	virtual ~CMemoryBitmap()
	{
		ZTRACE_RUNTIME("Destroying memory bitmap %p", this);
	};

	virtual double	GetExposure()
	{
		return m_fExposure;
	};

	virtual CMemoryBitmap& SetExposure(double fExposure)
	{
		m_fExposure = fExposure;
		return *this;
	};

	virtual double	GetAperture()
	{
		return m_fAperture;
	};

	virtual CMemoryBitmap& SetAperture(double fAperture)
	{
		m_fAperture = fAperture;
		return *this;
	};

	virtual int GetISOSpeed()
	{
		return m_lISOSpeed;
	};

	virtual CMemoryBitmap& SetISOSpeed(int lISOSpeed)
	{
		m_lISOSpeed = lISOSpeed;
		return *this;
	};

	virtual int GetGain()
	{
		return m_lGain;
	};

	virtual CMemoryBitmap& SetGain(int lGain)
	{
		m_lGain = lGain;
		return *this;
	};

	virtual int GetNrFrames()
	{
		return m_lNrFrames;
	};

	virtual CMemoryBitmap& SetNrFrames(int lNrFrames)
	{
		m_lNrFrames = lNrFrames;
		return *this;
	};

	virtual CMemoryBitmap& SetDescription(LPCTSTR szDescription)
	{
		m_strDescription = szDescription;
		return *this;
	};

	virtual CMemoryBitmap& GetDescription(CString& strDescription)
	{
		strDescription = m_strDescription;
		return *this;
	};

	virtual CMemoryBitmap& setFilterName(CString& name)
	{
		m_filterName = name;
		return *this;
	}

	virtual CString filterName()
	{
		return m_filterName;
	}

	virtual bool	Init(int lWidth, int lHeight) = 0;

	virtual void	SetPixel(int i, int j, double fRed, double fGreen, double fBlue) = 0;
	virtual void	SetPixel(int i, int j, double fGray) = 0;
	virtual void	GetPixel(int i, int j, double& fRed, double& fGreen, double& fBlue) = 0;
	virtual void	GetPixel(int i, int j, double& fGray) = 0;

	virtual void	SetValue(int i, int j, double fRed, double fGreen, double fBlue) {};
	virtual void	GetValue(int i, int j, double& fRed, double& fGreen, double& fBlue) {};
	virtual void	SetValue(int i, int j, double fGray) {};
	virtual void	GetValue(int i, int j, double& fGray) {};

	virtual bool	GetScanLine(int j, void* pScanLine) = 0;
	virtual bool	SetScanLine(int j, void* pScanLine) = 0;

	void GetPixel16(const int i, const int j, COLORREF16& crResult)
	{
		constexpr double scalingFactor = double{ 1 + std::numeric_limits<unsigned char>::max() };
		constexpr double maxValue = double{ std::numeric_limits<unsigned short>::max() };
		// Use get pixel
		double fRed, fGreen, fBlue;
		GetPixel(i, j, fRed, fGreen, fBlue);

		crResult.red = static_cast<WORD>(std::min(fRed * scalingFactor, maxValue));
		crResult.green = static_cast<WORD>(std::min(fGreen * scalingFactor, maxValue));
		crResult.blue = static_cast<WORD>(std::min(fBlue * scalingFactor, maxValue));
	};

	virtual int Width() = 0;
	virtual int Height() = 0;
	virtual int BitPerSample() = 0;
	virtual int IsFloat() = 0;

	virtual int RealWidth()
	{
		return Width();
	};

	virtual int RealHeight()
	{
		return Height();
	};

	void	SetOrientation(bool bTopDown)
	{
		m_bTopDown = bTopDown;
	};

	virtual bool	IsMonochrome() = 0;

	virtual void	SetMaster(bool bMaster)
	{
		m_bMaster = bMaster;
	};

	virtual bool	IsMaster()
	{
		return m_bMaster;
	};

	virtual bool isTopDown()
	{
		return m_bTopDown;
	}

	virtual void	SetCFA(bool bCFA)
	{
		m_bCFA = bCFA;
	};

	virtual bool	IsCFA()
	{
		return m_bCFA;
	};

	virtual BAYERCOLOR GetBayerColor(int x, int y)
	{
		return BAYER_UNKNOWN;
	};

	bool	IsOk()
	{
		return (Width() > 0) && (Height() > 0);
	};

	virtual CMemoryBitmap* Clone(bool bEmpty = false) = 0;

	virtual CMultiBitmap* CreateEmptyMultiBitmap() = 0;
	virtual void	AverageBitmap(CMemoryBitmap* pBitmap, CDSSProgress* pProgress) {};
	virtual void	RemoveHotPixels(CDSSProgress* pProgress = nullptr) = 0;
	virtual void	GetMedianFilterEngine(CMedianFilterEngine** pMedianFilterEngine) = 0;

	virtual void	GetIterator(CPixelIterator** ppIterator, int x = 0, int y = 0) = 0;
	virtual double	GetMaximumValue() = 0;
	virtual void	GetCharacteristics(CBitmapCharacteristics& bc) = 0;
};

typedef CSmartPtr<CMemoryBitmap::CPixelIterator> PixelIterator;


enum CFATYPE : std::uint32_t
{
	CFATYPE_NONE = 0,
	CFATYPE_BGGR = 1,
	CFATYPE_GRBG = 2,
	CFATYPE_GBRG = 3,
	CFATYPE_RGGB = 4,

	// A = Cyan		B = Green		C = Magenta		D = Yellow
	CFATYPE_CGMY = 0xABCD,
	CFATYPE_CGYM = 0xABDC,
	CFATYPE_CMGY = 0xACBD,
	CFATYPE_CMYG = 0xACDB,
	CFATYPE_CYMG = 0xADCB,
	CFATYPE_CYGM = 0xADBC,

	CFATYPE_GCMY = 0xBACD,
	CFATYPE_GCYM = 0xBADC,
	CFATYPE_GMCY = 0xBCAD,
	CFATYPE_GMYC = 0xBCDA,
	CFATYPE_GYCM = 0xBDAC,
	CFATYPE_GYMC = 0xBDCA,

	CFATYPE_MCGY = 0xCABD,
	CFATYPE_MCYG = 0xCADB,
	CFATYPE_MGYC = 0xCBDA,
	CFATYPE_MGCY = 0xCBAD,
	CFATYPE_MYGC = 0xCDBA,
	CFATYPE_MYCG = 0xCDAB,

	CFATYPE_YCGM = 0xDABC,
	CFATYPE_YCMG = 0xDACB,
	CFATYPE_YGMC = 0xDBCA,
	CFATYPE_YGCM = 0xDBAC,
	CFATYPE_YMCG = 0xDCAB,
	CFATYPE_YMGC = 0xDCBA,

	CFATYPE_CYMGCYGM = 0xADCBADBC,
	CFATYPE_YCGMYCMG = 0xDABCDACB,
	CFATYPE_CYGMCYMG = 0xADBCADCB,
	CFATYPE_YCMGYCGM = 0xDACBDABC,

	CFATYPE_MGYCMGCY = 0xCBDACBAD,
	CFATYPE_GMCYGMYC = 0xBCADBCDA,
	CFATYPE_MGCYMGYC = 0xCBADCBDA,
	CFATYPE_GMYCGMCY = 0xBCDABCAD,

	CFATYPE_GMCYMGCY = 0xBCADCBAD,
	CFATYPE_MGYCGMYC = 0xCBDABCDA,
	CFATYPE_MGCYGMCY = 0xCBADBCAD,
	CFATYPE_GMYCMGYC = 0xBCDACBDA,

	CFATYPE_CYMGYCMG = 0xADCBDACB,
	CFATYPE_YCGMCYGM = 0xDABCADBC,
	CFATYPE_YCMGCYMG = 0xDACBADCB,
	CFATYPE_CYGMYCGM = 0xADBCDABC
};


typedef enum CFATRANSFORMATION
{
	CFAT_NONE = 0,
	CFAT_SUPERPIXEL = 1,
	CFAT_RAWBAYER = 2,
	CFAT_BILINEAR = 3,
	CFAT_GRADIENT = 4,
	CFAT_AHD = 5
} CFATRANSFORMATION;


inline bool IsCYMGType(CFATYPE Type)
{
	return ((Type & 0xFFFFFFF0) != 0);
}

inline bool IsSimpleCYMG(CFATYPE Type)
{
	return !(Type & 0xFFFF0000);
};

inline int CMYGZeroIndex(BAYERCOLOR Color)
{
	return Color - BAYER_CYAN;
};


inline BAYERCOLOR GetBayerColor(int baseX, int baseY, CFATYPE CFAType, int xOffset = 0, int yOffset = 0)
{
	const int x = baseX + xOffset;		// Apply the X Bayer offset if supplied
	const int y = baseY + yOffset;		// Apply the Y Bayer offset if supplied

	switch (CFAType)
	{
	case CFATYPE_NONE:
		return BAYER_UNKNOWN;
		break;
	case CFATYPE_BGGR:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_RED;
			else
				return BAYER_GREEN;
		}
		else
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_BLUE;
		};
		break;
	case CFATYPE_GRBG:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_RED;
		}
		else
		{
			if (y & 1)
				return BAYER_BLUE;
			else
				return BAYER_GREEN;
		};
		break;
	case CFATYPE_GBRG:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_BLUE;
		}
		else
		{
			if (y & 1)
				return BAYER_RED;
			else
				return BAYER_GREEN;
		};
		break;
	case CFATYPE_RGGB:
		if (x & 1)
		{
			if (y & 1)
				return BAYER_BLUE;
			else
				return BAYER_GREEN;
		}
		else
		{
			if (y & 1)
				return BAYER_GREEN;
			else
				return BAYER_RED;
		};
		break;
	default:
	{	// CYMG Type
		if (IsSimpleCYMG(CFAType))
		{
			// 2 lines and 2 columns repeated pattern
			if (y & 1)
			{
				if (x & 1)
					return (BAYERCOLOR)(CFAType & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 4) & 0xF);
			}
			else
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 8) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 12) & 0xF);
			};
		}
		else
		{
			// 4 lines and 2 columns repeated pattern
			if (y % 4 == 0)
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 24) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 28) & 0xF);
			}
			else if (y % 4 == 1)
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 16) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 20) & 0xF);
			}
			else if (y % 4 == 2)
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 8) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 12) & 0xF);
			}
			else
			{
				if (x & 1)
					return (BAYERCOLOR)((CFAType >> 0) & 0xF);
				else
					return (BAYERCOLOR)((CFAType >> 4) & 0xF);
			};
		};
	};
	};
	return BAYER_UNKNOWN;
};


//
// Add parameter yOffset to specify CFA Matrix offset to be applied (for FITS files)
//
inline bool	IsBayerBlueLine(int baseY, CFATYPE CFAType, int yOffset = 0)
{
	int y = baseY + yOffset;

	if ((CFAType == CFATYPE_GRBG) || (CFAType == CFATYPE_RGGB))
		return (y & 1) ? true : false;
	else
		return (y & 1) ? false : true;
};

//
// Add parameter xOffset to specify CFA Matrix offset to be applied (for FITS files)
//
inline bool IsBayerBlueColumn(int baseX, CFATYPE CFAType, int xOffset = 0)
{
	int x = baseX + xOffset;

	if ((CFAType == CFATYPE_GBRG) || (CFAType == CFATYPE_RGGB))
		return (x & 1) ? true : false;
	else
		return (x & 1) ? false : true;
};

inline bool IsBayerRedLine(int baseY, CFATYPE CFAType, int yOffset = 0)
{
	return !IsBayerBlueLine(baseY, CFAType, yOffset);
};

//
// Add parameter xOffset to specify CFA Matrix offset to be applied (for FITS files)
//
inline bool IsBayerRedColumn(int baseX, CFATYPE CFAType, int xOffset = 0)
{
	return !IsBayerBlueColumn(baseX, CFAType, xOffset);
};


void CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double& fRed, double& fGreen, double& fBlue);


inline void ToHSL(double Red, double Green, double Blue, double& H, double& S, double& L)
{
	double minval = std::min(Red, std::min(Green, Blue));
	double maxval = std::max(Red, std::max(Green, Blue));
	double mdiff = maxval - minval;
	double msum = maxval + minval;

	L = msum / 510.0f;

	if (maxval == minval)
	{
		S = 0.0f;
		H = 0.0f;
	}
	else
	{
		double rnorm = (maxval - Red) / mdiff;
		double gnorm = (maxval - Green) / mdiff;
		double bnorm = (maxval - Blue) / mdiff;

		S = (L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

		if (Red == maxval) H = 60.0f * (6.0f + bnorm - gnorm);
		if (Green == maxval) H = 60.0f * (2.0f + rnorm - bnorm);
		if (Blue == maxval) H = 60.0f * (4.0f + gnorm - rnorm);
		if (H > 360.0f)
			H = H - 360.0f;
	}
};


class CBitmapCharacteristics
{
public:
	int m_lNrChannels;
	int m_lBitsPerPixel;
	bool m_bFloat;
	std::uint32_t m_dwWidth, m_dwHeight;

private:
	void	CopyFrom(const CBitmapCharacteristics& bc)
	{
		m_lNrChannels = bc.m_lNrChannels;
		m_lBitsPerPixel = bc.m_lBitsPerPixel;
		m_bFloat = bc.m_bFloat;
		m_dwWidth = bc.m_dwWidth;
		m_dwHeight = bc.m_dwHeight;
	};

public:
	CBitmapCharacteristics() :
		m_lNrChannels{ 0 },
		m_lBitsPerPixel{ 0 },
		m_bFloat{ false },
		m_dwWidth{ 0 },
		m_dwHeight{ 0 }
	{}
	~CBitmapCharacteristics() {};

	CBitmapCharacteristics(const CBitmapCharacteristics& bc)
	{
		CopyFrom(bc);
	};

	CBitmapCharacteristics& operator= (const CBitmapCharacteristics& bc)
	{
		CopyFrom(bc);
		return *this;
	};
};


bool CreateBitmap(const CBitmapCharacteristics& bc, CMemoryBitmap** ppOutBitmap);


class CCFABitmapInfo
{
protected:
	CFATRANSFORMATION m_CFATransform;
	CFATYPE m_CFAType;
	bool m_bCYMG;
	int m_xBayerOffset;
	int m_yBayerOffset;

protected:
	virtual void SetCFA(bool bCFA) = 0;

public:
	void	InitFrom(const CCFABitmapInfo* const pCFABitmapInfo)
	{
		m_CFATransform = pCFABitmapInfo->m_CFATransform;
		m_CFAType = pCFABitmapInfo->m_CFAType;
		m_bCYMG = pCFABitmapInfo->m_bCYMG;
		m_xBayerOffset = pCFABitmapInfo->m_xBayerOffset;
		m_yBayerOffset = pCFABitmapInfo->m_yBayerOffset;
	};

public:
	CCFABitmapInfo() :
		m_CFATransform{ CFAT_NONE },
		m_CFAType{ CFATYPE_NONE },
		m_bCYMG{ false },
		m_xBayerOffset{ 0 },
		m_yBayerOffset{ 0 }
	{}

	void	SetCFAType(CFATYPE Type)
	{
		m_CFAType = Type;
		m_bCYMG = IsCYMGType(m_CFAType);
	};

	CCFABitmapInfo& setXoffset(int xOffset) noexcept
	{
		m_xBayerOffset = xOffset;
		return *this;
	};

	inline int xOffset() noexcept
	{
		return m_xBayerOffset;
	}

	CCFABitmapInfo& setYoffset(int yOffset) noexcept
	{
		m_yBayerOffset = yOffset;
		return *this;
	};


	inline int yOffset() noexcept
	{
		return m_yBayerOffset;
	}

	inline CFATYPE	GetCFAType() noexcept
	{
		return m_CFAType;
	};

	void	UseSuperPixels(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_SUPERPIXEL;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseRawBayer(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_RAWBAYER;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseBilinear(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_BILINEAR;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseAHD(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_AHD;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	CFATRANSFORMATION GetCFATransformation()
	{
		return m_CFATransform;
	};
};


class CBitmapPartFile
{
public:
	CString m_strFile;
	int m_lStartRow;
	int m_lEndRow;
	int m_lWidth;
	int m_lNrBitmaps;

private:
	void	CopyFrom(const CBitmapPartFile& bp)
	{
		m_strFile = bp.m_strFile;
		m_lStartRow = bp.m_lStartRow;
		m_lEndRow = bp.m_lEndRow;
		m_lWidth = bp.m_lWidth;
		m_lNrBitmaps = bp.m_lNrBitmaps;
	};

public:
	CBitmapPartFile(LPCTSTR szFile, int lStartRow, int lEndRow)
	{
		m_strFile = szFile;
		m_lStartRow = lStartRow;
		m_lEndRow = lEndRow;
		m_lWidth = 0;
		m_lNrBitmaps = 0;
	};

	CBitmapPartFile(const CBitmapPartFile& bp)
	{
		CopyFrom(bp);
	};

	const CBitmapPartFile& operator= (const CBitmapPartFile& bp)
	{
		CopyFrom(bp);
		return *this;
	};

	virtual ~CBitmapPartFile()
	{}
};

typedef std::vector<CBitmapPartFile> BITMAPPARTFILEVECTOR;


class CMultiBitmap : public CRefCount
{
protected:
	CSmartPtr<CMemoryBitmap> m_pBitmapModel;
	CSmartPtr<CMemoryBitmap> m_pHomBitmap;
	MULTIBITMAPPROCESSMETHOD m_Method;
	double m_fKappa;
	int m_lNrIterations;
	int m_lNrBitmaps;
	int m_lNrAddedBitmaps;
	BITMAPPARTFILEVECTOR m_vFiles;
	int m_lWidth;
	int m_lHeight;
	bool m_bInitDone;
	bool m_bHomogenization;
	double m_fMaxWeight;
	std::vector<int> m_vImageOrder;

private:
	void	DestroyTempFiles();
	void	InitParts();
	void	SmoothOut(CMemoryBitmap* pBitmap, CMemoryBitmap** ppOutBitmap);

public:
	CMultiBitmap() :
		m_lNrBitmaps{ 0 },
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_bInitDone{ false },
		m_lNrAddedBitmaps{ 0 },
		m_bHomogenization{ false },
		m_fMaxWeight{ 0 },
		m_Method{ MULTIBITMAPPROCESSMETHOD{0} },
		m_fKappa{ 0.0 },
		m_lNrIterations{ 0 }
	{}

	virtual ~CMultiBitmap()
	{
		DestroyTempFiles();
	};

	void			SetBitmapModel(CMemoryBitmap* pBitmap);
	virtual bool	SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines) = 0;
	virtual bool	CreateNewMemoryBitmap(CMemoryBitmap** ppBitmap) = 0;
	virtual bool	CreateOutputMemoryBitmap(CMemoryBitmap** ppBitmap) = 0;

	virtual void	SetNrBitmaps(int lNrBitmaps)
	{
		m_lNrBitmaps = lNrBitmaps;
	};

	int GetNrBitmaps()
	{
		return m_lNrBitmaps;
	};

	int GetNrAddedBitmaps()
	{
		return m_lNrAddedBitmaps;
	};

	void SetImageOrder(const std::vector<int>& vImageOrder)
	{
		m_vImageOrder = vImageOrder;
	};

	virtual bool AddBitmap(CMemoryBitmap* pMemoryBitmap, CDSSProgress* pProgress = nullptr);
	virtual bool GetResult(CMemoryBitmap** ppBitmap, CDSSProgress* pProgress = nullptr);
	virtual int GetNrChannels() = 0;
	virtual int GetNrBytesPerChannel() = 0;

	void SetProcessingMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
	{
		m_Method = Method;
		m_fKappa = fKappa;
		m_lNrIterations = lNrIterations;
	};

	void SetHomogenization(bool bSet)
	{
		m_bHomogenization = bSet;
	};

	bool GetHomogenization() const
	{
		return m_bHomogenization;
	}

	int GetProcessingMethod() const
	{
		return m_Method;
	}

	auto GetProcessingParameters() const
	{
		return std::make_tuple(m_fKappa, m_lNrIterations);
	}
};


template <typename TType, typename TTypeOutput = TType>
class CGrayMultiBitmapT : public CMultiBitmap
{
protected:
	virtual bool CreateNewMemoryBitmap(CMemoryBitmap** ppBitmap) override
	{
		CSmartPtr<CMemoryBitmap> pBitmap;
		if (m_pBitmapModel)
		{
			CBitmapCharacteristics bc;
			m_pBitmapModel->GetCharacteristics(bc);
			if (bc.m_lNrChannels == 1)
				pBitmap.Attach(m_pBitmapModel->Clone(true));
			else
			{
				bc.m_lNrChannels = 1;
				CreateBitmap(bc, &pBitmap);
			};
		}
		else
			pBitmap.Attach(new CGrayBitmapT<TType>);

		return pBitmap.CopyTo(ppBitmap);
	};

	virtual bool CreateOutputMemoryBitmap(CMemoryBitmap** ppBitmap) override
	{
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CGrayBitmapT<TTypeOutput>);

		if (pBitmap && m_pBitmapModel)
		{
			CCFABitmapInfo* pSrc;
			CCFABitmapInfo* pDst;

			pBitmap->SetCFA(m_pBitmapModel->IsCFA());
			pSrc = dynamic_cast<CCFABitmapInfo*>(m_pBitmapModel.m_p);
			pDst = dynamic_cast<CCFABitmapInfo*>(pBitmap.m_p);

			if (pSrc != nullptr && pDst != nullptr)
				pDst->InitFrom(pSrc);
		};

		return pBitmap.CopyTo(ppBitmap);
	};

	virtual bool SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines) override
	{
		bool bResult = false;
		// Each scan line consist of lWidth TType values
		std::vector<TType> vValues;
		std::vector<TType> vAuxValues;
		std::vector<TType> vWorkingBuffer1;
		std::vector<TType> vWorkingBuffer2;
		std::vector<double> vdWork1;			// Used for AutoAdaptiveWeightedAverage
		std::vector<double> vdWork2;			// Used for AutoAdaptiveWeightedAverage

		const double fMaximum = pBitmap->GetMaximumValue();
		const int lWidth = pBitmap->RealWidth();

		std::vector<TTypeOutput> outputScanBuffer;
		try {
			outputScanBuffer.resize(lWidth);
		} catch (...) {
			ZOutOfMemory e("Could not allocate storage for output scanline");
			ZTHROW(e);
		}
		TTypeOutput* pCurrentValue = outputScanBuffer.data();

		vValues.reserve(vScanLines.size());
		vWorkingBuffer1.reserve(vScanLines.size());
		vWorkingBuffer2.reserve(vScanLines.size());
		vdWork1.reserve(vScanLines.size());
		vdWork2.reserve(vScanLines.size());

		for (int i = 0; i < lWidth; i++)
		{
			TType* pValue;
			double fWeight = 1.0;

			vValues.resize(0);
			for (size_t j = 0; j < vScanLines.size(); j++)
			{
				pValue = (TType*)vScanLines[j];
				pValue += i;

				if (*pValue || m_vImageOrder.size()) // Remove 0
					vValues.push_back(*pValue);
			};

			// Process the value
			if (m_bHomogenization)
			{
				//	if ((i==559) && (lLine==154))
				//		DebugBreak();
				if (m_pHomBitmap)
				{
					double fAverage;
					const double fSigma = Sigma2(vValues, fAverage);
					m_pHomBitmap->SetPixel(i, lLine, fSigma / std::max(1.0, fAverage) * 256.0);
				};

				if (!m_vImageOrder.empty())
				{
					// Change the order to respect the order of the images
					vAuxValues = vValues;
					vValues.resize(0);
					for (size_t k = 0; k < m_vImageOrder.size(); k++)
						if (const auto auxVal = vAuxValues[m_vImageOrder[k]])
							vValues.push_back(auxVal);

					Homogenize(vValues, fMaximum);
				}
				else
				{
					Homogenize(vValues, fMaximum);
				};
			};

			if (m_Method == MBP_MEDIAN)
				*pCurrentValue = Median(vValues);
			else if (m_Method == MBP_AVERAGE)
				*pCurrentValue = Average(vValues);
			else if (m_Method == MBP_MAXIMUM)
				*pCurrentValue = Maximum(vValues);
			else if (m_Method == MBP_SIGMACLIP)
			{
				*pCurrentValue = KappaSigmaClip(vValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
			}
			else if (m_Method == MBP_MEDIANSIGMACLIP)
				*pCurrentValue = MedianKappaSigmaClip(vValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
			else if (m_Method == MBP_AUTOADAPTIVE)
				*pCurrentValue = AutoAdaptiveWeightedAverage(vValues, m_lNrIterations, vdWork1);

			//if (m_bHomogenization)
			//	*pCurrentValue = fHomogenization*(double)(*pCurrentValue);
			pCurrentValue++;
		};

		pBitmap->SetScanLine(lLine, outputScanBuffer.data());
		bResult = true;

		return bResult;
	};

public:
	CGrayMultiBitmapT()
	{}

	virtual ~CGrayMultiBitmapT()
	{}

	virtual int GetNrChannels() override
	{
		return 1;
	};

	virtual int GetNrBytesPerChannel() override
	{
		return sizeof(TType);
	};
};


class CMedianFilterEngine : public CRefCount
{
protected:
	int m_lFilterSize;
	CDSSProgress* m_pProgress;

public:
	CMedianFilterEngine() :
		m_lFilterSize{ 1 },
		m_pProgress{ nullptr }
	{}

	virtual ~CMedianFilterEngine() {};

	virtual bool GetFilteredImage(CMemoryBitmap** ppBitmap, int lFilterSize, CDSSProgress* pProgress) = 0;
};


template <typename TType>
class CGrayMedianFilterEngineT : public CMedianFilterEngine
{
private:
	CGrayBitmapT<TType>* m_pInBitmap;

public:
	CGrayMedianFilterEngineT() {};
	virtual ~CGrayMedianFilterEngineT() {};

	void SetInputBitmap(CGrayBitmapT<TType>* pInBitmap)
	{
		m_pInBitmap = pInBitmap;
	};

	virtual bool GetFilteredImage(CMemoryBitmap** ppBitmap, int lFilterSize, CDSSProgress* pProgress) override;
};


template <typename TType>
class CGrayBitmapT : public CMemoryBitmap, public CCFABitmapInfo
{
	friend CColorBitmapT<TType>;

public:
	template <typename TType> class CGrayPixelIterator : public CPixelIterator
	{
	private:
		CSmartPtr<CGrayBitmapT<TType>> m_pBitmap;
		TType* m_pValue;
		double m_fMultiplier;
		int m_lWidth, m_lHeight;

	public:
		CGrayPixelIterator() :
			CPixelIterator{},
			m_pValue{ nullptr },
			m_fMultiplier{ 1.0 },
			m_lWidth{ 0 },
			m_lHeight{ 0 }
		{}

		~CGrayPixelIterator() {};

		virtual void Reset(int x, int y) override
		{
			m_lX = x;
			m_lY = y;
			const size_t lOffset = m_pBitmap->GetOffset(x, y);
			m_pValue = &(m_pBitmap->m_vPixels[lOffset]);
		};

		void Init(int x, int y, CGrayBitmapT<TType>* pBitmap)
		{
			m_pBitmap = pBitmap;
			m_fMultiplier = pBitmap->m_fMultiplier;
			m_lWidth = pBitmap->m_lWidth;
			m_lHeight = pBitmap->m_lHeight;
			Reset(x, y);
		};

		virtual void GetPixel(double& fRed, double& fGreen, double& fBlue) override
		{
			if (m_pValue)
				fRed = fGreen = fBlue = (double)(*m_pValue) / m_fMultiplier;
			else
				fRed = fGreen = fBlue = 0.0;
		};

		virtual void GetPixel(double& fGray) override
		{
			if (m_pValue)
				fGray = (double)(*m_pValue) / m_fMultiplier;
			else
				fGray = 0.0;
		};

		virtual void SetPixel(double fRed, double fGreen, double fBlue) override
		{}

		virtual void SetPixel(double fGray) override
		{
			if (m_pValue != nullptr)
				*m_pValue = fGray * m_fMultiplier;
		};

		virtual void operator++(int) override
		{
			if (m_pValue != nullptr)
			{
				if (m_lX < m_lWidth - 1)
					m_lX++;
				else if (m_lY < m_lHeight - 1)
				{
					m_lY++;
					m_lX = 0;
				}
				else
				{
					// End of iteration
					m_lX = -1;
					m_lY = -1;
					m_pValue = nullptr;
				};
				if (m_pValue != nullptr)
					m_pValue++;
			};
		};
	};

	friend CGrayPixelIterator<TType>;
	friend CGrayMedianFilterEngineT<TType>;

public:
	std::vector<TType>	m_vPixels;

private:
	int m_lWidth;
	int m_lHeight;
	bool m_bWord;
	bool m_bDouble;
	bool m_bDWord;
	bool m_bFloat;
	double m_fMultiplier;

private:
	bool InitInternals()
	{
		const size_t nrPixels = static_cast<size_t>(m_lWidth) * static_cast<size_t>(m_lHeight);
		m_vPixels.clear();
		m_vPixels.resize(nrPixels);

		return true; // Otherwise m_vPixels.resize() would have thrown bad_alloc.
	};

	inline void	CheckXY(int x, int y)
	{
		ZASSERTSTATE(IsXYOk(x, y));
	};

	inline bool	IsXYOk(int x, int y)
	{
		return (x >= 0 && x < m_lWidth && y >= 0 && y < m_lHeight);
	};

	size_t GetOffset(int x, int y)
	{
		CheckXY(x, y);
		return static_cast<size_t>(m_lWidth) * static_cast<size_t>(y) + static_cast<size_t>(x);
	};

	virtual bool IsMonochrome() override
	{
		return true;
	};

	TType GetPrimary(int x, int y, const COLORREF16& crColor)
	{
		switch (::GetBayerColor(x, y, m_CFAType, m_xBayerOffset, m_yBayerOffset))
		{
		case BAYER_RED:
			return crColor.red;
			break;
		case BAYER_GREEN:
			return crColor.green;
			break;
		case BAYER_BLUE:
			return crColor.blue;
			break;
		};

		return 0;
	};

	double GetPrimary(int x, int y, double fRed, double fGreen, double fBlue)
	{
		switch (::GetBayerColor(x, y, m_CFAType, m_xBayerOffset, m_yBayerOffset))
		{
		case BAYER_RED:
			return fRed;
			break;
		case BAYER_GREEN:
			return fGreen;
			break;
		case BAYER_BLUE:
			return fBlue;
			break;
		};

		return 0;
	};

	double InterpolateGreen(int x, int y, TType* pValue = nullptr)
	{
		double fResult = 0.0;
		int lNrValues = 0;

		if (!pValue)
			pValue = &m_vPixels[GetOffset(x, y)];

		if (x > 0)
		{
			fResult += *(pValue - 1);
			lNrValues++;
		};
		if (x < m_lWidth - 1)
		{
			fResult += *(pValue + 1);
			lNrValues++;
		};
		if (y > 0)
		{
			fResult += *(pValue - m_lWidth);
			lNrValues++;
		};
		if (y < m_lHeight - 1)
		{
			fResult += *(pValue + m_lWidth);
			lNrValues++;
		};

		return fResult / lNrValues;
	};

	double InterpolateBlue(int x, int y, TType* pValue = nullptr)
	{
		double fResult = 0.0;
		int lNrValues = 0;

		if (!pValue)
			pValue = &m_vPixels[GetOffset(x, y)];

		if (IsBayerBlueLine(y, m_CFAType, m_yBayerOffset))
		{
			// Pixel between 2 blue pixel (horizontaly)
			if (x > 0)
			{
				fResult += *(pValue - 1);
				lNrValues++;
			};
			if (x < m_lWidth - 1)
			{
				fResult += *(pValue + 1);
				lNrValues++;
			};
		}
		else if (IsBayerBlueColumn(x, m_CFAType, m_xBayerOffset))
		{
			// Pixel between 2 blue pixels (verticaly)
			if (y > 0)
			{
				fResult += *(pValue - m_lWidth);
				lNrValues++;
			};
			if (y < m_lHeight - 1)
			{
				fResult += *(pValue + m_lWidth);
				lNrValues++;
			};
		}
		else
		{
			// Use 4 pixels to interpolate
			if (x > 0 && y > 0)
			{
				fResult += *(pValue - 1 - m_lWidth);
				lNrValues++;
			}
			if ((x > 0) && (y < m_lHeight - 1))
			{
				fResult += *(pValue - 1 + m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth - 1) && (y < m_lHeight - 1))
			{
				fResult += *(pValue + 1 + m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth - 1) && (y > 0))
			{
				fResult += *(pValue + 1 - m_lWidth);
				lNrValues++;
			};
		};

		return fResult / lNrValues;
	};

	double InterpolateRed(int x, int y, TType* pValue = nullptr)
	{
		double fResult = 0.0;
		int lNrValues = 0;

		if (!pValue)
			pValue = &m_vPixels[GetOffset(x, y)];

		if (IsBayerRedLine(y, m_CFAType, m_yBayerOffset))
		{
			// Pixel between 2 blue pixel (horizontaly)
			if (x > 0)
			{
				fResult += *(pValue - 1);
				lNrValues++;
			};
			if (x < m_lWidth - 1)
			{
				fResult += *(pValue + 1);
				lNrValues++;
			};
		}
		else if (IsBayerRedColumn(x, m_CFAType, m_xBayerOffset))
		{
			// Pixel between 2 blue pixels (verticaly)
			if (y > 0)
			{
				fResult += *(pValue - m_lWidth);
				lNrValues++;
			};
			if (y < m_lHeight - 1)
			{
				fResult += *(pValue + m_lWidth);
				lNrValues++;
			};
		}
		else
		{
			// Use 4 pixels to interpolate
			if (x > 0 && y > 0)
			{
				fResult += *(pValue - 1 - m_lWidth);
				lNrValues++;
			}
			if ((x > 0) && (y < m_lHeight - 1))
			{
				fResult += *(pValue - 1 + m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth - 1) && (y < m_lHeight - 1))
			{
				fResult += *(pValue + 1 + m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth - 1) && (y > 0))
			{
				fResult += *(pValue + 1 - m_lWidth);
				lNrValues++;
			};
		};

		return fResult / lNrValues;
	};

	void InterpolateAll(double* pfValues, int x, int y)
	{
		int lIndice;
		int lNrValues[4] = { 0 };

		pfValues[0] = pfValues[1] = pfValues[2] = pfValues[3] = 0;

		for (int i = std::max(0, x - 1); i <= std::min(m_lWidth - 1, x + 1); i++)
			for (int j = std::max(0, y - 1); j <= std::min(m_lHeight - 1, y + 1); j++)
			{
				lIndice = CMYGZeroIndex(::GetBayerColor(i, j, m_CFAType, m_xBayerOffset, m_yBayerOffset));
				pfValues[lIndice] += m_vPixels[GetOffset(i, j)];
				lNrValues[lIndice] ++;
			};

		pfValues[0] /= std::max(1, lNrValues[0]);
		pfValues[1] /= std::max(1, lNrValues[1]);
		pfValues[2] /= std::max(1, lNrValues[2]);
		pfValues[3] /= std::max(1, lNrValues[3]);

		/*
				// It's used only for CYMG - so cut it down to the basic

				if (x==m_lWidth-1)
					x = m_lWidth-2;
				if (y==m_lHeight-1)
					y = m_lHeight-2;

				pfValues[0]  = pfValues[1]  = pfValues[2]  = pfValues[3]  = 0;

				for (int i = x;i<=x+1;i++)
					for (int j = y;j<=y+1;j++)
						pfValues[CMYGZeroIndex(::GetBayerColor(i, j, m_CFAType))]  = m_vPixels[GetOffset(i, j)];
		*/
	};

protected:
	virtual void SetCFA(bool bCFA) override
	{
		m_bCFA = bCFA;
	};

public:
	CGrayBitmapT() :
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_bWord{ std::is_same_v<TType, WORD> },
		m_bDouble{ std::is_same_v<TType, double> },
		m_bDWord{ std::is_same_v<TType, std::uint32_t> },
		m_bFloat{ std::is_same_v<TType, float> },
		m_fMultiplier{ 1.0 }
	{
		if (m_bWord || m_bDouble || m_bFloat)
			m_fMultiplier = 256.0;
		else if (m_bDWord)
			m_fMultiplier = 256.0 * 65536.0;
	};

	virtual ~CGrayBitmapT() {};

	void SetMultiplier(double fMultiplier)
	{
		m_fMultiplier = fMultiplier;
	};

	virtual bool Init(int lWidth, int lHeight) override
	{
		m_lWidth = lWidth;
		m_lHeight = lHeight;
		return InitInternals();
	};

	virtual CMemoryBitmap* Clone(bool bEmpty = false) override
	{
		CGrayBitmapT<TType>* const pResult = new CGrayBitmapT<TType>;
		if (!bEmpty)
		{
			pResult->m_vPixels = m_vPixels;
			pResult->m_lWidth = m_lWidth;
			pResult->m_lHeight = m_lHeight;
		};
		pResult->m_bWord = m_bWord;
		pResult->m_bDouble = m_bDouble;
		pResult->m_bDWord = m_bDWord;
		pResult->m_bFloat = m_bFloat;
		pResult->m_CFATransform = m_CFATransform;
		pResult->m_CFAType = m_CFAType;
		pResult->m_bCYMG = m_bCYMG;
		pResult->CopyFrom(*this);

		return pResult;
	};

	virtual BAYERCOLOR GetBayerColor(int x, int y) override
	{
		return ::GetBayerColor(x, y, m_CFAType, m_xBayerOffset, m_yBayerOffset);
	};

	virtual int BitPerSample() override
	{
		return sizeof(TType) * 8;
	};

	virtual int IsFloat() override
	{
		return m_bFloat;
	};

	virtual int Width() override
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
			return m_lWidth / 2;
		else
			return m_lWidth;
	};

	virtual int Height() override
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
			return m_lHeight / 2;
		else
			return m_lHeight;
	};

	virtual int RealHeight() override
	{
		return m_lHeight;
	};

	virtual int RealWidth() override
	{
		return m_lWidth;
	};

	virtual void SetValue(int i, int j, double fGray) override
	{
		CheckXY(i, j);
		m_vPixels[GetOffset(i, j)] = fGray;
	};

	virtual void GetValue(int i, int j, double& fGray) override
	{
		//if (CFAT_SUPERPIXEL == m_CFATransform)  Bug fix 15th August 2020
		//{
		//	i *= 2; j *= 2;
		//}
		CheckXY(i, j);
		fGray = m_vPixels[GetOffset(i, j)];
	};

	virtual void SetPixel(int i, int j, double fRed, double fGreen, double fBlue) override
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
		{
			SetPixel(i * 2, j * 2, fRed);
			SetPixel(i * 2, j * 2 + 1, fGreen);
			SetPixel(i * 2 + 1, j * 2, fGreen);
			SetPixel(i * 2 + 1, j * 2 + 1, fBlue);
		}
		else if (m_CFATransform == CFAT_NONE)
			SetPixel(i, j, fRed);
		else
			SetPixel(i, j, GetPrimary(i, j, fRed, fGreen, fBlue));
	};

	inline virtual void SetPixel(int i, int j, double fGray) override
	{
		SetValue(i, j, fGray * m_fMultiplier);
	};

	virtual void GetPixel(int i, int j, double& fRed, double& fGreen, double& fBlue) override
	{
		CheckXY(i, j);
		fRed = fGreen = fBlue = 0.0;

		if (m_CFATransform == CFAT_SUPERPIXEL)
		{
			assert(m_bWord);
			if (IsXYOk((i - 1) * 2, (j - 1) * 2) && IsXYOk((i + 1) * 2 + 1, (j + 1) * 2 + 1))
			{
				TType* pValue = &(m_vPixels[GetOffset(i * 2, j * 2)]);

				switch (m_CFAType)
				{
				case CFATYPE_GRBG:
					fRed = (*(pValue + 1)) / m_fMultiplier;
					fGreen = ((*pValue) + (*(pValue + 1 + m_lWidth))) / 2.0 / m_fMultiplier;
					fBlue = (*(pValue + m_lWidth)) / m_fMultiplier;
					break;
				case CFATYPE_GBRG:
					fRed = (*(pValue + m_lWidth)) / m_fMultiplier;
					fGreen = ((*pValue) + (*(pValue + 1 + m_lWidth))) / 2.0 / m_fMultiplier;
					fBlue = (*(pValue + 1)) / m_fMultiplier;
					break;
				case CFATYPE_BGGR:
					fRed = (*(pValue + 1 + m_lWidth)) / m_fMultiplier;
					fGreen = ((*(pValue + m_lWidth)) + (*(pValue + 1))) / 2.0 / m_fMultiplier;
					fBlue = (*pValue) / m_fMultiplier;
					break;
				case CFATYPE_RGGB:
					fRed = (*pValue) / m_fMultiplier;
					fGreen = ((*(pValue + m_lWidth)) + (*(pValue + 1))) / 2.0 / m_fMultiplier;
					fBlue = (*(pValue + 1 + m_lWidth)) / m_fMultiplier;
					break;
				}
			}
		}
		else if (m_CFATransform == CFAT_RAWBAYER)
		{
			assert(m_bWord);
			TType* pValue = &(m_vPixels[GetOffset(i, j)]);

			switch (::GetBayerColor(i, j, m_CFAType, m_xBayerOffset, m_yBayerOffset))
			{
			case BAYER_RED:
				fRed = (*pValue) / m_fMultiplier;
				break;
			case BAYER_GREEN:
				fGreen = (*pValue) / m_fMultiplier;
				break;
			case BAYER_BLUE:
				fBlue = (*pValue) / m_fMultiplier;
				break;
			};
		}
		else if ((m_CFATransform == CFAT_BILINEAR) || (m_CFATransform == CFAT_AHD))
		{
			assert(m_bWord);
			if (m_bCYMG)
			{
				double fValue[4]; // Myself
				InterpolateAll(fValue, i, j);

				CYMGToRGB(fValue[CMYGZeroIndex(BAYER_CYAN)] / m_fMultiplier,
					fValue[CMYGZeroIndex(BAYER_YELLOW)] / m_fMultiplier,
					fValue[CMYGZeroIndex(BAYER_MAGENTA)] / m_fMultiplier,
					fValue[CMYGZeroIndex(BAYER_GREEN2)] / m_fMultiplier,
					fRed, fGreen, fBlue
				);
			}
			else
			{
				TType* pValue = &(m_vPixels[GetOffset(i, j)]);
				switch (::GetBayerColor(i, j, m_CFAType, m_xBayerOffset, m_yBayerOffset))
				{
				case BAYER_RED:
					fRed = (*pValue) / m_fMultiplier;
					fGreen = InterpolateGreen(i, j, pValue) / m_fMultiplier;
					fBlue = InterpolateBlue(i, j, pValue) / m_fMultiplier;
					break;
				case BAYER_GREEN:
					fRed = InterpolateRed(i, j, pValue) / m_fMultiplier;
					fGreen = (*pValue) / m_fMultiplier;
					fBlue = InterpolateBlue(i, j, pValue) / m_fMultiplier;
					break;
				case BAYER_BLUE:
					fRed = InterpolateRed(i, j, pValue) / m_fMultiplier;
					fGreen = InterpolateGreen(i, j, pValue) / m_fMultiplier;
					fBlue = (*pValue) / m_fMultiplier;
					break;
				};
			};

		}
		else if (m_CFATransform == CFAT_GRADIENT)
		{
		}
		else
		{
			fRed = fBlue = fGreen = m_vPixels[GetOffset(i, j)] / m_fMultiplier;
		};
	};

	virtual void inline GetPixel(int i, int j, double& fGray) override
	{
		GetValue(i, j, fGray);
		fGray /= m_fMultiplier;
	};

	virtual bool GetScanLine(int j, void* pScanLine) override
	{
		bool bResult = false;

		if (j < m_lHeight)
		{
			memcpy(pScanLine, &(m_vPixels[j * m_lWidth]), sizeof(TType) * m_lWidth);
			bResult = true;
		};

		return bResult;
	};

	virtual bool SetScanLine(int j, void* pScanLine) override
	{
		bool bResult = false;

		if (j < m_lHeight)
		{
			memcpy(&(m_vPixels[j * m_lWidth]), pScanLine, sizeof(TType) * m_lWidth);
			bResult = true;
		};

		return bResult;
	};

	virtual CMultiBitmap* CreateEmptyMultiBitmap() override
	{
		CMultiBitmap* const pResult = new CGrayMultiBitmapT<TType>();
		pResult->SetBitmapModel(this);
		return pResult;
	};

	virtual void RemoveHotPixels(CDSSProgress* pProgress = nullptr) override;

	virtual void GetIterator(CPixelIterator** ppIterator, int x = 0, int y = 0) override
	{
		CSmartPtr<CPixelIterator> pResult;
		CGrayPixelIterator<TType>* pIterator = new CGrayPixelIterator<TType>;

		pResult.Attach(pIterator);
		pIterator->Init(x, y, this);

		pResult.CopyTo(ppIterator);
	};

	TType* GetGrayPixel(int i, int j)
	{
		return &(m_vPixels[GetOffset(i, j)]);
	};

	double GetMultiplier() const
	{
		return m_fMultiplier;
	};

	virtual void GetMedianFilterEngine(CMedianFilterEngine** ppMedianFilterEngine) override
	{
		CSmartPtr<CGrayMedianFilterEngineT<TType>> pMedianFilterEngine;
		CSmartPtr<CMedianFilterEngine> pMedianFilterEngine2;

		pMedianFilterEngine.Attach(new CGrayMedianFilterEngineT<TType>);
		pMedianFilterEngine->SetInputBitmap(this);

		pMedianFilterEngine2 = pMedianFilterEngine;
		pMedianFilterEngine2.CopyTo(ppMedianFilterEngine);
	};

	virtual double GetMaximumValue() override
	{
		return m_fMultiplier * 256.0;
	};

	virtual void GetCharacteristics(CBitmapCharacteristics& bc) override
	{
		bc.m_bFloat = m_bFloat;
		bc.m_dwHeight = m_lHeight;
		bc.m_dwWidth = m_lWidth;
		bc.m_lNrChannels = 1;
		bc.m_lBitsPerPixel = BitPerSample();
	};
};

/* ------------------------------------------------------------------- */

typedef CGrayBitmapT<double> CGrayBitmap;
typedef CGrayBitmapT<std::uint8_t> C8BitGrayBitmap;
typedef CGrayBitmapT<std::uint16_t> C16BitGrayBitmap;
typedef CGrayBitmapT<std::uint32_t> C32BitGrayBitmap;
typedef CGrayBitmapT<float> C32BitFloatGrayBitmap;


template <typename TType, typename TTypeOutput = TType>
class CColorMultiBitmapT : public CMultiBitmap
{
protected:
	virtual bool CreateNewMemoryBitmap(CMemoryBitmap** ppBitmap) override
	{
		CSmartPtr<CMemoryBitmap> pBitmap;

		if (m_pBitmapModel)
		{
			CBitmapCharacteristics bc;
			m_pBitmapModel->GetCharacteristics(bc);
			if (bc.m_lNrChannels == 3)
				pBitmap.Attach(m_pBitmapModel->Clone(true));
			else
			{
				bc.m_lNrChannels = 3;
				CreateBitmap(bc, &pBitmap);
			};
		}
		else
			pBitmap.Attach(new CColorBitmapT<TType>);

		return pBitmap.CopyTo(ppBitmap);
	};

	virtual bool CreateOutputMemoryBitmap(CMemoryBitmap** ppBitmap) override
	{
		CSmartPtr<CMemoryBitmap> pBitmap;
		pBitmap.Attach(new CColorBitmapT<TTypeOutput>);

		return pBitmap.CopyTo(ppBitmap);
	};

	virtual bool SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines) override
	{
		bool bResult = false;
		// Each scan line consist of lWidth TType values
		TTypeOutput* pRedCurrentValue;
		TTypeOutput* pGreenCurrentValue;
		TTypeOutput* pBlueCurrentValue;
		std::vector<TType> vRedValues;
		std::vector<TType> vGreenValues;
		std::vector<TType> vBlueValues;
		std::vector<TType> vWorkingBuffer1;
		std::vector<TType> vWorkingBuffer2;
		std::vector<double> vdWork1;			// Used for AutoAdaptiveWeightedAverage
		std::vector<double> vdWork2;			// Used for AutoAdaptiveWeightedAverage
		double fMaximum = pBitmap->GetMaximumValue();
		const int lWidth = pBitmap->RealWidth();

		std::vector<TTypeOutput> outputScanBuffer;
		try {
			outputScanBuffer.resize(lWidth * 3);
		}
		catch (...) {
			ZOutOfMemory e("Could not allocate storage for output scanline");
			ZTHROW(e);
		}

		pRedCurrentValue = outputScanBuffer.data();
		pGreenCurrentValue = pRedCurrentValue + lWidth;
		pBlueCurrentValue = pGreenCurrentValue + lWidth;

		vRedValues.reserve(vScanLines.size());
		vGreenValues.reserve(vScanLines.size());
		vBlueValues.reserve(vScanLines.size());
		vWorkingBuffer1.reserve(vScanLines.size());
		vWorkingBuffer2.reserve(vScanLines.size());
		vdWork1.reserve(vScanLines.size());
		vdWork2.reserve(vScanLines.size());

		for (int i = 0; i < lWidth; i++)
		{
			TType* pRedValue;
			TType* pGreenValue;
			TType* pBlueValue;

			vRedValues.resize(0);
			vGreenValues.resize(0);
			vBlueValues.resize(0);
			for (auto p : vScanLines)
			{
				pRedValue = static_cast<TType*>(p) + i;
				pGreenValue = pRedValue + lWidth;
				pBlueValue = pGreenValue + lWidth;

				if (*pRedValue || !m_vImageOrder.empty())	// Remove 0
					vRedValues.push_back(*pRedValue);
				if (*pGreenValue || !m_vImageOrder.empty())	// Remove 0
					vGreenValues.push_back(*pGreenValue);
				if (*pBlueValue || !m_vImageOrder.empty())	// Remove 0
					vBlueValues.push_back(*pBlueValue);
			}

			if (m_bHomogenization)
			{
				//	if ((i==843) && (lLine==934))
				//		DebugBreak();

				if (m_pHomBitmap)
				{
					double fAverage, fSigma;
					double fRed, fGreen, fBlue;

					fSigma = Sigma2(vRedValues, fAverage);
					fRed = fSigma / std::max(1.0, fAverage) * 256.0;
					fSigma = Sigma2(vGreenValues, fAverage);
					fGreen = fSigma / std::max(1.0, fAverage) * 256.0;
					fSigma = Sigma2(vBlueValues, fAverage);
					fBlue = fSigma / std::max(1.0, fAverage) * 256.0;

					m_pHomBitmap->SetPixel(i, lLine, fRed, fGreen, fBlue);
				};

				if (!m_vImageOrder.empty())
				{
					std::vector<TType> vAuxRedValues;
					std::vector<TType> vAuxGreenValues;
					std::vector<TType> vAuxBlueValues;
					// Change the order to respect the order of the images
					std::swap(vAuxRedValues, vRedValues);
					std::swap(vAuxGreenValues, vGreenValues);
					std::swap(vAuxBlueValues, vBlueValues);
					for (const size_t imgOrder : m_vImageOrder)
					{
						if (vAuxRedValues[imgOrder] || vAuxGreenValues[imgOrder] || vAuxBlueValues[imgOrder])
						{
							vRedValues.push_back(vAuxRedValues[imgOrder]);
							vGreenValues.push_back(vAuxGreenValues[imgOrder]);
							vBlueValues.push_back(vAuxBlueValues[imgOrder]);
						}
					}

					Homogenize(vRedValues, fMaximum);
					Homogenize(vGreenValues, fMaximum);
					Homogenize(vBlueValues, fMaximum);
				}
				else
				{
					Homogenize(vRedValues, fMaximum);
					Homogenize(vGreenValues, fMaximum);
					Homogenize(vBlueValues, fMaximum);
				};

				if (vRedValues.empty() || vGreenValues.empty() || vBlueValues.empty())
				{
					vRedValues.resize(0);
					vGreenValues.resize(0);
					vBlueValues.resize(0);
				};
			}
			else
			{
				if constexpr (sizeof(TType) == 4 && std::is_integral<TType>::value)
				{

					for (auto& x : vRedValues) x >>= 16;
					for (auto& x : vGreenValues) x >>= 16;
					for (auto& x : vBlueValues) x >>= 16;
				}
			}

			// Process the value
			if (m_Method == MBP_MEDIAN)
			{
				*pRedCurrentValue = Median(vRedValues);
				*pGreenCurrentValue = Median(vGreenValues);
				*pBlueCurrentValue = Median(vBlueValues);
			}
			else if (m_Method == MBP_AVERAGE)
			{
				*pRedCurrentValue = Average(vRedValues);
				*pGreenCurrentValue = Average(vGreenValues);
				*pBlueCurrentValue = Average(vBlueValues);
			}
			else if (m_Method == MBP_MAXIMUM)
			{
				*pRedCurrentValue = Maximum(vRedValues);
				*pGreenCurrentValue = Maximum(vGreenValues);
				*pBlueCurrentValue = Maximum(vBlueValues);
			}
			else if (m_Method == MBP_SIGMACLIP)
			{
				*pRedCurrentValue = KappaSigmaClip(vRedValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
				*pGreenCurrentValue = KappaSigmaClip(vGreenValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
				*pBlueCurrentValue = KappaSigmaClip(vBlueValues, m_fKappa, m_lNrIterations, vWorkingBuffer1);
			}
			else if (m_Method == MBP_MEDIANSIGMACLIP)
			{
				*pRedCurrentValue = MedianKappaSigmaClip(vRedValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
				*pGreenCurrentValue = MedianKappaSigmaClip(vGreenValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
				*pBlueCurrentValue = MedianKappaSigmaClip(vBlueValues, m_fKappa, m_lNrIterations, vWorkingBuffer1, vWorkingBuffer2);
			}
			else if (m_Method == MBP_AUTOADAPTIVE)
			{
				*pRedCurrentValue = AutoAdaptiveWeightedAverage(vRedValues, m_lNrIterations, vdWork1);
				*pGreenCurrentValue = AutoAdaptiveWeightedAverage(vGreenValues, m_lNrIterations, vdWork1);
				*pBlueCurrentValue = AutoAdaptiveWeightedAverage(vBlueValues, m_lNrIterations, vdWork1);
			};

			pRedCurrentValue++;
			pGreenCurrentValue++;
			pBlueCurrentValue++;
		};

		pBitmap->SetScanLine(lLine, outputScanBuffer.data());
		bResult = true;

		return bResult;
	};

public:
	CColorMultiBitmapT()
	{
	};

	virtual ~CColorMultiBitmapT()
	{
	};

	virtual int GetNrChannels() override
	{
		return 3;
	};

	virtual int GetNrBytesPerChannel() override
	{
		return sizeof(TType);
	};
};


template <typename TType>
class CColorMedianFilterEngineT : public CMedianFilterEngine
{
private:
	CColorBitmapT<TType>* m_pInBitmap;

public:
	CColorMedianFilterEngineT() {};
	virtual ~CColorMedianFilterEngineT() {};

	void SetInputBitmap(CColorBitmapT<TType>* pInBitmap)
	{
		m_pInBitmap = pInBitmap;
	};

	virtual bool GetFilteredImage(CMemoryBitmap** ppBitmap, int lFilterSize, CDSSProgress* pProgress) override;
};


class CColorBitmap
{
public:
	CColorBitmap()
	{}

	virtual ~CColorBitmap()
	{}

	virtual CMemoryBitmap* GetRed() = 0;
	virtual CMemoryBitmap* GetGreen() = 0;
	virtual CMemoryBitmap* GetBlue() = 0;
};
/* ------------------------------------------------------------------- */

template <typename TType>
class CColorBitmapT : public CMemoryBitmap, public CColorBitmap
{
	template <typename TType> class CColorPixelIterator;
	friend CColorPixelIterator<TType>;

public:
	template <typename TType>
	class CColorPixelIterator : public CPixelIterator
	{
	private:
		CSmartPtr<CColorBitmapT<TType>> m_pBitmap;
		TType* m_pRedValue;
		TType* m_pGreenValue;
		TType* m_pBlueValue;
		double m_fMultiplier;
		int m_lWidth, m_lHeight;

	public:
		CColorPixelIterator() :
			m_pRedValue{ nullptr },
			m_pGreenValue{ nullptr },
			m_pBlueValue{ nullptr },
			m_fMultiplier{ 1.0 },
			m_lWidth{ 0 },
			m_lHeight{ 0 }
		{}

		~CColorPixelIterator() {};

		virtual void Reset(int x, int y) override
		{
			m_lX = x;
			m_lY = y;

			const size_t lOffset = m_pBitmap->GetOffset(x, y);
			m_pRedValue = &(m_pBitmap->m_Red.m_vPixels[lOffset]);
			m_pGreenValue = &(m_pBitmap->m_Green.m_vPixels[lOffset]);
			m_pBlueValue = &(m_pBitmap->m_Blue.m_vPixels[lOffset]);
		};

		void Init(int x, int y, CColorBitmapT<TType>* pBitmap)
		{
			m_pBitmap = pBitmap;
			m_fMultiplier = pBitmap->m_fMultiplier;
			m_lWidth = pBitmap->m_lWidth;
			m_lHeight = pBitmap->m_lHeight;
			Reset(x, y);
		};

		virtual void GetPixel(double& fRed, double& fGreen, double& fBlue) override
		{
			if (m_pRedValue != nullptr)
			{
				fRed = static_cast<double>(*m_pRedValue) / m_fMultiplier;
				fGreen = static_cast<double>(*m_pGreenValue) / m_fMultiplier;
				fBlue = static_cast<double>(*m_pBlueValue) / m_fMultiplier;
			}
			else
				fRed = fGreen = fBlue = 0.0;
		};

		virtual void GetPixel(double& fGray) override
		{
			if (m_pRedValue != nullptr)
			{
				double H, S, L;

				ToHSL(static_cast<double>(*m_pRedValue) / m_fMultiplier,
					static_cast<double>(*m_pGreenValue) / m_fMultiplier,
					static_cast<double>(*m_pBlueValue) / m_fMultiplier,
					H, S, L);
				fGray = L * 255.0;
			}
			else
				fGray = 0.0;
		};

		virtual void SetPixel(double fRed, double fGreen, double fBlue) override
		{
			if (m_pRedValue != nullptr)
			{
				*m_pRedValue = fRed * m_fMultiplier;
				*m_pGreenValue = fGreen * m_fMultiplier;
				*m_pBlueValue = fBlue * m_fMultiplier;
			};
		};

		virtual void SetPixel(double fGray) override
		{
			if (m_pRedValue != nullptr)
			{
				const double value = fGray * m_fMultiplier;
				*m_pRedValue = value;
				*m_pGreenValue = value;
				*m_pBlueValue = value;
			};
		};

		virtual void operator++(int) override
		{
			if (m_pRedValue != nullptr && m_pGreenValue != nullptr && m_pBlueValue != nullptr)
			{
				if (m_lX < m_lWidth - 1)
					m_lX++;
				else if (m_lY < m_lHeight - 1)
				{
					m_lY++;
					m_lX = 0;
				}
				else
				{
					// End of iteration
					m_lX = -1;
					m_lY = -1;
					m_pRedValue = m_pGreenValue = m_pBlueValue = nullptr;
				};
				if (m_pRedValue != nullptr)
					m_pRedValue++;
				if (m_pGreenValue != nullptr)
					m_pGreenValue++;
				if (m_pBlueValue != nullptr)
					m_pBlueValue++;
			};
		};
	};
	friend CColorPixelIterator<TType>;
	friend CColorMedianFilterEngineT<TType>;

private:
	int m_lHeight;
	int m_lWidth;
	bool m_bWord;
	bool m_bDouble;
	bool m_bDWord;
	bool m_bFloat;
	const double m_fMultiplier;

public:
	CGrayBitmapT<TType> m_Red;
	CGrayBitmapT<TType> m_Green;
	CGrayBitmapT<TType> m_Blue;

private:
	inline void CheckXY(int x, int y)
	{
		ZASSERTSTATE(x >= 0 && x < m_lWidth && y >= 0 && y < m_lHeight);
	};

	size_t GetOffset(int x, int y)
	{
		CheckXY(x, y);
		if (m_bTopDown)
			return static_cast<size_t>(m_lWidth) * static_cast<size_t>(y) + static_cast<size_t>(x);
		else
			return static_cast<size_t>(m_lWidth) * (static_cast<size_t>(m_lHeight) - 1 - static_cast<size_t>(y)) + static_cast<size_t>(x);
	};

public:
	CColorBitmapT() :
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_bWord{ std::is_same_v<TType, std::uint16_t> },
		m_bDouble{ std::is_same_v<TType, double> },
		m_bDWord{ std::is_same_v<TType, std::uint32_t> },
		m_bFloat{ std::is_same_v<TType, float> },
		m_fMultiplier{ (std::is_same_v<TType, WORD> || std::is_same_v<TType, double> || std::is_same_v<TType, float>) ? 256.0 : (std::is_same_v<TType, std::uint32_t> ? 256.0 * 65536.0 : 1.0) }
	{
		m_bTopDown = true;
		m_Red.AddRef();
		m_Green.AddRef();
		m_Blue.AddRef();
	};

	virtual ~CColorBitmapT() {};

	virtual CMemoryBitmap* Clone(bool bEmpty = false) override
	{
		CColorBitmapT<TType>* const pResult = new CColorBitmapT<TType>();
		if (!bEmpty)
		{
			pResult->m_lHeight = m_lHeight;
			pResult->m_lWidth = m_lWidth;
			pResult->m_Red.m_vPixels = m_Red.m_vPixels; //vRedPixel	= m_Red.m_vPixels;
			pResult->m_Green.m_vPixels = m_Green.m_vPixels; //vRedPixel	= m_Red.m_vPixels;
			pResult->m_Blue.m_vPixels = m_Blue.m_vPixels; //vRedPixel	= m_Red.m_vPixels;
		};
		pResult->m_bWord = m_bWord;
		pResult->m_bDouble = m_bDouble;
		pResult->m_bDWord = m_bDWord;
		pResult->m_bFloat = m_bFloat;

		pResult->CopyFrom(*this);

		return pResult;
	};

	virtual int BitPerSample() override
	{
		return sizeof(TType) * 8;
	};

	virtual int IsFloat() override
	{
		return m_bFloat;
	};

	virtual int Width() override
	{
		return m_lWidth;
	};

	virtual int Height() override
	{
		return m_lHeight;
	};

	virtual bool Init(int lWidth, int lHeight)
	{
		m_lWidth = lWidth;
		m_lHeight = lHeight;

		const bool bResult = m_Red.Init(lWidth, lHeight) && m_Green.Init(lWidth, lHeight) && m_Blue.Init(lWidth, lHeight);
		return bResult;
	};

	virtual bool IsMonochrome() override
	{
		return false;
	};

	virtual void SetValue(int i, int j, double fRed, double fGreen, double fBlue) override
	{
		CheckXY(i, j);

		const size_t lOffset = GetOffset(i, j);

		m_Red.m_vPixels[lOffset] = fRed;
		m_Green.m_vPixels[lOffset] = fGreen;
		m_Blue.m_vPixels[lOffset] = fBlue;
	};

	virtual void GetValue(int i, int j, double& fRed, double& fGreen, double& fBlue) override
	{
		CheckXY(i, j);

		const size_t lOffset = GetOffset(i, j);

		fRed = m_Red.m_vPixels[lOffset];
		fGreen = m_Green.m_vPixels[lOffset];
		fBlue = m_Blue.m_vPixels[lOffset];
	};

	virtual void SetPixel(int i, int j, double fRed, double fGreen, double fBlue) override
	{
		CheckXY(i, j);

		const size_t lOffset = GetOffset(i, j);
		m_Red.m_vPixels[lOffset] = fRed * m_fMultiplier;
		m_Green.m_vPixels[lOffset] = fGreen * m_fMultiplier;
		m_Blue.m_vPixels[lOffset] = fBlue * m_fMultiplier;
	};

	virtual void SetPixel(int i, int j, double fGray) override
	{

		CheckXY(i, j);		// Throw if not

		const size_t lOffset = GetOffset(i, j);
		const double value = fGray * m_fMultiplier;
		m_Red.m_vPixels[lOffset] = value;
		m_Green.m_vPixels[lOffset] = value;
		m_Blue.m_vPixels[lOffset] = value;
	};

	virtual void GetPixel(int i, int j, double& fRed, double& fGreen, double& fBlue) override
	{
		fRed = fGreen = fBlue = 0.0;

		CheckXY(i, j);

		const size_t lOffset = GetOffset(i, j);

		fRed = m_Red.m_vPixels[lOffset] / m_fMultiplier;
		fGreen = m_Green.m_vPixels[lOffset] / m_fMultiplier;
		fBlue = m_Blue.m_vPixels[lOffset] / m_fMultiplier;
	};

	virtual void GetPixel(int i, int j, double& fGray) override
	{
		double fRed, fGreen, fBlue;
		fGray = 0.0;

		GetPixel(i, j, fRed, fGreen, fBlue);
		double H, S, L;

		ToHSL(fRed, fGreen, fBlue, H, S, L);
		fGray = L * 255.0;
	};

	virtual bool GetScanLine(int j, void* pScanLine) override
	{
		bool bResult = false;

		if (j < m_lHeight)
		{
			BYTE* const pTempScan = static_cast<BYTE*>(pScanLine);
			const size_t w = static_cast<size_t>(m_lWidth);
			const size_t index = static_cast<size_t>(j) * w;

			memcpy(pTempScan, &(m_Red.m_vPixels[index]), sizeof(TType) * w);
			memcpy(pTempScan + sizeof(TType) * w, &(m_Green.m_vPixels[index]), sizeof(TType) * w);
			memcpy(pTempScan + sizeof(TType) * 2 * w, &(m_Blue.m_vPixels[index]), sizeof(TType) * w);
			bResult = true;
		};

		return bResult;
	};

	virtual bool SetScanLine(int j, void* pScanLine) override
	{
		bool bResult = false;

		if (j < m_lHeight)
		{
			const BYTE* const pTempScan = static_cast<const BYTE*>(pScanLine);
			const size_t w = static_cast<size_t>(m_lWidth);
			const size_t index = static_cast<size_t>(j) * w;

			memcpy(&(m_Red.m_vPixels[index]), pTempScan, sizeof(TType) * w);
			memcpy(&(m_Green.m_vPixels[index]), pTempScan + sizeof(TType) * w, sizeof(TType) * w);
			memcpy(&(m_Blue.m_vPixels[index]), pTempScan + sizeof(TType) * 2 * w, sizeof(TType) * w);
			bResult = true;
		};

		return bResult;
	};

	void Clear()
	{
		m_lHeight = 0;
		m_lWidth = 0;
		m_Red.m_vPixels.clear();
		m_Green.m_vPixels.clear();
		m_Blue.m_vPixels.clear();
	};

	virtual CMultiBitmap* CreateEmptyMultiBitmap() override
	{
		CMultiBitmap* const pResult = new CColorMultiBitmapT<TType>();
		pResult->SetBitmapModel(this);
		return pResult;
	};

	virtual void GetIterator(CPixelIterator** ppIterator, int x = 0, int y = 0) override
	{
		CSmartPtr<CPixelIterator> pResult;
		CColorPixelIterator<TType>* pIterator = new CColorPixelIterator<TType>;

		pResult.Attach(pIterator);
		pIterator->Init(x, y, this);

		pResult.CopyTo(ppIterator);
	};

	virtual void GetMedianFilterEngine(CMedianFilterEngine** ppMedianFilterEngine) override
	{
		CSmartPtr<CColorMedianFilterEngineT<TType>> pMedianFilterEngine;
		CSmartPtr<CMedianFilterEngine> pMedianFilterEngine2;

		pMedianFilterEngine.Attach(new CColorMedianFilterEngineT<TType>);
		pMedianFilterEngine->SetInputBitmap(this);

		pMedianFilterEngine2 = pMedianFilterEngine;
		pMedianFilterEngine2.CopyTo(ppMedianFilterEngine);
	};

	TType* GetRedPixel(int i, int j)
	{
		return &(m_Red.m_vPixels[GetOffset(i, j)]);
	};
	TType* GetGreenPixel(int i, int j)
	{
		return &(m_Green.m_vPixels[GetOffset(i, j)]);
	};
	TType* GetBluePixel(int i, int j)
	{
		return &(m_Blue.m_vPixels[GetOffset(i, j)]);
	};

	virtual CMemoryBitmap* GetRed() override
	{
		return &m_Red;
	};

	virtual CMemoryBitmap* GetGreen() override
	{
		return &m_Green;
	};

	virtual CMemoryBitmap* GetBlue() override
	{
		return &m_Blue;
	};

	double GetMultiplier()
	{
		return m_fMultiplier;
	};

	virtual double GetMaximumValue() override
	{
		return m_fMultiplier * 256.0;
	};

	virtual void RemoveHotPixels(CDSSProgress* pProgress = nullptr) override
	{
		m_Red.RemoveHotPixels(pProgress);
		m_Green.RemoveHotPixels(pProgress);
		m_Blue.RemoveHotPixels(pProgress);
	};

	virtual void GetCharacteristics(CBitmapCharacteristics& bc) override
	{
		bc.m_bFloat = m_bFloat;
		bc.m_dwHeight = m_lHeight;
		bc.m_dwWidth = m_lWidth;
		bc.m_lNrChannels = 3;
		bc.m_lBitsPerPixel = BitPerSample();
	};
};

/* ------------------------------------------------------------------- */

typedef CColorBitmapT<std::uint8_t> C24BitColorBitmap;
typedef CColorBitmapT<std::uint16_t> C48BitColorBitmap;
typedef CColorBitmapT<std::uint32_t> C96BitColorBitmap;
typedef CColorBitmapT<float> C96BitFloatColorBitmap;


template <class T>
class CopyableSmartPtr final
{
private:
	std::unique_ptr<T> p;
public:
	template <class OTHER> CopyableSmartPtr(std::unique_ptr<OTHER>&) = delete;
	CopyableSmartPtr() = delete;
	CopyableSmartPtr& operator=(const CopyableSmartPtr&) = delete;

	template <class OTHER>
	CopyableSmartPtr(std::unique_ptr<OTHER>&& rhs) : p{ std::move(rhs) } {}

	CopyableSmartPtr(const CopyableSmartPtr& rhs) : p{ rhs->clone() } {}

	typename std::unique_ptr<T>::pointer get() const noexcept { return p.get(); }
	typename std::unique_ptr<T>::pointer operator->() const noexcept { return this->get(); }
};
