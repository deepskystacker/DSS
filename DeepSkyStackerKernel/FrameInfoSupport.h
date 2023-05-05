#pragma once
//#include "commonresource.h"
// inline void	ExposureToString(double fExposure, CString& strText)
// {
// 	// DELETE THIS ONE DAY
// 	if (fExposure)
// 	{
// 		int			lExposure;
// 
// 		if (fExposure >= 1)
// 		{
// 			lExposure = fExposure;
// 			std::uint32_t	dwRemainingTime = lExposure;
// 			std::uint32_t	dwHour,
// 				dwMin,
// 				dwSec;
// 
// 			dwHour = dwRemainingTime / 3600;
// 			dwRemainingTime -= dwHour * 3600;
// 			dwMin = dwRemainingTime / 60;
// 			dwRemainingTime -= dwMin * 60;
// 			dwSec = dwRemainingTime;
// 
// 			if (dwHour)
// 				strText.Format(IDS_EXPOSURETIME3, dwHour, dwMin, dwSec);
// 			else if (dwMin)
// 				strText.Format(IDS_EXPOSURETIME2, dwMin, dwSec);
// 			else
// 				strText.Format(IDS_EXPOSURETIME1, dwSec);
// 		}
// 		else
// 		{
// 			lExposure = 1.0 / fExposure + 0.5;
// 			strText.Format(IDS_EXPOSUREFORMAT_INF, lExposure);
// 		};
// 	}
// 	else
// 		strText = "-";
// }

inline QString exposureToString(double fExposure)
{
	QString strText;

	if (fExposure)
	{
		qint64			exposure;

		if (fExposure >= 1)
		{
			exposure = fExposure;
			qint64			remainingTime = exposure;
			qint64			hours, mins, secs;

			hours = remainingTime / 3600;
			remainingTime -= hours * 3600;
			mins = remainingTime / 60;
			remainingTime -= mins * 60;
			secs = remainingTime;

			if (hours)
				strText = QCoreApplication::translate("StackRecap", "%1 hr %2 mn %3 s", "IDS_EXPOSURETIME3")
				.arg(hours)
				.arg(mins)
				.arg(secs);
			else if (mins)
				strText = QCoreApplication::translate("StackRecap", "%1 mn %2 s", "IDS_EXPOSURETIME2")
				.arg(mins)
				.arg(secs);
			else
				strText = QCoreApplication::translate("StackRecap", "%1 s", "IDS_EXPOSURETIME1")
				.arg(secs);
		}
		else
		{
			exposure = 1.0 / fExposure + 0.5;
			strText = QCoreApplication::translate("StackRecap", "1/%1 s", "IDS_EXPOSUREFORMAT_INF")
				.arg(exposure);
		};
	}
	else
		strText = "-";

	return strText;
}

inline void	ISOToString(int lISOSpeed, QString& strText)
{
	if (lISOSpeed)
		strText = QString::number(lISOSpeed);
	else
		strText = "-";
}

inline void	GainToString(int lGain, QString& strText)
{
	if (lGain >= 0)
		strText = QString::number(lGain);
	else
		strText = "-";
}
