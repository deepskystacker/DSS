#include <stdafx.h>
#include "StackingEngine.h"
#include "Registry.h"
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

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI			3.141592654
#endif

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::SetReferenceFrame(LPCTSTR szReferenceFrame)
{
	ZFUNCTRACE_RUNTIME();
	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szName[1+_MAX_FNAME];

	_tsplitpath(szReferenceFrame, szDrive, szDir, szName, NULL);

	m_strReferenceFrame = szReferenceFrame;
	m_strStackingFileInfo.Format(_T("%s%s%s.stackinfo.txt"), szDrive, szDir, szName);

	DWORD					dwAlignmentTransformation = 2;			
	CWorkspace				workspace;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), dwAlignmentTransformation);

	// Init from the file
	FILE *				hFile;

	m_vLightFrameStackingInfo.clear();
	hFile = _tfopen((LPCTSTR)m_strStackingFileInfo, _T("rt"));
	if (hFile)
	{
		BOOL			bEnd = FALSE;
		CHAR			szLine[10000];

		if (fgets(szLine, sizeof(szLine), hFile))
		{
			LONG		lSavedAlignmentTransformation;
			CString		strValue;
			
			strValue = CA2TEX<sizeof(szLine)>(szLine);
			strValue.TrimRight(_T("\n"));

			lSavedAlignmentTransformation = _ttol((LPCTSTR)strValue);

			if (lSavedAlignmentTransformation != dwAlignmentTransformation)
				bEnd = TRUE;
		}
		else
			bEnd = TRUE;

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
					bEnd = TRUE;
			}
			else
				bEnd = TRUE;
		};

		while (!bEnd)
		{
			CLightFrameStackingInfo		lfsi;
			BOOL						bResult = TRUE;

			if (fgets(szLine, sizeof(szLine), hFile))
			{
				lfsi.m_strInfoFileName = szLine;
				lfsi.m_strInfoFileName.TrimRight(_T("\n"));
			}
			else
				bEnd = TRUE;

			if (fgets(szLine, sizeof(szLine), hFile))
			{
				lfsi.m_strFileName = szLine;
				lfsi.m_strFileName.TrimRight(_T("\n"));
			}
			else
				bEnd = TRUE;

			if (fgets(szLine, sizeof(szLine), hFile))
			{
				CString					strParameters;

				strParameters = szLine;
				strParameters.TrimRight(_T("\n"));
				bResult = lfsi.m_BilinearParameters.FromText((LPCTSTR)strParameters);
			}
			else
				bEnd = TRUE;

			if (!bEnd && bResult)
				m_vLightFrameStackingInfo.push_back(lfsi);
		};

		fclose(hFile);
		std::sort(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end());
	};
};

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::GetInfoFileName(LPCTSTR szLightFrame, CString & strInfoFileName)
{
	ZFUNCTRACE_RUNTIME();

	TCHAR				szDrive[1+_MAX_DRIVE];
	TCHAR				szDir[1+_MAX_DIR];
	TCHAR				szName[1+_MAX_FNAME];

	_tsplitpath(szLightFrame, szDrive, szDir, szName, NULL);

	strInfoFileName.Empty();
	strInfoFileName.Format(_T("%s%s%s.Info.txt"), szDrive, szDir, szName);

	// Retrieve the file date/time
	FILETIME		FileTime;
	SYSTEMTIME		SystemTime;
	TCHAR			szTime[200];
	TCHAR			szDate[200];
	CString			strDateTime;

	if (GetFileCreationDateTime((LPCTSTR)strInfoFileName, FileTime))
	{
		FileTimeToSystemTime(&FileTime, &SystemTime);
		SystemTimeToTzSpecificLocalTime(NULL, &SystemTime, &SystemTime);

		GetDateFormat(LOCALE_USER_DEFAULT, 0, &SystemTime, NULL, szDate, sizeof(szDate));
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, &SystemTime, NULL, szTime, sizeof(szTime));

		strInfoFileName.Format(_T("%s%s%s.Info.txt [%s %s]"), szDrive, szDir, szName, szDate, szTime);
	}
	else
		strInfoFileName.Empty();
};

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::AddLightFrame(LPCTSTR szLightFrame, const CBilinearParameters & bp)
{
	ZFUNCTRACE_RUNTIME();

	CLightFrameStackingInfo			lfsi(szLightFrame);
	LIGHTFRAMESTACKINGINFOITERATOR	it;
	CString							strInfoFileName;

	GetInfoFileName(szLightFrame, strInfoFileName);
	it = std::lower_bound(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end(), lfsi);
	if (it != m_vLightFrameStackingInfo.end() && 
		!(*it).m_strFileName.CompareNoCase(szLightFrame))
	{
		// There is already this light frame
		(*it).m_strInfoFileName		= strInfoFileName;
		(*it).m_BilinearParameters	= bp;
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

BOOL	CLightFramesStackingInfo::GetParameters(LPCTSTR szLightFrame, CBilinearParameters & bp)
{
	ZFUNCTRACE_RUNTIME();
	BOOL							bResult = FALSE;
	LIGHTFRAMESTACKINGINFOITERATOR	it;

	it = std::lower_bound(m_vLightFrameStackingInfo.begin(), m_vLightFrameStackingInfo.end(), CLightFrameStackingInfo(szLightFrame));
	if (it != m_vLightFrameStackingInfo.end() && 
		!(*it).m_strFileName.CompareNoCase(szLightFrame))
	{
		CString						strInfoFileName;

		GetInfoFileName(szLightFrame, strInfoFileName);

		if (!strInfoFileName.CompareNoCase((*it).m_strInfoFileName))
		{
			bp = (*it).m_BilinearParameters;
			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CLightFramesStackingInfo::Save()
{
	ZFUNCTRACE_RUNTIME();

	if (m_strReferenceFrame.GetLength() && m_strStackingFileInfo.GetLength())
	{
		FILE *				hFile;

		hFile = _tfopen((LPCTSTR)m_strStackingFileInfo, _T("wt"));
		if (hFile)
		{
			// Save the alignment transformation used
			DWORD					dwAlignmentTransformation = 2;			
			CWorkspace				workspace;

			workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), dwAlignmentTransformation);
			fprintf(hFile,"%ld\n", dwAlignmentTransformation);

			CString					strInfoFileName;

			GetInfoFileName((LPCTSTR)m_strReferenceFrame, strInfoFileName);
			fprintf(hFile, "%s\n", (LPCSTR)(CT2CA(strInfoFileName, CP_UTF8)));

			for (LONG i = 0;i<m_vLightFrameStackingInfo.size();i++)
			{
				fprintf(hFile, "%s\n", (LPCSTR)(CT2CA(m_vLightFrameStackingInfo[i].m_strInfoFileName, CP_UTF8)));
				fprintf(hFile, "%s\n", (LPCSTR)(CT2CA(m_vLightFrameStackingInfo[i].m_strFileName, CP_UTF8)));

				CString			strParameters;

				m_vLightFrameStackingInfo[i].m_BilinearParameters.ToText(strParameters);
				fprintf(hFile, "%s\n", (LPCSTR)CT2CA(strParameters, CP_UTF8));
			};

			fclose(hFile);
		};
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	RemoveStars(CMemoryBitmap * pBitmap,CPixelTransform & PixTransform, const STARVECTOR & vStars)
{
	ZFUNCTRACE_RUNTIME();

	if (pBitmap)
	{
		double				fWidth	= pBitmap->Width();
		double				fHeight = pBitmap->Height();
		BOOL				bMonochrome = pBitmap->IsMonochrome();

		for (LONG k = 0;k<vStars.size();k++)
		{
			double			fRadius = vStars[k].m_fMeanRadius*2.35*1.0;// /1.5;
			CPointExt		ptCenter(vStars[k].m_fX, vStars[k].m_fY);
			//double			fIntensity = 0;
			//LONG			lNrIntensities = 0;

			ptCenter = PixTransform.Transform(ptCenter);

			for (double i = max(0, ptCenter.X - 2.0*fRadius);i<=min(ptCenter.X + 2.0*fRadius, fWidth-1);i++)
			{
				for (double j = max(0, ptCenter.Y - 2.0*fRadius);j<=min(ptCenter.Y + 2.0*fRadius, fHeight-1);j++)
				{
					// Compute the distance to the center
					double		fDistance;
					double		fXDistance = fabs(i-ptCenter.X);
					double		fYDistance = fabs(j-ptCenter.Y);

					fDistance = sqrt(fXDistance * fXDistance + fYDistance * fYDistance);
					
					if (fDistance<=fRadius)
					{
						/*
						if (fDistance<=fRadius/2)
						{
							double			fGray;

							pBitmap->GetPixel(i+0.5, j+0.5, fGray);
							fIntensity += fGray;
							lNrIntensities ++;
						};*/
						pBitmap->SetPixel(i+0.5, j+0.5, 0.0);
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
		for (LONG k = 0;k<vStars.size();k++)
		{
			double			fRadius = vStars[k].m_fMeanRadius*2.35;// /1.5;
			CPointExt		ptCenter(vStars[k].m_fX, vStars[k].m_fY);
			double			fIntensity = 0;
			LONG			lNrIntensities = 0;

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

	TRANSFORMATIONTYPE		TTResult = TT_BILINEAR;
	DWORD					dwAlignmentTransformation = 2;			
	CWorkspace				workspace;

	workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), dwAlignmentTransformation);

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

BOOL	CStackingEngine::AddLightFramesToList(CAllStackingTasks & tasks)
{
	ZFUNCTRACE_RUNTIME();

	LONG				i, j;
	BOOL				bReferenceFrameFound;

	if (m_strReferenceFrame.GetLength())
		bReferenceFrameFound = FALSE;
	else
		bReferenceFrameFound = TRUE;

	m_vBitmaps.clear();
	for (i = 0;i<tasks.m_vTasks.size();i++)
	{
		if (tasks.m_vTasks[i].m_TaskType == PICTURETYPE_LIGHTFRAME)
		{
			for (j = 0;j<tasks.m_vTasks[i].m_vBitmaps.size();j++)
			{
				CLightFrameInfo			lfi;

				lfi.SetBitmap(tasks.m_vTasks[i].m_vBitmaps[j].m_strFileName, FALSE, FALSE);

				if (lfi.IsRegistered())
				{
					lfi = tasks.m_vTasks[i].m_vBitmaps[j];
					lfi.RefreshSuperPixel();

					if (!m_strReferenceFrame.CompareNoCase(lfi.m_strFileName))
					{
						lfi.m_bStartingFrame = TRUE;
						bReferenceFrameFound = TRUE;
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

		if (fi.InitFromFile(m_strReferenceFrame, PICTURETYPE_LIGHTFRAME))
		{
			lfi.SetBitmap(m_strReferenceFrame, FALSE, FALSE);
			if (lfi.IsRegistered())
			{
				lfi = fi;
				lfi.m_bStartingFrame = TRUE;
				lfi.m_bDisabled		 = TRUE;
				m_vBitmaps.push_back(lfi);
			};
		};
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL CStackingEngine::ComputeLightFrameOffset(LONG lBitmapIndice, CMatchingStars & MatchingStars)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;
	CBilinearParameters	BilinearParameters;

	m_CriticalSection.Lock();
	bResult = m_StackingInfo.GetParameters(m_vBitmaps[lBitmapIndice].m_strFileName, BilinearParameters);
	m_CriticalSection.Unlock();

	if (bResult)
	{
		BilinearParameters.Offsets(m_vBitmaps[lBitmapIndice].m_fXOffset, m_vBitmaps[lBitmapIndice].m_fYOffset);
		m_vBitmaps[lBitmapIndice].m_fAngle   = BilinearParameters.Angle(m_vBitmaps[lBitmapIndice].RenderedWidth());
		m_vBitmaps[lBitmapIndice].m_BilinearParameters = BilinearParameters;
	}
	else if (GetTransformationType() == TT_NONE)
	{
		// Automatic acknowledgment of the transformation
		m_CriticalSection.Lock();
		m_StackingInfo.AddLightFrame(m_vBitmaps[lBitmapIndice].m_strFileName, BilinearParameters);
		m_CriticalSection.Unlock();

		bResult = TRUE;
	}
	else if ((m_vBitmaps[lBitmapIndice].m_vStars.size() > 4) &&
		((m_vBitmaps[lBitmapIndice].m_vStars.size() > m_vBitmaps[0].m_vStars.size()/5) ||
		 (m_vBitmaps[lBitmapIndice].m_vStars.size()>=30)))
	{
		// Try to identify patterns in the placement of stars

		STARVECTOR &		vStarsOrg = m_vBitmaps[0].m_vStars;
		STARVECTOR &		vStarsDst = m_vBitmaps[lBitmapIndice].m_vStars;
		LONG				i;
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
			for (i = 0;i<min(vStarsOrg.size(), 100);i++)
				MatchingStars.AddReferenceStar(vStarsOrg[i].m_fX*fXRatio, vStarsOrg[i].m_fY*fYRatio);
		};
		MatchingStars.ClearTarget();
		for (i = 0;i<min(vStarsDst.size(), 100);i++)
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
			m_StackingInfo.AddLightFrame(m_vBitmaps[lBitmapIndice].m_strFileName, BilinearParameters);
			m_CriticalSection.Unlock();
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CComputeOffsetTask : public CMultitask
{
private :
	CStackingEngine *	m_pStackingEngine;
	LONG				m_lLast;

public :
	CComputeOffsetTask()
	{
	};

	virtual ~CComputeOffsetTask()
	{
	};

	void	Init(LONG lLast, CStackingEngine * pStackingEngine)
	{
		m_lLast			  = lLast;
		m_pStackingEngine = pStackingEngine;
	};

	virtual BOOL	Process();
	virtual BOOL	DoTask(HANDLE hEvent);
};

/* ------------------------------------------------------------------- */

BOOL	CComputeOffsetTask::Process()
{
	ZFUNCTRACE_RUNTIME();

	BOOL			bStop = FALSE;
	CString			strText;

	if (m_pStackingEngine->m_pProgress)
		m_pStackingEngine->m_pProgress->SetNrUsedProcessors(GetNrThreads());

	for (LONG i = 1;i<m_lLast && !bStop;i++)
	{
		if (m_pStackingEngine->m_pProgress)
		{
			strText.Format(IDS_COMPUTINGSTACKINGINFO, (LPCTSTR)m_pStackingEngine->m_vBitmaps[i].m_strFileName);
			m_pStackingEngine->m_pProgress->Progress1(strText, i+1);
			bStop = m_pStackingEngine->m_pProgress->IsCanceled();
		};

		DWORD			dwThreadId;
		
		dwThreadId = GetAvailableThreadId();
		PostThreadMessage(dwThreadId, WM_MT_PROCESS, 0, i);
	};

	CloseAllThreads();

	if (m_pStackingEngine->m_pProgress)
		m_pStackingEngine->m_pProgress->SetNrUsedProcessors();

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL	CComputeOffsetTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();

	BOOL			bResult = TRUE;
	BOOL			bEnd = FALSE;
	MSG				msg;

	{
		CMatchingStars  MatchingStars;

		// Create a message queue and signal the event
		PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
		SetEvent(hEvent);
		while (!bEnd && GetMessage(&msg, NULL, 0, 0))
		{
			if (msg.message == WM_MT_PROCESS)
			{
				if (m_pStackingEngine->ComputeLightFrameOffset(msg.lParam, MatchingStars))
				{
					m_pStackingEngine->m_vBitmaps[msg.lParam].m_bDisabled = FALSE; 
					m_CriticalSection.Lock();
					m_pStackingEngine->m_lNrStackable++;
					if (m_pStackingEngine->m_vBitmaps[msg.lParam].m_bComet)
						m_pStackingEngine->m_lNrCometStackable++;
					m_CriticalSection.Unlock();
				}
				else
					m_pStackingEngine->m_vBitmaps[msg.lParam].m_bDisabled = TRUE;

				SetEvent(hEvent);
			}
			else if (msg.message == WM_MT_STOP)
				bEnd = TRUE;
		};
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

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

BOOL	CStackingEngine::ComputeMissingCometPositions()
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = TRUE;

	if (m_lNrCometStackable>=2)
	{
		// Add all the valid light frames to a vector
		std::vector<CLightFrameInfo *>	vpLightFrames;

		for (LONG i = 0;i<m_vBitmaps.size();i++)
		{
			if (!m_vBitmaps[i].m_bDisabled)
				vpLightFrames.push_back(&(m_vBitmaps[i]));
		};

		// Now sort the list by ascending date/time
		std::sort(vpLightFrames.begin(), vpLightFrames.end(), CompareLightFrameDate);

		std::vector<LONG>				vNewComet;

		for (LONG i = 1;i<vpLightFrames.size()-1;i++)
		{
			//if (!vpLightFrames[i]->m_bComet)
			{
				CLightFrameInfo *				pPreviousComet = NULL;
				CLightFrameInfo *				pNextComet = NULL;
				LONG							lPreviousIndex,
												lNextIndex;

				for (LONG j = i-1;j>=0 && !pPreviousComet;j--)
				{
					if (vpLightFrames[j]->m_bComet)
					{
						pPreviousComet = vpLightFrames[j];
						lPreviousIndex = j;
					};
				};

				for (LONG j = i+1;j<vpLightFrames.size() && !pNextComet;j++)
				{
					if (vpLightFrames[j]->m_bComet)
					{
						pNextComet = vpLightFrames[j];
						lNextIndex = j;
					};
				};

				if (pPreviousComet && pNextComet)
				{
					// Try to find another previous and/or next computed comet position
					// so that the elapsed time between the two is less than 12 hours
					BOOL				bContinue = FALSE;
					do
					{
						BOOL			bFound = FALSE;
						double			fElapsed;

						bContinue = FALSE;
						for (LONG j = lPreviousIndex-1;j>=0 && !bFound;j--)
						{
							if (vpLightFrames[j]->m_bComet)
							{
								fElapsed = ElapsedTime(vpLightFrames[j]->m_DateTime, pNextComet->m_DateTime);
								if (fElapsed/3600 < 12)
								{
									bFound = TRUE;
									bContinue = TRUE;
									pPreviousComet = vpLightFrames[j];
									lPreviousIndex = j;
								};
							};
						};
						bFound = FALSE;
						for (LONG j = lNextIndex+1;j<vpLightFrames.size() && !bFound;j++)
						{
							if (vpLightFrames[j]->m_bComet)
							{
								fElapsed = ElapsedTime(pPreviousComet->m_DateTime, vpLightFrames[j]->m_DateTime);
								if (fElapsed/3600 < 12)
								{
									bFound     = TRUE;
									bContinue  = TRUE;
									pNextComet = vpLightFrames[j];
									lNextIndex = j;
								};
							};
						};
					}
					while (bContinue);

					// Compute the comet position in the two frames
					CPointExt			ptPreviousComet = CPointExt(pPreviousComet->m_fXComet, pPreviousComet->m_fYComet);
					CPointExt			ptNextComet     = CPointExt(pNextComet->m_fXComet, pNextComet->m_fYComet);

					ptPreviousComet = pPreviousComet->m_BilinearParameters.Transform(ptPreviousComet);
					ptNextComet		= pNextComet->m_BilinearParameters.Transform(ptNextComet);

					double				fElapsed2,
										fElapsedCurrent;

					fElapsed2 = ElapsedTime(pPreviousComet->m_DateTime, pNextComet->m_DateTime);
					fElapsedCurrent = ElapsedTime(pPreviousComet->m_DateTime, vpLightFrames[i]->m_DateTime);

					if (fElapsed2)
					{
						CPointExt			ptCurrentComet;
						double				fAdvance = fElapsedCurrent/fElapsed2;

						ptCurrentComet.X = ptPreviousComet.X + fAdvance * (ptNextComet.X - ptPreviousComet.X);
						ptCurrentComet.Y = ptPreviousComet.Y + fAdvance * (ptNextComet.Y - ptPreviousComet.Y);

						// Set the comet position - already shifted
						vNewComet.push_back(i);
						vpLightFrames[i]->m_bTransformedCometPosition = TRUE;
						vpLightFrames[i]->m_fXComet = ptCurrentComet.X;
						vpLightFrames[i]->m_fYComet = ptCurrentComet.Y;

						if (!vpLightFrames[i]->m_bComet)
							m_lNrCometStackable++;
					};

					/*CPointExt			ptTestComet = CPointExt(vpLightFrames[i]->m_fXComet, vpLightFrames[i]->m_fYComet);
					vpLightFrames[i]->m_BilinearParameters.Transform(ptTestComet);*/
				};
			};
		};
		for (LONG i = 0;i<vNewComet.size();i++)
			vpLightFrames[vNewComet[i]]->m_bComet = TRUE;
	};

	return bResult;
};


/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::ComputeOffsets()
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;
	LONG				i;
	LONG				lTotalStacked = 1;
	LONG				lNrStacked = 0;
	CString				strText;
	BOOL				bStop = FALSE;
	LONG				lLast;

	std::sort(m_vBitmaps.begin(), m_vBitmaps.end());

	if (m_vBitmaps.size())
	{
		if (m_vBitmaps[0].m_bDisabled)
			m_lNrStackable = 0;
		else
			m_lNrStackable = min((LONG)m_vBitmaps.size(), 1);
		m_lNrCometStackable = 0;
		strText.LoadString(IDS_COMPUTINGOFFSETS);

		lLast = m_vBitmaps.size()*m_fKeptPercentage/100.0;
		if (m_pProgress)
			m_pProgress->Start(strText, lLast, FALSE);

		// The first bitmap is the best one
		if (m_vBitmaps.size() > 1)
		{
			std::sort(m_vBitmaps[0].m_vStars.begin(), m_vBitmaps[0].m_vStars.end());

			for (i = 1;i<m_vBitmaps.size();i++)
				m_vBitmaps[i].m_bDisabled = TRUE;

			if (m_vBitmaps[0].m_bComet)
				m_lNrCometStackable++;

			m_StackingInfo.SetReferenceFrame(m_vBitmaps[0].m_strFileName);
			CComputeOffsetTask		ComputeOffsetTask;

			ComputeOffsetTask.Init(lLast, this);
			ComputeOffsetTask.StartThreads();
			ComputeOffsetTask.Process();

			ComputeMissingCometPositions();
			m_StackingInfo.Save();
		};

		m_bOffsetComputed = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::IsLightFrameStackable(LPCTSTR szFile)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	for (LONG i = 0;i<m_vBitmaps.size() && !bResult;i++)
	{
		if (!m_vBitmaps[i].m_strFileName.CompareNoCase(szFile))
		{
			if (!m_vBitmaps[i].m_bDisabled)
				bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::RemoveNonStackableLightFrames(CAllStackingTasks & tasks)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;
	LONG				i, j;

	for (i = 0;i<tasks.m_vTasks.size();i++)
	{
		if (tasks.m_vTasks[i].m_TaskType == PICTURETYPE_LIGHTFRAME)
		{
			CTaskInfo &			LightTask = tasks.m_vTasks[i];
			FRAMEINFOVECTOR		vNewList;

			for (j = 0;j<LightTask.m_vBitmaps.size();j++)
			{
				if (IsLightFrameStackable(LightTask.m_vBitmaps[j].m_strFileName))
					vNewList.push_back(LightTask.m_vBitmaps[j]);
			};

			// Remove non stackable light frames from the list
			if (vNewList.size() < LightTask.m_vBitmaps.size())
				LightTask.m_vBitmaps = vNewList;

			// If the list is empty - consider that the task is done
			if (!vNewList.size())
				LightTask.m_bDone = TRUE;
		};
	};
	
	return bResult;
};

/* ------------------------------------------------------------------- */

void CStackingEngine::GetResultISOSpeed()
{
	ZFUNCTRACE_RUNTIME();

	m_lISOSpeed = m_vBitmaps[0].m_lISOSpeed;
	for (LONG i = 1;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			if (m_lISOSpeed != m_vBitmaps[i].m_lISOSpeed)
				m_lISOSpeed = 0;
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
	for (LONG i = 0;i<m_vBitmaps[0].m_ExtraInfo.m_vExtras.size();i++)
	{
		if (m_vBitmaps[0].m_ExtraInfo.m_vExtras[i].m_bPropagate)
			m_ExtraInfo.AddInfo(m_vBitmaps[0].m_ExtraInfo.m_vExtras[i]);
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

inline void ExpandWithPoint(LONG & lLeft, LONG & lRight, LONG & lTop, LONG & lBottom, const CPointExt & pt)
{
	lLeft	= min(lLeft, pt.X);
	lRight	= max(lRight, pt.X);
	lTop	= min(lTop, pt.Y);
	lBottom = max(lBottom, pt.Y);
};

void CStackingEngine::ComputeLargestRectangle(CRect & rc)
{
	ZFUNCTRACE_RUNTIME();

	LONG				i;
	BOOL				bFirst = TRUE;
	LONG				lLeft = 0, 
						lRight = 0, 
						lTop = 0, 
						lBottom = 0;

	for (i = 0;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			CPointExt			pt1(0, 0), 
								pt2(0, m_vBitmaps[i].RenderedHeight()),
								pt3(m_vBitmaps[i].RenderedWidth(), 0),
								pt4(m_vBitmaps[i].RenderedWidth(), m_vBitmaps[i].RenderedHeight()),
								pt5(0, m_vBitmaps[i].RenderedHeight()/2),
								pt6(m_vBitmaps[i].RenderedWidth(), m_vBitmaps[i].RenderedHeight()/2),
								pt7(m_vBitmaps[i].RenderedWidth()/2, 0),
								pt8(m_vBitmaps[i].RenderedWidth()/2, m_vBitmaps[i].RenderedHeight());

			CPixelTransform		PixTransform(m_vBitmaps[i].m_BilinearParameters);

			PixTransform.SetPixelSizeMultiplier(m_lPixelSizeMultiplier);

			pt1 = PixTransform.Transform(pt1);
			pt2 = PixTransform.Transform(pt2);
			pt3 = PixTransform.Transform(pt3);
			pt4 = PixTransform.Transform(pt4);
			pt5 = PixTransform.Transform(pt5);
			pt6 = PixTransform.Transform(pt6);
			pt7 = PixTransform.Transform(pt7);
			pt8 = PixTransform.Transform(pt8);

			if (bFirst)
			{
				lLeft = lRight = pt1.X;
				lTop  = lBottom = pt1.Y;
				bFirst = FALSE;
			}
			else
				ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt1);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt2);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt3);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt4);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt5);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt6);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt7);
			ExpandWithPoint(lLeft, lRight, lTop, lBottom, pt8);
		};
	};

	rc.left		= lLeft;
	rc.right	= lRight;
	rc.top		= lTop;
	rc.bottom	= lBottom;
};

/* ------------------------------------------------------------------- */

bool CStackingEngine::ComputeSmallestRectangle(CRect & rc)
{
	ZFUNCTRACE_RUNTIME();

	bool				bResult = false;
	LONG				i;
	BOOL				bFirst = TRUE;
	LONG				lLeft = 0, 
						lRight = 0, 
						lTop = 0, 
						lBottom = 0;

	for (i = 0;i<m_vBitmaps.size();i++)
	{
		if (!m_vBitmaps[i].m_bDisabled)
		{
			CPointExt			pt1(0, 0), 
								pt2(0, m_vBitmaps[i].RenderedHeight()),
								pt3(m_vBitmaps[i].RenderedWidth(), 0),
								pt4(m_vBitmaps[i].RenderedWidth(), m_vBitmaps[i].RenderedHeight());

			CPixelTransform		PixTransform(m_vBitmaps[i].m_BilinearParameters);

			PixTransform.SetPixelSizeMultiplier(m_lPixelSizeMultiplier);

			pt1 = PixTransform.Transform(pt1);
			pt2 = PixTransform.Transform(pt2);
			pt3 = PixTransform.Transform(pt3);
			pt4 = PixTransform.Transform(pt4);

			if (bFirst)
			{
				lLeft = pt1.X;		lRight = pt4.X;
				lTop  = pt1.Y;		lBottom = pt4.Y;
				bFirst = FALSE;
			}
			else
			{
				lLeft = max(max(lLeft, pt1.X), pt2.X);
				lRight = min(min(lRight, pt4.X), pt3.X);
				lTop = max(max(lTop, pt1.Y), pt3.Y);
				lBottom = min(min(lBottom, pt4.Y), pt2.Y);
			};
		};
	};

	if ((lLeft+50 < lRight) && (lTop+50 < lBottom))
	{
		rc.left		= lLeft+2;
		rc.right	= lRight-2;
		rc.top		= lTop+2;
		rc.bottom	= lBottom-2;
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
LONG CStackingEngine::FindBitmapIndice(LPCTSTR szFile)
{
	ZFUNCTRACE_RUNTIME();

	LONG				lResult = -1;
	LONG				i;

	for (i = 0;i<m_vBitmaps.size() && (lResult == -1);i++)
	{
		if (!m_vBitmaps[i].m_strFileName.CompareNoCase(szFile))
			lResult = i;
	};

	return lResult;
};

/* ------------------------------------------------------------------- */

BOOL CStackingEngine::ComputeBitmap()
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = TRUE;

	if (m_pMasterLight &&  m_pMasterLight->GetNrAddedBitmaps())
	{
		CSmartPtr<CMemoryBitmap>	pBitmap;

		if (m_pProgress)
		{
			CString						strText;
			CString						strMethod;

			FormatFromMethod(strMethod, m_pLightTask->m_Method, m_pLightTask->m_fKappa, m_pLightTask->m_lNrIterations);

			strText.Format(IDS_COMPUTINGMEDIANLIGHT, (LPCTSTR)strMethod);

			m_pProgress->Start(strText, 1, FALSE);
			m_pProgress->Progress1(strText, 0);
			m_pProgress->SetJointProgress(TRUE);
		};

		ZTRACE_RUNTIME("Compute resulting bitmap");
		if (m_vCometShifts.size())
		{
			std::vector<LONG>		vImageOrder;

			std::sort(m_vCometShifts.begin(), m_vCometShifts.end());
			for (LONG i = 0;i<m_vCometShifts.size();i++)
				vImageOrder.push_back(m_vCometShifts[i].m_lImageIndex);
			m_pMasterLight->SetImageOrder(vImageOrder);

			double					fX1 = m_vCometShifts[0].m_fXShift, 
									fY1 = m_vCometShifts[0].m_fYShift,
									fX2 = m_vCometShifts[m_vCometShifts.size()-1].m_fXShift, 
									fY2 = m_vCometShifts[m_vCometShifts.size()-1].m_fYShift;
			m_fStarTrailsAngle = atan2(fY2-fY1, fX2-fX1);
		};
		bResult = m_pMasterLight->GetResult(&pBitmap, m_pProgress);

		if (m_pProgress)
			m_pProgress->SetJointProgress(FALSE);

		m_pOutput = pBitmap;
		m_pMasterLight.Release();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::AdjustEntropyCoverage()
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_pEntropyCoverage)
	{
		ZTRACE_RUNTIME("Adjust Entropy Coverage");

		LONG		i, j;
		BOOL		bColor;

		bColor = !m_pEntropyCoverage->IsMonochrome();

		for (j = 0;j<m_pEntropyCoverage->Height();j++)
		{
			for (i = 0;i<m_pEntropyCoverage->Width();i++)
			{
				if (bColor)
				{
					double		fRed,
								fGreen,
								fBlue;
					double		fEntropyRed,
								fEntropyGreen,
								fEntropyBlue;

					m_pEntropyCoverage->GetValue(i, j, fEntropyRed, fEntropyGreen, fEntropyBlue);
					m_pOutput->GetValue(i, j, fRed, fGreen, fBlue);
					if (fEntropyRed)
						fRed /= fEntropyRed;
					if (fEntropyGreen)
						fGreen /= fEntropyGreen;
					if (fEntropyBlue)
						fBlue /= fEntropyBlue;
					m_pOutput->SetValue(i, j, fRed, fGreen, fBlue);
				}
				else
				{
					double		fGray;
					double		fEntropyGray;

					m_pEntropyCoverage->GetValue(i, j, fEntropyGray);
					m_pOutput->GetValue(i, j, fGray);
					if (fEntropyGray)
						fGray /= fEntropyGray;
					m_pOutput->SetValue(i, j, fGray);
				};
			};
		};
		m_pEntropyCoverage.Release();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::AdjustBayerDrizzleCoverage()
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	if (m_vPixelTransforms.size())
	{

		ZTRACE_RUNTIME("Adjust Bayer Drizzle Coverage");

		LONG		i, j, k, 
					lNrBitmaps;
		double		fMaxCoverage = 0;
		LONG		lProgress = 0;
		CString		strText;
		CSmartPtr<C96BitFloatColorBitmap>	pCover;

		pCover.Attach(new C96BitFloatColorBitmap());
		pCover->Init(m_rcResult.Width(), m_rcResult.Height());

		strText.LoadString(IDS_STACKING_COMPUTINGADJUSTMENT);
		if (m_pProgress)
			m_pProgress->Start(strText, (LONG)m_vPixelTransforms.size(), FALSE);

		for (lNrBitmaps = 0;lNrBitmaps<m_vPixelTransforms.size();lNrBitmaps++)
		{
			CPixelTransform	&	PixTransform = m_vPixelTransforms[lNrBitmaps];
			
			lProgress = 0;

			if (m_pProgress)
			{

				strText.Format(IDS_COMPUTINGADJUSTMENT, lNrBitmaps+1, m_vPixelTransforms.size());
				m_pProgress->Progress1(strText, lNrBitmaps+1);
				m_pProgress->Start2(_T(" "), m_rcResult.Width() * m_rcResult.Height());
			};

			for (j = 0;j<m_rcResult.Height();j++)
			{
				for (i = 0;i<m_rcResult.Width();i++)
				{
					CPointExt	pt(i, j);
					CPointExt	ptOut;

					lProgress++;

					ptOut = PixTransform.Transform(pt);

					if (ptOut.IsInRect(0, 0, m_rcResult.Width(), m_rcResult.Height()))
					{
						PIXELDISPATCHVECTOR		vPixels;

						ComputePixelDispatch(ptOut, vPixels);

						for (k = 0;k<vPixels.size();k++)
						{
							// For each plane adjust the values
							if (vPixels[k].m_lX >= 0 && vPixels[k].m_lX < m_rcResult.Width() && 
								vPixels[k].m_lY >= 0 && vPixels[k].m_lY < m_rcResult.Height())
							{
								double			fRedCover, 
												fGreenCover, 
												fBlueCover;

								pCover->GetValue(vPixels[k].m_lX, vPixels[k].m_lY, fRedCover, fGreenCover, fBlueCover);

								switch (GetBayerColor(i, j, m_InputCFAType))
								{
								case BAYER_RED :
									fRedCover += vPixels[k].m_fPercentage;
									break;
								case BAYER_GREEN :
									fGreenCover += vPixels[k].m_fPercentage;
									break;
								case BAYER_BLUE :
									fBlueCover += vPixels[k].m_fPercentage;
									break;
								};

								pCover->SetValue(vPixels[k].m_lX, vPixels[k].m_lY, fRedCover, fGreenCover, fBlueCover);
							};
						};
					};
				};
				if (m_pProgress)
					m_pProgress->Progress2(NULL, lProgress);
			};

			if (m_pProgress)
				m_pProgress->End2();
		};


		m_vPixelTransforms.clear();

		lProgress = 0;
		if (m_pProgress)
		{
			strText.LoadString(IDS_STACKING_APPLYINGADJUSTMENT);
			m_pProgress->Start(strText, 2, FALSE);
			strText.LoadString(IDS_STACKING_COMPUTEMAXADJUSTMENT);
			m_pProgress->Start2(strText, m_rcResult.Width() * m_rcResult.Height());
		};

		// Compute the maximum coverage
		for (j = 0;j<m_rcResult.Height();j++)
		{
			for (i = 0;i<m_rcResult.Width();i++)
			{
				double			fRedCover,
								fGreenCover,
								fBlueCover;

				lProgress++;

				pCover->GetValue(i, j, fRedCover, fGreenCover, fBlueCover);

				fMaxCoverage = max(fMaxCoverage, fRedCover);
				fMaxCoverage = max(fMaxCoverage, fGreenCover);
				fMaxCoverage = max(fMaxCoverage, fBlueCover);
			};

			if (m_pProgress)
				m_pProgress->Progress2(NULL, lProgress);
		};

		if (m_pProgress)
		{
			m_pProgress->End2();
			m_pProgress->Progress1(NULL, 1);
		};

		lProgress = 0;
		if (m_pProgress)
		{
			strText.LoadString(IDS_STACKING_APPLYADJUSTMENT);
			m_pProgress->Start2(strText, m_rcResult.Width() * m_rcResult.Height());
		};

		// Adjust the coverage of all pixels
		for (j = 0;j<m_rcResult.Height();j++)
		{
			for (i = 0;i<m_rcResult.Width();i++)
			{
				lProgress++;
				double					fRedCover,
										fGreenCover,
										fBlueCover;
				double					fRed,
										fGreen,
										fBlue;

				pCover->GetValue(i, j, fRedCover, fGreenCover, fBlueCover);
				m_pOutput->GetValue(i, j, fRed, fGreen, fBlue);

				if (fRedCover>0)
					fRed *= fMaxCoverage/fRedCover;
				if (fGreenCover>0)
					fGreen *= fMaxCoverage/fGreenCover;
				if (fBlueCover>0)
					fBlue *= fMaxCoverage/fBlueCover;

				m_pOutput->SetValue(i, j, fRed, fGreen, fBlue);
			};

			if (m_pProgress)
				m_pProgress->Progress2(NULL, lProgress);
		};

		if (m_pProgress)
			m_pProgress->End2();
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::SaveCalibratedAndRegisteredLightFrame(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_strCurrentLightFrame.GetLength() && pBitmap)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, NULL);

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
			CString				strText;

			strText.Format(IDS_SAVINGINTERMEDIATE, strOutputFile);
			m_pProgress->Start2(strText, 0);
		};
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pBitmap, m_pProgress, _T("Registered and Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_fExposure);
		else
			bResult = WriteFITS(strOutputFile, pBitmap, m_pProgress, _T("Registered and Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_fExposure);
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::SaveCalibratedLightFrame(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_strCurrentLightFrame.GetLength() && pBitmap)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, NULL);

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
			CString				strText;

			strText.Format(IDS_SAVINGCALIBRATED, strOutputFile);
			m_pProgress->Start2(strText, 0);
		};

		CSmartPtr<CMemoryBitmap>		pOutBitmap;

		if (m_bSaveCalibratedDebayered)
		{
			// Debayer the image
			if (!DebayerPicture(pBitmap, &pOutBitmap, m_pProgress))
				pOutBitmap = pBitmap;
		}
		else
			pOutBitmap = pBitmap;

		// Check and remove super pixel settings
		CCFABitmapInfo *			pCFABitmapInfo;
		CFATRANSFORMATION			CFATransform = CFAT_NONE;

		pCFABitmapInfo = dynamic_cast<CCFABitmapInfo *>(pOutBitmap.m_p);
		if (pCFABitmapInfo)
		{
			CFATransform = pCFABitmapInfo->GetCFATransformation();
			if (CFATransform == CFAT_SUPERPIXEL)
				pCFABitmapInfo->UseBilinear(TRUE);
		};
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pOutBitmap, m_pProgress, _T("Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_fExposure);
		else
			bResult = WriteFITS(strOutputFile, pOutBitmap, m_pProgress, _T("Calibrated light frame"), m_pLightTask->m_lISOSpeed, m_pLightTask->m_fExposure);

		if ((CFATransform == CFAT_SUPERPIXEL) && pCFABitmapInfo)
			pCFABitmapInfo->UseSuperPixels(TRUE);
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::SaveDeltaImage(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_strCurrentLightFrame.GetLength() && pBitmap)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, NULL);

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
			m_pProgress->Start2(NULL, 0);
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

BOOL CStackingEngine::SaveCometImage(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_strCurrentLightFrame.GetLength() && pBitmap)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, NULL);

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
			CString				strText;

			strText.Format(IDS_SAVINGCALIBRATED, strOutputFile);
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

BOOL CStackingEngine::SaveCometlessImage(CMemoryBitmap * pBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = FALSE;

	if (m_strCurrentLightFrame.GetLength() && pBitmap)
	{
		TCHAR			szDrive[1+_MAX_DRIVE];
		TCHAR			szDir[1+_MAX_DIR];
		TCHAR			szName[1+_MAX_FNAME];
		CString			strOutputFile;

		_tsplitpath(m_strCurrentLightFrame, szDrive, szDir, szName, NULL);

		strOutputFile = szDrive;
		strOutputFile += szDir;
		if (m_IntermediateFileFormat == IFF_TIFF)
			strOutputFile += "Cometless.tif";
		else
		{
			CString		strExt;

			GetFITSExtension(m_strCurrentLightFrame, strExt);

			strOutputFile += "Cometless"+strExt;
		};

		if (m_pProgress)
		{
			CString				strText;

			strText.Format(IDS_SAVINGCALIBRATED, strOutputFile);
			m_pProgress->Start2(strText, 0);
		};
		if (m_IntermediateFileFormat == IFF_TIFF)
			bResult = WriteTIFF(strOutputFile, pBitmap, m_pProgress, _T("Cometless image"));
		else
			bResult = WriteFITS(strOutputFile, pBitmap, m_pProgress, _T("Cometless image"));
		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

class CStackTask : public CMultitask
{
private :
	HANDLE						m_hPixelEvent;
	CStackingEngine *			m_pStackingEngine;
	CDSSProgress *				m_pProgress;
	std::vector<CPoint>			m_vLockedPixels;

public :
	CEntropyInfo				m_EntropyWindow;
	CSmartPtr<CMemoryBitmap>	m_pTempBitmap;
	BOOL						m_bColor;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CPixelTransform				m_PixTransform;
	CTaskInfo *					m_pLightTask;
	CBackgroundCalibration		m_BackgroundCalibration;
	CRect						m_rcResult;
	LONG						m_lPixelSizeMultiplier;
	CSmartPtr<CMemoryBitmap>	m_pOutput;
	CSmartPtr<CMemoryBitmap>	m_pEntropyCoverage;

public :
	CStackTask()
	{
		ZFUNCTRACE_RUNTIME();
		m_hPixelEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	};

	virtual ~CStackTask()
	{
		ZFUNCTRACE_RUNTIME();
		CloseHandle(m_hPixelEvent);
	};

	void	Init(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
	{
		ZFUNCTRACE_RUNTIME();
		m_pBitmap	= pBitmap;
		m_pProgress = pProgress;
	};

	virtual BOOL	DoTask(HANDLE hEvent);
	virtual BOOL	Process();
};

/* ------------------------------------------------------------------- */

BOOL	CStackTask::DoTask(HANDLE hEvent)
{
	ZFUNCTRACE_RUNTIME();

	BOOL					bResult = TRUE;

	LONG					i, j;
	BOOL					bEnd = FALSE;
	MSG						msg;
	LONG					lWidth = m_pBitmap->Width();
	PIXELDISPATCHVECTOR		vPixels;

	vPixels.reserve(16);

	// Create a message queue and signal the event
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(hEvent);
	while (!bEnd && GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_MT_PROCESS)
		{
			for (j = msg.wParam;j<msg.wParam+msg.lParam;j++)
			{
				for (i = 0;i<lWidth;i++)
				{
					CPointExt	pt(i, j);
					CPointExt	ptOut;

					ptOut = m_PixTransform.Transform(pt);

					COLORREF16		crColor;
					float			Red, 
									Green, 
									Blue;
					double			fRedEntropy = 1.0,
									fGreenEntropy = 1.0,
									fBlueEntropy = 1.0;

					if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
						crColor = m_EntropyWindow.GetPixel(i, j, fRedEntropy, fGreenEntropy, fBlueEntropy);
					else
						crColor = m_pBitmap->GetPixel16(i, j);

					Red		= crColor.red;
					Green	= crColor.green;
					Blue	= crColor.blue;

					if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
						m_BackgroundCalibration.ApplyCalibration(Red, Green, Blue);

					if ((Red || Green || Blue) && ptOut.IsInRect(0, 0, m_rcResult.Width()-1, m_rcResult.Height()-1))
					{
						vPixels.resize(0);
						ComputePixelDispatch(ptOut, m_lPixelSizeMultiplier, vPixels);

						for (LONG k = 0;k<vPixels.size();k++)
						{
							CPixelDispatch &		Pixel = vPixels[k];

							// For each plane adjust the values
							if (Pixel.m_lX >= 0 && Pixel.m_lX < m_rcResult.Width() && 
								Pixel.m_lY >= 0 && Pixel.m_lY < m_rcResult.Height())
							{
								// Special case for entropy average
								if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
								{
									if (m_bColor)
									{
										double				fOldRed,
															fOldGreen,
															fOldBlue;

										m_pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
										fOldRed   += Pixel.m_fPercentage*fRedEntropy;
										fOldGreen += Pixel.m_fPercentage*fGreenEntropy;
										fOldBlue  += Pixel.m_fPercentage*fBlueEntropy;
										m_pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);

										m_pOutput->GetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
										fOldRed   += Red*Pixel.m_fPercentage*fRedEntropy;
										fOldGreen += Green*Pixel.m_fPercentage*fGreenEntropy;
										fOldBlue  += Blue*Pixel.m_fPercentage*fBlueEntropy;
										m_pOutput->SetValue(Pixel.m_lX, Pixel.m_lY, fOldRed, fOldGreen, fOldBlue);
									}
									else
									{
										double				fOldGray;

										m_pEntropyCoverage->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
										fOldGray   += Pixel.m_fPercentage*fRedEntropy;
										m_pEntropyCoverage->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);

										m_pOutput->GetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
										fOldGray   += Red*Pixel.m_fPercentage*fRedEntropy;
										m_pOutput->SetValue(Pixel.m_lX, Pixel.m_lY, fOldGray);
									};
								}

								double		fPreviousRed, 
											fPreviousGreen, 
											fPreviousBlue;

								m_pTempBitmap->GetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
								fPreviousRed   += (double)Red/256.0 * Pixel.m_fPercentage;
								fPreviousGreen += (double)Green/256.0 * Pixel.m_fPercentage;
								fPreviousBlue  += (double)Blue/256.0 * Pixel.m_fPercentage;
								fPreviousRed   = min(fPreviousRed, 255.0);
								fPreviousGreen = min(fPreviousGreen, 255.0);
								fPreviousBlue  = min(fPreviousBlue, 255.0);
								m_pTempBitmap->SetPixel(Pixel.m_lX, Pixel.m_lY, fPreviousRed, fPreviousGreen, fPreviousBlue);
							};
						};
					};
				};
			};

			SetEvent(hEvent);
		}
		else if (msg.message == WM_MT_STOP)
			bEnd = TRUE;
	};

	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL	CStackTask::Process()
{
	ZFUNCTRACE_RUNTIME();

	BOOL				bResult = TRUE;
	LONG				lHeight = m_pBitmap->Height();
	LONG				i = 0;
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

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::CreateMasterLightMultiBitmap(CMemoryBitmap * pInBitmap, bool bColor, CMultiBitmap ** ppMultiBitmap)
{
	ZFUNCTRACE_RUNTIME();

	BOOL						bResult = FALSE;
	CSmartPtr<CMultiBitmap>		pMultiBitmap;

	C24BitColorBitmap * p24BitColorBitmap = dynamic_cast<C24BitColorBitmap *>(pInBitmap);
	C48BitColorBitmap * p48BitColorBitmap = dynamic_cast<C48BitColorBitmap *>(pInBitmap);
	C96BitColorBitmap * p96BitColorBitmap = dynamic_cast<C96BitColorBitmap *>(pInBitmap);
	C96BitFloatColorBitmap * p96BitFloatColorBitmap = dynamic_cast<C96BitFloatColorBitmap *>(pInBitmap);

	CGrayBitmap *		pGrayBitmap  = dynamic_cast<CGrayBitmap *>(pInBitmap);
	C8BitGrayBitmap *	p8BitGrayBitmap  = dynamic_cast<C8BitGrayBitmap *>(pInBitmap);
	C16BitGrayBitmap *	p16BitGrayBitmap  = dynamic_cast<C16BitGrayBitmap *>(pInBitmap);
	C32BitGrayBitmap *	p32BitGrayBitmap  = dynamic_cast<C32BitGrayBitmap *>(pInBitmap);
	C32BitFloatGrayBitmap * p32BitFloatGrayBitmap  = dynamic_cast<C32BitFloatGrayBitmap *>(pInBitmap);


	if (bColor)
	{
		if (p96BitColorBitmap || p32BitGrayBitmap)
			pMultiBitmap.Attach(new CColorMultiBitmapT<DWORD, float>);
		else if (p96BitFloatColorBitmap || p32BitFloatGrayBitmap)
			pMultiBitmap.Attach(new CColorMultiBitmapT<float, float>);
		else
			pMultiBitmap.Attach(new CColorMultiBitmapT<WORD, float>);
	}
	else
	{
		if (p96BitColorBitmap || p32BitGrayBitmap)
			pMultiBitmap.Attach(new CGrayMultiBitmapT<DWORD, float>);
		else if (p96BitFloatColorBitmap || p32BitFloatGrayBitmap)
			pMultiBitmap.Attach(new CGrayMultiBitmapT<float, float>);
		else
			pMultiBitmap.Attach(new CGrayMultiBitmapT<WORD, float>);
	};

	bResult = pMultiBitmap.CopyTo(ppMultiBitmap);

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::StackLightFrame(CMemoryBitmap * pInBitmap, CPixelTransform & PixTransform, double fExposure, BOOL bComet)
{
	ZFUNCTRACE_RUNTIME();

	BOOL						bResult = FALSE;
	LONG						lWidth,
								lHeight;
	bool						bColor = true;
	bool						bComputeCoverage = false;
	CString						strStart2;
	CString						strText;
	bool						bFirst = !m_lNrStacked;
	bool						bEntropyCoverage = FALSE;
	LONG						i, j;
	CSmartPtr<CMemoryBitmap>	pBitmap;

	// Two cases : Bayer Drizzle or not Bayer Drizzle - that is the question
	if (pInBitmap && m_pLightTask)
	{
		if (m_pProgress)
			m_pProgress->GetStart2Text(strStart2);

		C16BitGrayBitmap *	pGrayBitmap = dynamic_cast<C16BitGrayBitmap *>(pInBitmap);
		if (pGrayBitmap && (pGrayBitmap->GetCFATransformation() == CFAT_AHD))
		{
			// Start by demosaicing the input bitmap
			if (m_pProgress)
			{
				CString			strText;

				strText.LoadString(IDS_AHDDEMOSAICING);
				m_pProgress->Start2((LPCTSTR)strText, 0);
			};
			AHDDemosaicing(pGrayBitmap, &pBitmap, m_pProgress);
		}
		else
			pBitmap = pInBitmap;

		CStackTask		StackTask;

		StackTask.Init(pBitmap, m_pProgress);

		// Create the output bitmap
		lWidth = pBitmap->Width();
		lHeight = pBitmap->Height();

		bColor = !pBitmap->IsMonochrome() || pBitmap->IsCFA();

		if (pGrayBitmap && (m_pLightTask->m_Method == MBP_FASTAVERAGE))
		{
			if (pGrayBitmap->GetCFATransformation() == CFAT_RAWBAYER)
			{
				// A coverage is needed with Bayer Drizzle
				m_InputCFAType = pGrayBitmap->GetCFAType();
				m_vPixelTransforms.push_back(PixTransform);
			};
		}
		else if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
		{
			// A coverage is needed with Entropy Average
			if (!m_pEntropyCoverage)
			{
				if (bColor)
					m_pEntropyCoverage.Attach(new C96BitFloatColorBitmap); 
				else
					m_pEntropyCoverage.Attach(new C32BitFloatGrayBitmap); 

				m_pEntropyCoverage->Init(m_rcResult.Width(), m_rcResult.Height());
			};
		};


		// Compute entropy window info

		if (m_pLightTask->m_Method == MBP_ENTROPYAVERAGE)
		{
			if (m_pProgress)
			{
				strText.LoadString(IDS_COMPUTINGENTROPY);
				m_pProgress->Start2(strText, 0);
			};
			StackTask.m_EntropyWindow.Init(pBitmap, 10, m_pProgress);
		};

		// Compute histogram for median/min/max and picture backgound calibration
		// information
		if (m_BackgroundCalibration.m_BackgroundCalibrationMode != BCM_NONE)
		{
			if (m_pProgress)
			{
				strText.LoadString(IDS_COMPUTINGBACKGROUNDCALIBRATION);
				m_pProgress->Start2(strText, 0);
			};
			m_BackgroundCalibration.ComputeBackgroundCalibration(pBitmap, bFirst, m_pProgress);
		};

		// Create a master light to enable stacking
		if (!m_pMasterLight)
		{
			// Create a master light ... but not from the input bitmap
			// -> use Gray scale or Color depending on the bColor parameter
			CreateMasterLightMultiBitmap(pInBitmap, bColor, &m_pMasterLight);
			m_pMasterLight->SetProcessingMethod(m_pLightTask->m_Method, m_pLightTask->m_fKappa, m_pLightTask->m_lNrIterations);
			m_pMasterLight->SetNrBitmaps(m_lNrCurrentStackable);

			if (m_bCometStacking && m_bCreateCometImage)
				m_pMasterLight->SetHomogenization(TRUE);
		};

		// Create temporary bitmap
		//CSmartPtr<CMemoryBitmap>		pTempBitmap;

		if (m_pMasterLight)
		{
			m_pMasterLight->CreateNewMemoryBitmap(&StackTask.m_pTempBitmap);
			if (StackTask.m_pTempBitmap)
			{
				StackTask.m_pTempBitmap->Init(m_rcResult.Width(), m_rcResult.Height());
				StackTask.m_pTempBitmap->SetISOSpeed(pBitmap->GetISOSpeed());
				StackTask.m_pTempBitmap->SetExposure(pBitmap->GetExposure());
				StackTask.m_pTempBitmap->SetNrFrames(pBitmap->GetNrFrames());
			};
		};

		// Create output bitmap only when necessary (full 32 bits float)
		if ((m_pLightTask->m_Method == MBP_FASTAVERAGE) ||
			(m_pLightTask->m_Method == MBP_ENTROPYAVERAGE) ||
			(m_pLightTask->m_Method == MBP_MAXIMUM))
		{
			if (!m_pOutput)
			{
				// Allocate output bitmap
				if (bColor)
					m_pOutput.Attach(new C96BitFloatColorBitmap); 
				else
					m_pOutput.Attach(new C32BitFloatGrayBitmap); 

				m_pOutput->Init(m_rcResult.Width(), m_rcResult.Height());
			};
		};

		if (StackTask.m_pTempBitmap)
		{
			LONG				lProgress = 0;

			if (m_pProgress)
				m_pProgress->Start2(strStart2, lHeight);

			StackTask.m_PixTransform			= PixTransform;
			StackTask.m_pLightTask				= m_pLightTask;
			StackTask.m_bColor					= bColor;
			StackTask.m_BackgroundCalibration	= m_BackgroundCalibration;
			StackTask.m_rcResult				= m_rcResult;
			StackTask.m_lPixelSizeMultiplier	= m_lPixelSizeMultiplier;
			StackTask.m_pOutput					= m_pOutput;
			StackTask.m_pEntropyCoverage		= m_pEntropyCoverage;
			StackTask.StartThreads();
			StackTask.Process();

			if (m_bCreateCometImage)
			{
				// At this point - remove the stars
				//RemoveStars(StackTask.m_pTempBitmap, PixTransform, vStars);
			}
			else if (m_pComet && bComet)
			{
				// Subtract the comet from the light frame
				//WriteTIFF("E:\\BeforeCometSubtraction.tiff", StackTask.m_pTempBitmap, m_pProgress);
				//WriteTIFF("E:\\SubtractedComet.tiff", m_pComet, m_pProgress);
				ShiftAndSubtract(StackTask.m_pTempBitmap, m_pComet, m_pProgress, -PixTransform.m_fXCometShift, -PixTransform.m_fYCometShift);
				//WriteTIFF("E:\\AfterCometSubtraction.tiff", StackTask.m_pTempBitmap, m_pProgress);
			};

			if (m_pLightTask->m_Method == MBP_FASTAVERAGE)
			{
				// Use the result to average
				for (j = 0;j<m_rcResult.Height();j++)
					for (i = 0;i<m_rcResult.Width();i++)
					{
						if (bColor)
						{
							double			fOutRed, fOutGreen, fOutBlue;
							double			fNewRed, fNewGreen, fNewBlue;

							m_pOutput->GetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
							StackTask.m_pTempBitmap->GetPixel(i, j, fNewRed, fNewGreen, fNewBlue);
							fOutRed		= (fOutRed * m_lNrStacked + fNewRed)/(double)(m_lNrStacked+1);
							fOutGreen	= (fOutGreen * m_lNrStacked + fNewGreen)/(double)(m_lNrStacked+1);
							fOutBlue	= (fOutBlue * m_lNrStacked + fNewBlue)/(double)(m_lNrStacked+1);
							m_pOutput->SetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
						}
						else
						{
							double			fOutGray;
							double			fNewGray;

							m_pOutput->GetPixel(i, j, fOutGray);
							StackTask.m_pTempBitmap->GetPixel(i, j, fNewGray);
							fOutGray	= (fOutGray * m_lNrStacked + fNewGray)/(double)(m_lNrStacked+1);
							m_pOutput->SetPixel(i, j, fOutGray);
						};
					};
			}
			else if (m_pLightTask->m_Method == MBP_MAXIMUM)
			{
				// Use the result to maximize
				for (j = 0;j<m_rcResult.Height();j++)
					for (i = 0;i<m_rcResult.Width();i++)
					{
						if (bColor)
						{
							double			fOutRed, fOutGreen, fOutBlue;
							double			fNewRed, fNewGreen, fNewBlue;

							m_pOutput->GetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
							StackTask.m_pTempBitmap->GetPixel(i, j, fNewRed, fNewGreen, fNewBlue);
							fOutRed		= max(fOutRed, fNewRed);
							fOutGreen	= max(fOutGreen, fNewGreen);
							fOutBlue	= max(fOutBlue, fNewBlue);;
							m_pOutput->SetPixel(i, j, fOutRed, fOutGreen, fOutBlue);
						}
						else
						{
							double			fOutGray;
							double			fNewGray;

							m_pOutput->GetPixel(i, j, fOutGray);
							StackTask.m_pTempBitmap->GetPixel(i, j, fNewGray);
							fOutGray	= max(fOutGray, fNewGray);
							m_pOutput->SetPixel(i, j, fOutGray);
						};
					};
			}
			else if ((m_pLightTask->m_Method != MBP_ENTROPYAVERAGE) && 
				      m_pMasterLight && StackTask.m_pTempBitmap)
				m_pMasterLight->AddBitmap(StackTask.m_pTempBitmap, m_pProgress);

			if (m_bSaveIntermediate && !m_bCreateCometImage)
			{
				// Save the pTempBitmap to a TIFF File
				StackTask.m_pTempBitmap->m_ExtraInfo = pInBitmap->m_ExtraInfo;
				StackTask.m_pTempBitmap->m_DateTime  = pInBitmap->m_DateTime;
				SaveCalibratedAndRegisteredLightFrame(StackTask.m_pTempBitmap);
			};

			m_fTotalExposure += fExposure;
			bResult = TRUE;
		}
		else
		{
			// Error - not enough memory
			bResult = FALSE;
		};

		if (m_pProgress)
			m_pProgress->End2();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::StackAll(CAllStackingTasks & tasks, CMemoryBitmap ** ppBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;
	BOOL				bContinue = TRUE;

	m_lNrStacked = 0;
	GetResultISOSpeed();
	GetResultDateTime();
	GetResultExtraInfo();

	if (ppBitmap)
		*ppBitmap = NULL;

	m_vCometShifts.clear();
	switch (tasks.GetStackingMode())
	{
	case SM_MOSAIC :
		{
			CString				strDrive;
			CString				strFreeSpace;
			CString				strNeededSpace;

			ComputeLargestRectangle(m_rcResult);
			__int64				ulNeededSpace;
			__int64				ulFreeSpace;
			CRect				rcResult(m_rcResult);

			rcResult.left   /= m_lPixelSizeMultiplier;
			rcResult.right  /= m_lPixelSizeMultiplier;
			rcResult.top    /= m_lPixelSizeMultiplier;
			rcResult.bottom /= m_lPixelSizeMultiplier;

			ulNeededSpace	= tasks.ComputeNecessaryDiskSpace(rcResult);
			ulFreeSpace		= tasks.AvailableDiskSpace(strDrive);

			if (m_pProgress && (ulNeededSpace>ulFreeSpace))
			{
				CString			strText;
				CString			strContinue;

				SpaceToString(ulFreeSpace, strFreeSpace);
				SpaceToString(ulNeededSpace, strNeededSpace);

				strText.Format(IDS_RECAP_WARNINGDISKSPACE, strNeededSpace, strDrive, strFreeSpace);
				strContinue.LoadString(IDS_WANTTOCONTINUE);

				strText += strContinue;
				bContinue = m_pProgress->Warning((LPCTSTR)strText);
			};
		};
		break;
	case SM_INTERSECTION:
		if (!ComputeSmallestRectangle(m_rcResult))
		{
			// Fall back to normal rectangle
			LONG            lBitmapIndice = 0;
			if (m_vBitmaps[0].m_bDisabled)
				lBitmapIndice = 1;

			m_rcResult.left = m_rcResult.top = 0;
			m_rcResult.right = m_vBitmaps[lBitmapIndice].RenderedWidth() * m_lPixelSizeMultiplier;
			m_rcResult.bottom = m_vBitmaps[lBitmapIndice].RenderedHeight() * m_lPixelSizeMultiplier;
		};
		break;
	case SM_CUSTOM :
		tasks.GetCustomRectangle(m_rcResult);
		m_rcResult.left		*= m_lPixelSizeMultiplier;
		m_rcResult.right	*= m_lPixelSizeMultiplier;
		m_rcResult.top		*= m_lPixelSizeMultiplier;
		m_rcResult.bottom	*= m_lPixelSizeMultiplier;
		break;
	case SM_NORMAL :
		{
			LONG			lBitmapIndice = 0;
			if (m_vBitmaps[0].m_bDisabled)
				lBitmapIndice = 1;

			m_rcResult.left = m_rcResult.top = 0;
			m_rcResult.right  = m_vBitmaps[lBitmapIndice].RenderedWidth() * m_lPixelSizeMultiplier;
			m_rcResult.bottom = m_vBitmaps[lBitmapIndice].RenderedHeight() * m_lPixelSizeMultiplier;
		}
		break;
	};

	ZTRACE_RUNTIME("Computed image rectangle m_rcResult left %ld, right %ld, top %ld, bottom %ld", \
		m_rcResult.left, m_rcResult.right, m_rcResult.top, m_rcResult.bottom);

	if (bContinue)
	{
		// Iterate all light tasks until everything is done
		LONG			lFirstTaskID = 0;
		BOOL			bEnd = FALSE;
		BOOL			bStop = FALSE;
		LONG			i;
		CString			strText;

		if (m_vBitmaps.size())
			lFirstTaskID = tasks.FindStackID(m_vBitmaps[0].m_strFileName);

		while (!bEnd)
		{
			bEnd = TRUE;
			CStackingInfo *		pStackingInfo = NULL;

			for (i = 0;i<tasks.m_vStacks.size() && bEnd;i++)
			{
				if (tasks.m_vStacks[i].m_pLightTask)
				{
					if (lFirstTaskID)
					{
						if (tasks.m_vStacks[i].m_pLightTask->m_dwTaskID == lFirstTaskID)
						{
							bEnd = FALSE;
							lFirstTaskID = 0;
							pStackingInfo = &(tasks.m_vStacks[i]);
						};
					}
					else if (!tasks.m_vStacks[i].m_pLightTask->m_bDone)
					{
						bEnd = FALSE;
						pStackingInfo = &(tasks.m_vStacks[i]);
					};
				};
			};

			if (pStackingInfo && 
				pStackingInfo->m_pLightTask && 
				pStackingInfo->m_pLightTask->m_vBitmaps.size())
			{
				// Do stack these
				CMasterFrames				MasterFrames;

				MasterFrames.LoadMasters(pStackingInfo, m_pProgress);

				m_pLightTask = pStackingInfo->m_pLightTask;

				if ((m_pLightTask->m_Method == MBP_AVERAGE) && !m_bCreateCometImage && !m_pComet)
					m_pLightTask->m_Method = MBP_FASTAVERAGE;

				for (i = 0;i<pStackingInfo->m_pLightTask->m_vBitmaps.size() && !bStop;i++)
				{
					// Stack this bitmap
					LONG			lIndice;

					lIndice = FindBitmapIndice(pStackingInfo->m_pLightTask->m_vBitmaps[i].m_strFileName);
					if (lIndice >= 0)
					{
						CString				strModel;

						if (!m_vBitmaps[lIndice].m_bDisabled)
						{
							BOOL			bComet = m_vBitmaps[lIndice].m_bComet;
							BOOL			bStack = TRUE;


							CPixelTransform		PixTransform(m_vBitmaps[lIndice].m_BilinearParameters);

							if (m_bCometStacking || m_bCreateCometImage)
							{
								if (m_vBitmaps[0].m_bComet && m_vBitmaps[lIndice].m_bComet)
									PixTransform.ComputeCometShift(m_vBitmaps[0].m_fXComet, m_vBitmaps[0].m_fYComet,
																   m_vBitmaps[lIndice].m_fXComet, m_vBitmaps[lIndice].m_fYComet, FALSE,
																   m_vBitmaps[lIndice].m_bTransformedCometPosition);
								else if (m_bCreateCometImage)
									bStack = FALSE;
							}
							else if (m_pComet)
							{
								if (m_vBitmaps[0].m_bComet && m_vBitmaps[lIndice].m_bComet)
									PixTransform.ComputeCometShift(m_vBitmaps[0].m_fXComet, m_vBitmaps[0].m_fYComet,
																   m_vBitmaps[lIndice].m_fXComet, m_vBitmaps[lIndice].m_fYComet, TRUE,
																   m_vBitmaps[lIndice].m_bTransformedCometPosition);
							};

							PixTransform.SetShift(-m_rcResult.left, -m_rcResult.top);
							PixTransform.SetPixelSizeMultiplier(m_lPixelSizeMultiplier);

							if (bStack)
							{
								ZTRACE_RUNTIME("Stack %s", (LPCTSTR)m_vBitmaps[lIndice].m_strFileName);

								if (m_pProgress)
								{
									strText.Format(IDS_STACKING_PICTURE, (m_lNrStacked+1), m_lNrCurrentStackable, m_vBitmaps[lIndice].m_fXOffset, m_vBitmaps[lIndice].m_fYOffset, m_vBitmaps[lIndice].m_fAngle * 180/M_PI);
									m_pProgress->Progress1(strText, m_lNrStacked+1);
								};

								CSmartPtr<CMemoryBitmap>		pBitmap;

								if (::LoadFrame(m_vBitmaps[lIndice].m_strFileName, PICTURETYPE_LIGHTFRAME, m_pProgress, &pBitmap))
								{
									CString				strDescription;

									strDescription = m_vBitmaps[lIndice].m_strInfos;
									if (m_vBitmaps[lIndice].m_lNrChannels==3)
										strText.Format(IDS_STACKRGBLIGHT, m_vBitmaps[lIndice].m_lBitPerChannels, (LPCTSTR)strDescription, (LPCTSTR)m_vBitmaps[lIndice].m_strFileName);
									else
										strText.Format(IDS_STACKGRAYLIGHT, m_vBitmaps[lIndice].m_lBitPerChannels, (LPCTSTR)strDescription, (LPCTSTR)m_vBitmaps[lIndice].m_strFileName);

									// First apply transformations
									MasterFrames.ApplyAllMasters(pBitmap, &(m_vBitmaps[lIndice].m_vStars), m_pProgress);

									// Here save the calibrated light frame if needed
									m_strCurrentLightFrame = m_vBitmaps[lIndice].m_strFileName;
									CSmartPtr<CMemoryBitmap>		pDelta;

									ApplyCosmetic(pBitmap, &pDelta, m_PostCalibrationSettings, m_pProgress);
									if (m_bSaveCalibrated)
										SaveCalibratedLightFrame(pBitmap);
									if (pDelta)
										SaveDeltaImage(pDelta);

									if (m_pProgress)
										m_pProgress->Start2(strText, 0);

									// Stack
									bStop = !StackLightFrame(pBitmap, PixTransform, m_vBitmaps[lIndice].m_fExposure, bComet);
									m_lNrStacked++;

									if (m_bCreateCometImage)
										m_vCometShifts.push_back(CImageCometShift((LONG)m_vCometShifts.size(), PixTransform.m_fXCometShift, PixTransform.m_fYCometShift));

									if (m_pProgress)
									{
										m_pProgress->End2();
										bStop = bStop || m_pProgress->IsCanceled();
									};
								};
							};
						};				
					};
				};

				pStackingInfo->m_pLightTask->m_bDone = TRUE;

				bEnd = bStop;
			};
		};

		bResult = !bStop;

		// Clear the cache
		ClearTaskCache();

		if (bResult)
		{
			if (m_pMasterLight && m_pMasterLight->GetNrAddedBitmaps())
				ComputeBitmap();
			AdjustEntropyCoverage();
			AdjustBayerDrizzleCoverage();

			if (m_pOutput)
			{
				m_pOutput->SetExposure(m_fTotalExposure);
				m_pOutput->SetISOSpeed(m_lISOSpeed);
				m_pOutput->SetNrFrames(m_lNrStacked);
				m_pOutput->m_DateTime = m_DateTime;
				m_pOutput->m_ExtraInfo = m_ExtraInfo;
			};

			bResult = m_pOutput.CopyTo(ppBitmap);
		};

		m_pLightTask = NULL;
	};

	// Clear everything
	m_pOutput.Release();
	m_pEntropyCoverage.Release();

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CStackingEngine::StackLightFrames(CAllStackingTasks & tasks, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap)
{
	ZFUNCTRACE_RUNTIME();
	BOOL						bResult = FALSE;
	BOOL						bContinue = TRUE;
	CString						strText;
	CSmartPtr<CMemoryBitmap>	pBitmap;

	m_InputCFAType = CFATYPE_NONE;
	m_pProgress = pProgress;

	// 1. compute light frames offsets
	// Only for registered light frame
	AddLightFramesToList(tasks);
	ComputeOffsets();

	// 2. disable non stackable light frames
	RemoveNonStackableLightFrames(tasks);

	if (pProgress && (m_lNrStackable==1) && (tasks.GetNrLightFrames()>1))
	{
		CString			strText;
		CString			strContinue;

		strText.Format(IDS_WARNING_ONLYONEFRAME, tasks.GetNrLightFrames());
		strContinue.LoadString(IDS_WANTTOCONTINUE);

		strText += strContinue;
		bContinue = m_pProgress->Warning((LPCTSTR)strText);
	};

	if (bContinue)
	{
		m_lNrCurrentStackable = m_lNrStackable;
		if (tasks.IsCometAvailable() && 
			(tasks.GetCometStackingMode()==CSM_COMETSTAR))
			 m_lNrCurrentStackable = m_lNrCometStackable;

		strText.LoadString(IDS_STACKING);
		if (pProgress)
			pProgress->Start(strText, m_lNrCurrentStackable, TRUE);

		// 3. do all pre-tasks (the one not already done by the registering process)
		bResult = m_lNrStackable && tasks.DoAllPreTasks(pProgress);

		// Again - in case pretasks change the progress settings
		if (pProgress)
			pProgress->Start(strText, m_lNrCurrentStackable, TRUE);

		// 4. Stack everything
		if (bResult)
		{
			if (tasks.IsCometAvailable() && 
				((tasks.GetCometStackingMode()==CSM_COMETONLY) ||
				 (tasks.GetCometStackingMode()==CSM_COMETSTAR)))
			{
				m_bCometStacking = TRUE;
				m_bCreateCometImage = (tasks.GetCometStackingMode()==CSM_COMETSTAR);
			};
			bResult = StackAll(tasks, &pBitmap);

			if (bResult && tasks.IsCometAvailable() && 
				(tasks.GetCometStackingMode()==CSM_COMETSTAR))
			{
				if (m_bApplyFilterToCometImage)
				{
					GetFilteredImage(pBitmap, &m_pComet, 1, pProgress);
					
					pBitmap = m_pComet;
					m_pComet.Release();
					CDirectionalImageFilter		Filter;

					Filter.SetAngle(m_fStarTrailsAngle+M_PI/2.0, 2);
					Filter.ApplyFilter(pBitmap, &m_pComet);
				}
				else
					m_pComet = pBitmap;

				if (m_bSaveIntermediateCometImages)
					SaveCometImage(m_pComet);

				pBitmap.Release();
				m_bCometStacking = FALSE;
				m_bCreateCometImage = FALSE;
				m_lNrCurrentStackable = m_lNrStackable;

				if (pProgress)
					pProgress->Start(strText, m_lNrCurrentStackable, TRUE);

				// Stack again but remove the comet before stacking
				tasks.ResetTasksStatus();
				bResult = StackAll(tasks, &pBitmap);

				if (m_bSaveIntermediateCometImages)
					SaveCometlessImage(pBitmap);

				// Then add the comet to the resulting image (simple addition combination)
				Add(pBitmap, m_pComet);
			};

			if (bResult && m_bChannelAlign)
			{
				CChannelAlign		channelAlign;
				
				channelAlign.AlignChannels(pBitmap, m_pProgress);
			};

			if (bResult)
				pBitmap.CopyTo(ppBitmap);
		};

		m_pProgress = NULL;
		m_pEntropyCoverage.Release();
		m_pComet.Release();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CStackingEngine::ComputeOffsets(CAllStackingTasks & tasks, CDSSProgress * pProgress)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bResult = FALSE;

	m_pProgress = pProgress;

	AddLightFramesToList(tasks);
	ComputeOffsets();

	m_pProgress = NULL;

	return bResult;
};

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
		_tsplitpath(m_vBitmaps[0].m_strFileName, szDrive, szDir, NULL, NULL);

		strOutputFolder = szDrive;
		strOutputFolder += szDir;
	};

	if (OutputSettings.m_bOtherFolder && OutputSettings.m_strFolder.GetLength())
	{
		strOutputFolder = OutputSettings.m_strFolder;
	};

	if (OutputSettings.m_bFileListFolder && strFileList.GetLength())
	{
		_tsplitpath(strFileList, szDrive, szDir, NULL, NULL);

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
			_tsplitpath(szFileList, NULL, NULL, szName, NULL);
			strBaseName = szName;
			if (!strBaseName.GetLength())
				strBaseName = _T("Autosave");
		};
	};

	{
		CString			strBasePath;
		CString			strExt;
		BOOL			bFileExists = FALSE;
		LONG			lNumber = 0;

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
				GetFITSExtension(m_vBitmaps[0].m_strFileName, strExt);
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
					bFileExists = TRUE;
					strFileName.Format(_T("%s%s%03ld%s"), (LPCTSTR)strBasePath, (LPCTSTR)strBaseName, lNumber, (LPCTSTR)strExt);
				}
				else
					bFileExists = FALSE;
			}
			while (bFileExists && (lNumber<1000));
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CStackingEngine::WriteDescription(CAllStackingTasks & tasks, LPCTSTR szOutputFile)
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

		_tsplitpath(szOutputFile, szDrive, szDir, szName, NULL);
		strOutputFile = szDrive;
		strOutputFile += szDir;
		strOutputFile += szName;
		strOutputFile += _T(".html");
		FILE *				hFile;

		hFile = _tfopen(strOutputFile, _T("wt"));
		if (hFile)
		{
			CString			strText;

			_tsplitpath(strOutputFile, NULL, NULL, szName, NULL);

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
			DWORD				dwDrizzle;

			dwDrizzle = tasks.GetPixelSizeMultiplier();
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
				fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
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
			LONG				i, j;
			LONG				lTotalExposure = 0;
			CString				strBackgroundCalibration;
			CString				strPerChannelBackgroundCalibration;
			CString				strDarkOptimization;
			CString				strDarkFactor;
			CString				strExposure;
			CString				strISO;
			CString				strHotPixels;
			CString				strYesNo;
			BACKGROUNDCALIBRATIONMODE	CalibrationMode;

			CalibrationMode = tasks.GetBackgroundCalibrationMode();

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
					LONG			lTaskExposure = 0;

					for (j = 0;j<si.m_pLightTask->m_vBitmaps.size();j++)
						lTaskExposure += si.m_pLightTask->m_vBitmaps[j].m_fExposure;

					lTotalExposure += lTaskExposure;

					ExposureToString(lTaskExposure, strExposure);
					ISOToString(si.m_pLightTask->m_lISOSpeed, strISO);

					strText.Format(IDS_RECAP_STEP, i+1, si.m_pLightTask->m_vBitmaps.size(), strISO);
					fprintf(hFile, "<a href=\"#Task%ld\">%s</a>", i, (LPCSTR)CT2CA(strText, CP_UTF8));
					fprintf(hFile, (LPCSTR)CT2CA(strExposure, CP_UTF8));
					fprintf(hFile, "<br>");
					fprintf(hFile, "<ul>");
					fprintf(hFile, (LPCSTR)CT2CA(strBackgroundCalibration, CP_UTF8));
					fprintf(hFile, "<br>");
					fprintf(hFile, (LPCSTR)CT2CA(strPerChannelBackgroundCalibration, CP_UTF8));
					fprintf(hFile, "</ul>");
					if (si.m_pLightTask->m_vBitmaps.size()>1)
					{
						fprintf(hFile, "<ul>");
						strText.Format(IDS_RECAP_METHOD);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						FormatFromMethod(strText, si.m_pLightTask->m_Method, si.m_pLightTask->m_fKappa, si.m_pLightTask->m_lNrIterations);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						fprintf(hFile, "</ul>");

						if ((si.m_pLightTask->m_Method != MBP_AVERAGE) && 
							(IsRawBayer() || IsFITSRawBayer()))
						{
							fprintf(hFile, "<br>");
							strText.Format(IDS_RECAP_WARNINGBAYERDRIZZLE);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						};
					};

					fprintf(hFile, "<hr>");
					if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
						fprintf(hFile, "<ul>");

					if (si.m_pOffsetTask)
					{
						ExposureToString(si.m_pOffsetTask->m_fExposure, strExposure);
						ISOToString(si.m_pOffsetTask->m_lISOSpeed, strISO);

						strText.Format(IDS_RECAP_OFFSET, si.m_pOffsetTask->m_vBitmaps.size(), strISO, strExposure);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));

						if (si.m_pOffsetTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							FormatFromMethod(strText, si.m_pOffsetTask->m_Method, si.m_pOffsetTask->m_fKappa, si.m_pOffsetTask->m_lNrIterations);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						}
						else
							fprintf(hFile, "<br>");

						if (si.m_pOffsetTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						{
							strText.Format(IDS_RECAP_ISOWARNING);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						};
						fprintf(hFile, "</ul>");
					}
					else
					{
						strText.Format(IDS_RECAP_NOOFFSET);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
					};
					if (si.m_pDarkTask)
					{
						ExposureToString(si.m_pDarkTask->m_fExposure, strExposure);
						ISOToString(si.m_pDarkTask->m_lISOSpeed, strISO);

						strText.Format(IDS_RECAP_DARK, si.m_pDarkTask->m_vBitmaps.size(), strISO, strExposure);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));

						if (si.m_pDarkTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							FormatFromMethod(strText, si.m_pDarkTask->m_Method, si.m_pDarkTask->m_fKappa, si.m_pDarkTask->m_lNrIterations);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						};

						fprintf(hFile, "<ul>");
						fprintf(hFile, (LPCSTR)CT2CA(strDarkOptimization, CP_UTF8));
						fprintf(hFile, (LPCSTR)CT2CA(strHotPixels, CP_UTF8));
						if (strDarkFactor.GetLength())
						{
							fprintf(hFile, (LPCSTR)CT2CA(strDarkFactor, CP_UTF8));
							fprintf(hFile, "<br>");
						};

						if (si.m_pDarkTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						{
							strText.Format(IDS_RECAP_ISOWARNING);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							fprintf(hFile, "<br>");
						};
						if (!AreExposureEquals(si.m_pDarkTask->m_fExposure, si.m_pLightTask->m_fExposure))
						{
							strText.Format(IDS_RECAP_EXPOSUREWARNING);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							fprintf(hFile, "<br>");
						};
						fprintf(hFile, "</ul>");
					}
					else
					{
						strText.Format(IDS_RECAP_NODARK);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
					};
					if (si.m_pDarkFlatTask && si.m_pFlatTask)
					{
						ExposureToString(si.m_pDarkFlatTask->m_fExposure, strExposure);
						ISOToString(si.m_pDarkFlatTask->m_lISOSpeed, strISO);

						strText.Format(IDS_RECAP_DARKFLAT, si.m_pDarkFlatTask->m_vBitmaps.size(), strISO, strExposure);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));

						if (si.m_pDarkFlatTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, (LPCSTR)CT2CA(strText+"<br>", CP_UTF8));
							FormatFromMethod(strText, si.m_pDarkFlatTask->m_Method, si.m_pDarkFlatTask->m_fKappa, si.m_pDarkFlatTask->m_lNrIterations);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						}
						else
							fprintf(hFile, "<br>");

						if (si.m_pDarkFlatTask->m_lISOSpeed != si.m_pFlatTask->m_lISOSpeed)
						{
							strText.Format(IDS_RECAP_ISOWARNINGDARKFLAT);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						};
						if (!AreExposureEquals(si.m_pDarkFlatTask->m_fExposure, si.m_pFlatTask->m_fExposure))
						{
							strText.Format(IDS_RECAP_EXPOSUREWARNINGDARKFLAT);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						};
						fprintf(hFile, "</ul>");
					};
					if (si.m_pFlatTask)
					{
						ExposureToString(si.m_pFlatTask->m_fExposure, strExposure);
						ISOToString(si.m_pFlatTask->m_lISOSpeed, strISO);

						strText.Format(IDS_RECAP_FLAT, si.m_pFlatTask->m_vBitmaps.size(), strISO, strExposure);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						if (si.m_pFlatTask->m_vBitmaps.size()>1)
						{
							fprintf(hFile, "<ul>");
							strText.Format(IDS_RECAP_METHOD);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							FormatFromMethod(strText, si.m_pFlatTask->m_Method, si.m_pFlatTask->m_fKappa, si.m_pFlatTask->m_lNrIterations);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
							fprintf(hFile, "</ul>");
						};

						if (si.m_pFlatTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						{
							strText.Format(IDS_RECAP_ISOWARNING);
							fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
						};
						fprintf(hFile, "</ul>");
					}
					else
					{
						strText.Format(IDS_RECAP_NOFLAT);
						fprintf(hFile, (LPCSTR)CT2CA(strText, CP_UTF8));
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
							fprintf(hFile, "%s<br>", (LPCSTR)CT2CA(si.m_pLightTask->m_vBitmaps[j].m_strFileName, CP_UTF8));

						if (si.m_pOffsetTask && si.m_pOffsetTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_OFFSET);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pOffsetTask->m_strOutputFile != si.m_pOffsetTask->m_vBitmaps[0].m_strFileName)
							{
								strText.LoadString(IDS_TYPE_MASTEROFFSET);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8), (LPCSTR)CT2CA(si.m_pOffsetTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pOffsetTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", (LPCSTR)CT2CA(si.m_pOffsetTask->m_vBitmaps[j].m_strFileName, CP_UTF8));
						};

						if (si.m_pDarkTask && si.m_pDarkTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_DARK);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pDarkTask->m_strOutputFile != si.m_pDarkTask->m_vBitmaps[0].m_strFileName)
							{
								strText.LoadString(IDS_TYPE_MASTERDARK);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8), (LPCSTR)CT2CA(si.m_pDarkTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pDarkTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", (LPCSTR)CT2CA(si.m_pDarkTask->m_vBitmaps[j].m_strFileName, CP_UTF8));
						};

						if (si.m_pDarkFlatTask && si.m_pDarkFlatTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_DARKFLAT);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pDarkFlatTask->m_strOutputFile != si.m_pDarkFlatTask->m_vBitmaps[0].m_strFileName)
							{
								strText.LoadString(IDS_TYPE_MASTERDARKFLAT);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8), 
									(LPCSTR)CT2CA(si.m_pDarkFlatTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pDarkFlatTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", (LPCSTR)CT2CA(si.m_pDarkFlatTask->m_vBitmaps[j].m_strFileName, CP_UTF8));
						};
						if (si.m_pFlatTask && si.m_pFlatTask->m_vBitmaps.size())
						{
							strText.LoadString(IDS_TYPE_FLAT);
							fprintf(hFile, "<b>%s</b><br>\n", (LPCSTR)CT2CA(strText, CP_UTF8));
							if (si.m_pFlatTask->m_strOutputFile != si.m_pFlatTask->m_vBitmaps[0].m_strFileName)
							{
								strText.LoadString(IDS_TYPE_MASTERFLAT);
								fprintf(hFile, "%s -> %s<br>", (LPCSTR)CT2CA(strText, CP_UTF8),
									(LPCSTR)CT2CA(si.m_pFlatTask->m_strOutputFile, CP_UTF8));
							};
							for (j = 0;j<si.m_pFlatTask->m_vBitmaps.size();j++)
								fprintf(hFile, "%s<br>", (LPCSTR)CT2CA(si.m_pFlatTask->m_vBitmaps[j].m_strFileName, CP_UTF8));
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
