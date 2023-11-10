#pragma once
#include "GrayBitmap.h"
#include "ColorBitmap.h"

namespace DSS { class ProgressBase; }

//////////////////////////////////////////////////////////////////////////
// Perhaps these classes here (CMedianFilterEngine based) should be in their own headers.
class CMedianFilterEngine
{
protected:
	int m_lFilterSize;
	DSS::ProgressBase* m_pProgress;

public:
	CMedianFilterEngine() :
		m_lFilterSize{ 1 },
		m_pProgress{ nullptr }
	{}

	virtual ~CMedianFilterEngine() {};

	virtual std::shared_ptr<CMemoryBitmap> GetFilteredImage(int lFilterSize, DSS::ProgressBase* pProgress) const = 0;
};

template <typename TType>
class CGrayMedianFilterEngineT : public CMedianFilterEngine
{
private:
	const CGrayBitmapT<TType>* m_pInBitmap;

public:
	CGrayMedianFilterEngineT() : m_pInBitmap{ nullptr }
	{};
	virtual ~CGrayMedianFilterEngineT() {};

	void SetInputBitmap(const CGrayBitmapT<TType>* pInBitmap)
	{
		m_pInBitmap = pInBitmap;
	}

	virtual std::shared_ptr<CMemoryBitmap> GetFilteredImage(const int lFilterSize, DSS::ProgressBase* pProgress) const override;
};

template <typename TType>
class CColorMedianFilterEngineT : public CMedianFilterEngine
{
private:
	const CColorBitmapT<TType>* m_pInBitmap;

public:
	CColorMedianFilterEngineT() : 
		m_pInBitmap{ nullptr }
	{};
	virtual ~CColorMedianFilterEngineT() {};

	void SetInputBitmap(const CColorBitmapT<TType>* pInBitmap)
	{
		m_pInBitmap = pInBitmap;
	}

	virtual std::shared_ptr<CMemoryBitmap> GetFilteredImage(int lFilterSize, DSS::ProgressBase* pProgress) const override;
};

template <class T>
class CopyableSmartPtr final
{
private:
	std::unique_ptr<T> p;
public:
	template <class OTHER> CopyableSmartPtr(std::unique_ptr<OTHER>&) = delete;
	CopyableSmartPtr() = delete;
	CopyableSmartPtr& operator=(const CopyableSmartPtr&) = delete;

	template <class OTHER>
	CopyableSmartPtr(std::unique_ptr<OTHER>&& rhs) : p{ std::move(rhs) } {}

	CopyableSmartPtr(const CopyableSmartPtr& rhs) : p{ rhs->clone() } {}

	typename std::unique_ptr<T>::pointer get() const noexcept { return p.get(); }
	typename std::unique_ptr<T>::pointer operator->() const noexcept { return this->get(); }
};


//////////////////////////////////////////////////////////////////////////

template <typename TType>
class CInternalMedianFilterEngineT
{
public :
	const TType* m_pvInValues;
	TType* m_pvOutValues;
	int m_lWidth;
	int m_lHeight;
	CFATYPE m_CFAType;
	int m_lFilterSize;


	class CFilterTask
	{
	private :
		CInternalMedianFilterEngineT<TType>* m_pEngine = nullptr;
		DSS::ProgressBase* m_pProgress = nullptr;

	public :
		CFilterTask(CInternalMedianFilterEngineT<TType>* peng, DSS::ProgressBase* pprg) :
			m_pEngine{ peng },
			m_pProgress{ pprg }
		{}

		~CFilterTask() = default;

		void process();
	private:
		void processNonAvx(const int lineStart, const int lineEnd, std::vector<TType>& vValues);
	};


	friend CFilterTask;

public:
	CInternalMedianFilterEngineT() :
		m_pvInValues { nullptr },
		m_pvOutValues{ nullptr },
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_CFAType {CFAT_NONE},
		m_lFilterSize{ 0 }
	{}

	virtual ~CInternalMedianFilterEngineT() {};

	void ApplyFilter(DSS::ProgressBase* pProgress);
};


template <typename TType>
inline std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<TType>::GetFilteredImage(const int lFilterSize, DSS::ProgressBase* pProgress) const
{
	if (m_pInBitmap == nullptr)
		return std::shared_ptr<CMemoryBitmap>{};

	// Create Output Bitmap from Input Bitmap
	std::shared_ptr<CGrayBitmapT<TType>> pOutBitmap = std::dynamic_pointer_cast<CGrayBitmapT<TType>>(static_cast<std::shared_ptr<CMemoryBitmap>>(m_pInBitmap->Clone()));

	if (static_cast<bool>(pOutBitmap))
	{
		CInternalMedianFilterEngineT<TType>	InternalFilter;

		InternalFilter.m_pvInValues  = m_pInBitmap->m_vPixels.data();
		InternalFilter.m_pvOutValues = pOutBitmap->m_vPixels.data();
		InternalFilter.m_lWidth      = m_pInBitmap->RealWidth();
		InternalFilter.m_lHeight	 = m_pInBitmap->RealHeight();
		InternalFilter.m_CFAType	 = m_pInBitmap->GetCFAType();
		InternalFilter.m_lFilterSize = InternalFilter.m_CFAType != CFATYPE_NONE ? lFilterSize * 2 : lFilterSize;

		InternalFilter.ApplyFilter(pProgress);

		return pOutBitmap;
	}

	return std::shared_ptr<CMemoryBitmap>{};
}


template <typename TType>
inline std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<TType>::GetFilteredImage(int lFilterSize, DSS::ProgressBase* pProgress) const
{
	if (m_pInBitmap == nullptr)
		return std::shared_ptr<CMemoryBitmap>{};

	// Create Output Bitmap from Input Bitmap
	std::shared_ptr<CColorBitmapT<TType>> pOutBitmap = std::dynamic_pointer_cast<CColorBitmapT<TType>>(static_cast<std::shared_ptr<CMemoryBitmap>>(m_pInBitmap->Clone()));

	if (static_cast<bool>(pOutBitmap))
	{
		CInternalMedianFilterEngineT<TType>	InternalFilter;

		InternalFilter.m_lWidth      = m_pInBitmap->RealWidth();
		InternalFilter.m_lHeight	 = m_pInBitmap->RealHeight();
		InternalFilter.m_lFilterSize = lFilterSize;
		InternalFilter.m_CFAType	 = CFATYPE_NONE;

		InternalFilter.m_pvInValues = m_pInBitmap->m_Red.m_vPixels.data();
		InternalFilter.m_pvOutValues = pOutBitmap->m_Red.m_vPixels.data();
		InternalFilter.ApplyFilter(pProgress);

		InternalFilter.m_pvInValues  = m_pInBitmap->m_Green.m_vPixels.data();
		InternalFilter.m_pvOutValues = pOutBitmap->m_Green.m_vPixels.data();
		InternalFilter.ApplyFilter(pProgress);

		InternalFilter.m_pvInValues  = m_pInBitmap->m_Blue.m_vPixels.data();
		InternalFilter.m_pvOutValues = pOutBitmap->m_Blue.m_vPixels.data();
		InternalFilter.ApplyFilter(pProgress);

		return pOutBitmap;
	}

	return std::shared_ptr<CMemoryBitmap>{};
}

