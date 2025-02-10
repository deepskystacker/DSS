#include "stdafx.h"
#include "DeepSkyStacker.h"
#include "ProcessingDlg.h"
#include "progressdlg.h"
#include <ztrace.h>

#define dssApp DeepSkyStacker::instance()

namespace DSS
{
	void ProcessingDlg::setButtonIcons()
	{
		setRedButtonIcon();
		setGreenButtonIcon();
		setBlueButtonIcon();
	}

	/* ------------------------------------------------------------------- */

	//
	// If the image is monochrome only show one gradient slider control in 
	// shades of grey.  Otherwise show all three as R, G, B
	//
	void ProcessingDlg::modifyRGBKGradientControls()
	{
		if (dssApp->deepStack().IsLoaded())
		{
			bool monochrome{ dssApp->deepStack().GetStackedBitmap().IsMonochrome() };

			controls->greenHAC->setVisible(monochrome ? false : true);
			controls->greenGradient->setVisible(monochrome ? false : true);
			controls->blueHAC->setVisible(monochrome ? false : true);
			controls->blueGradient->setVisible(monochrome ? false : true);
			controls->linkedSettings->setVisible(monochrome ? false : true);

			if (monochrome)
			{
				// Change the colors of the red gradient to grey
				controls->linkedSettings->setChecked(true);
				controls->redGradient->setColorAt(0.5, QColor(qRgb(128, 128, 128)));
				controls->redGradient->setColorAt(0.999, Qt::white);
				controls->redGradient->setColorAt(1.0, Qt::white);
			}
			else
			{
				// Change the colors of the red gradient
				controls->redGradient->setColorAt(0.5, QColor(qRgb(128, 0, 0)));
				controls->redGradient->setColorAt(0.999, Qt::red);
				controls->redGradient->setColorAt(1.0, Qt::red);
			}
			controls->redGradient->update();
		}
	}

	//
	// Slots
	//
	void ProcessingDlg::setRedButtonIcon()
	{
		//
		// HistogramAdjustmentCurve is 1 based so need to subtract one 
		//
		auto index = static_cast<qsizetype>(redAdjustmentCurve_) - 1;
		const QString curveName = iconNames[index];
		QString iconName = QString{ ":/processing/%1%2.png" }.arg(curveName).arg(iconModifier);
		controls->redHAC->setIcon(QIcon(iconName));
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::setGreenButtonIcon()
	{
		//
		// HistogramAdjustmentCurve is 1 based so need to subtract one 
		//
		auto index = static_cast<qsizetype>(greenAdjustmentCurve_) - 1;
		const QString curveName = iconNames[index];
		QString iconName = QString{ ":/processing/%1%2.png" }.arg(curveName).arg(iconModifier);
		controls->greenHAC->setIcon(QIcon(iconName));
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::setBlueButtonIcon()
	{
		//
		// HistogramAdjustmentCurve is 1 based so need to subtract one 
		//
		auto index = static_cast<qsizetype>(blueAdjustmentCurve_) - 1;
		const QString curveName = iconNames[index];
		QString iconName = QString{ ":/processing/%1%2.png" }.arg(curveName).arg(iconModifier);
		controls->blueHAC->setIcon(QIcon(iconName));
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::redChanging(int peg)
	{
		qDebug() << "ProcessingDlg::RedChanging(" << peg << ")";

		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ controls->redGradient->gradient() };
		QGradientStops stops{ gradient.stops() };

		//
		// Adjust stop values if necessary
		//
		ZASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;
		bool adjust{ false };

		switch (peg)
		{
		case 1:
			// Black point moving
			if (blackPoint > whitePoint - 0.02)
			{
				blackPoint = whitePoint - 0.02;
				adjust = true;
			};
			if (blackPoint > greyPoint - 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			break;
		case 2:
			// Gray point moving
			if (greyPoint < blackPoint + 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			if (greyPoint > whitePoint - 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		case 3:
			// White point moving
			if (whitePoint < blackPoint + 0.02)
			{
				whitePoint = blackPoint + 0.02;
				adjust = true;
			};
			if (whitePoint < greyPoint + 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		};
		if (adjust)
		{
			stops[1].first = blackPoint;
			stops[2].first = greyPoint;
			stops[3].first = whitePoint;
			gradient.setStops(stops);
			controls->redGradient->update();
		};

		//
		// If the settings are linked, the green and blue pegs also need to move
		// 
		if (controls->linkedSettings->isChecked())
		{
			controls->greenGradient->setPeg(peg, stops[peg].first); controls->greenGradient->update();
			controls->blueGradient->setPeg(peg, stops[peg].first); controls->blueGradient->update();
		}

		setDirty();
		emit showHistogram();

	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::redChanged(int peg)
	{
		qDebug() << "ProcessingDlg::RedChanged(" << peg << ")";
		//
		// Before applying the changes, make any corrections necessary by invoking gammaChanging 
		// one final time
		//
		redChanging(peg);

		QLinearGradient& gradient{ controls->redGradient->gradient() };
		QGradientStops stops{ gradient.stops() };
		//
		// Adjust stop values if necessary
		//
		ZASSERT(5 == stops.size());

		//
		// If the settings are linked, the green and blue pegs also need to move
		// 
		if (controls->linkedSettings->isChecked())
		{
			controls->greenGradient->setPeg(peg, stops[peg].first);
			controls->blueGradient->setPeg(peg, stops[peg].first);
		}

		setDirty();
		emit showHistogram();

		qDebug() << "red sliders changed";

	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::greenChanging(int peg)
	{
		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ controls->greenGradient->gradient() };
		QGradientStops stops{ gradient.stops() };

		//
		// Adjust stop values if necessary
		//
		ZASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;
		bool adjust{ false };

		switch (peg)
		{
		case 1:
			// Black point moving
			if (blackPoint > whitePoint - 0.02)
			{
				blackPoint = whitePoint - 0.02;
				adjust = true;
			};
			if (blackPoint > greyPoint - 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			break;
		case 2:
			// Gray point moving
			if (greyPoint < blackPoint + 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			if (greyPoint > whitePoint - 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		case 3:
			// White point moving
			if (whitePoint < blackPoint + 0.02)
			{
				whitePoint = blackPoint + 0.02;
				adjust = true;
			};
			if (whitePoint < greyPoint + 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		};
		if (adjust)
		{
			stops[1].first = blackPoint;
			stops[2].first = greyPoint;
			stops[3].first = whitePoint;
			gradient.setStops(stops);
			controls->greenGradient->update();
		}

		//
		// If the settings are linked, the red and blue pegs also need to move
		// 
		if (controls->linkedSettings->isChecked())
		{
			controls->redGradient->setPeg(peg, stops[peg].first); controls->redGradient->update();
			controls->blueGradient->setPeg(peg, stops[peg].first); controls->blueGradient->update();
		}

		setDirty();
		emit showHistogram();
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::greenChanged(int peg)
	{
		//
		// Before applying the changes, make any corrections necessary by invoking gammaChanging 
		// one final time
		//
		greenChanging(peg);

		QLinearGradient& gradient{ controls->greenGradient->gradient() };
		QGradientStops stops{ gradient.stops() };
		//
		// Adjust stop values if necessary
		//
		ZASSERT(5 == stops.size());

		//
		// If the settings are linked, the red and blue pegs also need to move
		// 
		if (controls->linkedSettings->isChecked())
		{
			controls->redGradient->setPeg(peg, stops[peg].first); controls->redGradient->update();
			controls->blueGradient->setPeg(peg, stops[peg].first); controls->blueGradient->update();
		}

		qDebug() << "green sliders changed";

		setDirty();
		emit showHistogram();
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::blueChanging(int peg)
	{
		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ controls->blueGradient->gradient() };
		QGradientStops stops{ gradient.stops() };

		//
		// Adjust stop values if necessary
		//
		ZASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;
		bool adjust{ false };

		switch (peg)
		{
		case 1:
			// Black point moving
			if (blackPoint > whitePoint - 0.02)
			{
				blackPoint = whitePoint - 0.02;
				adjust = true;
			};
			if (blackPoint > greyPoint - 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			break;
		case 2:
			// Gray point moving
			if (greyPoint < blackPoint + 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			if (greyPoint > whitePoint - 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		case 3:
			// White point moving
			if (whitePoint < blackPoint + 0.02)
			{
				whitePoint = blackPoint + 0.02;
				adjust = true;
			};
			if (whitePoint < greyPoint + 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		};
		if (adjust)
		{
			stops[1].first = blackPoint;
			stops[2].first = greyPoint;
			stops[3].first = whitePoint;
			gradient.setStops(stops);
			controls->blueGradient->update();
		}

		//
		// If the settings are linked, the red and green pegs also need to move
		// 
		if (controls->linkedSettings->isChecked())
		{
			controls->redGradient->setPeg(peg, stops[peg].first);  controls->redGradient->update();
			controls->greenGradient->setPeg(peg, stops[peg].first); controls->greenGradient->update();
		}

		setDirty();
		emit showHistogram();
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::blueChanged(int peg)
	{
		//
		// Before applying the changes, make any corrections necessary by invoking gammaChanging 
		// one final time
		//
		blueChanging(peg);

		QLinearGradient& gradient{ controls->blueGradient->gradient() };
		QGradientStops stops{ gradient.stops() };
		//
		// Adjust stop values if necessary
		//
		ZASSERT(5 == stops.size());

		//
		// If the settings are linked, the red and green pegs also need to move
		// 
		if (controls->linkedSettings->isChecked())
		{
			controls->redGradient->setPeg(peg, stops[peg].first);  controls->redGradient->update();
			controls->greenGradient->setPeg(peg, stops[peg].first); controls->greenGradient->update();
		}

		setDirty();
		emit showHistogram();

		qDebug() << "blue sliders changed";

	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::redButtonPressed()
	{
		HistogramAdjustmentCurve hac{ HistogramAdjustmentCurve::Linear };
		const QAction* a = hacMenu.exec(QCursor::pos());
		if (a == linearAction) hac = HistogramAdjustmentCurve::Linear;
		if (a == cubeRootAction) hac = HistogramAdjustmentCurve::CubeRoot;
		if (a == squareRootAction) hac = HistogramAdjustmentCurve::SquareRoot;
		if (a == logAction) hac = HistogramAdjustmentCurve::Log;
		if (a == logLogAction) hac = HistogramAdjustmentCurve::LogLog;
		if (a == logSquareRootAction) hac = HistogramAdjustmentCurve::LogSquareRoot;
		if (a == asinHAction) hac = HistogramAdjustmentCurve::ASinH;

		setRedAdjustmentCurve(hac);
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::greenButtonPressed()
	{
		HistogramAdjustmentCurve hac{ HistogramAdjustmentCurve::Linear };
		const QAction* a = hacMenu.exec(QCursor::pos());
		if (a == linearAction) hac = HistogramAdjustmentCurve::Linear;
		if (a == cubeRootAction) hac = HistogramAdjustmentCurve::CubeRoot;
		if (a == squareRootAction) hac = HistogramAdjustmentCurve::SquareRoot;
		if (a == logAction) hac = HistogramAdjustmentCurve::Log;
		if (a == logLogAction) hac = HistogramAdjustmentCurve::LogLog;
		if (a == logSquareRootAction) hac = HistogramAdjustmentCurve::LogSquareRoot;
		if (a == asinHAction) hac = HistogramAdjustmentCurve::ASinH;

		setGreenAdjustmentCurve(hac);
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::blueButtonPressed()
	{
		HistogramAdjustmentCurve hac{ HistogramAdjustmentCurve::Linear };
		const QAction* a = hacMenu.exec(QCursor::pos());
		if (a == linearAction) hac = HistogramAdjustmentCurve::Linear;
		if (a == cubeRootAction) hac = HistogramAdjustmentCurve::CubeRoot;
		if (a == squareRootAction) hac = HistogramAdjustmentCurve::SquareRoot;
		if (a == logAction) hac = HistogramAdjustmentCurve::Log;
		if (a == logLogAction) hac = HistogramAdjustmentCurve::LogLog;
		if (a == logSquareRootAction) hac = HistogramAdjustmentCurve::LogSquareRoot;
		if (a == asinHAction) hac = HistogramAdjustmentCurve::ASinH;

		setBlueAdjustmentCurve(hac);
	}
} // namespace DSS

