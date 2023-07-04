#pragma once

#include "DSSCommon.h"
#include "FrameInfo.h"

class CMultiBitmap;
class CMemoryBitmap;
namespace DSS { class ProgressBase; }
using namespace DSS;
class CTaskInfo
{
public:
	std::uint32_t m_dwTaskID;
	std::uint32_t m_groupID;
	PICTURETYPE m_TaskType;
	int m_lISOSpeed;
	int m_lGain;
	double m_fExposure;
	double m_fAperture;
	bool m_bUnmodified;
	bool m_bDone;
	fs::path m_strOutputFile;
	FRAMEINFOVECTOR m_vBitmaps;
	MULTIBITMAPPROCESSMETHOD m_Method;
	double m_fKappa;
	int m_lNrIterations;
	std::shared_ptr<CMultiBitmap> m_pMaster;

public:
	CTaskInfo();
	CTaskInfo(const CTaskInfo&) = default;
	CTaskInfo& operator=(const CTaskInfo&) = default;
	virtual ~CTaskInfo() = default;

	void SetMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations);
	void CreateEmptyMaster(const CMemoryBitmap* pBitmap);
	void AddToMaster(CMemoryBitmap* pBitmap, ProgressBase* pProgress);
	std::shared_ptr<CMemoryBitmap> GetMaster(ProgressBase* const pProgress);
	bool HasISOSpeed() const;
};
