#pragma once
#include "DSSProgress.h"
#include "LiveSettings.h"
#include "RegisterEngine.h"
#include "RunningStackingEngine.h"

class CMemoryBitmap;
class C32BitsBitmap;
using namespace DSS;

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

class CLiveEngineMsg
{
private :
	LIVEENGINEMSG				m_Msg;
	CString						m_strFile;
	std::shared_ptr<CMemoryBitmap>	m_pBitmap;
	std::shared_ptr<C32BitsBitmap>	m_pWndBitmap;
	QString						m_strText;
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
	QPointF					m_ptFootprint[4];

	STACKIMAGEINFO				m_ImageInfo;
	LONG						m_lNrPending;

public :
    CLiveEngineMsg()
    {
        m_Msg = LEM_NONE;
        m_lMax = 0;
        m_lProgress = 0;
        m_Msg = LIVEENGINEMSG(0);
        m_lMax = 0;
        m_lProgress = 0;
        m_bLogDateTime = false;
        m_bLogBold = false;
        m_bLogItalic = false;
        m_crLogColor = 0;
        m_lProgressAchieved = 0;
        m_lProgressTotal = 0;
        m_lNrStacked = 0;
        m_fExposure = 0;
        m_ImageStatus = IMAGESTATUS(0);
        m_fdX = 0;
        m_fdY = 0;
        m_fAngle = 0;
        m_ImageInfo = STACKIMAGEINFO(0);
        m_lNrPending = 0;
    }

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

	void	SetStackedImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LONG lNrStacked, double fExposure)
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

	void	SetFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4)
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

	void	SetLog(const QString& text, BOOL bDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0))
	{
		m_Msg = LEM_ADDTOLOG;
		m_strText		= text;
		m_bLogDateTime	= bDateTime;
		m_bLogBold		= bBold;
		m_bLogItalic	= bItalic;
		m_crLogColor	= crColor;
	};

	void	SetProgress(const QString& text, LONG lAchieved, LONG lTotal)
	{
		m_Msg = LEM_PROGRESSING;
		m_strText			= text;
		m_lProgressAchieved = lAchieved;
		m_lProgressTotal    = lTotal;
	};

	void	SetEndProgress()
	{
		m_Msg = LEM_ENDPROGRESS;
	};

	void	SetFileLoaded(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LPCTSTR szFileName)
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

	void	SetWarning(const QString& warning)
	{
		m_Msg			= LEM_WARNING;
		m_strText		= warning;
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

	BOOL	GetFootprint(QPointF & pt1, QPointF & pt2, QPointF & pt3, QPointF & pt4)
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

	BOOL	GetImage(std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<C32BitsBitmap>& pWndBitmap, CString& strFileName)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_FILELOADED)
		{
			pBitmap = m_pBitmap;
			pWndBitmap = m_pWndBitmap;
			strFileName = m_strFile;
			bResult = TRUE;
		};

		return bResult;
	};

	BOOL	GetStackedImage(std::shared_ptr<CMemoryBitmap>& pBitmap, std::shared_ptr<C32BitsBitmap>& pWndBitmap, LONG & lNrStacked, double & fExposure)
	{
		BOOL			bResult = FALSE;

		if (m_Msg == LEM_SETSTACKEDIMAGE)
		{
			pBitmap = m_pBitmap;
			pWndBitmap = m_pWndBitmap;
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

	bool GetLog(QString & text, bool & bDateTime, bool & bBold, bool & bItalic, COLORREF & crColor)
	{
		bool bResult = false;

		if (m_Msg == LEM_ADDTOLOG)
		{
			text		= m_strText;
			bDateTime	= m_bLogDateTime;
			bBold		= m_bLogBold;
			bItalic		= m_bLogItalic;
			crColor		= m_crLogColor;
			bResult		= true;
		};

		return bResult;
	};

	bool	GetProgress(QString & strText, LONG & lAchieved, LONG & lTotal)
	{
		bool bResult = false;

		if (m_Msg == LEM_PROGRESSING)
		{
			strText		= m_strText;
			lAchieved	= m_lProgressAchieved;
			lTotal		= m_lProgressTotal;
			bResult		= true;
		};

		return bResult;
	};

	bool	GetPending(long & lNrPending)
	{
		bool bResult = false;

		if (m_Msg == LEM_UPDATEPENDING)
		{
			lNrPending = m_lNrPending;
			bResult = true;
		};

		return bResult;
	};

	bool	GetWarning(QString & strWarning)
	{
		bool bResult = false;

		if (m_Msg == LEM_WARNING)
		{
			strWarning = m_strText;
			bResult = false;
		};

		return bResult;
	};
};

typedef std::shared_ptr<CLiveEngineMsg>			LIVEENGINEMSGPTR;
typedef std::deque<LIVEENGINEMSGPTR >			LIVEENGINEMSGLIST;
typedef LIVEENGINEMSGLIST::iterator				LIVEENGINEMSGITERATOR;

/* ------------------------------------------------------------------- */

class CLiveEngine : public ProgressBase
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
	BOOL	GetMessage(std::shared_ptr<CLiveEngineMsg>& rMsg, LIVEENGINEMSGLIST & msglist);
	void	PostOutMessage(const std::shared_ptr<CLiveEngineMsg>& pMsg);
	void	PostToLog(const QString& text, BOOL bDateTime = FALSE, BOOL bBold = FALSE, BOOL bItalic = FALSE, COLORREF crColor = RGB(0, 0, 0));
	void	PostStrippedToLogWithDateStamp(const QString& text);
	void	PostProgress(const QString& szText, LONG lAchieved, LONG lTotal);
	void	PostEndProgress();
	void	PostUpdatePending();
	void	PostFileLoaded(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LPCTSTR szFileName);
	void	PostFileRegistered(LPCTSTR szFileName);
	void	PostChangeImageStatus(LPCTSTR szFileName, IMAGESTATUS status);
	void	PostChangeImageInfo(LPCTSTR szFileName, STACKIMAGEINFO info);
	void	PostUpdateImageOffsets(LPCTSTR szFileName, double fdX, double fdY, double fAngle);
	void	SaveStackedImage(const std::shared_ptr<CMemoryBitmap>& pBitmap);
	void    SaveStackedImage();
	void	PostFootprint(QPointF pt1, QPointF pt2, QPointF pt3, QPointF pt4);
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

	BOOL	GetMessage(std::shared_ptr<CLiveEngineMsg>& rMsg);
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
	QString m_strProgress1;
	QString m_strProgress2;
	LONG m_lTotal1;
	LONG m_lTotal2;
	LONG m_lAchieved1;
	LONG m_lAchieved2;
	QString m_strLastSent[2];

	// ProgressBase
	virtual void applyStart1Text(const QString& strText) override;
	virtual void applyStart2Text(const QString& strText) override;
	virtual void applyProgress1(int lAchieved) override;
	virtual void applyProgress2(int lAchieved) override;
	virtual void applyTitleText(const QString& strText) override;
	virtual void initialise() override;
	virtual void endProgress2() override;
	virtual bool hasBeenCanceled() override;
	virtual void closeProgress() override;
	virtual bool doWarning(const QString& szText) override;
	virtual void applyProcessorsUsed(int nCount) override {};

private:
	enum eOutputType
	{
		OT_TITLE = 0,
		OT_HEADING,
		OT_DETAIL,
		OT_PROGRESS,
		OT_MAX,
	};

	QString m_strLastOut[OT_MAX];
};

