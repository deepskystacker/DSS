#pragma once

#include "DSSCommon.h"
#include "DSSProgress.h"
#include "dssrect.h"
#include "TaskInfo.h"

namespace DSS { class ProgressBase; }

class CMemoryBitmap;
class CFrameInfo;
/* ------------------------------------------------------------------- */

bool LoadFrame(const fs::path filePath, PICTURETYPE PictureType, DSS::ProgressBase * pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap);
bool AreExposureEquals(double fExposure1, double fExposure2);

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
class CTaskInfo;
bool GetTaskResult(const CTaskInfo* pTaskInfo, DSS::ProgressBase* pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap);
void ClearTaskCache();

/* ------------------------------------------------------------------- */

class CStackingInfo
{
	Q_DECLARE_TR_FUNCTIONS(StackingTasks)

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
	bool	CheckForExistingOffset(fs::path& strMasterFile);
	bool	CheckForExistingDark(fs::path& strMasterFile);
	bool	CheckForExistingDarkFlat(fs::path& strMasterFile);
	bool	CheckForExistingFlat(fs::path& strMasterFile);

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

	bool	DoOffsetTask(DSS::ProgressBase* const pProgress);
	bool	DoDarkTask(DSS::ProgressBase* const pProgress);
	bool	DoFlatTask(DSS::ProgressBase* const pProgress);
	bool	DoDarkFlatTask(DSS::ProgressBase* const pProgress);
};

/* ------------------------------------------------------------------- */

class CAllStackingTasks
{
private:
	bool	m_bCalibrating{ false };
	bool	m_bUsingJPEG{ false };
	bool	m_bUsingFITS{ false };
	bool	customRectEnabled{ false };
	bool	m_bCometAvailable{ false };
	DSSRect	customRect{};
	bool	m_bDarkUsed{ false };
	bool	m_bBiasUsed{ false };
	bool	m_bFlatUsed{ false };
	bool	m_bUsingBayer{ false };
	bool	m_bUsingColorImages{ false };
	int		m_lNrLightFrames{ 0 };
	int		m_lNrBiasFrames{ 0 };
	int		m_lNrDarkFrames{ 0 };
	int		m_lNrDarkFlatFrames{ 0 };
	int		m_lNrFlatFrames{ 0 };
	double	m_fMaxExposureTime{ 0.0 };

public:
	std::vector<CTaskInfo>		m_vTasks{};
	std::vector<CStackingInfo>	m_vStacks{};

private:
	CTaskInfo* FindBestMatchingTask(const CTaskInfo& ti, PICTURETYPE TaskType);

public:
	CAllStackingTasks() = default;
	CAllStackingTasks(const CAllStackingTasks&) = default;
	CAllStackingTasks& operator=(const CAllStackingTasks&) = default;
	~CAllStackingTasks() = default;

	void Clear()
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
	}

	bool AreCalibratingJPEGFiles() const
	{
		return m_bCalibrating && m_bUsingJPEG;
	}

	bool AreBayerImageUsed() const
	{
		return m_bUsingBayer;
	}

	bool AreFITSImageUsed() const
	{
		return m_bUsingFITS;
	}

	bool AreColorImageUsed() const
	{
		return m_bUsingColorImages;
	}

	void SetCometAvailable(bool bSet)
	{
		m_bCometAvailable = bSet;
	}

	bool IsCometAvailable() const
	{
		return m_bCometAvailable;
	}

	bool AreDarkUsed() const
	{
		return m_bDarkUsed;
	}

	bool AreFlatUsed() const
	{
		return m_bFlatUsed;
	}

	bool AreBiasUsed() const
	{
		return m_bBiasUsed;
	}

	int GetNrLightFrames() const
	{
		return m_lNrLightFrames;
	}
	int GetNrBiasFrames() const
	{
		return m_lNrBiasFrames;
	}
	int GetNrDarkFrames() const
	{
		return m_lNrDarkFrames;
	}
	int GetNrFlatFrames() const
	{
		return m_lNrFlatFrames;
	}
	int GetNrDarkFlatFrames() const
	{
		return m_lNrDarkFlatFrames;
	}
	double GetMaxExposureTime() const
	{
		return m_fMaxExposureTime;
	}

	void AddFileToTask(const CFrameInfo& frameInfo, const std::uint32_t dwGroupID);

	void setCustomRectangle(const DSSRect& rcCustom)
	{
		if (rcCustom.isEmpty())
		{
			customRectEnabled = false;
		}
		else
		{
			customRectEnabled = true;
			customRect = rcCustom;
		}
	}

	bool getCustomRectangle(DSSRect& rcCustom) const
	{
		rcCustom = customRect;
		return !customRect.isEmpty();
	}

	void enableCustomRect(bool v = true)
	{
		customRectEnabled = v;
	}

	void ResolveTasks();
	void ResetTasksStatus();
	void UpdateTasksMethods();

	int FindStackID(const fs::path& szLightFrame);

	STACKINGMODE getStackingMode() const;

	bool DoOffsetTasks(DSS::ProgressBase* pProgress);
	bool DoDarkTasks(DSS::ProgressBase* pProgress);
	bool DoFlatTasks(DSS::ProgressBase* pProgress);
	bool DoDarkFlatTasks(DSS::ProgressBase* pProgress);
	bool DoAllPreTasks(DSS::ProgressBase* pProgress);

	std::int64_t computeNecessaryDiskSpace(const DSSRect& rcOutput);
	std::int64_t computeNecessaryDiskSpace();
	std::int64_t AvailableDiskSpace(fs::path& strDrive);

	bool	checkReadOnlyStatus(QStringList & folders);

	static	QString GetTemporaryFilesFolder();
	static	void GetTemporaryFilesFolder(fs::path& tempPath);
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
	static  bool	GetCreateIntermediates();
	static  bool	GetSaveCalibrated();
	static  bool	GetSaveCalibratedDebayered();
	static	void	ClearCache();
	static std::uint16_t GetAlignmentMethod();
	static  int	GetPixelSizeMultiplier();
	static  bool	GetChannelAlign();
	static  bool	GetSaveIntermediateCometImages();
	static  bool	GetApplyMedianFilterToCometImage();
	static  INTERMEDIATEFILEFORMAT GetIntermediateFileFormat();
	static	COMETSTACKINGMODE GetCometStackingMode();
};

/* ------------------------------------------------------------------- */

void SpaceToQString(std::int64_t ulSpace, QString& strSpace);

/* ------------------------------------------------------------------- */

