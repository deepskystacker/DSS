#include <stdafx.h>
#include "DeBloom.h"
#include "GrayBitmap.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "BackgroundCalibration.h"
#include "Ztrace.h"

using namespace DSS;

#ifdef DSSBETA
//#define DEBUGDEBLOOM
#endif

/* ------------------------------------------------------------------- */

inline bool IsBloomedValue(double fValue)
{
	return fValue > 200.0;
}

inline bool IsBloomedBorderValue(double fValue)
{
	return (fValue < 150.0) && (fValue > 0.0);
}

inline bool IsBloomedProcessedValue(double fValue)
{
	return (fValue > 150.0) && (fValue < 200.0);
}


inline bool IsEdge(const double pfGray[])
{
	const double fGradient0 = std::abs(pfGray[1] - pfGray[0]) / 256.0;
	const double fGradient1 = std::abs(pfGray[2] - pfGray[0]) / 256.0;
	const double fGradient2 = std::abs(pfGray[3] - pfGray[1]) / 256.0;
	// const double fGradient3 = std::abs(pfGray[4] - pfGray[2]) / 256.0;

	const double fEdge = (fGradient2 - fGradient0) / 2.0 / (fGradient0 - 2.0 * fGradient1 + fGradient2) + 2;

	return (fGradient1 > fGradient0 && fGradient1 > fGradient2 && fEdge > -0.3 && fGradient1 > 0.7);
}


bool CDeBloom::IsLeftEdge(CMemoryBitmap * pBitmap, int x, int y)
{
	double			fGray[5];

	pBitmap->GetPixel(std::max(0, x-0), y, fGray[0]); // Current Pixel
	pBitmap->GetPixel(std::max(0, x-1), y, fGray[1]); //
	pBitmap->GetPixel(std::max(0, x-2), y, fGray[2]);
	pBitmap->GetPixel(std::max(0, x-3), y, fGray[3]);
	pBitmap->GetPixel(std::max(0, x-4), y, fGray[4]);

	for (size_t i = 0; i < 5; i++)
		fGray[i] = (fGray[i] - m_fBackground) / (256.0 - m_fBackground) * 256.0;

	return IsEdge(fGray);
}


bool CDeBloom::IsRightEdge(CMemoryBitmap * pBitmap, int x, int y)
{
	double			fGray[5];

	pBitmap->GetPixel(std::min(m_lWidth-1, x+0), y, fGray[0]); // Current Pixel
	pBitmap->GetPixel(std::min(m_lWidth-1, x+1), y, fGray[1]); // Current Pixel
	pBitmap->GetPixel(std::min(m_lWidth-1, x+2), y, fGray[2]); // Current Pixel
	pBitmap->GetPixel(std::min(m_lWidth-1, x+3), y, fGray[3]); // Current Pixel
	pBitmap->GetPixel(std::min(m_lWidth-1, x+4), y, fGray[4]); // Current Pixel

	for (size_t i = 0; i < 5; i++)
		fGray[i] = (fGray[i] - m_fBackground) / (256.0 - m_fBackground) * 256.0;

	return IsEdge(fGray);
}


bool	CDeBloom::IsTopEdge(CMemoryBitmap * pBitmap, int x, int y)
{
	double			fGray[5];

	pBitmap->GetPixel(x, std::max(0, y-0), fGray[0]); // Current Pixel
	pBitmap->GetPixel(x, std::max(0, y-1), fGray[1]); // Current Pixel
	pBitmap->GetPixel(x, std::max(0, y-2), fGray[2]); // Current Pixel
	pBitmap->GetPixel(x, std::max(0, y-3), fGray[3]); // Current Pixel
	pBitmap->GetPixel(x, std::max(0, y-4), fGray[4]); // Current Pixel

	for (size_t i = 0; i < 5; i++)
		fGray[i] = (fGray[i] - m_fBackground) / (256.0 - m_fBackground) * 256.0;

	return IsEdge(fGray);
}


bool CDeBloom::IsBottomEdge(CMemoryBitmap * pBitmap, int x, int y)
{
	double			fGray[5];

	pBitmap->GetPixel(x, std::min(m_lHeight-1, y+0), fGray[0]); // Current Pixel
	pBitmap->GetPixel(x, std::min(m_lHeight-1, y+1), fGray[1]); // Current Pixel
	pBitmap->GetPixel(x, std::min(m_lHeight-1, y+2), fGray[2]); // Current Pixel
	pBitmap->GetPixel(x, std::min(m_lHeight-1, y+3), fGray[3]); // Current Pixel
	pBitmap->GetPixel(x, std::min(m_lHeight-1, y+4), fGray[4]); // Current Pixel

	for (size_t i = 0; i < 5; i++)
		fGray[i] = (fGray[i] - m_fBackground) / (256.0 - m_fBackground) * 256.0;

	return IsEdge(fGray);
}


inline double ComputeCenter(std::vector<double> & vValues)
{
	double fResult = -1.0;
	int i;
	bool bEmptyValues = false;
	int lStartEmpty = -1, lEndEmpty = -1;
	double fSum = 0, fNrValues = 0;

	for (i = 0; i < vValues.size(); i++)
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
		int				lNrLeftKeptValues,
							lNrRightKeptValues,
							lNrKeptValues;

		lNrLeftKeptValues  = lStartEmpty-1;
		lNrRightKeptValues = (int)vValues.size()-lEndEmpty;
		lNrKeptValues = std::min(lNrLeftKeptValues, lNrRightKeptValues);

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

static double	InterpolatePixelValue(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, QPointF pt, bool bNoBloom = false)
{
	int				x0 = floor(pt.x() - 0.5), x1 = 1 + x0,
					y0 = floor(pt.y() - 0.5), y1 = 1 + y0;

	int				width = pBitmap->Width(),
						height = pBitmap->Height();


	double				fd00 = distance(x0+0.5, y0+0.5, pt.x(), pt.y()),
						fd10 = distance(x0+1.5, y0+0.5, pt.x(), pt.y()),
						fd01 = distance(x0+0.5, y0+1.5, pt.x(), pt.y()),
						fd11 = distance(x0+1.5, y0+1.5, pt.x(), pt.y());

	double				fv00 = -1.0,
						fv10 = -1.0,
						fv01 = -1.0,
						fv11 = -1.0;
	double				fMask = -1.0;

	bool				bBloom = false;

	//
	// Make sure we ask for a valid pixel's value (i.e. one that is in the image)
	//
	if (x0 >= 0 && x0 < (width - 1) && y0 >= 0 && y0 < (height - 1))
	{
		pMask->GetPixel(x0, y0, fMask);
		if (!IsBloomedValue(fMask))
			pBitmap->GetPixel(x0, y0, fv00);
		else
			bBloom = true;
	}

	//
	// Make sure we ask for a valid pixel's value 
	//
	if (x0 >= 0 && x0 < (width - 1) && y1 >= 0 && y1 < (height - 1))
	{
		pMask->GetPixel(x0, y1, fMask);
		if (!IsBloomedValue(fMask))
			pBitmap->GetPixel(x0, y1, fv01);
		else
			bBloom = true;
	}

	//
	// Make sure we ask for a valid pixel's value 
	//
	if (x1 >= 0 && x1 < (width - 1) && y0 >= 0 && y0 < (height - 1))
	{
		pMask->GetPixel(x1, y0, fMask);
		if (!IsBloomedValue(fMask))
			pBitmap->GetPixel(x1, y0, fv10);
		else
			bBloom = true;
	}

	//
	// Make sure we ask for a valid pixel's value 
	//
	if (x1 >= 0 && x1 < (width - 1) && y1 >= 0 && y1 < (height - 1))
	{
		pMask->GetPixel(x1, y1, fMask);
		if (!IsBloomedValue(fMask))
			pBitmap->GetPixel(x1, y1, fv11);
		else
			bBloom = true;
	}

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
	QPointF			ptNW,
						ptSW,
						ptNE,
						ptSE;

	ptNW = ptSW = ptNE = ptSE = bsg.ptStar;

	ptNW.rx() += -fRadius+bsg.fdX;		ptNW.ry() += -fRadius+bsg.fdY;
	ptNE.rx() +=  fRadius+bsg.fdX;		ptNE.ry() += -fRadius+bsg.fdY;
	ptSW.rx() += -fRadius+bsg.fdX;		ptSW.ry() +=  fRadius+bsg.fdY;
	ptSE.rx() +=  fRadius+bsg.fdX;		ptSE.ry() +=  fRadius+bsg.fdY;

	bsg.fNW = InterpolatePixelValue(pBitmap, pMask, ptNW, true);
	bsg.fSW = InterpolatePixelValue(pBitmap, pMask, ptSW, true);
	bsg.fNE = InterpolatePixelValue(pBitmap, pMask, ptNE, true);
	bsg.fSE = InterpolatePixelValue(pBitmap, pMask, ptSE, true);

	double			fDiff = 0,
					fDiffPercent = 0;
	int			fNrDiff = 0;

	if (bsg.fNW>=0 && bsg.fSW>=0)
	{
		fDiff += fabs(bsg.fSW-bsg.fNW);
		fDiffPercent += fabs(bsg.fSW-bsg.fNW)/std::max(bsg.fSW, bsg.fNW)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNW>=0 && bsg.fNE>=0)
	{
		fDiff += fabs(bsg.fNW-bsg.fNE);
		fDiffPercent += fabs(bsg.fNW-bsg.fNE)/std::max(bsg.fNW, bsg.fNE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNE>=0 && bsg.fSE>=0)
	{
		fDiff += fabs(bsg.fNE-bsg.fSE);
		fDiffPercent += fabs(bsg.fNE-bsg.fSE)/std::max(bsg.fNE, bsg.fSE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fSE>=0 && bsg.fSW>=0)
	{
		fDiff += fabs(bsg.fSW-bsg.fSE);
		fDiffPercent += fabs(bsg.fSW-bsg.fSE)/std::max(bsg.fSW, bsg.fSE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNW>=0 && bsg.fSE>=0)
	{
		fDiff += fabs(bsg.fNW-bsg.fSE);
		fDiffPercent += fabs(bsg.fNW-bsg.fSE)/std::max(bsg.fNW, bsg.fSE)*100.0;
		fNrDiff ++;
	};
	if (bsg.fNE>=0 && bsg.fSW>=0)
	{
		fDiff += fabs(bsg.fNE-bsg.fSW);
		fDiffPercent += fabs(bsg.fNE-bsg.fSW)/std::max(bsg.fNE, bsg.fSW)*100.0;
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

	fValue[0] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX-1.0, fY-1.0), true);
	fValue[1] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX    , fY-1.0), true);
	fValue[2] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX+1.0, fY-1.0), true);
	fValue[3] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX-1.0, fY    ), true);
	fValue[4] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX+1.0, fY    ), true);
	fValue[5] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX-1.0, fY+1.0), true);
	fValue[6] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX    , fY+1.0), true);
	fValue[7] = InterpolatePixelValue(pBitmap, pMask, QPointF(fX+1.0, fY+1.0), true);

	for (int i = 0;i<8 && bOk;i++)
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
	double fdX = 0;
	double				fndX = 0;
						// fndY = 0;
	double				fX = bs.m_ptStar.x(),
						fY = bs.m_ptStar.y();

	// Check that 2 bloomed stars are not side by side
	bool				bLeftBloomed = false,
						bRightBloomed = false;
	bool				bBloomCross = false;

	for (int i = floor(fX+0.5);i>=std::max(0.0, floor(fX+0.5-bs.m_fRadius*2.0)) && !bLeftBloomed;i--)
	{
		double			fMask;

		pMask->GetPixel(i, floor(fY+0.5), fMask);
		if (IsBloomedBorderValue(fMask))
			bBloomCross = true;
		else if (bBloomCross && IsBloomedValue(fMask))
			bLeftBloomed = true;
	};

	bBloomCross = false;
	for (int i = floor(fX+0.5);i<=std::min(static_cast<double>(m_lWidth-1), floor(fX+0.5+bs.m_fRadius*2.0)) && !bRightBloomed;i++)
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

			fValueL1 = InterpolatePixelValue(pBitmap, pMask, QPointF(fX-fDistance+fndX, fY), true);
			fValueL2 = InterpolatePixelValue(pBitmap, pMask, QPointF(fX-fDistance-1+fndX, fY), true);
			fValueR1 = InterpolatePixelValue(pBitmap, pMask, QPointF(fX+fDistance+fndX, fY), true);
			fValueR2 = InterpolatePixelValue(pBitmap, pMask, QPointF(fX+fDistance+1+fndX, fY), true);

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

		fX = bs.m_ptStar.x() + fdX;
		bs.m_ptStar.setX(fX);
	};
};

/* ------------------------------------------------------------------- */

void	CDeBloom::RefineStarCenter2(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs)
{
	CBloomedStarGradient	bsg,
							bsgn;

	bsg.ptStar = bs.m_ptStar;
	bsgn = bsg;

	double				fRadius = bs.m_fRadius;
	double				fMinimum = -1.0;

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

	bs.m_ptStar.rx() += bsg.fdX;
	bs.m_ptStar.ry() += bsg.fdY;
};

/* ------------------------------------------------------------------- */

void	CDeBloom::ComputeStarCenter(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, CBloomedStar & bs)
{
	int				i;
	double				//fAverageX = 0,
						fdAverageY = 0;
	std::vector<double>	vValues;
	double				fRadius;
	double				fX = bs.m_ptStar.x();
	double				fY = bs.m_ptStar.y();


	fRadius=bs.m_fRadius;
	vValues.reserve(fRadius*2+1);


	std::vector<double>	vYCenters;

	for (i = std::max(0.0, fX-fRadius-0.5);i<=std::min(static_cast<double>(m_lWidth-1), fX+fRadius+0.5);i++)
	{
		bool			bBloomed = false;
		double			fLocalMaximum = -1.0;
		double			fdYMaximum = -1.0;
		double			fdY;

		vValues.clear();
		for (fdY = -fRadius;fdY<=fRadius && !bBloomed;fdY+=0.1)
		{
			double			fValue;

			fValue = InterpolatePixelValue(pBitmap, pMask, QPointF(i, fY+fdY), true);

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

	bs.m_ptStar.ry() += fdAverageY;

	RefineStarCenter(pBitmap, pMask, bs);

	fX = bs.m_ptStar.x();
	fY = bs.m_ptStar.y();

	bool				bFound = false;
	double				fRadiusLeft  = -1.0,
						fRadiusRight = -1.0;
	double				fBloomLeft   = -1.0,
						fBloomRight  = -1.0;
	double				fBloom = 0.0;

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

		fValueL1 = InterpolatePixelValue(pBitmap, pMask, QPointF(std::max(0.0, x1L), fY), true)-m_fBackground;
		fValueR1 = InterpolatePixelValue(pBitmap, pMask, QPointF(std::max(0.0, x1R), fY), true)-m_fBackground;

		if (fValueL1 > 0 && fValueR1 > 0)
		{
			double			fValueL2,
							fValueR2;

			x2L = x1L-1.0;
			x2R = x1R+1.0;

			fValueL2 = InterpolatePixelValue(pBitmap, pMask, QPointF(std::max(0.0, x2L), fY), true)-m_fBackground;
			fValueR2 = InterpolatePixelValue(pBitmap, pMask, QPointF(std::max(0.0, x2R), fY), true)-m_fBackground;

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

	for (int a = 0;a<vAngles.size();a++)
	{
		bFound = false;
		for (i = 1;i<=fRadius*2.0 && !bFound;i++)
		{
			double				fValue1,
								fValue2;
			QPointF			pt1,
							pt2;

			pt1.rx() = fX + (double)i*cos(vAngles[a]*M_PI/180.0);
			pt1.ry() = fY + (double)i*sin(vAngles[a]*M_PI/180.0);
			pt2.rx() = fX + (double)(i+1)*cos(vAngles[a]*M_PI/180.0);
			pt2.ry() = fY + (double)(i+1)*sin(vAngles[a]*M_PI/180.0);

			fValue1 = InterpolatePixelValue(pBitmap, pMask, pt1, true)-m_fBackground;
			fValue2 = InterpolatePixelValue(pBitmap, pMask, pt2, true)-m_fBackground;

			if (fValue1 > 0 && fValue2 > 0 && fValue1 > fValue2)
			{
				double			d1,
								d2;
				CBloomInfo		bi;

				d1 = distance(fX, fY, pt1.x(), pt1.y());
				d2 = distance(fX, fY, pt2.x(), pt2.y());

				bi.m_fAngle  = vAngles[a];
				bi.m_fRadius = sqrt(-(d2*d2-d1*d1)/2.0/log(fValue2/fValue1));
				bi.m_fBloom  = fValue1/exp(-d1*d1/2.0/(bi.m_fRadius*bi.m_fRadius));
				
				bi.m_ptRef.rx() = (pt1.x() + pt2.x())/2.0;
				bi.m_ptRef.ry() = (pt1.y() + pt2.y())/2.0;

				bs.m_vBlooms.push_back(bi);
				bFound = true;
			};
		};
	};

	bs.m_fRadius  = fRadius;
	bs.m_fBloom   = fBloom;
};

/* ------------------------------------------------------------------- */

void	CDeBloom::MarkBloomBorder(CMemoryBitmap * pMask, int x, int y, std::vector<QPointF> & vBorders)
{
	double						fMask;

	if (x>=0 && x<m_lWidth && y>=0 && y<m_lHeight)
	{
		pMask->GetPixel(x, y, fMask);
		if (fMask<100)
		{
			pMask->SetPixel(x, y, 140.0);
			vBorders.push_back(QPointF(x, y));
		};
	};
};

/* ------------------------------------------------------------------- */

void	CDeBloom::MarkBorderAsBloomed(CMemoryBitmap * pMask, int x, int y, std::vector<QPoint> & vBloomed)
{
	if (x>=0 && x<m_lWidth && y>=0 && y<m_lHeight)
	{
		bool					bBloomed = true;
		std::vector<QPointF>	vTests;

		vTests.emplace_back(x - 1, y - 1);
		vTests.emplace_back(x - 1, y - 0);
		vTests.emplace_back(x - 1, y + 1);
		vTests.emplace_back(x - 0, y - 1);
		vTests.emplace_back(x - 0, y + 1);
		vTests.emplace_back(x + 1, y - 1);
		vTests.emplace_back(x + 1, y - 0);
		vTests.emplace_back(x + 1, y + 1);

		for (int i = 0;i<vTests.size() && bBloomed;i++)
		{
			//
			// Don't attempt to check Pixel values that are out of bounds
			//
			if (false == (vTests[i].x() >= 0 && vTests[i].x() < m_lWidth && vTests[i].y() >= 0 && vTests[i].y() < m_lHeight))
				continue;
			double				fMask;

			pMask->GetPixel(vTests[i].x(), vTests[i].y(), fMask);
			bBloomed = fMask>0;
		};

		if (bBloomed)
		{
			pMask->SetPixel(x, y, 255.0);
			vBloomed.emplace_back(x, y);
		};
	};
};

/* ------------------------------------------------------------------- */

void	CDeBloom::ExpandBloomedArea(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask, int x, int y)
{
	bool bEnd = false;
	std::vector<QPoint>	vBloomed;
	int	lLargestY = y;
	int	lTopY = 0;
	int	lLargestWidth = 0;
	int	lBloomHeight = 0;
	CBloomedStar bs;

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
	for (int j = y; j >= 0 && !bEnd; j--)
	{
		double				fGray;
		double				fMask;
		bool				bEndRow = false;
		int				lMinX = x,
							lMaxX = x;
		bool				bEndLeft = true;
		bool				bEndRight = true;

		bEnd = true;
		// Expand horizontally to the left
		for (int i = x;i>=0 && !bEndRow;i--)
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
		for (int i = x;i<m_lWidth && !bEndRow;i++)
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
			for (int i = lMinX;i<=lMaxX;i++)
			{
				pMask->SetPixel(i, j, 255.0);
				vBloomed.emplace_back(i, j);
			};

			// Try to expand one pixel to the bottom
			if ((j==y) && (j<m_lHeight-1))
			{
				bool			bExtraHeight = false;

				for (int i = lMinX;i<=lMaxX;i++)
				{
					if (IsBottomEdge(pBitmap, i, j))
					{
						pMask->SetPixel(i, j+1, 255.0);
						vBloomed.emplace_back(i, j+1);
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
				bs.m_ptStar.rx() = (lMaxX+lMinX)/2.0;
				bs.m_ptStar.ry() = lLargestY;
			};
		};
	};

	// Try to expand one pixel to the top
	if (lTopY > 1)
	{
		if (IsTopEdge(pBitmap, x, lTopY))
		{
			pMask->SetPixel(x, lTopY-1, 255.0);
			vBloomed.emplace_back(x, lTopY-1);
			lBloomHeight++;
			lTopY--;
		}
	}

	if ((lBloomHeight>2) && (lLargestWidth>1) && (lBloomHeight>lLargestWidth))
	{
		// Mark the possible star
		double					fRadius = lLargestWidth/2.0+3.0;
		std::vector<QPointF>	vBorders;

		//ComputeStarCenter(ptStar.X, ptStar.Y, fRadius);

		// Mark bloom area borders
		for (int i = 0;i<vBloomed.size();i++)
		{
			MarkBloomBorder(pMask, vBloomed[i].x()-1, vBloomed[i].y()-1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()  , vBloomed[i].y()-1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()+1, vBloomed[i].y()-1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()-1, vBloomed[i].y()  , vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()+1, vBloomed[i].y()  , vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()-1, vBloomed[i].y()+1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()  , vBloomed[i].y()+1, vBorders);
			MarkBloomBorder(pMask, vBloomed[i].x()+1, vBloomed[i].y()+1, vBorders);
		};

		// And mark lonely borders as bloomed
		for (int i = 0;i<vBorders.size();i++)
			MarkBorderAsBloomed(pMask, vBorders[i].x(), vBorders[i].y(), vBloomed);

		bs.m_fRadius = fRadius;
		bs.m_vBloomed  = vBloomed;
		m_vBloomedStars.push_back(bs);
	}
	else
	{
		// Reset the bloomed area
		for (int i = 0;i<vBloomed.size();i++)
			pMask->SetPixel(vBloomed[i].x(), vBloomed[i].y(), 0.0);
	}
}


std::shared_ptr<C8BitGrayBitmap> CDeBloom::CreateMask(CMemoryBitmap* pBitmap)
{
	std::shared_ptr<C8BitGrayBitmap> pMask;

	if (pBitmap != nullptr && pBitmap->IsMonochrome() && !pBitmap->IsCFA())
	{
		m_lWidth  = pBitmap->Width();
		m_lHeight = pBitmap->Height();

		pMask = std::make_shared<C8BitGrayBitmap>();
		pMask->Init(m_lWidth, m_lHeight);

		m_fBackground = ComputeBackgroundValue(pBitmap);

		if (m_pProgress != nullptr)
			m_pProgress->Start2(m_lHeight);

		// Start at the bottom
		for (int j = m_lHeight-1;j>=0;j--)
		{
			for (int i = 0;i<m_lWidth;i++)
			{
				double				fGray;
				double				fMask;

				pBitmap->GetPixel(i, j, fGray);
				if (fGray >= 256.0*m_fBloomThreshold)
				{
					pMask->GetPixel(i, j, fMask);
					if (!IsBloomedValue(fMask))
						ExpandBloomedArea(pBitmap, pMask.get(), i, j);
				}
			}
			if (m_pProgress != nullptr)
				m_pProgress->Progress2(j+1);
		}

		if (m_pProgress != nullptr)
			m_pProgress->End2();
	}

#ifdef DEBUGDEBLOOM
	WriteTIFF("E:\\BloomMask.tif", pMask, nullptr, nullptr);
	WriteTIFF("E:\\BloomImage.tif", pBitmap, nullptr, nullptr);
#endif

	return static_cast<bool>(pMask) && !this->m_vBloomedStars.empty() ? pMask : std::shared_ptr<C8BitGrayBitmap>{};
}


double CDeBloom::ComputeValue(CMemoryBitmap* pBitmap, C8BitGrayBitmap* pMask, int x, int y, bool& bDone)
{
	double						fResult = 255.0;
	double						fSum = 0,
								fWeight = 0;
	bDone = false;

	for (int i = std::max(0, x-5);i<=std::min(m_lWidth-1, x+5);i++)
	{
		for (int j = std::max(0, y-3);j<=std::min(m_lHeight-1, y+3);j++)
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

	for (int i = 0;i<bs.m_vBloomed.size();i++)
	{
		double			fMask,
						fValue,
						fValue3,
						fValue4;

		pMask->GetPixel(bs.m_vBloomed[i].x(), bs.m_vBloomed[i].y(), fMask);
		if (IsBloomedValue(fMask))
		{
			double		fDistance,
						fBaseValue;
			double		fAverage = -1.0;
			QPointF	pt(bs.m_vBloomed[i].x()+0.5, bs.m_vBloomed[i].y()+0.5);

			fDistance = distance(pt.x(), pt.y(), bs.m_ptStar.x(), bs.m_ptStar.y());

			fValue3 = InterpolatePixelValue(pBitmap, pMask, QPointF(bs.m_ptStar.x()-fDistance, bs.m_ptStar.y()), true);
			fValue4 = InterpolatePixelValue(pBitmap, pMask, QPointF(bs.m_ptStar.x()+fDistance, bs.m_ptStar.y()), true);

			if (fValue3 > 0 && fValue4 > 0)
				fAverage = std::min(fValue3,fValue4);
			else if (fValue3 > 0)
				fAverage = fValue3;
			else if (fValue4 > 0)
				fAverage = fValue4;

			double		fBloomValue = 0.0;
			double		fBloomWeight = 0.0;
			for (int a = 0;a<bs.m_vBlooms.size();a++)
			{
				double	fBloomDistance;

				fBloomDistance = distance(pt.x(), pt.y(), bs.m_vBlooms[a].m_ptRef.x(), bs.m_vBlooms[a].m_ptRef.y());
				fBloomWeight += 1.0/(fBloomDistance+1.0);

				fFactor1 = 2.0*pow(bs.m_vBlooms[a].m_fRadius, 2);
				fValue = m_fBackground+exp(-(fDistance * fDistance)/fFactor1)*bs.m_vBlooms[a].m_fBloom;

				fBloomValue += fValue/(fBloomDistance+1.0);
			};

			if (fBloomWeight)
				fBloomValue /= fBloomWeight;

			//fValue1 = m_fBackground + exp(-(fDistance * fDistance)/fFactor1)*bs.m_fBloom;
			//fValue2 = m_fBackground + exp(-(fDistance * fDistance)/fFactor2)*bs.m_fBloom2;

			pBitmap->GetPixel(bs.m_vBloomed[i].x(), bs.m_vBloomed[i].y(), fBaseValue);

			fValue = fBaseValue;
			if (fAverage>0 && fBloomValue>0)
				fBloomValue = std::min(fAverage, fBloomValue);

			if (fBloomValue)
				fValue = std::max(fBaseValue, fBloomValue);
			/*
			if (fAverage>0)
			{
				double			fRatio = 1.0/(1.0+fDistance*fDistance);
				fValue = fValue*(1.0-fRatio)+fAverage*fRatio;
			};*/

			pBitmap->SetPixel(bs.m_vBloomed[i].x(), bs.m_vBloomed[i].y(), std::min(255.0, fValue));
		};
	};
};

/* ------------------------------------------------------------------- */

void    CDeBloom::SmoothMaskBorders(CMemoryBitmap * pBitmap, C8BitGrayBitmap * pMask)
{
	std::vector<double>		vValues;

	vValues.resize(8);

	for (int i = 1;i<m_lWidth-1;i++)
	{
		for (int j = 1;j<m_lHeight-1;j++)
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
			}
		}
	}
}


double CDeBloom::ComputeBackgroundValue(CMemoryBitmap* pBitmap)
{
	double					fResult = 0.0;
	CBackgroundCalibration	BackgroundCalibration;

	BackgroundCalibration.m_BackgroundCalibrationMode = BCM_PERCHANNEL;
	BackgroundCalibration.m_BackgroundInterpolation   = BCI_LINEAR;
	BackgroundCalibration.ComputeBackgroundCalibration(pBitmap, true, m_pProgress);
	fResult = BackgroundCalibration.m_fTgtRedBk/256.0;

	return fResult;
}


void CDeBloom::DeBloom(CMemoryBitmap* pBitmap, std::shared_ptr<C8BitGrayBitmap> pMask)
{
	ZFUNCTRACE_RUNTIME();
	// First compute background value
	m_fBackground = ComputeBackgroundValue(pBitmap);

	{
		if (m_pProgress != nullptr)
			m_pProgress->Start2(static_cast<int>(m_vBloomedStars.size()));

		for (size_t i = 0; i < m_vBloomedStars.size(); i++)
		{
			if (m_pProgress != nullptr)
				m_pProgress->Progress2(static_cast<int>(i) + 1);
			ComputeStarCenter(pBitmap, pMask.get(), m_vBloomedStars[i]);
		}

		if (m_pProgress != nullptr)
			m_pProgress->End2();
	}

	if (m_pProgress != nullptr)
		m_pProgress->Start2(m_lWidth);

	std::vector<QPoint> vUnprocessed;
	std::vector<QPoint> vProcessed;

	for (int i = 0; i < m_lWidth; i++)
	{
		for (int j = 0; j < m_lHeight; j++)
		{
			double fMask;
			pMask->GetPixel(i, j, fMask);
			if (IsBloomedValue(fMask))
			{
				double fValue;
				bool bDone;

				fValue = ComputeValue(pBitmap, pMask.get(), i, j, bDone);

				if (bDone)
				{
					pBitmap->SetPixel(i, j, fValue);
					vProcessed.emplace_back(i, j);
				}
				else
				{
					// the coordinates so that they can be processed later on
					vUnprocessed.emplace_back(i, j);
				}
			}
		}
		if (m_pProgress != nullptr)
			m_pProgress->Progress2(i+1);
	}

	// Process recursively unprocessed
	int					lNrUnprocessed = 0;

	if (vUnprocessed.size())
	{
		std::vector<QPoint>			vNewlyProcessed = vProcessed;

		while (vUnprocessed.size() && (vUnprocessed.size() != lNrUnprocessed))
		{
			for (int i = 0;i<vNewlyProcessed.size();i++)
				pMask->SetPixel(vNewlyProcessed[i].x(), vNewlyProcessed[i].y(), 190.0);
			vNewlyProcessed.clear();

			lNrUnprocessed = (int)vUnprocessed.size();

			std::vector<QPoint>			vToProcess = vUnprocessed;

			vUnprocessed.clear();

			for (int i = 0;i<vToProcess.size();i++)
			{
				double				fValue;
				bool				bDone;

				fValue = ComputeValue(pBitmap, pMask.get(), vToProcess[i].x(), vToProcess[i].y(), bDone);

				if (bDone)
				{
					pBitmap->SetPixel(vToProcess[i].x(), vToProcess[i].y(), fValue);
					vProcessed.push_back(vToProcess[i]);
					vNewlyProcessed.push_back(vToProcess[i]);
				}
				else
				{
					// the coordinates so that they can be processed later on
					vUnprocessed.push_back(vToProcess[i]);
				}
			}
		}

		//for (int i = 0; i < vProcessed.size(); i++)
		for (const QPoint& point : vProcessed)
			pMask->SetPixel(point.x(), point.y(), 255.0);
	}

	if (m_pProgress != nullptr)
		m_pProgress->End2();

#ifdef DEBUGDEBLOOM
	WriteTIFF("E:\\BloomImage_Step1.tif", pBitmap, nullptr, nullptr);
#endif

	for (int i = 0;i<m_vBloomedStars.size();i++)
	{
		AddStar(pBitmap, pMask.get(), m_vBloomedStars[i]);
	}

	SmoothMaskBorders(pBitmap, pMask.get());
#ifdef DEBUGDEBLOOM
	WriteTIFF("E:\\BloomImage_Step2.tif", pBitmap, nullptr, nullptr);
#endif
}


void CDeBloom::CreateBloomMask(CMemoryBitmap* pBitmap, ProgressBase* pProgress)
{
	m_pProgress = pProgress;
	this->m_pMask = CreateMask(pBitmap);
}


void CDeBloom::DeBloomImage(CMemoryBitmap * pBitmap, ProgressBase * pProgress)
{
	m_pProgress = pProgress;
	if (static_cast<bool>(m_pMask))
		DeBloom(pBitmap, m_pMask);
}
