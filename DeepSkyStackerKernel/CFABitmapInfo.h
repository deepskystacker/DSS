#pragma once
#include "BitmapConstants.h"


class CCFABitmapInfo
{
protected:
	CFATRANSFORMATION m_CFATransform;
	CFATYPE m_CFAType;
	bool m_bCYMG;
	int m_xBayerOffset;
	int m_yBayerOffset;

protected:
	virtual void SetCFA(bool bCFA) = 0;

public:
	void	InitFrom(const CCFABitmapInfo* const pCFABitmapInfo)
	{
		m_CFATransform = pCFABitmapInfo->m_CFATransform;
		m_CFAType = pCFABitmapInfo->m_CFAType;
		m_bCYMG = pCFABitmapInfo->m_bCYMG;
		m_xBayerOffset = pCFABitmapInfo->m_xBayerOffset;
		m_yBayerOffset = pCFABitmapInfo->m_yBayerOffset;
	};

public:
	CCFABitmapInfo() :
		m_CFATransform{ CFAT_NONE },
		m_CFAType{ CFATYPE_NONE },
		m_bCYMG{ false },
		m_xBayerOffset{ 0 },
		m_yBayerOffset{ 0 }
	{}

	void	SetCFAType(CFATYPE Type)
	{
		m_CFAType = Type;
		m_bCYMG = IsCYMGType(m_CFAType);
	};

	CCFABitmapInfo& setXoffset(int xOffset) noexcept
	{
		m_xBayerOffset = xOffset;
		return *this;
	};

	inline int xOffset() const noexcept
	{
		return m_xBayerOffset;
	}

	CCFABitmapInfo& setYoffset(int yOffset) noexcept
	{
		m_yBayerOffset = yOffset;
		return *this;
	};

	inline int yOffset() const noexcept
	{
		return m_yBayerOffset;
	}

	inline CFATYPE GetCFAType() const noexcept
	{
		return m_CFAType;
	};

	void	UseSuperPixels(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_SUPERPIXEL;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseRawBayer(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_RAWBAYER;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseBilinear(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_BILINEAR;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	void	UseAHD(bool bUse)
	{
		if (bUse)
		{
			m_CFATransform = CFAT_AHD;
			SetCFA(true);
		}
		else
			m_CFATransform = CFAT_NONE;
	};

	CFATRANSFORMATION GetCFATransformation() const
	{
		return m_CFATransform;
	};
};
