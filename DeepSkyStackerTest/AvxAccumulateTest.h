#pragma once

#include "../DeepSkyStacker/DSSCommon.h"

class CAllStackingTasks
{
public:
	static BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode();
	static BACKGROUNDCALIBRATIONINTERPOLATION GetBackgroundCalibrationInterpolation();
	static RGBBACKGROUNDCALIBRATIONMETHOD GetRGBBackgroundCalibrationMethod();
};
