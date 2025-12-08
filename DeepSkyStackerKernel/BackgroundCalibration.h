#pragma once
#include "DSSCommon.h"
#include "RationalInterpolation.h"
#include "LinearInterpolationh.h"

class CMemoryBitmap;
namespace DSS { class OldProgressBase; }
using namespace DSS;

class CBackgroundCalibration
{
public:
	bool				m_bInitOk;
	double				m_fTgtRedBk{ 0.0 },
						m_fTgtGreenBk{ 0.0 },
						m_fTgtBlueBk{ 0.0 };
	double				m_fSrcRedBk{ 0.0 },
						m_fSrcGreenBk{ 0.0 },
						m_fSrcBlueBk{ 0.0 };

	double				m_fSrcRedMax{ 0.0 },
						m_fSrcGreenMax{ 0.0 },
						m_fSrcBlueMax{ 0.0 };

	BACKGROUNDCALIBRATIONMODE			m_BackgroundCalibrationMode;
	BACKGROUNDCALIBRATIONINTERPOLATION	m_BackgroundInterpolation;
	RGBBACKGROUNDCALIBRATIONMETHOD		m_RGBBackgroundMethod;

	double				m_fMultiplier;

	CRationalInterpolation	m_riRed;
	CRationalInterpolation	m_riGreen;
	CRationalInterpolation	m_riBlue;

	CLinearInterpolation	m_liRed;
	CLinearInterpolation	m_liGreen;
	CLinearInterpolation	m_liBlue;

public:
	CBackgroundCalibration();

	CBackgroundCalibration(const CBackgroundCalibration& bc) = default;

	virtual ~CBackgroundCalibration() = default;

	CBackgroundCalibration& operator=(const CBackgroundCalibration& bc) = default;

	void SetMultiplier(double fMultiplier)
	{
		m_fMultiplier = fMultiplier;
	}

	void SetMode(BACKGROUNDCALIBRATIONMODE BackgroundCalibrationMode,
				BACKGROUNDCALIBRATIONINTERPOLATION BackgroundInterpolation,
				RGBBACKGROUNDCALIBRATIONMETHOD RGBBackgroundMethod)
	{
		m_BackgroundCalibrationMode	= BackgroundCalibrationMode;
		m_BackgroundInterpolation	= BackgroundInterpolation;
		m_RGBBackgroundMethod		= RGBBackgroundMethod;
	}
	void ComputeBackgroundCalibration(const CMemoryBitmap* pBitmap, const char8_t* pFileName, bool bFirst, OldProgressBase* pProgress);

	template <class T>
	void ApplyCalibration(T& fRed, T& fGreen, T& fBlue) const
	{
		static_assert(std::is_same_v<T, float> || std::is_same_v<T, double>);

		if (m_BackgroundInterpolation == BCI_RATIONAL)
		{
			fRed = m_riRed.Interpolate(fRed);
			fGreen = m_riGreen.Interpolate(fGreen);
			fBlue = m_riBlue.Interpolate(fBlue);
		}
		else
		{
			fRed = m_liRed.Interpolate(fRed);
			fGreen = m_liGreen.Interpolate(fGreen);
			fBlue = m_liBlue.Interpolate(fBlue);
		}
	}

private:
	static constexpr size_t HistogramSize() { return std::numeric_limits<std::uint16_t>::max() + size_t{ 1 }; }
	void ompCalcHistogram(const CMemoryBitmap* pBitmap, OldProgressBase* pProgress, std::vector<int>& redHisto, std::vector<int>& greenHisto, std::vector<int>& blueHisto) const;
};
