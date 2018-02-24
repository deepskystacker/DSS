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
	DWORD						m_dwDebloom;

public :
	CMasterFrames()
	{
		CWorkspace			workspace;

		m_dwDebloom = 0;
		workspace.GetValue(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Debloom"), m_dwDebloom);
	};

	virtual ~CMasterFrames()
	{
	};

	void	ApplyMasterOffset(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyMasterDark(CMemoryBitmap * pBitmap, STARVECTOR * pStars, CDSSProgress * pProgress);
	void	ApplyMasterFlat(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyHotPixelInterpolation(CMemoryBitmap * pBitmap, CDSSProgress * pProgress);
	void	ApplyAllMasters(CMemoryBitmap * pBitmap, STARVECTOR * pStars, CDSSProgress * pProgress);

	BOOL	LoadMasters(CStackingInfo * pStackingInfo, CDSSProgress * pProgress);
};

#endif // __MASTERFRAMES_H__