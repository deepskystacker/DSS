#include <stdafx.h>
#include "DeBloom.h"
#include "TIFFUtil.h"
#include "Filters.h"
#include "BackgroundCalibration.h"

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef DSSBETA
//#define DEBUGDEBLOOM
#endif

/* ------------------------------------------------------------------- */

inline	bool IsBloomedValue(double fValue)
{
	return fValue > 200.0;
};

inline	bool IsBloomedBorderValue(double fValue)
{
	return (fValue < 150.0) && (fValue>0.0);
};

inline	bool IsBloomedProcessedValue(double fValue)
{
	return (fValue > 150.0) && (fValue < 200.0);
};

/* ------------------------------------------------------------------- */

inline bool IsEdgeSimple(double * pfGray)
{
	bool			bResult;

	double			fVariation1 =(pfGray[0]-pfGray[1])/256.0,
					fVariation2 =(pfGray[1]-pfGray[2])/256.0,
					fVariation3 =fabs(pfGray[2]-pfGray[3])/256.0;

	if (fVariation1>0.70)
		bResult = false;
	else if (fVariation1>0.50)
	{
		if (fVariation2<0.10)
			bResult = false;
		else
			bResult = (fVariation3<0.05);
	}
	else
	{
		if (fVariation2>0.10)
			bResult = (fVariation3<0.05);
		else
			bResult = false;
			
	};

/*	double			fAvgVariation1 = (fabs(pfGray[4]-pfGray[3])/max(1.0, pfGray[4])+fabs(pfGray[3]-pfGray[2])/max(1.0, pfGray[3]))/2.0;
	double			fAvgVariation2 = (fabs(pfGray[1]-pfGray[2]))/max(1.0, pfGray[2]);

	bResult = fAvgVariation1*2 < fAvgVariation2;

	bResult = (fVariation1<0) && (fVariation2<0) && (fVariation3>0);*/

	return bResult;
};

/* ------------------------------------------------------------------- */

inline bool IsEdge(double * pfGray)
{
	bool			bResult = false;
	double			fGradient0,
					fGradient1,
					fGradient2,
					fGradient3;

	fGradient0 = fabs(pfGray[1]-pfGray[0])/256.0;
	fGradient1 = fabs(pfGray[2]-pfGray[0])/256.0;
	fGradient2 = fabs(pfGray[3]-pfGray[1])/256.0;
	fGradient3 = fabs(pfGray[4]-pfGray[2])/256.0;

	double			fEdge;

	fEdge = (fGradient2-fGradient0)/2.0/(fGradient0-2.0*fGradient1+fGradient2)+2;

	if (fGradient1>fGradient0 && fGradient1>fGradient2 && fEdge>-0.3 && fGradient1 > 0.7)
		bResult = true;

	return bResult;
};


/* ------------------------------------------------------------------- */

bool	CDeBloom::IsLeftEdge(CMemoryBitmap * pBitmap, LONG x, LONG y)
{
	double			fGray[5];

	pBitmap->GetPixel(max(0L, x-0), y, fGray[0]); // Current Pixel
	pBitmap->GetPixel(max(0L, x-1), y, fGray[1]); // 
	pBitmap->GetPixel(max(0L, x-2), y, fGray[2]);
	pBitmap->GetPixel(max(0L, x-3), y, fGray[3]);
	pBitmap->GetPixel(max(0L, x-4), y, fGray[4]);

	for (LONG i = 0;i<5;i++)
		fGray[i] = (fGray[i]-m_fBackground)/(256.0-m_fBackground)*256.0;

	return IsEdge(fGray);
};

/* ------------------------------------------------------------------- */

bool	CDeBloom::IsRightEdge(CMemoryBitmap * pBitmap, LONG x, LONG y)
{
	double			fGray[5];

	pBitmap->GetPixel(min(m_lWidth-1, x+0), y, fGray[0]); // Current Pixel
	pBitmap->GetPixel(min(m_lWidth-1, x+1), y, fGray[1]); // Current Pixel
	pBitmap->GetPixel(min(m_lWidth-1, x+2), y, fGray[2]); // Current Pixel
	pBitmap->GetPixel(min(m_lWidth-1, x+3), y, fGray[3]); // Current Pixel
	pBitmap->GetPixel(min(m_lWidth-1, x+4), y, fGray[4]); // Current Pixel

	for (LONG i = 0;i<5;i++)
		fGray[i] = (fGray[i]-m_fBackground)/(256.0-m_fBackground)*256.0;

	return IsEdge(fGray);
};

/* ------------------------------------------------------------------- */

bool	CDeBloom::IsTopEdge(CMemoryBitmap * pBitmap, LONG x, LONG y)
{
	double			fGray[5];

	pBitmap->GetPixel(x, max(0L, y-0), fGray[0]); // Current Pixel
	pBitmap->GetPixel(x, max(0L, y-1), fGray[1]); // Current Pixel
	pBitmap->GetPixel(x, max(0L, y-2), fGray[2]); // Current Pixel
	pBitmap->GetPixel(x, max(0L, y-3), fGray[3]); // Current Pixel
	pBitmap->GetPixel(x, max(0L, y-4), fGray[4]); // Current Pixel

	for (LONG i = 0;i<5;i++)
		fGray[i] = (fGray[i]-m_fBackground)/(256.0-m_fBackground)*256.0;

	return IsEdge(fGray);
};

/* ------------------------------------------------------------------- */

bool	CDeBloom::IsBottomEdge(CMemoryBitmap * pBitmap, LONG x, LONG y)
{
	double			fGray[5];

	pBitmap->GetPixel(x, min(m_lHeight-1, y+0), fGray[0]); // Current Pixel
	pBitmap->GetPixel(x, min(m_lHeight-1, y+1), fGray[1]); // Current Pixel
	pBitmap->GetPixel(x, min(m_lHeight-1, y+2), fGray[2]); // Current Pixel
	pBitmap->GetPixel(x, min(m_lHeight-1, y+3), fGray[3]); // Current Pixel
	pBitmap->GetPixel(x, min(m_lHeight-1, y+4), fGray[4]); // Current Pixel

	for (LONG i = 0;i<5;i++)
		fGray[i] = (fGray[i]-m_fBackground)/(256.0-m_fBackground)*256.0;

	return IsEdge(fGray);
};

/* ------------------------------------------------------------------- */

inline double	ComputeCenter(std::vector<double> & vValues)
{
	double					fResult = -1.0;
	LONG					i;
	bool					bEmptyValues = false;
	LONG					lStartEmpty = -1,
							lEndEmpty   = -1;
	double					fSum = 0,
							fNrValues = 0;

	for (i = 0;i<vValues.size();i++)
	{
		if (vValues[i]<0)
		{
			lEndEmpty = i+1;
			if (lStartEmpty<0)
				lStartEmpty = i+1;
			bEmptyValues = true;
		};
	};

	if (bEmptyValues)
	{
		// Keep only the same number of values on each side
		//         S    E
		//   12345678901234567890
		//
		LONG				lNrLeftKeptValues,
							lNrRightKeptValues,
							lNrKeptValues;

		lNrLeftKeptValues  = lStartEmpty-1;
		lNrRightKeptValues = (LONG)vValues.size()-lEndEmpty; 
		lNrKeptValues = min(lNrLeftKeptValues, lNrRightKeptValues);

		if (lNrLeftKeptValues>lNrKeptValues)
		{
			for (i = 0;i<lNrLeftKeptValues-lNrKeptValues;i++)
				vValues[i] = -1.0;
		}
		else if (lNrRightKeptValues>lNrKeptValues)
		{
			for (i = 0;i<lNrRightKeptValues-lNrKeptValues;i++)
				vValues[vValues.size()-1-i] = -1.0;
		};
	};
	fSum = 0;
	fNrValues = 0;
	for (i = 0;i<vValues.size();i++)
	{
		if (vValues[i]>=0)
		{
			fSum += vValues[i] * ((double)i+0.5);
			fNrValues += vValues[i];
		};
	};

	if (fNrValues)
		fResult = fSum/fNrValues;

	return fResult;
};


/* ------------------------------------------------------------------- */

inline double			distance(double x1, double y1, double x2, double y2)
{
	return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
};

static double	InterpolatePixelValue(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CPointExt & pt, bool bNoBloom = false)
{
	double				x0 = floor(pt.X-0.5), 
						y0 = floor(pt.Y-0.5);

	double				fd00 = distance(x0+0.5, y0+0.5, pt.X, pt.Y),
						fd10 = distance(x0+1.5, y0+0.5, pt.X, pt.Y),
						fd01 = distance(x0+0.5, y0+1.5, pt.X, pt.Y),
						fd11 = distance(x0+1.5, y0+1.5, pt.X, pt.Y);

	double				fv00 = -1.0,
						fv10 = -1.0,
						fv01 = -1.0,
						fv11 = -1.0;
	double				fMask;
	bool				bBloom = false;

	pMask->GetPixel(x0, y0, fMask);
	if (!IsBloomedValue(fMask))
		pBitmap->GetPixel(x0, y0, fv00);
	else
		bBloom = true;

	pMask->GetPixel(x0, y0+1.0, fMask);
	if (!IsBloomedValue(fMask))
		pBitmap->GetPixel(x0, y0+1.0, fv01);
	else
		bBloom = true;

	pMask->GetPixel(x0+1.0, y0, fMask);
	if (!IsBloomedValue(fMask))
		pBitmap->GetPixel(x0+1.0, y0, fv10);
	else
		bBloom = true;

	pMask->GetPixel(x0+1.0, y0+1.0, fMask);
	if (!IsBloomedValue(fMask))
		pBitmap->GetPixel(x0+1.0, y0+1.0, fv11);
	else
		bBloom = true;

	double				fWeight = 0,
						fSum    = 0;

	if (fv00>=0)
	{
		fWeight += 1.5-fd00;
		fSum    += (1.5-fd00)*fv00;
	};
	if (fv10>=0)
	{
		fWeight += 1.5-fd10;
		fSum    += (1.5-fd10)*fv10;
	};
	if (fv01>=0)
	{
		fWeight += 1.5-fd01;
		fSum    += (1.5-fd01)*fv01;
	};
	if (fv11>=0)
	{
		fWeight += 1.5-fd11;
		fSum    += (1.5-fd11)*fv11;
	};

	if (!fWeight || (bBloom && !bNoBloom))
		return -1.0;
	else
		return fSum/fWeight;
};

/* ------------------------------------------------------------------- */

double	CDeBloom::ComputeStarGradient(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStarGradient & bsg, double fRadius)
{
	CPointExt			ptNW,
						ptSW,
						ptNE,
						ptSE;

	ptNW = ptSW = ptNE = ptSE = bsg.ptStar;

	ptNW.X += -fRadius+bsg.fdX;		ptNW.Y += -fRadius+bsg.fdY;
	ptNE.X +=  fRadius+bsg.fdX;		ptNE.Y += -fRadius+bsg.fdY;
	ptSW.X += -fRadius+bsg.fdX;		ptSW.Y +=  fRadius+bsg.fdY;
	ptSE.X +=  fRadius+bsg.fdX;		ptSE.Y +=  fRadius+bsg.fdY;

	bsg.fNW = InterpolatePixelValue(pBitmap, pMask, ptNW, true);
	bsg.fSW = InterpolatePixelValue(pBitmap, pMask, ptSW, true);
	bsg.fNE = InterpolatePixelValue(pBitmap, pMask, ptNE, true);
	bsg.fSE = InterpolatePixelValue(pBitmap, pMask, ptSE, true);

	double			fDiff = 0,
					fDiffPercent = 0;
	LONG			fNrDiff = 0;

	if (bsg.fNW>=0 && bsg.fSW>=0)
	{
		fDiff += fabs(bsg.fSW-bsg.fNW);
		fDiffPercent += fabs(bsg.fSW-bsg.fNW)/max(bsg.fSW, bsg.fNW)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNW>=0 && bsg.fNE>=0)
	{
		fDiff += fabs(bsg.fNW-bsg.fNE);
		fDiffPercent += fabs(bsg.fNW-bsg.fNE)/max(bsg.fNW, bsg.fNE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNE>=0 && bsg.fSE>=0)
	{
		fDiff += fabs(bsg.fNE-bsg.fSE);
		fDiffPercent += fabs(bsg.fNE-bsg.fSE)/max(bsg.fNE, bsg.fSE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fSE>=0 && bsg.fSW>=0)
	{
		fDiff += fabs(bsg.fSW-bsg.fSE);
		fDiffPercent += fabs(bsg.fSW-bsg.fSE)/max(bsg.fSW, bsg.fSE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNW>=0 && bsg.fSE>=0)
	{
		fDiff += fabs(bsg.fNW-bsg.fSE);
		fDiffPercent += fabs(bsg.fNW-bsg.fSE)/max(bsg.fNW, bsg.fSE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNE>=0 && bsg.fSW>=0)
	{
		fDiff += fabs(bsg.fNE-bsg.fSW);
		fDiffPercent += fabs(bsg.fNE-bsg.fSW)/max(bsg.fNE, bsg.fSW)*100.0;
		fNrDiff ++;
	};

	if (fNrDiff == 6)
	{
		bsg.fGradient = fDiff/fNrDiff;
		bsg.fPercentGradient = fDiffPercent/fNrDiff;
	}
	else
	{
		bsg.fGradient = -1.0;
		bsg.fPercentGradient = -1.0;
	};

	return bsg.fGradient;
};

/* ------------------------------------------------------------------- */

const	double			NOEDGEANGLE = -1000.0;

inline	double			GetEdgeAngle(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, double fX, double fY)
{
	double				fResult = NOEDGEANGLE;
	double				fValue[8];
	bool				bOk = true;

	//     0    1    2
	// 
	//     3         4
	//
	//     5    6    7

	fValue[0] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX-1.0, fY-1.0), true);
	fValue[1] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX    , fY-1.0), true);
	fValue[2] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX+1.0, fY-1.0), true);
	fValue[3] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX-1.0, fY    ), true);
	fValue[4] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX+1.0, fY    ), true);
	fValue[5] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX-1.0, fY+1.0), true);
	fValue[6] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX    , fY+1.0), true);
	fValue[7] = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX+1.0, fY+1.0), true);

	for (LONG i = 0;i<8 && bOk;i++)
		bOk = (fValue[i]>0);

	if (bOk)
	{
		// Compute Gx and Gy
		double			Gx, Gy;

		//  Gx matrix  
		//  -1   0   1
		//  -2   0   2
		//  -1   0   1
		Gx = -fValue[0]+fValue[2]-2.0*fValue[3]+2.0*fValue[4]-fValue[5]+fValue[7];

		//  Gy matrix  
		//  -1  -2  -1
		//   0   0   0
		//   1   2   1
		Gy = -fValue[0]-2.0*fValue[1]-fValue[2]+fValue[5]+2.0*fValue[6]+fValue[7];

		fResult = atan2(Gy, Gx)*360.0/M_PI;
	};

	return fResult;
};

inline double NormalizeAngle(double fAngle)
{
	while (fAngle<=-180)
		fAngle += 360.0;
	while (fAngle>180)
		fAngle -= 360.0;

	return fAngle;
};

void	CDeBloom::RefineStarCenter(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs)
{
	double				fMinimum = NOEDGEANGLE;
	double				fdX = 0,
						fdY = 0,
						fdYLeft = 0,
						fdYRight = 0;
	double				fndX = 0,
						fndY = 0;
	double				fX = bs.m_ptStar.X,
						fY = bs.m_ptStar.Y;
	bool				bFound = false;
	double				fOffsetX = 0.0,
						fOffsetY = 0.0;

	// Check that 2 bloomed stars are not side by side
	bool				bLeftBloomed = false,
						bRightBloomed = false;
	bool				bBloomCross = false;

	for (LONG i = floor(fX+0.5);i>=max(0.0, floor(fX+0.5-bs.m_fRadius*2.0)) && !bLeftBloomed;i--)
	{
		double			fMask;

		pMask->GetPixel(i, floor(fY+0.5), fMask);
		if (IsBloomedBorderValue(fMask))
			bBloomCross = true;
		else if (bBloomCross && IsBloomedValue(fMask))
			bLeftBloomed = true;
	};

	bBloomCross = false;
	for (LONG i = floor(fX+0.5);i<=min(static_cast<double>(m_lWidth-1), floor(fX+0.5+bs.m_fRadius*2.0)) && !bRightBloomed;i++)
	{
		double			fMask;

		pMask->GetPixel(i, floor(fY+0.5), fMask);
		if (IsBloomedBorderValue(fMask))
			bBloomCross = true;
		else if (bBloomCross && IsBloomedValue(fMask))
			bRightBloomed = true;
	};

	if (!bRightBloomed && !bLeftBloomed)
	{
		// Now try to find the correct X so that each left and right are the same (distance wise)
		bool					bEnd = false;
		double					fDistance = 1.0;
		int						lDirection = 0;

		fMinimum = -1.0;
		do
		{
			bEnd = true;
			double			fValueL1,
							fValueR1;
			double			fValueL2,
							fValueR2;

			fValueL1 = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX-fDistance+fndX, fY), true);
			fValueL2 = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX-fDistance-1+fndX, fY), true);
			fValueR1 = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX+fDistance+fndX, fY), true);
			fValueR2 = InterpolatePixelValue(pBitmap, pMask, CPointExt(fX+fDistance+1+fndX, fY), true);

			if  (fValueL1>0 && fValueL2>0 && fValueR1>0 && fValueR2>0 && 
				 fValueL1>fValueL2 && fValueR1>fValueR2)
			{
				double		fLeft, fRight;
				double		fGradient;

				fLeft  = (fValueL1+fValueL2)/2.0;
				fRight = (fValueR1+fValueR2)/2.0;
				fGradient = fabs(fLeft-fRight);

				if (fGradient<fMinimum || fMinimum<0)
				{
					fdX = fndX;
					fMinimum = fGradient;
				}
				if (fLeft > fRight)
				{
					// One step to the left
					bEnd = lDirection>0;
					fndX-=0.1;
					lDirection = -1;
				}
				else if (fLeft < fRight)
				{
					// one step to the right
					bEnd = lDirection<0;
					fndX+=0.1;
					lDirection = +1;
				}
				fDistance = 1.0;
			}
			else
			{
				fDistance++;
				bEnd = fDistance>bs.m_fRadius*2.0;
			};
		}
		while (!bEnd);

		fX = bs.m_ptStar.X + fdX;
		bs.m_ptStar.X = fX;
	};
};

/* ------------------------------------------------------------------- */

void	CDeBloom::RefineStarCenter2(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs)
{
	CBloomedStarGradient	bsg,
							bsgn;

	bsg.ptStar = bs.m_ptStar;
	bsgn = bsg;

	double				fStep = 0.8;
	double				fRadius = bs.m_fRadius;
	double				fMinimum = -1.0;
	bool				bEnd = true;

	for (double fdX = -5.0;fdX<=5.0;fdX+=0.2)
	{
		for (double fdY = -5.0;fdY<=5.0;fdY+=0.2)
		{
			bsgn.fdX = fdX;
			bsgn.fdY = fdY;

			double		fGradient;

			fGradient = ComputeStarGradient(pBitmap, pMask, bsgn, fRadius);
			if (fGradient>0 && (fGradient<fMinimum || fMinimum<0))
			{
				bsg = bsgn;
				fMinimum = fGradient;
			};
		};
	};

	bs.m_ptStar.X += bsg.fdX;
	bs.m_ptStar.Y += bsg.fdY;
};

/* ------------------------------------------------------------------- */

void	CDeBloom::ComputeStarCenter(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs)
{
	LONG				i;
	double				//fAverageX = 0,
						fdAverageY = 0;
	std::vector<double>	vValues;
	double				fRadius;
	double				fX = bs.m_ptStar.X;
	double				fY = bs.m_ptStar.Y;


	fRadius=bs.m_fRadius;
	vValues.reserve(fRadius*2+1);


	LONG				lNrColumns = 0;
	std::vector<double>	vYCenters;

	for (i = max(0.0, fX-fRadius-0.5);i<=min(static_cast<double>(m_lWidth-1), fX+fRadius+0.5);i++)
	{
		bool			bBloomed = false;
		double			fLocalMaximum = -1.0;
		double			fdYMaximum = -1.0;
		double			fdY;

		vValues.clear();
		for (fdY = -fRadius;fdY<=fRadius && !bBloomed;fdY+=0.1)
		{
			double			fValue;

			fValue = InterpolatePixelValue(pBitmap, pMask, CPointExt(i, fY+fdY), true);

			if (fValue>0)
			{
				if (fValue>fLocalMaximum)
				{
					fdYMaximum = fdY;
					fLocalMaximum = fValue;
				};
			}
			else
				bBloomed = true;
		};

		if (!bBloomed && fLocalMaximum>0)
			vYCenters.push_back(fdYMaximum);
	};
	fdAverageY = Median(vYCenters);	
	
	bs.m_ptStar.Y += fdAverageY;

	RefineStarCenter(pBitmap, pMask, bs);

	fX = bs.m_ptStar.X;
	fY = bs.m_ptStar.Y;

	bool				bFound = false;
	double				fRadiusLeft  = -1.0,
						fRadiusRight = -1.0;
	double				fBloomLeft   = -1.0,
						fBloomRight  = -1.0;
	double				fBloom;

	//	|     |     |     |     |     |     |     |     |     
	//     i                  x
	//     3     2     1     0     1     2     3
	// i base 0

	/*
	r = sqrt(-i*i/(2*log(f1/b))
	r = sqrt(-(i+1)*(i+1)/(2*log(f2/b))

	i*i/log(f1/b) = (i+1)*(i+1)/log(f2/b)
	
	log(f2/b) * (i*i) = log(f1/b) *((i+1)*(i+1))

	h = i*i/((i+1)/(i+1)

	log(f2/b) * h = log(f1/b)

	h*log(f2)-h*log(b) = log(f1)-log(b)
	log(b) = (log(f1)-h*log(f2))/(1-h)

	b = exp((log(f1)-h*log(f2))/(1-h))*/


	for (i = 1;i<=fRadius+3.0 && !bFound;i++)
	{
		double			fValueL1,
						fValueR1;
		double			x1L, x2L,
						x1R, x2R;

		x1L = fX-i;
		x1R = fX+i;

		fValueL1 = InterpolatePixelValue(pBitmap, pMask, CPointExt(max(0.0, x1L), fY), true)-m_fBackground;
		fValueR1 = InterpolatePixelValue(pBitmap, pMask, CPointExt(max(0.0, x1R), fY), true)-m_fBackground;
		
		if (fValueL1 > 0 && fValueR1 > 0)
		{
			double			fValueL2,
							fValueR2;

			x2L = x1L-1.0;
			x2R = x1R+1.0;

			fValueL2 = InterpolatePixelValue(pBitmap, pMask, CPointExt(max(0.0, x2L), fY), true)-m_fBackground;
			fValueR2 = InterpolatePixelValue(pBitmap, pMask, CPointExt(max(0.0, x2R), fY), true)-m_fBackground;

			if (fValueL2>0 && fValueR2>0 && (fValueL1>fValueL2) && (fValueR1>fValueR2))
			{
				double		d1, d2;

				d1 = fabs(x1L-fX);
				d2 = fabs(x2L-fX);

				fRadiusLeft = sqrt(-(d2*d2-d1*d1)/2.0/log(fValueL2/fValueL1));
				fBloomLeft  = fValueL1/exp(-d1*d1/2.0/(fRadiusLeft*fRadiusLeft));

				d1 = fabs(x1R-fX);
				d2 = fabs(x2R-fX);

				fRadiusRight = sqrt(-(d2*d2-d1*d1)/2.0/log(fValueR2/fValueR1));
				fBloomRight  = fValueR1/exp(-d1*d1/2.0/(fRadiusRight*fRadiusRight));

				fRadius = (fRadiusLeft+ fRadiusRight)/2.0;
				fBloom  = (fBloomLeft+ fBloomRight)/2.0;

				bFound = true;
			};
		};
	};

	std::vector<double>			vAngles;

	vAngles.push_back(0);
	vAngles.push_back(45);
	vAngles.push_back(135);
	vAngles.push_back(180);
	vAngles.push_back(225);
	vAngles.push_back(315);

	for (LONG a = 0;a<vAngles.size();a++)
	{
		bFound = false;
		for (i = 1;i<=fRadius*2.0 && !bFound;i++)
		{
			double				fValue1,
								fValue2;
			CPointExt			pt1,
								pt2;

			pt1.X = fX + (double)i*cos(vAngles[a]*M_PI/180.0);
			pt1.Y = fY + (double)i*sin(vAngles[a]*M_PI/180.0);
			pt2.X = fX + (double)(i+1)*cos(vAngles[a]*M_PI/180.0);
			pt2.Y = fY + (double)(i+1)*sin(vAngles[a]*M_PI/180.0);

			fValue1 = InterpolatePixelValue(pBitmap, pMask, pt1, true)-m_fBackground;
			fValue2 = InterpolatePixelValue(pBitmap, pMask, pt2, true)-m_fBackground;

			if (fValue1 > 0 && fValue2 > 0 && fValue1 > fValue2)
			{
				double			d1,
								d2;
				CBloomInfo		bi;

				d1 = distance(fX, fY, pt1.X, pt1.Y);
				d2 = distance(fX, fY, pt2.X, pt2.Y);

				bi.m_fAngle  = vAngles[a];
				bi.m_fRadius = sqrt(-(d2*d2-d1*d1)/2.0/log(fValue2/fValue1));
				bi.m_fBloom  = fValue1/exp(-d1*d1/2.0/(bi.m_fRadius*bi.m_fRadius));
				bi.m_ptRef.X = (pt1.X + pt2.X)/2.0;
				bi.m_ptRef.Y = (pt1.Y + pt2.Y)/2.0;

				bs.m_vBlooms.push_back(bi);
				bFound = true;
			};
		};
	};

	bs.m_fRadius  = fRadius;
	bs.m_fBloom   = fBloom;
};

/* ------------------------------------------------------------------- */

void	CDeBloom::MarkBloomBorder(CMemoryBitmap * pMask, LONG x, LONG y, std::vector<CPointExt> & vBorders)
{
	double						fMask;

	if (x>=0 && x<m_lWidth && y>=0 && y<m_lHeight)
	{
		pMask->GetPixel(x, y, fMask);
		if (fMask<100)
		{
			pMask->SetPixel(x, y, 140.0);
			vBorders.push_back(CPointExt(x, y));
		};
	};
};

/* ------------------------------------------------------------------- */

void	CDeBloom::MarkBorderAsBloomed(CMemoryBitmap * pMask, LONG x, LONG y, std::vector<CPoint> & vBloomed)
{
	if (x>=0 && x<m_lWidth && y>=0 && y<m_lHeight)
	{
		bool					bBloomed = true;
		std::vector<CPointExt>	vTests;

		vTests.push_back(CPointExt(x-1, y-1));
		vTests.push_back(CPointExt(x-1, y-0));
		vTests.push_back(CPointExt(x-1, y+1));
		vTests.push_back(CPointExt(x-0, y-1));
		vTests.push_back(CPointExt(x-0, y+1));
		vTests.push_back(CPointExt(x+1, y-1));
		vTests.push_back(CPointExt(x+1, y-0));
		vTests.push_back(CPointExt(x+1, y+1));

		for (LONG i = 0;i<vTests.size() && bBloomed;i++)
		{
			double				fMask;

			pMask->GetPixel(vTests[i].X, vTests[i].Y, fMask);
			bBloomed = fMask>0;
		};

		if (bBloomed)
		{
			pMask->SetPixel(x, y, 255.0);
			vBloomed.push_back(CPoint(x, y));
		};
	};
};

/* ------------------------------------------------------------------- */

void	CDeBloom::ExpandBloomedArea(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, LONG x, LONG y)
{
	bool						bEnd = false;
	std::vector<CPoint>			vBloomed;
	LONG						lLargestY = y;
	LONG						lTopY = 0;
	LONG						lLargestWidth = 0;
	LONG						lBloomHeight = 0;
	CBloomedStar				bs;

	// Since it started at the bottom the bloomed are can only go up...well normally
	// So go down a little to get everything that is above 90% of the threshold
	do
	{
		double				fGray,
							fMask;

		bEnd = true;
		if (y<m_lHeight-1)
		{
			pBitmap->GetPixel(x, y+1, fGray);
			pMask->GetPixel(x, y+1, fMask);

			if ((fGray >= 256.0*m_fBloomThreshold*0.90) &&
				!IsBloomedValue(fMask) && !IsBloomedBorderValue(fMask))
			{
				y = y+1;
				bEnd = false;
			};
		};

	}
	while (!bEnd);
	bEnd = false;
	//
	for (LONG j = y;j>=0 && !bEnd;j--)
	{
		double				fGray;
		double				fMask;
		bool				bEndRow = false;
		LONG				lMinX = x,
							lMaxX = x;
		bool				bEndLeft = true;
		bool				bEndRight = true;

		bEnd = true;
		// Expand horizontally to the left
		for (LONG i = x;i>=0 && !bEndRow;i--)
		{
			pBitmap->GetPixel(i, j, fGray);
			if (fGray >= 256.0*m_fBloomThreshold*0.90)
			{
				pMask->GetPixel(i, j, fMask);
				if (!IsBloomedValue(fMask))
				{
					lMinX = i;
					bEnd = false;
					bEndLeft = false;
				}
				else
					bEndRow = true;
			}
			else
				bEndRow = true;
		};
		// Expand horizontally to the right
		bEndRow = false;
		for (LONG i = x;i<m_lWidth && !bEndRow;i++)
		{
			pBitmap->GetPixel(i, j, fGray);
			if (fGray >= 256.0*m_fBloomThreshold*0.90)
			{
				pMask->GetPixel(i, j, fMask);
				if (!IsBloomedValue(fMask))
				{
					lMaxX = i;
					bEnd = false;
					bEndRight = false;
				}
				else
					bEndRow = true;
			}
			else
				bEndRow = true;
		};

		// Mark the mask as being done
		if (!bEnd)
		{
			lBloomHeight++;
			lTopY = j;

			// Check one pixel to the left to expand the mask (edge condition)
			if (!bEndLeft && lMinX>1)
			{
				if (IsLeftEdge(pBitmap, lMinX, j))
					lMinX--;
			};

			// Check one pixel to the right to expand the mask
			if (!bEndRight && lMaxX<m_lWidth-2)
			{
				if (IsRightEdge(pBitmap, lMaxX, j))
					lMaxX++;
			};

			vBloomed.reserve(vBloomed.size()+lMaxX-lMinX+1);
			for (LONG i = lMinX;i<=lMaxX;i++)
			{
				pMask->SetPixel(i, j, 255.0);
				vBloomed.push_back(CPoint(i, j));
			};

			// Try to expand one pixel to the bottom
			if ((j==y) && (j<m_lHeight-1))
			{
				bool			bExtraHeight = false;

				for (LONG i = lMinX;i<=lMaxX;i++)
				{
					if (IsBottomEdge(pBitmap, i, j))
					{
						pMask->SetPixel(i, j+1, 255.0);
						vBloomed.push_back(CPoint(i, j+1));
						bExtraHeight=true;
					};
				};
				if (bExtraHeight)
					lBloomHeight++;
			};

			if (lLargestWidth < (lMaxX-lMinX+1))
			{
				lLargestWidth = lMaxX-lMinX+1;
				lLargestY = j;
				bs.m_ptStar.X = (lMaxX+lMinX)/2.0;
				bs.m_ptStar.Y = lLargestY;
			};
		};
	};

	// Try to expand one pixel to the top
	if (lTopY > 1)
	{
		if (IsTopEdge(pBitmap, x, lTopY))
		{
			pMask->SetPixel(x, lTopY-1, 255.0);
			vBloomed.push_back(CPoint(x, lTopY-1));
			lBloomHeight++;
			lTopY--;
		};
	};

	if ((lBloomHeight>2) && (lLargestWidth>1) && (lBloomHeight>lLargestWidth))
	{
		// Mark the possible star
		double					fRadius = lLargestWidth/2.0+3.0;
		std::vector<CPointExt>	vBorders;

		//ComputeStarCenter(ptStar.X, ptStar.Y, fRadius);

		// Mark bloom area borders
		for (LONG i = 0;i<vBloomed.size();i++)
		{
			MarkBloomBorder(pMask, vBloomed[i].x-1, vBloomed[i].y-1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x  , vBloomed[i].y-1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x+1, vBloomed[i].y-1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x-1, vBloomed[i].y  , vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x+1, vBloomed[i].y  , vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x-1, vBloomed[i].y+1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x  , vBloomed[i].y+1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x+1, vBloomed[i].y+1, vBorders);
		};

		// And mark lonely borders as bloomed
		for (LONG i = 0;i<vBorders.size();i++)
			MarkBorderAsBloomed(pMask, vBorders[i].X, vBorders[i].Y, vBloomed);

		bs.m_fRadius = fRadius;
		bs.m_vBloomed  = vBloomed;
		m_vBloomedStars.push_back(bs);
	}
	else
	{
		// Reset the bloomed area
		for (LONG i = 0;i<vBloomed.size();i++)
			pMask->SetPixel(vBloomed[i].x, vBloomed[i].y, 0.0);
	};
};

/* ------------------------------------------------------------------- */

bool	CDeBloom::CreateMask(CMemoryBitmap * pBitmap, C8BitGrayBitmap ** ppMask)
{
	CSmartPtr<C8BitGrayBitmap>		pMask;

	*ppMask = NULL;
	if (pBitmap && pBitmap->IsMonochrome() && !pBitmap->IsCFA())
	{
		m_lWidth  = pBitmap->Width();
		m_lHeight = pBitmap->Height();

		pMask.Create();
		pMask->Init(m_lWidth, m_lHeight);

		m_fBackground = ComputeBackgroundValue(pBitmap);

		if (m_pProgress)
			m_pProgress->Start2(NULL, m_lHeight);

		// Start at the bottom
		for (LONG j = m_lHeight-1;j>=0;j--)
		{
			for (LONG i = 0;i<m_lWidth;i++)
			{
				double				fGray;
				double				fMask;

				pBitmap->GetPixel(i, j, fGray);
				if (fGray >= 256.0*m_fBloomThreshold)
				{
					pMask->GetPixel(i, j, fMask);
					if (!IsBloomedValue(fMask))
						ExpandBloomedArea(pBitmap, pMask, i, j);
				};
			};
			if (m_pProgress)
				m_pProgress->Progress2(NULL, j+1);
		};

		if (m_pProgress)
			m_pProgress->End2();

		pMask.CopyTo(ppMask);
	};

#ifdef DEBUGDEBLOOM
	WriteTIFF("E:\\BloomMask.tif", pMask, NULL, NULL);
	WriteTIFF("E:\\BloomImage.tif", pBitmap, NULL, NULL);
#endif

	return m_vBloomedStars.size()>0;
};

/* ------------------------------------------------------------------- */

double	CDeBloom::ComputeValue(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, LONG x, LONG y, bool & bDone)
{
	double						fResult = 255.0;
	double						fSum = 0,
								fWeight = 0;
	bDone = false;

	for (LONG i = max(0L, x-5);i<=min(m_lWidth-1, x+5);i++)
	{
		for (LONG j = max(0L, y-3);j<=min(m_lHeight-1, y+3);j++)
		{
			double				fValue,
								fMask;

			pMask->GetPixel(i, j, fMask);
			if (!IsBloomedValue(fMask) && !IsBloomedBorderValue(fMask))
			{
				double			fDistance = 1.0/(1.0+(double)fabs((double)i-x)+(j-y)*(j-y));

				pBitmap->GetPixel(i, j, fValue);
				fSum += fValue*fDistance;
				fWeight += fDistance;
			};
		};
	};

	if (fWeight)
	{
		fResult = fSum/fWeight;
		bDone = true;
	};

	return fResult;
};

/* ------------------------------------------------------------------- */

void	CDeBloom::AddStar(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs)
{
	double					fFactor1;

	fFactor1 = 2.0*bs.m_fRadius*bs.m_fRadius;

	for (LONG i = 0;i<bs.m_vBloomed.size();i++)
	{
		double			fMask,
						fValue,
						fValue3,
						fValue4;

		pMask->GetPixel(bs.m_vBloomed[i].x, bs.m_vBloomed[i].y, fMask);
		if (IsBloomedValue(fMask))
		{
			double		fDistance,
						fBaseValue;
			double		fAverage = -1.0;
			CPointExt	pt(bs.m_vBloomed[i].x+0.5, bs.m_vBloomed[i].y+0.5);

			fDistance = distance(pt.X, pt.Y, bs.m_ptStar.X, bs.m_ptStar.Y);

			fValue3 = InterpolatePixelValue(pBitmap, pMask, CPointExt(bs.m_ptStar.X-fDistance, bs.m_ptStar.Y), true);
			fValue4 = InterpolatePixelValue(pBitmap, pMask, CPointExt(bs.m_ptStar.X+fDistance, bs.m_ptStar.Y), true);

			if (fValue3 > 0 && fValue4 > 0)
				fAverage = min(fValue3,fValue4);
			else if (fValue3 > 0)
				fAverage = fValue3;
			else if (fValue4 > 0)
				fAverage = fValue4;

			double		fBloomValue = 0.0;
			double		fBloomWeight = 0.0;
			for (LONG a = 0;a<bs.m_vBlooms.size();a++)
			{
				double	fBloomDistance;

				fBloomDistance = distance(pt.X, pt.Y, bs.m_vBlooms[a].m_ptRef.X, bs.m_vBlooms[a].m_ptRef.X);
				fBloomWeight += 1.0/(fBloomDistance+1.0);

				fFactor1 = 2.0*pow(bs.m_vBlooms[a].m_fRadius, 2);
				fValue = m_fBackground+exp(-(fDistance * fDistance)/fFactor1)*bs.m_vBlooms[a].m_fBloom;

				fBloomValue += fValue/(fBloomDistance+1.0);
			};

			if (fBloomWeight)
				fBloomValue /= fBloomWeight;

			//fValue1 = m_fBackground + exp(-(fDistance * fDistance)/fFactor1)*bs.m_fBloom;
			//fValue2 = m_fBackground + exp(-(fDistance * fDistance)/fFactor2)*bs.m_fBloom2;

			pBitmap->GetPixel(bs.m_vBloomed[i].x, bs.m_vBloomed[i].y, fBaseValue);

			fValue = fBaseValue;
			if (fAverage>0 && fBloomValue>0)
				fBloomValue = min(fAverage, fBloomValue);
				
			if (fBloomValue)
				fValue = max(fBaseValue, fBloomValue);
			/*
			if (fAverage>0)
			{
				double			fRatio = 1.0/(1.0+fDistance*fDistance);
				fValue = fValue*(1.0-fRatio)+fAverage*fRatio;
			};*/

			pBitmap->SetPixel(bs.m_vBloomed[i].x, bs.m_vBloomed[i].y, min(255.0, fValue));
		};
	};
};

/* ------------------------------------------------------------------- */

void    CDeBloom::SmoothMaskBorders(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask)
{
	std::vector<double>		vValues;

	vValues.resize(8);

	for (LONG i = 1;i<m_lWidth-1;i++)
	{
		for (LONG j = 1;j<m_lHeight-1;j++)
		{
			double				fMask;

			pMask->GetPixel(i, j, fMask);
			if (IsBloomedBorderValue(fMask))
			{
				// Interpolate pixel
				double			fValue;

				pBitmap->GetPixel(i-1, j-1, vValues[0]);
				pBitmap->GetPixel(i-0, j-1, vValues[1]);
				pBitmap->GetPixel(i+1, j-1, vValues[2]);
				pBitmap->GetPixel(i-1, j-0, vValues[3]);
				pBitmap->GetPixel(i+1, j-0, vValues[4]);
				pBitmap->GetPixel(i-1, j+1, vValues[5]);
				pBitmap->GetPixel(i-0, j+1, vValues[6]);
				pBitmap->GetPixel(i+1, j+1, vValues[7]);

				fValue = Median(vValues);
				pBitmap->SetPixel(i, j, fValue);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

double	CDeBloom::ComputeBackgroundValue(CMemoryBitmap * pBitmap)
{
	double					fResult = 0.0;
	CBackgroundCalibration	BackgroundCalibration;

	BackgroundCalibration.m_BackgroundCalibrationMode = BCM_PERCHANNEL;
	BackgroundCalibration.m_BackgroundInterpolation   = BCI_LINEAR;
	BackgroundCalibration.ComputeBackgroundCalibration(pBitmap, TRUE, m_pProgress);
	fResult = BackgroundCalibration.m_fTgtRedBk/256.0;

	return fResult;
};

/* ------------------------------------------------------------------- */

void	CDeBloom::DeBloom(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask)
{
	ZFUNCTRACE_RUNTIME();
	// First compute background value
	m_fBackground = ComputeBackgroundValue(pBitmap);

	// Apply Median Filter to smooth the noise
	{
		//CMedianImageFilter			filter;
		//CSmartPtr<CMemoryBitmap>	pFiltered;

		//filter.ApplyFilter(pBitmap, &pFiltered, m_pProgress);

		if (m_pProgress)
			m_pProgress->Start2(NULL, (LONG)m_vBloomedStars.size());

		for (LONG i = 0;i<m_vBloomedStars.size();i++)
		{
			if (m_pProgress)
				m_pProgress->Progress2(NULL, i+1);
			ComputeStarCenter(pBitmap, pMask, m_vBloomedStars[i]);
		};

		if (m_pProgress)
			m_pProgress->End2();
	};

	if (m_pProgress)
		m_pProgress->Start2(NULL, m_lWidth);
	
	std::vector<CPoint>			vUnprocessed;
	std::vector<CPoint>			vProcessed;

	for (LONG i = 0;i<m_lWidth;i++)
	{
		for (LONG j = 0;j<m_lHeight;j++)
		{
			double					fMask;

			pMask->GetPixel(i, j, fMask);
			if (IsBloomedValue(fMask))
			{
				double				fValue;
				bool				bDone;

				fValue = ComputeValue(pBitmap, pMask, i, j, bDone);

				if (bDone)
				{
					pBitmap->SetPixel(i, j, fValue);
					vProcessed.push_back(CPoint(i, j));
				}
				else
				{
					// the coordinates so that they can be processed later on
					vUnprocessed.push_back(CPoint(i, j));
				};
			};
		};
		if (m_pProgress)
			m_pProgress->Progress2(NULL, i+1);
	};

	// Process recursively unprocessed
	LONG					lNrUnprocessed = 0;

	if (vUnprocessed.size())
	{
		std::vector<CPoint>			vNewlyProcessed = vProcessed;

		while (vUnprocessed.size() && (vUnprocessed.size() != lNrUnprocessed))
		{
			for (LONG i = 0;i<vNewlyProcessed.size();i++)
				pMask->SetPixel(vNewlyProcessed[i].x, vNewlyProcessed[i].y, 190.0);
			vNewlyProcessed.clear();

			lNrUnprocessed = (LONG)vUnprocessed.size();
			
			std::vector<CPoint>			vToProcess = vUnprocessed;

			vUnprocessed.clear();

			for (LONG i = 0;i<vToProcess.size();i++)
			{
				double				fValue;
				bool				bDone;

				fValue = ComputeValue(pBitmap, pMask, vToProcess[i].x, vToProcess[i].y, bDone);

				if (bDone)
				{
					pBitmap->SetPixel(vToProcess[i].x, vToProcess[i].y, fValue);
					vProcessed.push_back(vToProcess[i]);
					vNewlyProcessed.push_back(vToProcess[i]);
				}
				else
				{
					// the coordinates so that they can be processed later on
					vUnprocessed.push_back(vToProcess[i]);
				};
			};
		};

		for (LONG i = 0;i<vProcessed.size();i++)
			pMask->SetPixel(vProcessed[i].x, vProcessed[i].y, 255.0);
	};


	if (m_pProgress)
		m_pProgress->End2();

#ifdef DEBUGDEBLOOM
	WriteTIFF("E:\\BloomImage_Step1.tif", pBitmap, NULL, NULL);
#endif

	for (LONG i = 0;i<m_vBloomedStars.size();i++)
	{
		AddStar(pBitmap, pMask, m_vBloomedStars[i]);
	};

	SmoothMaskBorders(pBitmap, pMask);
#ifdef DEBUGDEBLOOM
	WriteTIFF("E:\\BloomImage_Step2.tif", pBitmap, NULL, NULL);
#endif
};

/* ------------------------------------------------------------------- */

bool	CDeBloom::CreateBloomMask(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	m_pProgress = pProgress;
	m_pMask.Release();
	return CreateMask(pBitmap, &m_pMask);
};

/* ------------------------------------------------------------------- */

void	CDeBloom::DeBloomImage(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
{
	m_pProgress = pProgress;
	if (m_pMask)
		DeBloom(pBitmap, m_pMask);
};

/* ------------------------------------------------------------------- */
