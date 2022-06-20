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

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** pOutBitmap, CDSSProgress * pProgress = nullptr) = 0;
};

/* ------------------------------------------------------------------- */

class CFilterMatrix
{
public :
	std::vector<double>			m_vMatrix;
	int						m_lSize;

private :
	void	CopyFrom(const CFilterMatrix & fm)
	{
		m_vMatrix	= fm.m_vMatrix;
		m_lSize		= fm.m_lSize;
	};

public :
	CFilterMatrix()
	{
        m_lSize = 0;
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

	void	Init(int lSize)
	{
		m_lSize = lSize;
		m_vMatrix.resize((lSize*2)+1);
	};

};

/* ------------------------------------------------------------------- */

class CMedianImageFilter : public CImageFilter
{
private :
	bool						m_bMonochrome;
	bool						m_bCFA;
	int						m_lWidth,
								m_lHeight;
	CSmartPtr<CMemoryBitmap>	m_pInBitmap;
	CSmartPtr<CMemoryBitmap>	m_pOutBitmap;
	int						m_lFilterSize;
	std::vector<double>			m_vGrayPixelValues;
	std::vector<double>			m_vRedPixelValues;
	std::vector<double>			m_vGreenPixelValues;
	std::vector<double>			m_vBluePixelValues;

public :
	void	ComputeMedianAt(int x, int y, double & fGrayValue, BAYERCOLOR BayerColor = BAYER_UNKNOWN);
	void	ComputeMedianAt(int x, int y, double & fRedValue, double & fGreenValue, double & fBlueValue);

public :
	CMedianImageFilter()
	{
		m_bMonochrome	= false;
		m_bCFA			= false;
		m_lFilterSize	= 1;
        m_lWidth        = 0;
        m_lHeight       = 0;
	};

	virtual ~CMedianImageFilter()
	{
	};

	void	SetFilterSize(int lFilterSize)
	{
		m_lFilterSize = lFilterSize;
	};

	bool	IsMonochrome()
	{
		return m_bMonochrome;
	};

	bool	IsCFA()
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

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** pOutBitmap, CDSSProgress * pProgress = nullptr);
};

/* ------------------------------------------------------------------- */

class CExtendedMedianImageFilter : public CImageFilter
{
private :
	bool				m_bFilterHot;
	bool				m_bFilterCold;
	bool				m_bRecursive;
	double				m_fHotThreshold;
	double				m_fColdThreshold;
	double				m_fRejectHotThreshold;
	double				m_fRejectColdThreshold;
	bool				m_bUseRejectThreshold;
	EXCLUDEDPIXELVECTOR	m_vExcludedPixels;

private :
	void	AnalyzeImage(CMemoryBitmap * pInBitmap, bool bComputeThresholds);
	void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap * pOutBitmap, CDSSProgress * pProgress = nullptr);

public :
	CExtendedMedianImageFilter()
	{
		m_bFilterHot		 = true;
		m_bFilterCold		 = true;
		m_bRecursive		 = false;
		m_fHotThreshold		 = 0.0;
		m_fColdThreshold	 = 0.0;
		m_fRejectHotThreshold  = 0;
		m_fRejectColdThreshold = 0;
		m_bUseRejectThreshold  = false;
	};

	virtual ~CExtendedMedianImageFilter() {};

	void	SetRejectThreshold(double fRejectColdThreshold, double fRejectHotThreshold)
	{
		m_fRejectColdThreshold = fRejectColdThreshold;
		m_fRejectHotThreshold  = fRejectHotThreshold;
		m_bUseRejectThreshold  = true;
	};

	void	SetUseRecursive(bool bSet)
	{
		m_bRecursive = bSet;
	};

	void	SetThresholds(double fColdThreshold, double fHotThreshold)
	{
		m_fColdThreshold = fColdThreshold;
		m_fHotThreshold  = fHotThreshold;
		m_bUseRejectThreshold  = false;
	};

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** ppOutBitmap, CDSSProgress * pProgress = nullptr);
};

/* ------------------------------------------------------------------- */

class CDirectionalImageFilter : public CImageFilter
{
private :
	double						m_fAngle;
	double						m_lSize;
	CSmartPtr<CMemoryBitmap>	m_pInBitmap;
	bool						m_bMonochrome;

private :
	void	GetValuesAlongAngle(int x, int y, double fAngle, std::vector<double> & vValues);
	void	GetValuesAlongAngle(int x, int y, double fAngle, std::vector<double> & vRedValues, std::vector<double> & vGreenValues, std::vector<double> & vBlueValues);
	void	InitFilterMatrix(CFilterMatrix & fm);

public :
	CDirectionalImageFilter()
	{
		m_fAngle = 0;
		m_lSize  = 1;
        m_bMonochrome = false;
	};
	~CDirectionalImageFilter() {};

	void			SetAngle(double fAngle, int lSize)
	{
		m_fAngle = fAngle;
		m_lSize  = lSize;
	};

	virtual void	ApplyFilter(CMemoryBitmap * pInBitmap, CMemoryBitmap ** pOutBitmap, CDSSProgress * pProgress = nullptr);
};

/* ------------------------------------------------------------------- */

#endif // __FILTERS_H__
