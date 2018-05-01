#ifndef _BITMAPEXT_H__
#define _BITMAPEXT_H__

#include "Multitask.h"

#ifndef DSSFILEDECODING
#define DSSFILEDECODING 1
#endif

#define ALTERNATEHOMOGENEIZATION

/* ------------------------------------------------------------------- */

#if DSSFILEDECODING==1
BOOL	IsFITSRawBayer();		// From FITSUtil.h
BOOL	IsFITSSuperPixels();	// From FITSUtil.h
#else
inline BOOL	IsFITSRawBayer()	{ return FALSE; };	// From FITSUtil.h
inline BOOL	IsFITSSuperPixels() { return FALSE; };	// From FITSUtil.h
#endif // !DSSFILEDECODING

/* ------------------------------------------------------------------- */

typedef enum tagEXTRAINFOTYPE
{
	EIT_UNKNOWN			= 0,
	EIT_STRING			= 1,
	EIT_LONG			= 2,
	EIT_DOUBLE			= 3
}EXTRAINFOTYPE;

class CExtraInfo
{
public :
	EXTRAINFOTYPE		m_Type;
	CString				m_strName;
	CString				m_strValue;
	CString				m_strComment;
	LONG				m_lValue;
	double				m_fValue;
	bool				m_bPropagate;

private :
	void	CopyFrom(const CExtraInfo & ei)
	{
		m_Type		= ei.m_Type;
		m_strName	= ei.m_strName;
		m_strValue	= ei.m_strValue;
		m_lValue	= ei.m_lValue;
		m_fValue	= ei.m_fValue;
		m_strComment= ei.m_strComment;
		m_bPropagate= ei.m_bPropagate;
	};

public :
	CExtraInfo() 
	{
		m_bPropagate = false;
	};
	CExtraInfo(const CExtraInfo & ei) 
	{
		CopyFrom(ei);
	};
	~CExtraInfo() {};

	CExtraInfo & operator = (const CExtraInfo & ei)
	{
		CopyFrom(ei);
		return (*this);
	};
};

typedef std::vector<CExtraInfo>			EXTRAINFOVECTOR;

/* ------------------------------------------------------------------- */

class CBitmapExtraInfo
{
public :
	EXTRAINFOVECTOR			m_vExtras;

public :
	CBitmapExtraInfo() {};
	~CBitmapExtraInfo() {};

	void	AddInfo(const CExtraInfo & ei)
	{
		m_vExtras.push_back(ei);
	};

	void	AddInfo(LPCTSTR szName, LPCTSTR szValue, LPCTSTR szComment = NULL, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type		= EIT_STRING;
		ei.m_strName	= szName;
		ei.m_strComment = szComment;
		ei.m_strValue	= szValue;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};
	void	AddInfo(LPCTSTR szName, LONG lValue, LPCTSTR szComment = NULL, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type		= EIT_LONG;
		ei.m_strName	= szName;
		ei.m_lValue		= lValue;
		ei.m_strComment = szComment;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};
	void	AddInfo(LPCTSTR szName, double fValue, LPCTSTR szComment = NULL, bool bPropagate = false)
	{
		CExtraInfo		ei;

		ei.m_Type		= EIT_DOUBLE;
		ei.m_strName	= szName;
		ei.m_fValue		= fValue;
		ei.m_strComment = szComment;
		ei.m_bPropagate = bPropagate;
		m_vExtras.push_back(ei);
	};

	void	Clear()
	{
		m_vExtras.clear();
	};
};

/* ------------------------------------------------------------------- */

class CDSSProgress;

#pragma pack(push, HDCOLORREFT, 1)

template <typename TType>
class COLORREFT
{
public :
	TType		red;
	TType		green;
	TType		blue;

private :
	void	CopyFrom(const COLORREFT<TType> & cr)
	{
		red		= cr.red;
		green	= cr.green;
		blue	= cr.blue;
	};

public :
	COLORREFT(TType r = 0, TType g = 0, TType b = 0)
	{
		red		= r;
		green	= g;
		blue	= b;
	};

	COLORREFT(const COLORREFT<TType> & cr)
	{
		CopyFrom(cr);
	};

	COLORREFT<TType> & operator = (const COLORREFT<TType> & cr)
	{
		CopyFrom(cr);
		return *this;
	};

};

typedef COLORREFT<BYTE>				COLORREF8;
typedef COLORREFT<WORD>				COLORREF16;
typedef COLORREFT<DWORD>			COLORREF32;
typedef COLORREFT<float>			COLORREF32F;

#pragma pack(pop, HDCOLORREFT)

/* ------------------------------------------------------------------- */

inline void ToHSL(double Red, double Green, double Blue, double & H, double & S, double & L)
{
    double minval = min(Red, min(Green, Blue));
    double maxval = max(Red, max(Green, Blue));
    double mdiff  = maxval - minval;
    double msum   = maxval + minval;
   
    L = msum / 510.0f;

    if (maxval == minval) 
    {
      S = 0.0f;
      H = 0.0f; 
    }   
    else 
    { 
      double rnorm = (maxval - Red  ) / mdiff;      
      double gnorm = (maxval - Green) / mdiff;
      double bnorm = (maxval - Blue ) / mdiff;   

      S = (L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

      if (Red   == maxval) H = 60.0f * (6.0f + bnorm - gnorm);
      if (Green == maxval) H = 60.0f * (2.0f + rnorm - bnorm);
      if (Blue  == maxval) H = 60.0f * (4.0f + gnorm - rnorm);
      if (H > 360.0f) 
		  H = H - 360.0f;
    }
};

inline double ToRGB1(float rm1, float rm2, float rh)
{
  if (rh > 360.0f) 
	  rh -= 360.0f;
  else if (rh <   0.0f) 
	  rh += 360.0f;
 
  if      (rh <  60.0f) 
	  rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;   
  else if (rh < 180.0f) 
	  rm1 = rm2;
  else if (rh < 240.0f) 
	  rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;      
                   
  return (rm1 * 255.0);
}

inline void ToRGB(double H, double S, double L, double & Red, double & Green, double & Blue)
{
    if (S == 0.0) 
    {
      Red = Green = Blue = L * 255.0;
    }
    else
    {
      double rm1, rm2;
         
      if (L <= 0.5f) 
		  rm2 = (double)(L + L * S);  
      else                     
		  rm2 = (double)(L + S - L * S);
      rm1 = (double)(2.0f * L - rm2);   

      Red   = ToRGB1(rm1, rm2, (double)(H + 120.0f));   
      Green = ToRGB1(rm1, rm2, (double)H);
      Blue  = ToRGB1(rm1, rm2, (double)(H - 120.0f));
    }
};

inline double GetIntensity(COLORREF crColor)
{
	double	H, S, L;

	ToHSL(GetRValue(crColor), GetGValue(crColor), GetBValue(crColor), H, S, L);

	return L;
};

inline double GetIntensity(COLORREF16 crColor)
{
	double	H, S, L;

	ToHSL(crColor.red/256.0, crColor.green/256.0, crColor.blue/256.0, H, S, L);

	return L;
};

/* ------------------------------------------------------------------- */

inline double ClampPixel(double fValue)
{
	if (fValue<0)
		return 0;
	else if (fValue>255.0*256.0)
		return 255.0*256.0;
	else
		return fValue;
};

/* ------------------------------------------------------------------- */

class CMemoryBitmap;

typedef enum
{
	MBP_AVERAGE		= 1,
	MBP_MEDIAN	    = 2,
	MBP_MAXIMUM		= 3,
	MBP_SIGMACLIP	= 4,
	MBP_ENTROPYAVERAGE	= 5,
	MBP_AUTOADAPTIVE	= 6,
	MBP_MEDIANSIGMACLIP = 7,
	MBP_FASTAVERAGE		= 8
}MULTIBITMAPPROCESSMETHOD;

/* ------------------------------------------------------------------- */

inline void	FormatFromMethod(CString & strText, MULTIBITMAPPROCESSMETHOD Method, double fKappa, LONG lNrIterations)
{
	strText = "";
	switch (Method)
	{
	case MBP_AVERAGE :
		strText.Format(IDS_RECAP_AVERAGE);
		break;
	case MBP_MEDIAN :
		strText.Format(IDS_RECAP_MEDIAN);
		break;
	case MBP_MAXIMUM :
		strText.Format(IDS_RECAP_MAXIMUM);
		break;
	case MBP_SIGMACLIP :
		strText.Format(IDS_RECAP_KAPPASIGMA, fKappa, lNrIterations);
		break;
	case MBP_AUTOADAPTIVE :
		strText.Format(IDS_RECAP_AUTOADAPTIVE, lNrIterations);
		break;
	case MBP_ENTROPYAVERAGE :
		strText.Format(IDS_RECAP_ENTROPYAVERAGE);
		break;
	case MBP_MEDIANSIGMACLIP :
		strText.Format(IDS_RECAP_MEDIANSIGMACLIP, fKappa, lNrIterations);
	};
};

/* ------------------------------------------------------------------- */

class CBitmapPartFile
{
public :
	CString						m_strFile;
	LONG						m_lStartRow;
	LONG						m_lEndRow;
	LONG						m_lWidth;
	LONG						m_lNrBitmaps;

private :
	void	CopyFrom(const CBitmapPartFile & bp)
	{
		m_strFile	= bp.m_strFile;
		m_lStartRow = bp.m_lStartRow;
		m_lEndRow	= bp.m_lEndRow;
		m_lWidth	= bp.m_lWidth;
		m_lNrBitmaps= bp.m_lNrBitmaps;
	};

public :
	CBitmapPartFile(LPCTSTR szFile, LONG lStartRow, LONG lEndRow)
	{
		m_strFile	= szFile;
		m_lStartRow = lStartRow;
		m_lEndRow	= lEndRow;
	};

	CBitmapPartFile(const CBitmapPartFile & bp)
	{
		CopyFrom(bp);
	};

	const CBitmapPartFile & operator = (const CBitmapPartFile & bp)
	{
		CopyFrom(bp);
		return (*this);
	};

	virtual ~CBitmapPartFile()
	{
	};
};

typedef std::vector<CBitmapPartFile>	BITMAPPARTFILEVECTOR;

class CMultiBitmap : public CRefCount
{
protected :
	CSmartPtr<CMemoryBitmap>	m_pBitmapModel;
	CSmartPtr<CMemoryBitmap>	m_pHomBitmap;
	MULTIBITMAPPROCESSMETHOD	m_Method;
	double						m_fKappa;
	LONG						m_lNrIterations;
	LONG						m_lNrBitmaps;
	LONG						m_lNrAddedBitmaps;
	BITMAPPARTFILEVECTOR		m_vFiles;
	LONG						m_lWidth,
								m_lHeight;
	BOOL						m_bInitDone;
	BOOL						m_bHomogenization;
	double						m_fMaxWeight;
	std::vector<LONG>			m_vImageOrder;

private :
	void	DestroyTempFiles();
	void	InitParts();
	void	SmoothOut(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppOutBitmap);

public :
	virtual BOOL	SetScanLines(CMemoryBitmap * pBitmap, LONG lLine, const std::vector<void*>	& vScanLines) = 0;

public :
	CMultiBitmap()
	{
		m_lNrBitmaps = 0;
		m_lWidth	 = 0;
		m_lHeight	 = 0;
		m_bInitDone	 = FALSE;
		m_lNrAddedBitmaps = 0;
		m_bHomogenization = FALSE;
		m_fMaxWeight	  = 0;
	};

	virtual ~CMultiBitmap()
	{
		DestroyTempFiles();
	};

	void			SetBitmapModel(CMemoryBitmap * pBitmap);
	virtual BOOL	CreateNewMemoryBitmap(CMemoryBitmap ** ppBitmap) = 0;
	virtual BOOL	CreateOutputMemoryBitmap(CMemoryBitmap ** ppBitmap) = 0;

	virtual void	SetNrBitmaps(LONG lNrBitmaps)
	{
		m_lNrBitmaps = lNrBitmaps;
	};

	LONG			GetNrBitmaps()
	{
		return m_lNrBitmaps;
	};

	LONG			GetNrAddedBitmaps()
	{
		return m_lNrAddedBitmaps;
	};

	void			SetImageOrder(const std::vector<LONG>	& vImageOrder)
	{
		m_vImageOrder = vImageOrder;
	};

	virtual BOOL	AddBitmap(CMemoryBitmap * pMemoryBitmap, CDSSProgress * pProgress = NULL);
	virtual BOOL	GetResult(CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress = NULL);
	virtual LONG	GetNrChannels() = 0;
	virtual LONG	GetNrBytesPerChannel() = 0;

	void	SetProcessingMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, LONG lNrIterations)
	{
		m_Method = Method;
		m_fKappa = fKappa;
		m_lNrIterations = lNrIterations;
	};

	void	SetHomogenization(BOOL bSet)
	{
		m_bHomogenization = bSet;
	};
};

/* ------------------------------------------------------------------- */

typedef enum
{
	BAYER_UNKNOWN	= 0,
	BAYER_RED		= 1,
	BAYER_GREEN		= 2,
	BAYER_BLUE		= 3,
	BAYER_CYAN		= 0xA,
	BAYER_GREEN2	= 0xB,
	BAYER_MAGENTA	= 0xC,
	BAYER_YELLOW	= 0xD,
	BAYER_NRCOLORS  = 0xD+1 
}BAYERCOLOR;

inline LONG	CMYGZeroIndex(BAYERCOLOR Color)
{
	return Color-BAYER_CYAN;
};

typedef enum CFATYPE
{
	CFATYPE_NONE		= 0,
	CFATYPE_BGGR		= 1,	
	CFATYPE_GRBG		= 2,
	CFATYPE_GBRG		= 3,
	CFATYPE_RGGB		= 4,

	// A = Cyan		B = Green		C = Magenta		D = Yellow
	CFATYPE_CGMY		= 0xABCD,
	CFATYPE_CGYM		= 0xABDC,
	CFATYPE_CMGY		= 0xACBD,
	CFATYPE_CMYG		= 0xACDB,
	CFATYPE_CYMG		= 0xADCB,
	CFATYPE_CYGM		= 0xADBC,
						  
	CFATYPE_GCMY		= 0xBACD,
	CFATYPE_GCYM		= 0xBADC,
	CFATYPE_GMCY		= 0xBCAD,
	CFATYPE_GMYC		= 0xBCDA,
	CFATYPE_GYCM		= 0xBDAC,
	CFATYPE_GYMC		= 0xBDCA,
						  
	CFATYPE_MCGY		= 0xCABD,
	CFATYPE_MCYG		= 0xCADB,
	CFATYPE_MGYC		= 0xCBDA,
	CFATYPE_MGCY		= 0xCBAD,
	CFATYPE_MYGC		= 0xCDBA,
	CFATYPE_MYCG		= 0xCDAB,
						  
	CFATYPE_YCGM		= 0xDABC,
	CFATYPE_YCMG		= 0xDACB,
	CFATYPE_YGMC		= 0xDBCA,
	CFATYPE_YGCM		= 0xDBAC,
	CFATYPE_YMCG		= 0xDCAB,
	CFATYPE_YMGC		= 0xDCBA,

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
}CFATYPE;

inline BOOL IsCYMGType(CFATYPE Type)
{
	return ((Type & 0xFFFFFFF0) != 0);
}

inline BOOL IsSimpleCYMG(CFATYPE Type)
{
	return !(Type & 0xFFFF0000);
};

/* ------------------------------------------------------------------- */

class CMedianFilterEngine : public CRefCount
{
protected :
	LONG					m_lFilterSize;
	CDSSProgress *			m_pProgress;

public :
	CMedianFilterEngine() 
	{
		m_lFilterSize = 1;
		m_pProgress   = NULL;
	};

	virtual ~CMedianFilterEngine() {};

	virtual BOOL	GetFilteredImage(CMemoryBitmap ** ppBitmap, LONG lFilterSize, CDSSProgress * pProgress) = 0;
};

/* ------------------------------------------------------------------- */

class CBitmapCharacteristics
{
public :
	LONG				m_lNrChannels;
	LONG				m_lBitsPerPixel;
	bool				m_bFloat;
	DWORD				m_dwWidth,
						m_dwHeight;

private :
	void	CopyFrom(const CBitmapCharacteristics & bc)
	{
		m_lNrChannels		= bc.m_lNrChannels;
		m_lBitsPerPixel		= bc.m_lBitsPerPixel;
		m_bFloat			= bc.m_bFloat;
		m_dwWidth			= bc.m_dwWidth;
		m_dwHeight			= bc.m_dwHeight;
	};

public :
	CBitmapCharacteristics() {};
	~CBitmapCharacteristics() {};

	CBitmapCharacteristics(const CBitmapCharacteristics & bc)
	{
		CopyFrom(bc);
	};

	CBitmapCharacteristics & operator = (const CBitmapCharacteristics & bc)
	{
		CopyFrom(bc);
		return (*this);
	};
};

BOOL	CreateBitmap(const CBitmapCharacteristics & bc, CMemoryBitmap ** ppOutBitmap);

/* ------------------------------------------------------------------- */

class CMemoryBitmap : public CRefCount 
{
public :
	class CPixelIterator : public CRefCount
	{
	protected :
		LONG								m_lX,
											m_lY;
	public :
		CPixelIterator() 
		{
			m_lX = -1;
			m_lY = -1;
		};

		~CPixelIterator() {};

		virtual void	Reset(LONG x, LONG y) = 0;

		virtual void	GetPixel(double & fRed, double & fGreen, double & fBlue)= 0;
		virtual void	GetPixel(double & fGray) = 0;
		virtual void	SetPixel(double fRed, double fGreen, double fBlue) = 0;
		virtual void	SetPixel(double fGray) {};

		virtual void operator ++(int) = 0;
		virtual void operator +=(int lIncrement)
		{
			while (lIncrement>0)
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
public :
	CBitmapExtraInfo	m_ExtraInfo;
	SYSTEMTIME			m_DateTime;

protected :
	BOOL				m_bTopDown;
	BOOL				m_bMaster;
	BOOL				m_bCFA;
	double				m_fExposure;
	LONG				m_lISOSpeed;
	LONG				m_lNrFrames;
	CString				m_strDescription;

protected :
	void			CopyFrom(const CMemoryBitmap & mb)
	{
		m_ExtraInfo			= mb.m_ExtraInfo;
		m_bTopDown			= mb.m_bTopDown;
		m_bMaster			= mb.m_bMaster;
		m_bCFA				= mb.m_bCFA;
		m_fExposure			= mb.m_fExposure;
		m_lISOSpeed			= mb.m_lISOSpeed;
		m_lNrFrames			= mb.m_lNrFrames;
		m_strDescription	= mb.m_strDescription;
		m_DateTime			= mb.m_DateTime;
	};

public :
	CMemoryBitmap() 
	{
		m_bMaster  = FALSE;
		m_bTopDown = FALSE;
		m_bCFA	   = FALSE;
		m_fExposure = 0;
		m_lISOSpeed = 0;
		m_lNrFrames = 0;
		m_DateTime.wYear = 0;
	};
	virtual ~CMemoryBitmap() 
	{
		ZTRACE_RUNTIME("Destroying memory bitmap %p", this);
	};

	virtual double	GetExposure()
	{
		return m_fExposure;
	};

	virtual void	SetExposure(double fExposure)
	{
		m_fExposure = fExposure;
	};

	virtual LONG	GetISOSpeed()
	{
		return m_lISOSpeed;
	};

	virtual void	SetISOSpeed(LONG lISOSpeed)
	{
		m_lISOSpeed = lISOSpeed;
	};

	virtual LONG	GetNrFrames()
	{
		return m_lNrFrames;
	};

	virtual void	SetNrFrames(LONG lNrFrames)
	{
		m_lNrFrames = lNrFrames;
	};

	virtual void	SetDescription(LPCTSTR szDescription)
	{
		m_strDescription = szDescription;
	};

	virtual void	GetDescription(CString & strDescription)
	{
		strDescription = m_strDescription;
	};

	virtual BOOL	Init(LONG lWidth, LONG lHeight) = 0;

	virtual BOOL	SetPixel(LONG i, LONG j, double fRed, double fGreen, double fBlue)  =0;
	virtual BOOL	SetPixel(LONG i, LONG j, double fGray)  =0;
	virtual BOOL	GetPixel(LONG i, LONG j, double & fRed, double & fGreen, double & fBlue) = 0;
	virtual BOOL	GetPixel(LONG i, LONG j, double & fGray) = 0;

	virtual BOOL	SetValue(LONG i, LONG j, double fRed, double fGreen, double fBlue) { return FALSE; };
	virtual BOOL	GetValue(LONG i, LONG j, double & fRed, double & fGreen, double & fBlue) { return FALSE; };
	virtual BOOL	SetValue(LONG i, LONG j, double fGray) { return FALSE; };
	virtual BOOL	GetValue(LONG i, LONG j, double & fGray) { return FALSE; };

	virtual BOOL	GetScanLine(LONG j, void * pScanLine) = 0;
	virtual BOOL	SetScanLine(LONG j, void * pScanLine) = 0;

	virtual COLORREF GetPixel(LONG i, LONG j)
	{
		COLORREF	crResult;
		double		fRed, fGreen, fBlue;

		GetPixel(i, j, fRed, fGreen, fBlue);
		crResult = RGB(fRed, fGreen, fBlue);

		return crResult;
	};

	virtual COLORREF16 GetPixel16(LONG i, LONG j)
	{
		// Use get pixel
		COLORREF16		crResult;
		double			fRed, fGreen, fBlue;
		

		GetPixel(i, j, fRed, fGreen, fBlue);

		crResult.red = (WORD)(fRed * 256.0);
		crResult.green = (WORD)(fGreen * 256.0);
		crResult.blue = (WORD)(fBlue * 256.0);

		return crResult;
	};

	virtual BOOL	SetPixel16(LONG i, LONG j, COLORREF16 crColor)
	{
		return SetPixel(i, j, (double)crColor.red/256.0, (double)crColor.green/256.0, (double)crColor.blue/256.0);
	};

	virtual LONG	Width() = 0;
	virtual LONG	Height() = 0;
	virtual LONG	BitPerSample() = 0;
	virtual LONG	IsFloat() = 0;
	virtual LONG	RealWidth()
	{
		return Width();
	};

	virtual LONG	RealHeight()
	{
		return Height();
	};

	void	SetOrientation(BOOL bTopDown)
	{
		m_bTopDown = bTopDown;
	};

	virtual BOOL	IsMonochrome() = 0;

	virtual void	SetMaster(BOOL bMaster)
	{
		m_bMaster = bMaster;
	};

	virtual BOOL	IsMaster()
	{
		return m_bMaster;
	};

	virtual void	SetCFA(BOOL bCFA)
	{
		m_bCFA = bCFA;
	};

	virtual BOOL	IsCFA()
	{
		return m_bCFA;
	};

	virtual BAYERCOLOR GetBayerColor(LONG x, LONG y)
	{
		return BAYER_UNKNOWN;
	};

	BOOL	IsOk()
	{
		return (Width() > 0) && (Height() > 0);
	};

	virtual CMemoryBitmap *	Clone(BOOL bEmpty = FALSE) = 0;

	virtual CMultiBitmap * CreateEmptyMultiBitmap() = 0;
	virtual void	AverageBitmap(CMemoryBitmap * pBitmap, CDSSProgress * pProgress) {};
	virtual void	RemoveHotPixels(CDSSProgress * pProgress = NULL) {};
	virtual void	GetMedianFilterEngine(CMedianFilterEngine ** pMedianFilterEngine)  = 0;

	virtual void	GetIterator(CPixelIterator ** ppIterator, LONG x = 0, LONG y = 0) = 0;
	virtual double	GetMaximumValue()  =0;
	virtual void	GetCharacteristics(CBitmapCharacteristics & bc) = 0;
};

typedef CSmartPtr<CMemoryBitmap::CPixelIterator>	PixelIterator;

/* ------------------------------------------------------------------- */

BOOL Subtract(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress = NULL, double fRedFactor = 1.0, double fGreenFactor = 1.0, double fBlueFactor = 1.0);
BOOL Add(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress = NULL);
BOOL ShiftAndSubtract(CMemoryBitmap * pTarget, CMemoryBitmap * pSource, CDSSProgress * pProgress = NULL, double fXShift = 0, double fYShift = 0);
BOOL Multiply(CMemoryBitmap * pTarget, double fRedFactor, double fGreenFactor, double fBlueFactor, CDSSProgress * pProgress = NULL);

/* ------------------------------------------------------------------- */

template <typename TType> class CGrayBitmapT;

template <typename TType> 
class CGrayMedianFilterEngineT : public CMedianFilterEngine
{
private :
	CGrayBitmapT<TType> *		m_pInBitmap;

public :
	CGrayMedianFilterEngineT() {};
	virtual ~CGrayMedianFilterEngineT() {};

	void	SetInputBitmap(CGrayBitmapT<TType> * pInBitmap)
	{
		m_pInBitmap = pInBitmap;
	};

	virtual BOOL	GetFilteredImage(CMemoryBitmap ** ppBitmap, LONG lFilterSize, CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

inline BAYERCOLOR GetBayerColor(LONG x, LONG y, CFATYPE CFAType)
{
	switch (CFAType)
	{
		case CFATYPE_NONE :
			return BAYER_UNKNOWN;
			break;
		case CFATYPE_BGGR :
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
		case CFATYPE_GRBG :
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
		case CFATYPE_GBRG :
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
		case CFATYPE_RGGB :
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

inline BOOL	IsBayerBlueLine(LONG y, CFATYPE CFAType)
{
	if ((CFAType == CFATYPE_GRBG) || (CFAType == CFATYPE_RGGB))
		return (y & 1) ? TRUE : FALSE;
	else
		return (y & 1) ? FALSE : TRUE;
};

inline BOOL IsBayerBlueColumn(LONG x, CFATYPE CFAType)
{
	if ((CFAType == CFATYPE_GBRG) || (CFAType == CFATYPE_RGGB))
		return (x & 1) ? TRUE : FALSE;
	else
		return (x & 1) ? FALSE : TRUE;
};

inline BOOL IsBayerRedLine(LONG y, CFATYPE CFAType)
{
	return !IsBayerBlueLine(y, CFAType);
};

inline BOOL IsBayerRedColumn(LONG x, CFATYPE CFAType)
{
	return !IsBayerBlueColumn(x, CFAType);
};

void	CYMGToRGB(double fCyan, double fYellow, double fMagenta, double fGreen2, double & fRed, double & fGreen, double & fBlue);


inline void	CYMGToRGB2(double fCyan, double fYellow, double fMagenta, double fGreen2, double & fRed, double & fGreen, double & fBlue)
{
	double			fR, fG, fB;

	fR   = (fMagenta+fYellow-fCyan)/2.0;
	fG = (fYellow+fCyan-fMagenta)/2.0;
	fB  = (fMagenta+fCyan-fYellow)/2.0;

	fRed   = 2.088034662 * fR + -3.663103328 * fG + 3.069027325 * fB;
	fGreen = -0.28607719 * fR +	1.706598409	* fG + 0.24881043 * fB;
	fBlue  = -0.180853396 * fR + -7.714219397 * fG + 9.438903145 * fB;

	fRed = max(0, min (255.0, fRed));
	fGreen = max(0, min (255.0, fRed));
	fBlue = max(0, min (255.0, fRed));
};

inline void	CYMGToRGB3(double fCyan, double fYellow, double fMagenta, double fGreen2, double & fRed, double & fGreen, double & fBlue)
{
	fRed = fGreen = fBlue = 0;
	// Basic formulae
	// M = R + B
	// Y = R + G
	// C = B + G


	// RGB from CYM
	// R = (M+Y-C)/2
	// G = (Y+C-M)/2
	// B = (M+C-Y)/2
	fRed   = max(0, fMagenta+fYellow-fCyan)/2.0;
	fGreen = max(0, fYellow+fCyan-fMagenta)/2.0;
	fBlue  = max(0 ,fMagenta+fCyan-fYellow)/2.0;

/*	if (fGreen2)
	{
		fRed  *= fGreen2/fGreen;
		fBlue *= fGreen2/fGreen;
		fGreen = fGreen2;
	};
	fRed = min(fRed, 255);
	fBlue = min(fBlue, 255);
	fGreen = min(fGreen, 255);*/

	// RGB from CYG
	// G = G
	// R = Y - G
	// B = C - G
	fGreen += fGreen2;
	fRed   += max(0, fYellow-fGreen2);
	fBlue  += max(0, fCyan-fGreen2);

	// RGB from CMG
	// G = G
	// B = C - G
	// R = M - B = M - C + G
//	fGreen += fGreen2;
	fBlue  += max(0, fCyan-fGreen2);
	fRed   += max(0, fMagenta - fCyan + fGreen2);

	// RGB from YMG
	// G = G
	// R = Y - G
	// B = M - R = M - Y + G
//	fGreen += fGreen2;
	fRed   += max(0, fYellow - fGreen2);
	fBlue  += max(0, fMagenta - fYellow + fGreen2);

	// Average the results
	fRed /= 4.0;
	fBlue /= 4.0;
	fGreen /= 2.0;

	fRed   = max(0.0, fRed);
	fBlue  = max(0.0, fBlue);
	fGreen = max(0.0, fGreen);
};

typedef enum CFATRANSFORMATION
{
	CFAT_NONE			= 0,
	CFAT_SUPERPIXEL		= 1,
	CFAT_RAWBAYER		= 2,
	CFAT_BILINEAR		= 3,
	CFAT_GRADIENT		= 4,
	CFAT_AHD			= 5
}CFATRANSFORMATION;

/* ------------------------------------------------------------------- */

class CCFABitmapInfo
{
protected :
	CFATRANSFORMATION	m_CFATransform;
	CFATYPE				m_CFAType;
	BOOL				m_bCYMG;

protected :
	virtual void SetCFA(BOOL bCFA) = 0;

public :
	void	InitFrom(CCFABitmapInfo * pCFABitmapInfo)
	{
		m_CFATransform = pCFABitmapInfo->m_CFATransform;
		m_CFAType	   = pCFABitmapInfo->m_CFAType;
		m_bCYMG		   = pCFABitmapInfo->m_bCYMG;
	};

public :
	CCFABitmapInfo()
	{
		m_CFATransform = CFAT_NONE;
		m_CFAType	   = CFATYPE_NONE;
		m_bCYMG		   = FALSE;
	};

	void	SetCFAType(CFATYPE Type)
	{
		m_CFAType = Type;
		m_bCYMG = IsCYMGType(m_CFAType);
	};

	CFATYPE	GetCFAType()
	{
		return m_CFAType;
	};

	void	UseSuperPixels(BOOL bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_SUPERPIXEL;
			SetCFA(TRUE);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseRawBayer(BOOL bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_RAWBAYER;
			SetCFA(TRUE);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseBilinear(BOOL bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_BILINEAR;
			SetCFA(TRUE);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseAHD(BOOL bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_AHD;
			SetCFA(TRUE);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	CFATRANSFORMATION GetCFATransformation()
	{
		return m_CFATransform;
	};
};

/* ------------------------------------------------------------------- */

template <typename TType, typename TTypeOutput = TType>
class CGrayMultiBitmapT : public CMultiBitmap
{
protected :
	virtual BOOL	CreateNewMemoryBitmap(CMemoryBitmap ** ppBitmap)
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		if (m_pBitmapModel)
		{
			CBitmapCharacteristics		bc;

			m_pBitmapModel->GetCharacteristics(bc);
			if (bc.m_lNrChannels == 1)
				pBitmap.Attach(m_pBitmapModel->Clone(TRUE));
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

	virtual BOOL	CreateOutputMemoryBitmap(CMemoryBitmap ** ppBitmap)
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		pBitmap.Attach(new CGrayBitmapT<TTypeOutput>);

		if (pBitmap && m_pBitmapModel)
		{
			CCFABitmapInfo *		pSrc;
			CCFABitmapInfo *		pDst;

			pBitmap->SetCFA(m_pBitmapModel->IsCFA());
			pSrc = dynamic_cast<CCFABitmapInfo *>(m_pBitmapModel.m_p);
			pDst = dynamic_cast<CCFABitmapInfo *>(pBitmap.m_p);

			if (pSrc && pDst)
				pDst->InitFrom(pSrc);
		};

		return pBitmap.CopyTo(ppBitmap);
	};

	virtual BOOL	SetScanLines(CMemoryBitmap * pBitmap, LONG lLine, const std::vector<void*> & vScanLines)
	{
		BOOL						bResult = FALSE;
		// Each scan line consist of lWidth TType values
		LONG						lWidth;
		TTypeOutput *				pOutputScanLine;
		TTypeOutput *				pCurrentValue;
		std::vector<TType>			vValues;
		std::vector<TType>			vAuxValues;
		double						fMaximum = pBitmap->GetMaximumValue();

		lWidth = pBitmap->RealWidth();
		pOutputScanLine = (TTypeOutput *)malloc(lWidth * sizeof(TTypeOutput));
		pCurrentValue = pOutputScanLine;

		vValues.reserve(vScanLines.size());
		vAuxValues.reserve(vScanLines.size());

		for (LONG i = 0;i<lWidth && pOutputScanLine;i++)
		{
			TType *					pValue;
			double					fWeight = 1.0;

			vValues.resize(0);
			for (LONG j = 0;j<vScanLines.size();j++)
			{
				pValue = (TType *)vScanLines[j];
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
					double			fAverage, fSigma;

					fSigma = Sigma2(vValues, fAverage);
					m_pHomBitmap->SetPixel(i, lLine, fSigma/max(1.0, fAverage)*256.0);
				};

				if (m_vImageOrder.size())
				{
					// Change the order to respect the order of the images
					vAuxValues = vValues;
					vValues.resize(0);
					for (LONG k = 0;k<m_vImageOrder.size();k++)
						if (vAuxValues[m_vImageOrder[k]])
							vValues.push_back(vAuxValues[m_vImageOrder[k]]);

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
				*pCurrentValue = KappaSigmaClip(vValues, m_fKappa, m_lNrIterations);
			else if (m_Method == MBP_MEDIANSIGMACLIP)
				*pCurrentValue = MedianKappaSigmaClip(vValues, m_fKappa, m_lNrIterations);
			else if (m_Method == MBP_AUTOADAPTIVE)
				*pCurrentValue = AutoAdaptiveWeightedAverage(vValues, m_lNrIterations);

			//if (m_bHomogenization)
			//	*pCurrentValue = fHomogenization*(double)(*pCurrentValue);
			pCurrentValue++;
		};

		if (pOutputScanLine)
		{
			pBitmap->SetScanLine(lLine, pOutputScanLine);
			free(pOutputScanLine);
			bResult = TRUE;
		};

		return bResult;
	};

public :
	CGrayMultiBitmapT()
	{
	};

	virtual ~CGrayMultiBitmapT()
	{
	};

	virtual LONG	GetNrChannels()
	{
		return 1;
	};

	virtual LONG	GetNrBytesPerChannel()
	{
		return sizeof(TType);
	};
};

/* ------------------------------------------------------------------- */

template <typename TType> class CColorBitmapT;

template <typename TType>
class CGrayBitmapT : public CMemoryBitmap,
					 public CCFABitmapInfo
{
	friend CColorBitmapT<TType>;

public :
	template <typename TType> class CGrayPixelIterator : public CPixelIterator
	{
	private :
		CSmartPtr<CGrayBitmapT<TType> >		m_pBitmap;
		LONG								m_lX,
											m_lY;
		TType *								m_pValue;
		double								m_fMultiplier;
		LONG								m_lWidth,
											m_lHeight;

	public :
		CGrayPixelIterator()
		{
			m_pValue = 0;
			m_fMultiplier = 1.0;
		};

		~CGrayPixelIterator() {};

		virtual void	Reset(LONG x, LONG y)
		{
			m_lX = x;
			m_lY = y;
			
			size_t			lOffset = m_pBitmap->GetOffset(x, y);
			m_pValue		= &(m_pBitmap->m_vPixels[lOffset]);
		};

		void	Init(LONG x, LONG y, CGrayBitmapT<TType> * pBitmap)
		{
			m_pBitmap		= pBitmap;
			m_fMultiplier	= pBitmap->m_fMultiplier;
			m_lWidth		= pBitmap->m_lWidth;
			m_lHeight		= pBitmap->m_lHeight;
			Reset(x, y);
		};

		virtual void	GetPixel(double & fRed, double & fGreen, double & fBlue)
		{
			if (m_pValue)
				fRed = fGreen = fBlue = (double)(*m_pValue)/m_fMultiplier;
			else
				fRed = fGreen = fBlue = 0.0;
		};

		virtual void	GetPixel(double & fGray)
		{
			if (m_pValue)
				fGray = (double)(*m_pValue)/m_fMultiplier;
			else
				fGray = 0.0;
		};

		virtual void	SetPixel(double fRed, double fGreen, double fBlue)
		{
			
		};

		virtual void	SetPixel(double fGray)
		{
			if (m_pValue)
				*m_pValue = fGray * m_fMultiplier;
		};

		virtual void operator ++(int)
		{
			if (m_pValue)
			{
				if (m_lX < m_lWidth-1)
					m_lX++;
				else if (m_lY < m_lHeight-1)
				{
					m_lY++;
					m_lX = 0;
				}
				else
				{
					// End of iteration
					m_lX = -1;
					m_lY = -1;
					m_pValue = NULL;
				};
				if (m_pValue)
					m_pValue++;
			};
		};
	};
	friend CGrayPixelIterator<TType>;

	template <typename TType> class CHotPixelTask : public CMultitask
	{
	private :
		CGrayBitmapT<TType> *	m_pBitmap;
		CDSSProgress *			m_pProgress;

	public :
		std::vector<size_t>		m_vHotOffsets;

	public :
		CHotPixelTask()
		{
		};

		virtual ~CHotPixelTask()
		{
		};

		void	Init(CGrayBitmapT<TType> * pBitmap, CDSSProgress * pProgress)
		{
			m_pBitmap	= pBitmap;
			m_pProgress = pProgress;
		};

		virtual BOOL	DoTask(HANDLE hEvent)
		{
			BOOL				bResult = TRUE;

			LONG					i, j;
			BOOL					bEnd = FALSE;
			MSG						msg;
			LONG					lWidth = m_pBitmap->Width();
			std::vector<size_t>		vHotOffsets;

			// Create a message queue and signal the event
			PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
			SetEvent(hEvent);
			while (!bEnd && GetMessage(&msg, NULL, 0, 0))
			{
				if (msg.message == WM_MT_PROCESS)
				{
					for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
					{
						for (i = 2;i<lWidth-2;i++)
						{
							size_t				lOffset = m_pBitmap->GetOffset(i, j);
							size_t				vOffsets[4];

							vOffsets[0] = m_pBitmap->GetOffset(i-1, j);
							vOffsets[1] = m_pBitmap->GetOffset(i+1, j);
							vOffsets[2] = m_pBitmap->GetOffset(i, j+1);
							vOffsets[3] = m_pBitmap->GetOffset(i, j-1);

							TType				fValue = m_pBitmap->m_vPixels[lOffset];
							BOOL				bHot = TRUE;

							for (LONG k = 0;k<4 && bHot;k++)
							{
								if (fValue <= 4.0 * m_pBitmap->m_vPixels[vOffsets[k]])
									bHot = FALSE;
							};

							if (bHot)
							{
								vHotOffsets.push_back(lOffset);
								i++; // The next one cannot be a hot pixel
							};
						};
					};

					SetEvent(hEvent);
				}
				else if (msg.message == WM_MT_STOP)
					bEnd = TRUE;
			};

			// Add the vHotOffsets vector to the main one
			m_CriticalSection.Lock();
			for (i = 0;i<vHotOffsets.size();i++)
				m_vHotOffsets.push_back(vHotOffsets[i]);
			m_CriticalSection.Unlock();
			return TRUE;
		};

		virtual BOOL	Process()
		{
			BOOL				bResult = TRUE;
			LONG				lHeight = m_pBitmap->Height()-4;
			LONG				i = 2;
			LONG				lStep;
			LONG				lRemaining;

			if (m_pProgress)
				m_pProgress->SetNrUsedProcessors(GetNrThreads());
			lStep		= max(1, lHeight/50);
			lRemaining	= lHeight;
			bResult = TRUE;
			while (i<lHeight)
			{
				LONG			lAdd = min(lStep, lRemaining);
				DWORD			dwThreadId;
				
				dwThreadId = GetAvailableThreadId();
				PostThreadMessage(dwThreadId, WM_MT_PROCESS, i, lAdd);

				i			+=lAdd;
				lRemaining	-= lAdd;
				if (m_pProgress)
					m_pProgress->Progress2(NULL, i);
			};

			CloseAllThreads();

			if (m_pProgress)
				m_pProgress->SetNrUsedProcessors();

			return bResult;
		};
	};

	friend CHotPixelTask<TType>;

	friend CGrayMedianFilterEngineT<TType>;

public :
	std::vector<TType>	m_vPixels;

private :
	LONG				m_lWidth;
	LONG				m_lHeight;
	bool				m_bWord;
	bool				m_bDouble;
	bool				m_bDWord;
	bool				m_bFloat;
	double				m_fMultiplier;

private :
	BOOL	InitInternals()
	{
		m_vPixels.clear();
		m_vPixels.resize(m_lWidth * m_lHeight);

		return ((size_t)m_vPixels.size() == (size_t)m_lWidth * (size_t)m_lHeight);
	};

	BOOL	IsXYOk(LONG x, LONG y)
	{
		return (x>=0 && x<m_lWidth && y>=0 && y<m_lHeight);
	};

	size_t	GetOffset(LONG x, LONG y)
	{
		if (IsXYOk(x, y))
			return (size_t)m_lWidth * (size_t)y + (size_t)x;
		else
			return 0;
	};

	virtual BOOL	IsMonochrome()
	{
		return TRUE;
	};

	TType	GetPrimary(LONG x, LONG y, COLORREF16 crColor)
	{
		switch (::GetBayerColor(x, y, m_CFAType))
		{
		case BAYER_RED :
			return crColor.red;
			break;
		case BAYER_GREEN :
			return crColor.green;
			break;
		case BAYER_BLUE :
			return crColor.blue;
			break;
		};

		return 0;
	};

	double	GetPrimary(LONG x, LONG y, double fRed, double fGreen, double fBlue)
	{
		switch (::GetBayerColor(x, y, m_CFAType))
		{
		case BAYER_RED :
			return fRed;
			break;
		case BAYER_GREEN :
			return fGreen;
			break;
		case BAYER_BLUE :
			return fBlue;
			break;
		};

		return 0;
	};

	double	InterpolateGreen(LONG x, LONG y, TType * pValue = NULL)
	{
		double		fResult = 0.0;
		LONG		lNrValues = 0;

		if (!pValue)
			pValue = &m_vPixels[GetOffset(x, y)];

		if (x > 0)
		{
			fResult += *(pValue-1);
			lNrValues++;
		};
		if (x < m_lWidth -1)
		{
			fResult += *(pValue+1);
			lNrValues++;
		};
		if (y > 0)
		{
			fResult += *(pValue-m_lWidth);
			lNrValues++;
		};
		if (y < m_lHeight -1)
		{
			fResult += *(pValue+m_lWidth);
			lNrValues++;
		};

		return fResult/lNrValues;
	};

	double	InterpolateBlue(LONG x, LONG y, TType * pValue = NULL)
	{
		double		fResult = 0.0;
		LONG		lNrValues = 0;
		if (!pValue)
			pValue = &m_vPixels[GetOffset(x, y)];

		if (IsBayerBlueLine(y, m_CFAType))
		{
			// Pixel between 2 blue pixel (horizontaly)
			if (x > 0)
			{
				fResult += *(pValue-1);
				lNrValues++;
			};
			if (x < m_lWidth - 1)
			{
				fResult += *(pValue+1);
				lNrValues++;
			};
		}
		else if (IsBayerBlueColumn(x, m_CFAType))
		{
			// Pixel between 2 blue pixels (verticaly)
			if (y > 0)
			{
				fResult += *(pValue-m_lWidth);
				lNrValues++;
			};
			if (y < m_lHeight - 1)
			{
				fResult += *(pValue+m_lWidth);
				lNrValues++;
			};
		}
		else
		{
			// Use 4 pixels to interpolate
			if (x > 0 && y > 0)
			{
				fResult += *(pValue-1-m_lWidth);
				lNrValues++;
			}
			if ((x > 0) && (y < m_lHeight -1))
			{
				fResult += *(pValue-1+m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth-1) && (y < m_lHeight -1))
			{
				fResult += *(pValue+1+m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth-1) && (y > 0))
			{
				fResult += *(pValue+1-m_lWidth);
				lNrValues++;
			};
		};

		return fResult/lNrValues;
	};

	double	InterpolateRed(LONG x, LONG y, TType * pValue = NULL)
	{
		double		fResult = 0.0;
		LONG		lNrValues = 0;
		if (!pValue)
			pValue = &m_vPixels[GetOffset(x, y)];

		if (IsBayerRedLine(y, m_CFAType))
		{
			// Pixel between 2 blue pixel (horizontaly)
			if (x > 0)
			{
				fResult += *(pValue-1);
				lNrValues++;
			};
			if (x < m_lWidth - 1)
			{
				fResult += *(pValue+1);
				lNrValues++;
			};
		}
		else if (IsBayerRedColumn(x, m_CFAType))
		{
			// Pixel between 2 blue pixels (verticaly)
			if (y > 0)
			{
				fResult += *(pValue-m_lWidth);
				lNrValues++;
			};
			if (y < m_lHeight - 1)
			{
				fResult += *(pValue+m_lWidth);
				lNrValues++;
			};
		}
		else
		{
			// Use 4 pixels to interpolate
			if (x > 0 && y > 0)
			{
				fResult += *(pValue-1-m_lWidth);
				lNrValues++;
			}
			if ((x > 0) && (y < m_lHeight -1))
			{
				fResult += *(pValue-1+m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth-1) && (y < m_lHeight -1))
			{
				fResult += *(pValue+1+m_lWidth);
				lNrValues++;
			};
			if ((x < m_lWidth-1) && (y > 0))
			{
				fResult += *(pValue+1-m_lWidth);
				lNrValues++;
			};
		};

		return fResult/lNrValues;
	};

	void	InterpolateAll(double * pfValues, LONG x, LONG y)
	{
		LONG			lIndice;
		LONG			lNrValues[4];
		
		lNrValues[0] = lNrValues[1] = lNrValues[2] = lNrValues[3] = 0;
		pfValues[0]  = pfValues[1]  = pfValues[2]  = pfValues[3]  = 0;
		
		for (LONG i = max(0, x-1);i<=min(m_lWidth-1, x+1);i++)
			for (LONG j = max(0, y-1);j<=min(m_lHeight-1, y+1);j++)
			{
				lIndice = CMYGZeroIndex(::GetBayerColor(i, j, m_CFAType));
				pfValues[lIndice]  += m_vPixels[GetOffset(i, j)];
				lNrValues[lIndice] ++;
			};

		pfValues[0] /= max(1, lNrValues[0]);
		pfValues[1] /= max(1, lNrValues[1]);
		pfValues[2] /= max(1, lNrValues[2]);
		pfValues[3] /= max(1, lNrValues[3]);

/*
		// It's used only for CYMG - so cut it down to the basic

		

		if (x==m_lWidth-1)
			x = m_lWidth-2;
		if (y==m_lHeight-1)
			y = m_lHeight-2;

		pfValues[0]  = pfValues[1]  = pfValues[2]  = pfValues[3]  = 0;
		
		for (LONG i = x;i<=x+1;i++)
			for (LONG j = y;j<=y+1;j++)
				pfValues[CMYGZeroIndex(::GetBayerColor(i, j, m_CFAType))]  = m_vPixels[GetOffset(i, j)];*/
	};

protected :
	virtual void SetCFA(BOOL bCFA)
	{
		m_bCFA = bCFA;
	};

public :
	CGrayBitmapT()
	{
		m_lWidth	= 0;
		m_lHeight	= 0;
		m_bWord    = (typeid(TType) == typeid(WORD));
		m_bDouble  = (typeid(TType) == typeid(double));
		m_bDWord   = (typeid(TType) == typeid(DWORD));
		m_bFloat   = (typeid(TType) == typeid(float));

		m_fMultiplier = 1.0;
		if (m_bWord || m_bDouble || m_bFloat)
			m_fMultiplier = 256.0;
		else if (m_bDWord)
			m_fMultiplier = 256.0 * 65536.0;
	};

	virtual ~CGrayBitmapT() {};

	void	SetMultiplier(double fMultiplier)
	{
		m_fMultiplier = fMultiplier;
	};

	virtual BOOL	Init(LONG lWidth, LONG lHeight)
	{
		m_lWidth	= lWidth;
		m_lHeight	= lHeight;
		return InitInternals();
	};

	virtual CMemoryBitmap *	Clone(BOOL bEmpty = FALSE)
	{
		CGrayBitmapT<TType> *	pResult;

		pResult = new CGrayBitmapT<TType>;
		if (pResult)
		{
			if (!bEmpty)
			{
				pResult->m_vPixels	= m_vPixels;
				pResult->m_lWidth	= m_lWidth;
				pResult->m_lHeight	= m_lHeight;
			};
			pResult->m_bWord	= m_bWord;
			pResult->m_bDouble	= m_bDouble;
			pResult->m_bDWord	= m_bDWord;
			pResult->m_bFloat	= m_bFloat;
			pResult->m_CFATransform = m_CFATransform;
			pResult->m_CFAType  = m_CFAType;
			pResult->m_bCYMG	= m_bCYMG;

			pResult->CopyFrom(*this);
		};

		return pResult;
	};


	virtual BAYERCOLOR GetBayerColor(LONG x, LONG y)
	{
		return ::GetBayerColor(x, y, m_CFAType);
	};

	virtual LONG	BitPerSample()
	{
		return sizeof(TType)*8;
	};

	virtual LONG	IsFloat()
	{
		return m_bFloat;
	};

	virtual LONG	Width()
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
			return m_lWidth/2;
		else
			return m_lWidth;
	};

	virtual LONG	Height()
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
			return m_lHeight/2;
		else
			return m_lHeight;
	};

	virtual LONG	RealHeight()
	{
		return m_lHeight;
	};

	virtual LONG	RealWidth()
	{
		return m_lWidth;
	};

	virtual BOOL	SetValue(LONG i, LONG j, double fGray)
	{ 
		BOOL		bResult = TRUE;

		if (IsXYOk(i, j))
			m_vPixels[GetOffset(i, j)] = fGray;
		else
			bResult = FALSE;

		return bResult; 
	};

	virtual BOOL	GetValue(LONG i, LONG j, double & fGray)
	{ 
		BOOL		bResult = TRUE;

		if (IsXYOk(i, j))
			fGray = m_vPixels[GetOffset(i, j)];
		else
			bResult = FALSE;

		return bResult; 
	};

	virtual BOOL	SetPixel(LONG i, LONG j, double fRed, double fGreen, double fBlue)
	{
		BOOL			bResult = FALSE;

		if (m_CFATransform == CFAT_SUPERPIXEL)
		{
			bResult = SetPixel(i*2, j*2, fRed) &&
					  SetPixel(i*2, j*2+1, fGreen) &&
					  SetPixel(i*2+1, j*2, fGreen) &&
					  SetPixel(i*2+1, j*2+1, fBlue);
		}
		else if (m_CFATransform == CFAT_NONE)
			bResult = SetPixel(i, j, fRed);
		else
			bResult = SetPixel(i, j, GetPrimary(i, j, fRed, fGreen, fBlue));

		return bResult;
	};

	virtual BOOL	SetPixel(LONG i, LONG j, double fGray)
	{
		if (IsXYOk(i, j))
		{
			fGray *= m_fMultiplier;
			m_vPixels[GetOffset(i, j)] = fGray;
			return TRUE;
		}
		else
			return FALSE;
	};

	virtual BOOL	GetPixel(LONG i, LONG j, double & fRed, double & fGreen, double & fBlue)
	{
		BOOL			bResult = FALSE;

		fRed = fGreen = fBlue = 0.0;
		if (m_CFATransform == CFAT_SUPERPIXEL)
		{
			ASSERT(m_bWord);
			if (IsXYOk((i-1)*2, (j-1)*2) && IsXYOk((i+1)*2+1, (j+1)*2+1))
			{
				TType *			pValue = &(m_vPixels[GetOffset(i*2, j*2)]);

				bResult = TRUE; 
				switch (m_CFAType)
				{
				case CFATYPE_GRBG :
					fRed	= (*(pValue+1))/m_fMultiplier;
					fGreen	= ((*pValue)+(*(pValue+1+m_lWidth)))/2.0/m_fMultiplier;
					fBlue	= (*(pValue+m_lWidth))/m_fMultiplier;
					break;
				case CFATYPE_GBRG :
					fRed	= (*(pValue+m_lWidth))/m_fMultiplier;
					fGreen	= ((*pValue)+(*(pValue+1+m_lWidth)))/2.0/m_fMultiplier;
					fBlue	= (*(pValue+1))/m_fMultiplier;
					break;
				case CFATYPE_BGGR :
					fRed	= (*(pValue+1+m_lWidth))/m_fMultiplier;
					fGreen	= ((*(pValue+m_lWidth))+(*(pValue+1)))/2.0/m_fMultiplier;
					fBlue	= (*pValue)/m_fMultiplier;
					break;
				case CFATYPE_RGGB :
					fRed	= (*pValue)/m_fMultiplier;
					fGreen	= ((*(pValue+m_lWidth))+(*(pValue+1)))/2.0/m_fMultiplier;
					fBlue	= (*(pValue+1+m_lWidth))/m_fMultiplier;
					break;
				}
			};
		}
		else if (m_CFATransform == CFAT_RAWBAYER)
		{
			ASSERT(m_bWord);
			if (IsXYOk(i, j))
			{
				bResult = TRUE; 
				TType *		pValue = &(m_vPixels[GetOffset(i, j)]);

				switch (::GetBayerColor(i, j, m_CFAType))
				{
				case BAYER_RED :
					fRed	= (*pValue)/m_fMultiplier;
					break;
				case BAYER_GREEN :
					fGreen	= (*pValue)/m_fMultiplier;
					break;
				case BAYER_BLUE :
					fBlue	= (*pValue)/m_fMultiplier;
					break;
				};
			};
		}
		else if ((m_CFATransform == CFAT_BILINEAR) || (m_CFATransform == CFAT_AHD))
		{
			ASSERT(m_bWord);
			if (IsXYOk(i, j))
			{
				bResult = TRUE;

				if (m_bCYMG)
				{
					double			fValue[4]; // Myself

					InterpolateAll(fValue , i, j);

					CYMGToRGB(fValue[CMYGZeroIndex(BAYER_CYAN)]/m_fMultiplier, 
							  fValue[CMYGZeroIndex(BAYER_YELLOW)]/m_fMultiplier, 
							  fValue[CMYGZeroIndex(BAYER_MAGENTA)]/m_fMultiplier, 
							  fValue[CMYGZeroIndex(BAYER_GREEN2)]/m_fMultiplier, 
							  fRed, fGreen, fBlue);
				}
				else
				{
					TType *			pValue = &(m_vPixels[GetOffset(i, j)]);
					switch (::GetBayerColor(i, j, m_CFAType))
					{
					case BAYER_RED :
						fRed	= (*pValue)/m_fMultiplier;
						fGreen	= InterpolateGreen(i, j, pValue)/m_fMultiplier;
						fBlue	= InterpolateBlue(i, j, pValue)/m_fMultiplier;
						break;
					case BAYER_GREEN :
						fRed	= InterpolateRed(i, j, pValue)/m_fMultiplier;
						fGreen	= (*pValue)/m_fMultiplier;
						fBlue	= InterpolateBlue(i, j, pValue)/m_fMultiplier;
						break;
					case BAYER_BLUE :
						fRed	= InterpolateRed(i, j, pValue)/m_fMultiplier;
						fGreen	= InterpolateGreen(i, j, pValue)/m_fMultiplier;
						fBlue	= (*pValue)/m_fMultiplier;
						break;
					};
				};
			};
		}
		else if (m_CFATransform == CFAT_GRADIENT)
		{
		}
		else if (IsXYOk(i, j))
		{
			bResult = TRUE; 
			fRed = fBlue = fGreen = m_vPixels[GetOffset(i, j)]/m_fMultiplier;
		};

		return bResult;	
	};

	virtual BOOL	GetPixel(LONG i, LONG j, double & fGray)
	{
		BOOL			bResult = FALSE;
		fGray = 0.0;

		if (IsXYOk(i, j))
		{
			bResult = TRUE;
			fGray = m_vPixels[GetOffset(i, j)]/m_fMultiplier;
		}

		return bResult;
	};

	virtual BOOL	GetScanLine(LONG j, void * pScanLine)
	{
		BOOL			bResult = FALSE;

		if (j<m_lHeight)
		{
			memcpy(pScanLine, &(m_vPixels[j*m_lWidth]), sizeof(TType)*m_lWidth);
			bResult = TRUE;
		};

		return bResult;
	};

	virtual BOOL	SetScanLine(LONG j, void * pScanLine)
	{
		BOOL			bResult = FALSE;

		if (j<m_lHeight)
		{
			memcpy(&(m_vPixels[j*m_lWidth]), pScanLine, sizeof(TType)*m_lWidth);
			bResult = TRUE;
		};

		return bResult;
	};

	virtual CMultiBitmap * CreateEmptyMultiBitmap()
	{
		CMultiBitmap *		pResult;

		pResult = new CGrayMultiBitmapT<TType>();
		if (pResult)
			pResult->SetBitmapModel(this);

		return pResult;
	};

	virtual void RemoveHotPixels(CDSSProgress * pProgress = NULL) 
	{
		/*LONG					i, j;
		std::vector<LONG>		vHotOffsets;*/

		if (pProgress)
		{
			CString			strText;

			strText.Format(IDS_REMOVINGHOTPIXELS);
			pProgress->Start2(strText, m_lHeight);
		};

		CHotPixelTask<TType>	HotPixelTask;

		HotPixelTask.Init(this, pProgress);
		HotPixelTask.StartThreads();
		HotPixelTask.Process();

		if (pProgress)
			pProgress->End2();

		/*
		for (i = 2;i<m_lWidth-2;i++)
		{
			for (j = 2;j<m_lHeight-2;j++)
			{
				LONG				lOffset = GetOffset(i, j);
				std::vector<LONG>	vOffsets;

				vOffsets.push_back(GetOffset(i-1, j));
				vOffsets.push_back(GetOffset(i+1, j));
				vOffsets.push_back(GetOffset(i, j+1));
				vOffsets.push_back(GetOffset(i, j-1));

				TType				fValue = m_vPixels[lOffset];
				BOOL				bHot = TRUE;

				for (LONG k = 0;k<vOffsets.size() && bHot;k++)
				{
					if (fValue <= 4.0 * m_vPixels[vOffsets[k]])
						bHot = FALSE;
				};

				if (bHot)
					vHotOffsets.push_back(lOffset);
			};

			if (pProgress)
				pProgress->Progress2(NULL, i+1);
		};*/

		for (LONG i = 0;i<HotPixelTask.m_vHotOffsets.size();i++)
		{
			m_vPixels[HotPixelTask.m_vHotOffsets[i]] = 0;
		};
	};

	virtual void GetIterator(CPixelIterator ** ppIterator, LONG x = 0, LONG y = 0)
	{
		CSmartPtr<CPixelIterator>	pResult;
		CGrayPixelIterator<TType> *	pIterator = new CGrayPixelIterator<TType>;


		pResult.Attach(pIterator);
		pIterator->Init(x, y, this);

		pResult.CopyTo(ppIterator);
	};
	TType *	GetGrayPixel(LONG i, LONG j)
	{
		return &(m_vPixels[GetOffset(i, j)]);
	};

	double	GetMultiplier()
	{
		return m_fMultiplier;
	};

	virtual void GetMedianFilterEngine(CMedianFilterEngine ** ppMedianFilterEngine)
	{
		CSmartPtr<CGrayMedianFilterEngineT<TType> >		pMedianFilterEngine;
		CSmartPtr<CMedianFilterEngine>					pMedianFilterEngine2;

		pMedianFilterEngine.Attach(new CGrayMedianFilterEngineT<TType>);

		pMedianFilterEngine->SetInputBitmap(this);

		pMedianFilterEngine2 = pMedianFilterEngine;
		pMedianFilterEngine2.CopyTo(ppMedianFilterEngine);
	};

	virtual double GetMaximumValue()
	{
		return m_fMultiplier*256.0;
	};

	virtual void	GetCharacteristics(CBitmapCharacteristics & bc)
	{
		bc.m_bFloat		= m_bFloat;
		bc.m_dwHeight	= m_lHeight;
		bc.m_dwWidth	= m_lWidth;
		bc.m_lNrChannels = 1;
		bc.m_lBitsPerPixel = BitPerSample();
	};
};

/* ------------------------------------------------------------------- */

typedef CGrayBitmapT<double>			CGrayBitmap;
typedef CGrayBitmapT<BYTE>				C8BitGrayBitmap;
typedef CGrayBitmapT<WORD>				C16BitGrayBitmap;
typedef CGrayBitmapT<DWORD>				C32BitGrayBitmap;
typedef CGrayBitmapT<float>				C32BitFloatGrayBitmap;

/* ------------------------------------------------------------------- */

#if DSSFILEDECODING==1
class CWindowsBitmap : public CRefCount
{
private :
	HBITMAP				m_hBitmap;

public :
	CWindowsBitmap() 
	{
		m_hBitmap = NULL;
	};

	~CWindowsBitmap()
	{
		if (m_hBitmap)
			::DeleteObject(m_hBitmap);
	};

	HBITMAP		GetHBitmap()
	{
		return m_hBitmap;
	};

	void		SetHBitmap(HBITMAP hBitmap)
	{
		if (m_hBitmap)
			::DeleteObject(m_hBitmap);
		m_hBitmap = hBitmap;
	};
};

/* ------------------------------------------------------------------- */

class C32BitsBitmap : public CRefCount
{
private :
	HBITMAP				m_hBitmap;
	VOID *				m_lpBits;
	LONG				m_lWidth,
						m_lHeight;
	LPBYTE*				m_pLine;
	DWORD				m_dwByteWidth;

private :
	void	InitInternals()
	{
		if (m_pLine)
			free(m_pLine);

		m_pLine = (LPBYTE *)malloc(m_lHeight * sizeof(LPBYTE));

		m_dwByteWidth   = (((m_lWidth * 32 + 31) & ~31) >> 3);
		LONG			y = m_lHeight - 1;

		for( LONG i = 0; y >= 0; y--, i++ )
		{
			m_pLine[i] = (LPBYTE)m_lpBits + y * m_dwByteWidth;
		}
	};


public :
	C32BitsBitmap() 
	{
		m_hBitmap	= NULL;
		m_lpBits	= NULL;
		m_lWidth	= 0;
		m_lHeight	= 0;
		m_pLine		= NULL;
	};


	virtual ~C32BitsBitmap()
	{
		Free();
	};

	VOID *	GetBits()
	{
		return m_lpBits;
	};

	virtual LONG	Width()
	{
		return m_lWidth;
	};

	virtual LONG	Height()
	{
		return m_lHeight;
	};

	LONG	ByteWidth()
	{
		return m_dwByteWidth;
	};

	virtual void	Init(LONG lWidth, LONG lHeight)
	{
		Create(lWidth, lHeight);
	};

	HBITMAP	Create(LONG lWidth, LONG lHeight)
	{
		Free();

        HBITMAP			hBitmap;
        BITMAPINFO		bmpInfo;
        VOID *			pBits;

        memset(&bmpInfo, 0, sizeof(bmpInfo)); 
        bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
        bmpInfo.bmiHeader.biWidth = lWidth;
        bmpInfo.bmiHeader.biHeight= lHeight;
        bmpInfo.bmiHeader.biPlanes= 1;;
        bmpInfo.bmiHeader.biBitCount= 32;
        bmpInfo.bmiHeader.biCompression= BI_RGB;
        bmpInfo.bmiHeader.biSizeImage= 0;
        bmpInfo.bmiHeader.biXPelsPerMeter = (LONG)(96*100.0/2.54);
        bmpInfo.bmiHeader.biYPelsPerMeter = (LONG)(96*100.0/2.54);
        bmpInfo.bmiHeader.biClrUsed = 0;
        bmpInfo.bmiHeader.biClrImportant = 0;

		HDC				hDC;
		hDC = GetDC(NULL);
        hBitmap = CreateDIBSection(hDC, &bmpInfo, 0, &pBits, NULL, 0);
		ReleaseDC(NULL, hDC);

		if (hBitmap)
		{
			m_hBitmap	= hBitmap;
			m_lpBits	= pBits;
			m_lWidth	= lWidth;
			m_lHeight	= lHeight;
			InitInternals();
		};

        return hBitmap;
	};

	BOOL	InitFrom(CMemoryBitmap * pBitmap);
	BOOL	CopyToClipboard();

	BOOL	IsEmpty()
	{
		return (m_hBitmap == NULL);
	};

	HBITMAP GetHBITMAP()
	{
		return m_hBitmap;
	};

	void	Free()
	{
		if (m_hBitmap)
		{
			DeleteObject(m_hBitmap);
		};
		m_hBitmap	= NULL;
		m_lpBits	= NULL;
		if (m_pLine)
			free(m_pLine);
		m_pLine = NULL;
	};

	HBITMAP	Detach()
	{
		HBITMAP		hResult = m_hBitmap;

		m_hBitmap = NULL;
		Free();

		return hResult;
	};

	COLORREF	GetPixel(LONG x, LONG y)
	{
		COLORREF			crColor = RGB(0, 0, 0);

		if( (x >= 0) && (x < m_lWidth) && (y >=0) && (y < m_lHeight))
		{
			LPBYTE		pPixel = m_pLine[y] + ((x * 32) >> 3);
			DWORD		dwPixel;

			dwPixel = *(LPDWORD)pPixel;
			RGBQUAD		rgbq = *(LPRGBQUAD)&dwPixel;

			crColor = RGB(rgbq.rgbRed,rgbq.rgbGreen,rgbq.rgbBlue);
		};

		return crColor;
	};

	LPBYTE		GetPixelBase(LONG x, LONG y)
	{
		return m_pLine[y] + x * 4;
	};

	void		SetPixel(LONG x, LONG y, COLORREF crColor)
	{
		if( (x >= 0) && (x < m_lWidth) && (y >=0) && (y < m_lHeight))
		{
			LPBYTE		pPixel = m_pLine[y] + ((x * 32) >> 3);
			RGBQUAD		rgbq;
			
			rgbq.rgbRed			= GetRValue(crColor);
			rgbq.rgbGreen		= GetGValue(crColor);
			rgbq.rgbBlue		= GetBValue(crColor);
			rgbq.rgbReserved	= 0;

			*(LPDWORD)pPixel = *(LPDWORD)(&rgbq);
		};
	};

	virtual BOOL	SetPixel16(LONG i, LONG j, COLORREF16 crColor16)
	{
		COLORREF			crColor;

		crColor = RGB((BYTE)(crColor16.red/256.0), (BYTE)(crColor16.green/256.0),(BYTE)(crColor16.blue/256.0));

		SetPixel(i, j, crColor);

		return TRUE;
	};

};

class	CGammaTransformation
{
public :
	std::vector<BYTE>		m_vTransformation;

public :
	CGammaTransformation() {};
	~CGammaTransformation() {};

	void		InitTransformation(double fBlackPoint, double fGrayPoint, double fWhitePoint);
	void		InitTransformation(double fGamma);

	BOOL		IsInitialized()
	{
		return (m_vTransformation.size() == 65537);
	};
};

BOOL	ApplyGammaTransformation(C32BitsBitmap * pOutBitmap, CMemoryBitmap * pInBitmap, CGammaTransformation & gammatrans);
#endif // DSSFILEDECODING

/* ------------------------------------------------------------------- */

template <typename TType, typename TTypeOutput = TType>
class CColorMultiBitmapT : public CMultiBitmap
{
protected :
	virtual BOOL	CreateNewMemoryBitmap(CMemoryBitmap ** ppBitmap)
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		if (m_pBitmapModel)
		{
			CBitmapCharacteristics		bc;

			m_pBitmapModel->GetCharacteristics(bc);
			if (bc.m_lNrChannels == 3)
				pBitmap.Attach(m_pBitmapModel->Clone(TRUE));
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

	virtual BOOL	CreateOutputMemoryBitmap(CMemoryBitmap ** ppBitmap)
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		pBitmap.Attach(new CColorBitmapT<TTypeOutput>);

		return pBitmap.CopyTo(ppBitmap);
	};

	virtual BOOL	SetScanLines(CMemoryBitmap * pBitmap, LONG lLine, const std::vector<void*> & vScanLines)
	{
		BOOL						bResult = FALSE;
		// Each scan line consist of lWidth TType values
		LONG						lWidth;
		TTypeOutput *				pOutputScanLine;
		TTypeOutput *				pRedCurrentValue;
		TTypeOutput *				pGreenCurrentValue;
		TTypeOutput *				pBlueCurrentValue;
		std::vector<TType>			vRedValues;
		std::vector<TType>			vGreenValues;
		std::vector<TType>			vBlueValues;
		std::vector<TType>			vAuxRedValues;
		std::vector<TType>			vAuxGreenValues;
		std::vector<TType>			vAuxBlueValues;
		double						fMaximum = pBitmap->GetMaximumValue();

		lWidth = pBitmap->RealWidth();
		pOutputScanLine = (TTypeOutput *)malloc(lWidth * 3 * sizeof(TTypeOutput));

		pRedCurrentValue   = pOutputScanLine;
		pGreenCurrentValue = pRedCurrentValue + lWidth;
		pBlueCurrentValue  = pGreenCurrentValue + lWidth;

		vRedValues.reserve(vScanLines.size());
		vGreenValues.reserve(vScanLines.size());
		vBlueValues.reserve(vScanLines.size());
		vAuxRedValues.reserve(vScanLines.size());
		vAuxGreenValues.reserve(vScanLines.size());
		vAuxBlueValues.reserve(vScanLines.size());

		for (LONG i = 0;i<lWidth && pOutputScanLine;i++)
		{
			TType *					pRedValue;
			TType *					pGreenValue;
			TType *					pBlueValue;

			vRedValues.resize(0);
			vGreenValues.resize(0);
			vBlueValues.resize(0);
			for (LONG j = 0;j<vScanLines.size();j++)
			{
				pRedValue = (TType *)vScanLines[j];
				pRedValue += i;
				pGreenValue = pRedValue + lWidth;
				pBlueValue  = pGreenValue + lWidth;

				if (*pRedValue || m_vImageOrder.size())	// Remove 0
					vRedValues.push_back(*pRedValue);
				if (*pGreenValue || m_vImageOrder.size())	// Remove 0
					vGreenValues.push_back(*pGreenValue);
				if (*pBlueValue || m_vImageOrder.size())	// Remove 0
					vBlueValues.push_back(*pBlueValue);
			};

			if (m_bHomogenization)
			{
			//	if ((i==843) && (lLine==934))
			//		DebugBreak();

				if (m_pHomBitmap)
				{
					double			fAverage, fSigma;
					double			fRed, fGreen, fBlue;

					fSigma = Sigma2(vRedValues, fAverage);
					fRed = fSigma/max(1.0, fAverage)*256.0;
					fSigma = Sigma2(vGreenValues, fAverage);
					fGreen = fSigma/max(1.0, fAverage)*256.0;
					fSigma = Sigma2(vBlueValues, fAverage);
					fBlue = fSigma/max(1.0, fAverage)*256.0;

					m_pHomBitmap->SetPixel(i, lLine, fRed, fGreen, fBlue);
				};

				if (m_vImageOrder.size())
				{
					// Change the order to respect the order of the images
					vAuxRedValues	= vRedValues;
					vAuxGreenValues = vGreenValues;
					vAuxBlueValues  = vBlueValues;
					vRedValues.resize(0);
					vGreenValues.resize(0);
					vBlueValues.resize(0);
					for (LONG k = 0;k<m_vImageOrder.size();k++)
					{
						if (vAuxRedValues[m_vImageOrder[k]] ||
							vAuxGreenValues[m_vImageOrder[k]] ||
							vAuxBlueValues[m_vImageOrder[k]])
						{
							vRedValues.push_back(vAuxRedValues[m_vImageOrder[k]]);
							vGreenValues.push_back(vAuxGreenValues[m_vImageOrder[k]]);
							vBlueValues.push_back(vAuxBlueValues[m_vImageOrder[k]]);
						};
					};

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

				if (!vRedValues.size() || !vGreenValues.size() || !vBlueValues.size())
				{
					vRedValues.resize(0);
					vGreenValues.resize(0);
					vBlueValues.resize(0);
				};
			};

			// Process the value
			if (m_Method == MBP_MEDIAN)
			{
				*pRedCurrentValue	= Median(vRedValues);
				*pGreenCurrentValue = Median(vGreenValues);
				*pBlueCurrentValue	= Median(vBlueValues);
			}
			else if (m_Method == MBP_AVERAGE)
			{
				*pRedCurrentValue	= Average(vRedValues);
				*pGreenCurrentValue = Average(vGreenValues);
				*pBlueCurrentValue	= Average(vBlueValues);
			}
			else if (m_Method == MBP_MAXIMUM)
			{
				*pRedCurrentValue	= Maximum(vRedValues);
				*pGreenCurrentValue = Maximum(vGreenValues);
				*pBlueCurrentValue	= Maximum(vBlueValues);
			}
			else if (m_Method == MBP_SIGMACLIP)
			{
				*pRedCurrentValue	= KappaSigmaClip(vRedValues, m_fKappa, m_lNrIterations);
				*pGreenCurrentValue = KappaSigmaClip(vGreenValues, m_fKappa, m_lNrIterations);
				*pBlueCurrentValue	= KappaSigmaClip(vBlueValues, m_fKappa, m_lNrIterations);
			}
			else if (m_Method == MBP_MEDIANSIGMACLIP)
			{
				*pRedCurrentValue	= MedianKappaSigmaClip(vRedValues, m_fKappa, m_lNrIterations);
				*pGreenCurrentValue = MedianKappaSigmaClip(vGreenValues, m_fKappa, m_lNrIterations);
				*pBlueCurrentValue	= MedianKappaSigmaClip(vBlueValues, m_fKappa, m_lNrIterations);
			}
			else if (m_Method == MBP_AUTOADAPTIVE)
			{
				*pRedCurrentValue	= AutoAdaptiveWeightedAverage(vRedValues, m_lNrIterations);
				*pGreenCurrentValue = AutoAdaptiveWeightedAverage(vGreenValues, m_lNrIterations);
				*pBlueCurrentValue	= AutoAdaptiveWeightedAverage(vBlueValues, m_lNrIterations);
			};

			pRedCurrentValue++;
			pGreenCurrentValue++;
			pBlueCurrentValue++;
		};

		if (pOutputScanLine)
		{
			pBitmap->SetScanLine(lLine, pOutputScanLine);
			free(pOutputScanLine);
			bResult = TRUE;
		};

		return bResult;
	};

public :
	CColorMultiBitmapT()
	{
	};

	virtual ~CColorMultiBitmapT()
	{
	};

	virtual LONG	GetNrChannels()
	{
		return 3;
	};

	virtual LONG	GetNrBytesPerChannel()
	{
		return sizeof(TType);
	};
	
};

/* ------------------------------------------------------------------- */

template <typename TType> 
class CColorMedianFilterEngineT : public CMedianFilterEngine
{
private :
	CColorBitmapT<TType> *		m_pInBitmap;

public :
	CColorMedianFilterEngineT() {};
	virtual ~CColorMedianFilterEngineT() {};

	void	SetInputBitmap(CColorBitmapT<TType> * pInBitmap)
	{
		m_pInBitmap = pInBitmap;
	};

	virtual BOOL	GetFilteredImage(CMemoryBitmap ** ppBitmap, LONG lFilterSize, CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

class CColorBitmap
{
public:
	
	CColorBitmap()
	{
		
	}

	~CColorBitmap()
	{
		
	}

	virtual CMemoryBitmap *	GetRed() = 0;
	virtual CMemoryBitmap *	GetGreen() = 0;
	virtual CMemoryBitmap *	GetBlue() = 0;
};
/* ------------------------------------------------------------------- */

template <typename TType>
class CColorBitmapT : public CMemoryBitmap,
					  public CColorBitmap
{
	template <typename TType> class CColorPixelIterator;
	friend CColorPixelIterator<TType>;

public :
	template <typename TType> 
	class CColorPixelIterator : public CPixelIterator
	{
	private :
		CSmartPtr<CColorBitmapT<TType> >	m_pBitmap;
		TType *								m_pRedValue;
		TType *								m_pGreenValue;
		TType *								m_pBlueValue;
		double								m_fMultiplier;
		LONG								m_lWidth,
											m_lHeight;

	public :
		CColorPixelIterator()
		{
			m_pRedValue = 0;
			m_pGreenValue = 0;
			m_pBlueValue = 0;
			m_fMultiplier = 1.0;
		};

		~CColorPixelIterator() {};

		virtual void	Reset(LONG x, LONG y)
		{
			m_lX = x;
			m_lY = y;
			
			size_t				lOffset = m_pBitmap->GetOffset(x, y);
			m_pRedValue		= &(m_pBitmap->m_Red.m_vPixels[lOffset]);
			m_pGreenValue	= &(m_pBitmap->m_Green.m_vPixels[lOffset]);
			m_pBlueValue	= &(m_pBitmap->m_Blue.m_vPixels[lOffset]);
		};
		void	Init(LONG x, LONG y, CColorBitmapT<TType> * pBitmap)
		{
			m_pBitmap		= pBitmap;
			m_fMultiplier	= pBitmap->m_fMultiplier;
			m_lWidth		= pBitmap->m_lWidth;
			m_lHeight		= pBitmap->m_lHeight;
			Reset(x, y);
		};

		virtual void	GetPixel(double & fRed, double & fGreen, double & fBlue)
		{
			if (m_pRedValue)
			{
				fRed	= (double)(*m_pRedValue)/m_fMultiplier;
				fGreen	= (double)(*m_pGreenValue)/m_fMultiplier;
				fBlue	= (double)(*m_pBlueValue)/m_fMultiplier;
			}
			else
				fRed = fGreen = fBlue = 0.0;
		};

		virtual void	GetPixel(double & fGray)
		{
			if (m_pRedValue)
			{
				double		H, S, L;

				ToHSL((double)(*m_pRedValue)/m_fMultiplier, 
					  (double)(*m_pGreenValue)/m_fMultiplier, 
					  (double)(*m_pBlueValue)/m_fMultiplier, 
					  H, S, L);
				fGray = L * 255.0;
			}
			else
				fGray = 0.0;
		};

		virtual void	SetPixel(double fRed, double fGreen, double fBlue)
		{
			if (m_pRedValue)
			{
				*m_pRedValue	= fRed * m_fMultiplier;
				*m_pGreenValue	= fGreen * m_fMultiplier;
				*m_pBlueValue	= fBlue * m_fMultiplier;
			};
		};

		virtual void	SetPixel(double fGray)
		{
			if (m_pRedValue)
			{
				*m_pRedValue	= fGray * m_fMultiplier;
				*m_pGreenValue	= fGray * m_fMultiplier;
				*m_pBlueValue	= fGray * m_fMultiplier;
			};
		};

		virtual void operator ++(int)
		{
			if (m_pRedValue && m_pGreenValue && m_pBlueValue)
			{
				if (m_lX < m_lWidth-1)
					m_lX++;
				else if (m_lY < m_lHeight-1)
				{
					m_lY++;
					m_lX = 0;
				}
				else
				{
					// End of iteration
					m_lX = -1;
					m_lY = -1;
					m_pRedValue = m_pGreenValue = m_pBlueValue = NULL;
				};
				if (m_pRedValue)
					m_pRedValue++;
				if (m_pGreenValue)
					m_pGreenValue++;
				if (m_pBlueValue)
					m_pBlueValue++;
			};
		};
	};
	friend CColorPixelIterator<TType>;
	friend CColorMedianFilterEngineT<TType>;

private :
	LONG					m_lHeight;
	LONG					m_lWidth;
	BOOL					m_bWord;
	BOOL					m_bDouble;
	BOOL					m_bDWord;
	BOOL					m_bFloat;
	double					m_fMultiplier;

public :
	CGrayBitmapT<TType>		m_Red;
	CGrayBitmapT<TType>		m_Green;
	CGrayBitmapT<TType>		m_Blue;

private :
	BOOL	IsXYOk(LONG x, LONG y)
	{
		return (x>=0 && x<m_lWidth && y>=0 && y<m_lHeight);
	};
	size_t	GetOffset(LONG x, LONG y)
	{
		if (IsXYOk(x, y))
		{
			if (m_bTopDown)
				return (size_t)m_lWidth * (size_t)y + (size_t)x;
			else
				return (size_t)m_lWidth * ((size_t)m_lHeight-1- (size_t)y) + (size_t)x;
		}
		else
			return 0;
	};

public :
	CColorBitmapT() 
	{
		m_lWidth   = 0;
		m_lHeight  = 0;
		m_bWord    = (typeid(TType) == typeid(WORD));
		m_bDouble  = (typeid(TType) == typeid(double));
		m_bDWord   = (typeid(TType) == typeid(DWORD));
		m_bFloat   = (typeid(TType) == typeid(float)) ;
		m_bTopDown = TRUE;

		m_fMultiplier = 1.0;
		if (m_bWord || m_bDouble || m_bFloat)
			m_fMultiplier = 256.0;
		else if (m_bDWord)
			m_fMultiplier = 256.0 * 65536.0;

		m_Red.AddRef();
		m_Green.AddRef();
		m_Blue.AddRef();
	};
	virtual ~CColorBitmapT() {};

	virtual CMemoryBitmap *	Clone(BOOL bEmpty = FALSE)
	{
		CColorBitmapT<TType> *	pResult;

		pResult = new CColorBitmapT<TType>();
		if (pResult)
		{
			if (!bEmpty)
			{
				pResult->m_lHeight		= m_lHeight;
				pResult->m_lWidth		= m_lWidth;
				pResult->m_Red.m_vPixels	= m_Red.m_vPixels;//vRedPixel	= m_Red.m_vPixels;
				pResult->m_Green.m_vPixels	= m_Green.m_vPixels;//vRedPixel	= m_Red.m_vPixels;
				pResult->m_Blue.m_vPixels	= m_Blue.m_vPixels;//vRedPixel	= m_Red.m_vPixels;
			};
			pResult->m_bWord		= m_bWord;
			pResult->m_bDouble		= m_bDouble;
			pResult->m_bDWord		= m_bDWord;
			pResult->m_bFloat		= m_bFloat;

			pResult->CopyFrom(*this);
		};

		return pResult;
	};

	virtual LONG	BitPerSample()
	{
		return sizeof(TType)*8;
	};

	virtual LONG	IsFloat()
	{
		return m_bFloat;
	};

	virtual LONG	Width()
	{
		return m_lWidth;
	};

	virtual LONG	Height()
	{
		return m_lHeight;
	};

	virtual BOOL	Init(LONG lWidth, LONG lHeight)
	{
		BOOL			bResult = TRUE;

		m_lWidth  = lWidth;
		m_lHeight = lHeight;

		bResult = m_Red.Init(lWidth, lHeight) &&
				  m_Green.Init(lWidth, lHeight) &&
				  m_Blue.Init(lWidth, lHeight);

		return bResult;
	};

	virtual BOOL	IsMonochrome()
	{
		return FALSE;
	};

	virtual BOOL	SetValue(LONG i, LONG j, double fRed, double fGreen, double fBlue)
	{ 
		BOOL		bResult = TRUE;

		if (IsXYOk(i, j))
		{
			m_Red.m_vPixels[GetOffset(i, j)]	= fRed;
			m_Green.m_vPixels[GetOffset(i, j)]	= fGreen;
			m_Blue.m_vPixels[GetOffset(i, j)]	= fBlue;
		}
		else
			bResult = FALSE;

		return bResult; 
	};

	virtual BOOL	GetValue(LONG i, LONG j, double & fRed, double & fGreen, double & fBlue)
	{ 
		BOOL		bResult = TRUE;

		if (IsXYOk(i, j))
		{
			fRed   = m_Red.m_vPixels[GetOffset(i, j)];
			fGreen = m_Green.m_vPixels[GetOffset(i, j)];
			fBlue  = m_Blue.m_vPixels[GetOffset(i, j)];
		}
		else
			bResult = FALSE;

		return bResult; 
	};

	virtual BOOL	SetPixel(LONG i, LONG j, double fRed, double fGreen, double fBlue)
	{
		BOOL				bResult = FALSE;

		if (IsXYOk(i, j))
		{
			size_t			lOffset = GetOffset(i, j);
			m_Red.m_vPixels[lOffset]	= fRed * m_fMultiplier;
			m_Green.m_vPixels[lOffset]	= fGreen * m_fMultiplier;
			m_Blue.m_vPixels[lOffset]	= fBlue * m_fMultiplier;

			bResult = TRUE;
		};

		return bResult;
	};

	virtual BOOL	SetPixel(LONG i, LONG j, double fGray)
	{
		BOOL				bResult = FALSE;

		if (IsXYOk(i, j))
		{
			size_t			lOffset = GetOffset(i, j);
			m_Red.m_vPixels[lOffset]	= fGray * m_fMultiplier;
			m_Green.m_vPixels[lOffset]	= fGray * m_fMultiplier;
			m_Blue.m_vPixels[lOffset]	= fGray * m_fMultiplier;

			bResult = TRUE;
		};

		return bResult;
	};

	virtual BOOL	GetPixel(LONG i, LONG j, double & fRed, double & fGreen, double & fBlue)
	{
		BOOL			bResult = FALSE;
		fRed = fGreen = fBlue = 0.0;

		if (IsXYOk(i, j))
		{
			size_t	lOffset = GetOffset(i, j);

			fRed   = m_Red.m_vPixels[lOffset]/m_fMultiplier;
			fGreen = m_Green.m_vPixels[lOffset]/m_fMultiplier;
			fBlue  = m_Blue.m_vPixels[lOffset]/m_fMultiplier;

			bResult = TRUE;
		}

		return bResult;
	};

	virtual BOOL	GetPixel(LONG i, LONG j, double & fGray)
	{
		BOOL			bResult = FALSE;
		double			fRed, fGreen, fBlue;

		fGray = 0.0;

		if (GetPixel(i, j , fRed, fGreen, fBlue))
		{
			double		H, S, L;

			ToHSL(fRed, fGreen, fBlue, H, S, L);
			fGray = L * 255.0;
			bResult = TRUE;
		}

		return bResult;
	};


	virtual BOOL	GetScanLine(LONG j, void * pScanLine)
	{
		BOOL			bResult = FALSE;

		if (j<m_lHeight)
		{
			BYTE *		pTempScan = (BYTE *)pScanLine;

			memcpy(pTempScan, &(m_Red.m_vPixels[(size_t)j*(size_t)m_lWidth]), sizeof(TType)*(size_t)m_lWidth);
			pTempScan += sizeof(TType)*m_lWidth;
			memcpy(pTempScan, &(m_Green.m_vPixels[(size_t)j*(size_t)m_lWidth]), sizeof(TType)*(size_t)m_lWidth);
			pTempScan += sizeof(TType)*m_lWidth;
			memcpy(pTempScan, &(m_Blue.m_vPixels[(size_t)j*(size_t)m_lWidth]), sizeof(TType)*(size_t)m_lWidth);
			bResult = TRUE;
		};

		return bResult;
	};

	virtual BOOL	SetScanLine(LONG j, void * pScanLine)
	{
		BOOL			bResult = FALSE;

		if (j<m_lHeight)
		{
			BYTE *		pTempScan = (BYTE *)pScanLine;

			memcpy(&(m_Red.m_vPixels[(size_t)j*(size_t)m_lWidth]), pTempScan, sizeof(TType)*(size_t)m_lWidth);
			pTempScan += sizeof(TType)*m_lWidth;
			memcpy(&(m_Green.m_vPixels[(size_t)j*(size_t)m_lWidth]), pTempScan, sizeof(TType)*(size_t)m_lWidth);
			pTempScan += sizeof(TType)*m_lWidth;
			memcpy(&(m_Blue.m_vPixels[(size_t)j*(size_t)m_lWidth]), pTempScan, sizeof(TType)*(size_t)m_lWidth);
			bResult = TRUE;
		};

		return bResult;
	};

	void	Clear()
	{
		m_lHeight = 0;
		m_lWidth  = 0;
		m_Red.m_vPixels.clear();
		m_Green.m_vPixels.clear();
		m_Blue.m_vPixels.clear();
	};

	virtual CMultiBitmap * CreateEmptyMultiBitmap()
	{
		CMultiBitmap *		pResult;

		pResult = new CColorMultiBitmapT<TType>();
		if (pResult)
			pResult->SetBitmapModel(this);

		return pResult;
	};

	virtual void	GetIterator(CPixelIterator ** ppIterator, LONG x = 0, LONG y = 0)
	{
		CSmartPtr<CPixelIterator>		pResult;
		CColorPixelIterator<TType> *	pIterator = new CColorPixelIterator<TType>;

		pResult.Attach(pIterator);
		pIterator->Init(x, y, this);

		pResult.CopyTo(ppIterator);
	};

	virtual void GetMedianFilterEngine(CMedianFilterEngine ** ppMedianFilterEngine)
	{
		CSmartPtr<CColorMedianFilterEngineT<TType> >	pMedianFilterEngine;
		CSmartPtr<CMedianFilterEngine>					pMedianFilterEngine2;

		pMedianFilterEngine.Attach(new CColorMedianFilterEngineT<TType>);

		pMedianFilterEngine->SetInputBitmap(this);

		pMedianFilterEngine2 = pMedianFilterEngine;
		pMedianFilterEngine2.CopyTo(ppMedianFilterEngine);
	};

	TType *	GetRedPixel(LONG i, LONG j)
	{
		return &(m_Red.m_vPixels[GetOffset(i, j)]);
	};
	TType *	GetGreenPixel(LONG i, LONG j)
	{
		return &(m_Green.m_vPixels[GetOffset(i, j)]);
	};
	TType *	GetBluePixel(LONG i, LONG j)
	{
		return &(m_Blue.m_vPixels[GetOffset(i, j)]);
	};

	virtual CMemoryBitmap *	GetRed()
	{
		return &m_Red;
	};

	virtual CMemoryBitmap *	GetGreen()
	{
		return &m_Green;
	};

	virtual CMemoryBitmap *	GetBlue()
	{
		return &m_Blue;
	};

	double	GetMultiplier()
	{
		return m_fMultiplier;
	};

	virtual double GetMaximumValue()
	{
		return m_fMultiplier*256.0;
	};

	virtual void RemoveHotPixels(CDSSProgress * pProgress = NULL) 
	{
		m_Red.RemoveHotPixels(pProgress);
		m_Green.RemoveHotPixels(pProgress);
		m_Blue.RemoveHotPixels(pProgress);
	};

	virtual void	GetCharacteristics(CBitmapCharacteristics & bc)
	{
		bc.m_bFloat		= m_bFloat ? true : false;
		bc.m_dwHeight	= m_lHeight;
		bc.m_dwWidth	= m_lWidth;
		bc.m_lNrChannels = 3;
		bc.m_lBitsPerPixel = BitPerSample();
	};
};

/* ------------------------------------------------------------------- */

typedef CColorBitmapT<BYTE>			C24BitColorBitmap;
typedef CColorBitmapT<WORD>			C48BitColorBitmap;
typedef CColorBitmapT<DWORD>		C96BitColorBitmap;
typedef CColorBitmapT<float>		C96BitFloatColorBitmap;

/* ------------------------------------------------------------------- */

#include "MedianFilterEngine.h"

/* ------------------------------------------------------------------- */

class CBitmapInfo
{
public :
	CString				m_strFileName;
	CString				m_strFileType;
	CString				m_strModel;
	LONG				m_lISOSpeed;
	double				m_fExposure;
	LONG				m_lWidth;
	LONG				m_lHeight;
	LONG				m_lBitPerChannel;
	LONG				m_lNrChannels;
	BOOL				m_bCanLoad;
	BOOL				m_bFloat;
	CFATYPE				m_CFAType;
	BOOL				m_bMaster;
	BOOL				m_bFITS16bit;
	CString				m_strDateTime;
	SYSTEMTIME			m_DateTime;
	SYSTEMTIME			m_InfoTime;
	CBitmapExtraInfo	m_ExtraInfo;

private :
	void	CopyFrom(const CBitmapInfo & bi)
	{
		m_strFileName	=bi.m_strFileName	;
		m_strFileType	=bi.m_strFileType	;
		m_strModel		=bi.m_strModel		;
		m_lISOSpeed		=bi.m_lISOSpeed		;
		m_fExposure		=bi.m_fExposure		;
		m_lWidth		=bi.m_lWidth		;
		m_lHeight		=bi.m_lHeight		;
		m_lBitPerChannel=bi.m_lBitPerChannel;
		m_lNrChannels	=bi.m_lNrChannels	;
		m_bCanLoad		=bi.m_bCanLoad		;
		m_bFloat		=bi.m_bFloat		;
		m_CFAType		=bi.m_CFAType		;
		m_bMaster		=bi.m_bMaster		;
		m_bFITS16bit	=bi.m_bFITS16bit	;
		m_strDateTime	=bi.m_strDateTime	;
		m_DateTime		=bi.m_DateTime		;
		m_InfoTime		=bi.m_InfoTime		;
		m_ExtraInfo		=bi.m_ExtraInfo		;
	};

public :
	CBitmapInfo()
	{
		m_lWidth		 = 0;
		m_lHeight		 = 0;
		m_lBitPerChannel = 0;
		m_lNrChannels	 = 0;
		m_bCanLoad		 = FALSE;
		m_CFAType		 = CFATYPE_NONE;
		m_bMaster		 = FALSE;
		m_bFloat		 = FALSE;
		m_lISOSpeed		 = 0;
		m_fExposure		 = 0;
		m_bFITS16bit	 = FALSE;
		m_DateTime.wYear = 0;
	};

	CBitmapInfo(const CBitmapInfo & bi)
	{
		CopyFrom(bi);
	};

	CBitmapInfo(LPCTSTR szFileName)
	{
		m_strFileName = szFileName;
	};

	CBitmapInfo & operator = (const CBitmapInfo & bi)
	{
		CopyFrom(bi);
		return (*this);
	};

	bool operator < (const CBitmapInfo & bi) const
	{
		return (m_strFileName.CompareNoCase(bi.m_strFileName) < 0);
	};

	virtual ~CBitmapInfo()
	{
	};

	BOOL	CanLoad()
	{
		return m_bCanLoad;
	};

	BOOL	IsCFA()
	{
		return (m_CFAType != CFATYPE_NONE);
	};

	BOOL	IsMaster()
	{
		return m_bMaster;
	};

	void	GetDescription(CString & strDescription)
	{
		strDescription = m_strFileType;
		if (m_strModel.GetLength())
			strDescription.Format(_T("%s (%s)"), (LPCTSTR)m_strFileType, (LPCTSTR)m_strModel);
	};

	BOOL	IsInitialized()
	{
		return m_lWidth && m_lHeight;
	};
};

inline bool CompareBitmapInfoDateTime (const CBitmapInfo & bi1, const CBitmapInfo & bi2)
{
	if (bi1.m_DateTime.wYear < bi2.m_DateTime.wYear)
		return true;
	else if (bi1.m_DateTime.wYear > bi2.m_DateTime.wYear)
		return false;
	else if (bi1.m_DateTime.wMonth < bi2.m_DateTime.wMonth)
		return true;
	else if (bi1.m_DateTime.wMonth > bi2.m_DateTime.wMonth)
		return false;
	else if (bi1.m_DateTime.wDay < bi2.m_DateTime.wDay)
		return true;
	else if (bi1.m_DateTime.wDay > bi2.m_DateTime.wDay)
		return false;
	else if (bi1.m_DateTime.wHour < bi2.m_DateTime.wHour)
		return true;
	else if (bi1.m_DateTime.wHour > bi2.m_DateTime.wHour)
		return false;
	else if (bi1.m_DateTime.wMinute < bi2.m_DateTime.wMinute)
		return true;
	else if (bi1.m_DateTime.wMinute > bi2.m_DateTime.wMinute)
		return false;
	else if (bi1.m_DateTime.wSecond < bi2.m_DateTime.wSecond)
		return true;
	else if (bi1.m_DateTime.wSecond > bi2.m_DateTime.wSecond)
		return false;
	else
		return (bi1.m_DateTime.wMilliseconds < bi2.m_DateTime.wMilliseconds);
};

/* ------------------------------------------------------------------- */

#if DSSFILEDECODING==1

class	CAllDepthBitmap
{
public :
	BOOL						m_bDontUseAHD;

public :
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CSmartPtr<C32BitsBitmap>	m_pWndBitmap;

private :
	void	CopyFrom(const CAllDepthBitmap & adb)
	{
		m_bDontUseAHD	= adb.m_bDontUseAHD;
		m_pBitmap		= adb.m_pBitmap;
		m_pWndBitmap	= adb.m_pWndBitmap;
	};

public :
	CAllDepthBitmap() {};
	~CAllDepthBitmap() {};
	CAllDepthBitmap(const CAllDepthBitmap & adb)
	{
		CopyFrom(adb);
	};

	CAllDepthBitmap & operator = (const CAllDepthBitmap & adb)
	{
		CopyFrom(adb);
		return (*this);
	};

	void	Clear()
	{
		m_pBitmap.Release();
		m_pWndBitmap.Release();
	};

	void	SetDontUseAHD(BOOL bSet)
	{
		m_bDontUseAHD = bSet;
	};
};

void	CopyBitmapToClipboard(HBITMAP hBitmap);

BOOL	RetrieveEXIFInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);
//HBITMAP LoadPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap = NULL);
BOOL	LoadPicture(LPCTSTR szFileName, CAllDepthBitmap & AllDepthBitmap, CDSSProgress * pProgress = NULL);
BOOL	DebayerPicture(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress);

#endif // DSSFILEDECODING
BOOL	LoadPicture(LPCTSTR szFileName, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress);

BOOL	GetPictureInfo(LPCTSTR szFileName, CBitmapInfo & BitmapInfo);

BOOL	GetFilteredImage(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, LONG lFilterSize, CDSSProgress * pProgress = NULL);

/* ------------------------------------------------------------------- */

inline BOOL	IsCFA(CMemoryBitmap * pBitmap)
{
	if (pBitmap)
		return pBitmap->IsCFA();
	else
		return FALSE;
};

/* ------------------------------------------------------------------- */

CFATYPE	GetCFAType(CMemoryBitmap * pBitmap);

/* ------------------------------------------------------------------- */

inline BOOL IsMonochrome(CMemoryBitmap * pBitmap)
{
	if (pBitmap)
		return pBitmap->IsMonochrome();
	else
		return FALSE;
};

/* ------------------------------------------------------------------- */
#include "RAWUtils.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "AHDDemosaicing.h"

#endif // _BITMAPEXT_H__