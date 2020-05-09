#ifndef __BACKGROUNDCALIBRATION_H__
#define __BACKGROUNDCALIBRATION_H__

/* ------------------------------------------------------------------- */
class CBackgroundCalibrationTask;

#include "StackingTasks.h"

class CBackgroundCalibration
{
friend CBackgroundCalibrationTask;

public :
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

private :
	/*
	void	ApplyCalibrationInternal(float & fColor, double fBk, double fMax)
	{
		if (fColor < fBk)
		{
			// Adjust between 0 and m_fTgtBk
			fColor *= m_fTgtBk/fBk;
		}
		else
		{
			// Adjust between fTgtBk and fMax;
			fColor-=fBk;
			fColor *= (fMax-m_fTgtBk)/(fMax-fBk);
			fColor += m_fTgtBk;
		};
	};

	void	ApplyCalibrationInternal(double & fColor, double fBk, double fMax)
	{
		if (fColor < fBk)
		{
			// Adjust between 0 and m_fTgtBk
			fColor *= m_fTgtBk/fBk;
		}
		else
		{
			// Adjust between fTgtBk and fMax;
			fColor-=fBk;
			fColor *= (fMax-m_fTgtBk)/(fMax-fBk);
			fColor += m_fTgtBk;
		};
	};*/

	void	CopyFrom(const CBackgroundCalibration & bc)
	{
		m_bInitOk		= bc.m_bInitOk;
		//m_fTgtBk		= bc.m_fTgtBk;
		m_fSrcRedBk		= bc.m_fSrcRedBk;
		m_fSrcGreenBk	= bc.m_fSrcGreenBk;
		m_fSrcBlueBk	= bc.m_fSrcBlueBk;
		m_fSrcRedMax	= bc.m_fSrcRedMax;
		m_fSrcGreenMax	= bc.m_fSrcGreenMax;
		m_fSrcBlueMax	= bc.m_fSrcBlueMax;
		m_fMultiplier	= bc.m_fMultiplier;
		m_BackgroundCalibrationMode	= bc.m_BackgroundCalibrationMode;
		m_BackgroundInterpolation   = bc.m_BackgroundInterpolation;
		m_RGBBackgroundMethod		= bc.m_RGBBackgroundMethod;
		m_riRed			= bc.m_riRed;
		m_riBlue		= bc.m_riBlue;
		m_riGreen		= bc.m_riGreen;

		m_liRed			= bc.m_liRed;
		m_liBlue		= bc.m_liBlue;
		m_liGreen		= bc.m_liGreen;

        m_fTgtRedBk     = bc.m_fTgtRedBk;
        m_fTgtGreenBk   = bc.m_fTgtGreenBk;
        m_fTgtBlueBk    = bc.m_fTgtBlueBk;
	};

public :
	CBackgroundCalibration()
	{
		m_bInitOk = false;
		m_fMultiplier = 1.0;
		m_BackgroundCalibrationMode = CAllStackingTasks::GetBackgroundCalibrationMode();
		m_BackgroundInterpolation	= CAllStackingTasks::GetBackgroundCalibrationInterpolation();
		m_RGBBackgroundMethod		= CAllStackingTasks::GetRGBBackgroundCalibrationMethod();
	};

	CBackgroundCalibration(const CBackgroundCalibration & bc)
	{
		CopyFrom(bc);
	};

	virtual ~CBackgroundCalibration()
	{
	};

	CBackgroundCalibration & operator = (const CBackgroundCalibration & bc)
	{
		CopyFrom(bc);
		return (*this);
	};

	void	SetMultiplier(double fMultiplier)
	{
		m_fMultiplier = fMultiplier;
	};

	void	SetMode(BACKGROUNDCALIBRATIONMODE BackgroundCalibrationMode,
					BACKGROUNDCALIBRATIONINTERPOLATION BackgroundInterpolation,
					RGBBACKGROUNDCALIBRATIONMETHOD RGBBackgroundMethod)
	{
		m_BackgroundCalibrationMode	= BackgroundCalibrationMode;
		m_BackgroundInterpolation	= BackgroundInterpolation;
		m_RGBBackgroundMethod		= RGBBackgroundMethod;
	};
	void	ComputeBackgroundCalibration(CMemoryBitmap * pBitmap, bool bFirst, CDSSProgress * pProgress);
	void	ApplyCalibration(float & fRed, float & fGreen, float & fBlue)
	{
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
		};
		/*ApplyCalibrationInternal(fRed,		m_fSrcRedBk,	m_fSrcRedMax);
		ApplyCalibrationInternal(fGreen,	m_fSrcGreenBk,	m_fSrcGreenMax);
		ApplyCalibrationInternal(fBlue,		m_fSrcBlueBk,	m_fSrcBlueMax);	*/
	};
	void	ApplyCalibration(double & fRed, double & fGreen, double & fBlue)
	{
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
		};

		/*ApplyCalibrationInternal(fRed,		m_fSrcRedBk,	m_fSrcRedMax);
		ApplyCalibrationInternal(fGreen,	m_fSrcGreenBk,	m_fSrcGreenMax);
		ApplyCalibrationInternal(fBlue,		m_fSrcBlueBk,	m_fSrcBlueMax);	*/
	};
};

#endif // __BACKGROUNDCALIBRATION_H__