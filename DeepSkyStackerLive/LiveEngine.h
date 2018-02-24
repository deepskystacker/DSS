#ifndef __LIVEENGINE_H__
#define __LIVEENGINE_H__

#include <queue>
#include <list>
#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "LiveSettings.h"
#include "RegisterEngine.h"
#include "RunningStackingEngine.h"

/* ------------------------------------------------------------------- */

const DWORD						WM_LIVEENGINE	= WM_USER + 200;

typedef enum tagLIVEENGINEMSG
{
	LEM_NONE					= 0,
	LEM_NEWFILE					= 1,
	LEM_STARTPROGRESS			= 2,
	LEM_PROGRESSING				= 3,
	LEM_ENDPROGRESS				= 4,
	LEM_FILELOADED				= 6,
	LEM_FILEREGISTERED			= 7,
	LEM_SETSTACKEDIMAGE			= 8,
	LEM_ADDTOLOG				= 9,
	LEM_UPDATESETTINGS			= 10,
	LEM_STOP					= 11,
	LEM_CHANGEIMAGESTATUS		= 12,
	LEM_ENABLESTACKING			= 13,
	LEM_DISABLESTACKING			= 14,
	LEM_UPDATEIMAGEOFFSETS		= 15,
	LEM_UPDATEIMAGEINFO			= 16,
	LEM_UPDATEPENDING			= 17,
	LEM_WARNING					= 18,
	LEM_ENABLEREGISTERING		= 19,
	LEM_DISABLEREGISTERING		= 20,
	LEM_SAVESTACKEDIMAGE		= 21,
	LEM_STACKEDIMAGESAVED		= 22,
	LEM_CLEARSTACKEDIMAGE		= 23,
	LEM_CLEARPENDINGIMAGES		= 24,
	LEM_SETFOOTPRINT			= 25
}LIVEENGINEMSG;

typedef enum tagIMAGESTATUS
{
	IS_LOADED					= 1,
	IS_REGISTERED				= 2,
	IS_STACKDELAYED				= 3,
	IS_NOTSTACKABLE				= 4,
	IS_STACKED					= 5
}IMAGESTATUS;

typedef enum tagSTACKIMAGEINFO
{
	II_SETREFERENCE				= 0,
	II_DONTSTACK_NONE			= 1,
	II_DONTSTACK_SCORE			= 2,
	II_DONTSTACK_STARS			= 3,
	II_DONTSTACK_FWHM			= 4,
	II_DONTSTACK_DX				= 5,
	II_DONTSTACK_DY				= 6,
	II_DONTSTACK_ANGLE			= 7,
	II_DONTSTACK_SKYBACKGROUND	= 8,
	II_WARNING_NONE				= 101,
	II_WARNING_SCORE			= 102,
	II_WARNING_STARS			= 103,
	II_WARNING_FWHM				= 104,
	II_WARNING_DX				= 105,
	II_WARNING_DY				= 106,
	II_WARNING_ANGLE			= 107,
	II_WARNING_SKYBACKGROUND	= 108
}STACKIMAGEINFO;

/* ------------------------------------------------------------------- */

class CLiveEngineMsg : public CRefCount
{
private :
	LIVEENGINEMSG				m_Msg;
	CString						m_strFile;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CSmartPtr<C32BitsBitmap>	m_pWndBitmap;
	CString						m_strText;
	LONG						m_lMax;
	LONG						m_lProgress;

	BOOL						m_bLogDateTime;
	BOOL						m_bLogBold;
	BOOL						m_bLogItalic;
	COLORREF					m_crLogColor;

	LONG						m_lProgressAchieved;
	LONG						m_lProgressTotal;

	LONG						m_lNrStacked;
	double						m_fExposure;

	IMAGESTATUS					m_ImageStatus;
	double						m_fdX,
								m_fdY,
								m_fAngle;
	CPointExt					m_ptFootprint[4];

	STACKIMAGEINFO				m_ImageInfo;
	LONG						m_lNrPending;
	
public :
	CLiveEngineMsg()
	{
		m_Msg = LEM_NONE;
		m_lMax = 0;
		m_lProgress = 0;
	};

	~CLiveEngineMsg()
	{
	};

	LIVEENGINEMSG	GetMessage()
	{
		return m_Msg;
	};

	void	SetNewFile(LPCTSTR szFile)
	{
		m_Msg = LEM_NEWFILE;
		m_strFile = szFile;
	};

	void	SetUpdateSettings()
	{
		m_Msg = LEM_UPDATESETTINGS;
	};
	void	SetStop()
	{
		m_Msg = LEM_STOP;
	};
	void	SetEnableStacking()
	{
		m_Msg = LEM_ENABLESTACKING;
	};
	void	SetDisableStacking()
	{
		m_Msg = LEM_DISABLESTACKING;
	};
	void	SetEnableRegistering()
	{
		m_Msg = LEM_ENABLEREGISTERING;
	};
	void	SetDisableRegistering()
	{
		m_Msg = LEM_DISABLEREGISTERING;
	};

	void	SetSaveStackedImage()
	{
		m_Msg = LEM_SAVESTACKEDIMAGE;
	};

	void	SetStackedImageSaved()
	{
		m_Msg = LEM_STACKEDIMAGESAVED;
	};

	void	SetClearStackedImage()
	{
		m_Msg = LEM_CLEARSTACKEDIMAGE;
	};

	void	SetClearPendingImages()
	{
		m_Msg = LEM_CLEARPENDINGIMAGES;
	};

	void	SetRegisteredImage(LPCTSTR szFile)
	{
		m_Msg = LEM_FILEREGISTERED;
		m_strFile = szFile;
	};

	void	SetStackedImage(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap, LONG lNrStacked, double fExposure)
	{
		m_Msg = LEM_SETSTACKEDIMAGE;
		m_pBitmap = pBitmap;
		m_pWndBitmap = pWndBitmap;
		m_lNrStacked = lNrStacked;
		m_fExposure  = fExposure;
	};

	void	SetImageOffsets(LPCTSTR szFile, double fdX, double fdY, double fAngle)
	{
		m_Msg = LEM_UPDATEIMAGEOFFSETS;
		m_strFile	= szFile;
		m_fdX		= fdX;
		m_fdY		= fdY;
		m_fAngle	= fAngle;
	};

	void	SetFootprint(CPointExt pt1, CPointExt pt2, CPointExt pt3, CPointExt pt4)
	{
		m_Msg = LEM_SETFOOTPRINT;
		m_ptFootprint[0] = pt1;
		m_ptFootprint[1] = pt2;
		m_ptFootprint[2] = pt3;
		m_ptFootprint[3] = pt4;
	};

	void	SetImageStatus(LPCTSTR szFile, IMAGESTATUS status)
	{
		m_Msg = LEM_CHANGEIMAGESTATUS;
		m_ImageStatus = status;
		m_strFile = szFile;
	};

	void	SetImageInfo(LPCTSTR szFile, STACKIMAGEINFO info)
	{
		m_Msg = LEM_UPDATEIMAGEINFO;
		m_ImageInfo = info;
		m_strFile = szFile;
	};

	void	SetLog(LPCTSTR szText, BOOL bDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0))
	{
		m_Msg = LEM_ADDTOLOG;
		m_strText		= szText;
		m_bLogDateTime	= bDateTime;
		m_bLogBold		= bBold;
		m_bLogItalic	= bItalic;
		m_crLogColor	= crColor;
	};

	void	SetProgress(LPCTSTR szText, LONG lAchieved, LONG lTotal)
	{
		m_Msg = LEM_PROGRESSING;
		m_strText			= szText;
		m_lProgressAchieved = lAchieved;
		m_lProgressTotal    = lTotal;
	};

	void	SetEndProgress()
	{
		m_Msg = LEM_ENDPROGRESS;
	};

	void	SetFileLoaded(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap, LPCTSTR szFileName)
	{
		m_Msg			= LEM_FILELOADED;
		m_pBitmap		= pBitmap;
		m_pWndBitmap	= pWndBitmap;
		m_strFile		= szFileName;
	};

	void	SetPending(LONG lNrPending)
	{
		m_Msg			= LEM_UPDATEPENDING;
		m_lNrPending	= lNrPending;
	};

	void	SetWarning(LPCTSTR szWarning)
	{
		m_Msg			= LEM_WARNING;
		m_strText		= szWarning;
	};

	BOOL	GetNewFile(CString & strFileName)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_NEWFILE)
		{
			strFileName = m_strFile;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetRegisteredImage(CString & strFileName)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_FILEREGISTERED)
		{
			strFileName = m_strFile;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetImageOffsets(CString & strFileName, double & fdX, double & fdY, double & fAngle)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_UPDATEIMAGEOFFSETS)
		{
			strFileName = m_strFile;
			fdX			= m_fdX;
			fdY			= m_fdY;
			fAngle		= m_fAngle;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetFootprint(CPointExt & pt1, CPointExt & pt2, CPointExt & pt3, CPointExt & pt4)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_SETFOOTPRINT)
		{
			pt1 = m_ptFootprint[0];
			pt2 = m_ptFootprint[1];
			pt3 = m_ptFootprint[2];
			pt4 = m_ptFootprint[3];
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetImageStatus(CString & strFileName, IMAGESTATUS & status)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_CHANGEIMAGESTATUS)
		{
			strFileName = m_strFile;
			status		= m_ImageStatus;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetImage(CMemoryBitmap ** ppBitmap, C32BitsBitmap ** ppWndBitmap, CString & strFileName)
	{
		BOOL			bResult = FALSE;

		if (ppBitmap)
			*ppBitmap = NULL;
		if (ppWndBitmap)
			*ppWndBitmap = NULL;

		if (m_Msg == LEM_FILELOADED)
		{
			m_pBitmap.CopyTo(ppBitmap);
			m_pWndBitmap.CopyTo(ppWndBitmap);
			strFileName = m_strFile;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetStackedImage(CMemoryBitmap ** ppBitmap, C32BitsBitmap ** ppWndBitmap, LONG & lNrStacked, double & fExposure)
	{
		BOOL			bResult = FALSE;

		if (ppBitmap)
			*ppBitmap = NULL;
		if (ppWndBitmap)
			*ppWndBitmap = NULL;

		if (m_Msg == LEM_SETSTACKEDIMAGE)
		{
			m_pBitmap.CopyTo(ppBitmap);
			m_pWndBitmap.CopyTo(ppWndBitmap);
			lNrStacked = m_lNrStacked;
			fExposure  = m_fExposure;
			bResult = TRUE;
		};

		return bResult;
	};


	BOOL	GetImageInfo(CString & strFile, STACKIMAGEINFO & info)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_UPDATEIMAGEINFO)
		{
			bResult = TRUE;
			strFile = m_strFile;
			info	= m_ImageInfo;
		};

		return bResult;
	};

	BOOL	GetLog(CString & strText, BOOL & bDateTime, BOOL & bBold, BOOL & bItalic, COLORREF & crColor)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_ADDTOLOG)
		{
			strText		= m_strText;
			bDateTime	= m_bLogDateTime;
			bBold		= m_bLogBold;
			bItalic		= m_bLogItalic;
			crColor		= m_crLogColor;
			bResult		= TRUE;
		};

		return bResult;
	};

	BOOL	GetProgress(CString & strText, LONG & lAchieved, LONG & lTotal)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_PROGRESSING)
		{
			strText		= m_strText;
			lAchieved	= m_lProgressAchieved;
			lTotal		= m_lProgressTotal;
			bResult		= TRUE;
		};

		return bResult;
	};

	BOOL	GetPending(LONG & lNrPending)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_UPDATEPENDING)
		{
			lNrPending = m_lNrPending;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetWarning(CString & strWarning)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_WARNING)
		{
			strWarning = m_strText;
			bResult = TRUE;
		};

		return bResult;
	};
};

typedef CSmartPtr<CLiveEngineMsg>				LIVEENGINEMSGPTR;
typedef std::deque<LIVEENGINEMSGPTR >			LIVEENGINEMSGLIST;
typedef LIVEENGINEMSGLIST::iterator				LIVEENGINEMSGITERATOR;

/* ------------------------------------------------------------------- */

class CLiveEngine : public CDSSProgress
{
private :
	CComAutoCriticalSection		m_CriticalSection;
	LIVEENGINEMSGLIST			m_InMessages;
	LIVEENGINEMSGLIST			m_OutMessages;
	HWND						m_hWndOut;
	HANDLE						m_hThread;
	DWORD						m_dwThreadID;
	HANDLE						m_hEvent;
	CLiveSettings				m_LiveSettings;
	std::deque<CString>			m_qToRegister;	
	std::deque<CLightFrameInfo>	m_qToStack;	
	BOOL						m_bStackingOn;
	BOOL						m_bRegisteringOn;
	BOOL						m_bReferenceFrameSet;
	CRunningStackingEngine		m_RunningStackingEngine;
	LONG						m_lNrUnsavedImages;
	
private :
	void	StartEngine();
	void	CloseEngine();
	BOOL	GetMessage(CLiveEngineMsg ** ppMsg, LIVEENGINEMSGLIST & msglist);
	void	PostOutMessage(CLiveEngineMsg * pMsg);
	void	PostToLog(LPCTSTR szText, BOOL bDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0));
	void	PostProgress(LPCTSTR szText, LONG lAchieved, LONG lTotal);
	void	PostEndProgress();
	void	PostUpdatePending();
	void	PostFileLoaded(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap, LPCTSTR szFileName);
	void	PostFileRegistered(LPCTSTR szFileName);
	void	PostChangeImageStatus(LPCTSTR szFileName, IMAGESTATUS status);
	void	PostChangeImageInfo(LPCTSTR szFileName, STACKIMAGEINFO info);
	void	PostUpdateImageOffsets(LPCTSTR szFileName, double fdX, double fdY, double fAngle);
	void	SaveStackedImage(CMemoryBitmap * pBitmap = NULL);
	void	PostFootprint(CPointExt pt1, CPointExt pt2, CPointExt pt3, CPointExt pt4);
	void	PostStackedImage();
	void	PostStackedImageSaved();
	void	PostWarning(LPCTSTR szWarning);
	BOOL	IsImageStackable1(LPCTSTR szFile, double fStarCount, double fFWHM, double fScore, double fSkyBackground, CString & strError);
	BOOL	IsImageStackable2(LPCTSTR szFile, double fdX, double fdY, double fAngle, CString & strError);
	BOOL	IsImageWarning1(LPCTSTR szFile, double fStarCount, double fFWHM, double fScore, double fSkyBackground, CString & strWarning);
	BOOL	IsImageWarning2(LPCTSTR szFile, double fdX, double fdY, double fAngle, CString & strWarning);
	BOOL	LoadFile(LPCTSTR szFileName);
	BOOL	ProcessNext();
	void	MoveImage(LPCTSTR szFileName);

public :
	CLiveEngine();
	virtual ~CLiveEngine();

	void	LiveEngine();

	void	SetWindow(HWND hWnd)
	{
		m_hWndOut = hWnd;
	};

	BOOL	IsFileAvailable(LPCTSTR szFileName);

	BOOL	GetMessage(CLiveEngineMsg ** ppMsg);	
	void	AddFileToProcess(LPCTSTR szFile);

	void	UpdateSettings();
	void	EnableRegistering(BOOL bEnable = TRUE);
	void	EnableStacking(BOOL bEnable = TRUE);
	void	PostSaveStackedImage();

	void	ClearStackedImage();
	void	ClearPendingImages();
	void	ClearAll();

// DSSProgress methods
private :
	CString						m_strProgress1;
	CString						m_strProgress2;
	LONG						m_lTotal1,
								m_lTotal2;
	LONG						m_lAchieved1,
								m_lAchieved2;

public :
	virtual void	GetStartText(CString & strText);
	virtual void	GetStart2Text(CString & strText);
	virtual	void	Start(LPCTSTR szTitle, LONG lTotal1, BOOL bEnableCancel = TRUE);
	virtual void	Progress1(LPCTSTR szText, LONG lAchieved1);
	virtual void	Start2(LPCTSTR szText, LONG lTotal2);
	virtual void	Progress2(LPCTSTR szText, LONG lAchieved2);
	virtual void	End2();
	virtual BOOL	IsCanceled();
	virtual BOOL	Close();
	virtual BOOL	Warning(LPCTSTR szText);
};

/* ------------------------------------------------------------------- */

#endif // __LIVEENGINE_H__