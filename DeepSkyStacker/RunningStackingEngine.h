#ifndef __RUNNINGSTACKINGENGINE_H__
#define __RUNNINGSTACKINGENGINE_H__

#include "RegisterEngine.h"
#include "PixelTransform.h"
#include "BackgroundCalibration.h"

/* ------------------------------------------------------------------- */

class CRunningStackingEngine
{
private :
	CSmartPtr<CMemoryBitmap>		m_pStackedBitmap;
	CSmartPtr<CMemoryBitmap>		m_pPublicBitmap;
	CBackgroundCalibration			m_BackgroundCalibration;
	LONG							m_lNrStacked;
	double							m_fTotalExposure;
	CMatchingStars					m_MatchingStars;

private:
	void	CreatePublicBitmap();

public :
	CRunningStackingEngine();
	~CRunningStackingEngine();

	BOOL	ComputeOffset(CLightFrameInfo & lfi);
	BOOL	AddImage(CLightFrameInfo & lfi, CDSSProgress * pProgress);
	BOOL	GetStackedImage(CMemoryBitmap ** ppBitmap)
	{
		BOOL			bResult = FALSE;
		
		if (ppBitmap)
			*ppBitmap = NULL;

		if (ppBitmap && m_pPublicBitmap)
			bResult = m_pPublicBitmap.CopyTo(ppBitmap);

		return bResult;
	};

	LONG	GetNrStackedImages()
	{
		return m_lNrStacked;
	};

	double	GetTotalExposure()
	{
		return m_fTotalExposure;
	};

	void	Clear()
	{
		m_pStackedBitmap.Release();
		m_pPublicBitmap.Release();
		m_lNrStacked = 0;
		m_fTotalExposure = 0;
	};
};


#endif // __RUNNINGSTACKINGENGINE_H__