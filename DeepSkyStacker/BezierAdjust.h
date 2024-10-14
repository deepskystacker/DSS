#pragma once
/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
**
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
/* ------------------------------------------------------------------- */
//#undef PI
#include "DSSCommon.h"

namespace DSS
{
	class BezierCurvePoint
	{
	public:
		double x;
		double y;

	public:
		BezierCurvePoint(double nx = 0, double ny = 0) :
			x{ nx },
			y{ ny }
		{
		};

		BezierCurvePoint(const BezierCurvePoint& rhs) = default;

		virtual ~BezierCurvePoint() {};

		BezierCurvePoint& operator = (const BezierCurvePoint& rhs) = default;

		bool operator < (const BezierCurvePoint& rhs) const
		{
			return x < rhs.x;
		}
	};

	typedef std::vector<BezierCurvePoint>::iterator	CurvePointIterator;

	class BezierAdjust
	{
	public:

		double
			m_fDarknessAngle,
			m_fDarknessPower,
			m_fMidtone,
			m_fMidtoneAngle,
			m_fHighlightAngle,
			m_fHighlightPower;
		double 	m_fSaturationShift;

		std::vector<BezierCurvePoint> curvePoints;

		BezierAdjust()
		{
			reset();
		}

		BezierAdjust(const BezierAdjust& rhs) = default;
		virtual ~BezierAdjust() {};
		BezierAdjust& operator = (const BezierAdjust& rhs) = default;

		void	clear()
		{
			init();
		}

		void reset(bool bNeutral = false)
		{
			if (bNeutral)
			{
				m_fDarknessAngle = 0;
				m_fDarknessPower = 0;
				m_fMidtone = 50;
				m_fMidtoneAngle = 45;
				m_fHighlightAngle = 0;
				m_fHighlightPower = 0;
				m_fSaturationShift = 20;
			}
			else
			{
				m_fDarknessAngle = 0;
				m_fDarknessPower = 80;
				m_fMidtone = 33;
				m_fMidtoneAngle = 10;
				m_fHighlightAngle = 0;
				m_fHighlightPower = 50;
				m_fSaturationShift = 20;
			};
			clear();
		}

		double	GetValue(double x)
		{
			double			fResult = x;

			if (!curvePoints.size())
				init();

			CurvePointIterator	it;

			it = std::lower_bound(curvePoints.begin(), curvePoints.end(), BezierCurvePoint(x, 0));
			if (it != curvePoints.end())
				fResult = it->y;

			return fResult;
		}

		double	AdjustSaturation(double S)
		{
			double			fResult = S;

			if (m_fSaturationShift > 0)
				fResult = pow(S, 1.0 / (m_fSaturationShift / 10.0));
			else if (m_fSaturationShift < 0)
				fResult = pow(S, -m_fSaturationShift / 10.0);

			return fResult;
		}

		void loadSettings(const QString& group)
		{
			ZFUNCTRACE_RUNTIME();
			QString groupName{ group + "/BezierAdjust" };
			QSettings settings;
			settings.beginGroup(groupName);
			m_fDarknessAngle = settings.value("DarkAngle").toDouble();
			m_fDarknessPower = settings.value("DarkPower").toDouble();
			m_fMidtone = settings.value("Midtone").toDouble();
			m_fMidtoneAngle = settings.value("MidtoneAngle").toDouble();
			m_fHighlightAngle = settings.value("HighlightAngle").toDouble();
			m_fHighlightPower = settings.value("HighlightPower").toDouble();
		}

		void saveSettings(const QString& group) const
		{
			ZFUNCTRACE_RUNTIME();
			QString groupName{ group + "/BezierAdjust" };
			QSettings settings;
			settings.beginGroup(groupName);
			settings.setValue("DarkAngle", m_fDarknessAngle);
			settings.setValue("DarkPower", m_fDarknessPower);
			settings.setValue("Midtone", m_fMidtone);
			settings.setValue("MidtoneAngle", m_fMidtoneAngle);
			settings.setValue("HighlightAngle", m_fHighlightAngle);
			settings.setValue("HighlightPower", m_fHighlightPower);
		}

		const QString toString() const
		{
			return QString("Bezier{DA=%1;DP=%2;MA=%3;MP=%4;HA=%5;HP=%6;SS=%7;}")
				.arg(m_fDarknessAngle, 0, 'f', 2)
				.arg(m_fDarknessPower, 0, 'f', 2)
				.arg(m_fMidtoneAngle, 0, 'f', 2)
				.arg(m_fMidtone, 0, 'f', 2)
				.arg(m_fHighlightAngle, 0, 'f', 2)
				.arg(m_fHighlightPower, 0, 'f', 2)
				.arg(m_fSaturationShift, 0, 'f', 2);
		}

		void fromString(const QString& szParameters)
		{
			m_fDarknessAngle = ExtractValue(szParameters, "DA");
			m_fDarknessPower = ExtractValue(szParameters, "DP");
			m_fMidtoneAngle = ExtractValue(szParameters, "MA");
			m_fMidtone = ExtractValue(szParameters, "MP");
			m_fHighlightAngle = ExtractValue(szParameters, "HA");
			m_fHighlightPower = ExtractValue(szParameters, "HP");
			m_fSaturationShift = ExtractValue(szParameters, "SS");
		}

	private:

		void addBezierPoints(double x1, double y1, double vx1, double vy1, double x2, double y2, double vx2, double vy2)
		{
			for (double t = 0; t <= 1.0; t += 0.01)
			{
				BezierCurvePoint	pt;

				pt.x = (double)x1 * pow((1 - t), 3) + 3 * vx1 * pow((1 - t), 2) * t + 3 * vx2 * (1 - t) * t * t + x2 * t * t * t;
				pt.y = (double)y1 * pow((1 - t), 3) + 3 * vy1 * pow((1 - t), 2) * t + 3 * vy2 * (1 - t) * t * t + y2 * t * t * t;

				curvePoints.push_back(pt);
			};
		}

		void init()
		{
			BezierCurvePoint	pt1, pt2, pt3, pt4;
			const double size = 1.0;

			curvePoints.clear();

			// Set the first 4 control points of the bezier curve
			pt1.x = 0;
			pt1.y = 0;

			pt2.x = size * cos(m_fDarknessAngle * PI / 180.0) * m_fDarknessPower * m_fMidtone / 100.0 / 100.0;
			pt2.y = sin(m_fDarknessAngle * PI / 180.0) * size * m_fDarknessPower * m_fMidtone / 100.0 / 100.0;

			pt3.x = size * m_fMidtone / 100.0;
			pt3.y = size * m_fMidtone / 100.0;

			pt4 = pt3;
			pt4.x -= sin(m_fMidtoneAngle * PI / 180.0) * size * m_fDarknessPower * m_fMidtone / 100.0 / 100.0;
			pt4.y -= cos(m_fMidtoneAngle * PI / 180.0) * size * m_fDarknessPower * m_fMidtone / 100.0 / 100.0;

			addBezierPoints(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);

			// Set the next 4 control points of the bezier curve
			pt1 = pt3;
			pt2.x = pt1.x + sin(m_fMidtoneAngle * PI / 180.0) * size * m_fHighlightPower / 100.0;
			pt2.y = pt1.y + cos(m_fMidtoneAngle * PI / 180.0) * size * m_fHighlightPower / 100.0;;
			pt3.x = size;
			pt3.y = size;
			pt4 = pt3;
			pt4.x -= cos(m_fHighlightAngle * PI / 180.0) * size * m_fHighlightPower * (100.0 - m_fMidtone) / 100.0 / 100.0;
			pt4.y -= sin(m_fHighlightAngle * PI / 180.0) * size * m_fHighlightPower * (100.0 - m_fMidtone) / 100.0 / 100.0;

			addBezierPoints(pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y, pt4.x, pt4.y);

			std::sort(curvePoints.begin(), curvePoints.end());
		}

		double	ExtractValue(const QString& szString, const QString& szVariable)
		{
			double fValue = 0.0;

			QString strVariable(szVariable + "=");
			qsizetype nPos = szString.indexOf(strVariable, 0);
			if (nPos >= 0)
			{
				qsizetype nStart = nPos + strVariable.length();
				qsizetype nEnd = szString.indexOf(";", nStart);
				if (nEnd < 0)
					nEnd = szString.indexOf("}", nStart);
				if (nEnd > nStart)
				{
					fValue = szString.mid(nStart, nEnd - nStart).toFloat();
				}
			}
			return fValue;
		}

	};
}