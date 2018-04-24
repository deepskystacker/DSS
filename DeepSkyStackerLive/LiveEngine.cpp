#include <stdafx.h>
#include "LiveEngine.h"
#include "RegisterEngine.h"
#include "TIFFUtil.h"

#define _USE_MATH_DEFINES
#include <cmath>

const DWORD				WM_LE_MESSAGE		= WM_USER+1;

#ifndef M_PI
#define M_PI			3.141592654
#endif

extern void	SetUILanguage();

/* ------------------------------------------------------------------- */

DWORD	WINAPI	LiveEngineThreadProc(LPVOID lpParameter)
{
	DWORD						dwResult = 0;
	CLiveEngine *				pLiveEngine = reinterpret_cast<CLiveEngine *>(lpParameter);

	SetUILanguage();
	if (pLiveEngine)
		pLiveEngine->LiveEngine();

	return dwResult;
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::MoveImage(LPCTSTR szFileName)
{
	if (m_LiveSettings.IsStack_Move())
	{
		// Move szFileName to the NonStackable subfolder
		// (create it if necessary)
		TCHAR					szDrive[_MAX_DRIVE];
		TCHAR					szDir[_MAX_DIR];
		TCHAR					szName[_MAX_FNAME];
		TCHAR					szExt[_MAX_EXT];

		_tsplitpath(szFileName, szDrive, szDir, szName, szExt);
		CString					strSubFolder;
		CString					strFileName;

		strSubFolder.Format(_T("%s%sNonStackable"), szDrive, szDir);
		strFileName.Format(_T("%s%s"), szName, szExt);
		// Create sub folder
		BOOL					bResult;

		bResult = CreateDirectory(strSubFolder, NULL);
		if (!bResult)
			bResult = (GetLastError() == ERROR_ALREADY_EXISTS);

		if (bResult)
		{
			CString					strNewFileName;

			strNewFileName.Format(_T("%s\\%s%s"), strSubFolder, szName, szExt);
			bResult = MoveFile(szFileName, strNewFileName);
		};

		if (bResult)
		{
			CString				strMsg;

			strMsg.Format(IDS_FILEMOVED, (LPCTSTR)strFileName, strSubFolder);
			PostToLog(strMsg, TRUE, FALSE, FALSE, RGB(128, 0, 0));
		}
		else
		{
			CString				strMsg;

			strMsg.Format(IDS_ERRORMOVINGFILE, (LPCTSTR)strFileName, strSubFolder);
			PostToLog(strMsg, TRUE, TRUE, FALSE, RGB(255, 0, 0));
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::IsImageStackable1(LPCTSTR szFile, double fStarCount, double fFWHM, double fScore, double fSkyBackground, CString & strError)
{
	BOOL						bResult = TRUE;

	if (m_LiveSettings.IsDontStack_Score())
	{
		if (fScore < m_LiveSettings.GetScore())
		{
			bResult = FALSE;
			strError.Format(IDS_NOSTACK_SCORE, fScore, m_LiveSettings.GetScore());
			PostChangeImageInfo(szFile, II_DONTSTACK_SCORE);
		};
	};

	if (m_LiveSettings.IsDontStack_Stars())
	{
		if (fStarCount < m_LiveSettings.GetStars())
		{
			bResult = FALSE;
			strError.Format(IDS_NOSTACK_STARS, fStarCount, (double)m_LiveSettings.GetStars());
			PostChangeImageInfo(szFile, II_DONTSTACK_STARS);
		};
	};

	if (m_LiveSettings.IsDontStack_FWHM())
	{
		if (fFWHM > m_LiveSettings.GetFWHM())
		{
			bResult = FALSE;
			strError.Format(IDS_NOSTACK_FWHM, fFWHM, (double)m_LiveSettings.GetFWHM());
			PostChangeImageInfo(szFile, II_DONTSTACK_FWHM);
		};
	};

	if (m_LiveSettings.IsWarning_SkyBackground())
	{
		if (fSkyBackground > m_LiveSettings.GetSkyBackground())
		{
			bResult = TRUE;
			strError.Format(IDS_NOSTACK_SKYBACKGROUND, fSkyBackground, (double)m_LiveSettings.GetSkyBackground());
			PostChangeImageInfo(szFile, II_DONTSTACK_SKYBACKGROUND);
		};
	};


	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CLiveEngine::IsImageStackable2(LPCTSTR szFile, double fdX, double fdY, double fAngle, CString & strError)
{
	BOOL						bResult = TRUE;

	if (m_LiveSettings.IsDontStack_Offset())
	{
		if ((fabs(fdX) > m_LiveSettings.GetOffset()) || (fabs(fdY) > m_LiveSettings.GetOffset()))
		{
			bResult = FALSE;
			strError.Format(IDS_NOSTACK_OFFSET, fdX, fdY, m_LiveSettings.GetOffset());
		};
		if (fabs(fdX) > m_LiveSettings.GetOffset())
			PostChangeImageInfo(szFile, II_DONTSTACK_DX);
		if (fabs(fdY) > m_LiveSettings.GetOffset())
			PostChangeImageInfo(szFile, II_DONTSTACK_DY);
	};

	if (bResult && m_LiveSettings.IsDontStack_Angle())
	{
		if (fabs(fAngle) > m_LiveSettings.GetAngle())
		{
			bResult = FALSE;
			strError.Format(IDS_NOSTACK_ANGLE, fAngle, (double)m_LiveSettings.GetAngle());
			PostChangeImageInfo(szFile, II_DONTSTACK_ANGLE);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::IsImageWarning1(LPCTSTR szFile, double fStarCount, double fFWHM, double fScore, double fSkyBackground, CString & strWarning)
{
	BOOL						bResult = FALSE;

	if (m_LiveSettings.IsWarning_Score())
	{
		if (fScore < m_LiveSettings.GetScore())
		{
			bResult = TRUE;
			strWarning.Format(IDS_NOSTACK_SCORE, fScore, m_LiveSettings.GetScore());
			PostChangeImageInfo(szFile, II_WARNING_SCORE);
		};
	};

	if (m_LiveSettings.IsWarning_Stars())
	{
		if (fStarCount < m_LiveSettings.GetStars())
		{
			bResult = TRUE;
			strWarning.Format(IDS_NOSTACK_STARS, fStarCount, (double)m_LiveSettings.GetStars());
			PostChangeImageInfo(szFile, II_WARNING_STARS);
		};
	};

	if (m_LiveSettings.IsWarning_FWHM())
	{
		if (fFWHM > m_LiveSettings.GetFWHM())
		{
			bResult = TRUE;
			strWarning.Format(IDS_NOSTACK_FWHM, fFWHM, (double)m_LiveSettings.GetFWHM());
			PostChangeImageInfo(szFile, II_WARNING_FWHM);
		};
	};

	if (m_LiveSettings.IsWarning_SkyBackground())
	{
		if (fSkyBackground > m_LiveSettings.GetSkyBackground())
		{
			bResult = TRUE;
			strWarning.Format(IDS_NOSTACK_SKYBACKGROUND, fSkyBackground, (double)m_LiveSettings.GetSkyBackground());
			PostChangeImageInfo(szFile, II_WARNING_SKYBACKGROUND);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::IsImageWarning2(LPCTSTR szFile, double fdX, double fdY, double fAngle, CString & strWarning)
{
	BOOL						bResult = FALSE;

	if (m_LiveSettings.IsWarning_Offset())
	{
		if ((fabs(fdX) > m_LiveSettings.GetOffset()) || (fabs(fdY) > m_LiveSettings.GetOffset()))
		{
			bResult = FALSE;
			strWarning.Format(IDS_NOSTACK_OFFSET, fdX, fdY, m_LiveSettings.GetOffset());
		};
		if (fabs(fdX) > m_LiveSettings.GetOffset())
		{
			bResult = TRUE;
			PostChangeImageInfo(szFile, II_WARNING_DX);
		};
		if (fabs(fdY) > m_LiveSettings.GetOffset())
		{
			bResult = TRUE;
			PostChangeImageInfo(szFile, II_WARNING_DY);
		};
	};

	if (m_LiveSettings.IsWarning_Angle())
	{
		if (fabs(fAngle) > m_LiveSettings.GetAngle())
		{
			bResult = TRUE;
			strWarning.Format(IDS_NOSTACK_ANGLE, fAngle, (double)m_LiveSettings.GetAngle());
			PostChangeImageInfo(szFile, II_WARNING_ANGLE);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CLiveEngine::IsFileAvailable(LPCTSTR szFileName)
{
	BOOL						bResult = FALSE;
	HANDLE						hFile;

	hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile)
	{
		bResult = TRUE;
		CloseHandle(hFile);
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

BOOL CLiveEngine::LoadFile(LPCTSTR szFileName)
{
	BOOL						bResult = FALSE;

	CBitmapInfo			bmpInfo;

	if (GetPictureInfo(szFileName, bmpInfo) && bmpInfo.CanLoad())
	{
		CString						strText;
		CString						strDescription;
		BOOL						bOverrideRAW = TRUE;

		bmpInfo.GetDescription(strDescription);
		if (bmpInfo.m_lNrChannels==3)
			strText.Format(IDS_LOADRGBLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFileName);
		else
			strText.Format(IDS_LOADGRAYLIGHT, bmpInfo.m_lBitPerChannel, (LPCTSTR)strDescription, szFileName);

		Start2(strText, 0);
		CAllDepthBitmap				adb;
		adb.SetDontUseAHD(TRUE);

		bResult = LoadPicture(szFileName, adb, this);
		End2();
		if (bResult)
		{
			PostFileLoaded(adb.m_pBitmap, adb.m_pWndBitmap, szFileName);
			PostChangeImageStatus(szFileName, IS_LOADED);

			// Now register the image
			CLightFrameInfo			lfi;

			strText.Format(IDS_REGISTERINGNAME, (LPCTSTR)szFileName);
			Start2(strText, 0);
			lfi.SetBitmap(szFileName, FALSE, FALSE);
			lfi.SetProgress(this);
			lfi.RegisterPicture(adb.m_pBitmap);
			lfi.SaveRegisteringInfo();
			lfi.m_lISOSpeed = bmpInfo.m_lISOSpeed;
			lfi.m_fExposure = bmpInfo.m_fExposure;
			End2();
			PostFileRegistered(szFileName);
			PostChangeImageStatus(szFileName, IS_REGISTERED);

			TCHAR					szName[_MAX_FNAME];
			TCHAR					szExt[_MAX_EXT];
			CString					strName;

			_tsplitpath(szFileName, NULL, NULL, szName, szExt);
			strName.Format(_T("%s%s"), szName, szExt);
			strText.Format(IDS_LOG_REGISTERRESULTS, (LPCTSTR)strName, lfi.m_vStars.size(), lfi.m_fFWHM, lfi.m_fOverallQuality);
			PostToLog(strText, TRUE);

			CString					strError;
			BOOL					bWarning;
			CString					strWarning;

			bWarning = IsImageWarning1(szFileName, lfi.m_vStars.size(), lfi.m_fFWHM, lfi.m_fOverallQuality, lfi.m_SkyBackground.m_fLight*100.0, strWarning);
			PostChangeImageInfo(szFileName, II_DONTSTACK_NONE);
			if (bWarning)
			{
				strText.Format(IDS_LOG_WARNING, (LPCTSTR)szFileName, (LPCTSTR) strWarning);
				PostToLog(strText, TRUE, FALSE, TRUE, RGB(208, 127, 0));
				PostWarning(strWarning);
			};
			if (IsImageStackable1(szFileName, lfi.m_vStars.size(), lfi.m_fFWHM, lfi.m_fOverallQuality, lfi.m_SkyBackground.m_fLight*100.0, strError))
			{
				// Check against stacking conditions before adding it to
				// the stack list
				m_qToStack.push_back(lfi);
			}
			else
			{
				strText.Format(IDS_LOG_IMAGENOTSTACKABLE1, (LPCTSTR)szFileName, (LPCTSTR) strError);
				PostToLog(strText, TRUE, TRUE, FALSE, RGB(255, 0, 0));
				PostChangeImageStatus(szFileName, IS_NOTSTACKABLE);
				MoveImage(szFileName);
			};
		}
		else
		{
			strText.Format(IDS_LOG_ERRORLOADINGFILE, szFileName);
			PostToLog(strText, TRUE, TRUE, FALSE, RGB(255, 0, 0)); 
			MoveImage(szFileName);
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CLiveEngine::SaveStackedImage(CMemoryBitmap * pBitmap)
{
	CSmartPtr<CMemoryBitmap>	pStackedImage;

	if (pBitmap)
		pStackedImage = pBitmap;
	else
		m_RunningStackingEngine.GetStackedImage(&pStackedImage);

	if (pStackedImage)
	{
		CString				strFolder;
		CString				strOutputFile;

		m_LiveSettings.GetStackedOutputFolder(strFolder);
		strOutputFile.Format(_T("%s\\Autostack.tif"), (LPCTSTR)strFolder);

		CString				strText;

		strText.Format(IDS_SAVINGSTACKEDIMAGE, (LPCTSTR)strFolder);
		this->Start2(strText, 0);

		WriteTIFF(strOutputFile, pStackedImage, this, _T("Autostacked Image"), 0, m_RunningStackingEngine.GetTotalExposure());
		this->End2();

		PostStackedImageSaved();
	};
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostFootprint(CPointExt pt1, CPointExt pt2, CPointExt pt3, CPointExt pt4)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetFootprint(pt1, pt2, pt3, pt4);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostStackedImage()
{
	CSmartPtr<CMemoryBitmap>	pStackedImage;

	m_RunningStackingEngine.GetStackedImage(&pStackedImage);

	// Transform the stacked image to a window one
	CSmartPtr<C32BitsBitmap>	pWndImage;

	pWndImage.Create();
	pWndImage->InitFrom(pStackedImage);

	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetStackedImage(pStackedImage, pWndImage, 
						  m_RunningStackingEngine.GetNrStackedImages(), 
						  m_RunningStackingEngine.GetTotalExposure());
	PostOutMessage(pMsg);

	m_lNrUnsavedImages++;
	if (m_LiveSettings.IsStack_Save() &&
		m_LiveSettings.GetSaveCount()<=m_lNrUnsavedImages)
	{
		// Save the stacked image in the output folder
		SaveStackedImage(pStackedImage);

		m_lNrUnsavedImages = 0;
	};
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::PostWarning(LPCTSTR szWarning)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetWarning(szWarning);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostStackedImageSaved()
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetStackedImageSaved();
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

BOOL CLiveEngine::ProcessNext()
{
	// Returns FALSE is there is nothing to do
	BOOL				bResult = FALSE;

	if (m_qToStack.size() && m_bStackingOn)
	{
		if (!m_bReferenceFrameSet)
		{
			if (m_LiveSettings.IsDontStack_Delayed() && m_qToStack.size() < m_LiveSettings.GetMinImages())
			{
				bResult = FALSE;
			}
			else
			{
				// Select the best reference frame from all the available images
				// (best score)
				std::deque<CLightFrameInfo>::iterator	it,
														bestit = m_qToStack.end();
				double									fMaxScore = 0;

				for (it = m_qToStack.begin();it != m_qToStack.end();it++)
				{
					if ((*it).m_fOverallQuality > fMaxScore)
					{
						bestit = it;
						fMaxScore = (*it).m_fOverallQuality;
					};
				};

				if (bestit != m_qToStack.end())
				{
					m_RunningStackingEngine.ComputeOffset((*bestit));
					PostUpdateImageOffsets((*bestit).m_strFileName, 0, 0, 0);
					m_RunningStackingEngine.AddImage((*bestit), this);
					PostChangeImageStatus((*bestit).m_strFileName, IS_STACKED);
					PostChangeImageInfo((*bestit).m_strFileName, II_SETREFERENCE);
					m_qToStack.erase(bestit);
					m_bReferenceFrameSet = TRUE;
					PostStackedImage();
				}
				else
					m_qToStack.clear();
				bResult = TRUE;
			};
		}
		else
		{
			CLightFrameInfo			lfi;
			double					fdX, fdY, fAngle;
			CString					strError;
			CString					strText;
			BOOL					bError = FALSE;
			BOOL					bWarning = FALSE;
			CString					strWarning;

			lfi = m_qToStack.front();
			m_qToStack.pop_front();
			if (m_RunningStackingEngine.ComputeOffset(lfi))
			{
				lfi.m_BilinearParameters.Offsets(fdX, fdY);
				fAngle = lfi.m_BilinearParameters.Angle(lfi.RenderedWidth()) * 180.0/M_PI;
				PostUpdateImageOffsets(lfi.m_strFileName, fdX, fdY, fAngle);
				PostChangeImageInfo(lfi.m_strFileName, II_DONTSTACK_NONE);
				bWarning = IsImageWarning2(lfi.m_strFileName, fdX, fdY, fAngle, strWarning);
				if (bWarning)
					PostWarning(strWarning);
				if (IsImageStackable2(lfi.m_strFileName, fdX, fdY, fAngle, strError))
				{
					m_RunningStackingEngine.AddImage(lfi, this);
					PostChangeImageStatus(lfi.m_strFileName, IS_STACKED);

					CPointExt		pt1, pt2, pt3, pt4;

					lfi.m_BilinearParameters.Footprint(pt1, pt2, pt3, pt4);
					PostFootprint(pt1, pt2, pt3, pt4);
					PostStackedImage();
				}
				else
				{
					bError = TRUE;
				};
			}
			else
			{
				// Can't find transformation - impossible to stack the image
				strError.LoadString(IDS_NOSTACK_NOTRANSFORMATION);
				bError = TRUE;
			};

			if (bWarning)
			{
				strText.Format(IDS_LOG_WARNING, (LPCTSTR)lfi.m_strFileName, (LPCTSTR) strWarning);
				PostToLog(strText, TRUE, FALSE, TRUE, RGB(208, 127, 0));
			};
			if (bError)
			{
				strText.Format(IDS_LOG_IMAGENOTSTACKABLE1, (LPCTSTR)lfi.m_strFileName, (LPCTSTR) strError);
				PostToLog(strText, TRUE, TRUE, FALSE, RGB(255, 0, 0));
				PostChangeImageStatus(lfi.m_strFileName, IS_NOTSTACKABLE);
				MoveImage(lfi.m_strFileName);
			};

			bResult = TRUE;
		};
	}
	if (m_bRegisteringOn && !bResult && m_qToRegister.size())
	{
		CString			strFileName;

		strFileName = m_qToRegister.front();
		m_qToRegister.pop_front();
		PostUpdatePending();
		if (IsFileAvailable(strFileName))
			LoadFile(strFileName);
		else
			m_qToRegister.push_back(strFileName);
		bResult = TRUE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void CLiveEngine::LiveEngine()
{
	BOOL				bEnd = FALSE;
	MSG					msg;

	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
	SetEvent(m_hEvent);
	while (!bEnd && ::GetMessage(&msg, NULL, 0, 0))
	{
		// Check for settings update
		if (msg.message == WM_LE_MESSAGE)
		{
			CSmartPtr<CLiveEngineMsg>	pMsg;
			if (GetMessage(&pMsg, m_InMessages))
			{
				// Do the stuff
				switch (pMsg->GetMessage())
				{
				case LEM_NEWFILE :
					{
						CString				strFileName;

						if (pMsg->GetNewFile(strFileName))
						{
							// Add the file to the to do list
							m_qToRegister.push_back(strFileName);
							PostUpdatePending();
						};
					};
					break;
				case LEM_UPDATESETTINGS :
					m_LiveSettings.LoadFromRegistry();
					break;
				case LEM_SAVESTACKEDIMAGE :
					SaveStackedImage();
					break;
				case LEM_ENABLESTACKING :
					m_bRegisteringOn = TRUE;
					m_bStackingOn = TRUE;
					break;
				case LEM_DISABLESTACKING :
					m_bStackingOn = FALSE;
					break;
				case LEM_ENABLEREGISTERING :
					m_bRegisteringOn = TRUE;
					break;
				case LEM_DISABLEREGISTERING :
					m_bRegisteringOn = FALSE;
					m_bStackingOn    = FALSE;
					break;
				case LEM_CLEARSTACKEDIMAGE :
					m_RunningStackingEngine.Clear();
					m_bReferenceFrameSet = FALSE;
					PostStackedImage();
					break;
				case LEM_CLEARPENDINGIMAGES :
					m_qToRegister.clear();
					m_qToStack.clear();
					PostUpdatePending();
					break;
				case LEM_STOP :
					m_bRegisteringOn = FALSE;
					m_bStackingOn    = FALSE;
					bEnd = TRUE;
					break;
				};
			};

			while (!bEnd && !::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) && ProcessNext());
			SetEvent(m_hEvent);
		};
	};
};

/* ------------------------------------------------------------------- */

void CLiveEngine::StartEngine()
{
	if (!m_hThread)
	{
		// Create the thread
		m_hEvent	= CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_hEvent)
		{
			m_hThread = CreateThread(NULL, 0, LiveEngineThreadProc, (LPVOID)this, CREATE_SUSPENDED, &m_dwThreadID);
			if (m_hThread)
			{
				SetThreadPriority(m_hThread, THREAD_PRIORITY_BELOW_NORMAL);
				ResetEvent(m_hEvent);
				ResumeThread(m_hThread);
				WaitForMultipleObjects(1, &m_hEvent, TRUE, INFINITE);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CLiveEngine::CloseEngine()
{
	if (m_hThread)
	{
		m_CriticalSection.Lock();
		m_InMessages.clear();
		m_CriticalSection.Unlock();
		WaitForMultipleObjects(1, &m_hEvent, TRUE, INFINITE);

		CSmartPtr<CLiveEngineMsg>		pMsg;

		pMsg.Create();
		pMsg->SetStop();
		m_CriticalSection.Lock();
		m_InMessages.push_front(pMsg);
		m_CriticalSection.Unlock();

		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
		WaitForMultipleObjects(1, &m_hThread, TRUE, INFINITE);

		CloseHandle(m_hThread);
		CloseHandle(m_hEvent);
		m_hThread = NULL;
		m_hEvent  = NULL;
	};
};

/* ------------------------------------------------------------------- */

BOOL CLiveEngine::GetMessage(CLiveEngineMsg ** ppMsg, LIVEENGINEMSGLIST & msglist)
{
	BOOL			bResult = FALSE;

	if (ppMsg)
		*ppMsg = NULL;

	m_CriticalSection.Lock();
	if (msglist.size())
	{
		CSmartPtr<CLiveEngineMsg>	pMsg;
		pMsg = msglist.front();

		// Remove the first item
		msglist.pop_front();

		bResult = TRUE;
		pMsg.CopyTo(ppMsg);
	};

	m_CriticalSection.Unlock();

	return bResult;
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostOutMessage(CLiveEngineMsg * pMsg)
{
	m_CriticalSection.Lock();
	m_OutMessages.push_back(pMsg);
	m_CriticalSection.Unlock();
	if (m_hWndOut)
		PostMessage(m_hWndOut, WM_LIVEENGINE, 0, 0);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostToLog(LPCTSTR szText, BOOL bDateTime, BOOL bBold, BOOL bItalic, COLORREF crColor)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetLog(szText, bDateTime, bBold, bItalic, crColor);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostProgress(LPCTSTR szText, LONG lAchieved, LONG lTotal)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetProgress(szText, lAchieved, lTotal);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostEndProgress()
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetEndProgress();
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostFileLoaded(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap, LPCTSTR szFileName)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetFileLoaded(pBitmap, pWndBitmap, szFileName);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostFileRegistered(LPCTSTR szFileName)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetRegisteredImage(szFileName);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostChangeImageStatus(LPCTSTR szFileName, IMAGESTATUS status)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetImageStatus(szFileName, status);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::PostChangeImageInfo(LPCTSTR szFileName, STACKIMAGEINFO info)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetImageInfo(szFileName, info);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostUpdateImageOffsets(LPCTSTR szFileName, double fdX, double fdY, double fAngle)
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetImageOffsets(szFileName, fdX, fdY, fAngle);
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::PostUpdatePending()
{
	CSmartPtr<CLiveEngineMsg>	pMsg;

	pMsg.Create();
	pMsg->SetPending((LONG)(m_qToRegister.size()));
	PostOutMessage(pMsg);
};

/* ------------------------------------------------------------------- */

CLiveEngine::CLiveEngine()
{
	m_hWndOut			= NULL;
	m_hThread			= NULL;
	m_dwThreadID		= NULL;
	m_hEvent			= NULL;
	m_bStackingOn		= FALSE;
	m_bRegisteringOn	= TRUE;
	m_bReferenceFrameSet = FALSE;
	m_lNrUnsavedImages   = 0;
	m_LiveSettings.LoadFromRegistry();
};

/* ------------------------------------------------------------------- */

CLiveEngine::~CLiveEngine()
{
	CloseEngine();
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::GetMessage(CLiveEngineMsg ** ppMsg)
{
	return GetMessage(ppMsg, m_OutMessages);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::AddFileToProcess(LPCTSTR szFile)
{
	CSmartPtr<CLiveEngineMsg>		pMsg;

	pMsg.Create();
	pMsg->SetNewFile(szFile);
	m_CriticalSection.Lock();
	m_InMessages.push_back(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::UpdateSettings()
{
	CSmartPtr<CLiveEngineMsg>		pMsg;

	pMsg.Create();
	pMsg->SetUpdateSettings();
	m_CriticalSection.Lock();
	m_InMessages.push_front(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::EnableRegistering(BOOL bEnable)
{
	CSmartPtr<CLiveEngineMsg>		pMsg;
	BOOL							bRemoveEnable;

	pMsg.Create();
	if (bEnable)
	{
		pMsg->SetEnableRegistering();
		bRemoveEnable = FALSE;
	}
	else
	{
		pMsg->SetDisableRegistering();
		bRemoveEnable = TRUE;
	};
	m_CriticalSection.Lock();
	{
		LIVEENGINEMSGITERATOR			it;
		BOOL							bFound = FALSE;

		// Remove enable or disable messages already in the queue
		do
		{
			bFound = FALSE;

			for (it = m_InMessages.begin();it!=m_InMessages.end() && !bFound;it++)
			{
				bFound = (((*it)->GetMessage() == LEM_DISABLEREGISTERING) && !bRemoveEnable) ||
						 (((*it)->GetMessage() == LEM_ENABLEREGISTERING) && bRemoveEnable);

				if (bFound)
					m_InMessages.erase(it);
			};
		}
		while (bFound);
	};
	m_InMessages.push_front(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void CLiveEngine::EnableStacking(BOOL bEnable)
{
	CSmartPtr<CLiveEngineMsg>		pMsg;
	BOOL							bRemoveEnable;

	pMsg.Create();
	if (bEnable)
	{
		pMsg->SetEnableStacking();
		bRemoveEnable = FALSE;
	}
	else
	{
		pMsg->SetDisableStacking();
		bRemoveEnable = TRUE;
	};
	m_CriticalSection.Lock();
	{
		LIVEENGINEMSGITERATOR			it;
		BOOL							bFound = FALSE;

		// Remove enable or disable messages already in the queue
		do
		{
			bFound = FALSE;

			for (it = m_InMessages.begin();it!=m_InMessages.end() && !bFound;it++)
			{
				bFound = (((*it)->GetMessage() == LEM_DISABLESTACKING) && !bRemoveEnable) ||
						 (((*it)->GetMessage() == LEM_ENABLESTACKING) && bRemoveEnable);

				if (bFound)
					m_InMessages.erase(it);
			};
		}
		while (bFound);
	};

	m_InMessages.push_front(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::PostSaveStackedImage()
{
	CSmartPtr<CLiveEngineMsg>		pMsg;

	pMsg.Create();
	pMsg->SetSaveStackedImage();
	m_CriticalSection.Lock();
	m_InMessages.push_front(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::ClearStackedImage()
{
	CSmartPtr<CLiveEngineMsg>		pMsg;

	pMsg.Create();
	pMsg->SetClearStackedImage();
	m_CriticalSection.Lock();
	m_InMessages.push_back(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::ClearPendingImages()
{
	CSmartPtr<CLiveEngineMsg>		pMsg;

	pMsg.Create();
	pMsg->SetClearPendingImages();
	m_CriticalSection.Lock();
	m_InMessages.push_back(pMsg);
	m_CriticalSection.Unlock();
	StartEngine();
	if (m_hThread && m_dwThreadID)
		PostThreadMessage(m_dwThreadID, WM_LE_MESSAGE, 0, 0);
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::ClearAll()
{
	ClearStackedImage();
	ClearPendingImages();
};

/* ------------------------------------------------------------------- */
// DSSProgress methods

void	CLiveEngine::GetStartText(CString & strText)
{
	strText = m_strProgress1;
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::GetStart2Text(CString & strText)
{
	strText = m_strProgress2;
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::Start(LPCTSTR szTitle, LONG lTotal1, BOOL bEnableCancel)
{
	CString			strText = szTitle;

	if (strText.GetLength())
	{
		m_strProgress1 = szTitle;
		strText.Replace(_T("\n"), _T(" "));
		strText += "\n";
		PostToLog(strText, TRUE);
	};
	if (lTotal1)
		m_lTotal1      = lTotal1;
	m_lAchieved1   = 0;
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::Progress1(LPCTSTR szText, LONG lAchieved1)
{
	if (szText)
		m_strProgress1 = szText;
	if (((double)(lAchieved1-m_lAchieved1)/(double)m_lTotal1) > 0.10)
	{
		PostProgress(m_strProgress1, lAchieved1, m_lTotal1);
		m_lAchieved1 = lAchieved1;
	};
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::Start2(LPCTSTR szText, LONG lTotal2)
{
	CString			strText = szText;

	if (strText.GetLength())
	{
		m_strProgress2 = szText;
		strText.Replace(_T("\n"), _T(" "));
		strText += "\n";
		PostToLog(strText, TRUE);
	};
	if (lTotal2)
		m_lTotal2      = lTotal2;
	m_lAchieved2   = 0;
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::Progress2(LPCTSTR szText, LONG lAchieved2)
{
	if (szText)
		m_strProgress2 = szText;
	if ((((double)(lAchieved2-m_lAchieved2)/(double)m_lTotal2) > 0.10) ||
		 (lAchieved2 == m_lTotal2))
	{
		PostProgress(m_strProgress2, lAchieved2, m_lTotal2);
		m_lAchieved2 = lAchieved2;
	};
};

/* ------------------------------------------------------------------- */

void	CLiveEngine::End2()
{
	PostEndProgress();
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::IsCanceled()
{
	return FALSE;
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::Close()
{
	PostEndProgress();
	return TRUE;
};

/* ------------------------------------------------------------------- */

BOOL	CLiveEngine::Warning(LPCTSTR szText)
{
	return TRUE;
};

/* ------------------------------------------------------------------- */
