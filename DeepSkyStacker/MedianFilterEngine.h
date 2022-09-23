#ifndef __MEDIANFILTERENGINE_H__
#define __MEDIANFILTERENGINE_H__


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
		CDSSProgress* m_pProgress = nullptr;

	public :
		CFilterTask(CInternalMedianFilterEngineT<TType>* peng, CDSSProgress* pprg) :
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
		m_lWidth{ 0 },
		m_lHeight{ 0 },
		m_lFilterSize{ 0 }
	{}

	virtual ~CInternalMedianFilterEngineT() {};

	void ApplyFilter(CDSSProgress* pProgress);
};


template <typename TType>
inline std::shared_ptr<CMemoryBitmap> CGrayMedianFilterEngineT<TType>::GetFilteredImage(const int lFilterSize, CDSSProgress* pProgress) const
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
		InternalFilter.m_lWidth      = m_pInBitmap->m_lWidth;
		InternalFilter.m_lHeight	 = m_pInBitmap->m_lHeight;
		InternalFilter.m_CFAType	 = m_pInBitmap->m_CFAType;
		InternalFilter.m_lFilterSize = InternalFilter.m_CFAType != CFATYPE_NONE ? lFilterSize * 2 : lFilterSize;

		InternalFilter.ApplyFilter(pProgress);

		return pOutBitmap;
	}

	return std::shared_ptr<CMemoryBitmap>{};
}


template <typename TType>
inline std::shared_ptr<CMemoryBitmap> CColorMedianFilterEngineT<TType>::GetFilteredImage(int lFilterSize, CDSSProgress* pProgress) const
{
	if (m_pInBitmap == nullptr)
		return std::shared_ptr<CMemoryBitmap>{};

	// Create Output Bitmap from Input Bitmap
	std::shared_ptr<CColorBitmapT<TType>> pOutBitmap = std::dynamic_pointer_cast<CColorBitmapT<TType>>(static_cast<std::shared_ptr<CMemoryBitmap>>(m_pInBitmap->Clone()));

	if (static_cast<bool>(pOutBitmap))
	{
		CInternalMedianFilterEngineT<TType>	InternalFilter;

		InternalFilter.m_lWidth      = m_pInBitmap->m_lWidth;
		InternalFilter.m_lHeight	 = m_pInBitmap->m_lHeight;
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

#endif
