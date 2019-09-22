#ifndef __STARMASK_H__
#define __STARMASK_H__

#include "RegisterEngine.h"

/* ------------------------------------------------------------------- */

typedef enum tagSTARMASKSTYLE
{
	SMS_BELL			= 0,
	SMS_TRUNCATEDBELL	= 1,
	SMS_LINEAR			= 2,
	SMS_TRUNCATEDLINEAR	= 3,
	SMS_CUBIC			= 4,
	SMS_QUADRATIC		= 5
}STARMASKSTYLE;

class		CStarMaskFunction
{
protected :
	double				m_fRadius;

public :
	CStarMaskFunction()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		m_fRadius = fRadius;
	};
	virtual double	Compute(double fValue) = 0;
};

/* ------------------------------------------------------------------- */

class		CStarMaskFunction_Bell : public CStarMaskFunction
{
protected :
	double			fFactor1,
					fFactor2;

public :
	CStarMaskFunction_Bell()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		CStarMaskFunction::SetRadius(fRadius);
		fFactor1 = 1.0;
		fFactor2 = 2*fRadius*fRadius;
	};

	virtual double	Compute(double fValue)
	{
		return fFactor1 * exp(-(fValue * fValue)/fFactor2);
	};
};

/* ------------------------------------------------------------------- */

class		CStarMaskFunction_BellTruncated : public CStarMaskFunction_Bell
{
public :
	CStarMaskFunction_BellTruncated()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		CStarMaskFunction_Bell::SetRadius(fRadius);
		fFactor1 = 1.0/exp(-0.5);
	};

	virtual double	Compute(double fValue)
	{
		return min(1.0, fFactor1 * exp(-(fValue * fValue)/fFactor2));
	};
};

/* ------------------------------------------------------------------- */

class		CStarMaskFunction_Linear : public CStarMaskFunction
{
protected :

public :
	CStarMaskFunction_Linear()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		CStarMaskFunction::SetRadius(fRadius);
	};

	virtual double	Compute(double fValue)
	{
		return max(0.0, 1-fValue/3.0/m_fRadius);
	};
};

/* ------------------------------------------------------------------- */

class		CStarMaskFunction_LinearTruncated : public CStarMaskFunction
{
protected :

public :
	CStarMaskFunction_LinearTruncated()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		CStarMaskFunction::SetRadius(fRadius);
	};

	virtual double	Compute(double fValue)
	{
		return max(0.0, 1.5-fValue*1.5/3.0/m_fRadius);
	};
};

/* ------------------------------------------------------------------- */

class		CStarMaskFunction_Cubic : public CStarMaskFunction
{
protected :

public :
	CStarMaskFunction_Cubic()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		CStarMaskFunction::SetRadius(fRadius);
	};

	virtual double	Compute(double fValue)
	{
		fValue /= 3.0*m_fRadius;
		return max(0.0, 1.0-fValue*fValue*fValue);
	};
};

/* ------------------------------------------------------------------- */

class		CStarMaskFunction_Quadratic : public CStarMaskFunction
{
protected :

public :
	CStarMaskFunction_Quadratic()
	{
	};

	virtual void	SetRadius(double fRadius)
	{
		CStarMaskFunction::SetRadius(fRadius);
	};

	virtual double	Compute(double fValue)
	{
		fValue /= 3.0*m_fRadius;
		return max(0.0, 1.0-fValue*fValue*fValue*fValue);
	};
};

/* ------------------------------------------------------------------- */

class CStarMaskEngine
{
private :
	double				m_fMinLuminancy;
	BOOL				m_bRemoveHotPixels;
	double				m_fPercentIncrease;
	double				m_fPixelIncrease;
	double				m_fMinSize,
						m_fMaxSize;
	STARMASKSTYLE		m_StarShape;

private :
	void	GetShapeFunction(CStarMaskFunction ** ppStarMaskFunction)
	{
		switch (m_StarShape)
		{
		case SMS_BELL :
			*ppStarMaskFunction = new CStarMaskFunction_Bell;
			break;
		case SMS_TRUNCATEDBELL :
			*ppStarMaskFunction = new CStarMaskFunction_BellTruncated;
			break;
		case SMS_LINEAR	:
			*ppStarMaskFunction = new CStarMaskFunction_Linear;
			break;
		case SMS_TRUNCATEDLINEAR	:
			*ppStarMaskFunction = new CStarMaskFunction_LinearTruncated;
			break;
		case SMS_CUBIC :
			*ppStarMaskFunction = new CStarMaskFunction_Cubic;
			break;
		case SMS_QUADRATIC :
			*ppStarMaskFunction = new CStarMaskFunction_Quadratic;
			break;
		};
	};

public :
	CStarMaskEngine()
	{
		CRegistry			reg;
		DWORD				bHotPixels = 0;
		DWORD				dwThreshold = 10;
		DWORD				dwPercent = 100;
		DWORD				dwPixel = 0;
		DWORD				dwMinSize = 2;
		DWORD				dwMaxSize = 25;
		DWORD				dwStarShape = 1;

		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("DetectHotPixels"), bHotPixels);
		m_bRemoveHotPixels = bHotPixels;
		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("DetectionThreshold"), dwThreshold);
		m_fMinLuminancy = (double)dwThreshold/100.0;

		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("PercentRadius"), dwPercent);
		m_fPercentIncrease = (double)dwPercent/100.0;
		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("PixelIncrease"), dwPixel);
		m_fPixelIncrease = (double)dwPixel;

		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("MinSize"), dwMinSize);
		m_fMinSize = (double)dwMinSize;
		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("MaxSize"), dwMaxSize);
		m_fMaxSize = (double)dwMaxSize;

		reg.LoadKey(REGENTRY_BASEKEY_STARMASK, _T("StarShape"), dwStarShape);
		m_StarShape = (STARMASKSTYLE)dwStarShape;
	};

	virtual ~CStarMaskEngine() {};

	void	SetDetectionThreshold(double fMinLuminancy)
	{
		m_fMinLuminancy = fMinLuminancy;
	};

	void	SetHotPixelRemoval(BOOL bHotPixels)
	{
		m_bRemoveHotPixels = bHotPixels;
	};

	BOOL	CreateStarMask(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress = NULL);
	BOOL	CreateStarMask2(CMemoryBitmap * pBitmap, CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress = NULL);
};

#endif // __STARMASK_H__