#ifndef _STACKINGTASKS_H__
#define _STACKINGTASKS_H__

#include <QString>

#include "BitmapExt.h"
#include "FrameInfo.h"
//#include "FrameList.h"


/* ------------------------------------------------------------------- */

bool	LoadFrame(LPCTSTR szFile, PICTURETYPE PistureType, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap);
bool	AreExposureEquals(double fExposure1, double fExposure2);

/* ------------------------------------------------------------------- */

class	CPostCalibrationSettings
{
public :
	bool				m_bHot;
	int				m_lHotFilter;
	double				m_fHotDetection;

	bool				m_bCold;
	int				m_lColdFilter;
	double				m_fColdDetection;

	bool				m_bSaveDeltaImage;
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
		m_bHot			= false;
		m_lHotFilter	= 1;
		m_fHotDetection = 50.0;
		m_bCold			= false;
		m_lColdFilter	= 1;
		m_fColdDetection= 50.0;
		m_bSaveDeltaImage = false;
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
	QString				m_strFolder;

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
public:
	std::uint32_t				m_dwTaskID;
	std::uint32_t				m_dwGroupID;
	PICTURETYPE					m_TaskType;
	int							m_lISOSpeed;
	int							m_lGain;
	double						m_fExposure;
	double						m_fAperture;
	bool						m_bUnmodified;
	bool						m_bDone;
	CString						m_strOutputFile;
	FRAMEINFOVECTOR				m_vBitmaps;
	MULTIBITMAPPROCESSMETHOD	m_Method;
	double						m_fKappa;
	int							m_lNrIterations;
	CSmartPtr<CMultiBitmap>		m_pMaster;

private:
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

public:
	CTaskInfo()
	{
		m_dwTaskID  = 0;
		m_dwGroupID = 0;
		m_lISOSpeed = 0;
		m_lGain     = -1;
		m_fExposure = 0.0;
		m_fAperture = 0.0;
		m_bDone	    = false;
		m_Method	= MBP_MEDIAN;
		m_fKappa	= 2.0;
		m_lNrIterations = 5;
		m_bUnmodified = false;
        m_TaskType = PICTURETYPE(0);
	};

	CTaskInfo(const CTaskInfo& ti)
	{
		CopyFrom(ti);
	};

	const CTaskInfo& operator=(const CTaskInfo & ti)
	{
		CopyFrom(ti);
		return (*this);
	};

	virtual ~CTaskInfo() = default;

	void	SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
	{
		m_Method	= Method;
		m_fKappa	= fKappa;
		m_lNrIterations = lNrIterations;
	};

	void	CreateEmptyMaster(CMemoryBitmap* pBitmap)
	{
		m_pMaster.Attach(pBitmap->CreateEmptyMultiBitmap());
		m_pMaster->SetNrBitmaps((int)m_vBitmaps.size());
		m_pMaster->SetProcessingMethod(m_Method, m_fKappa, m_lNrIterations);
	};

	void	AddToMaster(CMemoryBitmap* pBitmap, CDSSProgress* pProgress)
	{
		if (m_pMaster)
			m_pMaster->AddBitmap(pBitmap, pProgress);
	};

	bool	GetMaster(CMemoryBitmap** ppBitmap, CDSSProgress* pProgress)
	{
		bool			bResult = false;

		if (ppBitmap)
		{
			CSmartPtr<CMemoryBitmap>	pBitmap;

			*ppBitmap = nullptr;
			if (m_pMaster && m_pMaster->GetNrBitmaps() > 1)
			{
				bResult = m_pMaster->GetResult(&pBitmap, pProgress);
				if (pBitmap)
					pBitmap->SetMaster(true);
			};
			m_pMaster.Release();;
			bResult = pBitmap.CopyTo(ppBitmap);
		};

		return bResult;
	};

	bool 	HasISOSpeed() const
	{
		// Has valid ISOSpeed value or no valid Gain value.
		return m_lISOSpeed != 0 || m_lGain < 0;
	};
};

/* ------------------------------------------------------------------- */

bool	GetTaskResult(CTaskInfo * pTaskInfo, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap);
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
	bool	CheckForExistingOffset(CString & strMasterFile);
	bool	CheckForExistingDark(CString & strMasterFile);
	bool	CheckForExistingDarkFlat(CString & strMasterFile);
	bool	CheckForExistingFlat(CString & strMasterFile);

public :
	CStackingInfo()
	{
		m_pOffsetTask	= nullptr;
		m_pDarkTask		= nullptr;
		m_pFlatTask		= nullptr;
		m_pLightTask	= nullptr;
		m_pDarkFlatTask	= nullptr;
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

	bool	DoOffsetTask(CDSSProgress * pProgress);
	bool	DoDarkTask(CDSSProgress * pProgress);
	bool	DoFlatTask(CDSSProgress * pProgress);
	bool	DoDarkFlatTask(CDSSProgress * pProgress);
};

/* ------------------------------------------------------------------- */

class CAllStackingTasks
{
private :
	bool						m_bCalibrating;
	bool						m_bUsingJPEG;
	bool						m_bUsingFITS;
	bool						m_bUseCustomRectangle;
	bool						m_bCometAvailable;
	CRect						m_rcCustom;
	bool						m_bDarkUsed;
	bool						m_bBiasUsed;
	bool						m_bFlatUsed;
	bool						m_bUsingBayer;
	bool						m_bUsingColorImages;
	int							m_lNrLightFrames;
	int							m_lNrBiasFrames;
	int							m_lNrDarkFrames;
	int							m_lNrDarkFlatFrames;
	int							m_lNrFlatFrames;
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

        m_bDarkUsed         = tasks.m_bDarkUsed;
        m_bBiasUsed         = tasks.m_bBiasUsed;
        m_bFlatUsed         = tasks.m_bFlatUsed;
	};

public :
	CAllStackingTasks()
	{
		m_bCalibrating = false;
		m_bUsingJPEG   = false;
		m_bUsingFITS   = false;
		m_bUseCustomRectangle = false;
		m_bCometAvailable	  = false;
		m_dwJobID		= GUID_NULL;
		m_bDarkUsed		= false;
		m_bBiasUsed		= false;
		m_bFlatUsed		= false;
		m_bUsingBayer	= false;
		m_bUsingColorImages = false;

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

	~CAllStackingTasks() = default;

	void	Clear()
	{
		m_vTasks.clear();
		m_vStacks.clear();
		m_bDarkUsed		= false;
		m_bBiasUsed		= false;
		m_bFlatUsed		= false;
		m_bUsingBayer	= false;
		m_bUsingFITS	= false;
		m_bUsingJPEG    = false;

		m_lNrLightFrames	= 0;
		m_lNrBiasFrames		= 0;
		m_lNrDarkFrames		= 0;
		m_lNrDarkFlatFrames	= 0;
		m_lNrFlatFrames		= 0;
		m_fMaxExposureTime	= 0;
	};

	bool	AreCalibratingJPEGFiles()
	{
		return m_bCalibrating && m_bUsingJPEG;
	};

	bool	AreBayerImageUsed()
	{
		return m_bUsingBayer;
	};

	bool	AreFITSImageUsed()
	{
		return m_bUsingFITS;
	};

	bool	AreColorImageUsed()
	{
		return m_bUsingColorImages;
	};

	void	SetCometAvailable(bool bSet)
	{
		m_bCometAvailable = bSet;
	};

	bool	IsCometAvailable()
	{
		return m_bCometAvailable;
	};

	bool	AreDarkUsed()
	{
		return m_bDarkUsed;
	};

	bool	AreFlatUsed()
	{
		return m_bFlatUsed;
	};

	bool	AreBiasUsed()
	{
		return m_bBiasUsed;
	};

	int	GetNrLightFrames()
	{
		return m_lNrLightFrames;
	};
	int	GetNrBiasFrames()
	{
		return m_lNrBiasFrames;
	};
	int	GetNrDarkFrames()
	{
		return m_lNrDarkFrames;
	};
	int	GetNrFlatFrames()
	{
		return m_lNrFlatFrames;
	};
	int	GetNrDarkFlatFrames()
	{
		return m_lNrDarkFlatFrames;
	};
	double	GetMaxExposureTime()
	{
		return m_fMaxExposureTime;
	};

	void	AddFileToTask(const CFrameInfo & FrameInfo, std::uint32_t dwGroupID = 0);
	void	SetCustomRectangle(const CRect & rcCustom)
	{
		if (rcCustom.IsRectEmpty())
		{
			m_bUseCustomRectangle = false;
		}
		else
		{
			m_bUseCustomRectangle = true;
			m_rcCustom = rcCustom;
		};
	};

	void	UseCustomRectangle(bool bUse)
	{
		if (!m_rcCustom.IsRectEmpty())
			m_bUseCustomRectangle = bUse;
		else
			m_bUseCustomRectangle = false;
	};

	bool	GetCustomRectangle(CRect & rcCustom)
	{
		bool			bResult = !m_rcCustom.IsRectEmpty();

		rcCustom = m_rcCustom;

		return bResult;
	};

	bool	IsCustomRectangleUsed()
	{
		return m_bUseCustomRectangle;
	};

	void	ResolveTasks();
	void	ResetTasksStatus();
	void	UpdateTasksMethods();

	int	FindStackID(LPCTSTR szLightFrame);

	STACKINGMODE	GetStackingMode()
	{
		if (m_bUseCustomRectangle)
			return SM_CUSTOM;
		else
			return GetResultMode();
	};

	bool DoOffsetTasks(CDSSProgress* pProgress);
	bool DoDarkTasks(CDSSProgress* pProgress);
	bool DoFlatTasks(CDSSProgress* pProgress);
	bool DoDarkFlatTasks(CDSSProgress* pProgress);
	bool DoAllPreTasks(CDSSProgress* pProgress);

	__int64	ComputeNecessaryDiskSpace(CRect & rcOutput);
	__int64	ComputeNecessaryDiskSpace();
	__int64	AvailableDiskSpace(CString & strDrive);

	bool	CheckReadOnlyStatus(std::vector<CString> & vFolders);

	static	QString GetTemporaryFilesFolder();
	static	void SetTemporaryFilesFolder(QString strFolder);

	static	void GetPostCalibrationSettings(CPostCalibrationSettings & pcs);
	static	void SetPostCalibrationSettings(const CPostCalibrationSettings & pcs);

	static	void GetOutputSettings(COutputSettings & os);
	static	void SetOutputSettings(const COutputSettings & os);

	static  BACKGROUNDCALIBRATIONMODE	GetBackgroundCalibrationMode();
	static  BACKGROUNDCALIBRATIONINTERPOLATION	GetBackgroundCalibrationInterpolation();
	static  RGBBACKGROUNDCALIBRATIONMETHOD	GetRGBBackgroundCalibrationMethod();
	static	bool	GetDarkOptimization();
	static  double	GetDarkFactor();
	static	bool	GetHotPixelsDetection();
	static	bool	GetBadLinesDetection();
	static  STACKINGMODE	GetResultMode();
	static  bool	GetCreateIntermediates();
	static  bool	GetSaveCalibrated();
	static  bool	GetSaveCalibratedDebayered();
	static	void	ClearCache();
	static  WORD	GetAlignmentMethod();
	static  int	GetPixelSizeMultiplier();
	static  bool	GetChannelAlign();
	static  bool	GetSaveIntermediateCometImages();
	static  bool	GetApplyMedianFilterToCometImage();
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

inline void	SpaceToQString(__int64 ulSpace, QString & strSpace)
{
	double fKb(ulSpace / 1024.0);
	double fMb(fKb / 1024.0);
	double fGb(fMb / 1024.0);

	if (fKb < 900)
		strSpace = QString(QCoreApplication::translate("StackRecap", "%L1 kB", "IDS_RECAP_KILOBYTES"))
		.arg(fKb, 0, 'f', 1);
	else if (fMb < 900)
		strSpace = QString(QCoreApplication::translate("StackRecap", "%L1 MB", "IDS_RECAP_MEGABYTES"))
		.arg(fMb, 0, 'f', 1);
	else
		strSpace = QString(QCoreApplication::translate("StackRecap", "%L1 GB", "IDS_RECAP_GIGABYTES"))
		.arg(fGb, 0, 'f', 1);
};

/* ------------------------------------------------------------------- */

#endif // _STACKINGTASKS_H__
