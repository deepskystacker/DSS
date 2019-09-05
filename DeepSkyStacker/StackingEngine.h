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
	LONG						m_lImageIndex;
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
	CImageCometShift(LONG lIndex = 0, double fXShift = 0, double fYShift = 0) 
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
	CLightFrameStackingInfo(LPCTSTR szFileName = NULL)
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
	BOOL	GetParameters(LPCTSTR szLightFrame, CBilinearParameters & bp);
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
friend CComputeOffsetTask;

private :
	LIGHTFRAMEINFOVECTOR		m_vBitmaps;
	CLightFramesStackingInfo	m_StackingInfo;
	CDSSProgress *				m_pProgress;
	BOOL						m_bOffsetComputed;
	CString						m_strReferenceFrame;
	LONG						m_lNrCurrentStackable;
	LONG						m_lNrStackable;
	LONG						m_lNrCometStackable;
	LONG						m_lISOSpeed;
	LONG						m_lGain;
	SYSTEMTIME					m_DateTime;
	CBitmapExtraInfo			m_ExtraInfo;
	CRect						m_rcResult;
	double						m_fTotalExposure;
	CSmartPtr<CMemoryBitmap>	m_pOutput;
	CSmartPtr<CMemoryBitmap>	m_pEntropyCoverage;
	CSmartPtr<CMemoryBitmap>	m_pComet;
	IMAGECOMETSHIFTVECTOR		m_vCometShifts;
	double						m_fStarTrailsAngle;
	PIXELTRANSFORMVECTOR		m_vPixelTransforms;
	CBackgroundCalibration		m_BackgroundCalibration;
	CSmartPtr<CMultiBitmap>		m_pMasterLight;
	CTaskInfo *					m_pLightTask;
	LONG						m_lNrStacked;
	double						m_fKeptPercentage;
	BOOL						m_bSaveCalibrated;
	BOOL						m_bSaveIntermediate;
	BOOL						m_bSaveCalibratedDebayered;
	CString						m_strCurrentLightFrame;
	CFATYPE						m_InputCFAType;
	LONG						m_lPixelSizeMultiplier;
	INTERMEDIATEFILEFORMAT		m_IntermediateFileFormat;
	BOOL						m_bCometStacking;
	BOOL						m_bCometInterpolating;
	BOOL						m_bCreateCometImage;
	BOOL						m_bSaveIntermediateCometImages;
	BOOL						m_bApplyFilterToCometImage;
	CPostCalibrationSettings	m_PostCalibrationSettings;
	BOOL						m_bChannelAlign;

	CComAutoCriticalSection		m_CriticalSection;

private :
	BOOL	AddLightFramesToList(CAllStackingTasks & tasks);
	BOOL	ComputeLightFrameOffset(LONG lBitmapIndice, CMatchingStars & MatchingStars);
	BOOL	ComputeMissingCometPositions();
	BOOL	ComputeOffsets();
	BOOL	IsLightFrameStackable(LPCTSTR szFile);
	BOOL	RemoveNonStackableLightFrames(CAllStackingTasks & tasks);
	void	GetResultISOSpeed();
	void	GetResultGain();
	void	GetResultDateTime();
	void	GetResultExtraInfo();
	void	ComputeLargestRectangle(CRect & rc);
	bool	ComputeSmallestRectangle(CRect & rc);
	LONG	FindBitmapIndice(LPCTSTR szFile);
	BOOL	ComputeBitmap();
	BOOL	CreateMasterLightMultiBitmap(CMemoryBitmap * pInBitmap, bool bColor, CMultiBitmap ** ppMultiBitmap);
	BOOL	StackAll(CAllStackingTasks & tasks, CMemoryBitmap ** ppBitmap);
	BOOL	StackLightFrame(CMemoryBitmap * pBitmap, CPixelTransform & PixTransform, double fExposure, BOOL bComet);
	BOOL	AdjustEntropyCoverage();
	BOOL	AdjustBayerDrizzleCoverage();
	BOOL	SaveCalibratedAndRegisteredLightFrame(CMemoryBitmap * pBitmap);
	BOOL	SaveCalibratedLightFrame(CMemoryBitmap * pBitmap);
	BOOL	SaveDeltaImage(CMemoryBitmap * pBitmap);
	BOOL	SaveCometImage(CMemoryBitmap * pBitmap);
	BOOL	SaveCometlessImage(CMemoryBitmap * pBitmap);
	TRANSFORMATIONTYPE GetTransformationType();

public :
	CStackingEngine()
	{
		m_lNrStackable			= 0;
		m_lNrCometStackable		= 0;
		m_lISOSpeed				= 0;
		m_lGain				= -1;
		m_DateTime.wYear		= 0;
		m_pLightTask			= NULL;
		m_lNrStacked			= 0;
		m_fTotalExposure		= 0;
		m_fKeptPercentage		= 100.0;

		m_bSaveCalibrated		= CAllStackingTasks::GetSaveCalibrated();
		m_bSaveCalibratedDebayered = CAllStackingTasks::GetSaveCalibratedDebayered();
		m_bSaveIntermediate		= CAllStackingTasks::GetCreateIntermediates();
		m_InputCFAType			= CFATYPE_NONE;
		m_lPixelSizeMultiplier	= CAllStackingTasks::GetPixelSizeMultiplier();
		m_IntermediateFileFormat= CAllStackingTasks::GetIntermediateFileFormat();
		m_bCometStacking		= FALSE;
		m_bCreateCometImage		= FALSE;
		m_bSaveIntermediateCometImages	= CAllStackingTasks::GetSaveIntermediateCometImages();
		m_bApplyFilterToCometImage		= CAllStackingTasks::GetApplyMedianFilterToCometImage();
		m_bChannelAlign			= CAllStackingTasks::GetChannelAlign();
		m_bCometInterpolating	= FALSE;

		CAllStackingTasks::GetPostCalibrationSettings(m_PostCalibrationSettings);
	};

	virtual ~CStackingEngine()
	{
	};

	void	SetReferenceFrame(LPCTSTR szRefFrame)
	{
		m_strReferenceFrame = szRefFrame;
	};

	void	SetSaveIntermediate(BOOL bSaveIntermediate)
	{
		m_bSaveIntermediate = bSaveIntermediate;
	};

	void	SetSaveCalibrated(BOOL bSaveCalibrated)
	{
		m_bSaveCalibrated= bSaveCalibrated;
	};

	void	SetKeptPercentage(double fPercent)
	{
		m_fKeptPercentage = fPercent;
	};
	BOOL	ComputeOffsets(CAllStackingTasks & tasks, CDSSProgress * pProgress);
	BOOL	StackLightFrames(CAllStackingTasks & tasks, CDSSProgress * pProgress, CMemoryBitmap ** ppBitmap);

	LIGHTFRAMEINFOVECTOR & LightFrames()
	{
		return m_vBitmaps;
	};

	void	SetCometInterpolating(BOOL bSet)
	{
		m_bCometInterpolating = bSet;
	};

	bool	GetDefaultOutputFileName(CString & strFileName, LPCTSTR szFileList, bool bTIFF = true);
	void	WriteDescription(CAllStackingTasks & tasks, LPCTSTR szOutputFile);
};


#endif // __STACKINGENGINE_H__