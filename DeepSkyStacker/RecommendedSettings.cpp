// RecommendedSettings.cpp : implementation file
//
#include "stdafx.h"
#include "RecommendedSettings.h"
#include "ui/ui_RecommendedSettings.h"
#include "DeepSkyStacker.h"
#include "StackingDlg.h"


bool RecommendationItem::differsFromWorkspace()
{
	bool					bResult = false;
	Workspace				workspace;

	// Check that the current values are (or not)
	for (const auto setting : vSettings)
	{
		QString				keyName;
		QVariant			value;
		QVariant			currentValue;


		keyName = setting.key();

		currentValue = workspace.value(keyName);
		value = setting.value();

		switch (static_cast<QMetaType::Type>(value.typeId()))
		{
		case QMetaType::Bool:
			bResult = value.toBool() != currentValue.toBool();
			break;
		case QMetaType::Double:
			bResult = value.toDouble() != currentValue.toDouble();
			break;
		default:
			bResult = value.toString() != currentValue.toString();
		}

		//
		// If different, no need to check any more
		//
		if (bResult) break;
	};
	return bResult;
}

void RecommendationItem::applySettings()
{
	Workspace				workspace;

	for (size_t i = 0; i < vSettings.size(); i++)
	{
		QString				keyName;
		QVariant			value;

		keyName = vSettings[i].key();
		value = vSettings[i].value();

		workspace.setValue(keyName, value);
	};
}


// RecommendedSettings dialog

RecommendedSettings::RecommendedSettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RecommendedSettings),
	workspace(new Workspace()),
	pStackingTasks(nullptr),
	initialised(false),
	darkTheme { Qt::ColorScheme::Dark == QGuiApplication::styleHints()->colorScheme() },
	//
	// If Windows Dark Theme is active set blueColour to be lightskyblue instead of deepskyblue
	// 
	blueColour{ darkTheme ? QColorConstants::Svg::lightskyblue : QColorConstants::Svg::deepskyblue }
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	//
	// Don't want the TextBrowser to try to follow links, we handle that in an AnchorClicked slot
	//
	ui->textBrowser->setOpenLinks(false);
}

RecommendedSettings::~RecommendedSettings()
{
	delete ui;
}

void RecommendedSettings::showEvent(QShowEvent *event)
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

void RecommendedSettings::onInitDialog()
{
	QSettings settings;

	//
	// Restore Window position etc..
	//
	QByteArray ba = settings.value("Dialogs/Recommended/geometry").toByteArray();
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

	workspace->Push();

	ui->checkBox->setVisible(false);

	if (!pStackingTasks)
	{
		DeepSkyStacker::instance()->getStackingDlg().fillTasks(stackingTasks);
		pStackingTasks = &stackingTasks;
	};

	fillWithRecommendedSettings();
};

void RecommendedSettings::on_textBrowser_anchorClicked(const QUrl &url)
{
	setSetting(url.toString().toInt());
}

void RecommendedSettings::accept()
{
	QSettings settings;

	settings.setValue("Dialogs/Recommended/geometry", saveGeometry());

	workspace->Pop(false);
	workspace->saveSettings();

	Inherited::accept();
}

void RecommendedSettings::reject()
{
	QSettings settings;

	settings.setValue("Dialogs/Recommended/geometry", saveGeometry());

	workspace->Pop();		// Restore status-quo ante

	Inherited::reject();
}

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

void RecommendedSettings::insertHeader()
{
	QPalette palette;
	QColor	colour{ darkTheme ? Qt::gray : Qt::lightGray };

	QString	strHTML{ QString("<body bgcolor=%1>")
							.arg(colour.name()) };

	strHTML += "<table border=1 align=center cellpadding=4 cellspacing=4 bgcolortop=#ececec bgcolorbottom=\"white\" width=\"100%%\"><tr>";
	strHTML += "<td>";

	strHTML += tr("These are recommended settings.<br>"
		"They may not work in all the situations but they are often a good starting point.", "IDS_RECO_DISCLAIMER");
	strHTML += "<br><br>";

	strHTML += tr("Click on the proposed link to change the setting accordingly", "IDS_RECO_CLICKTOSET");
	strHTML += "<br>";

	strHTML += "<font color=" + QColor(qRgb(86, 170, 86)).name() + ">";
	strHTML += tr("Settings that are already set are shown in green", "IDS_RECO_ALREADYSET");
	strHTML += "</font>";

	strHTML += "</td></tr></table><br>";

	ui->textBrowser->insertHtml(strHTML);
};

/* ------------------------------------------------------------------- */

void RecommendedSettings::insertHTML(const QString& html, const QColor& colour, bool bBold, bool bItalic, int lLinkID)

{
	QString					strText;
	QString					strInputText(html);
	QColor	linkColour{ darkTheme ? Qt::cyan : Qt::darkBlue };

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
	}
	
	if (-1 != lLinkID)
	{
		strText = QString("<a href = \"%1\"><span style=\"color: %2;\">%3</span></a>")
			.arg(lLinkID)
			.arg(linkColour.name())
			.arg(strInputText);
		strInputText = strText;
	};

	strText = QString("<font color = %1>%2</font>")
		.arg(colour.name())
		.arg(strInputText);

	strInputText = strText;

	ui->textBrowser->insertHtml(strInputText);
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

static void AddRAWNarrowBandRecommendation(RECOMMENDATIONVECTOR & vRecommendations)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"If you are processing narrowband images (especially H%1)",
		"IDS_RECO_RAWNARROWBAND_REASON")
		.arg("\xce\xb1"));			// Greek letter lower case alpha (α)

	ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
		"Use super-pixel mode",
		"IDS_RECO_RAWNARROWBAND_TEXT"));


	ri.addSetting("RawDDP/SuperPixels", true);
	ri.addSetting("RawDDP/RawBayer", false);
	ri.addSetting("RawDDP/AHD", false);

	rec.isImportant = false;
	rec.addItem(ri);

	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddNarrowBandPerChannelBackgroundCalibration(RECOMMENDATIONVECTOR & vRecommendations)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"If you are processing narrowband images (especially H%1)",
		"IDS_RECO_RAWNARROWBAND_REASON")
		.arg("\xce\xb1"));			// Greek letter lower case alpha (α)

	ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
		"Use Per Channel background calibration",
		"IDS_RECO_USEPERCHANNEL"));

	rec.isImportant = false;

	ri.addSetting("Stacking/BackgroundCalibration", false);
	ri.addSetting("Stacking/PerChannelBackgroundCalibration", true);

	rec.addItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddRAWDebayering(RECOMMENDATIONVECTOR & vRecommendations, double fExposureTime)
{
	RecommendationItem			ri;
	Recommendation				rec;

	if (fExposureTime > 4*60.0)
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are processing long exposure and possibly good SNR images",
			"IDS_RECO_RAWHIGHSNR_REASON"));
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use AHD debayering",
			"IDS_RECO_RAWHIGHSNR_TEXT"));
	}
	else
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are processing short exposure and probably low SNR images",
			"IDS_RECO_RAWLOWSNR_REASON"));
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Bilinear debayering",
			"IDS_RECO_RAWLOWSNR_TEXT"));
	};

	if (fExposureTime > 4*60.0)
	{
		ri.addSetting("RawDDP/SuperPixels", false);
		ri.addSetting("RawDDP/RawBayer", false);
		ri.addSetting("RawDDP/Interpolation", "AHD");
		ri.addSetting("RawDDP/AHD", true);
	}
	else
	{
		ri.addSetting("RawDDP/SuperPixels", false);
		ri.addSetting("RawDDP/RawBayer", false);
		ri.addSetting("RawDDP/Interpolation", "Bilinear");
		ri.addSetting("RawDDP/AHD", false);
	}

	rec.addItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */
#if(0)
static void AddRAWBlackPoint(RECOMMENDATIONVECTOR & vRecommendations, bool bFlat, bool bBias)
{
	RecommendationItem			ri;
	Recommendation				rec;

	if (bBias)
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are using bias frames",
			"IDS_RECO_RAWSETBP_REASON"));
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Set the black point to 0 to improve the calibration",
			"IDS_RECO_RAWSETBP_TEXT"));
		ri.addSetting("RawDDP/BlackPointTo0", true);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (bFlat)
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are using flat frames without bias frames",
			"IDS_RECO_RAWCLEARBP_REASON"));
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Don't set the black point to 0",
			"IDS_RECO_RAWCLEARBP_TEXT"));
		ri.addSetting("RawDDP/BlackPointTo0", false);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};

};
#endif
/* ------------------------------------------------------------------- */

static void AddRegisterUseOfMedianFilter(RECOMMENDATIONVECTOR & vRecommendations)
{
	RecommendationItem			ri;
	Recommendation				rec;
	Workspace					workspace;

	if (Workspace{}.value("Register/DetectionThreshold").toUInt() <= 5)
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are using a low star detection threshold",
			"IDS_RECO_MEDIANFILTER_REASON"));
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Apply a Median Filter before registering the images to reduce the noise and improve the star detection",
			"IDS_RECO_MEDIANFILTER_TEXT"));
		ri.addSetting("Register/ApplyMedianFilter", true);

		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddModdedDSLR(RECOMMENDATIONVECTOR & vRecommendations, bool bFITS)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"If you are using a modified DSLR",
		"IDS_RECO_MODDEDDSLR_REASON"));
	ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
		"Reset all white balance settings",
		"IDS_RECO_MODDEDDSLR_TEXT"));

	if (bFITS)
	{
		ri.addSetting("FitsDDP/Brightness", 1.0);
		ri.addSetting("FitsDDP/RedScale", 1.0);
		ri.addSetting("FitsDDP/BlueScale", 1.0);
	}
	else
	{
		ri.addSetting("RawDDP/NoWB", false);
		ri.addSetting("RawDDP/CameraWB", false);
		ri.addSetting("RawDDP/Brightness", 1.0);
		ri.addSetting("RawDDP/RedScale", 1.0);
		ri.addSetting("RawDDP/BlueScale", 1.0);
	};

	rec.isImportant = false;

	rec.addItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddCometStarTrails(RECOMMENDATIONVECTOR & vRecommendations, int lNrLightFrames)
{
	RecommendationItem			ri;
	Recommendation				rec;
	Workspace					workspace;

	const auto dwCometMode = workspace.value("Stacking/CometStackingMode").toUInt();

	if (dwCometMode == CSM_COMETONLY)
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are trying to create a comet image with star trails",
			"IDS_RECO_COMETSTARTRAILS_REASON"));
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Average combination method",
			"IDS_RECO_USEAVERAGECOMBINE"));

		ri.addSetting("Stacking/Light_Method", (uint)MBP_AVERAGE);

		rec.addItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (dwCometMode == CSM_COMETSTAR)
	{
		rec.setText(QCoreApplication::translate("RecommendedSettings",
			"You are trying to create a comet image aligned on the stars and the comet from %1 light frame(s)",
			"IDS_RECO_COMETSTARSMANY_REASON")
			.arg(lNrLightFrames));

		if (lNrLightFrames>15)
		{
			ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
				"Use Kappa-Sigma clipping combination method",
				"IDS_RECO_USESIGMACLIPPING"));

			ri.addSetting("Stacking/Light_Method", (uint)MBP_SIGMACLIP);
		}
		else
		{
			ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
				"Use Median combination method",
				"IDS_RECO_USEMEDIAN"));

			ri.addSetting("Stacking/Light_Method", (uint)MBP_MEDIAN);
		};
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddLightMethod(RECOMMENDATIONVECTOR & vRecommendations, int lNrFrames)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"You are stacking %1 light frame(s)",
		"IDS_RECO_LIGHT_REASON")
		.arg(lNrFrames));

	if (lNrFrames > 15)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Kappa-Sigma clipping combination method",
			"IDS_RECO_USESIGMACLIPPING"));

		ri.addSetting("Stacking/Light_Method", (uint)MBP_SIGMACLIP);
		rec.addItem(ri);

		ri.clear();

		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Auto Adaptive Weighted Average combination method",
			"IDS_RECO_USEAUTOADAPTIVEAVERAGE"));

		ri.addSetting("Stacking/Light_Method", (uint)MBP_AUTOADAPTIVE);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Average combination method",
			"IDS_RECO_USEAVERAGECOMBINE"));
		ri.addSetting("Stacking/Light_Method", (uint)MBP_AVERAGE);
		rec.addItem(ri);

		ri.clear();

		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median combination method",
			"IDS_RECO_USEMEDIAN"));
		ri.addSetting("Stacking/Light_Method", (uint)MBP_MEDIAN);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddDarkMethod(RECOMMENDATIONVECTOR & vRecommendations, int lNrFrames)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"You are creating a master dark from %1 dark frame(s)",
		"IDS_RECO_DARK_REASON")
		.arg(lNrFrames));

	if (lNrFrames > 15)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median Kappa-Sigma clipping combination method",
			"IDS_RECO_USESIGMAMEDIAN"));
		ri.addSetting("Stacking/Dark_Method", (uint)MBP_MEDIANSIGMACLIP);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median combination method",
			"IDS_RECO_USEMEDIAN"));
		ri.addSetting("Stacking/Dark_Method", (uint)MBP_MEDIAN);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddBiasMethod(RECOMMENDATIONVECTOR & vRecommendations, int lNrFrames)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"You are creating a master bias from %1 bias frame(s)",
		"IDS_RECO_BIAS_REASON")
		.arg(lNrFrames));

	if (lNrFrames > 15)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median Kappa-Sigma clipping combination method",
			"IDS_RECO_USESIGMAMEDIAN"));
		ri.addSetting("Stacking/Offset_Method", (uint)MBP_MEDIANSIGMACLIP);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median combination method",
			"IDS_RECO_USEMEDIAN"));
		ri.addSetting("Stacking/Offset_Method", (uint)MBP_MEDIAN);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddFlatMethod(RECOMMENDATIONVECTOR & vRecommendations, int lNrFrames)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"You are creating a master flat from %1 flat frame(s)",
		"IDS_RECO_FLAT_REASON")
		.arg(lNrFrames));

	if (lNrFrames > 15)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median Kappa-Sigma clipping combination method",
			"IDS_RECO_USESIGMAMEDIAN"));
		ri.addSetting("Stacking/Flat_Method", (uint)MBP_MEDIANSIGMACLIP);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	}
	else if (lNrFrames > 1)
	{
		ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
			"Use Median combination method",
			"IDS_RECO_USEMEDIAN"));
		ri.addSetting("Stacking/Flat_Method", (uint)MBP_MEDIAN);
		rec.addItem(ri);
		vRecommendations.push_back(rec);
	};
};

/* ------------------------------------------------------------------- */

static void AddPerChannelBackgroundCalibration(RECOMMENDATIONVECTOR & vRecommendations)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"If the resulting images look too gray",
		"IDS_RECO_PERCHANNELCALIBRATION_REASON"));
	ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
		"Use Per Channel background calibration",
		"IDS_RECO_USEPERCHANNEL"));

	ri.addSetting("Stacking/BackgroundCalibration", false);
	ri.addSetting("Stacking/PerChannelBackgroundCalibration", true);

	rec.isImportant = false;

	rec.addItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddRGBChannelBackgroundCalibration(RECOMMENDATIONVECTOR & vRecommendations)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"If the color balance in the resulting images is hard to fix in post-processing",
		"IDS_RECO_RGBCALIBRATION_REASON"));
	ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
		"Use RGB background calibration",
		"IDS_RECO_USERGBCALIBRATION"));

	ri.addSetting("Stacking/BackgroundCalibration", true);
	ri.addSetting("Stacking/PerChannelBackgroundCalibration", false);

	rec.isImportant = false;

	rec.addItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

static void AddPerChannelBackgroundCalibrationGray(RECOMMENDATIONVECTOR & vRecommendations)
{
	RecommendationItem			ri;
	Recommendation				rec;

	rec.setText(QCoreApplication::translate("RecommendedSettings",
		"You are stacking grayscale images and they may have slightly different background values",
		"IDS_RECO_PERCHANNELCALIBRATIONGRAY_REASON"));
	ri.setRecommendation(QCoreApplication::translate("RecommendedSettings",
		"Use Per Channel background calibration",
		"IDS_RECO_USEPERCHANNEL"));

	ri.addSetting("Stacking/BackgroundCalibration", false);
	ri.addSetting("Stacking/PerChannelBackgroundCalibration", true);

	rec.addItem(ri);
	vRecommendations.push_back(rec);
};

/* ------------------------------------------------------------------- */

void RecommendedSettings::fillWithRecommendedSettings()
{
	QPalette palette;
	QColor windowTextColour = palette.color(QPalette::WindowText);

	if (pStackingTasks && pStackingTasks->GetNrLightFrames())
	{
		size_t					lPosition;

		insertHeader();

		AddRegisterUseOfMedianFilter(vRecommendations);

		lPosition = vRecommendations.size();

		if (pStackingTasks->AreBayerImageUsed())
		{
			AddRAWDebayering(vRecommendations, pStackingTasks->GetMaxExposureTime());
			AddModdedDSLR(vRecommendations, pStackingTasks->AreFITSImageUsed());
			AddRAWNarrowBandRecommendation(vRecommendations);
		//	if (!pStackingTasks->AreFITSImageUsed())
		//		AddRAWBlackPoint(vRecommendations, pStackingTasks->AreFlatUsed(), pStackingTasks->AreBiasUsed());
		};

		if (pStackingTasks->AreColorImageUsed())
			AddNarrowBandPerChannelBackgroundCalibration(vRecommendations);

		if (vRecommendations.size()!=lPosition)
			vRecommendations[lPosition].breakBefore = true;
		lPosition = vRecommendations.size();

		if (pStackingTasks->IsCometAvailable())
		{
			AddCometStarTrails(vRecommendations, pStackingTasks->GetNrLightFrames());
		};

		if (vRecommendations.size()!=lPosition)
			vRecommendations[lPosition].breakBefore = true;
		lPosition = vRecommendations.size();

		AddLightMethod(vRecommendations, pStackingTasks->GetNrLightFrames());

		if (pStackingTasks->GetNrBiasFrames())
			AddBiasMethod(vRecommendations, pStackingTasks->GetNrBiasFrames());

		if (max(pStackingTasks->GetNrDarkFrames(), pStackingTasks->GetNrDarkFlatFrames()))
			AddDarkMethod(vRecommendations, max(pStackingTasks->GetNrDarkFrames(), pStackingTasks->GetNrDarkFlatFrames()));

		if (pStackingTasks->GetNrFlatFrames())
			AddFlatMethod(vRecommendations, pStackingTasks->GetNrFlatFrames());

		if (vRecommendations.size()!=lPosition)
			vRecommendations[lPosition].breakBefore = true;
		lPosition = vRecommendations.size();

		if (pStackingTasks->AreColorImageUsed())
		{
			AddPerChannelBackgroundCalibration(vRecommendations);
			AddRGBChannelBackgroundCalibration(vRecommendations);
		}
		else
		{
			AddPerChannelBackgroundCalibrationGray(vRecommendations);
		};

		if (vRecommendations.size()!=lPosition)
			vRecommendations[lPosition].breakBefore = true;
		lPosition = vRecommendations.size();

		int					lLastLinkID = 0;
		
		QString strOr(tr("or", "IDS_OR"));

		for (auto& recommendation : vRecommendations)
		{
			QColor			crColor;
			bool				bDifferent = false;

			if (recommendation.breakBefore)
				insertHTML("<hr style=\"background-color:" + windowTextColour.name() + "\">");
			for (size_t j = 0;j<recommendation.vRecommendations.size() && !bDifferent;j++)
			{
				bDifferent = recommendation.vRecommendations[j].differsFromWorkspace();
			};

			if (bDifferent)
			{
				if (recommendation.isImportant)
				{
					if (darkTheme)
						crColor = Qt::yellow;
					else
						crColor = Qt::darkRed;
				}
				else
					crColor = blueColour;
			}
			else
			{
				crColor = windowTextColour;
			}

			insertHTML(recommendation.text, crColor);
			insertHTML("<br>");

			for (size_t j = 0;j<recommendation.vRecommendations.size();j++)
			{
				RecommendationItem &	ri = recommendation.vRecommendations[j];
				bool					bAlreadySet;
				int					lLinkID = 0;

				bAlreadySet = !ri.differsFromWorkspace();

				if (j)
				{
					insertHTML("&nbsp;&nbsp;");
					insertHTML(strOr+"<br>", windowTextColour, false, true);
				};

				insertHTML("->  ", windowTextColour);
				if (bAlreadySet)
				{
					insertHTML(ri.recommendation, qRgb(86, 170, 86));
				}
				else
				{
					lLastLinkID++;
					lLinkID = lLastLinkID;
					insertHTML(ri.recommendation, blueColour , false, false, lLinkID);
				};
				ri.linkID = lLinkID;
			};
			insertHTML("<br><br>");
		};
	}
	else
	{
		insertHTML(tr("You must first add images to the list and check them.", "IDS_RECO_PREREQUISITES"), Qt::red, true);
	};
	insertHTML("</body>");
};

/* ------------------------------------------------------------------- */

void RecommendedSettings::setSetting(int lID)
{
	bool					bFound = false;

	for (size_t i = 0;i<vRecommendations.size() && !bFound;i++)
	{
		for (size_t j = 0;j<vRecommendations[i].vRecommendations.size() && !bFound;j++)
		{
			RecommendationItem &	ri = vRecommendations[i].vRecommendations[j];

			if (ri.linkID == lID)
			{
				bFound = true;
				ri.applySettings();
			};
		};
	};

	if (bFound)
	{
		ui->textBrowser->clear();
		vRecommendations.clear();
		fillWithRecommendedSettings();
	}
};

/* ------------------------------------------------------------------- */