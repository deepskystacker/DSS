#ifndef __MASTERFRAMES_H__
#define __MASTERFRAMES_H__

#include "BitmapExt.h"
#include "FlatFrame.h"
#include "DarkFrame.h"
#include "StackingTasks.h"
#include "Stars.h"

#include "Workspace.h"

class CMasterFrames
{
private :
	std::shared_ptr<CMemoryBitmap> m_pMasterOffset;
	CDarkFrame					m_MasterDark;
	CFlatFrame					m_MasterFlat;
	bool						m_fDebloom;

public :
	CMasterFrames()
	{
		Workspace			workspace;

		m_fDebloom = workspace.value("Stacking/Debloom", false).toBool();
	};

	virtual ~CMasterFrames()
	{
	};

	void ApplyMasterOffset(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase * pProgress);
	void ApplyMasterDark(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR*, ProgressBase * pProgress);
	void ApplyMasterFlat(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase * pProgress);
	void ApplyHotPixelInterpolation(std::shared_ptr<CMemoryBitmap> pBitmap, ProgressBase * pProgress);
	void ApplyAllMasters(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR* pStars, ProgressBase * pProgress);

	bool	LoadMasters(const CStackingInfo* pStackingInfo, ProgressBase* pProgress);
};

#endif // __MASTERFRAMES_H__