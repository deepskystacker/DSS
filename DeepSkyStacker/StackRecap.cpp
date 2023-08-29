// StackRecap.cpp : implementation file
//
#include "stdafx.h"
#include "StackRecap.h"
#include "ui/ui_StackRecap.h"
#include "DeepSkyStacker.h"
#include "Ztrace.h"
#include "StackingTasks.h"
#include "Multitask.h"
#include "FrameInfoSupport.h"
#include "BitmapExt.h"
#include "ZExcBase.h"
#include "StackSettings.h"
#include "RecommendedSettings.h"

extern bool	g_bShowRefStars;

constexpr int SSTAB_RESULT = 0;
constexpr int SSTAB_COMET = 1;
constexpr int SSTAB_LIGHT = 2;
constexpr int SSTAB_DARK = 3;
constexpr int SSTAB_FLAT = 4;
constexpr int SSTAB_OFFSET = 5;
constexpr int SSTAB_ALIGNMENT = 6;
constexpr int SSTAB_INTERMEDIATE = 7;
constexpr int SSTAB_POSTCALIBRATION = 9;
constexpr int SSTAB_OUTPUT = 10;


StackRecap::StackRecap(QWidget *parent) :
	QDialog(parent),
	ui{ new Ui::StackRecap() },
	workspace { std::make_unique<Workspace>() },
	pStackingTasks { nullptr },
	initialised{ false },
	windowTextColour{ palette().color(QPalette::ColorRole::WindowText) },
	blueColour { QColorConstants::Svg::deepskyblue }
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	//
	// Don't want the TextBrowser to try to follow links, we handle that in an AnchorClicked slot
	//
	ui->textBrowser->setOpenLinks(false);

	//
	// If Windows Dark Theme is active set blueColour to be lightskyblue instead of deepskyblue
	// 
	if (Qt::ColorScheme::Dark == QGuiApplication::styleHints()->colorScheme())
		blueColour = QColorConstants::Svg::lightskyblue;
}

StackRecap::~StackRecap()
{
	delete ui;
}

void StackRecap::showEvent(QShowEvent *event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}

void StackRecap::onInitDialog()
{
	QSettings settings;

	//
	// Restore Window position etc..
	//
	QByteArray ba = settings.value("Dialogs/StackingSteps/geometry").toByteArray();
	if (!ba.isEmpty())
	{
		restoreGeometry(ba);
	}
	else
	{
		//
		// Get main Window rectangle
		//
		const QRect r{ DeepSkyStacker::instance()->rect() };
		QSize size = this->size();

		int top = (r.top() + (r.height() / 2) - (size.height() / 2));
		int left = (r.left() + (r.width() / 2) - (size.width() / 2));
		move(left, top);
	}

	fillWithAllTasks();
};

/* ------------------------------------------------------------------- */

void	StackRecap::insertHeader(QString & strHTML)
{
	QPalette palette;
	QColor	colour = palette.color(QPalette::Window);

	QString strText = QString("<body link=#0000ff bgcolor=%1></body>")
		.arg(colour.name());

	strHTML += strText;
};

/* ------------------------------------------------------------------- */

void	StackRecap::insertHTML(QString & strHTML, const QString& szText, QColor colour, bool bBold, bool bItalic, int lLinkID)
{
	QString					strText;
	QString					strInputText = szText;

	if (bBold && bItalic)
	{
		strText = QString("<b><i>%1</i></b>")
			.arg(strInputText);
		strInputText = strText;
	}
	else if (bBold)
	{
		strText = QString("<b>%1</b>")
			.arg(strInputText);
		strInputText = strText;
	}
	else if (bItalic)
	{
		strText = QString("<i>%1</i>")
			.arg(strInputText);
		strInputText = strText;
	};

	if (-1 != lLinkID)
	{
		strText = QString("<a href = \"%1\">%2</a>")
			.arg(lLinkID)
			.arg(strInputText);
		strInputText = strText;
	};

	strText = QString("<font color = %1>%2</font>")
		.arg(colour.name())
		.arg(strInputText);

	strInputText = strText;

	strHTML += strInputText;
};

/* ------------------------------------------------------------------- */
static void GetISOGainStrings(CTaskInfo *pTask, QString const &strISO, QString const &strGain,
	QString &strISOGainText, QString &strISOGainValue)
{
	if (pTask->HasISOSpeed())
	{
		strISOGainValue.setNum(pTask->m_lISOSpeed);
		strISOGainText = strISO;
	}
	else
	{
		strISOGainValue.setNum(pTask->m_lGain);
		strISOGainText = strGain;
	}
}

void StackRecap::fillWithAllTasks()
{
	ZFUNCTRACE_RUNTIME();
	if (pStackingTasks)
	{
		QString				strHTML;

		insertHeader(strHTML);

		QString				strText;
		QString				strExposure;
		QString				strISOGainValue;
		QString				strISOGainText;
		QString				strISOText(tr("ISO", "IDS_ISO"));
		QString				strGainText(tr("Gain", "IDS_GAIN"));
		int				i, j;
		int				lTotalExposure = 0;
		__int64				ulNeededSpace;
		__int64				ulFreeSpace;
		QString				strFreeSpace;
		QString				strNeededSpace;
		STACKINGMODE		ResultMode{ pStackingTasks->getStackingMode() };
		bool				bSaveIntermediates;

		ulNeededSpace = pStackingTasks->computeNecessaryDiskSpace();
		fs::path drive;
		ulFreeSpace = pStackingTasks->AvailableDiskSpace(drive);
		bSaveIntermediates = pStackingTasks->GetCreateIntermediates();

		SpaceToQString(ulFreeSpace, strFreeSpace);
		SpaceToQString(ulNeededSpace, strNeededSpace);

		const QString		strYes(tr("Yes", "IDS_YES"));
		const QString		strNo(tr("No", "IDS_NO"));
		QString				strYesNo;
		QString				strBackgroundCalibration;
		QString				strPerChannelBackgroundCalibration;
		QString				strDarkOptimization;
		QString				strDarkFactor;
		QString				strHotPixels;
		BACKGROUNDCALIBRATIONMODE	CalibrationMode;

		CalibrationMode = pStackingTasks->GetBackgroundCalibrationMode();


		strYesNo = (CalibrationMode == BCM_RGB) ? strYes : strNo;
		strBackgroundCalibration = 
			tr("RGB Channels Background Calibration : %1", "IDS_RECAP_BACKGROUNDCALIBRATION")
				.arg(strYesNo);

		strYesNo = (CalibrationMode == BCM_PERCHANNEL) ? strYes : strNo;
		strPerChannelBackgroundCalibration =
			tr("Per Channel Background Calibration: %1", "IDS_RECAP_PERCHANNELBACKGROUNDCALIBRATION")
				.arg(strYesNo);

		strYesNo = pStackingTasks->GetDarkOptimization() ? strYes : strNo;
		strDarkOptimization = 
			tr("Dark optimization: %1", "IDS_RECAP_DARKOPTIMIZATION")
				.arg(strYesNo);

		double				fDarkFactor = pStackingTasks->GetDarkFactor();
		if (fDarkFactor != 1.0)
			strDarkFactor = tr("Dark Multiplication Factor: %1", "IDS_RECAP_DARKMULTIPLICATIONFACTOR")
				.arg(fDarkFactor, 0, 'f', 2);

		strYesNo = pStackingTasks->GetHotPixelsDetection() ? strYes : strNo;
		strHotPixels = tr("Hot Pixel detection and removal: %1", "IDS_RECAP_HOTPIXELS")
			.arg(strYesNo);

		if (ulFreeSpace < ulNeededSpace)
		{
			// Warning about the available space on drive
			strHTML +=
				"<table border='1px' cellpadding='5' bordercolor=#ffc0c0 bordercolorlight=#ffc0c0"
					"bordercolordark=#ffffc0 cellspacing=0 width='100%'><tr><td>";
			strText = tr("The process temporarily requires %1 of free space on the %2 drive.<br>"
					"Only %3 are available on this drive.", "IDS_RECAP_WARNINGDISKSPACE")
				.arg(strNeededSpace)
				.arg(drive.wstring().c_str())
				.arg(strFreeSpace);
			insertHTML(strHTML, strText, QColorConstants::Red, true, false);
			if (ResultMode == SM_MOSAIC)
			{
				strHTML += "<br>";
				insertHTML(strHTML, 
					tr("Note: the necessary disk space is computed using an image the size of the reference frame. "
						"Depending of the resulting image total size more space may be necessary.", "IDS_RECAP_MOSAICWARNING"),
					windowTextColour);
			};
			strHTML += "</td></tr></table>";
		};

		if (pStackingTasks->AreCalibratingJPEGFiles())
		{
			strHTML +=
				"<table border='1px' cellpadding='5' bordercolor=#ffc0c0 bordercolorlight=#ffc0c0"
					"bordercolordark=#ffffc0 cellspacing=0 width='100%'><tr><td>";
			strText = tr("Warning: you are using dark, flat or bias frames with JPEG files.<br>"
				"Because of the lossy compression, calibration doesn't work with JPEG files.", "IDS_RECAP_WARNINGJPEG");
			insertHTML(strHTML, strText, QColorConstants::Red, true, false);
			strHTML += "</td></tr></table>";
		};

		strHTML += "<table border=0 valign=middle cellspacing=0 width='100%'><tr>";
		strHTML += "<td width='48%'>";
		strText = tr("Stacking mode: ", "IDS_RECAP_STACKINGMODE");
		insertHTML(strHTML, strText, windowTextColour, true);
		switch (ResultMode)
		{
		case SM_NORMAL :
			strText = tr("Standard", "IDS_RECAP_STACKINGMODE_NORMAL");
			break;
		case SM_MOSAIC :
			strText = tr("Mosaic", "IDS_RECAP_STACKINGMODE_MOSAIC");
			break;
		case SM_CUSTOM :
			strText = tr("Custom Rectangle", "IDS_RECAP_STACKINGMODE_CUSTOM");
			break;
		case SM_INTERSECTION :
			strText = tr("Intersection", "IDS_RECAP_STACKINGMODE_INTERSECTION");
			break;
		};

		insertHTML(strHTML, strText, blueColour, false, false, SSTAB_RESULT);

		const auto dwAlignment = pStackingTasks->GetAlignmentMethod();

		strText = tr("Alignment method: ", "IDS_RECAP_ALIGNMENT");
		strHTML += "</td><td width='48%'>";
		insertHTML(strHTML, strText, windowTextColour, true);

		switch (dwAlignment)
		{
		case 0 :
		case 1 :
			strText = tr("Automatic", "IDS_ALIGN_AUTO");
			break;
		case 2 :
			strText = tr("Bilinear", "IDS_ALIGN_BILINEAR");
			break;
		case 3 :
			strText = tr("Bisquared", "IDS_ALIGN_BISQUARED");
			break;
		case 4 :
			strText = tr("Bicubic", "IDS_ALIGN_BICUBIC");
			break;
		case 5 :
			strText = tr("No Alignment", "IDS_ALIGN_NONE");
			break;
		};

		insertHTML(strHTML, strText, blueColour, false, false, SSTAB_ALIGNMENT);
		strHTML += "</td>";
		strHTML += "</tr></table><br>";

		const auto dwDrizzle = pStackingTasks->GetPixelSizeMultiplier();
		if (dwDrizzle > 1)
		{
			strText = tr("Drizzle x%1 enabled", "IDS_RECAP_DRIZZLE")
				.arg(dwDrizzle);
			insertHTML(strHTML, strText, blueColour, false, false, SSTAB_RESULT);
			strHTML += "<br>";
			if (IsRawBayer() || IsFITSRawBayer())
			{
				strText = tr("The selected drizzle option is not compatible with Bayer Drizzle mode.",
					"IDS_RECAP_WARNINGDRIZZLE");
				insertHTML(strHTML, strText, QColorConstants::Red, true);
				strHTML += "<br>";
			};
		};

		const auto lNrProcessors = CMultitask::GetNrProcessors(true);
		if (lNrProcessors > 1)
		{
			if (CMultitask::GetNrProcessors() > 1)
				strText = tr("%1 processors detected and used", "IDS_RECAP_DETECTEDANDUSEDPROCESSORS")
				.arg(lNrProcessors);
			else
				strText = tr("%1 processors detected - only one used", "IDS_RECAP_DETECTEDNOTUSEDPROCESSORS")
				.arg(lNrProcessors);
			insertHTML(strHTML, strText, blueColour, false, true);
			strHTML +=  "<br>";
		};

		// Comet Info
		if (pStackingTasks->IsCometAvailable())
		{
			COMETSTACKINGMODE	CometStackingMode;

			CometStackingMode = pStackingTasks->GetCometStackingMode();
			strText = tr("Comet processing : ", "IDS_RECAP_COMETSTACKING");
			insertHTML(strHTML, strText, windowTextColour, true, false);
			switch (CometStackingMode)
			{
			case CSM_STANDARD :
				strText = tr("Align on stars (no specific processing)", "IDS_RECAP_COMETSTACKING_NONE");
				break;
			case CSM_COMETONLY :
				strText = tr("Align on comet", "IDS_RECAP_COMETSTACKING_COMET");
				break;
			case CSM_COMETSTAR :
				strText = tr("Align on stars and comet", "IDS_RECAP_COMETSTACKING_BOTH");
				break;
			};
			insertHTML(strHTML, strText, blueColour, false, false, SSTAB_COMET);
			strHTML += "<br>";
		};
		strHTML += "<br>";

		CPostCalibrationSettings		pcs;

		pStackingTasks->GetPostCalibrationSettings(pcs);

		if (pcs.m_bHot)
		{
			strText = tr("Cosmetic applied to hot pixels (Filter = %1 px, Detection Threshold = %L2%)<br>",
					"IDS_RECAP_COSMETICHOT")
				.arg(pcs.m_lHotFilter)
				.arg(pcs.m_fHotDetection, 0, 'f', 1);
			insertHTML(strHTML, strText, blueColour, false, false, SSTAB_POSTCALIBRATION);
		};
		if (pcs.m_bCold)
		{
			strText = tr("Cosmetic applied to cold pixels (Filter = %1 px, Detection Threshold = %L2%)<br>",
					"IDS_RECAP_COSMETICCOLD")
				.arg(pcs.m_lColdFilter)
				.arg(pcs.m_fColdDetection, 0, 'f', 1);
			insertHTML(strHTML, strText, blueColour, false, false, SSTAB_POSTCALIBRATION);
		};

		if (pcs.m_bHot || pcs.m_bCold)
			strHTML += "<br>";

		for (i = 0;i<pStackingTasks->m_vStacks.size();i++)
		{
			CStackingInfo &			si = pStackingTasks->m_vStacks[i];

			if (si.m_pLightTask)
			{
				strHTML +=
					"<table border='1px' bgcolorleft=#fff9fa bgcolorright=#f9fbff bordercolordark=#fafafa"
					"bordercolor=#c0c0c0 bordercolorlight=#c0c0c0  cellspacing=0 cellpadding=5 width='100%'><tr><td>";
				int			lTaskExposure = 0;

				for (j = 0;j<si.m_pLightTask->m_vBitmaps.size();j++)
					lTaskExposure += si.m_pLightTask->m_vBitmaps[j].m_fExposure;

				lTotalExposure += lTaskExposure;

				strExposure = exposureToString(lTaskExposure);
				GetISOGainStrings(si.m_pLightTask, strISOText, strGainText, strISOGainText, strISOGainValue);

				strText = tr("Stacking step %1<br>  ->%2 frames (%3: %4) - total exposure: ",
						"IDS_RECAP_STEP")
					.arg(i+1)
					.arg(si.m_pLightTask->m_vBitmaps.size())
					.arg(strISOGainText)
					.arg(strISOGainValue);
				insertHTML(strHTML, strText, windowTextColour, true);
				insertHTML(strHTML, strExposure, QColorConstants::DarkGreen, true);
				strHTML += "<ul style=\"list-style: none; \">";
				strHTML += "<li>";
				insertHTML(strHTML, strBackgroundCalibration, blueColour, false, false, SSTAB_LIGHT);
				strHTML += "</li>";
				strHTML += "<li>";
				insertHTML(strHTML, strPerChannelBackgroundCalibration, blueColour, false, false, SSTAB_LIGHT);
				strHTML += "</li>";
				if (si.m_pLightTask->m_vBitmaps.size()>1)
				{
					strHTML += "<li>";
					strText = tr("Method: ", "IDS_RECAP_METHOD");
					insertHTML(strHTML, strText, windowTextColour);
					FormatMethod(strText, si.m_pLightTask->m_Method, si.m_pLightTask->m_fKappa, si.m_pLightTask->m_lNrIterations);
					insertHTML(strHTML, strText, blueColour, false, false, SSTAB_LIGHT);
					strHTML += "</li>";

					if ((si.m_pLightTask->m_Method != MBP_AVERAGE) &&
						(IsRawBayer() || IsFITSRawBayer()))
					{
						strHTML += "<li>";
						strHTML += "&nbsp;&nbsp;&nbsp;&nbsp;";
						strText = tr(
							"Warning: the Bayer Drizzle option selected in the RAW DDP settings may lead to strange results with a method other than average.",
							"IDS_RECAP_WARNINGBAYERDRIZZLE");
						insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
						strHTML += "</li>";
					};
				};
				strHTML += "</ul>";

				strHTML += "<hr style=\"background-color:" + windowTextColour.name() + "\">";
				strHTML += "<ul style=\"list-style: none; \">";

				//if (si.m_pDarkTask || si.m_pOffsetTask || si.m_pFlatTask || si.m_pDarkFlatTask)
				//	strHTML += "<br>";

				if (si.m_pOffsetTask)
				{
					strHTML += "<li>";
					strExposure = exposureToString(si.m_pOffsetTask->m_fExposure);
					GetISOGainStrings(si.m_pOffsetTask, strISOText, strGainText, strISOGainText, strISOGainValue);
					strText = tr("-> Offset: %1 frames (%2: %3) exposure: %4", "IDS_RECAP_OFFSET")
						.arg(si.m_pOffsetTask->m_vBitmaps.size())
						.arg(strISOGainText)
						.arg(strISOGainValue)
						.arg(strExposure);
					insertHTML(strHTML, strText, windowTextColour);
					strHTML += "</li>";

					strHTML += "<ul style=\"list-style: none; \">";

					if (si.m_pOffsetTask->m_vBitmaps.size()>1)
					{
						strHTML += "<li>";
						strText = tr("Method: ", "IDS_RECAP_METHOD");
						insertHTML(strHTML, strText, windowTextColour);
						FormatMethod(strText, si.m_pOffsetTask->m_Method, si.m_pOffsetTask->m_fKappa, si.m_pOffsetTask->m_lNrIterations);
						insertHTML(strHTML, strText, blueColour, false, false, SSTAB_OFFSET);
						strHTML += "</li>";
					};

					if (si.m_pOffsetTask->HasISOSpeed())
					{
						if (si.m_pOffsetTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						{
							strHTML += "<li>";
							strText = tr("Warning: ISO speed does not match that of the light frames", "IDS_RECAP_ISOWARNING");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					}
					else
					{
						if (si.m_pOffsetTask->m_lGain != si.m_pLightTask->m_lGain)
						{
							strHTML += "<li>";
							strText = tr("Warning: Gain does not match that of the light frames", "IDS_RECAP_GAINWARNING");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";

						};
					};
					strHTML += "</ul>";
				}
				else
				{
					strHTML += "<li>";
					strText = tr("-> No Offset", "IDS_RECAP_NOOFFSET");
					insertHTML(strHTML, strText, QColorConstants::DarkRed);
					strHTML += "</li>";
				};
				if (si.m_pDarkTask)
				{
					strExposure = exposureToString(si.m_pDarkTask->m_fExposure);
					GetISOGainStrings(si.m_pDarkTask, strISOText, strGainText, strISOGainText, strISOGainValue);
					strHTML += "<li>";
					strText = tr("-> Dark: %1 frames (%2 : %3) exposure: %4", "IDS_RECAP_DARK")
						.arg(si.m_pDarkTask->m_vBitmaps.size())
						.arg(strISOGainText)
						.arg(strISOGainValue)
						.arg(strExposure);
					insertHTML(strHTML, strText, windowTextColour);
					strHTML += "</li>";

					strHTML += "<ul style=\"list-style: none; \">";

					if (si.m_pDarkTask->m_vBitmaps.size()>1)
					{
						strHTML += "<li>";
						strText = tr("Method: ", "IDS_RECAP_METHOD");
						insertHTML(strHTML, strText, windowTextColour);
						FormatMethod(strText, si.m_pDarkTask->m_Method, si.m_pDarkTask->m_fKappa, si.m_pDarkTask->m_lNrIterations);
						insertHTML(strHTML, strText, blueColour, false, false, SSTAB_DARK);
						strHTML += "</li>";
					};

					strHTML += "<li>";
					insertHTML(strHTML, strDarkOptimization, blueColour, false, false, SSTAB_DARK);
					strHTML += "</li>";
					strHTML += "<li>";
					insertHTML(strHTML, strHotPixels, blueColour, false, false, SSTAB_DARK);
					strHTML += "</li>";


					if (!strDarkFactor.isEmpty())
					{
						strHTML += "<li>";
						insertHTML(strHTML, strDarkFactor, blueColour, false, false, SSTAB_DARK);
						strHTML += "</li>";
					};

					if (si.m_pDarkTask->HasISOSpeed())
					{
						if (si.m_pDarkTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						{
							strHTML += "<li>";
							strText = tr("Warning: ISO speed does not match that of the light frames", "IDS_RECAP_ISOWARNING");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					}
					else
					{
						if (si.m_pDarkTask->m_lGain != si.m_pLightTask->m_lGain)
						{
							strHTML += "<li>";
							strText = tr("Warning: Gain does not match that of the light frames", "IDS_RECAP_GAINWARNING");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					};
					if (!AreExposureEquals(si.m_pDarkTask->m_fExposure, si.m_pLightTask->m_fExposure))
					{
						strHTML += "<li>";
						strText = tr("Warning: Exposure does not match that of the Light frames", "IDS_RECAP_EXPOSUREWARNING");
						insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
						strHTML += "</li>";
					};
					strHTML += "</ul>";
				}
				else
				{
					strHTML += "<li>";
					strText = tr("-> No Dark", "IDS_RECAP_NODARK");
					insertHTML(strHTML, strText, QColorConstants::DarkRed);
					strHTML += "</li>";
				};
				if (si.m_pDarkFlatTask && si.m_pFlatTask)
				{
					strExposure = exposureToString(si.m_pDarkFlatTask->m_fExposure);
					GetISOGainStrings(si.m_pDarkFlatTask, strISOText, strGainText, strISOGainText, strISOGainValue);
					strHTML += "<li>";
					strText = tr("-> Dark Flat: %1 frames (%2 : %3) exposure: %4", "IDS_RECAP_DARKFLAT")
						.arg(si.m_pDarkFlatTask->m_vBitmaps.size())
						.arg(strISOGainText)
						.arg(strISOGainValue)
						.arg(strExposure);
					insertHTML(strHTML, strText, windowTextColour);
					strHTML += "</li>";

					strHTML += "<ul style=\"list-style: none; \">";

					if (si.m_pDarkFlatTask->m_vBitmaps.size()>1)
					{
						strHTML += "<li>";
						strText = tr("Method: ", "IDS_RECAP_METHOD");
						insertHTML(strHTML, strText, windowTextColour);
						strHTML += "</li>";
						FormatMethod(strText, si.m_pDarkFlatTask->m_Method, si.m_pDarkFlatTask->m_fKappa, si.m_pDarkFlatTask->m_lNrIterations);
						insertHTML(strHTML, strText, blueColour, false, false, SSTAB_DARK);
						strHTML += "</li>";
					};

					if (si.m_pDarkFlatTask->HasISOSpeed())
					{
						if (si.m_pDarkFlatTask->m_lISOSpeed != si.m_pFlatTask->m_lISOSpeed)
						{
							strHTML += "<li>";
							strText = tr("Warning: ISO speed does not match that of the flat frames", "IDS_RECAP_ISOWARNINGDARKFLAT");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					}
					else
					{
						if (si.m_pDarkFlatTask->m_lGain != si.m_pFlatTask->m_lGain)
						{
							strHTML += "<li>";
							strText = tr("Warning: Gain does not match that of the flat frames", "IDS_RECAP_GAINWARNINGDARKFLAT");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					};
					if (!AreExposureEquals(si.m_pDarkFlatTask->m_fExposure, si.m_pFlatTask->m_fExposure))
					{
						strHTML += "<li>";
						strText = tr("Warning: Exposure does not match that of the flat frames", "IDS_RECAP_EXPOSUREWARNINGDARKFLAT");
						insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
						strHTML += "</li>";
					};
					strHTML += "</ul>";
				};
				if (si.m_pFlatTask)
				{
					strExposure  = exposureToString(si.m_pFlatTask->m_fExposure);
					GetISOGainStrings(si.m_pFlatTask, strISOText, strGainText, strISOGainText, strISOGainValue);

					strHTML += "<li>";
					strText = tr("->Flat: %1 frames(%2: %3) exposure : %4", "IDS_RECAP_FLAT")
						.arg(si.m_pFlatTask->m_vBitmaps.size())
						.arg(strISOGainText)
						.arg(strISOGainValue)
						.arg(strExposure);
					insertHTML(strHTML, strText, windowTextColour);
					strHTML += "</li>";

					strHTML += "<ul style=\"list-style: none; \">";
					if (si.m_pFlatTask->m_vBitmaps.size()>1)
					{
						strHTML += "<li>";
						strText = tr("Method: ", "IDS_RECAP_METHOD");
						insertHTML(strHTML, strText, windowTextColour);
						FormatMethod(strText, si.m_pFlatTask->m_Method, si.m_pFlatTask->m_fKappa, si.m_pFlatTask->m_lNrIterations);
						insertHTML(strHTML, strText, blueColour, false, false, SSTAB_FLAT);
						strHTML +=  "</li>";
					};

					if (si.m_pFlatTask->HasISOSpeed())
					{
						if (si.m_pFlatTask->m_lISOSpeed != si.m_pLightTask->m_lISOSpeed)
						{
							strHTML += "<li>";
							strText = tr("Warning: ISO speed does not match that of the light frames", "IDS_RECAP_ISOWARNING");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					}
					else
					{
						if (si.m_pFlatTask->m_lGain != si.m_pLightTask->m_lGain)
						{
							strHTML += "<li>";
							strText = tr("Warning: Gain does not match that of the light frames", "IDS_RECAP_GAINWARNING");
							insertHTML(strHTML, strText, QColorConstants::DarkRed, false, true);
							strHTML += "</li>";
						};
					};
					strHTML += "</ul>";
				}
				else
				{
					strHTML += "<li>";
					strText = tr("-> No Flat", "IDS_RECAP_NOFLAT");
					insertHTML(strHTML, strText, QColorConstants::DarkRed);
					strHTML += "</li>";

				};

				strHTML += "</ul>";
				strHTML += "</td></tr></table><br>";
			};
		};

		strExposure = exposureToString(lTotalExposure);
		strText = QString("<font color=#008000>%1</font>").arg(strExposure);
		strExposure = strText;
		strHTML += "<br>";
		strText = tr(
			"Estimated Total exposure time: %1<br>(the total exposure time is computed assuming that all the checked light frames are kept for the stacking process)",
			"IDS_RECAP_TOTALEXPOSURETIME")
			.arg(strExposure);
		insertHTML(strHTML, strText, windowTextColour, true, true);
		strHTML += "<br>";

		if (ulFreeSpace > ulNeededSpace)
		{
			strHTML += "<br>";
			strText = tr("The process will temporarily use %1 on the %2 drive (%3 free).",
				"IDS_RECAP_INFODISKSPACE")
				.arg(strNeededSpace)
				.arg(drive.wstring().c_str())
				.arg(strFreeSpace);
			insertHTML(strHTML, strText, windowTextColour);
			if (ResultMode == SM_MOSAIC)
			{
				strHTML += "<br>";
				strText = tr("Note: the necessary disk space is computed using an image the size of the reference frame. "
					"Depending of the resulting image total size more space may be necessary.",
					"IDS_RECAP_MOSAICWARNING");
				insertHTML(strHTML, strText, windowTextColour);
			};
			if (bSaveIntermediates)
			{
				strHTML += "<br><br>";
				strText = tr("Warning: the save registered and calibrated images option is checked. "
					"Please check that you have enough free disk space to store these files.",
					"IDS_RECAP_WARNINGINTERMEDIATESAVE");
				insertHTML(strHTML, strText, QColorConstants::DarkRed);
			};
		};
		ui->textBrowser->setHtml(strHTML);
	};
};

/* ------------------------------------------------------------------- */
void StackRecap::CallStackingSettings(int tab)
{
	ZFUNCTRACE_RUNTIME();

	ZASSERT(nullptr != pStackingTasks);

	STACKINGMODE stackingMode{ pStackingTasks->getStackingMode() };


	StackSettings			dlg(this);
	DSSRect					rcCustom;

	if (pStackingTasks->IsCometAvailable())
		dlg.enableCometStacking(true);

	dlg.setTabVisibility(pStackingTasks->AreDarkUsed(),
		pStackingTasks->AreFlatUsed(),
		pStackingTasks->AreBiasUsed());

	dlg.setStackingTasks(pStackingTasks);

	dlg.setStartingTab(tab);

	if (dlg.exec())
	{
		stackingMode = pStackingTasks->getStackingMode();
		pStackingTasks->UpdateTasksMethods();
		fillWithAllTasks();
	};

}

void StackRecap::on_stackSettings_clicked()
{
	CallStackingSettings();
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

void StackRecap::on_recommended_clicked()
{
	RecommendedSettings		dlg;

	dlg.setStackingTasks(pStackingTasks);
	if (dlg.exec() && pStackingTasks)
	{
		pStackingTasks->UpdateTasksMethods();
		fillWithAllTasks();
	};
}

void StackRecap::on_textBrowser_anchorClicked(const QUrl &url)
{
	CallStackingSettings(url.toString().toInt());
}

void StackRecap::accept()
{
	QSettings settings;

	settings.setValue("Dialogs/StackingSteps/geometry", saveGeometry());

	Inherited::accept();
}

void StackRecap::reject()
{
	QSettings settings;

	settings.setValue("Dialogs/StackingSteps/geometry", saveGeometry());

	Inherited::reject();
}

/* ------------------------------------------------------------------- */