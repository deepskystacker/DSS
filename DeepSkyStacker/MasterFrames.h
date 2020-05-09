#ifndef __MASTERFRAMES_H__
#define __MASTERFRAMES_H__

#include "BitmapExt.h"
#include "FlatFrame.h"
#include "DarkFrame.h"
#include "StackingTasks.h"
#include "Stars.h"
#include "Registry.h"
#include "Workspace.h"

class CMasterFrames
{
private :
	CSmartPtr<CMemoryBitmap>	m_pMasterOffset;
	CDarkFrame					m_MasterDark;
	CFlatFrame					m_MasterFlat;
	bool						m_fDebloom;

public :
	CMasterFrames()
	{
		CWorkspace			workspace;

		m_fDebloom = workspace.value("Stacking/Debloom", false).toBool();
	};

	virtual ~CMasterFrames()
	{
	};

	void	ApplyMasterOffset(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyMasterDark(CMemoryBitmap * pBitmap, STARVECTOR * pStars, CDSSProgress * pProgress);
	void	ApplyMasterFlat(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyHotPixelInterpolation(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyAllMasters(CMemoryBitmap * pBitmap, STARVECTOR * pStars, CDSSProgress * pProgress);

	bool	LoadMasters(CStackingInfo * pStackingInfo, CDSSProgress * pProgress);
};

#endif // __MASTERFRAMES_H__