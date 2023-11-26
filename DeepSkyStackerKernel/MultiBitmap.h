#pragma once
#include "DSSCommon.h"
#include "BitmapPartFile.h"

namespace DSS { class ProgressBase; }
using namespace DSS;
class CMemoryBitmap;
class CMultiBitmap
{
protected:
	std::shared_ptr<CMemoryBitmap> m_pBitmapModel;
	mutable std::shared_ptr<CMemoryBitmap> m_pHomBitmap;
	MULTIBITMAPPROCESSMETHOD  m_Method;
	double m_fKappa;
	int m_lNrIterations;
	int m_lNrBitmaps;
	int m_lNrAddedBitmaps;
	std::vector<CBitmapPartFile> m_vFiles;
	int m_lWidth;
	int m_lHeight;
	std::atomic_bool m_bInitDone;
	bool m_bHomogenization;
	double m_fMaxWeight;
	std::vector<int> m_vImageOrder;

private:
	void	removeTempFiles();
	void	InitParts();
	std::shared_ptr<CMemoryBitmap> SmoothOut(CMemoryBitmap* pBitmap, ProgressBase* const pProgress) const;

public:
	CMultiBitmap() :
		m_lNrBitmaps{ 0 },
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_bInitDone{ false },
		m_lNrAddedBitmaps{ 0 },
		m_bHomogenization{ false },
		m_fMaxWeight{ 0 },
		m_Method{ MULTIBITMAPPROCESSMETHOD{0} },
		m_fKappa{ 0.0 },
		m_lNrIterations{ 0 }
	{}

	virtual ~CMultiBitmap()
	{
		removeTempFiles();
	};

	void SetBitmapModel(const CMemoryBitmap* pBitmap);
	virtual bool SetScanLines(CMemoryBitmap* pBitmap, int lLine, const std::vector<void*>& vScanLines) = 0;
	virtual std::shared_ptr<CMemoryBitmap> CreateNewMemoryBitmap() const = 0;
	virtual std::shared_ptr<CMemoryBitmap> CreateOutputMemoryBitmap() const = 0;

	void SetNrBitmaps(int lNrBitmaps)
	{
		m_lNrBitmaps = lNrBitmaps;
	}

	int GetNrBitmaps() const
	{
		return m_lNrBitmaps;
	}

	int GetNrAddedBitmaps() const
	{
		return m_lNrAddedBitmaps;
	}

	void SetImageOrder(const std::vector<int>& vImageOrder)
	{
		m_vImageOrder = vImageOrder;
	}

	const std::vector<int>& GetImageOrder() const
	{
		return this->m_vImageOrder;
	}

	virtual bool AddBitmap(CMemoryBitmap* pMemoryBitmap, ProgressBase* pProgress = nullptr);
	virtual std::shared_ptr<CMemoryBitmap> GetResult(ProgressBase* pProgress = nullptr);
	virtual int GetNrChannels() const = 0;
	virtual int GetNrBytesPerChannel() const = 0;

	void SetProcessingMethod(MULTIBITMAPPROCESSMETHOD Method, double fKappa, int lNrIterations)
	{
		m_Method = Method;
		m_fKappa = fKappa;
		m_lNrIterations = lNrIterations;
	}

	void SetHomogenization(bool bSet)
	{
		m_bHomogenization = bSet;
	}

	bool GetHomogenization() const
	{
		return m_bHomogenization;
	}

	CMemoryBitmap* GetHomogenBitmap() const
	{
		if (static_cast<bool>(this->m_pHomBitmap))
			return m_pHomBitmap.get();
		return nullptr;
	}

	int GetProcessingMethod() const
	{
		return m_Method;
	}

	auto GetProcessingParameters() const
	{
		return std::make_tuple(m_fKappa, m_lNrIterations);
	}
};