#ifndef _STACKINGTASKS_H__
#define _STACKINGTASKS_H__

#include "BitmapExt.h"
#include "FrameInfo.h"
//#include "FrameList.h"

typedef enum INTERMEDIATEFILEFORMAT
{
	IFF_TIFF = 1,
	IFF_FITS = 2
}INTERMEDIATEFILEFORMAT;

typedef enum tagCOMETSTACKINGMODE
{
	CSM_STANDARD	= 0,
	CSM_COMETONLY	= 1,
	CSM_COMETSTAR	= 2
}COMETSTACKINGMODE;

typedef enum tagBACKGROUNDCALIBRATIONMODE
{
	BCM_NONE		= 0,
	BCM_PERCHANNEL	= 1,
	BCM_RGB			= 2
}BACKGROUNDCALIBRATIONMODE;

typedef enum tagBACKGROUNDCALIBRATIONINTERPOLATION
{
	BCI_LINEAR		= 0,
	BCI_RATIONAL	= 1
}BACKGROUNDCALIBRATIONINTERPOLATION;

typedef enum tagRGBBACKGROUNDCALIBRATIONMETHOD
{
	RBCM_MINIMUM		= 0,
	RBCM_MIDDLE			= 1,
	RBCM_MAXIMUM		= 2
}RGBBACKGROUNDCALIBRATIONMETHOD;

/* ------------------------------------------------------------------- */

BOOL	LoadFrame(LPCTSTR szFile, PICTURETYPE PistureType, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap);
BOOL	AreExposureEquals(double fExposure1, double fExposure2);

/* ------------------------------------------------------------------- */

typedef enum tagCOSMETICREPLACE
{
	CR_MEDIAN			= 1,
	CR_GAUSSIAN			= 2
}COSMETICREPLACE;

class	CPostCalibrationSettings
{
public :
	BOOL				m_bHot;
	LONG				m_lHotFilter;
	double				m_fHotDetection;

	BOOL				m_bCold;
	LONG				m_lColdFilter;
	double				m_fColdDetection;

	BOOL				m_bSaveDeltaImage;
	COSMETICREPLACE		m_Replace;

private :
	void	CopyFrom(const CPostCalibrationSettings & pcs)
	{
		m_bHot = pcs.m_bHot;
		m_lHotFilter = pcs.m_lHotFilter;
		m_fHotDetection = pcs.m_fHotDetection;

		m_bCold = pcs.m_bCold;
		m_lColdFilter = pcs.m_lColdFilter;
		m_fColdDetection = pcs.m_fColdDetection;

		m_bSaveDeltaImage = pcs.m_bSaveDeltaImage;
		m_Replace		  = pcs.m_Replace;
	};

public :
	CPostCalibrationSettings()
	{
		m_bHot			= FALSE;
		m_lHotFilter	= 1;
		m_fHotDetection = 50.0;
		m_bCold			= FALSE;
		m_lColdFilter	= 1;
		m_fColdDetection= 50.0;
		m_bSaveDeltaImage = FALSE;
		m_Replace		  = CR_MEDIAN;
	};
	virtual ~CPostCalibrationSettings() {};

	CPostCalibrationSettings(const CPostCalibrationSettings & pcs)
	{
		CopyFrom(pcs);
	};

	CPostCalibrationSettings & operator = (const CPostCalibrationSettings & pcs)
	{
		CopyFrom(pcs);
		return (*this);
	};
};

/* ------------------------------------------------------------------- */

class COutputSettings
{
public :
	bool				m_bOutput;
	bool				m_bOutputHTML;
	bool				m_bAutosave;
	bool				m_bFileList;
	bool				m_bAppend;
	bool				m_bRefFrameFolder;
	bool				m_bFileListFolder;
	bool				m_bOtherFolder;
	CString				m_strFolder;

private:
	void	CopyFrom(const COutputSettings & right)
	{
		m_bOutput			=right.m_bOutput			;
		m_bOutputHTML		=right.m_bOutputHTML		;
		m_bAutosave			=right.m_bAutosave			;
		m_bFileList			=right.m_bFileList			;
		m_bAppend			=right.m_bAppend			;
		m_bRefFrameFolder	=right.m_bRefFrameFolder	;
		m_bFileListFolder	=right.m_bFileListFolder	;
		m_bOtherFolder		=right.m_bOtherFolder		;
		m_strFolder			=right.m_strFolder			;
	};

public:
	COutputSettings()
	{
		m_bOutput	  = true;
		m_bOutputHTML = false;
		m_bAutosave   = true;
		m_bFileList	  = false;
		m_bAppend	  = true;
		m_bRefFrameFolder = true;
		m_bFileListFolder = false;
		m_bOtherFolder    = false;
	};
	~COutputSettings()
	{
	};

	COutputSettings(const COutputSettings & right)
	{
		CopyFrom(right);
	};

    COutputSettings& operator=(COutputSettings const& other)
    {
        CopyFrom(other);
        return *this;
    }
};


/* ------------------------------------------------------------------- */

class CTaskInfo
{
public :
	DWORD						m_dwTaskID;
	DWORD						m_dwGroupID;
	PICTURETYPE					m_TaskType;
	LONG						m_lISOSpeed;
	LONG						m_lGain;
	double						m_fExposure;
	double						m_fAperture;
	BOOL						m_bUnmodified;
	BOOL						m_bDone;
	CString						m_strOutputFile;
	FRAMEINFOVECTOR				m_vBitmaps;
	MULTIBITMAPPROCESSMETHOD	m_Method;
	double						m_fKappa;
	LONG						m_lNrIterations;
	CSmartPtr<CMultiBitmap>		m_pMaster;

private :
	void	CopyFrom(const CTaskInfo & ti)
	{
		m_dwTaskID		= ti.m_dwTaskID;
		m_dwGroupID		= ti.m_dwGroupID;
		m_TaskType		= ti.m_TaskType;
		m_lISOSpeed		= ti.m_lISOSpeed;
		m_lGain			= ti.m_lGain;
		m_fExposure		= ti.m_fExposure;
		m_fAperture     = ti.m_fAperture;
		m_vBitmaps		= ti.m_vBitmaps;
		m_bDone			= ti.m_bDone;
		m_bUnmodified	= ti.m_bUnmodified;
		m_strOutputFile	= ti.m_strOutputFile;
		m_Method		= ti.m_Method;
		m_fKappa		= ti.m_fKappa;
		m_lNrIterations	= ti.m_lNrIterations;
		m_pMaster		= ti.m_pMaster;
	};

public :
	CTaskInfo()
	{
		m_dwTaskID  = 0;
		m_dwGroupID = 0;
		m_lISOSpeed = 0;
		m_lGain     = -1;
		m_fExposure = 0.0;
		m_fAperture = 0.0;
		m_bDone	    = FALSE;
		m_Method	= MBP_MEDIAN;
		m_fKappa	= 2.0;
		m_lNrIterations = 5;
		m_bUnmodified = FALSE;
	};

	CTaskInfo(const CTaskInfo & ti)
	{
		CopyFrom(ti);
	};

	const CTaskInfo & operator = (const CTaskInfo & ti)
	{
		CopyFrom(ti);
		return (*this);
	};

	virtual ~CTaskInfo()
	{
	};

	void	SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, LONG lNrIterations)
	{
		m_Method	= Method;
		m_fKappa	= fKappa;
		m_lNrIterations = lNrIterations;
	};

	void	CreateEmptyMaster(CMemoryBitmap * pBitmap)
	{
		m_pMaster.Attach(pBitmap->CreateEmptyMultiBitmap());
		m_pMaster->SetNrBitmaps((LONG)m_vBitmaps.size());
		m_pMaster->SetProcessingMethod(m_Method, m_fKappa, m_lNrIterations);
	};

	void	AddToMaster(CMemoryBitmap * pBitmap, CDSSProgress * pProgress)
	{
		if (m_pMaster)
			m_pMaster->AddBitmap(pBitmap, pProgress);
	};

	BOOL	GetMaster(CMemoryBitmap ** ppBitmap, CDSSProgress * pProgress)
	{
		BOOL			bResult = FALSE;

		if (ppBitmap)
		{
			CSmartPtr<CMemoryBitmap>	pBitmap;

			*ppBitmap = NULL;
			if (m_pMaster && m_pMaster->GetNrBitmaps() > 1)
			{
				bResult = m_pMaster->GetResult(&pBitmap, pProgress);
				if (pBitmap)
					pBitmap->SetMaster(TRUE);
			};
			m_pMaster.Release();;
			bResult = pBitmap.CopyTo(ppBitmap);
		};

		return bResult;
	};

	BOOL 	HasISOSpeed() const
	{
		// Has valid ISOSpeed value or no valid Gain value.
		return m_lISOSpeed != 0 || m_lGain < 0;
	};
};

/* ------------------------------------------------------------------- */

BOOL	GetTaskResult(CTaskInfo * pTaskInfo, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap);
void	ClearTaskCache();

/* ------------------------------------------------------------------- */

class CStackingInfo
{
public :
	CTaskInfo *			m_pOffsetTask;
	CTaskInfo *			m_pDarkTask;
	CTaskInfo *			m_pFlatTask;
	CTaskInfo *			m_pLightTask;
	CTaskInfo *			m_pDarkFlatTask;

private :
	void	CopyFrom(const CStackingInfo & si)
	{
		m_pOffsetTask	= si.m_pOffsetTask;
		m_pDarkTask		= si.m_pDarkTask;;
		m_pFlatTask		= si.m_pFlatTask;
		m_pLightTask	= si.m_pLightTask;
		m_pDarkFlatTask = si.m_pDarkFlatTask;
	};

private :
	BOOL	CheckForExistingOffset(CString & strMasterFile);
	BOOL	CheckForExistingDark(CString & strMasterFile);
	BOOL	CheckForExistingDarkFlat(CString & strMasterFile);
	BOOL	CheckForExistingFlat(CString & strMasterFile);

public :
	CStackingInfo()
	{
		m_pOffsetTask	= NULL;
		m_pDarkTask		= NULL;
		m_pFlatTask		= NULL;
		m_pLightTask	= NULL;
		m_pDarkFlatTask	= NULL;
	};

	CStackingInfo(const CStackingInfo & si)
	{
		CopyFrom(si);
	};

	virtual ~CStackingInfo()
	{
	};

	const CStackingInfo & operator = (const CStackingInfo & si)
	{
		CopyFrom(si);
		return (*this);
	};

	BOOL	DoOffsetTask(CDSSProgress * pProgress);
	BOOL	DoDarkTask(CDSSProgress * pProgress);
	BOOL	DoFlatTask(CDSSProgress * pProgress);
	BOOL	DoDarkFlatTask(CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

typedef enum tagSTACKINGMODE
{
	SM_NORMAL		= 0,
	SM_MOSAIC		= 1,
	SM_CUSTOM		= 2,
	SM_INTERSECTION = 3
}STACKINGMODE;

class CAllStackingTasks
{
private :
	BOOL						m_bCalibrating;
	BOOL						m_bUsingJPEG;
	BOOL						m_bUsingFITS;
	BOOL						m_bUseCustomRectangle;
	BOOL						m_bCometAvailable;
	CRect						m_rcCustom;
	BOOL						m_bDarkUsed;
	BOOL						m_bBiasUsed;
	BOOL						m_bFlatUsed;
	BOOL						m_bUsingBayer;
	BOOL						m_bUsingColorImages;
	LONG						m_lNrLightFrames;
	LONG						m_lNrBiasFrames;
	LONG						m_lNrDarkFrames;
	LONG						m_lNrDarkFlatFrames;
	LONG						m_lNrFlatFrames;
	double						m_fMaxExposureTime;

public :
	std::vector<CTaskInfo>		m_vTasks;
	std::vector<CStackingInfo>	m_vStacks;
	GUID						m_dwJobID;
	CString						m_strJob;

private :
	CTaskInfo *	FindBestMatchingTask(const CTaskInfo & ti, PICTURETYPE TaskType);
	void		CopyFrom(const CAllStackingTasks & tasks)
	{
		m_bCalibrating		= tasks.m_bCalibrating;
		m_bUsingJPEG		= tasks.m_bUsingJPEG;
		m_bUsingFITS		= tasks.m_bUsingFITS;
		m_bUseCustomRectangle = tasks.m_bUseCustomRectangle;
		m_bCometAvailable	= tasks.m_bCometAvailable;
		m_rcCustom			= tasks.m_rcCustom;
		m_vTasks			= tasks.m_vTasks;
		m_vStacks			= tasks.m_vStacks;
		m_dwJobID			= tasks.m_dwJobID;
		m_strJob			= tasks.m_strJob;
		m_bUsingBayer		= tasks.m_bUsingBayer;
		m_bUsingColorImages = tasks.m_bUsingColorImages;

		m_lNrLightFrames	= tasks.m_lNrLightFrames;
		m_lNrBiasFrames		= tasks.m_lNrBiasFrames;
		m_lNrDarkFrames		= tasks.m_lNrDarkFrames;
		m_lNrDarkFlatFrames	= tasks.m_lNrDarkFlatFrames;
		m_lNrFlatFrames		= tasks.m_lNrFlatFrames;
		m_fMaxExposureTime	= tasks.m_fMaxExposureTime;
	};

public :
	CAllStackingTasks()
	{
		m_bCalibrating = FALSE;
		m_bUsingJPEG   = FALSE;
		m_bUsingFITS   = FALSE;
		m_bUseCustomRectangle = FALSE;
		m_bCometAvailable	  = FALSE;
		m_dwJobID		= GUID_NULL;
		m_bDarkUsed		= FALSE;
		m_bBiasUsed		= FALSE;
		m_bFlatUsed		= FALSE;
		m_bUsingBayer	= FALSE;
		m_bUsingColorImages = FALSE;

		m_lNrLightFrames	= 0;
		m_lNrBiasFrames		= 0;
		m_lNrDarkFrames		= 0;
		m_lNrDarkFlatFrames	= 0;
		m_lNrFlatFrames		= 0;
		m_fMaxExposureTime  = 0;
	};

	CAllStackingTasks(const CAllStackingTasks & tasks)
	{
		CopyFrom(tasks);
	};

	CAllStackingTasks & operator = (const CAllStackingTasks & tasks)
	{
		CopyFrom(tasks);
		return (*this);
	};

	~CAllStackingTasks()
	{
	};

	void	Clear()
	{
		m_vTasks.clear();
		m_vStacks.clear();
		m_bDarkUsed		= FALSE;
		m_bBiasUsed		= FALSE;
		m_bFlatUsed		= FALSE;
		m_bUsingBayer	= FALSE;
		m_bUsingFITS	= FALSE;
		m_bUsingJPEG    = FALSE;

		m_lNrLightFrames	= 0;
		m_lNrBiasFrames		= 0;
		m_lNrDarkFrames		= 0;
		m_lNrDarkFlatFrames	= 0;
		m_lNrFlatFrames		= 0;
		m_fMaxExposureTime	= 0;
	};

	BOOL	AreCalibratingJPEGFiles()
	{
		return m_bCalibrating && m_bUsingJPEG;
	};

	BOOL	AreBayerImageUsed()
	{
		return m_bUsingBayer;
	};

	BOOL	AreFITSImageUsed()
	{
		return m_bUsingFITS;
	};

	BOOL	AreColorImageUsed()
	{
		return m_bUsingColorImages;
	};

	void	SetCometAvailable(BOOL bSet)
	{
		m_bCometAvailable = bSet;
	};

	BOOL	IsCometAvailable()
	{
		return m_bCometAvailable;
	};

	BOOL	AreDarkUsed()
	{
		return m_bDarkUsed;
	};

	BOOL	AreFlatUsed()
	{
		return m_bFlatUsed;
	};

	BOOL	AreBiasUsed()
	{
		return m_bBiasUsed;
	};

	LONG	GetNrLightFrames()
	{
		return m_lNrLightFrames;
	};
	LONG	GetNrBiasFrames()
	{
		return m_lNrBiasFrames;
	};
	LONG	GetNrDarkFrames()
	{
		return m_lNrDarkFrames;
	};
	LONG	GetNrFlatFrames()
	{
		return m_lNrFlatFrames;
	};
	LONG	GetNrDarkFlatFrames()
	{
		return m_lNrDarkFlatFrames;
	};
	double	GetMaxExposureTime()
	{
		return m_fMaxExposureTime;
	};

	void	AddFileToTask(const CFrameInfo & FrameInfo, DWORD dwGroupID = 0);
	void	SetCustomRectangle(const CRect & rcCustom)
	{
		if (rcCustom.IsRectEmpty())
		{
			m_bUseCustomRectangle = FALSE;
		}
		else
		{
			m_bUseCustomRectangle = TRUE;
			m_rcCustom = rcCustom;
		};
	};

	void	UseCustomRectangle(BOOL bUse)
	{
		if (!m_rcCustom.IsRectEmpty())
			m_bUseCustomRectangle = bUse;
		else
			m_bUseCustomRectangle = FALSE;
	};

	BOOL	GetCustomRectangle(CRect & rcCustom)
	{
		BOOL			bResult = !m_rcCustom.IsRectEmpty();

		rcCustom = m_rcCustom;

		return bResult;
	};

	BOOL	IsCustomRectangleUsed()
	{
		return m_bUseCustomRectangle;
	};

	void	ResolveTasks();
	void	ResetTasksStatus();
	void	UpdateTasksMethods();

	LONG	FindStackID(LPCTSTR szLightFrame);

	STACKINGMODE	GetStackingMode()
	{
		if (m_bUseCustomRectangle)
			return SM_CUSTOM;
		else
			return GetResultMode();
	};

	BOOL	DoOffsetTasks(CDSSProgress * pProgress);
	BOOL	DoDarkTasks(CDSSProgress * pProgress);
	BOOL	DoFlatTasks(CDSSProgress * pProgress);
	BOOL	DoDarkFlatTasks(CDSSProgress * pProgress);
	BOOL	DoAllPreTasks(CDSSProgress * pProgress)
	{
		return DoOffsetTasks(pProgress) &&
			   DoDarkTasks(pProgress) &&
			   DoDarkFlatTasks(pProgress) &&
			   DoFlatTasks(pProgress);
	};

	__int64	ComputeNecessaryDiskSpace(CRect & rcOutput);
	__int64	ComputeNecessaryDiskSpace();
	__int64	AvailableDiskSpace(CString & strDrive);

	BOOL	CheckReadOnlyStatus(std::vector<CString> & vFolders);

	static	void GetTemporaryFilesFolder(CString & strFolder);
	static	void SetTemporaryFilesFolder(LPCTSTR szFolder);

	static	void GetPostCalibrationSettings(CPostCalibrationSettings & pcs);
	static	void SetPostCalibrationSettings(const CPostCalibrationSettings & pcs);

	static	void GetOutputSettings(COutputSettings & os);
	static	void SetOutputSettings(const COutputSettings & os);

	static  BACKGROUNDCALIBRATIONMODE	GetBackgroundCalibrationMode();
	static  BACKGROUNDCALIBRATIONINTERPOLATION	GetBackgroundCalibrationInterpolation();
	static  RGBBACKGROUNDCALIBRATIONMETHOD	GetRGBBackgroundCalibrationMethod();
	static	BOOL	GetDarkOptimization();
	static  double	GetDarkFactor();
	static	BOOL	GetHotPixelsDetection();
	static	BOOL	GetBadLinesDetection();
	static  STACKINGMODE	GetResultMode();
	static  BOOL	GetCreateIntermediates();
	static  BOOL	GetSaveCalibrated();
	static  BOOL	GetSaveCalibratedDebayered();
	static	void	ClearCache();
	static  WORD	GetAlignmentMethod();
	static  LONG	GetPixelSizeMultiplier();
	static  BOOL	GetChannelAlign();
	static  BOOL	GetSaveIntermediateCometImages();
	static  BOOL	GetApplyMedianFilterToCometImage();
	static  INTERMEDIATEFILEFORMAT GetIntermediateFileFormat();
	static	COMETSTACKINGMODE GetCometStackingMode();
};

/* ------------------------------------------------------------------- */

class	CAllStackingJobs
{
public :
	std::vector<CAllStackingTasks>		m_vStackingTasks;

public :
	CAllStackingJobs()
	{
	};

	virtual ~CAllStackingJobs()
	{
	};
};

/* ------------------------------------------------------------------- */

inline void	SpaceToString(__int64 ulSpace, CString & strSpace)
{
	double					fKb,
							fMb,
							fGb;

	fKb = ulSpace / 1024.0;
	fMb	= fKb / 1024.0;
	fGb = fMb / 1024.0;

	if (fKb < 900)
		strSpace.Format(IDS_RECAP_KILOBYTES, fKb);
	else if (fMb < 900)
		strSpace.Format(IDS_RECAP_MEGABYTES, fMb);
	else
		strSpace.Format(IDS_RECAP_GIGABYTES, fGb);
};

/* ------------------------------------------------------------------- */

#endif // _STACKINGTASKS_H__
