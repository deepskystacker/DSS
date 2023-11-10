#pragma once

#include "DSSCommon.h"
#include "dssrect.h"

class CAllStackingTasks
{
public:
	static BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode();
	static BACKGROUNDCALIBRATIONINTERPOLATION GetBackgroundCalibrationInterpolation();
	static RGBBACKGROUNDCALIBRATIONMETHOD GetRGBBackgroundCalibrationMethod();
};
