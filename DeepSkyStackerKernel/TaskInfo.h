#pragma once

#include "DSSCommon.h"
#include "FrameInfo.h"

class CMultiBitmap;
class CMemoryBitmap;
namespace DSS { class ProgressBase; }
using namespace DSS;

class CTaskInfo final
{
public:
	std::uint32_t m_dwTaskID{ 0 };
	std::uint32_t m_groupID{ 0 };
	PICTURETYPE m_TaskType{ PICTURETYPE_UNKNOWN };
	int m_lISOSpeed{ 0 };
	int m_lGain{ -1 };
	double m_fExposure{ 0.0 };
	double m_fAperture{ 0.0 };
	bool m_bUnmodified{ false };
	bool m_bDone{ false };
	fs::path m_strOutputFile{};
	FRAMEINFOVECTOR m_vBitmaps{};
	MULTIBITMAPPROCESSMETHOD m_Method{ MBP_MEDIAN };
	double m_fKappa{ 2.0 };
	int m_lNrIterations{ 5 };
	std::shared_ptr<CMultiBitmap> m_pMaster{};

public:
//	CTaskInfo() = default;
//	CTaskInfo(const CTaskInfo&) = default;
//	CTaskInfo& operator=(const CTaskInfo&) = default;
//	~CTaskInfo() = default;

	void SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations);
	void CreateEmptyMaster(const CMemoryBitmap* pBitmap);
	void AddToMaster(CMemoryBitmap* pBitmap, ProgressBase* pProgress);
	std::shared_ptr<CMemoryBitmap> GetMaster(ProgressBase* const pProgress);
	bool HasISOSpeed() const;
};
