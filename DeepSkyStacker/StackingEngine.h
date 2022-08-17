#ifndef __STACKINGENGINE_H__
#define __STACKINGENGINE_H__

#include "RegisterEngine.h"
#include "PixelTransform.h"
#include "BackgroundCalibration.h"

class CComputeOffsetTask;

/* ------------------------------------------------------------------- */

class	CImageCometShift
{
public :
	int						m_lImageIndex;
	double						m_fXShift,
								m_fYShift;

private :
	void	CopyFrom(const CImageCometShift & ics)
	{
		m_lImageIndex	= ics.m_lImageIndex;
		m_fXShift		= ics.m_fXShift;
		m_fYShift		= ics.m_fYShift;
	};

public :
	CImageCometShift(int lIndex = 0, double fXShift = 0, double fYShift = 0)
	{
		m_lImageIndex	= lIndex;
		m_fXShift		= fXShift;
		m_fYShift		= fYShift;
	};
	CImageCometShift(const CImageCometShift & ics)
	{
		CopyFrom(ics);
	};

	~CImageCometShift() {};

	CImageCometShift & operator = (const CImageCometShift & ics)
	{
		CopyFrom(ics);
		return (*this);
	};

	bool operator < (const CImageCometShift & ics) const
	{
		if (m_fXShift < ics.m_fXShift)
			return true;
		else if (m_fXShift > ics.m_fXShift)
			return false;
		else
			return (m_fYShift < ics.m_fYShift);
	};
};

typedef std::vector<CImageCometShift>		IMAGECOMETSHIFTVECTOR;

/* ------------------------------------------------------------------- */

class CLightFrameStackingInfo
{
public :
	CString						m_strFileName;
	CString						m_strInfoFileName;
	CBilinearParameters			m_BilinearParameters;

private :
	void	CopyFrom(const CLightFrameStackingInfo & lfsi)
	{
		m_strFileName		 = lfsi.m_strFileName;
		m_strInfoFileName	 = lfsi.m_strInfoFileName;
		m_BilinearParameters = lfsi.m_BilinearParameters;
	};
public :
	CLightFrameStackingInfo(LPCTSTR szFileName = nullptr)
	{
		m_strFileName = szFileName;
	};

	~CLightFrameStackingInfo() {};

	CLightFrameStackingInfo(const CLightFrameStackingInfo & lfsi)
	{
		CopyFrom(lfsi);
	};

	CLightFrameStackingInfo & operator = (const CLightFrameStackingInfo & lfsi)
	{
		CopyFrom(lfsi);
		return (*this);
	};

	bool operator < (const CLightFrameStackingInfo & lfsi) const
	{
		return (m_strFileName.CompareNoCase(lfsi.m_strFileName)<0);
	};

};

typedef std::vector<CLightFrameStackingInfo>		LIGHTFRAMESTACKINGINFOVECTOR;
typedef LIGHTFRAMESTACKINGINFOVECTOR::iterator		LIGHTFRAMESTACKINGINFOITERATOR;

/* ------------------------------------------------------------------- */

class CLightFramesStackingInfo
{
private :
	CString							m_strReferenceFrame;
	CString							m_strStackingFileInfo;
	LIGHTFRAMESTACKINGINFOVECTOR	m_vLightFrameStackingInfo;

private :
	void		GetInfoFileName(LPCTSTR szLightFrame, CString & strInfoFileName);

public :
	CLightFramesStackingInfo() {};
	virtual ~CLightFramesStackingInfo() {};

	void	SetReferenceFrame(LPCTSTR szReferenceFrame);
	void	AddLightFrame(LPCTSTR szLightFrame, const CBilinearParameters & bp);
	bool	GetParameters(LPCTSTR szLightFrame, CBilinearParameters & bp);
	void	Save();
	void	Clear()
	{
		m_strReferenceFrame.Empty();
		m_strStackingFileInfo.Empty();
		m_vLightFrameStackingInfo.clear();
	};
};

/* ------------------------------------------------------------------- */

class CStackingEngine
{
private :
	LIGHTFRAMEINFOVECTOR		m_vBitmaps;
	CLightFramesStackingInfo	m_StackingInfo;
	CDSSProgress *				m_pProgress;
	bool						m_bOffsetComputed;
	CString						m_strReferenceFrame;
	int						m_lNrCurrentStackable;
	int						m_lNrStackable;
	int						m_lNrCometStackable;
	int						m_lISOSpeed;
	int						m_lGain;
	SYSTEMTIME					m_DateTime;
	CBitmapExtraInfo			m_ExtraInfo;
	CRect						m_rcResult;
	double						m_fTotalExposure;
	std::shared_ptr<CMemoryBitmap> m_pOutput;
	std::shared_ptr<CMemoryBitmap> m_pEntropyCoverage;
	std::shared_ptr<CMemoryBitmap> m_pComet;
	IMAGECOMETSHIFTVECTOR		m_vCometShifts;
	double						m_fStarTrailsAngle;
	PIXELTRANSFORMVECTOR		m_vPixelTransforms;
	CBackgroundCalibration		m_BackgroundCalibration;
	std::shared_ptr<CMultiBitmap> m_pMasterLight;
	CTaskInfo *					m_pLightTask;
	int						m_lNrStacked;
	double						m_fKeptPercentage;
	bool						m_bSaveCalibrated;
	bool						m_bSaveIntermediate;
	bool						m_bSaveCalibratedDebayered;
	CString						m_strCurrentLightFrame;
	CFATYPE						m_InputCFAType;
	int						m_lPixelSizeMultiplier;
	INTERMEDIATEFILEFORMAT		m_IntermediateFileFormat;
	bool						m_bCometStacking;
	bool						m_bCometInterpolating;
	bool						m_bCreateCometImage;
	bool						m_bSaveIntermediateCometImages;
	bool						m_bApplyFilterToCometImage;
	CPostCalibrationSettings	m_PostCalibrationSettings;
	bool						m_bChannelAlign;

	CComAutoCriticalSection		m_CriticalSection;

public:
	bool ComputeLightFrameOffset(int lBitmapIndice);
	void incStackable() { ++m_lNrStackable; }
	void incCometStackable() { ++m_lNrCometStackable; }

private :
	bool	AddLightFramesToList(CAllStackingTasks & tasks);
	bool	ComputeMissingCometPositions();
	bool	ComputeOffsets();
	bool	IsLightFrameStackable(LPCTSTR szFile);
	bool	RemoveNonStackableLightFrames(CAllStackingTasks & tasks);
	void	GetResultISOSpeed();
	void	GetResultGain();
	void	GetResultDateTime();
	void	GetResultExtraInfo();
	void	ComputeLargestRectangle(CRect & rc);
	bool	ComputeSmallestRectangle(CRect & rc);
	int	FindBitmapIndice(LPCTSTR szFile);
	void	ComputeBitmap();
	std::shared_ptr<CMultiBitmap> CreateMasterLightMultiBitmap(const CMemoryBitmap* pInBitmap, const bool bColor);
	bool StackAll(CAllStackingTasks & tasks, std::shared_ptr<CMemoryBitmap>& rpBitmap);
	bool	StackLightFrame(std::shared_ptr<CMemoryBitmap> pBitmap, CPixelTransform& PixTransform, double fExposure, bool bComet);
	bool	AdjustEntropyCoverage();
	bool	AdjustBayerDrizzleCoverage();
	bool	SaveCalibratedAndRegisteredLightFrame(CMemoryBitmap * pBitmap) const;
	bool	SaveCalibratedLightFrame(std::shared_ptr<CMemoryBitmap> pBitmap) const;
	bool	SaveDeltaImage(CMemoryBitmap* pBitmap) const;
	bool	SaveCometImage(CMemoryBitmap* pBitmap) const;
	bool	SaveCometlessImage(CMemoryBitmap* pBitmap) const;
	TRANSFORMATIONTYPE GetTransformationType();

public :
	CStackingEngine() :
		m_lNrStackable{ 0 },
		m_lNrCometStackable{ 0 },
		m_lISOSpeed{ 0 },
		m_lGain{ -1 },
		m_pLightTask{ nullptr },
		m_lNrStacked{ 0 },
		m_fTotalExposure{ 0 },
		m_fKeptPercentage{ 100.0 },
		m_bSaveCalibrated{ CAllStackingTasks::GetSaveCalibrated() },
		m_bSaveCalibratedDebayered{ CAllStackingTasks::GetSaveCalibratedDebayered() },
		m_bSaveIntermediate{ CAllStackingTasks::GetCreateIntermediates() },
		m_InputCFAType{ CFATYPE_NONE },
		m_lPixelSizeMultiplier{ CAllStackingTasks::GetPixelSizeMultiplier() },
		m_IntermediateFileFormat{ CAllStackingTasks::GetIntermediateFileFormat() },
		m_bCometStacking{ false },
		m_bCreateCometImage{ false },
		m_bSaveIntermediateCometImages{ CAllStackingTasks::GetSaveIntermediateCometImages() },
		m_bApplyFilterToCometImage{ CAllStackingTasks::GetApplyMedianFilterToCometImage() },
		m_bChannelAlign{ CAllStackingTasks::GetChannelAlign() },
		m_bCometInterpolating{ false }
		
	{
		m_DateTime.wYear = 0;
		CAllStackingTasks::GetPostCalibrationSettings(m_PostCalibrationSettings);
	}

	virtual ~CStackingEngine() = default;

	CLightFrameInfo& getBitmap(const int n)
	{
		return this->m_vBitmaps[n];
	}

	void SetReferenceFrame(LPCTSTR szRefFrame)
	{
		m_strReferenceFrame = szRefFrame;
	}

	void SetSaveIntermediate(bool bSaveIntermediate)
	{
		m_bSaveIntermediate = bSaveIntermediate;
	}

	void SetSaveCalibrated(bool bSaveCalibrated)
	{
		m_bSaveCalibrated= bSaveCalibrated;
	}

	void SetKeptPercentage(double fPercent)
	{
		m_fKeptPercentage = fPercent;
	}

	bool ComputeOffsets(CAllStackingTasks& tasks, CDSSProgress* pProgress);
	bool StackLightFrames(CAllStackingTasks& tasks, CDSSProgress* const pProgress, std::shared_ptr<CMemoryBitmap>& rpBitmap);

	LIGHTFRAMEINFOVECTOR& LightFrames()
	{
		return m_vBitmaps;
	}

	void SetCometInterpolating(bool bSet)
	{
		m_bCometInterpolating = bSet;
	}

	bool GetDefaultOutputFileName(CString& strFileName, LPCTSTR szFileList, bool bTIFF = true);
	void WriteDescription(CAllStackingTasks& tasks, LPCTSTR szOutputFile);
};


#endif // __STACKINGENGINE_H__