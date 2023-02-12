#include <stdafx.h>
#include "StackingEngine.h"

#include "MasterFrames.h"
#include "MatchingStars.h"
#include "PixelTransform.h"
#include "EntropyInfo.h"
#include <math.h>
#include "TIFFUtil.h"
#include "FITSUtil.h"
#include "Multitask.h"
#include "Histogram.h"
#include "Filters.h"
#include "CosmeticEngine.h"
#include "ChannelAlign.h"
#include <iostream>
#include "FrameInfoSupport.h"
#include "avx.h"
#include "avx_avg.h"
#include <omp.h>
#include <QRectF>


#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::SetReferenceFrame(LPCTSTR szReferenceFrame)
{
	ZFUNCTRACE_RUNTIME();
	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szName[1+_MAX_FNAME];

	_tsplitpath(szReferenceFrame, szDrive, szDir, szName, nullptr);

	m_strReferenceFrame = szReferenceFrame;
	m_strStackingFileInfo.Format(_T("%s%s%s.stackinfo.txt"), szDrive, szDir, szName);

	unsigned int dwAlignmentTransformation = 2;
	Workspace workspace;

	dwAlignmentTransformation = workspace.value("Stacking/AlignmentTransformation", (uint)2).toUInt();

	// Init from the file
	m_vLightFrameStackingInfo.clear();
	FILE* hFile = _tfopen((LPCTSTR)m_strStackingFileInfo, _T("rt"));
	if (hFile)
	{
		bool			bEnd = false;
		CHAR			szLine[10000];

		if (fgets(szLine, sizeof(szLine), hFile))
		{
			int		lSavedAlignmentTransformation;
			CString		strValue;

			strValue = CA2TEX<sizeof(szLine)>(szLine);
			strValue.TrimRight(_T("\n"));

			lSavedAlignmentTransformation = _ttol((LPCTSTR)strValue);

			if (lSavedAlignmentTransformation != dwAlignmentTransformation)
				bEnd = true;
		}
		else
			bEnd = true;

		if (!bEnd)
		{
			if (fgets(szLine, sizeof(szLine), hFile))
			{
				CString		strInfoFileName;
				CString		strStoredInfoFileName;

				strStoredInfoFileName = CA2TEX<sizeof(szLine)>(szLine);
				strStoredInfoFileName.TrimRight(_T("\n"));

				GetInfoFileName((LPCTSTR)m_strReferenceFrame, strInfoFileName);
				if (strInfoFileName.CompareNoCase(strStoredInfoFileName))
					bEnd = true;
			}
			else
				bEnd = true;
		};

		while (!bEnd)
		{
			CLightFrameStackingInfo		lfsi;
			bool						bResult = true;

			if (fgets(szLine, sizeof(szLine), hFile))
			{
				lfsi.m_strInfoFileName = szLine;
				lfsi.m_strInfoFileName.TrimRight(_T("\n"));
			}
			else
				bEnd = true;

			if (fgets(szLine, sizeof(szLine), hFile))
			{
				lfsi.m_strFileName = szLine;
				lfsi.m_strFileName.TrimRight(_T("\n"));
			}
			else
				bEnd = true;

			if (fgets(szLine, sizeof(szLine), hFile))
			{
				CString					strParameters;

				strParameters = szLine;
				strParameters.TrimRight(_T("\n"));
				bResult = lfsi.m_BilinearParameters.FromText((LPCTSTR)strParameters);
			}
			else
				bEnd = true;

			if (!bEnd && bResult)
				m_vLightFrameStackingInfo.push_back(lfsi);
		};

		fclose(hFile);
		std::sort(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end());
	};
};

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::GetInfoFileName(LPCTSTR szLightFrame, CString& strInfoFileName)
{
	//ZFUNCTRACE_RUNTIME();

	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szName[1+_MAX_FNAME];

	_tsplitpath(szLightFrame, szDrive, szDir, szName, nullptr);

	strInfoFileName.Empty();
	strInfoFileName.Format(_T("%s%s%s.Info.txt"), szDrive, szDir, szName);

	// Retrieve the file date/time
	FILETIME		FileTime;
	SYSTEMTIME		SystemTime;
	TCHAR			szTime[200];
	TCHAR			szDate[200];

	if (GetFileCreationDateTime((LPCTSTR)strInfoFileName, FileTime))
	{
		FileTimeToSystemTime(&FileTime, &SystemTime);
		SystemTimeToTzSpecificLocalTime(nullptr, &SystemTime, &SystemTime);

		GetDateFormat(LOCALE_USER_DEFAULT, 0, &SystemTime, nullptr, szDate, sizeof(szDate)/sizeof(TCHAR));
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &SystemTime, nullptr, szTime, sizeof(szTime)/sizeof(TCHAR));

		strInfoFileName.Format(_T("%s%s%s.Info.txt [%s %s]"), szDrive, szDir, szName, szDate, szTime);
	}
	else
		strInfoFileName.Empty();
};

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::AddLightFrame(LPCTSTR szLightFrame, const CBilinearParameters& bp)
{
	ZFUNCTRACE_RUNTIME();

	CLightFrameStackingInfo lfsi(szLightFrame);
	CString strInfoFileName;

	GetInfoFileName(szLightFrame, strInfoFileName);
	LIGHTFRAMESTACKINGINFOITERATOR it = std::lower_bound(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end(), lfsi);
	if (it != m_vLightFrameStackingInfo.end() && !it->m_strFileName.CompareNoCase(szLightFrame))
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

bool CLightFramesStackingInfo::GetParameters(LPCTSTR szLightFrame, CBilinearParameters& bp)
{
	// ZFUNCTRACE_RUNTIME();
	bool bResult = false;

	LIGHTFRAMESTACKINGINFOITERATOR it = std::lower_bound(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end(), CLightFrameStackingInfo(szLightFrame));
	if (it != m_vLightFrameStackingInfo.end() && !it->m_strFileName.CompareNoCase(szLightFrame))
	{
		CString strInfoFileName;
		GetInfoFileName(szLightFrame, strInfoFileName);

		if (!strInfoFileName.CompareNoCase(it->m_strInfoFileName))
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

	if (m_strReferenceFrame.GetLength() && m_strStackingFileInfo.GetLength())
	{
		FILE* hFile = _tfopen((LPCTSTR)m_strStackingFileInfo, _T("wt"));
		if (hFile)
		{
			// Save the alignment transformation used
			unsigned int dwAlignmentTransformation = 2;
			Workspace workspace;

			dwAlignmentTransformation = workspace.value("Stacking/AlignmentTransformation", (uint)2).toUInt();
			fprintf(hFile,"%ld\n", dwAlignmentTransformation);

			CString strInfoFileName;
			GetInfoFileName((LPCTSTR)m_strReferenceFrame, strInfoFileName);
			fprintf(hFile, "%s\n", (LPCSTR)CT2CA(strInfoFileName, CP_UTF8));

			for (const auto& stackingInfo : m_vLightFrameStackingInfo)
			{
				fprintf(hFile, "%s\n", (LPCSTR)CT2CA(stackingInfo.m_strInfoFileName, CP_UTF8));
				fprintf(hFile, "%s\n", (LPCSTR)CT2CA(stackingInfo.m_strFileName, CP_UTF8));

				CString			strParameters;

				stackingInfo.m_BilinearParameters.ToText(strParameters);
				fprintf(hFile, "%s\n", (LPCSTR)CT2CA(strParameters, CP_UTF8));
			};

			fclose(hFile);
		};
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
		const bool bMonochrome = pBitmap->IsMonochrome();

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
	if (m_strReferenceFrame.GetLength())
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
				lfi.SetBitmap(bitmap.filePath.c_str(), false, false);

				if (lfi.IsRegistered())
				{
					lfi = bitmap;
					lfi.RefreshSuperPixel();

					if (!m_strReferenceFrame.CompareNoCase(lfi.filePath.c_str()))
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
		fs::path path { m_strReferenceFrame.GetString() };
		if (fi.InitFromFile(path, PICTURETYPE_LIGHTFRAME))
		{
			lfi.SetBitmap(path , false, false);
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

	m_CriticalSection.Lock();
	bResult = m_StackingInfo.GetParameters(m_vBitmaps[lBitmapIndice].filePath.c_str(), BilinearParameters);
	m_CriticalSection.Unlock();

	if (bResult)
	{
		BilinearParameters.Offsets(m_vBitmaps[lBitmapIndice].m_fXOffset, m_vBitmaps[lBitmapIndice].m_fYOffset);
		m_vBitmaps[lBitmapIndice].m_fAngle = BilinearParameters.Angle(m_vBitmaps[lBitmapIndice].RenderedWidth());
		m_vBitmaps[lBitmapIndice].m_BilinearParameters = BilinearParameters;
	}
	else if (GetTransformationType() == TT_NONE)
	{
		// Automatic acknowledgment of the transformation
		m_CriticalSection.Lock();
		m_StackingInfo.AddLightFrame(m_vBitmaps[lBitmapIndice].filePath.c_str(), BilinearParameters);
		m_CriticalSection.Unlock();

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

		m_CriticalSection.Lock();
		std::sort(vStarsOrg.begin(), vStarsOrg.end(), CompareStarLuminancy);
		std::sort(vStarsDst.begin(), vStarsDst.end(), CompareStarLuminancy);
		m_CriticalSection.Unlock();

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
			m_CriticalSection.Lock();
			m_StackingInfo.AddLightFrame(m_vBitmaps[lBitmapIndice].filePath.c_str(), BilinearParameters);
			m_CriticalSection.Unlock();
		};
	};

	return bResult;
};


inline bool CompareDateTime(const SYSTEMTIME & dt1, const SYSTEMTIME & dt2)
{
	if (dt1.wYear>dt2.wYear)
		return true;
	else if (dt1.wYear<dt2.wYear)
		return false;
	else if (dt1.wMonth>dt2.wMonth)
		return true;
	else if (dt1.wMonth<dt2.wMonth)
		return false;
	else if (dt1.wDay>dt2.wDay)
		return true;
	else if (dt1.wDay<dt2.wDay)
		return false;
	else if (dt1.wHour>dt2.wHour)
		return true;
	else if (dt1.wHour<dt2.wHour)
		return false;
	else if (dt1.wMinute>dt2.wMinute)
		return true;
	else if (dt1.wMinute<dt2.wMinute)
		return false;
	else if (dt1.wSecond>dt2.wSecond)
		return true;
	else if (dt1.wSecond<dt2.wSecond)
		return false;
	else
		return false;
};

/* ------------------------------------------------------------------- */

double	ElapsedTime(const SYSTEMTIME & dt1, const SYSTEMTIME & dt2)
{
	FILETIME			ft1, ft2;
	ULARGE_INTEGER		t1, t2;

	SystemTimeToFileTime(&dt1, &ft1);
	SystemTimeToFileTime(&dt2, &ft2);

	memcpy(&t1, &ft1, sizeof(t1));
	memcpy(&t2, &ft2, sizeof(t2));

	return (t2.QuadPart-t1.QuadPart)/10000000.0;
};

/* ------------------------------------------------------------------- */

inline bool CompareLightFrameDate (const CLightFrameInfo * plfi1, const CLightFrameInfo * plfi2)
{
	return CompareDateTime(plfi2->m_DateTime, plfi1->m_DateTime);
};

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
				ptrdiff_t lPreviousIndex, lNextIndex;

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
						double fElapsed;

						bContinue = false;
						for (ptrdiff_t j = lPreviousIndex - 1; j >= 0 && !bFound; j--)
						{
							if (vpLightFrames[j]->m_bComet)
							{
								fElapsed = ElapsedTime(vpLightFrames[j]->m_DateTime, pNextComet->m_DateTime);
								if (fElapsed / 3600 < 12)
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
								fElapsed = ElapsedTime(pPreviousComet->m_DateTime, vpLightFrames[j]->m_DateTime);
								if (fElapsed / 3600 < 12)
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

					double				fElapsed2,
										fElapsedCurrent;

					fElapsed2 = ElapsedTime(pPreviousComet->m_DateTime, pNextComet->m_DateTime);
					fElapsedCurrent = ElapsedTime(pPreviousComet->m_DateTime, vpLightFrames[i]->m_DateTime);

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

#pragma omp parallel for schedule(dynamic) default(none) shared(stop, nLoopCount, strText) if(nrProcessors > 1)
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

	std::sort(m_vBitmaps.begin(), m_vBitmaps.end());

	if (m_vBitmaps[0].m_bDisabled)
		m_lNrStackable = 0;
	else
		m_lNrStackable = std::min(static_cast<int>(m_vBitmaps.size()), 1);
	m_lNrCometStackable = 0;
	const QString strText(QCoreApplication::translate("StackingEngine", "Computing offsets", "IDS_COMPUTINGOFFSETS"));

	const int lLast = static_cast<int>(m_vBitmaps.size() * m_fKeptPercentage / 100.0);
	if (m_pProgress)
		m_pProgress->Start1(strText, lLast, false);

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
			ComputeMissingCometPositions();
			m_StackingInfo.Save();
		}
	}
}

/* ------------------------------------------------------------------- */

bool	CStackingEngine::IsLightFrameStackable(LPCTSTR szFile)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	for (int i = 0;i<m_vBitmaps.size() && !bResult;i++)
	{
		if (!m_vBitmaps[i].filePath.compare(szFile))
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
				if (IsLightFrameStackable(bitmap.filePath.c_str()))
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
int CStackingEngine::FindBitmapIndex(LPCTSTR szFile)
{
	ZFUNCTRACE_RUNTIME();

	for (size_t i = 0; i < m_vBitmaps.size(); i++)
	{
		if (m_vBitmaps[i].filePath.compare(szFile) == 0)
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

		ZTRACE_RUNTIME("Compute resulting bitmap");
		if (!m_vCometShifts.empty())
		{
			std::sort(m_vCometShifts.begin(), m_vCometShifts.end());

			std::vector<int> vImageOrder;
			std::transform(m_vCometShifts.cbegin(), m_vCometShifts.cend(), std::back_inserter(vImageOrder), [](const auto& cometShift) -> int { return cometShift.m_lImageIndex; });
			m_pMasterLight->SetImageOrder(vImageOrder);

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
	bool bResult = false;

	if (!m_vPixelTransforms.empty())
	{
		ZTRACE_RUNTIME("Adjust Bayer Drizzle Coverage");

		double fMaxCoverage = 0;
		int lProgress = 0;
		QString strText;

		std::unique_ptr<C96BitFloatColorBitmap> pCover = std::make_unique<C96BitFloatColorBitmap>();
		pCover->Init(m_rcResult.width(), m_rcResult.height());

		strText = QCoreApplication::translate("StackingEngine", "Stacking - Adjust Bayer - Compute adjustment", "IDS_STACKING_COMPUTINGADJUSTMENT");
		if (m_pProgress)
			m_pProgress->Start1(strText, static_cast<int>(m_vPixelTransforms.size()), false);

		int lNrBitmaps = 0;
		for (const CPixelTransform& PixTransform : m_vPixelTransforms)
		{
			++lNrBitmaps;
			if (m_pProgress != nullptr)
			{
				strText = QCoreApplication::translate("StackingEngine", "Compute adjustment %1 of %2", "IDS_COMPUTINGADJUSTMENT").arg(lNrBitmaps + 1).arg(m_vPixelTransforms.size());
				m_pProgress->Progress1(strText, lNrBitmaps + 1);
				m_pProgress->Start2(QString(" "), m_rcResult.width() * m_rcResult.height());
			}

			lProgress = 0;
			for (int j = 0; j < m_rcResult.height(); j++)
			{
				for (int i = 0; i < m_rcResult.width(); i++)
				{
					lProgress++;
					const QPointF ptOut = PixTransform.transform(QPointF(i, j));

					if (DSSRect{ 0, 0, m_rcResult.width(), m_rcResult.height() }.contains(ptOut))
					{
						PIXELDISPATCHVECTOR vPixels;
						ComputePixelDispatch(ptOut, vPixels);

						for (const CPixelDispatch& pixDispatch : vPixels)
						{
							// For each plane adjust the values
							if (pixDispatch.m_lX >= 0 && 
								pixDispatch.m_lX < m_rcResult.width() &&
								pixDispatch.m_lY >= 0 &&
								pixDispatch.m_lY < m_rcResult.height())
							{
								double fRedCover, fGreenCover, fBlueCover;
								pCover->GetValue(pixDispatch.m_lX, pixDispatch.m_lY, fRedCover, fGreenCover, fBlueCover);

								switch (GetBayerColor(i, j, m_InputCFAType))
								{
								case BAYER_RED:   fRedCover   += pixDispatch.m_fPercentage; break;
								case BAYER_GREEN: fGreenCover += pixDispatch.m_fPercentage; break;
								case BAYER_BLUE:  fBlueCover  += pixDispatch.m_fPercentage; break;
								}

								pCover->SetValue(pixDispatch.m_lX, pixDispatch.m_lY, fRedCover, fGreenCover, fBlueCover);
							}
						}
					}
				}
				if (m_pProgress != nullptr)
					m_pProgress->Progress2(lProgress);
			}

			if (m_pProgress != nullptr)
				m_pProgress->End2();
		}


		m_vPixelTransforms.clear();

		lProgress = 0;
		if (m_pProgress != nullptr)
		{
			strText = QCoreApplication::translate("StackingEngine", "Stacking - Adjust Bayer - Apply adjustment", "IDS_STACKING_APPLYINGADJUSTMENT");
			m_pProgress->Start1(strText, 2, false);
			strText = QCoreApplication::translate("StackingEngine", "Compute maximum adjustment", "IDS_STACKING_COMPUTEMAXADJUSTMENT");
			m_pProgress->Start2(strText, m_rcResult.width() * m_rcResult.height());
		};

		// Compute the maximum coverage
		for (int j = 0; j < m_rcResult.height(); j++)
		{
			for (int i = 0; i < m_rcResult.width(); i++)
			{
				double			fRedCover,
								fGreenCover,
								fBlueCover;

				lProgress++;

				pCover->GetValue(i, j, fRedCover, fGreenCover, fBlueCover);

				fMaxCoverage = max(fMaxCoverage, fRedCover);
				fMaxCoverage = max(fMaxCoverage, fGreenCover);
				fMaxCoverage = max(fMaxCoverage, fBlueCover);
			}

			if (m_pProgress != nullptr)
				m_pProgress->Progress2(lProgress);
		}

		if (m_pProgress != nullptr)
		{
			m_pProgress->End2();
			m_pProgress->Progress1(1);
		}

		lProgress = 0;
		if (m_pProgress != nullptr)
		{
			strText = QCoreApplication::translate("StackingEngine", "Applying adjustment", "IDS_STACKING_APPLYADJUSTMENT");
			m_pProgress->Start2(strText, m_rcResult.width() * m_rcResult.height());
		}

		// Adjust the coverage of all pixels
		for (int j = 0; j < m_rcResult.height(); j++)
		{
			for (int i = 0; i < m_rcResult.width(); i++)
			{
				lProgress++;
				double fRedCover, fGreenCover, fBlueCover;
				double fRed, fGreen, fBlue;

				pCover->GetValue(i, j, fRedCover, fGreenCover, fBlueCover);
				m_pOutput->GetValue(i, j, fRed, fGreen, fBlue);

				if (fRedCover > 0)
					fRed *= fMaxCoverage / fRedCover;
				if (fGreenCover > 0)
					fGreen *= fMaxCoverage / fGreenCover;
				if (fBlueCover > 0)
					fBlue *= fMaxCoverage / fBlueCover;

				m_pOutput->SetValue(i, j, fRed, fGreen, fBlue);
			}

			if (m_pProgress != nullptr)
				m_pProgress->Progress2(lProgress);
		}

		if (m_pProgress != nullptr)
			m_pProgress->End2();

		bResult = true;
	}

	return bResult;
}

/* ------------------------------------------------------------------- */

bool CStackingEngine::SaveCalibratedAndRegisteredLightFrame(CMemoryBitmap* pBitmap) const
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;

	if (m_strCurrentLightFrame.GetLength() != 0 && pBitmap != nullptr)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, nullptr);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += ".reg.tif";
		else
		{
			CString			strExt;

			GetFITSExtension(m_strCurrentLightFrame, strExt);
			strOutputFile += ".reg"+strExt;
		};

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Registered and Calibrated image in %1", "IDS_SAVINGINTERMEDIATE").arg(QString::fromWCharArray(strOutputFile.GetString())));
			m_pProgress->Start2(strText, 0);
		};
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pBitmap, m_pProgress, _T("Registered and Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure, m_pLightTask->m_fAperture);
		else
			bResult = WriteFITS(strOutputFile, pBitmap, m_pProgress, _T("Registered and Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure);
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

	if (m_strCurrentLightFrame.GetLength() != 0 && static_cast<bool>(pBitmap))
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, nullptr);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += ".cal.tif";
		else
		{
			CString			strExt;

			GetFITSExtension(m_strCurrentLightFrame, strExt);
			strOutputFile += ".cal"+strExt;
		};


		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(QString::fromWCharArray(strOutputFile.GetString())));
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
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pOutBitmap.get(), m_pProgress, _T("Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure, m_pLightTask->m_fAperture);
		else
			bResult = WriteFITS(strOutputFile, pOutBitmap.get(), m_pProgress, _T("Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_lGain, m_pLightTask->m_fExposure);

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

	if (m_strCurrentLightFrame.GetLength() != 0 && pBitmap != nullptr)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, nullptr);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += ".cosmetic.tif";
		else
		{
			CString			strExt;

			GetFITSExtension(m_strCurrentLightFrame, strExt);
			strOutputFile += ".cosmetic"+strExt;
		};

		if (m_pProgress)
			m_pProgress->Start2(0);
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pBitmap, m_pProgress, _T("Delta Cosmetic Image"));
		else
			bResult = WriteFITS(strOutputFile, pBitmap, m_pProgress, _T("Delta Cosmetic Image"));
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

	if (m_strCurrentLightFrame.GetLength() != 0 && pBitmap != nullptr)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, nullptr);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += "Comet.tif";
		else
		{
			CString		strExt;

			GetFITSExtension(m_strCurrentLightFrame, strExt);
			strOutputFile += "Comet"+strExt;
		};

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(QString::fromWCharArray(strOutputFile.GetString())));
			m_pProgress->Start2(strText, 0);
		};
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pBitmap, m_pProgress, _T("Comet alone"));
		else
			bResult = WriteFITS(strOutputFile, pBitmap, m_pProgress, _T("Comet alone"));
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

	if (m_strCurrentLightFrame.GetLength() != 0 && pBitmap != nullptr)
	{
		TCHAR			szDrive[1 + _MAX_DRIVE];
		TCHAR			szDir[1 + _MAX_DIR];
		TCHAR			szName[1 + _MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, nullptr);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += "Cometless.tif";
		else
		{
			CString		strExt;

			GetFITSExtension(m_strCurrentLightFrame, strExt);

			strOutputFile += "Cometless" + strExt;
		}

		if (m_pProgress)
		{
			const QString strText(QCoreApplication::translate("StackingEngine", "Saving Calibrated image in %1", "IDS_SAVINGCALIBRATED").arg(QString::fromWCharArray(strOutputFile.GetString())));
			m_pProgress->Start2(strText, 0);
		}

		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pBitmap, m_pProgress, _T("Cometless image"));
		else
			bResult = WriteFITS(strOutputFile, pBitmap, m_pProgress, _T("Cometless image"));

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
		m_pBitmap{ pBitmap },
		m_pProgress{ pProgress }
	{}

	void process();
private:
	void processNonAvx(const int lineStart, const int lineEnd);
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

#pragma omp parallel for default(none) firstprivate(avxStacking) shared(runOnlyOnce) if(nrProcessors > 1) // No "schedule" clause gives fastest result.
	for (int row = 0; row < height; row += lineBlockSize)
	{
		const int endRow = std::min(row + lineBlockSize, height);
		avxStacking.init(row, endRow);
		// First try AVX version, if it cannot run then process without AVX.
		if (avxStacking.stack(m_PixTransform, *m_pLightTask, m_BackgroundCalibration, m_lPixelSizeMultiplier) != 0)
		{
			this->processNonAvx(row, endRow);
		}
		else {
			if (runOnlyOnce.exchange(true) == false) // If it was false before -> we are the first one.
				ZTRACE_RUNTIME("AvxStacking::stack %d rows in chunks of size %d", height, lineBlockSize);
		}

		if (omp_get_thread_num() == 0 && m_pProgress != nullptr)
			m_pProgress->Progress2(progress += nrProcessors * lineBlockSize);
	}
}

void CStackTask::processNonAvx(const int lineStart, const int lineEnd)
{
	const int width = m_pBitmap->Width();
	PIXELDISPATCHVECTOR vPixels;
	vPixels.reserve(16);

	for (int j = lineStart; j < lineEnd; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			const QPointF ptOut = m_PixTransform.transform(QPointF(i, j));

			COLORREF16 crColor;
			double fRedEntropy = 1.0, fGreenEntropy = 1.0, fBlueEntropy = 1.0;

			if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
				m_EntropyWindow.GetPixel(i, j, fRedEntropy, fGreenEntropy, fBlueEntropy, crColor);
			else
				m_pBitmap->GetPixel16(i, j, crColor);

			float Red = crColor.red;
			float Green = crColor.green;
			float Blue = crColor.blue;

			if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
				m_BackgroundCalibration.ApplyCalibration(Red, Green, Blue);

			if ((0 != Red || 0 != Green || 0 != Blue) &&
				DSSRect { 0, 0, m_rcResult.width(), m_rcResult.height() }.contains(ptOut))
			{
				vPixels.resize(0);
				ComputePixelDispatch(ptOut, m_lPixelSizeMultiplier, vPixels);

				for (CPixelDispatch& Pixel : vPixels)
				{
					// For each plane adjust the values
					if (Pixel.m_lX >= 0 &&
						Pixel.m_lX < m_rcResult.width() &&
						Pixel.m_lY >= 0 && Pixel.m_lY < m_rcResult.height())
					{
						// Special case for entropy average
						if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
						{
							if (m_bColor)
							{
								double fOldRed, fOldGreen, fOldBlue;

								m_pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
								fOldRed += Pixel.m_fPercentage * fRedEntropy;
								fOldGreen += Pixel.m_fPercentage * fGreenEntropy;
								fOldBlue += Pixel.m_fPercentage * fBlueEntropy;
								m_pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);

								m_pOutput->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
								fOldRed += Red * Pixel.m_fPercentage * fRedEntropy;
								fOldGreen += Green * Pixel.m_fPercentage * fGreenEntropy;
								fOldBlue += Blue * Pixel.m_fPercentage * fBlueEntropy;
								m_pOutput->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
							}
							else
							{
								double fOldGray;

								m_pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
								fOldGray += Pixel.m_fPercentage * fRedEntropy;
								m_pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);

								m_pOutput->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
								fOldGray += Red * Pixel.m_fPercentage * fRedEntropy;
								m_pOutput->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
							}
						}

						double fPreviousRed, fPreviousGreen, fPreviousBlue;

						m_pTempBitmap->GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
						fPreviousRed += static_cast<double>(Red) / 256.0 * Pixel.m_fPercentage;
						fPreviousGreen += static_cast<double>(Green) / 256.0 * Pixel.m_fPercentage;
						fPreviousBlue += static_cast<double>(Blue) / 256.0 * Pixel.m_fPercentage;
						fPreviousRed = std::min(fPreviousRed, 255.0);
						fPreviousGreen = std::min(fPreviousGreen, 255.0);
						fPreviousBlue = std::min(fPreviousBlue, 255.0);
						m_pTempBitmap->SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
					}
				}
			}
		}
	}
}

/* ------------------------------------------------------------------- */

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

bool CStackingEngine::StackLightFrame(std::shared_ptr<CMemoryBitmap> pInBitmap, CPixelTransform& PixTransform, double fExposure, bool bComet)
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
				QString strText(QCoreApplication::translate("StackingEngine", "Interpolating with Adaptive Homogeneity Directed (AHD)", "IDS_AHDDEMOSAICING"));
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
			int lProgress = 0;

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
				m_pMasterLight->AddBitmap(StackTask.m_pTempBitmap.get(), m_pProgress);
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

	return bResult;
}

#include <future>

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
		switch (tasks.GetStackingMode())
		{
		case SM_MOSAIC:
		{
			CString strDrive;
			CString strFreeSpace;
			CString strNeededSpace;

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
				SpaceToString(ulFreeSpace, strFreeSpace);
				SpaceToString(ulNeededSpace, strNeededSpace);

				const QString strText(QCoreApplication::translate("StackingEngine", "The process needs temporarily %1 of free space on the %2 drive.\nOnly %3 are available on this drive.", "IDS_RECAP_WARNINGDISKSPACE").arg(QString::fromWCharArray(strNeededSpace)).arg(QString::fromWCharArray(strDrive)).arg(QString::fromWCharArray(strFreeSpace)) +
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
			tasks.GetCustomRectangle(m_rcResult);
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
							if (tasks.m_vStacks[i].m_pLightTask->m_dwTaskID == lFirstTaskID)
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
					MasterFrames.LoadMasters(pStackingInfo, m_pProgress);

					m_pLightTask = pStackingInfo->m_pLightTask;

					if ((m_pLightTask->m_Method == MBP_AVERAGE) && !m_bCreateCometImage && !static_cast<bool>(m_pComet)) {
						m_pLightTask->m_Method = MBP_FASTAVERAGE;
					}

					const auto readTask = [this, pStackingInfo, firstBitmap = m_vBitmaps.cbegin()](const size_t lightTaskNdx, ProgressBase* pProgress) -> std::pair<std::shared_ptr<CMemoryBitmap>, int>
					{
						if (lightTaskNdx >= pStackingInfo->m_pLightTask->m_vBitmaps.size())
							return { {}, -1 };
						const int bitmapNdx = FindBitmapIndex(pStackingInfo->m_pLightTask->m_vBitmaps[lightTaskNdx].filePath.c_str());
						if (bitmapNdx < 0)
							return { {}, -1 };
						const auto& lightframeInfo = m_vBitmaps[bitmapNdx];
						if (lightframeInfo.m_bDisabled)
							return { {}, -1 };

						ZTRACE_RUNTIME("Stack %s", lightframeInfo.filePath.generic_string().c_str());

						std::shared_ptr<CMemoryBitmap> rpBitmap;
						if (::LoadFrame(lightframeInfo.filePath.c_str(), PICTURETYPE_LIGHTFRAME, pProgress, rpBitmap))
							return { rpBitmap, bitmapNdx };
						else
							return { {}, -1 };
					};

					auto futureForRead = std::async(std::launch::deferred, readTask, 0, m_pProgress); // Load first lightframe synchronously.
					const auto firstBitmap = m_vBitmaps.cbegin();

					for (size_t i = 0; i < pStackingInfo->m_pLightTask->m_vBitmaps.size() && !bStop; ++i)
					{
						auto [pBitmap, bitmapNdx] = futureForRead.get();
						futureForRead = std::async(std::launch::async, readTask, i + 1, nullptr); // Immediately load next lightframe asynchronously (need to set progress pointer to null).

						if (bitmapNdx < 0)
							continue;

						const auto& lightframeInfo = m_vBitmaps[bitmapNdx];
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

						const auto strDescription = lightframeInfo.m_strInfos;
						if (lightframeInfo.m_lNrChannels == 3)
							strText = QCoreApplication::translate("StackingEngine", "Stacking %1 bit/ch %2 light frame\n%3", "IDS_STACKRGBLIGHT").arg(lightframeInfo.m_lBitPerChannels).arg(static_cast<LPCTSTR>(strDescription)).arg(static_cast<LPCTSTR>(lightframeInfo.filePath.c_str()));
						else
							strText = QCoreApplication::translate("StackingEngine", "Stacking %1 bits gray %2 light frame\n%3", "IDS_STACKGRAYLIGHT").arg(lightframeInfo.m_lBitPerChannels).arg(static_cast<LPCTSTR>(strDescription)).arg(static_cast<LPCTSTR>(lightframeInfo.filePath.c_str()));

						ZTRACE_RUNTIME(strText);
						// First apply transformations
						MasterFrames.ApplyAllMasters(pBitmap, std::addressof(lightframeInfo.m_vStars), m_pProgress);

						// Here save the calibrated light frame if needed
						m_strCurrentLightFrame = lightframeInfo.filePath.c_str();

						std::shared_ptr<CMemoryBitmap> pDelta = ApplyCosmetic(pBitmap, m_PostCalibrationSettings, m_pProgress);
						if (m_bSaveCalibrated)
							SaveCalibratedLightFrame(pBitmap);
						if (static_cast<bool>(pDelta))
							SaveDeltaImage(pDelta.get());

						if (m_pProgress != nullptr)
							m_pProgress->Start2(strText, 0);

						// Stack
						bStop = !StackLightFrame(pBitmap, PixTransform, lightframeInfo.m_fExposure, lightframeInfo.m_bComet);
						m_lNrStacked++;

						if (m_bCreateCometImage)
							m_vCometShifts.emplace_back(static_cast<int>(m_vCometShifts.size()), PixTransform.m_fXCometShift, PixTransform.m_fYCometShift);

						if (m_pProgress != nullptr)
						{
							m_pProgress->End2();
							bStop = bStop || m_pProgress->IsCanceled();
						}
					}
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
		CString errorMessage(CA2CT(e.what()));
#if defined(_CONSOLE)
		std::wcerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
	}
#if !defined(_CONSOLE)
	catch (CException & e)
	{
		e.ReportError();
		e.Delete();
	}
#endif
	catch (ZException & ze)
	{
		CString errorMessage;
		CString name(CA2CT(ze.name()));
		CString fileName(CA2CT(ze.locationAtIndex(0)->fileName()));
		CString functionName(CA2CT(ze.locationAtIndex(0)->functionName()));
		CString text(CA2CT(ze.text(0)));

		errorMessage.Format(
			_T("Exception %s thrown from %s Function: %s() Line: %lu\n\n%s"),
			name,
			fileName,
			functionName,
			ze.locationAtIndex(0)->lineNumber(),
			text);
#if defined(_CONSOLE)
		std::wcerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif
	}
	catch (...)
	{
		CString errorMessage(_T("Unknown exception caught"));
#if defined(_CONSOLE)
		std::wcerr << errorMessage;
#else
		AfxMessageBox(errorMessage, MB_OK | MB_ICONSTOP);
#endif

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
				CChannelAlign channelAlign;
				channelAlign.AlignChannels(pBitmap.get(), m_pProgress);
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

bool	CStackingEngine::GetDefaultOutputFileName(CString & strFileName, LPCTSTR szFileList, bool bTIFF)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult;
	// Retrieve the first light frame
	COutputSettings		OutputSettings;

	CAllStackingTasks::GetOutputSettings(OutputSettings);

	bResult = OutputSettings.m_bOutput;

	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szName[1+_MAX_FNAME];
	CString				strBaseName = strFileName;
	CString				strFileList = szFileList;
	CString				strOutputFolder;

	// By default use the folder of the first light frame
	if (m_vBitmaps.size())
	{
		// Use the folder of the first light frame
		_tsplitpath(m_vBitmaps[0].filePath.c_str(), szDrive, szDir, nullptr, nullptr);

		strOutputFolder = szDrive;
		strOutputFolder += szDir;
	};

	if (OutputSettings.m_bOtherFolder && OutputSettings.m_strFolder.length())
	{
		strOutputFolder = CString((LPCTSTR)OutputSettings.m_strFolder.utf16());
	};

	if (OutputSettings.m_bFileListFolder && strFileList.GetLength())
	{
		_tsplitpath(strFileList, szDrive, szDir, nullptr, nullptr);

		strOutputFolder = szDrive;
		strOutputFolder += szDir;
	};

	if (!strBaseName.GetLength())
	{
		CString			strFileList = szFileList;

		if (OutputSettings.m_bAutosave || !strFileList.GetLength())
			strBaseName = _T("Autosave");
		else
		{
			_tsplitpath(szFileList, nullptr, nullptr, szName, nullptr);
			strBaseName = szName;
			if (!strBaseName.GetLength())
				strBaseName = _T("Autosave");
		};
	};

	{
		CString			strBasePath;
		CString			strExt;
		bool			bFileExists = false;
		int			lNumber = 0;

		strBasePath = strOutputFolder;
		// Add trailing backslash
		if (strBasePath.Right(1) != _T("\\") && strBasePath.Right(1) != _T("/"))
			strBasePath += _T("\\");

		if (bTIFF)
		{
			strExt = ".tif";
			strFileName = strBasePath+strBaseName+".tif";
		}
		else
		{
			strExt = ".fit";
			if (m_vBitmaps.size())
				GetFITSExtension(m_vBitmaps[0].filePath, strExt);
			strFileName = strBasePath+strBaseName+strExt;
		};

		if (OutputSettings.m_bAppend)
		{
			do
			{
				FILE *		hFile;

				hFile = _tfopen(strFileName, _T("rb"));
				if (hFile)
				{
					fclose(hFile);
					lNumber++;
					bFileExists = true;
					strFileName.Format(_T("%s%s%03ld%s"), (LPCTSTR)strBasePath, (LPCTSTR)strBaseName, lNumber, (LPCTSTR)strExt);
				}
				else
					bFileExists = false;
			}
			while (bFileExists && (lNumber<1000));
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
static void GetISOGainStrings(CTaskInfo *pTask, CString const &strISO, CString const &strGain,
	CString const **ppstrISOGainText, CString *strISOGainValue)
{
	if (pTask->HasISOSpeed())
	{
		ISOToString(pTask->m_lISOSpeed, *strISOGainValue);
		*ppstrISOGainText = &strISO;
	}
	else
	{
		GainToString(pTask->m_lGain, *strISOGainValue);
		*ppstrISOGainText = &strGain;
	}
}

void	CStackingEngine::WriteDescription(CAllStackingTasks& tasks, LPCTSTR szOutputFile)
{
	ZFUNCTRACE_RUNTIME();

	COutputSettings		OutputSettings;
	CString				strOutputFile = szOutputFile;

	tasks.GetOutputSettings(OutputSettings);

	if (OutputSettings.m_bOutputHTML)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];

		_tsplitpath(szOutputFile, szDrive, szDir, szName, nullptr);
		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		strOutputFile += _T(".html");
		FILE *				hFile;

		hFile = _tfopen(strOutputFile, _T("wt"));
		if (hFile)
		{
			CString			strText;

			_tsplitpath(strOutputFile, nullptr, nullptr, szName, nullptr);

			fprintf(hFile, "<html>\n");

			fprintf(hFile, "<head>\n");
			fprintf(hFile, "<meta name=\"GENERATOR\" content=\"DeepSkyStacker\">");
			fprintf(hFile, "<title>DeepSkyStacker - %s</title>", (LPCSTR)CT2CA(szName, CP_UTF8));
			fprintf(hFile, "</head>\n");


			fprintf(hFile, "<body>\n");
			fprintf(hFile, "-> %s<br><br>\n", (LPCSTR)CT2CA(szName, CP_UTF8));

			// Stacking Mode
			strText.Format(IDS_RECAP_STACKINGMODE);
			fprintf(hFile, "%s", (LPCSTR)CT2CA(strText, CP_UTF8));
			switch (tasks.GetStackingMode())
			{
			case SM_NORMAL :
				strText.Format(IDS_RECAP_STACKINGMODE_NORMAL);
				break;
			case SM_MOSAIC :
				strText.Format(IDS_RECAP_STACKINGMODE_MOSAIC);
				break;
			case SM_INTERSECTION :
				strText.Format(IDS_RECAP_STACKINGMODE_INTERSECTION);
				break;
			case SM_CUSTOM :
				strText.Format(IDS_RECAP_STACKINGMODE_CUSTOM);
				break;
			};

			fprintf(hFile, "%s<br>", (LPCSTR)CT2CA(strText, CP_UTF8));

			// Alignment method
			strText.Format(IDS_RECAP_ALIGNMENT);
			fprintf(hFile, "%s", (LPCSTR)CT2CA(strText, CP_UTF8));

			switch (tasks.GetAlignmentMethod())
			{
			case 0 :
			case 1 :
				strText.Format(IDS_ALIGN_AUTO);
				break;
			case 2 :
				strText.Format(IDS_ALIGN_BILINEAR);
				break;
			case 3 :
				strText.Format(IDS_ALIGN_BISQUARED);
				break;
			case 4 :
				strText.Format(IDS_ALIGN_BICUBIC);
				break;
			case 5 :
				strText.Format(IDS_ALIGN_NONE);
				break;
			};
			fprintf(hFile, "%s<br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));

			// Drizzle ?
			const int dwDrizzle = tasks.GetPixelSizeMultiplier();
			if (dwDrizzle > 1)
			{
				strText.Format(IDS_RECAP_DRIZZLE, dwDrizzle);
				fprintf(hFile, "%s<br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
			};

			// Comet
			if (tasks.IsCometAvailable())
			{
				COMETSTACKINGMODE	CometStackingMode;

				CometStackingMode = tasks.GetCometStackingMode();
				strText.Format(IDS_RECAP_COMETSTACKING);
				fprintf(hFile, CT2CA(strText, CP_UTF8));
				switch (CometStackingMode)
				{
				case CSM_STANDARD :
					strText.Format(IDS_RECAP_COMETSTACKING_NONE);
					break;
				case CSM_COMETONLY :
					strText.Format(IDS_RECAP_COMETSTACKING_COMET);
					break;
				case CSM_COMETSTAR :
					strText.Format(IDS_RECAP_COMETSTACKING_BOTH);
					break;
				};
				fprintf(hFile, "%s<br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));;
			};

			// Post calibration settings
			CPostCalibrationSettings		pcs;

			tasks.GetPostCalibrationSettings(pcs);
			if (pcs.m_bHot)
			{
				strText.Format(IDS_RECAP_COSMETICHOT, pcs.m_lHotFilter, pcs.m_fHotDetection);
				fprintf(hFile, "%s<br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
			};
			if (pcs.m_bCold)
			{
				strText.Format(IDS_RECAP_COSMETICCOLD, pcs.m_lColdFilter, pcs.m_fColdDetection);
				fprintf(hFile, "%s<br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
			};

			if (pcs.m_bHot || pcs.m_bCold)
				fprintf(hFile, "<br><br>");

			// Now the list of tasks
			int				i, j;
			int				lTotalExposure = 0;
			CString				strBackgroundCalibration;
			CString				strPerChannelBackgroundCalibration;
			CString				strDarkOptimization;
			CString				strDarkFactor;
			CString				strExposure;
			CString				strISOGainValue;
			CString	const		*pstrISOGainText;
			CString				strISOText;
			CString				strGainText;
			CString				strHotPixels;
			CString				strYesNo;
			BACKGROUNDCALIBRATIONMODE	CalibrationMode;

			CalibrationMode = tasks.GetBackgroundCalibrationMode();

			strISOText.LoadString(IDS_ISO);
			strGainText.LoadString(IDS_GAIN);

			strYesNo.LoadString((CalibrationMode == BCM_RGB) ? IDS_YES : IDS_NO);
			strBackgroundCalibration.Format(IDS_RECAP_BACKGROUNDCALIBRATION, strYesNo);

			strYesNo.LoadString((CalibrationMode == BCM_PERCHANNEL) ? IDS_YES : IDS_NO);
			strPerChannelBackgroundCalibration.Format(IDS_RECAP_PERCHANNELBACKGROUNDCALIBRATION, strYesNo);

			for (i = 0;i<tasks.m_vStacks.size();i++)
			{
				CStackingInfo &			si = tasks.m_vStacks[i];

				if (si.m_pLightTask)
				{
					fprintf(hFile, "<table border='1px' cellspacing=0 cellpadding=5 width=100%%><tr><td>");
					int			lTaskExposure = 0;

					for (j = 0;j<si.m_pLightTask->m_vBitmaps.size();j++)
						lTaskExposure += si.m_pLightTask->m_vBitmaps[j].m_fExposure;

					lTotalExposure += lTaskExposure;

					ExposureToString(lTaskExposure, strExposure);
					GetISOGainStrings(si.m_pLightTask, strISOText, strGainText, &pstrISOGainText, &strISOGainValue);

					strText.Format(IDS_RECAP_STEP, i+1, si.m_pLightTask->m_vBitmaps.size(), *pstrISOGainText, strISOGainValue);
					fprintf(hFile, "<a href=\"#Task%ld\">%s</a>", i, (LPCSTR)CT2CA(strText, CP_UTF8));
					fprintf(hFile, CT2CA(strExposure, CP_UTF8));
					fprintf(hFile, "<br>");
					fprintf(hFile, "<ul>");
					fprintf(hFile, CT2CA(strBackgroundCalibration, CP_UTF8));
					fprintf(hFile, "<br>");
					fprintf(hFile, CT2CA(strPerChannelBackgroundCalibration, CP_UTF8));
					fprintf(hFile, "</ul>");
					if (si.m_pLightTask->m_vBitmaps.size()>1)
					{
						fprintf(hFile, "<ul>");
						strText.Format(IDS_RECAP_METHOD);
						fprintf(hFile, CT2CA(strText, CP_UTF8));
						FormatFromMethod(strText, si.m_pLightTask->m_Method, si.m_pLightTask->m_fKappa, si.m_pLightTask->m_lNrIterations);
						fprintf(hFile, CT2CA(strText, CP_UTF8));
						fprintf(hFile, "</ul>");

						if ((si.m_pLightTask->m_Method != MBP_AVERAGE) &&
							(IsRawBayer() || IsFITSRawBayer()))
						{
							fprintf(hFile, "<br>");
							strText.Format(IDS_RECAP_WARNINGBAYERDRIZZLE);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
						};
					};

					fprintf(hFile, "<hr>");
					if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
						fprintf(hFile, "<ul>");

					if (si.m_pOffsetTask)
					{
						ExposureToString(si.m_pOffsetTask->m_fExposure, strExposure);
						GetISOGainStrings(si.m_pOffsetTask, strISOText, strGainText, &pstrISOGainText, &strISOGainValue);

						strText.Format(IDS_RECAP_OFFSET, si.m_pOffsetTask->m_vBitmaps.size(), *pstrISOGainText, strISOGainValue, strExposure);
						fprintf(hFile, CT2CA(strText, CP_UTF8));

						if (si.m_pOffsetTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							FormatFromMethod(strText, si.m_pOffsetTask->m_Method, si.m_pOffsetTask->m_fKappa, si.m_pOffsetTask->m_lNrIterations);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						}
						else
							fprintf(hFile, "<br>");

						if (si.m_pOffsetTask->HasISOSpeed())
						{
							if (si.m_pOffsetTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
							{
								strText.Format(IDS_RECAP_ISOWARNING);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
							};
						}
						else
						{
							if (si.m_pOffsetTask->m_lGain != si.m_pLightTask->m_lGain)
							{
								strText.Format(IDS_RECAP_GAINWARNING);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
							};
						};
						fprintf(hFile, "</ul>");
					}
					else
					{
						strText.Format(IDS_RECAP_NOOFFSET);
						fprintf(hFile, CT2CA(strText, CP_UTF8));
					};
					if (si.m_pDarkTask)
					{
						ExposureToString(si.m_pDarkTask->m_fExposure, strExposure);
						GetISOGainStrings(si.m_pDarkTask, strISOText, strGainText, &pstrISOGainText, &strISOGainValue);

						strText.Format(IDS_RECAP_DARK, si.m_pDarkTask->m_vBitmaps.size(), *pstrISOGainText, strISOGainValue, strExposure);
						fprintf(hFile, CT2CA(strText, CP_UTF8));

						if (si.m_pDarkTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							FormatFromMethod(strText, si.m_pDarkTask->m_Method, si.m_pDarkTask->m_fKappa, si.m_pDarkTask->m_lNrIterations);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						};

						fprintf(hFile, "<ul>");
						fprintf(hFile, CT2CA(strDarkOptimization, CP_UTF8));
						fprintf(hFile, CT2CA(strHotPixels, CP_UTF8));
						if (strDarkFactor.GetLength())
						{
							fprintf(hFile, CT2CA(strDarkFactor, CP_UTF8));
							fprintf(hFile, "<br>");
						};

						if (si.m_pDarkTask->HasISOSpeed())
						{
							if (si.m_pDarkTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
							{
								strText.Format(IDS_RECAP_ISOWARNING);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
								fprintf(hFile, "<br>");
							};
						}
						else
						{
							if (si.m_pDarkTask->m_lGain != si.m_pLightTask->m_lGain)
							{
								strText.Format(IDS_RECAP_GAINWARNING);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
								fprintf(hFile, "<br>");
							};
						};
						if (!AreExposureEquals(si.m_pDarkTask->m_fExposure, si.m_pLightTask->m_fExposure))
						{
							strText.Format(IDS_RECAP_EXPOSUREWARNING);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							fprintf(hFile, "<br>");
						};
						fprintf(hFile, "</ul>");
					}
					else
					{
						strText.Format(IDS_RECAP_NODARK);
						fprintf(hFile, CT2CA(strText, CP_UTF8));
					};
					if (si.m_pDarkFlatTask && si.m_pFlatTask)
					{
						ExposureToString(si.m_pDarkFlatTask->m_fExposure, strExposure);
						GetISOGainStrings(si.m_pDarkFlatTask, strISOText, strGainText, &pstrISOGainText, &strISOGainValue);

						strText.Format(IDS_RECAP_DARKFLAT, si.m_pDarkFlatTask->m_vBitmaps.size(), *pstrISOGainText, strISOGainValue, strExposure);
						fprintf(hFile, CT2CA(strText, CP_UTF8));

						if (si.m_pDarkFlatTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, (LPCSTR)CT2CA(strText+"<br>", CP_UTF8));
							FormatFromMethod(strText, si.m_pDarkFlatTask->m_Method, si.m_pDarkFlatTask->m_fKappa, si.m_pDarkFlatTask->m_lNrIterations);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						}
						else
							fprintf(hFile, "<br>");

						if (si.m_pDarkFlatTask->HasISOSpeed())
						{
							if (si.m_pDarkFlatTask->m_lISOSpeed != si.m_pFlatTask->m_lISOSpeed)
							{
								strText.Format(IDS_RECAP_ISOWARNINGDARKFLAT);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
							};
						}
						else
						{
							if (si.m_pDarkFlatTask->m_lGain != si.m_pFlatTask->m_lGain)
							{
								strText.Format(IDS_RECAP_GAINWARNINGDARKFLAT);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
							};
						};
						if (!AreExposureEquals(si.m_pDarkFlatTask->m_fExposure, si.m_pFlatTask->m_fExposure))
						{
							strText.Format(IDS_RECAP_EXPOSUREWARNINGDARKFLAT);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
						};
						fprintf(hFile, "</ul>");
					};
					if (si.m_pFlatTask)
					{
						ExposureToString(si.m_pFlatTask->m_fExposure, strExposure);
						GetISOGainStrings(si.m_pFlatTask, strISOText, strGainText, &pstrISOGainText, &strISOGainValue);

						strText.Format(IDS_RECAP_FLAT, si.m_pFlatTask->m_vBitmaps.size(), *pstrISOGainText, strISOGainValue, strExposure);
						fprintf(hFile, CT2CA(strText, CP_UTF8));
						if (si.m_pFlatTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							FormatFromMethod(strText, si.m_pFlatTask->m_Method, si.m_pFlatTask->m_fKappa, si.m_pFlatTask->m_lNrIterations);
							fprintf(hFile, CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						};

						if (si.m_pFlatTask->HasISOSpeed())
						{
							if (si.m_pFlatTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
							{
								strText.Format(IDS_RECAP_ISOWARNING);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
							};
						}
						else
						{
							if (si.m_pFlatTask->m_lGain != si.m_pLightTask->m_lGain)
							{
								strText.Format(IDS_RECAP_GAINWARNING);
								fprintf(hFile, CT2CA(strText, CP_UTF8));
							};
						};
						fprintf(hFile, "</ul>");
					}
					else
					{
						strText.Format(IDS_RECAP_NOFLAT);
						fprintf(hFile, CT2CA(strText, CP_UTF8));
					};

					if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
						fprintf(hFile, "</ul>");
					fprintf(hFile, "</td></tr></table><br>");
				};
			};

			if (m_vBitmaps.size())
			{
				for (i = 0;i<tasks.m_vStacks.size();i++)
				{
					CStackingInfo &			si = tasks.m_vStacks[i];

					if (si.m_pLightTask)
					{
						fprintf(hFile, "<hr><br>\n");
						fprintf(hFile, "<a name=\"Task%ld\"></a>", i);
						strText.LoadString(IDS_TYPE_LIGHT);
						fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
						for (j = 0;j<si.m_pLightTask->m_vBitmaps.size();j++)
							fprintf(hFile, "%s<br>", reinterpret_cast<const char*>(si.m_pLightTask->m_vBitmaps[j].filePath.generic_u8string().c_str()));

						if (si.m_pOffsetTask && si.m_pOffsetTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_OFFSET);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pOffsetTask->m_strOutputFile != si.m_pOffsetTask->m_vBitmaps[0].filePath.c_str())
							{
								strText.LoadString(IDS_TYPE_MASTEROFFSET);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8), (LPCSTR)CT2CA(si.m_pOffsetTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pOffsetTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", reinterpret_cast<const char*>(si.m_pOffsetTask->m_vBitmaps[j].filePath.generic_u8string().c_str()));
						};

						if (si.m_pDarkTask && si.m_pDarkTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_DARK);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pDarkTask->m_strOutputFile != si.m_pDarkTask->m_vBitmaps[0].filePath.c_str())
							{
								strText.LoadString(IDS_TYPE_MASTERDARK);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8), (LPCSTR)CT2CA(si.m_pDarkTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pDarkTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", reinterpret_cast<const char*>(si.m_pDarkTask->m_vBitmaps[j].filePath.generic_u8string().c_str()));
						};

						if (si.m_pDarkFlatTask && si.m_pDarkFlatTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_DARKFLAT);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pDarkFlatTask->m_strOutputFile != si.m_pDarkFlatTask->m_vBitmaps[0].filePath.c_str())
							{
								strText.LoadString(IDS_TYPE_MASTERDARKFLAT);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8),
									(LPCSTR)CT2CA(si.m_pDarkFlatTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pDarkFlatTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", reinterpret_cast<const char*>(si.m_pDarkFlatTask->m_vBitmaps[j].filePath.generic_u8string().c_str()));
						};
						if (si.m_pFlatTask && si.m_pFlatTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_FLAT);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pFlatTask->m_strOutputFile != si.m_pFlatTask->m_vBitmaps[0].filePath.c_str())
							{
								strText.LoadString(IDS_TYPE_MASTERFLAT);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8),
									(LPCSTR)CT2CA(si.m_pFlatTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pFlatTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", reinterpret_cast<const char*>(si.m_pFlatTask->m_vBitmaps[j].filePath.generic_u8string().c_str()));
						};
					};
				};
			};

			fprintf(hFile, "<br><a href=\"http://deepskystacker.free.fr\">DeepSkyStacker %s</a>", VERSION_DEEPSKYSTACKER);
			fprintf(hFile, "</body>\n</html>\n");
			fclose(hFile);
		};
	};
};

/* ------------------------------------------------------------------- */
