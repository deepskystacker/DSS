#pragma once
#include "GrayBitmap.h"

class CColorBitmap
{
public:
	CColorBitmap()
	{}

	virtual ~CColorBitmap()
	{}

	virtual CMemoryBitmap* GetRed() = 0;
	virtual CMemoryBitmap* GetGreen() = 0;
	virtual CMemoryBitmap* GetBlue() = 0;
};

template <typename TType>
class CColorBitmapT : public CMemoryBitmap, public CColorBitmap
{
	//friend CColorMedianFilterEngineT<TType>;
public:
	CGrayBitmapT<TType> m_Red;
	CGrayBitmapT<TType> m_Green;
	CGrayBitmapT<TType> m_Blue;

	CColorBitmapT();
	virtual ~CColorBitmapT() = default;

private:
	static consteval double initMultiplier()
	{
		if (std::is_same_v<TType, std::uint16_t> || std::is_same_v<TType, double> || std::is_same_v<TType, float>)
			return 256.0;			// Range of [0.0, 65535.0]
		else if (std::is_same_v<TType, std::uint32_t>)
			return 256.0 * 65536.0;	// Range of [0.0, 65535.0]
		else
			return 1.0;				// Range of [0.0, 255.0]
	};

	static consteval double initClamp()
	{
		if (std::is_same_v<TType, double>)
			return std::numeric_limits<double>::max();
		else if (std::is_same_v<TType, float>)
			return std::numeric_limits<float>::max();
		else if (std::is_same_v<TType, std::uint16_t>)
			return static_cast<double>(std::numeric_limits<std::uint16_t>::max());	// Range of [0.0, 65535.0]
		else if (std::is_same_v<TType, std::uint32_t>)
			return static_cast<double>(std::numeric_limits<std::uint32_t>::max());	// Range of [0.0, 4294967295.0 ] 
		else
			return static_cast<double>(std::numeric_limits<std::uint8_t>::max());	// Range of [0.0, 255.0]
	};

	int m_lHeight;
	int m_lWidth;
	constinit inline static bool m_bWord{ std::is_same_v<TType, std::uint16_t> };
	constinit inline static bool m_bDouble{ std::is_same_v<TType, double> };
	constinit inline static bool m_bDWord{ std::is_same_v<TType, std::uint32_t> };
	constinit inline static bool m_bFloat{ std::is_same_v<TType, float> };
	double m_fMultiplier{ initMultiplier() };
	constexpr static double clampValue{ initClamp() };

public:
	void CheckXY(size_t x, size_t y) const;

	size_t GetOffset(const size_t x, const size_t y) const;
	size_t GetOffset(int x, int y) const;



	virtual std::unique_ptr<CMemoryBitmap> Clone(bool bEmpty = false) const override;

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
		return m_lWidth;
	}

	virtual int Height() const override
	{
		return m_lHeight;
	}

	virtual bool Init(int lWidth, int lHeight) override;

	virtual bool IsMonochrome() const override
	{
		return false;
	}

	virtual std::tuple<double, double, double> getValues(size_t, size_t) const override;

	virtual void SetValue(size_t i, size_t j, double fRed, double fGreen, double fBlue) override;
	virtual void GetValue(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) const override;
	virtual void SetPixel(size_t i, size_t j, double fRed, double fGreen, double fBlue) override;
	virtual void SetPixel(size_t i, size_t j, double fGray) override;
	virtual void GetPixel(size_t i, size_t j, double& fRed, double& fGreen, double& fBlue) override;
	virtual void GetPixel(size_t i, size_t j, double& fGray) override;
	virtual bool GetScanLine(size_t j, void* pScanLine) override;
	virtual bool SetScanLine(size_t j, void* pScanLine) override;
	void Clear();

	virtual std::shared_ptr<CMultiBitmap> CreateEmptyMultiBitmap() const override;
	virtual std::shared_ptr<CMedianFilterEngine> GetMedianFilterEngine() const override;

	TType* GetRedPixel(int i, int j)
	{
		return m_Red.m_vPixels.data() + GetOffset(i, j);
	}
	TType* GetGreenPixel(int i, int j)
	{
		return m_Green.m_vPixels.data() + GetOffset(i, j);
	}
	TType* GetBluePixel(int i, int j)
	{
		return m_Blue.m_vPixels.data() + GetOffset(i, j);
	}

	virtual CMemoryBitmap* GetRed() override
	{
		return &m_Red;
	}

	virtual CMemoryBitmap* GetGreen() override
	{
		return &m_Green;
	}

	virtual CMemoryBitmap* GetBlue() override
	{
		return &m_Blue;
	}

	double GetMultiplier()
	{
		return m_fMultiplier;
	}

	virtual double GetMaximumValue() const override
	{
		return m_fMultiplier * 256.0;
	}

	virtual void RemoveHotPixels(ProgressBase* pProgress = nullptr) override;
	virtual void GetCharacteristics(CBitmapCharacteristics& bc) override;

	virtual void InitIterator(void*& pRed, void*& pGreen, void*& pBlue, size_t& elementSize, const size_t x, const size_t y) override;
	virtual void InitIterator(const void*& pRed, const void*& pGreen, const void*& pBlue, size_t& elementSize, const size_t x, const size_t y) const override;

	virtual std::tuple<double, double, double> ConvertValue3(const void* pRed, const void* pGreen, const void* pBlue) const override;
	virtual double ConvertValue1(const void* pRed, const void* pGreen, const void* pBlue) const override;

	virtual void ReceiveValue(void* pRed, void* pGreen, void* pBlue, const double red, const double green, const double blue) const override;
	virtual void ReceiveValue(void* pRed, void* pGreen, void* pBlue, const double gray) const override;
};


typedef CColorBitmapT<std::uint8_t> C24BitColorBitmap;
typedef CColorBitmapT<std::uint16_t> C48BitColorBitmap;
typedef CColorBitmapT<std::uint32_t> C96BitColorBitmap;
typedef CColorBitmapT<float> C96BitFloatColorBitmap;
