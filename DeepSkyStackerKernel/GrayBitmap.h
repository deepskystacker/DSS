#pragma once
#include "MemoryBitmap.h"
#include "CFABitmapInfo.h"

namespace DSS { class ProgressDlg; }
using namespace DSS;

namespace {
	template <typename TType>
	consteval double initMultiplier()
	{
		if constexpr (std::is_same_v<TType, std::uint16_t> || std::is_same_v<TType, double> || std::is_same_v<TType, float>)
			return 256.0;			// Range of [0.0, 65535.0]
		else if constexpr (std::is_same_v<TType, std::uint32_t>)
			return 256.0 * 65536.0;	// Range of [0.0, 65535.0]
		else
			return 1.0;				// Range of [0.0, 255.0]
	}

	template <typename TType>
	consteval double initClamp()
	{
		if constexpr (std::is_same_v<TType, double>)
			return std::numeric_limits<double>::max();
		else if constexpr (std::is_same_v<TType, float>)
			return std::numeric_limits<float>::max();
		else if constexpr (std::is_same_v<TType, std::uint16_t>)
			return static_cast<double>(std::numeric_limits<std::uint16_t>::max());	// Range of [0.0, 65535.0]
		else if constexpr (std::is_same_v<TType, std::uint32_t>)
			return static_cast<double>(std::numeric_limits<std::uint32_t>::max());	// Range of [0.0, 4294967295.0 ] 
		else
			return static_cast<double>(std::numeric_limits<std::uint8_t>::max());	// Range of [0.0, 255.0]
	};
}

template <typename TType>
class CGrayBitmapT : public CMemoryBitmap, public CCFABitmapInfo
{
public:
	//friend CColorBitmapT<TType>;
	//friend CGrayMedianFilterEngineT<TType>;
	std::vector<TType>	m_vPixels;

	CGrayBitmapT();
	virtual ~CGrayBitmapT() = default;

private:
	int m_lHeight;
	int m_lWidth;
	constexpr static bool m_bWord{ std::is_same_v<TType, std::uint16_t> };
	constexpr static bool m_bFloat{ std::is_same_v<TType, float> };
	constexpr static double clampValue{ initClamp<TType>() };

	double m_fMultiplier{ initMultiplier<TType>() };


	bool InitInternals();

	void CheckXY(size_t x, size_t y) const;

	inline bool	IsXYOk(size_t x, size_t y) const
	{
		return (x >= 0 && x < m_lWidth && y >= 0 && y < m_lHeight);
	}

	size_t GetOffset(const size_t x, const size_t y) const
	{
		return static_cast<size_t>(m_lWidth) * y + x;
	}
	size_t GetOffset(int x, int y) const
	{
		CheckXY(x, y);
		return GetOffset(static_cast<size_t>(x), static_cast<size_t>(y));
	}

	virtual bool IsMonochrome() const override
	{
		return true;
	}

	TType GetPrimary(int x, int y, const COLORREF16& crColor) const;
	double GetPrimary(size_t x, size_t y, double fRed, double fGreen, double fBlue) const;
	double InterpolateGreen(size_t x, size_t y, const TType* pValue = nullptr) const;
	double InterpolateBlue(size_t x, size_t y, const TType* pValue = nullptr) const;
	double InterpolateRed(size_t x, size_t y, const TType* pValue = nullptr) const;
	void InterpolateAll(double* pfValues, size_t x, size_t y) const;

protected:
	virtual void SetCFA(bool bCFA) override
	{
		m_bCFA = bCFA;
	}

public:

	void SetMultiplier(double fMultiplier)
	{
		m_fMultiplier = fMultiplier;
	}

	virtual bool Init(int lWidth, int lHeight) override
	{
		m_lWidth = lWidth;
		m_lHeight = lHeight;
		return InitInternals();
	}

	virtual std::unique_ptr<CMemoryBitmap> Clone(bool bEmpty = false) const override;
	virtual BAYERCOLOR GetBayerColor(int x, int y) const override;

	virtual CFATYPE	GetCFAType() const override
	{
		return CCFABitmapInfo::GetCFAType();
	}

	virtual int BitPerSample() const override
	{
		return sizeof(TType) * 8;
	}

	virtual bool IsFloat() const override
	{
		return m_bFloat;
	}

	virtual int Width() const override
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
			return m_lWidth / 2;
		else
			return m_lWidth;
	}

	virtual int Height() const override
	{
		if (m_CFATransform == CFAT_SUPERPIXEL)
			return m_lHeight / 2;
		else
			return m_lHeight;
	}

	virtual int RealHeight() const override
	{
		return m_lHeight;
	}

	virtual int RealWidth() const override
	{
		return m_lWidth;
	}

	virtual void SetValue(size_t i, size_t j, double fGray) override
	{
		CheckXY(i, j);
		m_vPixels[GetOffset(i, j)] = fGray;
	}

	virtual void GetValue(size_t i, size_t j, double& fGray) const override
	{
		//if (CFAT_SUPERPIXEL == m_CFATransform)  Bug fix 15th August 2020
		//{
		//	i *= 2; j *= 2;
		//}
		CheckXY(i, j);
		fGray = m_vPixels[GetOffset(i, j)];
	}

	virtual double getValue(size_t i, size_t j) const override
	{
		CheckXY(i, j);
		return m_vPixels[GetOffset(i, j)];
	}
	double getUncheckedValue(const size_t x, const size_t y) const
	{
		return m_vPixels[GetOffset(x, y)]; // Note: the GetOffset function with size_t arguments makes no CheckXy call.
	}

	virtual void SetPixel(size_t i, size_t j, double fRed, double fGreen, double fBlue) override;
	virtual void SetPixel(size_t i, size_t j, double fGray) override;
	virtual void GetPixel(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) const override;
	virtual void GetPixel(size_t i, size_t j, double& fGray) const override;
	virtual bool GetScanLine(size_t j, void* pScanLine) const override;
	virtual bool SetScanLine(size_t j, void* pScanLine) override;
	virtual std::shared_ptr<CMultiBitmap> CreateEmptyMultiBitmap() const override;

	virtual void RemoveHotPixels(ProgressBase* pProgress = nullptr) override;

	TType* GetGrayPixel(const int i, const int j)
	{
		return m_vPixels.data() + GetOffset(i, j);
	}
	const TType* GetGrayPixel(const int i, const int j) const
	{
		return m_vPixels.data() + GetOffset(i, j);
	}

	double GetMultiplier() const
	{
		return m_fMultiplier;
	}

	virtual std::shared_ptr<CMedianFilterEngine> GetMedianFilterEngine() const override;

	virtual double GetMaximumValue() const override
	{
		return (m_fMultiplier * 256.0) - 1;
	}

	virtual void GetCharacteristics(CBitmapCharacteristics& bc) const override;

	virtual void InitIterator(void*& pRed, void*& pGreen, void*& pBlue, size_t& elementSize, const size_t x, const size_t y) override;
	virtual void InitIterator(const void*& pRed, const void*& pGreen, const void*& pBlue, size_t& elementSize, const size_t x, const size_t y) const override;

	virtual std::tuple<double, double, double> ConvertValue3(const void* pRed, const void*, const void*) const override
	{
		const double value = static_cast<double>(*static_cast<const TType*>(pRed)) / this->m_fMultiplier;
		return { value, value, value };
	}
	virtual double ConvertValue1(const void* pRed, const void*, const void*) const override
	{
		return static_cast<double>(*static_cast<const TType*>(pRed)) / this->m_fMultiplier;
	}

	virtual void ReceiveValue(void*, void*, void*, const double, const double, const double) const override
	{}
	virtual void ReceiveValue(void* pRed, void*, void*, const double gray) const override
	{
		*static_cast<TType*>(pRed) = static_cast<TType>(gray * this->m_fMultiplier);
	}
};


typedef CGrayBitmapT<double> CGrayBitmap;
typedef CGrayBitmapT<std::uint8_t> C8BitGrayBitmap;
typedef CGrayBitmapT<std::uint16_t> C16BitGrayBitmap;
typedef CGrayBitmapT<std::uint32_t> C32BitGrayBitmap;
typedef CGrayBitmapT<float> C32BitFloatGrayBitmap;
