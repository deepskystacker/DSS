#pragma once
#include "DarkFrame.h"
#include "FlatFrame.h"

class CMemoryBitmap;
class CStackingInfo;
namespace DSS { class OldProgressBase; }
class CMasterFrames final
{
private:
	std::shared_ptr<CMemoryBitmap> m_pMasterOffset;
	CDarkFrame m_MasterDark;
	CFlatFrame m_MasterFlat;
	const CStackingInfo* currentStackinfo; // Store the stackinfo of the last LoadMasters() call.
	bool m_fDebloom;

public :
	CMasterFrames();
	~CMasterFrames() = default;

	void ApplyMasterOffset(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::OldProgressBase * pProgress);
	void ApplyMasterDark(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR*, DSS::OldProgressBase* pProgress);
	void ApplyMasterFlat(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::OldProgressBase* pProgress);
	void ApplyHotPixelInterpolation(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::OldProgressBase* pProgress);
	void ApplyAllMasters(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR* pStars, DSS::OldProgressBase* pProgress);

	bool LoadMasters(const CStackingInfo& stackingInfo, DSS::OldProgressBase* pProgress);
};
