#pragma once

#include "../DeepSkyStacker/DSSCommon.h"
#include "../DeepSkyStacker/dssrect.h"

class CAllStackingTasks
{
public:
	static BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode();
	static BACKGROUNDCALIBRATIONINTERPOLATION GetBackgroundCalibrationInterpolation();
	static RGBBACKGROUNDCALIBRATIONMETHOD GetRGBBackgroundCalibrationMethod();
};
