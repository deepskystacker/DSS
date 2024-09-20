#pragma once
#include "DarkFrame.h"
#include "FlatFrame.h"

class CMemoryBitmap;
class CStackingInfo;
namespace DSS { class ProgressBase; }
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

	void ApplyMasterOffset(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::ProgressBase * pProgress);
	void ApplyMasterDark(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR*, DSS::ProgressBase* pProgress);
	void ApplyMasterFlat(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::ProgressBase* pProgress);
	void ApplyHotPixelInterpolation(std::shared_ptr<CMemoryBitmap> pBitmap, DSS::ProgressBase* pProgress);
	void ApplyAllMasters(std::shared_ptr<CMemoryBitmap> pBitmap, const STARVECTOR* pStars, DSS::ProgressBase* pProgress);

	bool LoadMasters(const CStackingInfo& stackingInfo, DSS::ProgressBase* pProgress);
};
