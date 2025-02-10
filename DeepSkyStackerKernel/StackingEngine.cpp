#include "stdafx.h"
#include "StackingEngine.h"

#include "MasterFrames.h"
#include "MatchingStars.h"
#include "PixelTransform.h"
#include "EntropyInfo.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "Multitask.h"
#include "histogram.h"
#include "Filters.h"
#include "CosmeticEngine.h"
#include "ChannelAlign.h"
#include "FrameInfoSupport.h"
#include "avx.h"
#include "avx_avg.h"
#include "ztrace.h"
#include "Workspace.h"
#include "MultiBitmap.h"
#include "ColorBitmap.h"
#include "ColorMultiBitmap.h"
#include "GreyMultiBitmap.h"
#include "AHDDemosaicing.h"
#include "BitmapIterator.h"


#define _USE_MATH_DEFINES

#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::SetReferenceFrame(const fs::path& path)
{
	ZFUNCTRACE_RUNTIME();

	const QFileInfo fileInfo(path);

	referenceFrame = path;
	m_strStackingFileInfo = QDir::toNativeSeparators(QString("%1%2%3.stackinfo.txt").arg(fileInfo.path()).arg(QDir::separator()).arg(fileInfo.baseName()));

	unsigned int dwAlignmentTransformation = 2;
	Workspace workspace;

	dwAlignmentTransformation = workspace.value("Stacking/AlignmentTransformation", (uint)2).toUInt();

	// Init from the file
	m_vLightFrameStackingInfo.clear();
	
	QFile file(m_strStackingFileInfo);
	if (!file.open(QIODevice::Text | QIODevice::ReadOnly | QIODeviceBase::Text))
		return;

	// Process line by line.
	QTextStream stream(&file);
	QString currentLine;

	bool			bEnd = false;
	static const int nMaxRead = 10000;

	currentLine = file.readLine(nMaxRead).simplified();
	if (currentLine.isEmpty())
	{
		bEnd = true;
	}
	else
	{
		bool bOK = false;
		int lSavedAlignmentTransformation = currentLine.toInt(&bOK);

		if (lSavedAlignmentTransformation != static_cast<int>(dwAlignmentTransformation))
			bEnd = true;
	}

	if (!bEnd)
	{
		currentLine = file.readLine(nMaxRead).simplified();
		if (currentLine.isEmpty())
		{
			bEnd = true;
		}
		else
		{
			QString strInfoFileName;
			const QString strStoredInfoFileName(currentLine);

			GetInfoFileName(referenceFrame, strInfoFileName);
			if (strInfoFileName.compare(strStoredInfoFileName, Qt::CaseInsensitive))
				bEnd = true;
		}
	};

	while (!bEnd)
	{
		CLightFrameStackingInfo		lfsi;
		bool						bResult = true;

		currentLine = file.readLine(nMaxRead).simplified();
		if (currentLine.isEmpty())
		{
			bEnd = true;
		}
		else
		{
			lfsi.m_strInfoFileName = currentLine;
		}

		currentLine = file.readLine(nMaxRead).simplified();
		if (currentLine.isEmpty())
		{
			bEnd = true;
		}
		else
		{
			lfsi.file = currentLine.toStdU16String().c_str();
		}

		currentLine = file.readLine(nMaxRead).simplified();
		if (currentLine.isEmpty())
		{
			bEnd = true;
		}
		else
		{
			QString strParameters(currentLine);
			bResult = lfsi.m_BilinearParameters.FromText(strParameters);
		}

		if (!bEnd && bResult)
			m_vLightFrameStackingInfo.push_back(lfsi);
	};

	file.close();
	std::sort(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end());
};

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::GetInfoFileName(const fs::path& lightFrame, QString& strInfoFileName)
{
	//ZFUNCTRACE_RUNTIME();
	fs::path file{ lightFrame };
	file.replace_extension(".info.txt");
	QFileInfo info{ file };

	//
	// Get the file creation date/time if possible. If not get the last modified date/time
	//
	QDateTime birthTime{ info.birthTime() };
	if (!birthTime.isValid())
	{
		birthTime = info.lastModified();
	}

	//
	// File doesn't exist
	// 
	if (!birthTime.isValid())
		strInfoFileName.clear();
	else
	{
		strInfoFileName = QString{ "%1 [%2]" }.arg(file.generic_u16string().c_str()).arg(birthTime.toString("yyyy/MM/dd hh:mm:ss"));
	}
}

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::AddLightFrame(const fs::path& szLightFrame, const CBilinearParameters & bp)
{
	ZFUNCTRACE_RUNTIME();

	CLightFrameStackingInfo lfsi(szLightFrame);
	QString strInfoFileName;

	GetInfoFileName(szLightFrame, strInfoFileName);
	LIGHTFRAMESTACKINGINFOITERATOR it = std::lower_bound(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end(), lfsi);
	if (it != m_vLightFrameStackingInfo.end() && !it->file.compare(szLightFrame))
	{
		// There is already this light frame
		it->m_strInfoFileName = strInfoFileName;
		it->m_BilinearParameters = bp;
	}
	else
	{
		// New light frame
		lfsi.m_strInfoFileName = strInfoFileName;
		lfsi.m_BilinearParameters = bp;

		m_vLightFrameStackingInfo.push_back(lfsi);
		std::sort(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end());
	};
};

/* ------------------------------------------------------------------- */

bool CLightFramesStackingInfo::GetParameters(const fs::path& szLightFrame, CBilinearParameters & bp)
{
	// ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	LIGHTFRAMESTACKINGINFOITERATOR it = std::lower_bound(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end(), CLightFrameStackingInfo(szLightFrame));
	if (it != m_vLightFrameStackingInfo.end() && !it->file.compare(szLightFrame))
	{
		QString strInfoFileName;
		GetInfoFileName(szLightFrame, strInfoFileName);

		if (!strInfoFileName.compare(it->m_strInfoFileName, Qt::CaseInsensitive))
		{
			bp = it->m_BilinearParameters;
			bResult = true;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CLightFramesStackingInfo::Save()
{
	ZFUNCTRACE_RUNTIME();

	if (!referenceFrame.empty() && !m_strStackingFileInfo.isEmpty())
	{
		QFile file(m_strStackingFileInfo);
		if (!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
			return;
		QByteArray buffer;
		QTextStream stream(&buffer);

		// Save the alignment transformation used
		unsigned int dwAlignmentTransformation = 2;
		Workspace workspace;

		dwAlignmentTransformation = workspace.value("Stacking/AlignmentTransformation", (uint)2).toUInt();
		stream << dwAlignmentTransformation << Qt::endl;

		QString strInfoFileName;
		GetInfoFileName(referenceFrame, strInfoFileName);
		stream << strInfoFileName << Qt::endl;

		for (const auto& stackingInfo : m_vLightFrameStackingInfo)
		{
			stream << stackingInfo.m_strInfoFileName << Qt::endl;
			stream << QString::fromStdU16String(stackingInfo.file.generic_u16string().c_str()) << Qt::endl;

			QString strParameters;
			stackingInfo.m_BilinearParameters.ToText(strParameters);
			stream << strParameters << Qt::endl;
		};

		auto bytesWritten = file.write(buffer);
		ZASSERTSTATE(bytesWritten == buffer.size());
		file.close();
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void RemoveStars(CMemoryBitmap* pBitmap, CPixelTransform& PixTransform, const STARVECTOR& vStars)
{
	ZFUNCTRACE_RUNTIME();

	if (pBitmap != nullptr)
	{
		const double fWidth = pBitmap->Width();
		const double fHeight = pBitmap->Height();
		//const bool bMonochrome = pBitmap->IsMonochrome();

		//for (int k = 0; k < vStars.size(); k++)
		for (const auto& star : vStars)
		{
			double			fRadius = star.m_fMeanRadius * 2.35 * 1.0;// /1.5;
			QPointF		ptCenter(star.m_fX, star.m_fY);
			//double			fIntensity = 0;
			//int			lNrIntensities = 0;

			ptCenter = PixTransform.transform(ptCenter);

			for (double i = std::max(0.0, ptCenter.x() - 2.0 * fRadius); i <= std::min(ptCenter.x() + 2.0 * fRadius, fWidth - 1); i++)
			{
				for (double j = std::max(0.0, ptCenter.y() - 2.0 * fRadius); j <= std::min(ptCenter.y() + 2.0 * fRadius, fHeight - 1); j++)
				{
					// Compute the distance to the center
					const double fXDistance = fabs(i - ptCenter.x());
					const double fYDistance = fabs(j - ptCenter.y());
					const double fDistance = sqrt(fXDistance * fXDistance + fYDistance * fYDistance);

					if (fDistance <= fRadius)
					{
						/*
						if (fDistance<=fRadius/2)
						{
							double			fGray;

							pBitmap->GetPixel(i+0.5, j+0.5, fGray);
							fIntensity += fGray;
							lNrIntensities ++;
						};*/
						pBitmap->SetPixel(i + 0.5, j + 0.5, 0.0);
					};
				};
			};
			/*
			if (lNrIntensities)
				fIntensity/=lNrIntensities;

			for (double i = max(0, ptCenter.X - 2.0*fRadius);i<=min(ptCenter.X + 2.0*fRadius, fWidth-1);i++)
			{
				for (double j = max(0, ptCenter.X - 2.0*fRadius);j<=min(ptCenter.Y + 2.0*fRadius, fHeight-1);j++)
				{
					// Compute the distance to the center
					double		fDistance;
					double		fXDistance = fabs(i-ptCenter.X);
					double		fYDistance = fabs(j-ptCenter.Y);
					double		fGray;

					pBitmap->GetPixel(i+0.5, j+0.5, fGray);

					fDistance = sqrt(fXDistance * fXDistance + fYDistance * fYDistance);

					if ((fDistance<=fRadius*2) && (fGray>=fIntensity/4))
						pBitmap->SetPixel(i+0.5, j+0.5, 0.0);
				};
			};*/
		};
/*
		for (int k = 0;k<vStars.size();k++)
		{
			double			fRadius = vStars[k].m_fMeanRadius*2.35;// /1.5;
			QPointF		ptCenter(vStars[k].m_fX, vStars[k].m_fY);
			double			fIntensity = 0;
			int			lNrIntensities = 0;

			ptCenter = PixTransform.Transform(ptCenter);

			for (double i = max(0, ptCenter.X - 2.0*fRadius);i<=min(ptCenter.X + 2.0*fRadius, fWidth-1);i++)
			{
				for (double j = max(0, ptCenter.X - 2.0*fRadius);j<=min(ptCenter.Y + 2.0*fRadius, fHeight-1);j++)
				{
					// Compute the distance to the center
					if (bMonochrome)
					{
						double		fGray;
						pBitmap->GetPixel(i+0.5, j+0.5, fGray);

						if (fGray)
							vGrays.push_back(fGray);
					}
					else
					{
						double		fRed, fGreen, fBlue;
						pBitmap->GetPixel(i+0.5, j+0.5, fRed, fGreen, fBlue);
						if (fRed || fGreen || fBlue)
						{
							vReds.push_back(fRed);
							vGreens.push_back(fGreen);
							vBlues.push_back(fBlue);
						};
					};
				};
			};
			double				fMedianRed   = Median(vReds),
								fMedianGreen = Median(vGreens),
								fMedianBlue  = Median(vBlues),
								fMedianGray  = Median(vGrays);
			for (double i = max(0, ptCenter.X - 2.0*fRadius);i<=min(ptCenter.X + 2.0*fRadius, fWidth-1);i++)
			{
				for (double j = max(0, ptCenter.X - 2.0*fRadius);j<=min(ptCenter.Y + 2.0*fRadius, fHeight-1);j++)
				{
					// Compute the distance to the center
					if (bMonochrome)
					{
						double		fGray;
						pBitmap->GetPixel(i+0.5, j+0.5, fGray);

						if (!fGray)
							pBitmap->SetPixel(i+0.5, j+0.5, fMedianGray);
					}
					else
					{
						double		fRed, fGreen, fBlue;
						pBitmap->GetPixel(i+0.5, j+0.5, fRed, fGreen, fBlue);
						if (!fRed && !fGreen && !fBlue)
							pBitmap->SetPixel(i+0.5, j+0.5, fMedianRed, fMedianGreen, fMedianBlue);
					};
				};
			};
		};*/
	};
};

/* ------------------------------------------------------------------- */

TRANSFORMATIONTYPE CStackingEngine::GetTransformationType()
{
	ZFUNCTRACE_RUNTIME();

	TRANSFORMATIONTYPE TTResult = TT_BILINEAR;
	unsigned int dwAlignmentTransformation = 2;
	Workspace workspace;

	dwAlignmentTransformation = workspace.value("Stacking/AlignmentTransformation", (uint)2).toUInt();

	switch (dwAlignmentTransformation)
	{
	case 1:
		TTResult = TT_LINEAR;
		break;
	case 2:
		TTResult = TT_BILINEAR;
		break;
	case 3:
		TTResult = TT_BISQUARED;
		break;
	case 4:
		TTResult = TT_BICUBIC;
		break;
	case 5:
		TTResult = TT_NONE;
		break;
	};

	return TTResult;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::AddLightFramesToList(CAllStackingTasks& tasks)
{
	ZFUNCTRACE_RUNTIME();

	bool bReferenceFrameFound;
	if (!referenceFrame.empty())
		bReferenceFrameFound = false;
	else
		bReferenceFrameFound = true;

	m_vBitmaps.clear();
	//for (i = 0; i < tasks.m_vTasks.size(); i++)
	for (auto& task : tasks.m_vTasks)
	{
		if (task.m_TaskType == PICTURETYPE_LIGHTFRAME)
		{
			for (auto& bitmap : task.m_vBitmaps)
			{
				CLightFrameInfo lfi;
				lfi.SetBitmap(bitmap.filePath);

				if (lfi.IsRegistered())
				{
					lfi = bitmap;
					lfi.RefreshSuperPixel();

					//
					// m_strReferenceFrame is a CString but contains the reference frame path
					// with / separators rather than \\
					//
					if (!referenceFrame.compare(lfi.filePath))
					{
						lfi.m_bStartingFrame = true;
						bReferenceFrameFound = true;
					};
					m_vBitmaps.push_back(lfi);
				};
			};
		};
	};

	if (!bReferenceFrameFound)
	{
		// Look for the reference frame and add it to the list
		CLightFrameInfo			lfi;
		CFrameInfo				fi;
		if (fi.InitFromFile(referenceFrame, PICTURETYPE_LIGHTFRAME))
		{
			lfi.SetBitmap(referenceFrame);
			if (lfi.IsRegistered())
			{
				lfi = fi;
				lfi.m_bStartingFrame = true;
				lfi.m_bDisabled		 = true;
				m_vBitmaps.push_back(lfi);
			};
		};
	};

	return true;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::ComputeLightFrameOffset(int lBitmapIndice)
{
	// ZFUNCTRACE_RUNTIME();

	bool				bResult = false;
	CBilinearParameters	BilinearParameters;

	{
		const std::lock_guard<std::mutex> lock(mutex);
		bResult = m_StackingInfo.GetParameters(m_vBitmaps[lBitmapIndice].filePath.c_str(), BilinearParameters);
	}

	if (bResult)
	{
		BilinearParameters.Offsets(m_vBitmaps[lBitmapIndice].m_fXOffset, m_vBitmaps[lBitmapIndice].m_fYOffset);
		m_vBitmaps[lBitmapIndice].m_fAngle = BilinearParameters.Angle(m_vBitmaps[lBitmapIndice].RenderedWidth());
		m_vBitmaps[lBitmapIndice].m_BilinearParameters = BilinearParameters;
	}
	else if (GetTransformationType() == TT_NONE)
	{
		// Automatic acknowledgment of the transformation
		const std::lock_guard<std::mutex> lock(mutex);
		m_StackingInfo.AddLightFrame(m_vBitmaps[lBitmapIndice].filePath.c_str(), BilinearParameters);

		bResult = true;
	}
	else if ((m_vBitmaps[lBitmapIndice].m_vStars.size() > 4) &&
		((m_vBitmaps[lBitmapIndice].m_vStars.size() > m_vBitmaps[0].m_vStars.size()/5) ||
		 (m_vBitmaps[lBitmapIndice].m_vStars.size()>=30)))
	{
		// Try to identify patterns in the placement of stars

		STARVECTOR &		vStarsOrg = m_vBitmaps[0].m_vStars;
		STARVECTOR &		vStarsDst = m_vBitmaps[lBitmapIndice].m_vStars;
		CMatchingStars		MatchingStars;

		{
			const std::lock_guard<std::mutex> lock(mutex);
			std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);
			std::sort(vStarsDst.begin(), vStarsDst.end(), CompareStarLuminancy);
		}

		if (!MatchingStars.IsReferenceSet())
		{
			double			fXRatio,
							fYRatio;

			fXRatio = (double)m_vBitmaps[lBitmapIndice].RenderedWidth()/(double)m_vBitmaps[0].RenderedWidth();
			fYRatio = (double)m_vBitmaps[lBitmapIndice].RenderedHeight()/(double)m_vBitmaps[0].RenderedHeight();
			for (size_t i = 0; i < std::min(vStarsOrg.size(), static_cast<STARVECTOR::size_type>(100)); i++)
				MatchingStars.AddReferenceStar(vStarsOrg[i].m_fX * fXRatio, vStarsOrg[i].m_fY * fYRatio);
		};
		MatchingStars.ClearTarget();
		for (size_t i = 0; i < std::min(vStarsDst.size(), static_cast<STARVECTOR::size_type>(100)); i++)
			MatchingStars.AddTargetedStar(vStarsDst[i].m_fX, vStarsDst[i].m_fY);

		MatchingStars.SetSizes(m_vBitmaps[lBitmapIndice].RenderedWidth(), m_vBitmaps[lBitmapIndice].RenderedHeight());
		bResult = MatchingStars.ComputeCoordinateTransformation(BilinearParameters);

		if (bResult)
		{
			BilinearParameters.Offsets(m_vBitmaps[lBitmapIndice].m_fXOffset, m_vBitmaps[lBitmapIndice].m_fYOffset);
			m_vBitmaps[lBitmapIndice].m_fAngle   = BilinearParameters.Angle(m_vBitmaps[lBitmapIndice].RenderedWidth());
			m_vBitmaps[lBitmapIndice].m_BilinearParameters = BilinearParameters;
			MatchingStars.GetVotedPairs(m_vBitmaps[lBitmapIndice].m_vVotedPairs);
			const std::lock_guard<std::mutex> lock(mutex);
			m_StackingInfo.AddLightFrame(m_vBitmaps[lBitmapIndice].filePath.c_str(), BilinearParameters);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

inline bool CompareLightFrameDate (const CLightFrameInfo * plfi1, const CLightFrameInfo * plfi2)
{
	return (plfi2->m_DateTime > plfi1->m_DateTime);
};

bool interpolateCometPositions(CStackingEngine& stackingEngine)
{
	ZFUNCTRACE_RUNTIME();

	if (stackingEngine.LightFrames().empty())
		return false;

	using ValT = double;
	using VecT = std::vector<ValT>;

	VecT times;
	VecT xPositions;
	VecT yPositions;
	times.reserve(8);
	xPositions.reserve(8);
	yPositions.reserve(8);

	const CLightFrameInfo& firstLightframe = stackingEngine.getBitmap(0);

	std::for_each(stackingEngine.LightFrames().cbegin(), stackingEngine.LightFrames().cend(),
		[&times, &xPositions, &yPositions, &firstLightframe](const CLightFrameInfo& lightframe) {
			// We use all lightframes (even those that are below a possible quality threshold), so there's no check for !lightframe.m_bDisabled.
			if (lightframe.m_bComet)
			{
				const QPointF position = lightframe.m_BilinearParameters.transform(QPointF{ lightframe.m_fXComet, lightframe.m_fYComet });
				times.emplace_back(firstLightframe.m_DateTime.secsTo(lightframe.m_DateTime));
				xPositions.push_back(position.x());
				yPositions.push_back(position.y());
			}
		}
	);

	if (times.size() < 2)
		return false;

	const auto average = [](const VecT& vec) {
		return std::accumulate(vec.cbegin(), vec.cend(), 0.0, [](const ValT accu, const ValT value) { return accu + value; }) / static_cast<ValT>(vec.size());
	};
	const auto secondMoment = [](const VecT& vec, const ValT average) {
		return std::accumulate(vec.cbegin(), vec.cend(), 0.0, [average](const ValT accu, const ValT value) { const ValT y = value - average; return accu + y * y; });
	};
	const auto crossCorrelation = [](const VecT& r, const ValT rMean, const VecT& s, const ValT sMean) {
		return std::inner_product(r.cbegin(), r.cend(), s.cbegin(), 0.0,
			[](const ValT product, const ValT accu) { return accu + product; },
			[rMean, sMean](const ValT rVal, const ValT sVal) { return (rVal - rMean) * (sVal - sMean); }
		);
	};

	const ValT tAvg = average(times);
	const ValT xAvg = average(xPositions);
	const ValT yAvg = average(yPositions);
	const ValT tSecondMoment = secondMoment(times, tAvg);

	if (tSecondMoment == 0.0) // All elapsed times are equal?
		return false;

	const ValT xGradient = crossCorrelation(times, tAvg, xPositions, xAvg) / tSecondMoment;
	const ValT xOffset = xAvg - xGradient * tAvg;

	const ValT yGradient = crossCorrelation(times, tAvg, yPositions, yAvg) / tSecondMoment;
	const ValT yOffset = yAvg - yGradient * tAvg;

	// Check if the deviations of the given comet positions is within +- 3*sigma of the linear regression.
	// Works with at least 3 positions.
	// It is an indication of an incorrectly marked comet position if the deviation is too large.
	// So we present a warning.
	if (times.size() > 2)
	{
		VecT deviations{ times };
		auto vIt = deviations.begin();
		auto tIt = times.cbegin();
		for (const ValT val : xPositions)
			*vIt++ = val - (*tIt++ * xGradient) - xOffset; // Given x-pos minus estimation_from_linear_regression (= time * gradient + offset)
		const ValT xVariance = secondMoment(deviations, average(deviations)) / static_cast<ValT>(times.size() - 1); // sigma^2 of x-deviations

		for (const ValT d : deviations)
			if ((d * d) > 9 * xVariance)
			{
				// TO DO
				// Shall we show a warning if the x-deviation of this position is larger than 3*sigma?
			}

		vIt = deviations.begin();
		tIt = times.cbegin();
		for (const ValT val : yPositions)
			*vIt++ = val - (*tIt++ * yGradient) - yOffset; // Given y-pos minus estimation_from_linear_regression (= time * gradient + offset)
		const ValT yVariance = secondMoment(deviations, average(deviations)) / static_cast<ValT>(times.size() - 1); // sigma^2 of y-deviations

		for (const ValT d : deviations)
			if ((d * d) > 9 * yVariance)
			{
				// TO DO
				// Shall we show a warning if the y-deviation of this position is larger than 3*sigma?
			}

		constexpr ValT MaxVariance = 25;
		ZTRACE_RUNTIME("interpolateCometPositions: x-stddev=%.1f, y-stddev=%.1f", std::sqrt(xVariance), std::sqrt(yVariance));

		if (xVariance > MaxVariance || yVariance > MaxVariance)
		{
			// Show a warning if the standard deviation is larger than 5 pixels
			const QString errorMessage{ QCoreApplication::translate(
				"StackingEngine",
				"The standard deviations of the marked comet positions are unusually large (x: %L1 pixels, y: %L2 pixels).\nThey should be smaller than %L3 pixels, so please check the defined comet centres.")
				.arg(std::sqrt(xVariance), 0, 'f', 1)
				.arg(std::sqrt(yVariance), 0, 'f', 1)
				.arg(std::sqrt(MaxVariance), 0, 'f', 1) };
			DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Warning);
		}
	}

	for (int i = 0; CLightFrameInfo& lightframe : stackingEngine.LightFrames())
	{
		// All active lightframes (those that are NOT disabled) without a comet position will get one using the linear regression.
		if (!lightframe.m_bComet && !lightframe.m_bDisabled)
		{
			const ValT time = static_cast<ValT>(firstLightframe.m_DateTime.secsTo(lightframe.m_DateTime));
			lightframe.m_fXComet = xGradient * time + xOffset;
			lightframe.m_fYComet = yGradient * time + yOffset;
			lightframe.m_bTransformedCometPosition = true;
			lightframe.m_bComet = true;
			stackingEngine.incCometStackableIfBitmapHasComet(i);
		}
		++i;
	}

	return true;
}

void CStackingEngine::ComputeMissingCometPositions()
{
	ZFUNCTRACE_RUNTIME();

	if (m_lNrCometStackable >= 2)
	{
		// Add all the valid light frames to a vector
		std::vector<CLightFrameInfo*> vpLightFrames;

		for (auto& bitmap : m_vBitmaps)
		{
			if (!bitmap.m_bDisabled)
				vpLightFrames.push_back(&bitmap);
		};

		// Now sort the list by ascending date/time
		std::sort(vpLightFrames.begin(), vpLightFrames.end(), CompareLightFrameDate);

		std::vector<int> vNewComet;

		for (size_t i = 1; !vpLightFrames.empty() && i < vpLightFrames.size() - 1; i++)
		{
			//if (!vpLightFrames[i]->m_bComet)
			{
				CLightFrameInfo* pPreviousComet = nullptr;
				CLightFrameInfo* pNextComet = nullptr;
				int64_t lPreviousIndex{ 0 }, lNextIndex{ 0 };

				for (ptrdiff_t j = i - 1; j >= 0 && pPreviousComet == nullptr; j--)
				{
					if (vpLightFrames[j]->m_bComet)
					{
						pPreviousComet = vpLightFrames[j];
						lPreviousIndex = j;
					};
				};

				for (size_t j = i + 1; j < vpLightFrames.size() && pNextComet == nullptr; j++)
				{
					if (vpLightFrames[j]->m_bComet)
					{
						pNextComet = vpLightFrames[j];
						lNextIndex = j;
					};
				};

				if (pPreviousComet != nullptr && pNextComet != nullptr)
				{
					// Try to find another previous and/or next computed comet position
					// so that the elapsed time between the two is less than 12 hours
					bool bContinue = false;
					do
					{
						bool bFound = false;
						qint64 elapsedSeconds;

						bContinue = false;
						for (ptrdiff_t j = lPreviousIndex - 1; j >= 0 && !bFound; j--)
						{
							if (vpLightFrames[j]->m_bComet)
							{
								elapsedSeconds = vpLightFrames[j]->m_DateTime.secsTo(pNextComet->m_DateTime);
								if (elapsedSeconds / 3600 < 12)
								{
									bFound = true;
									bContinue = true;
									pPreviousComet = vpLightFrames[j];
									lPreviousIndex = j;
								};
							};
						};
						bFound = false;
						for (size_t j = lNextIndex + 1; j < vpLightFrames.size() && !bFound; j++)
						{
							if (vpLightFrames[j]->m_bComet)
							{
								elapsedSeconds = pPreviousComet->m_DateTime.secsTo(vpLightFrames[j]->m_DateTime);
								if (elapsedSeconds / 3600 < 12)
								{
									bFound = true;
									bContinue = true;
									pNextComet = vpLightFrames[j];
									lNextIndex = j;
								};
							};
						};
					} while (bContinue);

					// Compute the comet position in the two frames
					QPointF			ptPreviousComet = QPointF(pPreviousComet->m_fXComet, pPreviousComet->m_fYComet);
					QPointF			ptNextComet     = QPointF(pNextComet->m_fXComet, pNextComet->m_fYComet);

					ptPreviousComet = pPreviousComet->m_BilinearParameters.transform(ptPreviousComet);
					ptNextComet		= pNextComet->m_BilinearParameters.transform(ptNextComet);

					qint64				fElapsed2,
										fElapsedCurrent;

					fElapsed2 = pPreviousComet->m_DateTime.secsTo(pNextComet->m_DateTime);
					fElapsedCurrent = pPreviousComet->m_DateTime.secsTo(vpLightFrames[i]->m_DateTime);

					if (fElapsed2)
					{
						QPointF			ptCurrentComet;
						double				fAdvance = fElapsedCurrent/fElapsed2;

						ptCurrentComet.rx() = ptPreviousComet.x() + fAdvance * (ptNextComet.x() - ptPreviousComet.x());
						ptCurrentComet.ry() = ptPreviousComet.y() + fAdvance * (ptNextComet.y() - ptPreviousComet.y());

						// Set the comet position - already shifted
						vNewComet.push_back(static_cast<int>(i));
						vpLightFrames[i]->m_bTransformedCometPosition = true;
						vpLightFrames[i]->m_fXComet = ptCurrentComet.x();
						vpLightFrames[i]->m_fYComet = ptCurrentComet.y();

						if (!vpLightFrames[i]->m_bComet)
							m_lNrCometStackable++;
					};

					/*QPointF			ptTestComet = QPointF(vpLightFrames[i]->m_fXComet, vpLightFrames[i]->m_fYComet);
					vpLightFrames[i]->m_BilinearParameters.Transform(ptTestComet);*/
				}
			}
		}

		for (const int cometIndex : vNewComet)
			vpLightFrames[cometIndex]->m_bComet = true;
	}
}


// Returns:
//   true:  offsets have been computed.
//   false: offset calculation was stopped by pressing "Cancel".
bool computeOffsets(CStackingEngine* const pStackingEngine, ProgressBase* const pProg, const int nrBitmaps)
{
	ZFUNCTRACE_RUNTIME();
	const int nrProcessors = CMultitask::GetNrProcessors();

	std::atomic_bool stop{ false };
	std::atomic<int> nLoopCount{ 1 };
	const QString strText(QCoreApplication::translate("StackingEngine", "Computing offsets", "IDS_COMPUTINGOFFSETS"));
	if (pProg != nullptr)
		pProg->Progress1(strText, 0);

#pragma omp parallel for schedule(dynamic) default(shared) shared(stop, nLoopCount, strText) if(nrProcessors > 1)
	for (int i = 1; i < nrBitmaps; ++i)
	{
		// OpenMP loops need to loop till the end, breaking earlier is difficult. 
		// Therefore, if "Cancel" has been pressed, we finish the loop by calling continue.
		if (stop.load())
			continue;

		if (omp_get_thread_num() == 0 && pProg != nullptr)
			pProg->Progress1(strText, nLoopCount.load());

		if (pStackingEngine->ComputeLightFrameOffset(i))
		{
			pStackingEngine->getBitmap(i).m_bDisabled = false;
			pStackingEngine->incStackable();
			pStackingEngine->incCometStackableIfBitmapHasComet(i);

 			if (omp_get_thread_num() == 0 && pProg != nullptr)
 			{
				pProg->Progress1(strText, nLoopCount.load());
 				stop = pProg->IsCanceled();
 			}
		}
		else
		{
			if (omp_get_thread_num() == 0 && pProg != nullptr)
			{
				pProg->Progress1(strText, nLoopCount.load());
				stop = pProg->IsCanceled();
			}
			pStackingEngine->getBitmap(i).m_bDisabled = true;			
		}

		++nLoopCount; // Note: For atomic<> ++x is faster than x++.
	}
	return !stop;
}


void CStackingEngine::ComputeOffsets()
{
	ZFUNCTRACE_RUNTIME();

	if (m_vBitmaps.empty())
		return;

	constexpr auto QualityComp = [](const CLightFrameInfo& l, const CLightFrameInfo& r)
	{
		if (l.m_bStartingFrame)
			return true;
		if (r.m_bStartingFrame)
			return false;
		return l.quality > r.quality;
	};
	std::ranges::sort(m_vBitmaps, QualityComp);

	if (m_vBitmaps[0].m_bDisabled)
		m_lNrStackable = 0;
	else
		m_lNrStackable = std::min(static_cast<int>(m_vBitmaps.size()), 1);
	m_lNrCometStackable = 0;
	const QString strText(QCoreApplication::translate("StackingEngine", "Computing offsets", "IDS_COMPUTINGOFFSETS"));

	const int lLast = static_cast<int>(m_vBitmaps.size() * m_fKeptPercentage / 100.0);
	if (m_pProgress)
		m_pProgress->Start1(strText, lLast, true);

	// The first bitmap is the best one
	if (m_vBitmaps.size() > 1)
	{
		auto& bitmapZero = m_vBitmaps[0];
		std::sort(bitmapZero.m_vStars.begin(), bitmapZero.m_vStars.end());

		std::for_each(m_vBitmaps.begin() + 1, m_vBitmaps.end(), [](auto& bitmap) { bitmap.m_bDisabled = true; });

		if (bitmapZero.m_bComet)
			++m_lNrCometStackable;

		m_StackingInfo.SetReferenceFrame(bitmapZero.filePath.c_str());

		if (computeOffsets(this, this->m_pProgress, lLast)) // Offset calculation was successful (not stopped by pressing "Cancel")
		{
			interpolateCometPositions(*this);
//			ComputeMissingCometPositions();
			m_StackingInfo.Save();
		}
	}
}

/* ------------------------------------------------------------------- */

bool	CStackingEngine::isLightFrameStackable(const fs::path& file)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	for (int i = 0;i<m_vBitmaps.size() && !bResult;i++)
	{
		if (!m_vBitmaps[i].filePath.compare(file))
		{
			if (!m_vBitmaps[i].m_bDisabled)
				bResult = true;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool	CStackingEngine::RemoveNonStackableLightFrames(CAllStackingTasks & tasks)
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = false;

	for (CTaskInfo& LightTask : tasks.m_vTasks)
	{
		if (LightTask.m_TaskType == PICTURETYPE_LIGHTFRAME)
		{
			FRAMEINFOVECTOR vNewList;
			//for (j = 0; j < LightTask.m_vBitmaps.size(); j++)
			for (const auto& bitmap : LightTask.m_vBitmaps)
			{
				if (isLightFrameStackable(bitmap.filePath))
					vNewList.push_back(bitmap);
			};

			// Remove non stackable light frames from the list
			if (vNewList.size() < LightTask.m_vBitmaps.size())
				LightTask.m_vBitmaps = vNewList;

			// If the list is empty - consider that the task is done
			if (vNewList.empty())
				LightTask.m_bDone = true;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackingEngine::GetResultISOSpeed()
{
	ZFUNCTRACE_RUNTIME();

	m_lISOSpeed = m_vBitmaps[0].m_lISOSpeed;
	for (int i = 1;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			if (m_lISOSpeed != m_vBitmaps[i].m_lISOSpeed)
				m_lISOSpeed = 0;
		};
	};
};

/* ------------------------------------------------------------------- */

void CStackingEngine::GetResultGain()
{
	ZFUNCTRACE_RUNTIME();

	m_lGain = m_vBitmaps[0].m_lGain;
	for (int i = 1;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			if (m_lGain != m_vBitmaps[i].m_lGain)
				m_lGain = -1;
		};
	};
};

/* ------------------------------------------------------------------- */

void	CStackingEngine::GetResultDateTime()
{
	m_DateTime = m_vBitmaps[0].m_DateTime;
};

/* ------------------------------------------------------------------- */

void	CStackingEngine::GetResultExtraInfo()
{
	for (int i = 0;i<m_vBitmaps[0].m_ExtraInfo.m_vExtras.size();i++)
	{
		if (m_vBitmaps[0].m_ExtraInfo.m_vExtras[i].m_bPropagate)
			m_ExtraInfo.AddInfo(m_vBitmaps[0].m_ExtraInfo.m_vExtras[i]);
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

inline void ExpandWithPoint(int & lLeft, int & lRight, int & lTop, int & lBottom, const QPointF & pt)
{
	lLeft	= min(lLeft, static_cast<int>(pt.x()));
	lRight	= max(lRight, static_cast<int>(pt.x()));
	lTop	= min(lTop, static_cast<int>(pt.y()));
	lBottom = max(lBottom, static_cast<int>(pt.y()));
};

DSSRect CStackingEngine::computeLargestRectangle()
{
	ZFUNCTRACE_RUNTIME();
	DSSRect result;

	int				i;
	bool			bFirst = true;
	int				lLeft = 0,
					lRight = 0,
					lTop = 0,
					lBottom = 0;

	for (i = 0;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			QPointF			pt1(0, 0),
							pt2(0, m_vBitmaps[i].RenderedHeight()),
							pt3(m_vBitmaps[i].RenderedWidth(), 0),
							pt4(m_vBitmaps[i].RenderedWidth(), m_vBitmaps[i].RenderedHeight()),
							pt5(0, m_vBitmaps[i].RenderedHeight()/2),
							pt6(m_vBitmaps[i].RenderedWidth(), m_vBitmaps[i].RenderedHeight()/2),
							pt7(m_vBitmaps[i].RenderedWidth()/2, 0),
							pt8(m_vBitmaps[i].RenderedWidth()/2, m_vBitmaps[i].RenderedHeight());

			CPixelTransform		PixTransform(m_vBitmaps[i].m_BilinearParameters);

			PixTransform.SetPixelSizeMultiplier(m_lPixelSizeMultiplier);

			pt1 = PixTransform.transform(pt1);
			pt2 = PixTransform.transform(pt2);
			pt3 = PixTransform.transform(pt3);
			pt4 = PixTransform.transform(pt4);
			pt5 = PixTransform.transform(pt5);
			pt6 = PixTransform.transform(pt6);
			pt7 = PixTransform.transform(pt7);
			pt8 = PixTransform.transform(pt8);

			if (bFirst)
			{
				lLeft = lRight = pt1.x();
				lTop  = lBottom = pt1.y();
				bFirst = false;
			}
			else
			{
				ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt1);
			}

			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt2);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt3);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt4);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt5);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt6);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt7);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt8);
		};
	};

	result.setCoords(lLeft, lTop, lRight, lBottom);
	return result;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::computeSmallestRectangle(DSSRect & rc)
{
	ZFUNCTRACE_RUNTIME();

	bool			bResult = false;
	int				i;
	bool			bFirst = true;
	int				lLeft = 0,
					lRight = 0,
					lTop = 0,
					lBottom = 0;

	for (i = 0;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			QPointF			pt1(0, 0),
							pt2(0, m_vBitmaps[i].RenderedHeight()),
							pt3(m_vBitmaps[i].RenderedWidth(), 0),
							pt4(m_vBitmaps[i].RenderedWidth(), m_vBitmaps[i].RenderedHeight());

			CPixelTransform		PixTransform(m_vBitmaps[i].m_BilinearParameters);

			PixTransform.SetPixelSizeMultiplier(m_lPixelSizeMultiplier);

			pt1 = PixTransform.transform(pt1);
			pt2 = PixTransform.transform(pt2);
			pt3 = PixTransform.transform(pt3);
			pt4 = PixTransform.transform(pt4);

			if (bFirst)
			{
				lLeft = pt1.x();	lRight = pt4.x();
				lTop  = pt1.y();	lBottom = pt4.y();
				bFirst = false;
			}
			else
			{
				//
				// Code changes contributed by Peter Wolsley to correct incorrect image cropping
				// of intersection mode stacking where a meridian flip has been done. 
				//
				if (pt1.x() > pt4.x())	// Meridian flipped image
				{
					lLeft = max(max(lLeft, static_cast<int>(pt4.x())), static_cast<int>(pt3.x()));
					lRight = min(min(lRight, static_cast<int>(pt1.x())), static_cast<int>(pt2.x()));
					lTop = max(max(lTop, static_cast<int>(pt4.y())), static_cast<int>(pt2.y()));
					lBottom = min(min(lBottom, static_cast<int>(pt1.y())), static_cast<int>(pt3.y()));
				}
				else                    // Same orientation as reference frame
				{
					lLeft = max(max(lLeft, static_cast<int>(pt1.x())), static_cast<int>(pt2.x()));
					lRight = min(min(lRight, static_cast<int>(pt4.x())), static_cast<int>(pt3.x()));
					lTop = max(max(lTop, static_cast<int>(pt1.y())), static_cast<int>(pt3.y()));
					lBottom = min(min(lBottom, static_cast<int>(pt4.y())), static_cast<int>(pt2.y()));
				}
			};
		};
	};

	if ((lLeft+50 < lRight) && (lTop+50 < lBottom))
	{
		rc.setCoords(lLeft + 2, lTop + 2, lRight - 2, lBottom - 2);
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
int CStackingEngine::findBitmapIndex(const fs::path& file)
{
	ZFUNCTRACE_RUNTIME();

	for (size_t i = 0; i < m_vBitmaps.size(); i++)
	{
		if (m_vBitmaps[i].filePath.compare(file) == 0)
		{
			return static_cast<int>(i);
		}
	};

	return -1;
};

/* ------------------------------------------------------------------- */

void CStackingEngine::ComputeBitmap()
{
	ZFUNCTRACE_RUNTIME();

	if (static_cast<bool>(m_pMasterLight) && m_pMasterLight->GetNrAddedBitmaps() != 0)
	{
		if (m_pProgress != nullptr)
		{
			QString strMethod;
			FormatFromMethod(strMethod, m_pLightTask->m_Method, m_pLightTask->m_fKappa, m_pLightTask->m_lNrIterations);

			const QString strText(QCoreApplication::translate("StackingEngine", "Computing Final Picture (%1)", "IDS_COMPUTINGMEDIANLIGHT").arg(strMethod));

			m_pProgress->Start1(strText, 1, true);
			m_pProgress->Progress1(strText, 0);
			m_pProgress->SetJointProgress(true);
		}

		const auto imageIndexVector = [](const auto& cometShifts) {
			std::vector<int> vImageOrder(cometShifts.size(), 0);
			std::ranges::transform(cometShifts, std::begin(vImageOrder), &CImageCometShift::m_lImageIndex);
			return vImageOrder;
		};

		ZTRACE_RUNTIME("Compute resulting bitmap");
		if (!m_vCometShifts.empty())
		{
			std::ranges::sort(m_vCometShifts, std::less{});
			m_pMasterLight->SetImageOrder(imageIndexVector(m_vCometShifts));

			const double	fX1 = m_vCometShifts.cbegin()->m_fXShift, // First one
							fY1 = m_vCometShifts.cbegin()->m_fYShift,
							fX2 = m_vCometShifts.crbegin()->m_fXShift, // Last one
							fY2 = m_vCometShifts.crbegin()->m_fYShift;
			m_fStarTrailsAngle = atan2(fY2 - fY1, fX2 - fX1);
		}
		m_pOutput = m_pMasterLight->GetResult(m_pProgress);
		m_pMasterLight.reset();

		if (m_pProgress)
			m_pProgress->SetJointProgress(false);
	}
}

/* ------------------------------------------------------------------- */

bool CStackingEngine::AdjustEntropyCoverage()
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = false;

	if (static_cast<bool>(m_pEntropyCoverage))
	{
		ZTRACE_RUNTIME("Adjust Entropy Coverage");

		const bool bColor = !m_pEntropyCoverage->IsMonochrome();

		for (int j = 0; j < m_pEntropyCoverage->Height(); j++)
		{
			for (int i = 0; i < m_pEntropyCoverage->Width(); i++)
			{
				const auto scaleColor = [](const double color, const double entropy) -> double
				{
					return entropy == 0.0 ? color : color / entropy;
				};

				if (bColor)
				{
					double fRed, fGreen, fBlue;
					double fEntropyRed, fEntropyGreen, fEntropyBlue;

					m_pEntropyCoverage->GetValue(i, j, fEntropyRed, fEntropyGreen, fEntropyBlue);
					m_pOutput->GetValue(i, j, fRed, fGreen, fBlue);

					fRed = scaleColor(fRed, fEntropyRed);
					fGreen = scaleColor(fGreen, fEntropyGreen);
					fBlue = scaleColor(fBlue, fEntropyBlue);

					m_pOutput->SetValue(i, j, fRed, fGreen, fBlue);
				}
				else
				{
					double fGray, fEntropyGray;
					m_pEntropyCoverage->GetValue(i, j, fEntropyGray);
					m_pOutput->GetValue(i, j, fGray);
					m_pOutput->SetValue(i, j, scaleColor(fGray, fEntropyGray));
				}
			}
		}
		m_pEntropyCoverage.reset();
		bResult = true;
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CStackingEngine::AdjustBayerDrizzleCoverage()
{
	ZFUNCTRACE_RUNTIME();

	if (m_vPixelTransforms.empty())
		return false;

	ZTRACE_RUNTIME("Adjust Bayer Drizzle Coverage");

	std::unique_ptr<C96BitFloatColorBitmap> pCover = std::make_unique<C96BitFloatColorBitmap>();
	pCover->Init(m_rcResult.width(), m_rcResult.height());

	QString strText = QCoreApplication::translate("StackingEngine", "Stacking - Adjust Bayer - Compute adjustment", "IDS_STACKING_COMPUTINGADJUSTMENT");
	if (m_pProgress != nullptr)
		m_pProgress->Start1(strText, static_cast<int>(m_vPixelTransforms.size()), false);

	float* const pRed = pCover->GetRedPixel(0, 0);
	float* const pGreen = pCover->GetGreenPixel(0, 0);
	float* const pBlue = pCover->GetBluePixel(0, 0);
	ZASSERTSTATE((reinterpret_cast<ptrdiff_t>(pRed) & (std::atomic_ref<float>::required_alignment - 1)) == 0);
	ZASSERTSTATE((reinterpret_cast<ptrdiff_t>(pRed + 1) & (std::atomic_ref<float>::required_alignment - 1)) == 0);
	ZASSERTSTATE((reinterpret_cast<ptrdiff_t>(pGreen) & (std::atomic_ref<float>::required_alignment - 1)) == 0);
	ZASSERTSTATE((reinterpret_cast<ptrdiff_t>(pBlue) & (std::atomic_ref<float>::required_alignment - 1)) == 0);

	for (int lNrBitmaps = 1; const CPixelTransform& PixTransform : m_vPixelTransforms)
	{
		if (m_pProgress != nullptr)
		{
			strText = QCoreApplication::translate("StackingEngine", "Compute adjustment %1 of %2", "IDS_COMPUTINGADJUSTMENT").arg(lNrBitmaps).arg(m_vPixelTransforms.size());
			m_pProgress->Progress1(strText, lNrBitmaps);
			m_pProgress->Start2(QString(" "), m_rcResult.width() * m_rcResult.height());
		}

#pragma omp parallel for schedule(static, 250) default(shared) shared(PixTransform) if(CMultitask::GetNrProcessors() > 1)
		for (int j = 0; j < m_rcResult.height(); ++j)
		{
			for (const int i : std::views::iota(0, m_rcResult.width()))
			{
				const QPointF ptOut = PixTransform.transform(QPointF(i, j));

				if (DSSRect{ 0, 0, m_rcResult.width(), m_rcResult.height() }.contains(ptOut))
				{
					std::array<int, 4> xcoords, ycoords;
					std::array<double, 4> percents = ComputeAll4PixelDispatches(ptOut, xcoords, ycoords);

					for (const int n : { 0, 1, 2, 3 })
					{
						const int x = xcoords[n];
						const int y = ycoords[n];

						// For each plane adjust the values
						if (const float percent = static_cast<float>(percents[n]);
							percent > 0.0f
							&& x >= 0 && x < m_rcResult.width()
							&& y >= 0 && y < m_rcResult.height())
						{
							const auto update = [offset = static_cast<size_t>(m_rcResult.width()) * y + x, percent](float* const vals)
							{
								std::atomic_ref{ vals[offset] } += percent;
							};

							switch (GetBayerColor(i, j, m_InputCFAType))
							{
							case BAYER_RED:   //fRedCover   += pixDispatch.m_fPercentage; break;
								update(pRed); break;
							case BAYER_GREEN: //fGreenCover += pixDispatch.m_fPercentage; break;
								update(pGreen); break;
							case BAYER_BLUE:  //fBlueCover  += pixDispatch.m_fPercentage; break;
								update(pBlue); break;
							}
						}
					}
				}
			}
			if (m_pProgress != nullptr && omp_get_thread_num() == 0)
				m_pProgress->Progress2(j * m_rcResult.width());
		}

		if (m_pProgress != nullptr && omp_get_thread_num() == 0)
			m_pProgress->End2();
		++lNrBitmaps;
	}

	m_vPixelTransforms.clear();

	if (m_pProgress != nullptr)
	{
		strText = QCoreApplication::translate("StackingEngine", "Stacking - Adjust Bayer - Apply adjustment", "IDS_STACKING_APPLYINGADJUSTMENT");
		m_pProgress->Start1(strText, 2, false);
		strText = QCoreApplication::translate("StackingEngine", "Compute maximum adjustment", "IDS_STACKING_COMPUTEMAXADJUSTMENT");
		m_pProgress->Start2(strText, m_rcResult.width() * m_rcResult.height());
	}

	//
	// Compute the maximum coverage
	//
	double fMaxCoverage = 0;
	BitmapIteratorConst<const C96BitFloatColorBitmap*> it{ pCover.get() };
	for (int j = 0; j < m_rcResult.height(); j++)
	{
		it.Reset(0, j);
		for (int i = 0; i < m_rcResult.width(); i++, ++it)
		{
			double fRedCover, fGreenCover, fBlueCover;

//				pCover->GetValue(i, j, fRedCover, fGreenCover, fBlueCover);
			it.GetPixel(fRedCover, fGreenCover, fBlueCover);

			fMaxCoverage = std::max(fMaxCoverage, fRedCover);
			fMaxCoverage = std::max(fMaxCoverage, fGreenCover);
			fMaxCoverage = std::max(fMaxCoverage, fBlueCover);
		}

		if (m_pProgress != nullptr)
			m_pProgress->Progress2(j * m_rcResult.width());
	}

	if (m_pProgress != nullptr)
	{
		m_pProgress->End2();
		m_pProgress->Progress1(1);
		strText = QCoreApplication::translate("StackingEngine", "Applying adjustment", "IDS_STACKING_APPLYADJUSTMENT");
		m_pProgress->Start2(strText, m_rcResult.width() * m_rcResult.height());
	}

	//
	// Adjust the coverage of all pixels
	//
	BitmapIterator outIt{ m_pOutput };
	for (int j = 0; j < m_rcResult.height(); j++)
	{
		it.Reset(0, j);
		outIt.Reset(0, j);
		for (int i = 0; i < m_rcResult.width(); i++, ++it, ++outIt)
		{
			double fRedCover, fGreenCover, fBlueCover;
			double fRed, fGreen, fBlue;

//				pCover->GetValue(i, j, fRedCover, fGreenCover, fBlueCover);
			it.GetPixel(fRedCover, fGreenCover, fBlueCover);
//				m_pOutput->GetValue(i, j, fRed, fGreen, fBlue);
			outIt.GetPixel(fRed, fGreen, fBlue);

			if (fRedCover > 0)
				fRed *= fMaxCoverage / fRedCover;
			if (fGreenCover > 0)
				fGreen *= fMaxCoverage / fGreenCover;
			if (fBlueCover > 0)
				fBlue *= fMaxCoverage / fBlueCover;

//				m_pOutput->SetValue(i, j, fRed, fGreen, fBlue);
			outIt.SetPixel(fRed, fGreen, fBlue);
		}

		if (m_pProgress != nullptr)
			m_pProgress->Progress2(j * m_rcResult.width());
	}

	if (m_pProgress != nullptr)
		m_pProgress->End2();

	return true;
}

/* ------------------------------------------------------------------- */

bool CStackingEngine::SaveCalibratedAndRegisteredLightFrame(CMemoryBitmap* pBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	if (!currentLightFrame.empty() && pBitmap != nullptr)
	{
		const QFileInfo fileInfo(currentLightFrame);		
		const QString strPath(fileInfo.path() + QDir::separator());
		const QString strBaseName(fileInfo.baseName());
		QString strOutputFile;

		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile = strPath + strBaseName + ".reg.tif";
		else
		{
			QString strFitsExt;
			GetFITSExtension(fileInfo.absoluteFilePath(), strFitsExt);
			strOutputFile = strPath + strBaseName + ".reg" + strFitsExt;
		}
		strOutputFile = QDir::toNativeSeparators(strOutputFile);

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Registered and Calibrated image in %1", "IDS_SAVINGINTERMEDIATE").arg(strOutputFile));
			m_pProgress->Start2(strText, 0);
		};
		const QString description("Registered and Calibrated light frame");
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile.toStdU16String(), pBitmap, m_pProgress, TF_UNKNOWN, TC_NONE, description, m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure, m_pLightTask->m_fAperture);
		else
			bResult = WriteFITS(strOutputFile.toStdU16String(), pBitmap, m_pProgress, description, m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure);
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::SaveCalibratedLightFrame(std::shared_ptr<CMemoryBitmap> pBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	if (!currentLightFrame.empty() && static_cast<bool>(pBitmap))
	{
		const QFileInfo fileInfo(currentLightFrame);
		const QString strPath(fileInfo.path() + QDir::separator());
		const QString strBaseName(fileInfo.baseName());
		QString strOutputFile;

		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile = strPath + strBaseName + ".cal.tif";
		else
		{
			QString strFitsExt;
			GetFITSExtension(fileInfo.absoluteFilePath(), strFitsExt);
			strOutputFile = strPath + strBaseName + ".cal" + strFitsExt;
		}
		strOutputFile = QDir::toNativeSeparators(strOutputFile);

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(strOutputFile));
			m_pProgress->Start2(strText, 0);
		};

		std::shared_ptr<CMemoryBitmap> pOutBitmap;

		if (m_bSaveCalibratedDebayered)
		{
			// Debayer the image
			if (!DebayerPicture(pBitmap.get(), pOutBitmap, m_pProgress))
				pOutBitmap = pBitmap;
		}
		else
			pOutBitmap = pBitmap;

		// Check and remove super pixel settings
		CFATRANSFORMATION CFATransform = CFAT_NONE;
		CCFABitmapInfo* pCFABitmapInfo = dynamic_cast<CCFABitmapInfo*>(pOutBitmap.get());
		if (pCFABitmapInfo != nullptr)
		{
			CFATransform = pCFABitmapInfo->GetCFATransformation();
			if (CFATransform == CFAT_SUPERPIXEL)
				pCFABitmapInfo->UseBilinear(true);
		}
		const QString description("Calibrated light frame");
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile.toStdU16String(), pOutBitmap.get(), m_pProgress, TF_UNKNOWN, TC_NONE, description, m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure, m_pLightTask->m_fAperture);
		else
			bResult = WriteFITS(strOutputFile.toStdU16String(), pOutBitmap.get(), m_pProgress, description, m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure);

		if ((CFATransform == CFAT_SUPERPIXEL) && pCFABitmapInfo)
			pCFABitmapInfo->UseSuperPixels(true);
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::SaveDeltaImage( CMemoryBitmap* pBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	if (!currentLightFrame.empty() && pBitmap != nullptr)
	{
		const QFileInfo fileInfo(currentLightFrame);
		const QString strPath(fileInfo.path() + QDir::separator());
		const QString strBaseName(fileInfo.baseName());
		QString strOutputFile;

		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile = strPath + strBaseName + ".cosmetic.tif";
		else
		{
			QString strFitsExt;
			GetFITSExtension(fileInfo.absoluteFilePath(), strFitsExt);
			strOutputFile = strPath + strBaseName + ".cosmetic" + strFitsExt;
		}
		strOutputFile = QDir::toNativeSeparators(strOutputFile);

		if (m_pProgress)
			m_pProgress->Start2(0);
		
		const QString description("Delta Cosmetic Image");
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile.toStdU16String(), pBitmap, m_pProgress, TF_UNKNOWN, TC_NONE, description);
		else
			bResult = WriteFITS(strOutputFile.toStdU16String(), pBitmap, m_pProgress, description);
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::SaveCometImage(CMemoryBitmap* pBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = false;

	if (!currentLightFrame.empty() && pBitmap != nullptr)
	{
		const QFileInfo fileInfo(currentLightFrame);
		const QString strPath(fileInfo.path() + QDir::separator());
		const QString strBaseName(fileInfo.baseName());
		QString strOutputFile;

		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile = strPath + strBaseName + ".Comet.tif";
		else
		{
			QString strFitsExt;
			GetFITSExtension(fileInfo.absoluteFilePath(), strFitsExt);
			strOutputFile = strPath + strBaseName + ".Comet" + strFitsExt;
		}
		strOutputFile = QDir::toNativeSeparators(strOutputFile);

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(strOutputFile));
			m_pProgress->Start2(strText, 0);
		};
		const QString description("Comet alone");
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile.toStdU16String(), pBitmap, m_pProgress, description);
		else
			bResult = WriteFITS(strOutputFile.toStdU16String(), pBitmap, m_pProgress, description);
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::SaveCometlessImage(CMemoryBitmap* pBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = false;

	if (!currentLightFrame.empty() && pBitmap != nullptr)
	{
		const QFileInfo fileInfo(currentLightFrame);
		const QString strPath(fileInfo.path() + QDir::separator());
		const QString strBaseName(fileInfo.baseName());
		QString strOutputFile;

		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile = strPath + strBaseName + ".Cometless.tif";
		else
		{
			QString strFitsExt;
			GetFITSExtension(fileInfo.absoluteFilePath(), strFitsExt);
			strOutputFile = strPath + strBaseName + ".Cometless" + strFitsExt;
		}
		strOutputFile = QDir::toNativeSeparators(strOutputFile);

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(strOutputFile));
			m_pProgress->Start2(strText, 0);
		}

		const QString description("Cometless image");
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile.toStdU16String(), pBitmap, m_pProgress, description);
		else
			bResult = WriteFITS(strOutputFile.toStdU16String(), pBitmap, m_pProgress, description);

		if (m_pProgress)
			m_pProgress->End2();
	}

	return bResult;
}

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CStackTask
{
private:
	CStackingEngine* m_pStackingEngine;
	ProgressBase* m_pProgress;
	std::vector<QPoint> m_vLockedPixels;

public:
	CEntropyInfo m_EntropyWindow;
	CMemoryBitmap* m_pBitmap;
	CPixelTransform m_PixTransform;
	CTaskInfo* m_pLightTask;
	CBackgroundCalibration m_BackgroundCalibration;
	DSSRect m_rcResult;
	std::shared_ptr<CMemoryBitmap> m_pTempBitmap;
	std::shared_ptr<CMemoryBitmap> m_pOutput;
	std::shared_ptr<CMemoryBitmap> m_pEntropyCoverage;
	AvxEntropy* m_pAvxEntropy;
	int m_lPixelSizeMultiplier;
	bool m_bColor;

public:
	CStackTask() = delete;
	~CStackTask() = default;
	CStackTask(CMemoryBitmap* pBitmap, ProgressBase* pProgress) :
		m_pStackingEngine {nullptr},
		m_pProgress{ pProgress },
		m_vLockedPixels{},
		m_EntropyWindow {},
		m_pBitmap{ pBitmap },
		m_PixTransform{},
		m_pLightTask { nullptr },
		m_BackgroundCalibration {},
		m_rcResult{},
		m_pAvxEntropy { nullptr }
	{}

	void process();
};

void CStackTask::process()
{
	ZFUNCTRACE_RUNTIME();
	const int height = m_pBitmap->Height();
	const int nrProcessors = CMultitask::GetNrProcessors();
	constexpr int lineBlockSize = 50;
	int progress = 0;
	std::atomic_bool runOnlyOnce{ false };

	AvxStacking avxStacking(0, 0, *m_pBitmap, *m_pTempBitmap, m_rcResult, *m_pAvxEntropy);

#pragma omp parallel for default(shared) firstprivate(avxStacking) shared(runOnlyOnce) if(nrProcessors > 1) // No "schedule" clause gives fastest result.
	for (int row = 0; row < height; row += lineBlockSize)
	{
		const int endRow = std::min(row + lineBlockSize, height);
		avxStacking.init(row, endRow);
		avxStacking.stack(m_PixTransform, *m_pLightTask, m_BackgroundCalibration, m_pOutput, m_lPixelSizeMultiplier);

		if (runOnlyOnce.exchange(true) == false) // If it was false before -> we are the first one.
			ZTRACE_RUNTIME("AvxStacking::stack %d rows in chunks of size %d", height, lineBlockSize);

		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(progress += nrProcessors * lineBlockSize);
	}
}


std::shared_ptr<CMultiBitmap> CStackingEngine::CreateMasterLightMultiBitmap(const CMemoryBitmap* pInBitmap, const bool bColor)
{
	ZFUNCTRACE_RUNTIME();
	if (dynamic_cast<const C96BitColorBitmap*>(pInBitmap) != nullptr || dynamic_cast<const C32BitGrayBitmap*>(pInBitmap) != nullptr)
		if (bColor)
			return std::make_shared<CColorMultiBitmapT<std::uint32_t, float>>();
		else
			return std::make_shared<CGrayMultiBitmapT<std::uint32_t, float>>();
	else if (dynamic_cast<const C96BitFloatColorBitmap*>(pInBitmap) != nullptr || dynamic_cast<const C32BitFloatGrayBitmap*>(pInBitmap) != nullptr)
		if (bColor)
			return std::make_shared<CColorMultiBitmapT<float, float>>();
		else
			return std::make_shared<CGrayMultiBitmapT<float, float>>();
	else
		if (bColor)
			return std::make_shared<CColorMultiBitmapT<std::uint16_t, float>>();
		else
			return std::make_shared<CGrayMultiBitmapT<std::uint16_t, float>>();
}

/* ------------------------------------------------------------------- */

template <class T>
std::pair<bool, T> CStackingEngine::StackLightFrame(std::shared_ptr<CMemoryBitmap> pInBitmap, CPixelTransform& PixTransform, double fExposure, bool bComet, T futureForWrite)
{
	ZFUNCTRACE_RUNTIME();

	bool bResult = false;
	QString strStart2;
	QString strText;
	const bool bFirst{ m_lNrStacked == 0 };
	std::shared_ptr<CMemoryBitmap> pBitmap;

	// Two cases : Bayer Drizzle or not Bayer Drizzle - that is the question
	if (static_cast<bool>(pInBitmap) && m_pLightTask != nullptr)
	{
		if (m_pProgress != nullptr)
			strStart2 = m_pProgress->GetStart2Text();

		C16BitGrayBitmap* pGrayBitmap = dynamic_cast<C16BitGrayBitmap*>(pInBitmap.get());
		if (pGrayBitmap != nullptr && pGrayBitmap->GetCFATransformation() == CFAT_AHD)
		{
			// Start by demosaicing the input bitmap
			if (m_pProgress != nullptr)
			{
				strText = QCoreApplication::translate("StackingEngine", "Interpolating with Adaptive Homogeneity Directed (AHD)", "IDS_AHDDEMOSAICING");
				m_pProgress->Start2(strText, 0);
			};
			AHDDemosaicing<std::uint16_t>(pGrayBitmap, pBitmap, m_pProgress);
		}
		else
			pBitmap = pInBitmap;

		CStackTask StackTask{ pBitmap.get(), m_pProgress };

		// Create the output bitmap
		const int lHeight = pBitmap->Height();
		const bool bColor = !pBitmap->IsMonochrome() || pBitmap->IsCFA();

		if (pGrayBitmap != nullptr && m_pLightTask->m_Method == MBP_FASTAVERAGE)
		{
			if (pGrayBitmap->GetCFATransformation() == CFAT_RAWBAYER)
			{
				// A coverage is needed with Bayer Drizzle
				m_InputCFAType = pGrayBitmap->GetCFAType();
				m_vPixelTransforms.push_back(PixTransform);
			}
		}
		else if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
		{
			// A coverage is needed with Entropy Average
			if (!static_cast<bool>(m_pEntropyCoverage))
			{
				if (bColor)
					m_pEntropyCoverage = std::make_shared<C96BitFloatColorBitmap>();
				else
					m_pEntropyCoverage = std::make_shared<C32BitFloatGrayBitmap>();

				m_pEntropyCoverage->Init(m_rcResult.width(), m_rcResult.height());
			}
		}

		// Compute entropy window info

		if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
		{
			if (m_pProgress != nullptr)
			{
				strText = QCoreApplication::translate("StackingEngine", "Computing Entropy", "IDS_COMPUTINGENTROPY");
				m_pProgress->Start2(strText, 0);
			}
			StackTask.m_EntropyWindow.Init(pBitmap, 10, m_pProgress);
		}

		// Compute histogram for median/min/max and picture backgound calibration
		// information
		if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
		{
			if (m_pProgress != nullptr)
			{
				strText = QCoreApplication::translate("StackingEngine", "Computing Background Calibration parameters", "IDS_COMPUTINGBACKGROUNDCALIBRATION");
				m_pProgress->Start2(strText, 0);
			}
			m_BackgroundCalibration.ComputeBackgroundCalibration(pBitmap.get(), bFirst, m_pProgress);
		}

		// Create a master light to enable stacking
		if (static_cast<bool>(m_pMasterLight) == false)
		{
			// Create a master light ... but not from the input bitmap
			// -> use Gray scale or Color depending on the bColor parameter
			m_pMasterLight = CreateMasterLightMultiBitmap(pInBitmap.get(), bColor);
			m_pMasterLight->SetProcessingMethod(m_pLightTask->m_Method, m_pLightTask->m_fKappa, m_pLightTask->m_lNrIterations);
			m_pMasterLight->SetNrBitmaps(m_lNrCurrentStackable);

			if (m_bCometStacking && m_bCreateCometImage)
				m_pMasterLight->SetHomogenization(true);
		}

		if (static_cast<bool>(m_pMasterLight))
		{
			StackTask.m_pTempBitmap = m_pMasterLight->CreateNewMemoryBitmap();
			if (static_cast<bool>(StackTask.m_pTempBitmap))
			{
				StackTask.m_pTempBitmap->Init(m_rcResult.width(), m_rcResult.height());
				StackTask.m_pTempBitmap->SetISOSpeed(pBitmap->GetISOSpeed());
				StackTask.m_pTempBitmap->SetGain(pBitmap->GetGain());
				StackTask.m_pTempBitmap->SetExposure(pBitmap->GetExposure());
				StackTask.m_pTempBitmap->SetNrFrames(pBitmap->GetNrFrames());
			}
		}

		// Create output bitmap only when necessary (full 32 bits float)
		if (m_pLightTask->m_Method == MBP_FASTAVERAGE || m_pLightTask->m_Method == MBP_ENTROPYAVERAGE || m_pLightTask->m_Method == MBP_MAXIMUM)
		{
			if (!static_cast<bool>(m_pOutput))
			{
				// Allocate output bitmap
				if (bColor)
					m_pOutput = std::make_shared<C96BitFloatColorBitmap>();
				else
					m_pOutput = std::make_shared<C32BitFloatGrayBitmap>();

				m_pOutput->Init(m_rcResult.width(), m_rcResult.height());
			}
		}

		if (static_cast<bool>(StackTask.m_pTempBitmap))
		{
			//int lProgress = 0;

			if (m_pProgress)
				m_pProgress->Start2(strStart2, lHeight);

			AvxEntropy avxEntropy(*pBitmap, StackTask.m_EntropyWindow, m_pEntropyCoverage.get());

			StackTask.m_PixTransform			= PixTransform;
			StackTask.m_pLightTask				= m_pLightTask;
			StackTask.m_bColor					= bColor;
			StackTask.m_BackgroundCalibration	= m_BackgroundCalibration;
			StackTask.m_rcResult				= m_rcResult;
			StackTask.m_lPixelSizeMultiplier	= m_lPixelSizeMultiplier;
			StackTask.m_pOutput					= m_pOutput;
			StackTask.m_pEntropyCoverage		= m_pEntropyCoverage;
			StackTask.m_pAvxEntropy				= &avxEntropy;

			StackTask.process();

			if (m_bCreateCometImage)
			{
				// At this point - remove the stars
				//RemoveStars(StackTask.m_pTempBitmap, PixTransform, vStars);
			}
			else if (static_cast<bool>(m_pComet) && bComet)
			{
				// Subtract the comet from the light frame
				//WriteTIFF("E:\\BeforeCometSubtraction.tiff", StackTask.m_pTempBitmap, m_pProgress, nullptr);
				//WriteTIFF("E:\\SubtractedComet.tiff", m_pComet, m_pProgress, nullptr);
				ShiftAndSubtract(StackTask.m_pTempBitmap, m_pComet, m_pProgress, -PixTransform.m_fXCometShift, -PixTransform.m_fYCometShift);
				//WriteTIFF("E:\\AfterCometSubtraction.tiff", StackTask.m_pTempBitmap, m_pProgress, nullptr);
			}

			// First try AVX accelerated code, if not supported -> run conventional code.
			AvxAccumulation avxAccumulation(m_rcResult, *m_pLightTask, *StackTask.m_pTempBitmap, *m_pOutput, avxEntropy);
			const int avxResult = avxAccumulation.accumulate(m_lNrStacked);

			if (m_pLightTask->m_Method == MBP_FASTAVERAGE)
			{
				if (avxResult != 0) // AVX code didn't run.
				{
					// Use the result to average
					for (int j = 0; j < m_rcResult.height(); j++)
					{
						for (int i = 0; i < m_rcResult.width(); i++)
						{
							if (bColor)
							{
								double			fOutRed, fOutGreen, fOutBlue;
								double			fNewRed, fNewGreen, fNewBlue;

								m_pOutput->GetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
								StackTask.m_pTempBitmap->GetPixel(i, j, fNewRed, fNewGreen, fNewBlue);
								fOutRed = (fOutRed * m_lNrStacked + fNewRed) / (double)(m_lNrStacked + 1);
								fOutGreen = (fOutGreen * m_lNrStacked + fNewGreen) / (double)(m_lNrStacked + 1);
								fOutBlue = (fOutBlue * m_lNrStacked + fNewBlue) / (double)(m_lNrStacked + 1);
								m_pOutput->SetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
							}
							else
							{
								double			fOutGray;
								double			fNewGray;

								m_pOutput->GetPixel(i, j, fOutGray);
								StackTask.m_pTempBitmap->GetPixel(i, j, fNewGray);
								fOutGray = (fOutGray * m_lNrStacked + fNewGray) / (double)(m_lNrStacked + 1);
								m_pOutput->SetPixel(i, j, fOutGray);
							};
						};
					};
				};
			}
			else if (m_pLightTask->m_Method == MBP_MAXIMUM)
			{
				if (avxResult != 0)
				{
					// Use the result to maximize
					for (int j = 0; j < m_rcResult.height(); j++)
					{
						for (int i = 0; i < m_rcResult.width(); i++)
						{
							if (bColor)
							{
								double			fOutRed, fOutGreen, fOutBlue;
								double			fNewRed, fNewGreen, fNewBlue;

								m_pOutput->GetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
								StackTask.m_pTempBitmap->GetPixel(i, j, fNewRed, fNewGreen, fNewBlue);
								fOutRed = max(fOutRed, fNewRed);
								fOutGreen = max(fOutGreen, fNewGreen);
								fOutBlue = max(fOutBlue, fNewBlue);;
								m_pOutput->SetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
							}
							else
							{
								double			fOutGray;
								double			fNewGray;

								m_pOutput->GetPixel(i, j, fOutGray);
								StackTask.m_pTempBitmap->GetPixel(i, j, fNewGray);
								fOutGray = max(fOutGray, fNewGray);
								m_pOutput->SetPixel(i, j, fOutGray);
							};
						};
					};
				};
			}
			else if ((m_pLightTask->m_Method != MBP_ENTROPYAVERAGE) && static_cast<bool>(m_pMasterLight) && static_cast<bool>(StackTask.m_pTempBitmap))
			{
				if (futureForWrite.valid())
					futureForWrite.get();
				const auto writeTask = [masterLight = this->m_pMasterLight](std::shared_ptr<CMemoryBitmap> tempBitmap) -> bool {
					return masterLight->AddBitmap(tempBitmap.get(), nullptr);
				};
//				m_pMasterLight->AddBitmap(StackTask.m_pTempBitmap.get(), m_pProgress);
				futureForWrite = std::async(std::launch::async, writeTask, StackTask.m_pTempBitmap);
			}

			if (m_bSaveIntermediate && !m_bCreateCometImage)
			{
				// Save the pTempBitmap to a TIFF File
				StackTask.m_pTempBitmap->m_ExtraInfo = pInBitmap->m_ExtraInfo;
				StackTask.m_pTempBitmap->m_DateTime  = pInBitmap->m_DateTime;
				SaveCalibratedAndRegisteredLightFrame(StackTask.m_pTempBitmap.get());
			}

			m_fTotalExposure += fExposure;
			bResult = true;
		}
		else
		{
			// Error - not enough memory
			bResult = false;
		}

		if (m_pProgress != nullptr)
			m_pProgress->End2();
	}

	return std::make_pair(bResult, std::move(futureForWrite));
}

bool CStackingEngine::StackAll(CAllStackingTasks& tasks, std::shared_ptr<CMemoryBitmap>& rpBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	bool bContinue = true;

	m_lNrStacked = 0;
	GetResultISOSpeed();
	GetResultGain();
	GetResultDateTime();
	GetResultExtraInfo();

	m_vCometShifts.clear();
	try
	{
		STACKINGMODE stackingMode = tasks.getStackingMode();

		switch (stackingMode)
		{
		case SM_MOSAIC:
		{
			fs::path strDrive;
			QString strFreeSpace;
			QString strNeededSpace;

			m_rcResult = computeLargestRectangle();
			std::int64_t ulNeededSpace;
			std::int64_t ulFreeSpace;
			DSSRect rcResult;

			rcResult.setCoords(m_rcResult.left / m_lPixelSizeMultiplier, m_rcResult.top / m_lPixelSizeMultiplier,
				m_rcResult.right / m_lPixelSizeMultiplier, m_rcResult.bottom / m_lPixelSizeMultiplier);

			ulNeededSpace = tasks.computeNecessaryDiskSpace(rcResult);
			ulFreeSpace = tasks.AvailableDiskSpace(strDrive);

			if (m_pProgress != nullptr && (ulNeededSpace > ulFreeSpace))
			{
				SpaceToQString(ulFreeSpace, strFreeSpace);
				SpaceToQString(ulNeededSpace, strNeededSpace);

				const QString strText(QCoreApplication::translate("StackingEngine", "The process needs temporarily %1 of free space on the %2 drive.\nOnly %3 are available on this drive.", "IDS_RECAP_WARNINGDISKSPACE").arg(strNeededSpace).arg(QString::fromWCharArray(strDrive.wstring().c_str())).arg(strFreeSpace) +
									  QCoreApplication::translate("StackingEngine", "\nDo you really want to continue?", "IDS_WANTTOCONTINUE"));
				bContinue = m_pProgress->Warning(strText);
			}
		} break;

		case SM_INTERSECTION:
		{
			if (!computeSmallestRectangle(m_rcResult))
			{
				// Fall back to normal rectangle
				int            lBitmapIndice = 0;
				if (m_vBitmaps[0].m_bDisabled)
					lBitmapIndice = 1;
				
				m_rcResult.setCoords(0, 0,
					m_vBitmaps[lBitmapIndice].RenderedWidth() * m_lPixelSizeMultiplier,
					m_vBitmaps[lBitmapIndice].RenderedHeight() * m_lPixelSizeMultiplier);
				//m_rcResult.left = m_rcResult.top = 0;
				//m_rcResult.right = m_vBitmaps[lBitmapIndice].RenderedWidth() * m_lPixelSizeMultiplier;
				//m_rcResult.bottom = m_vBitmaps[lBitmapIndice].RenderedHeight() * m_lPixelSizeMultiplier;
			};
		} break;

		case SM_CUSTOM:
		{
			tasks.getCustomRectangle(m_rcResult);
			m_rcResult.left *= m_lPixelSizeMultiplier;
			m_rcResult.right *= m_lPixelSizeMultiplier;
			m_rcResult.top *= m_lPixelSizeMultiplier;
			m_rcResult.bottom *= m_lPixelSizeMultiplier;
		} break;

		case SM_NORMAL:
		{
			const size_t lBitmapIndex = m_vBitmaps.cbegin()->m_bDisabled ? 1 : 0;

			m_rcResult.setCoords(0, 0,
				m_vBitmaps[lBitmapIndex].RenderedWidth() * m_lPixelSizeMultiplier,
				m_vBitmaps[lBitmapIndex].RenderedHeight() * m_lPixelSizeMultiplier);
			//m_rcResult.left = m_rcResult.top = 0;
			//m_rcResult.right = m_vBitmaps[lBitmapIndex].RenderedWidth() * m_lPixelSizeMultiplier;
			//m_rcResult.bottom = m_vBitmaps[lBitmapIndex].RenderedHeight() * m_lPixelSizeMultiplier;
		} break;

		}; // switch

		ZTRACE_RUNTIME("Computed image rectangle m_rcResult left %ld, top %ld, right %ld, bottom %ld",
			m_rcResult.left, m_rcResult.top, m_rcResult.right, m_rcResult.bottom);

		if (bContinue)
		{
			// Iterate all light tasks until everything is done
			bool bEnd = false;
			bool bStop = false;
			QString strText;

			int lFirstTaskID = m_vBitmaps.empty() ? 0 : tasks.FindStackID(m_vBitmaps.cbegin()->filePath.c_str());

			while (!bEnd)
			{
				bEnd = true;
				const CStackingInfo* pStackingInfo = nullptr;

				for (size_t i = 0; i < tasks.m_vStacks.size() && bEnd; i++)
				{
					if (tasks.m_vStacks[i].m_pLightTask)
					{
						if (lFirstTaskID)
						{
							if (tasks.m_vStacks[i].m_pLightTask->m_dwTaskID == static_cast<uint>(lFirstTaskID))
							{
								bEnd = false;
								lFirstTaskID = 0;
								pStackingInfo = tasks.m_vStacks.data() + i;
							}
						}
						else if (!tasks.m_vStacks[i].m_pLightTask->m_bDone)
						{
							bEnd = false;
							pStackingInfo = tasks.m_vStacks.data() + i;
						}
					}
				}

				if (pStackingInfo != nullptr && pStackingInfo->m_pLightTask != nullptr && !pStackingInfo->m_pLightTask->m_vBitmaps.empty())
				{
					// Do stack these
					CMasterFrames MasterFrames;
					MasterFrames.LoadMasters(*pStackingInfo, m_pProgress);

					m_pLightTask = pStackingInfo->m_pLightTask;

					if ((m_pLightTask->m_Method == MBP_AVERAGE) && !m_bCreateCometImage && !static_cast<bool>(m_pComet)) {
						m_pLightTask->m_Method = MBP_FASTAVERAGE;
					}

					const auto readTask = [this, pStackingInfo, firstBitmap = m_vBitmaps.cbegin()](const size_t lightTaskNdx, ProgressBase* pProgress) -> std::pair<std::shared_ptr<CMemoryBitmap>, int>
					{
						if (lightTaskNdx >= pStackingInfo->m_pLightTask->m_vBitmaps.size())
							return { {}, -1 };
						const int bitmapNdx = findBitmapIndex(pStackingInfo->m_pLightTask->m_vBitmaps[lightTaskNdx].filePath);
						if (bitmapNdx < 0)
							return { {}, -1 };
						const auto& lightframeInfo = m_vBitmaps[bitmapNdx];
						if (lightframeInfo.m_bDisabled)
							return { {}, -1 };

						ZTRACE_RUNTIME("Stack %s", lightframeInfo.filePath.generic_u8string().c_str());

						std::shared_ptr<CMemoryBitmap> rpBitmap;
						if (::LoadFrame(lightframeInfo.filePath, PICTURETYPE_LIGHTFRAME, pProgress, rpBitmap))
							return { rpBitmap, bitmapNdx };
						else
							return { {}, -1 };
					};

					auto futureForRead = std::async(std::launch::deferred, readTask, 0, m_pProgress); // Load first lightframe synchronously.
					const auto firstBitmap = m_vBitmaps.cbegin();

					using T = std::future<bool>;
					T futureForWriteTempFile{};

					for (size_t i = 0; i < pStackingInfo->m_pLightTask->m_vBitmaps.size() && !bStop; ++i)
					{
						auto [pBitmap, bitmapNdx] = futureForRead.get();
						futureForRead = std::async(std::launch::async, readTask, i + 1, nullptr); // Immediately load next lightframe asynchronously (need to set progress pointer to null).

						if (bitmapNdx < 0)
							continue;

						const auto& lightframeInfo = m_vBitmaps[bitmapNdx];

						if (pBitmap->IsMonochrome())
						{
							auto deb{ qDebug() };
							if (pBitmap->IsCFA())
								deb.nospace() << "CFA light frame: ";
							else
								deb.nospace() << "Mono light frame: ";
							deb << lightframeInfo.filePath.generic_u8string().c_str() << Qt::endl;
							for (size_t ix = 0; ix < 12; ix++)
								deb << " " << pBitmap->getValue(ix, 0);
						}
						else
						{
							auto deb{ qDebug() };
							deb.nospace() << "RGB light frame: " << lightframeInfo.filePath.generic_u8string().c_str() << Qt::endl;
							for (size_t ix = 0; ix < 4; ix++)
							{
								auto [r, g, b] = pBitmap->getValues(ix, 0);
								deb << r << " " << g << " " << b << Qt::endl;
							}
						}

						CPixelTransform PixTransform{ lightframeInfo.m_BilinearParameters };

						bool doStack = true;
						if (m_bCometStacking || m_bCreateCometImage)
						{
							if (firstBitmap->m_bComet && lightframeInfo.m_bComet)
								PixTransform.ComputeCometShift(firstBitmap->m_fXComet, firstBitmap->m_fYComet,
									lightframeInfo.m_fXComet, lightframeInfo.m_fYComet, false, lightframeInfo.m_bTransformedCometPosition);
							else
								doStack &= (!m_bCreateCometImage);
						}
						else if (static_cast<bool>(m_pComet))
						{
							if (firstBitmap->m_bComet && lightframeInfo.m_bComet)
								PixTransform.ComputeCometShift(firstBitmap->m_fXComet, firstBitmap->m_fYComet,
									lightframeInfo.m_fXComet, lightframeInfo.m_fYComet, true, lightframeInfo.m_bTransformedCometPosition);
						}
						if (!doStack)
							continue;

						PixTransform.SetShift(-m_rcResult.left, -m_rcResult.top);
						PixTransform.SetPixelSizeMultiplier(m_lPixelSizeMultiplier);

						if (m_pProgress != nullptr)
						{
							m_pProgress->Progress1(QCoreApplication::translate(
								"StackingEngine", "Stacking %1 of %2 - Offset [%3,%4] - Angle : %5\xc2\xb0 ", "IDS_STACKING_PICTURE")
								.arg(m_lNrStacked + 1).arg(m_lNrCurrentStackable).arg(lightframeInfo.m_fXOffset, 0, 'f', 1).arg(lightframeInfo.m_fYOffset, 0, 'f', 1)
								.arg(lightframeInfo.m_fAngle * 180 / M_PI, 0, 'f', 1), m_lNrStacked + 1);
						}

						if (lightframeInfo.m_lNrChannels == 3)
							strText = QCoreApplication::translate("StackingEngine", "Stacking %1 bit/ch %2 light frame\n%3", "IDS_STACKRGBLIGHT").arg(lightframeInfo.m_lBitsPerChannel).arg(lightframeInfo.m_strInfos).arg(QString::fromStdU16String(lightframeInfo.filePath.generic_u16string()));
						else
							strText = QCoreApplication::translate("StackingEngine", "Stacking %1 bits gray %2 light frame\n%3", "IDS_STACKGRAYLIGHT").arg(lightframeInfo.m_lBitsPerChannel).arg(lightframeInfo.m_strInfos).arg(QString::fromStdU16String(lightframeInfo.filePath.generic_u16string()));

						ZTRACE_RUNTIME(strText);
						// First apply transformations
						MasterFrames.ApplyAllMasters(pBitmap, std::addressof(lightframeInfo.m_vStars), m_pProgress);

						// Here save the calibrated light frame if needed
						currentLightFrame = lightframeInfo.filePath;

						std::shared_ptr<CMemoryBitmap> pDelta = ApplyCosmetic(pBitmap, m_PostCalibrationSettings, m_pProgress);
						if (m_bSaveCalibrated)
							SaveCalibratedLightFrame(pBitmap);
						if (static_cast<bool>(pDelta))
							SaveDeltaImage(pDelta.get());

						qDebug() << "Calibrated light:";
						if (pBitmap->IsMonochrome())
						{
							auto deb{ qDebug() };
							deb.nospace();
							for (size_t ix = 0; ix < 12; ix++)
								deb << " " << pBitmap->getValue(ix, 0);
						}
						else
						{
							for (size_t ix = 0; ix < 4; ix++)
							{
								auto [r, g, b] = pBitmap->getValues(ix, 0);
								qDebug().nospace() << r << " " << g << " " << b;
							}
						}

						if (m_pProgress != nullptr)
							m_pProgress->Start2(strText, 0);

						// Stack
						auto [stackSuccess, f] = StackLightFrame<T>(pBitmap, PixTransform, lightframeInfo.m_fExposure, lightframeInfo.m_bComet, std::move(futureForWriteTempFile));
						futureForWriteTempFile = std::move(f);
						bStop = !stackSuccess;
						m_lNrStacked++;

						if (m_bCreateCometImage)
							m_vCometShifts.emplace_back(static_cast<int>(m_vCometShifts.size()), PixTransform.m_fXCometShift, PixTransform.m_fYCometShift);

						if (m_pProgress != nullptr)
						{
							m_pProgress->End2();
							bStop = bStop || m_pProgress->IsCanceled();
						}
					}
					if (futureForWriteTempFile.valid())
						futureForWriteTempFile.get(); // Wait for last temp file to be written.
					pStackingInfo->m_pLightTask->m_bDone = true;
					bEnd = bStop;
				}
			}

			bResult = !bStop;

			// Clear the cache
			ClearTaskCache();

			if (bResult)
			{
				if (static_cast<bool>(m_pMasterLight) && m_pMasterLight->GetNrAddedBitmaps() != 0)
					ComputeBitmap();
				AdjustEntropyCoverage();
				AdjustBayerDrizzleCoverage();

				if (static_cast<bool>(m_pOutput))
				{
					m_pOutput->SetExposure(m_fTotalExposure);
					m_pOutput->SetISOSpeed(m_lISOSpeed);
					m_pOutput->SetGain(m_lGain);
					m_pOutput->SetNrFrames(m_lNrStacked);
					m_pOutput->m_DateTime = m_DateTime;
					m_pOutput->m_ExtraInfo = m_ExtraInfo;

					rpBitmap = m_pOutput;
				}
				else
					bResult = false;
			}
			m_pLightTask = nullptr;
		}
	}
	catch (std::exception & e)
	{
		const QString errorMessage(e.what());
		DSSBase::instance()->reportError(errorMessage, "");
	}
	catch (ZException& e)
	{
		QString errorMessage;
		if (e.locationAtIndex(0))
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from %2 Function : %3() Line : %4\n\n %5")
				.arg(e.name())
				.arg(e.locationAtIndex(0)->fileName())
				.arg(e.locationAtIndex(0)->functionName())
				.arg(e.text(0));
		}
		else
		{
			errorMessage = QCoreApplication::translate("Kernel",
				"Exception %1 thrown from an unknown Function.\n\n%2")
				.arg(e.name())
				.arg(e.text(0));
		}
		DSSBase::instance()->reportError(errorMessage, "", DSSBase::Severity::Critical);
	}
	catch (...)
	{
		const QString errorMessage(QCoreApplication::translate("Kernel", "Unknown exception caught"));
		DSSBase::instance()->reportError(errorMessage, "");
	}

	// Clear everything
	m_pOutput.reset();
	m_pEntropyCoverage.reset();

	if (!bResult)
		rpBitmap.reset();

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CStackingEngine::StackLightFrames(CAllStackingTasks& tasks, ProgressBase* const pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap)
{
	ZFUNCTRACE_RUNTIME();
	bool bResult = false;
	bool bContinue = true;

	m_InputCFAType = CFATYPE_NONE;
	m_pProgress = pProgress;

	// 1. compute light frames offsets
	// Only for registered light frame
	AddLightFramesToList(tasks);
	ComputeOffsets();

	// 2. disable non stackable light frames
	RemoveNonStackableLightFrames(tasks);

	if (pProgress != nullptr && (m_lNrStackable == 1) && (tasks.GetNrLightFrames() > 1))
	{
		const QString strText(QCoreApplication::translate("StackingEngine", "Only one frame (out of %1) will be stacked.\n\nYou should check/change the star detection threshold to detect more stars\nand help DeepSkyStacker find a transformation between the reference frame and the others.\n", "IDS_WARNING_ONLYONEFRAME").arg(tasks.GetNrLightFrames()) +
							  QCoreApplication::translate("StackingEngine", "\nDo you really want to continue?", "IDS_WANTTOCONTINUE"));
		bContinue = m_pProgress->Warning(strText);
	}

	if (bContinue)
	{
		m_lNrCurrentStackable = m_lNrStackable;
		if (tasks.IsCometAvailable() && tasks.GetCometStackingMode() == CSM_COMETSTAR)
			 m_lNrCurrentStackable = m_lNrCometStackable;

		const QString strText(QCoreApplication::translate("StackingEngine", "Stacking", "IDS_STACKING"));
		if (pProgress != nullptr)
			pProgress->Start1(strText, m_lNrCurrentStackable, true);

		// 3. do all pre-tasks (the one not already done by the registering process)
		bResult = m_lNrStackable != 0 && tasks.DoAllPreTasks(pProgress);

		// Again - in case pretasks change the progress settings
		if (pProgress != nullptr)
			pProgress->Start1(strText, m_lNrCurrentStackable+1, true);	// SCS: Add one so we don't sit at 100% whilst processing the last one.

		// 4. Stack everything
		if (bResult)
		{
			if (tasks.IsCometAvailable() && (tasks.GetCometStackingMode() == CSM_COMETONLY || tasks.GetCometStackingMode() == CSM_COMETSTAR))
			{
				ZTRACE_RUNTIME("Doing Comet +/- Star stacking");
				m_bCometStacking = true;
				m_bCreateCometImage = (tasks.GetCometStackingMode()==CSM_COMETSTAR);
			}

			ZTRACE_RUNTIME("CreateCometImage: %s", m_bCreateCometImage ? "true" : "false");
			std::shared_ptr<CMemoryBitmap> pBitmap;
			bResult = StackAll(tasks, pBitmap);
			ZTRACE_RUNTIME("StackAll returned %s", bResult ? "true" : "false");


			if (bResult && tasks.IsCometAvailable() && tasks.GetCometStackingMode() == CSM_COMETSTAR)
			{
				ZTRACE_RUNTIME("Doing Comet and Star");
				if (m_bApplyFilterToCometImage)
				{
					auto p = GetFilteredImage(pBitmap.get(), 1, pProgress);
					pBitmap = p;

					CDirectionalImageFilter Filter;
					Filter.SetAngle(m_fStarTrailsAngle + M_PI / 2.0, 2);
					ZTRACE_RUNTIME("Applying Comet Angle filter");
					m_pComet = Filter.ApplyFilter(pBitmap.get(), pProgress);
				}
				else
					m_pComet = pBitmap;

				if (m_bSaveIntermediateCometImages)
					SaveCometImage(m_pComet.get());

				m_bCometStacking = false;
				m_bCreateCometImage = false;
				m_lNrCurrentStackable = m_lNrStackable;

				if (pProgress != nullptr)
					pProgress->Start1(strText, m_lNrCurrentStackable, true);

				// Stack again but remove the comet before stacking
				tasks.ResetTasksStatus();
				bResult = StackAll(tasks, pBitmap);
				ZTRACE_RUNTIME("StackAll returned %s", bResult ? "true" : "false");

				if (m_bSaveIntermediateCometImages)
					SaveCometlessImage(pBitmap.get());

				// Then add the comet to the resulting image (simple addition combination)
				Add(pBitmap, m_pComet);
			}

			if (bResult && m_bChannelAlign)
			{
				CChannelAlign::AlignChannels(pBitmap, m_pProgress);
			}

			if (bResult)
				rpBitmap = pBitmap;
		}

		m_pProgress = nullptr;
		m_pEntropyCoverage.reset();
		m_pComet.reset();
	}
	return bResult;
}

/* ------------------------------------------------------------------- */

void CStackingEngine::ComputeOffsets(CAllStackingTasks& tasks, ProgressBase* pProgress)
{
	ZFUNCTRACE_RUNTIME();

	m_pProgress = pProgress;

	AddLightFramesToList(tasks);
	ComputeOffsets();

	m_pProgress = nullptr;
}

/* ------------------------------------------------------------------- */

bool	CStackingEngine::GetDefaultOutputFileName(fs::path& file, const fs::path& fileList, bool bTIFF)
{
	ZFUNCTRACE_RUNTIME();

	constexpr const char AutoSave[] = "Autosave";

	// Retrieve the first light frame
	COutputSettings OutputSettings;
	CAllStackingTasks::GetOutputSettings(OutputSettings);
	bool bResult = OutputSettings.m_bOutput;
	fs::path folder;
	if (m_vBitmaps.size())
	{
		// Use the folder of the first light frame
		folder = m_vBitmaps[0].filePath;
	}

	if (OutputSettings.m_bOtherFolder && OutputSettings.m_strFolder.length())
	{
		folder = OutputSettings.m_strFolder.toStdU16String();
	}

	if (OutputSettings.m_bFileListFolder && !fileList.empty())
	{
		folder = fileList;
	}

	folder.remove_filename();

	fs::path name{ file.stem() }; // Filename of the output file WITHOUT EXTENSION.
	bool addHyphen = false;

	if (name.empty())
	{
		if (OutputSettings.m_bAutosave || fileList.empty())
			name = AutoSave;
		else
		{
			name = fileList.stem();
			if (name.empty())
				name = AutoSave;
			else
				addHyphen = true;
		}
	}

	fs::path extension;
	if (bTIFF)
	{
		extension = ".tif";
	}
	else
	{
		extension = ".fit";
	}
	fs::path outputFile{ folder };

	if (OutputSettings.m_bAppend)
	{
		int i = 0;
		bool fileExists = false;
		QString suffix;
		do
		{
			fs::path newName{ name }; // newName does not yet have an extension.
			if (i > 0)
			{
				suffix = QString(addHyphen ? "-%1" : "%1").arg(i, 3, 10, QLatin1Char('0'));
				newName += suffix.toStdU16String();
			}
			outputFile.replace_filename(newName += extension);

			fileExists = exists(outputFile);
			if (!fileExists)
				break;
			++i;
		}
		while (fileExists && (i<1000));
	}
	else
	{
		outputFile.replace_filename(name.replace_extension(extension));
	}
	file = outputFile;
	return bResult;
};

/* ------------------------------------------------------------------- */
static void GetISOGainStrings(CTaskInfo *pTask, const QString& strISO, const QString& strGain, QString& strISOGainText, QString& strISOGainValue)
{
	if (pTask->HasISOSpeed())
	{
		ISOToString(pTask->m_lISOSpeed, strISOGainValue);
		strISOGainText = strISO;
	}
	else
	{
		GainToString(pTask->m_lGain, strISOGainValue);
		strISOGainText = strGain;
	}
}

void	CStackingEngine::WriteDescription(CAllStackingTasks& tasks, const fs::path& outputFile)
{
	ZFUNCTRACE_RUNTIME();

	COutputSettings	OutputSettings;
	tasks.GetOutputSettings(OutputSettings);
	if (!OutputSettings.m_bOutputHTML)
		return;

	const QFileInfo fileInfo(outputFile);
	const QString strOutputFile(QDir::toNativeSeparators(QString("%1%2%3.html").arg(fileInfo.path()).arg(QDir::separator()).arg(fileInfo.baseName())));

	QFile file(strOutputFile);
	if (!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
		return;
	QByteArray buffer;
	QTextStream stream(&buffer);

	QString strTempText;

	stream << "<html>" << Qt::endl;
	stream << "<head>" << Qt::endl;
	stream << "<meta name=\"GENERATOR\" content=\"DeepSkyStacker\">";
	stream << "<title>DeepSkyStacker - " << fileInfo.baseName() << "</title>";
	stream << "</head>" << Qt::endl;
	stream << "<body>" << Qt::endl;
	stream << "-> " << fileInfo.baseName() << "<br><br>" << Qt::endl;

	// Stacking Mode
	stream << QCoreApplication::translate("StackRecap", "Stacking mode: ", "IDS_RECAP_STACKINGMODE");
	switch (tasks.getStackingMode())
	{
	case SM_NORMAL :
		stream << QCoreApplication::translate("StackRecap", "Standard", "IDS_RECAP_STACKINGMODE_NORMAL");
		break;
	case SM_MOSAIC :
		stream << QCoreApplication::translate("StackRecap", "Mosaic", "IDS_RECAP_STACKINGMODE_MOSAIC");
		break;
	case SM_INTERSECTION :
		stream << QCoreApplication::translate("StackRecap", "Intersection", "IDS_RECAP_STACKINGMODE_INTERSECTION");
		break;
	case SM_CUSTOM :
		stream << QCoreApplication::translate("StackRecap", "Custom Rectangle", "IDS_RECAP_STACKINGMODE_CUSTOM");
		break;
	};

	stream << "<br>";

	// Alignment method
	stream << QCoreApplication::translate("StackRecap", "Alignment method: ", "IDS_RECAP_ALIGNMENT");

	switch (tasks.GetAlignmentMethod())
	{
	case 0 :
	case 1 :
		stream << QCoreApplication::translate("StackRecap", "Automatic", "IDS_ALIGN_AUTO");
		break;
	case 2 :
		stream << QCoreApplication::translate("StackRecap", "Bilinear", "IDS_ALIGN_BILINEAR");
		break;
	case 3 :
		stream << QCoreApplication::translate("StackRecap", "Bisquared", "IDS_ALIGN_BISQUARED");
		break;
	case 4 :
		stream << QCoreApplication::translate("StackRecap", "Bicubic", "IDS_ALIGN_BICUBIC");
		break;
	case 5 :
		stream << QCoreApplication::translate("StackRecap", "No Alignment", "IDS_ALIGN_NONE");
		break;
	};
	stream << "<br>" << Qt::endl;

	// Drizzle ?
	const int dwDrizzle = tasks.GetPixelSizeMultiplier();
	if (dwDrizzle > 1)
	{
		stream << QCoreApplication::translate("StackRecap", "Drizzle x%1 enabled", "IDS_RECAP_DRIZZLE").arg(dwDrizzle);
		stream << "<br>" << Qt::endl;
	};

	// Comet
	if (tasks.IsCometAvailable())
	{
		COMETSTACKINGMODE	CometStackingMode;

		CometStackingMode = tasks.GetCometStackingMode();
		stream << QCoreApplication::translate("StackRecap", "Comet processing: ", "IDS_RECAP_COMETSTACKING");
		switch (CometStackingMode)
		{
		case CSM_STANDARD :
			stream << QCoreApplication::translate("StackRecap", "Align on stars (no specific processing)", "IDS_RECAP_COMETSTACKING_NONE");
			break;
		case CSM_COMETONLY :
			stream << QCoreApplication::translate("StackRecap", "Align on comet", "IDS_RECAP_COMETSTACKING_COMET");
			break;
		case CSM_COMETSTAR :
			stream << QCoreApplication::translate("StackRecap", "Align on stars and comet", "IDS_RECAP_COMETSTACKING_BOTH");
			break;
		};
		stream << "<br>" << Qt::endl;
	};

	// Post calibration settings
	CPostCalibrationSettings		pcs;

	tasks.GetPostCalibrationSettings(pcs);
	if (pcs.m_bHot)
	{
		stream << QCoreApplication::translate("StackRecap", "Cosmetic applied to hot pixels (Filter = %1 px, Detection Threshold = %L2%)<br>", "IDS_RECAP_COSMETICHOT").arg(pcs.m_lHotFilter).arg(pcs.m_fHotDetection);
		stream << "<br>" << Qt::endl;
	};
	if (pcs.m_bCold)
	{
		stream << QCoreApplication::translate("StackRecap", "Cosmetic applied to cold pixels (Filter = %1 px, Detection Threshold = %L2%)<br>", "IDS_RECAP_COSMETICCOLD").arg(pcs.m_lColdFilter).arg(pcs.m_fColdDetection);
		stream << "<br>" << Qt::endl;
	};

	if (pcs.m_bHot || pcs.m_bCold)
		stream << "<br><br>";

	// Now the list of tasks
	int				i, j;
	int				lTotalExposure = 0;
	QString				strBackgroundCalibration;
	QString				strPerChannelBackgroundCalibration;
	QString				strExposure;
	QString				strISOGainValue;
	QString				strISOGainText;
	QString				strISOText;
	QString				strGainText;
	QString				strYesNo;
	QString strYes(QCoreApplication::translate("StackingEngine", "Yes", "IDS_YES"));
	QString strNo(QCoreApplication::translate("StackingEngine", "No", "IDS_NO"));
	BACKGROUNDCALIBRATIONMODE	CalibrationMode;

	CalibrationMode = tasks.GetBackgroundCalibrationMode();

	strISOText = QCoreApplication::translate("StackRecap", "ISO", "IDS_ISO");
	strGainText = QCoreApplication::translate("StackRecap", "Gain", "IDS_GAIN");
	strBackgroundCalibration = QCoreApplication::translate("StackRecap", "RGB Channels Background Calibration: %1", "IDS_RECAP_BACKGROUNDCALIBRATION").arg((CalibrationMode == BCM_RGB) ? strYes : strNo);
	strPerChannelBackgroundCalibration = QCoreApplication::translate("StackRecap", "Per Channel Background Calibration: %1", "IDS_RECAP_PERCHANNELBACKGROUNDCALIBRATION").arg((CalibrationMode == BCM_PERCHANNEL) ? strYes : strNo);

	for (i = 0;i<tasks.m_vStacks.size();i++)
	{
		CStackingInfo& si = tasks.m_vStacks[i];

		if (si.m_pLightTask)
		{
			stream << "<table border='1px' cellspacing=0 cellpadding=5 width=100%%><tr><td>";
			int			lTaskExposure = 0;

			for (j = 0;j<si.m_pLightTask->m_vBitmaps.size();j++)
				lTaskExposure += si.m_pLightTask->m_vBitmaps[j].m_fExposure;

			lTotalExposure += lTaskExposure;

			strExposure = exposureToString(lTaskExposure);
			GetISOGainStrings(si.m_pLightTask, strISOText, strGainText, strISOGainText, strISOGainValue);

			QString strText(QCoreApplication::translate("StackRecap", "Stacking step %1<br>  ->%2 frames (%3: %4) - total exposure: ",
				"IDS_RECAP_STEP")
							.arg(i + 1)
							.arg(si.m_pLightTask->m_vBitmaps.size())
							.arg(strISOGainText)
							.arg(strISOGainValue));

			stream << "<a href=\"#Task" << i << "\">" << strText << "</a>";
			stream << strExposure << "<br>";
			stream << "<ul>" << strBackgroundCalibration << "<br>" << strPerChannelBackgroundCalibration << "</ul>";

			if (si.m_pLightTask->m_vBitmaps.size()>1)
			{
				FormatFromMethod(strTempText, si.m_pLightTask->m_Method, si.m_pLightTask->m_fKappa, si.m_pLightTask->m_lNrIterations);						
				stream << "<ul>" << QCoreApplication::translate("StackRecap", "Method: ", "IDS_RECAP_METHOD") << strTempText << "</ul>";					

				if ((si.m_pLightTask->m_Method != MBP_AVERAGE) &&
					(IsRawBayer() || IsFITSRawBayer()))
				{
					stream << "<br>" << QCoreApplication::translate("StackRecap", "Warning: the Bayer Drizzle option selected in the RAW DDP settings may lead to strange results with a method other than average.", "IDS_RECAP_WARNINGBAYERDRIZZLE");
				};
			};

			stream << "<hr>";
			if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
				stream << "<ul>";

			if (si.m_pOffsetTask)
			{
				strExposure = exposureToString(si.m_pOffsetTask->m_fExposure);
				GetISOGainStrings(si.m_pOffsetTask, strISOText, strGainText, strISOGainText, strISOGainValue);

				stream << QCoreApplication::translate("StackRecap", "-> Offset: %1 frames (%2: %3) exposure: %4", "IDS_RECAP_OFFSET")
							.arg(si.m_pOffsetTask->m_vBitmaps.size())
							.arg(strISOGainText)
							.arg(strISOGainValue)
							.arg(strExposure);

				if (si.m_pOffsetTask->m_vBitmaps.size()>1)
				{
					FormatFromMethod(strTempText, si.m_pOffsetTask->m_Method, si.m_pOffsetTask->m_fKappa, si.m_pOffsetTask->m_lNrIterations);							
					stream << "<ul>" << QCoreApplication::translate("StackRecap", "Method: ", "IDS_RECAP_METHOD") << strTempText << "</ul>";
				}
				else
					stream << "<br>";

				if (si.m_pOffsetTask->HasISOSpeed())
				{
					if (si.m_pOffsetTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						stream << QCoreApplication::translate("StackRecap", "Warning: ISO speed does not match that of the light frames", "IDS_RECAP_ISOWARNING");
				}
				else
				{
					if (si.m_pOffsetTask->m_lGain != si.m_pLightTask->m_lGain)
						stream << QCoreApplication::translate("StackRecap", "Warning: Gain does not match that of the light frames", "IDS_RECAP_GAINWARNING");
				};
				stream << "</ul>";
			}
			else
			{
				stream << QCoreApplication::translate("StackRecap", "-> No Offset", "IDS_RECAP_NOOFFSET");
			}

			if (si.m_pDarkTask)
			{
				strExposure = exposureToString(si.m_pDarkTask->m_fExposure);
				GetISOGainStrings(si.m_pDarkTask, strISOText, strGainText, strISOGainText, strISOGainValue);

				stream << QCoreApplication::translate("StackRecap", "-> Dark: %1 frames (%2 : %3) exposure: %4", "IDS_RECAP_DARK")
							.arg(si.m_pDarkTask->m_vBitmaps.size())
							.arg(strISOGainText)
							.arg(strISOGainValue)
							.arg(strExposure);

				if (si.m_pDarkTask->m_vBitmaps.size()>1)
				{
					stream << "<ul>" << QCoreApplication::translate("StackRecap", "Method: ", "IDS_RECAP_METHOD");
					FormatFromMethod(strTempText, si.m_pDarkTask->m_Method, si.m_pDarkTask->m_fKappa, si.m_pDarkTask->m_lNrIterations);
					stream << strTempText << "</ul>";
				}

				stream << "<ul>";
				if (si.m_pDarkTask->HasISOSpeed())
				{
					if (si.m_pDarkTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						stream << QCoreApplication::translate("StackRecap", "Warning: ISO speed does not match that of the light frames", "IDS_RECAP_ISOWARNING") << "<br>";
				}
				else
				{
					if (si.m_pDarkTask->m_lGain != si.m_pLightTask->m_lGain)
						stream << QCoreApplication::translate("StackRecap", "Warning: Gain does not match that of the light frames", "IDS_RECAP_GAINWARNING") << "<br>";
				}
				if (!AreExposureEquals(si.m_pDarkTask->m_fExposure, si.m_pLightTask->m_fExposure))
					stream << QCoreApplication::translate("StackRecap", "Warning: Exposure does not match that of the Light frames", "IDS_RECAP_EXPOSUREWARNING") << "<br>";
				stream << "</ul>";
			}
			else
			{
				stream << QCoreApplication::translate("StackRecap", "-> No Dark", "IDS_RECAP_NODARK");
			}

			if (si.m_pDarkFlatTask && si.m_pFlatTask)
			{
				strExposure = exposureToString(si.m_pDarkFlatTask->m_fExposure);
				GetISOGainStrings(si.m_pDarkFlatTask, strISOText, strGainText, strISOGainText, strISOGainValue);

				stream << QCoreApplication::translate("StackRecap", "-> Dark Flat: %1 frames (%2 : %3) exposure: %4", "IDS_RECAP_DARKFLAT")
							.arg(si.m_pDarkFlatTask->m_vBitmaps.size())
							.arg(strISOGainText)
							.arg(strISOGainValue)
							.arg(strExposure);

				if (si.m_pDarkFlatTask->m_vBitmaps.size()>1)
				{
					stream << "<ul>" << QCoreApplication::translate("StackRecap", "Method: ", "IDS_RECAP_METHOD") << "<br>";
					FormatFromMethod(strTempText, si.m_pDarkFlatTask->m_Method, si.m_pDarkFlatTask->m_fKappa, si.m_pDarkFlatTask->m_lNrIterations);
					stream << strTempText << "</ul>";
				}
				else
					stream << "<br>";

				if (si.m_pDarkFlatTask->HasISOSpeed())
				{
					if (si.m_pDarkFlatTask->m_lISOSpeed != si.m_pFlatTask->m_lISOSpeed)
						stream << QCoreApplication::translate("StackRecap", "Warning: ISO speed does not match that of the flat frames", "IDS_RECAP_ISOWARNINGDARKFLAT");
				}
				else
				{
					if (si.m_pDarkFlatTask->m_lGain != si.m_pFlatTask->m_lGain)
						stream << QCoreApplication::translate("StackRecap", "Warning: Gain does not match that of the flat frames", "IDS_RECAP_GAINWARNINGDARKFLAT");
				}
				if (!AreExposureEquals(si.m_pDarkFlatTask->m_fExposure, si.m_pFlatTask->m_fExposure))
					stream << QCoreApplication::translate("StackRecap", "Warning: Exposure does not match that of the flat frames", "IDS_RECAP_EXPOSUREWARNINGDARKFLAT");
				stream << "</ul>";
			}

			if (si.m_pFlatTask)
			{
				strExposure = exposureToString(si.m_pFlatTask->m_fExposure);
				GetISOGainStrings(si.m_pFlatTask, strISOText, strGainText, strISOGainText, strISOGainValue);

				// SCS: I think is an error - if nothing else si.m_pDarkFlatTask can be NULL if you have a flat but no dark flat!
				//GetISOGainStrings(si.m_pDarkFlatTask, strISOText, strGainText, strISOGainText, strISOGainValue);

				stream << QCoreApplication::translate("StackRecap", "-> Flat: %1 frames (%2: %3) exposure: %4", "IDS_RECAP_FLAT")
							.arg(si.m_pFlatTask->m_vBitmaps.size())
							.arg(strISOGainText)
							.arg(strISOGainValue)
							.arg(strExposure);

				if (si.m_pFlatTask->m_vBitmaps.size()>1)
				{
					stream << "<ul>" << QCoreApplication::translate("StackRecap", "Method: ", "IDS_RECAP_METHOD") << "<br>";
					FormatFromMethod(strTempText, si.m_pFlatTask->m_Method, si.m_pFlatTask->m_fKappa, si.m_pFlatTask->m_lNrIterations);
					stream << strTempText << "</ul>";
				}

				if (si.m_pFlatTask->HasISOSpeed())
				{
					if (si.m_pFlatTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						stream << QCoreApplication::translate("StackRecap", "Warning: ISO speed does not match that of the light frames", "IDS_RECAP_ISOWARNING");
				}
				else
				{
					if (si.m_pFlatTask->m_lGain != si.m_pLightTask->m_lGain)
						stream << QCoreApplication::translate("StackRecap", "Warning: Gain does not match that of the light frames", "IDS_RECAP_GAINWARNING");
				}
				stream << "</ul>";
			}
			else
			{
				stream << QCoreApplication::translate("StackRecap", "-> No Flat", "IDS_RECAP_NOFLAT");
			}

			if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
				stream << "</ul>";
			stream << "</td></tr></table><br>";
		}
	}

	if (m_vBitmaps.size())
	{
		for (i = 0;i<tasks.m_vStacks.size();i++)
		{
			CStackingInfo &			si = tasks.m_vStacks[i];

			if (si.m_pLightTask)
			{
				stream << "<hr><br>" << Qt::endl;
				stream << "<a name=\"Task" << i << "\"></a>";
						
				stream << "<b>" << QCoreApplication::translate("DSS::Group", "Light", "IDS_TYPE_LIGHT")  << "</b><br>\n";
				for (j = 0; j < si.m_pLightTask->m_vBitmaps.size(); j++)
					stream << si.m_pLightTask->m_vBitmaps[j].filePath.string().c_str() << "<br>";

				if (si.m_pOffsetTask && si.m_pOffsetTask->m_vBitmaps.size())
				{
					stream << "<b>" << QCoreApplication::translate("DSS::Group", "Bias/Offset", "IDS_TYPE_OFFSET") << "</b><br>\n";							
					if (si.m_pOffsetTask->m_strOutputFile != si.m_pOffsetTask->m_vBitmaps[0].filePath)
						stream << QCoreApplication::translate("DSS::Group", "Master Offset", "IDS_TYPE_MASTEROFFSET") << " -> " << si.m_pOffsetTask->m_strOutputFile.string().c_str() << "<br>";
					for (j = 0; j < si.m_pOffsetTask->m_vBitmaps.size(); j++)
						stream << si.m_pOffsetTask->m_vBitmaps[j].filePath.string().c_str() << "<br>";
				}

				if (si.m_pDarkTask && si.m_pDarkTask->m_vBitmaps.size())
				{
					stream << "<b>" << QCoreApplication::translate("DSS::Group", "Dark", "IDS_TYPE_DARK") << "</b><br>\n";
					if (si.m_pDarkTask->m_strOutputFile != si.m_pDarkTask->m_vBitmaps[0].filePath)
						stream << QCoreApplication::translate("DSS::Group", "Master Dark", "IDS_TYPE_MASTERDARK") << " -> " << si.m_pDarkTask->m_strOutputFile.string().c_str() << "<br>";
					for (j = 0;j<si.m_pDarkTask->m_vBitmaps.size();j++)
						stream << si.m_pDarkTask->m_vBitmaps[j].filePath.string().c_str() << "<br>";
				}

				if (si.m_pDarkFlatTask && si.m_pDarkFlatTask->m_vBitmaps.size())
				{
					stream << "<b>" << QCoreApplication::translate("DSS::Group", "Dark Flat", "IDS_TYPE_DARKFLAT") << "</b><br>\n";
					if (si.m_pDarkFlatTask->m_strOutputFile != si.m_pDarkFlatTask->m_vBitmaps[0].filePath)
						stream << QCoreApplication::translate("DSS::Group", "Master Dark Flat", "IDS_TYPE_MASTERDARKFLAT") << " -> " << si.m_pDarkFlatTask->m_strOutputFile.string().c_str() << "<br>";
					for (j = 0;j<si.m_pDarkFlatTask->m_vBitmaps.size();j++)
						stream << si.m_pDarkFlatTask->m_vBitmaps[j].filePath.string().c_str() << "<br>";
				};
				if (si.m_pFlatTask && si.m_pFlatTask->m_vBitmaps.size())
				{
					stream << "<b>" << QCoreApplication::translate("DSS::Group", "Flat", "IDS_TYPE_FLAT") << "</b><br>\n";
					if (si.m_pFlatTask->m_strOutputFile != si.m_pFlatTask->m_vBitmaps[0].filePath)
						stream << QCoreApplication::translate("DSS::Group", "Master Flat", "IDS_TYPE_MASTERFLAT") << " -> " << si.m_pFlatTask->m_strOutputFile.string().c_str() << "<br>";
					for (j = 0;j<si.m_pFlatTask->m_vBitmaps.size();j++)
						stream << si.m_pFlatTask->m_vBitmaps[j].filePath.string().c_str() << "<br>";
				};
			};
		};
	};

	stream << "<br><a href=\"http://deepskystacker.free.fr\">DeepSkyStacker " << VERSION_DEEPSKYSTACKER << "</a>";
	stream << "</body>" << Qt::endl;
	stream << "</html>" << Qt::endl;

	auto bytesWritten = file.write(buffer);
	ZASSERTSTATE(bytesWritten == buffer.size());
}

/* ------------------------------------------------------------------- */
