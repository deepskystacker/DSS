#pragma once
#include "FlatPart.h"
#include "DynamicStats.h"
#include "avx_median.h"


/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */


typedef std::vector<QPointF>	POINTFVECTOR;
typedef POINTFVECTOR::iterator	POINTFITERATOR;

/* ------------------------------------------------------------------- */

inline double	Distance(double fX1, double fY1, double fX2, double fY2) noexcept
{
	return sqrt((fX1-fX2)*(fX1-fX2) + (fY1-fY2)*(fY1-fY2));
};

inline double Distance(const QPointF& pt1, const QPointF& pt2) noexcept
{
	return Distance(pt1.x(), pt1.y(), pt2.x(), pt2.y());
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

inline double Median(double v1, double v2, double v3) noexcept
{
  if (v1 > v2)
  {
    if (v3 > v1)
		return v1;
    else if (v3 < v2)
		return v2;
    return v3;
  }
  else
  {
    if (v3 < v1)
		return v1;
    else if (v3 > v2)
		return v2;
    return v3;
  }
}

/* ------------------------------------------------------------------- */

template <class T> inline
double Median(std::vector<T>& values)
{
    if (values.empty())
        return 0;

    const int size = static_cast<int>(values.size());

	return qMedian(values.data(), size, size / 2);
/*
    // benchmarked: at around 40 elements, partial sort stars becoming faster
    // O(N) or O(2*N) for even count vs O(N*log(N))
    if (size > 40)
    {
        auto n = size / 2;
        std::nth_element(values.begin(), values.begin() + n, values.end());
        double median = values[n];

        if (size & 1) // odd
        {
            return median;
        }
        else // even
        {
            auto max = std::max_element(values.begin(), values.begin() + n);

            return (*max + median) / 2;
        }
    }
    else // fallback to classic sort
    {
        auto n = size / 2;
        std::sort(values.begin(), values.end());

        if (size & 1) // odd
        {
            return values[n];
        }
        else // even
        {
            return (values[n] + values[n - 1]) / 2;
        }
    }
*/
}

/* ------------------------------------------------------------------- */

template <class T> inline
T Maximum(const std::vector<T>& values)
{
    if (values.empty())
        return 0;

	return *std::max_element(values.begin(), values.end());
};

/* ------------------------------------------------------------------- */

template <class T> inline
double Average(const std::vector<T>& values)
{
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
};

/* ------------------------------------------------------------------- */

template <class T> inline
double Sigma2(const std::vector<T>& values, double& average)
{
	double result = 0.0;
	double squareDiff = 0.0;

	// Compute the average
	average = Average(values);

    for (double val : values)
        squareDiff += std::pow(val - average, 2);

    if (values.size())
        result = sqrt(squareDiff / values.size());

	return result;
};

/* ------------------------------------------------------------------- */

template <class T> inline
double Sigma(const std::vector<T>& values)
{
	double average;

	return Sigma2(values, average);
};

template <typename T>
double CalculateSigmaFromAverage(const std::vector<T>& values, const double targetAverage)
{
	double squareDiff = 0.0;

    for (double val : values)
        squareDiff += std::pow(val - targetAverage, 2);

	return sqrt(squareDiff / values.size());
}

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

template <class T> inline
void	DetectFlatParts(std::vector<T> & vValues, double fMaximum, std::vector<CFlatPart> & vFlatParts, double & fAverageVariation)
{
	double					fSummedVariation = 0;
	double					fAverage = 0;
	double					fTotalVariation = 0;

	bool					bInFlatPart = false;
	CFlatPart				fp;
	std::vector<double>		vVariations;
	std::vector<double>		vAbsVariations;

	fAverage = 0;
	fAverageVariation = 0;
	vVariations.reserve(vValues.size());
	vAbsVariations.reserve(vValues.size());

	for (int i = 0;i<vValues.size();i++)
	{
		if (!i)
		{
			vVariations.push_back(0);
			vAbsVariations.push_back(0);
		}
		else
		{
			vVariations.push_back((double)(vValues[i]-vValues[i-1])/(double)std::max(static_cast<T>(1), vValues[i-1])*sqrt(vValues[i]/fMaximum));
			vAbsVariations.push_back((double)(vValues[i]-vValues[i-1])/fMaximum);
		};
		fTotalVariation += fabs(vAbsVariations[i]);
		fAverage += vValues[i];
	};

	if (vValues.size())
	{
		fAverage /= vValues.size();
		fAverageVariation = fTotalVariation/vValues.size();
	};

	for (double fThreshold = 0.05;fThreshold<=0.20;fThreshold+=0.05)
	{
		fSummedVariation = 0;
		for (int i = 0;i<vValues.size();i++)
		{
			if (bInFlatPart)
			{
				if (fabs(fSummedVariation + vVariations[i])>fThreshold ||
					fabs(vAbsVariations[i])>0.01)
				{
					// End the flat part
					if (fp.m_lEnd-fp.m_lStart>2)
					{
						// This is a flat (at least 3 values)
						fp.m_fAverage /= fp.Length();
						fp.m_fAverageVariation /= fp.Length();
						fp.m_fAbsAverageVariation /= fp.Length();
						vFlatParts.push_back(fp);

						// Start again - a little later
						i = fp.m_lStart+1;
					};
					bInFlatPart = false;
				}
				else
				{
					// Continue the flat part
					fp.m_lEnd = i;
					fSummedVariation += vVariations[i];
					fp.m_fAverage    += vValues[i];
					fp.m_fAverageVariation += fabs(vVariations[i]);
					fp.m_fAbsAverageVariation += fabs(vAbsVariations[i]);
				};
			}
			if (!bInFlatPart)// && (fabs(vVariations[i])<=fThreshold))
			{
				// Start a new flat part
				fp.m_lStart = i;
				fp.m_lEnd   = i;
				fSummedVariation = 0;//vVariations[i];
				bInFlatPart = true;
				fp.m_fAverage			= vValues[i];
				fp.m_fAverageVariation  = 0;//fabs(vVariations[i]);
				fp.m_fAbsAverageVariation = 0;
			};
		};

		if (bInFlatPart && fp.Length())
		{
			fp.m_fAverage /= fp.Length();
			fp.m_fAverageVariation /= fp.Length();
			fp.m_fAbsAverageVariation /= fp.Length();
			vFlatParts.push_back(fp);
			bInFlatPart		 = false;
			fSummedVariation = 0;
		};
	};

	if (vValues.size())
	{
		// Check that at least one flat part is below the average
		bool				bFound = false;

		for (int i = 0;i<vFlatParts.size() && !bFound;i++)
			bFound = (vFlatParts[i].m_fAverage <= fAverage);
		for (int i = 0;i<vFlatParts.size() && bFound;i++)
		{
			if (vFlatParts[i].m_fAverage > fAverage)
			{
				// Remove this one
				vFlatParts[i].m_lEnd = vFlatParts[i].m_lStart-1;
			};
		};
	};
};

/* ------------------------------------------------------------------- */

template <class T> inline
T Minimum(std::vector<T>& values, bool ignoreZeros)
{
    T result = 0;

    if (!values.empty())
    {
        if (values.front() || !ignoreZeros)
            result = values.front();
    }

    for (T const& val : values)
        if (val || !ignoreZeros)
            result = std::min(result, val);

	return result;
};

/* ------------------------------------------------------------------- */

template <class T> inline
double	Homogenize(std::vector<T> & vValues, double fMaximum)
{
	double						fResult = 1.0;
	std::vector<CFlatPart> 		vFlatParts;
	double						fAverageVariation;

	DetectFlatParts(vValues, fMaximum, vFlatParts, fAverageVariation);

	std::sort(vFlatParts.begin(), vFlatParts.end());
	if (vFlatParts.size() && (vFlatParts[0].Length()>1))
	{
		// Keep only the longest part
		std::vector<T>			vAuxValues;

		vAuxValues.reserve(vFlatParts[0].Length());

		for (int i = vFlatParts[0].m_lStart;i<=vFlatParts[0].m_lEnd;i++)
			vAuxValues.push_back(vValues[i]);

		vValues = vAuxValues;
		//fAverageVariation = vFlatParts[0].m_fAbsAverageVariation;
	}
	else// if (fAverageVariation>0.05)
	{
		// Use only the minimum value for each channel
		T						fMinimum;

		fMinimum = Minimum(vValues, true);

		vValues.resize(1);		vValues[0] = fMinimum;
	};

	fResult = std::min(1.0, std::max(0.0, (1.05 - fAverageVariation*10)));

	return fResult;
};

/* ------------------------------------------------------------------- */

template <class T> inline
double Homogenize3(std::vector<T> & vValues, int lNrSubStacks)
{
	double						fResult = 1.0;
	std::vector<double>			vStackValues;
	std::vector<T>				vWork;
	std::vector<T>				vSubStack;
	int						lNrValues = vValues.size();

	vStackValues.reserve(lNrSubStacks);
	vSubStack.reserve(lNrValues/lNrSubStacks+1);
	vWork.reserve(lNrValues / lNrSubStacks + 1);

	for (int j = 0;j<lNrSubStacks;j++)
	{
		vSubStack.clear();
		for (int i = j;i<lNrValues;i+=lNrSubStacks)
			vSubStack.push_back(vValues[i]);

		vStackValues.push_back(KappaSigmaClip(vSubStack, 1.5, 3, vWork));
	};

	fResult = Median(vStackValues);
	vValues.clear();
	vValues.push_back(fResult);

	return fResult;
};

/* ------------------------------------------------------------------- */

template <class T> inline
double Homogenize3(std::vector<T> & v1Values, std::vector<T> & v2Values, std::vector<T> & v3Values, int lNrSubStacks)
{
	Homogenize3(v1Values, lNrSubStacks);
	Homogenize3(v2Values, lNrSubStacks);
	Homogenize3(v3Values, lNrSubStacks);

	return 1.0;
};

/* ------------------------------------------------------------------- */

template <class T> inline
double	Homogenize(std::vector<T> & v1Values, std::vector<T> & v2Values, std::vector<T> & v3Values, double fMaximum)
{
	double						fResult = 1.0;
	std::vector<double>			vValues;

	vValues.reserve(v1Values.size());

	for (int i = 0;i<v1Values.size();i++)
		vValues.push_back(double(v1Values[i]+v2Values[i]+v3Values[i])/3.0);

/*
	std::vector<CFlatPart> 		v1FlatParts;
	std::vector<CFlatPart> 		v2FlatParts;
	std::vector<CFlatPart> 		v3FlatParts;
	double						f1AverageVariation;
	double						f2AverageVariation;
	double						f3AverageVariation;

	DetectFlatParts(v1Values, fMaximum, v1FlatParts, f1AverageVariation);
	DetectFlatParts(v2Values, fMaximum, v2FlatParts, f2AverageVariation);
	DetectFlatParts(v3Values, fMaximum, v3FlatParts, f3AverageVariation);

	std::vector<CFlatPart> 		vFlatParts;

	vFlatParts.reserve(v1FlatParts.size()+v2FlatParts.size()+v3FlatParts.size());
	vFlatParts = v1FlatParts;
	for (int i = 0;i<v2FlatParts.size();i++)
		vFlatParts.push_back(v2FlatParts[i]);
	for (int i = 0;i<v3FlatParts.size();i++)
		vFlatParts.push_back(v3FlatParts[i]);
*/
	std::vector<CFlatPart> 		vFlatParts;
	double						fAverageVariation;

	DetectFlatParts(vValues, fMaximum, vFlatParts, fAverageVariation);

	std::sort(vFlatParts.begin(), vFlatParts.end());
	if (vFlatParts.size() && (vFlatParts[0].Length()>1))
	{
		// Keep only the longest part
		std::vector<T>			v1AuxValues;
		std::vector<T>			v2AuxValues;
		std::vector<T>			v3AuxValues;

		v1AuxValues.reserve(vFlatParts[0].Length());
		v2AuxValues.reserve(vFlatParts[0].Length());
		v3AuxValues.reserve(vFlatParts[0].Length());

		for (int i = vFlatParts[0].m_lStart;i<=vFlatParts[0].m_lEnd;i++)
		{
			v1AuxValues.push_back(v1Values[i]);
			v2AuxValues.push_back(v2Values[i]);
			v3AuxValues.push_back(v3Values[i]);
		};

		v1Values = v1AuxValues;
		v2Values = v2AuxValues;
		v3Values = v3AuxValues;
	}
	else if (fAverageVariation>0.05)
	{
		// Use only the minimum value for each channel
		T						f1Minimum,
								f2Minimum,
								f3Minimum;

		f1Minimum = Minimum(v1Values, true);
		f2Minimum = Minimum(v2Values, true);
		f3Minimum = Minimum(v3Values, true);

		v1Values.resize(1);		v1Values[0] = f1Minimum;
		v2Values.resize(1);		v2Values[0] = f2Minimum;
		v3Values.resize(1);		v3Values[0] = f3Minimum;
	};

	return fResult;
};

/* ------------------------------------------------------------------- */

template <class T> inline
void	Homogenize2(std::vector<T> & vValues, double fMaximum)
{
	if (vValues.size()>3)
	{
		bool				bEnd = false;
		int				i;

		std::sort(vValues.begin(), vValues.end());
		// Compute the distance between the line and each point
		if (vValues[0] != vValues[vValues.size()-1])
		{
			double			fMin  = vValues[0],
							fMax  = vValues[vValues.size()-1];
			double			fSteep = (fMax-fMin)/vValues.size();
			double			fMaxDistance = 0;
			int			lIndice1;

			for (i = 0;i<vValues.size();i++)
			{
				double		fDistance;

				fDistance = (fSteep * i - (vValues[i]-fMin))/(fMax-fMin);
				if (fDistance>fMaxDistance)
				{
					fMaxDistance = fDistance;
					lIndice1 = i;
				};
			};
			if (fMaxDistance<0.2)
				lIndice1 = -1;

			// Compute the second indice based on the variation
			// between the minimum and the current value
			int			lIndice2 = -1;
			for (i = 0;i<vValues.size() && (lIndice2<0);i++)
			{
				double		fIncrease;

				fIncrease = (vValues[i]-fMin)/vValues[i]/sqrt(fMin/fMaximum);
				if (fIncrease>0.15)
					lIndice2 = i;
			};

			if (lIndice1>=0 || lIndice2>=0)
			{
				// Cut at this position
				int		lIndice = std::min(lIndice1==-1 ? 10000 : lIndice1,
										  lIndice2==-1 ? 10000 : lIndice2);
				vValues.resize(lIndice+1);
			}

			// Now analyze the distribution for its homogeneity
			//std::vector<double>		vAuxValues;
			fMax = vValues[vValues.size()-1];

			if (fMax>fMin)
			{

				/*vAuxValues.resize(vValues.size());
				for (i = 0;i<vAuxValues.size();i++)
					vAuxValues[i] = (vValues[i]-fMin)/(fMax-fMin);*/

				double		fSigma,
							fAverage;

				fSigma = Sigma2(vValues, fAverage);

				if (fSigma/fAverage>0.15) // Discard the whole set
					vValues.clear();
			};
		};
	};
};

/* ------------------------------------------------------------------- */

template <class T> inline
double	KappaSigmaClip(const std::vector<T> & vValues, double fKappa, int lIteration, std::vector<T> & vAuxValues)
{
	double			Result = 0;
	bool			bEnd = false;
	CDynamicStats	DynStats;

	//
	// Copy the data
	//
	vAuxValues = vValues;

	std::sort(vAuxValues.begin(), vAuxValues.end());

	FillDynamicStat(vAuxValues, DynStats);

	for (int i = 0;i<lIteration && !bEnd;i++)
	{
		double			fAverage;
		double			fSigma;
		int			lCurrentIndice = 0;
		int			j = 0;
		double			fMin,
						fMax;

		fSigma	 = DynStats.Sigma(); //Sigma2(vAuxValues, fAverage);
		fAverage = DynStats.Average();

		fMin = fAverage - fKappa * fSigma;
		fMax = fAverage + fKappa * fSigma;

		while (j<vAuxValues.size() && vAuxValues[j]<fMin)
		{
			DynStats.RemoveValue(vAuxValues[j]);
			j++;
		};
		while (j<vAuxValues.size() && vAuxValues[j]<=fMax)
		{
			vAuxValues[lCurrentIndice] = vAuxValues[j];
			lCurrentIndice++;
			j++;
		};

		while (j<vAuxValues.size())
		{
			DynStats.RemoveValue(vAuxValues[j]);
			j++;
		};

/*		for (int j = 0;j<vAuxValues.size();j++)
		{
			if (((double)vAuxValues[j]>= (fAverage - fKappa*fSigma)) &&
				((double)vAuxValues[j]<= (fAverage + fKappa*fSigma)))
				vTempValues.push_back(vAuxValues[j]);
			else
				DynStats.RemoveValue(vAuxValues[j]);
		};*/

		bEnd = !lCurrentIndice || (lCurrentIndice == vAuxValues.size());
		vAuxValues.resize(lCurrentIndice);
		//bEnd = !vTempValues.size() || (vAuxValues.size() == vTempValues.size());
		//vAuxValues = vTempValues;
	};

	Result = DynStats.Average();//Average(vAuxValues);

	return Result;
};

/* ------------------------------------------------------------------- */

template <class T> inline
double	MedianKappaSigmaClip(const std::vector<T> & vValues, double fKappa, int lIteration, std::vector<T>& vWorkingBuffer1, std::vector<T>& vWorkingBuffer2)
{
	double			Result = 0;

	// Set up the working buffers - we can flip between them to prevent
	// needless copying of vectors.
	vWorkingBuffer1.clear();
	vWorkingBuffer2.clear();

	std::vector<T>& vecCurrentPass = vWorkingBuffer1;
	std::vector<T>& vecTempBuffer = vWorkingBuffer2;

	// Initial copy into the working set to start us off.
	vWorkingBuffer1 = vValues;
	for (int i = 0;i<lIteration;i++)
	{
		double			fAverage;
		double			fSigma;
		T				fMedian;

		fMedian = Median(vecCurrentPass);
		fSigma = Sigma2(vecCurrentPass, fAverage);

		// Go through and populate the temp buffer according to the values.
		vecTempBuffer.clear();
		for (int j = 0;j< vecCurrentPass.size();j++)
		{
			if (((double)vecCurrentPass[j]>= (fAverage - fKappa*fSigma)) &&
				((double)vecCurrentPass[j]<= (fAverage + fKappa*fSigma)))
				vecTempBuffer.push_back(vecCurrentPass[j]);
			else
				vecTempBuffer.push_back(fMedian);
		}

		// Swap temp and working buffers for next pass.
		std::vector<T>& vecTmp = vecCurrentPass;
		vecCurrentPass = vecTempBuffer;
		vecTempBuffer = vecTmp;
	}

	// The final pass will now be in the current buffer (because of the swap at the end of the loop).
	Result = Average(vecCurrentPass);

	return Result;
};

/* ------------------------------------------------------------------- */

template <typename T>
double AutoAdaptiveWeightedAverage(const std::vector<T> & vValues, int lIterations, std::vector<double> & vWeights)
{
	// Computes the auto-adaptive weighted average of a set of numbers
	// (intended to be the values of the same pixel in different stacked images).
	//
	// Based on pp. 44-51 of:
	//
	// Peter B. Stetson (1989)
	//	"The Techniques of Least Squares and Stellar Photometry with CCDs"
	//
	// Which was presented at:
	//	V Escola Avançada de Astrofísica,
	//	Aguas de São Pedro, Brazil
	//
	// and is archived online at:
	//	https://ned.ipac.caltech.edu/level5/Stetson/Stetson_contents.html
	//	(section "Non-Gaussian Error Distributions")
	//
	// This was further interpreted (maybe changed) by an anonymous author at
	// https://archive.stsci.edu/hst/wfpc2/pipeline.html
	// (this author puts in a 1/sigma2 term that is constant and should have
	// no effect on normalized weights; Stetson's sigma is the sigma of the data
	// whereas the second author's sigma2 is "derived from read noise and gain").
	//
	// I am indebted to Michael A. Covington and Simon C. Smith for their
	// immense help in getting this working.  Much of the code is adapted from
	// Simon's code simply because it was C++ whereas Michael's written in C#
	//
	// Regardless any faults are my own!
	// David C. Partridge
	// 29 July 2019
	//

	double			fResult = 0;
	size_t			i = 0;
	size_t			nElements = vValues.size();

	// If the standard deviation is less than (say) five there's not much point iterating
	// the auto-adaptive average calculation, in that case just return a regular average.
	const double	SMALL = 5.0;

	//
	// Probably not too bright an idea to do weighted average on a sample size less than 3
	//
	if (nElements > 2)
	{
		double		fMaximum = vValues[0];

		for (i = 1; i < nElements; i++)
			fMaximum = std::max(fMaximum, (double)vValues[i]);

		if (fMaximum > 0)
		{
			vWeights.resize(nElements);

			// Start with the mean value of the set.
			double runningAverage = Average(vValues);
			double fSigma = 0.0;

			// Calculate weights for the values, but re-iterate with the
			// new "weighted" average each time. This should converge on
			// a single value over time. Rumour has it that 5 will do :)
			for (int nIteration = 0; nIteration < (int)lIterations; nIteration++)
			{
				//
				// Calculate the standard deviation from the current running average.
				//
				fSigma = CalculateSigmaFromAverage(vValues, runningAverage);

				//
				// Escape clause for small sigma - where all input data
				// are the same or darn nearly so.
				//
				if (0 == nIteration && fSigma <= SMALL) return runningAverage;

				//
				// Calculate the weights
				//
				for (size_t j = 0; j != nElements; j++)
				{
					const double r = fabs(vValues[j] - runningAverage);
					//
					// The 1/sigma^2 term is ignored as it will be constant
					// for all weights.
					//
					vWeights[j] = 1 / (1 + ((r / fSigma) * (r / fSigma)));
				}
				const double weightTotal = std::accumulate(vWeights.begin(), vWeights.end(), 0.0);

				// Calculate the new running average.
				runningAverage = 0.0;
				for (size_t nIndex = 0; nIndex < nElements; nIndex++)
					runningAverage += (vValues[nIndex] * vWeights[nIndex]) / weightTotal;
			}

			//
			// If we didn't already return because of small sigma, then the result is in
			// variable runningAverage
			//
			fResult = runningAverage;
		};
	}
	else
		fResult = Average(vValues);

	return fResult;
};
