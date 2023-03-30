#pragma once
#include "DSSCommon.h"
#include "RationalInterpolation.h"
#include "LinearInterpolationh.h"

class CMemoryBitmap;
namespace DSS { class ProgressBase; }
using namespace DSS;

class CBackgroundCalibration
{
public:
	bool				m_bInitOk;
	double				m_fTgtRedBk,
						m_fTgtGreenBk,
						m_fTgtBlueBk;
	double				m_fSrcRedBk,
						m_fSrcGreenBk,
						m_fSrcBlueBk;

	double				m_fSrcRedMax,
						m_fSrcGreenMax,
						m_fSrcBlueMax;

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
	void ComputeBackgroundCalibration(CMemoryBitmap* pBitmap, bool bFirst, ProgressBase* pProgress);

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
	void ompCalcHistogram(CMemoryBitmap* pBitmap, ProgressBase* pProgress, std::vector<int>& redHisto, std::vector<int>& greenHisto, std::vector<int>& blueHisto) const;
};
