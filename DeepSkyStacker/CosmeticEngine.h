#ifndef __COSMETICENGINE_H__
#define __COSMETICENGINE_H__

#include "StackingTasks.h"

class CCosmeticStats
{
public :
	LONG			m_lNrTotalPixels;
	LONG			m_lNrDetectedHotPixels;
	LONG			m_lNrDetectedColdPixels;

private :
	void		CopyFrom(const CCosmeticStats & cs)
	{
		m_lNrTotalPixels		= cs.m_lNrTotalPixels;
		m_lNrDetectedHotPixels	= cs.m_lNrDetectedHotPixels;
		m_lNrDetectedColdPixels = cs.m_lNrDetectedColdPixels;
	};
public :
	CCosmeticStats()
	{
		m_lNrTotalPixels = 0;
		m_lNrDetectedHotPixels = 0;
		m_lNrDetectedColdPixels = 0;
	};

	CCosmeticStats(const CCosmeticStats & cs)
	{
		CopyFrom(cs);
	};

	~CCosmeticStats() {};

	CCosmeticStats & operator = (const CCosmeticStats & cs)
	{
		CopyFrom(cs);
		return (*this);
	};
};

BOOL	ApplyCosmetic(CMemoryBitmap * pBitmap, CMemoryBitmap ** pDeltaBitmap, const CPostCalibrationSettings & pcs, CDSSProgress * pProgress);
BOOL	SimulateCosmetic(CMemoryBitmap * pBitmap, const CPostCalibrationSettings & pcs, CCosmeticStats & cs, CDSSProgress * pProgress);

#endif