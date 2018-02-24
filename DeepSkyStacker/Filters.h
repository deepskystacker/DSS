#ifndef __FILTERS_H__
#define __FILTERS_H__

#include "BitmapExt.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "DarkFrame.h"

/* ------------------------------------------------------------------- */

class CImageFilter
{
public :
	CImageFilter() {};
	virtual ~CImageFilter() {};

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** pOutBitmap, CDSSProgress * pProgress = NULL) = 0;
};

/* ------------------------------------------------------------------- */

class CFilterMatrix
{
public :
	std::vector<double>			m_vMatrix;
	LONG						m_lSize;

private :
	void	CopyFrom(const CFilterMatrix & fm)
	{
		m_vMatrix	= fm.m_vMatrix;
		m_lSize		= fm.m_lSize;
	};

public :
	CFilterMatrix()
	{
	};

	CFilterMatrix(const CFilterMatrix & fm)
	{
		CopyFrom(fm);
	};

	~CFilterMatrix() {};

	CFilterMatrix & operator = (const CFilterMatrix & fm)
	{
		CopyFrom(fm);
		return (*this);
	};

	void	Init(LONG lSize)
	{
		m_lSize = lSize;
		m_vMatrix.resize((lSize*2)+1);
	};

};

/* ------------------------------------------------------------------- */

class CMedianImageFilter : public CImageFilter
{
private :
	BOOL						m_bMonochrome;
	BOOL						m_bCFA;
	LONG						m_lWidth,
								m_lHeight;
	CSmartPtr<CMemoryBitmap>	m_pInBitmap;
	CSmartPtr<CMemoryBitmap>	m_pOutBitmap;
	LONG						m_lFilterSize;
	std::vector<double>			m_vGrayPixelValues;
	std::vector<double>			m_vRedPixelValues;
	std::vector<double>			m_vGreenPixelValues;
	std::vector<double>			m_vBluePixelValues;

public :
	void	ComputeMedianAt(LONG x, LONG y, double & fGrayValue, BAYERCOLOR BayerColor = BAYER_UNKNOWN);
	void	ComputeMedianAt(LONG x, LONG y, double & fRedValue, double & fGreenValue, double & fBlueValue);

public :
	CMedianImageFilter()
	{
		m_bMonochrome	= FALSE;
		m_bCFA			= FALSE;
		m_lFilterSize	= 1;
	};

	virtual ~CMedianImageFilter()
	{
	};

	void	SetFilterSize(LONG lFilterSize)
	{
		m_lFilterSize = lFilterSize;
	};

	BOOL	IsMonochrome()
	{
		return m_bMonochrome;
	};

	BOOL	IsCFA()
	{
		return m_bCFA;
	};

	void	SetBitmap(CMemoryBitmap * pBitmap)
	{
		m_pInBitmap = pBitmap;
		if (pBitmap)
		{
			m_bMonochrome = pBitmap->IsMonochrome();
			m_bCFA		  = pBitmap->IsCFA();
			m_lWidth	  = pBitmap->Width();
			m_lHeight	  = pBitmap->Height();
		};
	};

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** pOutBitmap, CDSSProgress * pProgress = NULL);
};

/* ------------------------------------------------------------------- */

class CExtendedMedianImageFilter : public CImageFilter
{
private :
	BOOL				m_bFilterHot;
	BOOL				m_bFilterCold;
	BOOL				m_bRecursive;
	double				m_fHotThreshold;
	double				m_fColdThreshold;
	double				m_fRejectHotThreshold;
	double				m_fRejectColdThreshold;
	BOOL				m_bUseRejectThreshold;
	EXCLUDEDPIXELVECTOR	m_vExcludedPixels;

private :
	void	AnalyzeImage(CMemoryBitmap * pInBitmap, BOOL bComputeThresholds);
	void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap * pOutBitmap, CDSSProgress * pProgress = NULL);

public :
	CExtendedMedianImageFilter() 
	{
		m_bFilterHot		 = TRUE;
		m_bFilterCold		 = TRUE;
		m_bRecursive		 = FALSE;
		m_fHotThreshold		 = 0.0;
		m_fColdThreshold	 = 0.0;
		m_fRejectHotThreshold  = 0;
		m_fRejectColdThreshold = 0;
		m_bUseRejectThreshold  = FALSE;
	};

	virtual ~CExtendedMedianImageFilter() {};

	void	SetRejectThreshold(double fRejectColdThreshold, double fRejectHotThreshold)
	{
		m_fRejectColdThreshold = fRejectColdThreshold;
		m_fRejectHotThreshold  = fRejectHotThreshold;
		m_bUseRejectThreshold  = TRUE;
	};

	void	SetUseRecursive(BOOL bSet)
	{
		m_bRecursive = bSet;
	};

	void	SetThresholds(double fColdThreshold, double fHotThreshold)
	{
		m_fColdThreshold = fColdThreshold;
		m_fHotThreshold  = fHotThreshold;
		m_bUseRejectThreshold  = FALSE;
	};

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress = NULL);
};

/* ------------------------------------------------------------------- */

class CDirectionalImageFilter : public CImageFilter
{
private :
	double						m_fAngle;
	double						m_lSize;
	CSmartPtr<CMemoryBitmap>	m_pInBitmap;
	BOOL						m_bMonochrome;

private :
	void	GetValuesAlongAngle(LONG x, LONG y, double fAngle, std::vector<double> & vValues);
	void	GetValuesAlongAngle(LONG x, LONG y, double fAngle, std::vector<double> & vRedValues, std::vector<double> & vGreenValues, std::vector<double> & vBlueValues);
	void	InitFilterMatrix(CFilterMatrix & fm);

public :
	CDirectionalImageFilter() 
	{
		m_fAngle = 0;
		m_lSize  = 1;
	};
	~CDirectionalImageFilter() {};

	void			SetAngle(double fAngle, LONG lSize)
	{
		m_fAngle = fAngle;
		m_lSize  = lSize;
	};

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** pOutBitmap, CDSSProgress * pProgress = NULL);
};

/* ------------------------------------------------------------------- */

#endif // __FILTERS_H__