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
#include "pch.h"
#include <QtConcurrentRun>
#include "DeepSkyStacker.h"
#include "ProcessingDlg.h"
#include "MtfAutostretchSettings.h"
#include "oldprogressdlg.h"
#include "selectrect.h"
#include "FrameInfoSupport.h"
#include "SavePicture.h"
#include "StarMaskDlg.h"
#include "StarMask.h"
#include "FITSUtil.h"
#include "TIFFUtil.h"
#include "QLinearGradientCtrl.h"
#include <QMenu>
#define dssApp DeepSkyStacker::instance()

/* ------------------------------------------------------------------- */

namespace
{
#if defined(Q_CC_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunneeded-member-function"
#endif
	class ColorOrder
	{
	public:
		QRgb		m_crColor;		// Qt 32-bit QRgb format (0xAARRGGBB)
		size_t		m_lSize;

		//ColorOrder() :
		//	m_crColor{ qRgb(0, 0, 0) },
		//	m_lSize{ 0 }
		//{}

		explicit ColorOrder(QRgb crColor, size_t lSize) : 
			m_crColor{ crColor },
			m_lSize{ lSize }
		{}
		~ColorOrder() = default;
		ColorOrder(const ColorOrder&) = default;

		ColorOrder& operator=(const ColorOrder&) = default;

		bool operator < (const ColorOrder& co) const
		{
			return m_lSize < co.m_lSize;
		}
	};
#if defined(Q_CC_CLANG)
#pragma clang diagnostic pop
#endif

}

namespace DSS
{
	ProcessingDlg::ProcessingDlg(QWidget *parent, ProcessingControls* processingControls) :
		QWidget(parent),
		controls{ processingControls },
		dirty_ { false },
		undoRedoStack{ dssApp->undoRedoStack() },
		previewTimer{ this },
		redSliderTimer{ this },
		greenSliderTimer{ this },
		blueSliderTimer{ this },
		mtfSliderTimer{ this }
	{
		ZFUNCTRACE_RUNTIME();
		setupUi(this);

		//
		// Allow selection of partial image, don't display "Drizzle" rectangles.
		//
		selectRect = new SelectRect(picture);
		selectRect->setShowDrizzle(false);
		selectRect->rectButtonPressed();

		previewTimer.setInterval(200);		// 200ms
		previewTimer.setSingleShot(true);	// Fires only once after started

		redSliderTimer.setSingleShot(true);		// Fires only once after started
		greenSliderTimer.setSingleShot(true);	// Fires only once after started
		blueSliderTimer.setSingleShot(true);		// Fires only once after started	

		mtfSliderTimer.setSingleShot(true);

		//
		// Disable keyboard tracking for the spin boxes so that the valueChanged
		// signal is only emitted when the user has finished changing the value
		//
		controls->asinhBPSpinBox->setKeyboardTracking(false);
		controls->asinhStretchSpinBox->setKeyboardTracking(false);

		//
		// Disable tracking for the sliders so that the valueChanged signal is 
		// only emitted when the user has finished changing the value
		// 
		controls->asinhBPSlider->setTracking(false);
		controls->asinhStretchSlider->setTracking(false);

		controls->showClipping->setChecked(QSettings{}.value("ShowBlackWhiteClipping", true).toBool());

		controls->mtfRedGradient->setColor(Qt::red);
		controls->mtfGreenGradient->setColor(Qt::green);
		controls->mtfBlueGradient->setColor(Qt::blue);

		QSizePolicy spGreen = controls->mtfGreenGradient->sizePolicy();
		spGreen.setRetainSizeWhenHidden(true);
		controls->mtfGreenGradient->setSizePolicy(spGreen);

		QSizePolicy spBlue = controls->mtfBlueGradient->sizePolicy();
		spBlue.setRetainSizeWhenHidden(true);
		controls->mtfBlueGradient->setSizePolicy(spBlue);

		QIcon linkIcon;
		linkIcon.addFile(":/processing/chain.svg", QSize(), QIcon::Normal, QIcon::Off);
		linkIcon.addFile(":/processing/chain-linked.svg", QSize(), QIcon::Normal, QIcon::On);
		controls->mtfLinkButton->setIcon(linkIcon);
		controls->mtfLinkButton->setIconSize(QSize(16, 16));
		setMtfClippingLabelText(0.0, 0.0);

		connectSignalsToSlots();

		updateControls();


	}

	ProcessingDlg::~ProcessingDlg()
	{
		ZFUNCTRACE_RUNTIME();
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::zeroAsinHControls()
	{
		const QSignalBlocker betaSpinBoxBlocker(controls->asinhStretchSpinBox);
		const QSignalBlocker betaSliderBlocker(controls->asinhStretchSlider);
		const QSignalBlocker bpSpinBoxBlocker(controls->asinhBPSpinBox);
		const QSignalBlocker bpSliderBlocker(controls->asinhBPSlider);

		asinhBeta = 0.0f;
		asinhBP = 0.0f;
		controls->asinhStretchSpinBox->setValue(0.0f);
		controls->asinhStretchSlider->setValue(0);
		controls->asinhStretchSlider->setSliderPosition(0);

		controls->asinhBPSpinBox->setValue(0.0f);
		controls->asinhBPSlider->setValue(0);
		controls->asinhBPSlider->setSliderPosition(0);
	}

	void ProcessingDlg::zeroMtfControls()
	{
		mtfParameters.clipPoint[0] = 0.0f; mtfParameters.clipPoint[1] = 0.0f; mtfParameters.clipPoint[2] = 0.0f;
		mtfParameters.midtoneBalance[0] = 0.5f; mtfParameters.midtoneBalance[1] = 0.5f; mtfParameters.midtoneBalance[2] = 0.5f;
		mtfParameters.whitePoint[0] = 1.0f; mtfParameters.whitePoint[1] = 1.0f; mtfParameters.whitePoint[2] = 1.0f;
		mtfTargetBkg = 0.125f;
		mtfShadowClip = 2.8f;

		controls->mtfRedGradient->setValues(0.0, 0.5, 1.0);
		controls->mtfGreenGradient->setValues(0.0, 0.5, 1.0);
		controls->mtfBlueGradient->setValues(0.0, 0.5, 1.0);
		syncMtfSpinBoxesFromModel();

		if (imageLoaded && !undoRedoStack.empty())
			showHistogram();
		else
			setMtfClippingLabelText(0.0, 0.0);
	}

	void ProcessingDlg::zeroColourBalanceControls()
	{
		const QSignalBlocker redSliderBlocker(controls->redSlider);
		const QSignalBlocker greenSliderBlocker(controls->greenSlider);
		const QSignalBlocker blueSliderBlocker(controls->blueSlider);

		redShift = 0.0f;
		greenShift = 0.0f;
		blueShift = 0.0f;

		//
		// Set the sliders to the middle of their range, which corresponds to zero shift
		// (all have the same range, so just use the red slider)
		//
		auto value = controls->redSlider->maximum() / 2;

		controls->redSlider->setValue(value);
		controls->redSlider->setSliderPosition(value);
		controls->greenSlider->setValue(value);
		controls->greenSlider->setSliderPosition(value);
		controls->blueSlider->setValue(value);
		controls->blueSlider->setSliderPosition(value);
	}

	void ProcessingDlg::setAdjustmentControlDefaults()
	{
		const QSignalBlocker betaSpinBoxBlocker(controls->asinhStretchSpinBox);
		const QSignalBlocker betaSliderBlocker(controls->asinhStretchSlider);
		const QSignalBlocker bpSpinBoxBlocker(controls->asinhBPSpinBox);
		const QSignalBlocker bpSliderBlocker(controls->asinhBPSlider);
		const QSignalBlocker humanWeightedBlocker(controls->asinhHumanWeighted);
		const QSignalBlocker previewBlocker(controls->previewCB);

		asinhBeta = DefaultAsinhBeta;
		asinhBP = DefaultAsinhBP;

		controls->asinhStretchSpinBox->setValue(DefaultAsinhBeta);
		controls->asinhStretchSlider->setValue(static_cast<int>(DefaultAsinhBeta * 10.0f));

		controls->asinhBPSpinBox->setValue(DefaultAsinhBP);
		controls->asinhBPSlider->setValue(static_cast<int>(DefaultAsinhBP * 1000.0f));

		zeroColourBalanceControls();
		zeroMtfControls();

		asinhHWLuminance = true;
		controls->asinhHumanWeighted->setChecked(true);

		preview = true;
		controls->previewCB->setChecked(true);

		//
		// Finally select the stretch tab
		//
		controls->tabWidget->setCurrentWidget(controls->mtfStretchTab);
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::updateControls()
	{
		//
		// Has an image been loaded, if so enable editing controls
		// 
		if (!undoRedoStack.empty() && dssApp->deepStack().IsLoaded())
		{
			controls->setEnabled(true);
			//
			// If there are saved processing settings we can navigate, enable the undo and redo
			// buttons as appropriate
			//
			controls->undoButton->setEnabled(undoRedoStack.backwardAvailable());
			controls->redoButton->setEnabled(undoRedoStack.forwardAvailable());

			StackedBitmap& bmp{ dssApp->deepStack().GetStackedBitmap() };
			bool colour{ !bmp.isMonochrome() };
			controls->colourBalanceTab->setEnabled(colour);

			if (colour)
			{
				mtfMonochrome = false;
				controls->mtfRedGradient->setColor(Qt::red);
				controls->mtfGreenGradient->setVisible(true);
				controls->mtfBlueGradient->setVisible(true);
				controls->mtfLinkButton->setEnabled(true);
			}
			else
			{
				mtfMonochrome = true;
				controls->mtfRedGradient->setColor(Qt::white);
				controls->mtfGreenGradient->setVisible(false);
				controls->mtfBlueGradient->setVisible(false);
				controls->mtfLinkButton->setEnabled(false);
			}
			syncMtfSpinBoxesFromModel();
		}
		else
		{
			controls->setEnabled(false);
		};
	};


	/* ------------------------------------------------------------------- */

	void ProcessingDlg::connectSignalsToSlots()
	{
		connect(selectRect, &SelectRect::selectRectChanged, this, &ProcessingDlg::setSelectionRect);

		connect(controls->undoButton, &QPushButton::pressed, this, &ProcessingDlg::onUndo);
		connect(controls->redoButton, &QPushButton::pressed, this, &ProcessingDlg::onRedo);
		connect(controls->resetButton, &QPushButton::pressed, this, &ProcessingDlg::onReset);

		connect(controls->mtfLinkButton, &QPushButton::toggled, this, &ProcessingDlg::onLinkToggled);
		connect(controls->mtfAutostretchButton, &QPushButton::clicked, this, &ProcessingDlg::onAutostretch);
		controls->mtfAutostretchButton->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(controls->mtfAutostretchButton, &QPushButton::customContextMenuRequested, this, &ProcessingDlg::onAutostretchContextMenu);

		//
		// If the user changes the ASinH stretch settings, update the controls to match and process the change
		//
		connect(controls->asinhStretchSpinBox, &QDoubleSpinBox::valueChanged, this, &ProcessingDlg::asinhStretchSpinBoxChanged);
		connect(controls->asinhStretchSlider, &QSlider::valueChanged, this, &ProcessingDlg::asinhStretchSliderChanged);
		connect(this, &ProcessingDlg::asinhStretchChanged, this, &ProcessingDlg::asinhStretchChangedHandler);

		connect(controls->asinhBPSpinBox, &QDoubleSpinBox::valueChanged, this, &ProcessingDlg::asinhBPSpinBoxChanged);
		connect(controls->asinhBPSlider, &QSlider::valueChanged, this, &ProcessingDlg::asinhBPSliderChanged);
		connect(this, &ProcessingDlg::asinhBPChanged, this, &ProcessingDlg::asinhBPChangedHandler);

		connect(controls->asinhHumanWeighted, &QCheckBox::checkStateChanged, this, &ProcessingDlg::asinhHumanWeightedChanged);

		connect(controls->asinhApply, &QPushButton::pressed, this, &ProcessingDlg::asinhApplyPressed);

		connect(controls->redSlider, &QSlider::valueChanged, this, [this]() { redSliderTimer.start(200);  });
		connect(&redSliderTimer, &QTimer::timeout, this, [this]() { emit redSliderChanged(controls->redSlider->value()); });
		connect(controls->greenSlider, &QSlider::valueChanged, this, [this]() { greenSliderTimer.start(200);  });
		connect(&greenSliderTimer, &QTimer::timeout, this, [this]() { emit greenSliderChanged(controls->greenSlider->value()); });
		connect(controls->blueSlider, &QSlider::valueChanged, this, [this]() { blueSliderTimer.start(200);  });
		connect(&blueSliderTimer, &QTimer::timeout, this, [this]() { emit blueSliderChanged(controls->blueSlider->value()); });

		connect(controls->cbApply, &QPushButton::pressed, this, &ProcessingDlg::cbApplyPressed);

		//
		// If the user changes the MTF stretch settings, update the controls to match and process the change
		//
		connect(controls->mtfRedGradient, &QMTFSlider::sliderMoved, this, &ProcessingDlg::mtfRedGradientSliderMoved);
		connect(controls->mtfGreenGradient, &QMTFSlider::sliderMoved, this, &ProcessingDlg::mtfGreenGradientSliderMoved);
		connect(controls->mtfBlueGradient, &QMTFSlider::sliderMoved, this, &ProcessingDlg::mtfBlueGradientSliderMoved);
		connect(controls->mtfRedGradient, &QMTFSlider::sliderClicked, this, [this]() {
			controls->mtfGreenGradient->clearSelectedHandle();
			controls->mtfBlueGradient->clearSelectedHandle();
			activeMtfChannel = 0;
			syncMtfSpinBoxesFromModel();
		});
		connect(controls->mtfGreenGradient, &QMTFSlider::sliderClicked, this, [this]() {
			controls->mtfRedGradient->clearSelectedHandle();
			controls->mtfBlueGradient->clearSelectedHandle();
			activeMtfChannel = 1;
			syncMtfSpinBoxesFromModel();
		});
		connect(controls->mtfBlueGradient, &QMTFSlider::sliderClicked, this, [this]() {
			controls->mtfRedGradient->clearSelectedHandle();
			controls->mtfGreenGradient->clearSelectedHandle();
			activeMtfChannel = 2;
			syncMtfSpinBoxesFromModel();
		});

		connect(controls->mtfShadowsSpinBox, &QDoubleSpinBox::valueChanged, this, &ProcessingDlg::mtfShadowsSpinBoxChanged);
		connect(controls->mtfMidtonesSpinBox, &QDoubleSpinBox::valueChanged, this, &ProcessingDlg::mtfMidtonesSpinBoxChanged);
		connect(controls->mtfHighlightsSpinBox, &QDoubleSpinBox::valueChanged, this, &ProcessingDlg::mtfHighlightsSpinBoxChanged);

		connect(&mtfSliderTimer, &QTimer::timeout, this, [this]() {
			if (preview) emit onPreview(ProcessingFunction::MtfStretch);
			else
			{
				controls->mtfApply->setEnabled(true);
				showHistogram();
			}
		});

		connect(controls->mtfApply, &QPushButton::pressed, this, &ProcessingDlg::mtfApplyPressed);

		connect(controls->previewCB, &QCheckBox::checkStateChanged, this, &ProcessingDlg::previewChanged);

		connect(controls->showClipping, &QCheckBox::checkStateChanged,
			this, &ProcessingDlg::clippingStateChanged);

		connect(picture, &DSS::ImageView::mouseMovedOverImage,
			this, &ProcessingDlg::updatePixelInfo);
		
		connect(controls->tabWidget, &QTabWidget::currentChanged,
			this, &ProcessingDlg::tabChanged);
	}

	/* ------------------------------------------------------------------- */

	bool ProcessingDlg::saveOnClose()
	{
		return askToSave();
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::copyToClipboard()
	{
		ZFUNCTRACE_RUNTIME();
		if (undoRedoStack.size() > 0 && dssApp->deepStack().IsLoaded())
		{
			QClipboard* clipboard = QGuiApplication::clipboard();

			QPixmap pix{ picture->grab() };
			clipboard->setPixmap(pix);
		}
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::createStarMask()
	{
		ZFUNCTRACE_RUNTIME();
		if (undoRedoStack.size() > 0 && dssApp->deepStack().IsLoaded())
		{
			StarMaskDlg dlg{ this, currentFile };

			if (QDialog::Accepted == dlg.exec())
			{
				OldProgressDlg progress{ DeepSkyStacker::instance() };
				StarMaskEngine starMaskEngine;

				progress.SetJointProgress(true);
				std::shared_ptr<CMemoryBitmap> pBitmap = dssApp->deepStack().GetStackedBitmap().GetBitmap(&progress);
				if (std::shared_ptr<CMemoryBitmap> pStarMask = starMaskEngine.createStarMask(pBitmap.get(), &progress))
				{
					// Save the star mask to a file
					fs::path file{ dlg.outputFile() };
					bool isFits{ dlg.outputIsFits() };

					const QString description{ tr("Star Mask created by DeepSkyStacker", "IDS_STARMASKDESCRIPTION") };

					const QString strText(tr("Saving the Star Mask in %1", "IDS_SAVINGSTARMASK").arg(file.generic_u16string()));
					progress.Start2(strText, 0);
					if (isFits)
						WriteFITS(file, pStarMask.get(), &progress, description);
					else
						WriteTIFF(file, pStarMask.get(), &progress, description);
				}
				progress.End2();

			}

		}
		return;
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::loadStackedImage(const fs::path& file)
	{
		ZFUNCTRACE_RUNTIME();
		imageLoaded = false;

		//
		// Load the output file created at the end of the stacking process.
		//
		OldProgressDlg dlg{ DeepSkyStacker::instance() };
		bool ok { false };

		undoRedoStack.clear();
		//
		// Initialize the undo-redo image stack with an unpopulated DeepStack
		//
		undoRedoStack.add(DeepStack{});

		//
		// Reset the preview DeepStack object so we don't pick up old previewmages
		//
		previewDeepStack.reset();

		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		dssApp->deepStack().reset();
		dssApp->deepStack().SetProgress(&dlg);
		ok = dssApp->deepStack().LoadStackedInfo(file);
		dssApp->deepStack().SetProgress(nullptr);
		QGuiApplication::restoreOverrideCursor();

		if (ok)
		{
			imageLoaded = true;
			currentFile = file;

			updateInformation();

			showHistogram(); 

			picture->clear();
			setAdjustmentControlDefaults();

			updateControls();

			//
			// Do this before the preview stuff otherwise the dirty flag isn't cleared if preview is active
			// 
			setDirty(false);

			if (preview)
			{
				emit onPreview(ProcessingFunction::MtfStretch);
			}
			else
			{
				processAndShow();
			}
		}
		else
		{
			QApplication::beep();
			QString message{ QString(tr("Failed to load image %1").arg(file.generic_u16string())) };
			QMessageBox::warning(this, "DeepSkyStacker",
				message);
		}
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::loadImage()
	{
		ZFUNCTRACE_RUNTIME();
		imageLoaded = false;
		bool ok{ false };

		if (askToSave())
		{

			QFileDialog			fileDialog(this);
			QSettings			settings;
			QString				directory;
			QString				extension;
			QString				strTitle;
			fs::path file;

			//
			// Always use the Qt Widget file dialog for consistency
			// 
			fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

			DSS::OldProgressDlg dlg{ DeepSkyStacker::instance() };

			directory = settings.value("Folders/SaveDSIFolder").toString();
			extension = settings.value("Folders/SavePictureExtension").toString();
			if (extension.isEmpty()) extension = "tif";
			fileDialog.setDefaultSuffix(extension);
			fileDialog.setFileMode(QFileDialog::ExistingFile);	// There can be only one

			fileDialog.setNameFilter(tr("TIFF and FITS Files (*.tif *.tiff *.fits *.fit *.fts)", "IDS_FILTER_DSIIMAGETIFF"));
			fileDialog.selectFile(QString());		// No file(s) selected
			if (!directory.isEmpty())
				fileDialog.setDirectory(directory);

			if (QDialog::Accepted == fileDialog.exec())
			{
				QStringList files = fileDialog.selectedFiles();

				//
				// Now get the file as a standard fs::path object
				//
				if (!files.empty())		// Never, ever attempt to add zero rows!!!
				{
					file = files.at(0).toStdU16String();

					if (file.has_parent_path())
						directory = QString::fromStdU16String(file.parent_path().generic_u16string());
					else
						directory = QString::fromStdU16String(file.root_path().generic_u16string());

					extension = QString::fromStdU16String(file.extension().generic_u16string());
				}

				settings.setValue("Folders/SaveDSIFolder", directory);
				settings.setValue("Folders/SavePictureExtension", extension);

				undoRedoStack.clear();
				//
				// Initialize the undo-redo image stack with an unpopulated DeepStack
				//
				undoRedoStack.add(DeepStack{});

				//
				// Reset the preview DeepStack object so we don't pick up old previewmages
				//
				previewDeepStack.reset();

				QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				dssApp->deepStack().reset();
				dssApp->deepStack().SetProgress(&dlg);
				ok = dssApp->deepStack().LoadStackedInfo(file);
				dssApp->deepStack().SetProgress(nullptr);
				QGuiApplication::restoreOverrideCursor();

				if (ok)
				{
					imageLoaded = true;
					currentFile = file;

					updateInformation();

					picture->clear();
					setAdjustmentControlDefaults();

					updateControls();

					//
					// Do this before preview stuff otherwise the dirty flag doesn't get reset if preview is true
					// 
					setDirty(false);

					if (preview)
					{
						onPreview(ProcessingFunction::MtfStretch);
					}
					else
					{
						processAndShow();
						showHistogram();
					}
				}
				else
				{
					QApplication::beep();
					QString message{ QString(tr("Failed to load image %1").arg(file.generic_u16string())) };
					QMessageBox::warning(this, "DeepSkyStacker",
						message);
				}
			}
		}
	}

	/* ------------------------------------------------------------------- */

	bool ProcessingDlg::saveImage()
	{
		ZFUNCTRACE_RUNTIME();
		bool result = false;


		if (undoRedoStack.size() > 0 && dssApp->deepStack().IsLoaded())
		{
			QSettings settings;

			auto directory{ settings.value("Folders/SavePictureFolder").toString() };
			auto extension{ settings.value("Folders/SavePictureExtension").toString().toLower() };
			auto apply{ settings.value("Folders/SaveApplySetting", false).toBool() };
			TIFFCOMPRESSION compression{ static_cast<TIFFCOMPRESSION>(
				settings.value("Folders/SaveCompression", static_cast<uint>(TC_NONE)).toUInt()) };
			auto filterIndex{ settings.value("Folders/SavePictureIndex", 0).toUInt() };
			if (filterIndex > 5) filterIndex = 0;

			QStringList fileFilters{
				tr("TIFF Image 16 bit/ch (*.tif)", "IDS_FILTER_OUTPUT"),
				tr("TIFF Image 32 bit/ch - integer (*.tif)", "IDS_FILTER_OUTPUT"),
				tr("TIFF Image 32 bit/ch - rational (*.tif)", "IDS_FILTER_OUTPUT"),
				tr("FITS Image 16 bit/ch (*.fits)", "IDS_FILTER_OUTPUT"),
				tr("FITS Image 32 bit/ch - integer (*.fits)", "IDS_FILTER_OUTPUT"),
				tr("FITS Image 32 bit/ch - rational (*.fits)", "IDS_FILTER_OUTPUT")
			};

			if (filterIndex > 2) extension = ".fits";
			else extension = ".tif";

			//
			// SavePicture is a sub-class of QFileDialog, we'll set the QFileDialog vars first
			//
			SavePicture dlg{ this, tr("Save Image"), directory };
			dlg.setDefaultSuffix(extension);
			dlg.setNameFilters(fileFilters);
			auto filter{ fileFilters.at(filterIndex) };
			dlg.selectNameFilter(filter);
			//
			// selectNameFilter doesn't fire the QFileDialog::filterSelected signal
			// so need to drive the slot ourself
			//
			dlg.onFilter(filter);

			//
			// Now set our sub-class variables
			//
			dlg.setCompression(TIFFCOMPRESSION(compression));
			dlg.setApply(apply);
			if (!selectionRect.isEmpty()) dlg.setUseRect(true);

			//
			// display the dialogue
			//
			if (QDialog::Accepted == dlg.exec())
			{
				fs::path file = dlg.selectedFiles().at(0).toStdU16String();

				apply = dlg.apply();
				compression = dlg.compression();
				bool useRect = dlg.useRect();
				DSSRect rect;			// Empty rectangle
				DSS::OldProgressDlg progress{ DeepSkyStacker::instance() };


				//
				// If only wish to save the selected rectangle, copy it
				// to our working rectangle
				//
				if (useRect) rect = selectionRect;

				//
				// Save the image in the format the user has selected
				//
				auto index{ fileFilters.indexOf(dlg.selectedNameFilter()) };
				//
				// If the user has asked to apply the editing changes, choose the
				// current image from the undo-redo stack, otherwise use the original
				// stacked bitmap
				//
				StackedBitmap& stackedBitmap{ undoRedoStack.at(0).GetStackedBitmap() };

				if (apply)
				{
					stackedBitmap = undoRedoStack.current().GetStackedBitmap();
				}

				QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

				switch (index)
				{
				case 0:		// TIFF 16 bit
					stackedBitmap.SaveTIFF16Bitmap(file, rect, &progress, compression);
					break;
				case 1:		// TIFF 32 bit integer
					stackedBitmap.SaveTIFF32Bitmap(file, rect, &progress, false, compression);
					break;
				case 2:		// TIFF 32 bit rational
					stackedBitmap.SaveTIFF32Bitmap(file, rect, &progress, true, compression);
					break;
				case 3:		// FITS 16 bit
					stackedBitmap.SaveFITS16Bitmap(file, rect, &progress);
					break;
				case 4:		// FITS 32 bit integer
					stackedBitmap.SaveFITS32Bitmap(file, rect, &progress, false);
					break;
				case 5:		// FITS 32 bit rational
					stackedBitmap.SaveFITS32Bitmap(file, rect, &progress, true);
					break;
				default: break;
				}

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());

				settings.setValue("Folders/SavePictureFolder", directory);
				settings.setValue("Folders/SavePictureExtension", extension);
				settings.setValue("Folders/SavePictureIndex", index);
				settings.setValue("Folders/SaveApplySetting", apply);
				settings.setValue("Folders/SaveCompression", static_cast<uint>(compression));

				QGuiApplication::restoreOverrideCursor();
				currentFile = file;
				updateInformation();
				setDirty(false);
				result = true;

			}
		}
		else
		{
			QMessageBox::information(this, "DeepSkyStacker",
				tr("There is no picture to save.", "IDS_MSG_NOPICTURETOSAVE"));
		}

		return result;
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::updateInformation()
	{
		ZFUNCTRACE_RUNTIME();
		int		isoSpeed;
		int		gain;
		int		totalTime;
		int		nrFrames;
		QString text{ tr("No information available", "IDS_NOINFO") };

		if (!currentFile.empty())
		{
			StackedBitmap& bmp{ dssApp->deepStack().GetStackedBitmap() };

			//
			// Technically this doesn't belong here, but it is a very convenient place to 
			// enable/disable the colour balance controls based on whether the image is
			// is monochrome or not.
			//
			bool colour{ !bmp.isMonochrome() };
			controls->colourBalanceTab->setEnabled(colour);

			isoSpeed = bmp.GetISOSpeed();
			gain = bmp.GetGain();
			totalTime = bmp.GetTotalTime();
			nrFrames = bmp.GetNrStackedFrames();
			QString isoText, gainText, timeText, framesText;

			if (isoSpeed) isoText = QString("%1 ISO - ").arg(isoSpeed);
			if (gain >= 0) gainText = QString("%1 Gain - ").arg(gain);
			if (totalTime) timeText = tr("Exposure: %1 ").arg(exposureToString(totalTime));
			if (nrFrames) framesText = tr("%n frames", "IDS_NRFRAMES", nrFrames);

			text = QString("%1\n%2%3%4(%5)").arg(currentFile.generic_u8string().c_str())
				.arg(isoText)
				.arg(gainText)
				.arg(timeText)
				.arg(framesText);
		}

		information->setText(text);
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::showHistogram()
	{
		
		// Adjust Histogram
		RGBHistogram histogram{};
		histogram.SetSize(65535.0, static_cast<size_t>(controls->histogram->width()));

		if (usePreviewDeepStack)
		{
			//
			// If using the preview DeepStack, need to compute the histogram from the preview bitmap
			//
			previewDeepStack.computeDisplayHistogram(histogram);
		}
		else
		{
			undoRedoStack.current().computeDisplayHistogram(histogram);
		}

		updateMtfClippingLabels(histogram);
		
		drawHistogram(histogram);
	}

	void ProcessingDlg::updateMtfClippingLabels(RGBHistogram& histogram)
	{
		const size_t binCount = histogram.GetRedHistogram().GetNrValues();
		if (binCount == 0)
		{
			setMtfClippingLabelText(0.0, 0.0);
			return;
		}

		const bool monochrome = !controls->mtfGreenGradient->isVisible() || !controls->mtfBlueGradient->isVisible();

		double shadowThreshold = mtfParameters.clipPoint[0];	// Default to red channel shadow threshold
		double highlightThreshold = mtfParameters.whitePoint[0];	// Default to red channel highlight threshold
		if (!monochrome)
		{
			shadowThreshold = (mtfParameters.clipPoint[0] + mtfParameters.clipPoint[1] + mtfParameters.clipPoint[2]) / 3.0;
			highlightThreshold = (mtfParameters.whitePoint[0] + mtfParameters.whitePoint[1] + mtfParameters.whitePoint[2]) / 3.0;
		}

		shadowThreshold = std::clamp(shadowThreshold, 0.0, 1.0);
		highlightThreshold = std::clamp(highlightThreshold, 0.0, 1.0);

		const double maxBinIndex = static_cast<double>(binCount - 1);
		const size_t shadowBin = static_cast<size_t>(std::clamp(shadowThreshold * maxBinIndex, 0.0, maxBinIndex));
		const size_t highlightBin = static_cast<size_t>(std::clamp(highlightThreshold * maxBinIndex, 0.0, maxBinIndex));

		double total[3] = { 0.0, 0.0, 0.0 };
		double clippedShadow[3] = { 0.0, 0.0, 0.0 };
		double clippedHighlight[3] = { 0.0, 0.0, 0.0 };

		for (size_t i = 0; i < binCount; i++)
		{
			double redCount = 0.0;
			double greenCount = 0.0;
			double blueCount = 0.0;
			histogram.GetValues(i, redCount, greenCount, blueCount);

			total[0] += redCount;
			total[1] += greenCount;
			total[2] += blueCount;

			if (i <= shadowBin)
			{
				clippedShadow[0] += redCount;
				clippedShadow[1] += greenCount;
				clippedShadow[2] += blueCount;
			}
			if (i >= highlightBin)
			{
				clippedHighlight[0] += redCount;
				clippedHighlight[1] += greenCount;
				clippedHighlight[2] += blueCount;
			}
		}

		double shadowPercent = 0.0;
		double highlightPercent = 0.0;
		if (monochrome)
		{
			if (total[0] > 0.0)
			{
				shadowPercent = (clippedShadow[0] / total[0]) * 100.0;
				highlightPercent = (clippedHighlight[0] / total[0]) * 100.0;
			}
		}
		else
		{
			double shadowAccumulator = 0.0;
			double highlightAccumulator = 0.0;
			int validChannels = 0;

			for (int ch = 0; ch < 3; ch++)
			{
				if (total[ch] <= 0.0)
					continue;

				shadowAccumulator += (clippedShadow[ch] / total[ch]) * 100.0;
				highlightAccumulator += (clippedHighlight[ch] / total[ch]) * 100.0;
				validChannels++;
			}

			if (validChannels > 0)
			{
				shadowPercent = shadowAccumulator / static_cast<double>(validChannels);
				highlightPercent = highlightAccumulator / static_cast<double>(validChannels);
			}
		}

		setMtfClippingLabelText(shadowPercent, highlightPercent);
	}

	//
	// Split clipping labels into separate text and value labels for precise layout control:
	// - Text label ("Shadow Clipping:") remains fixed width
	// - Value label ("X.XXX%") is right-aligned and can be independently styled
	//
	void ProcessingDlg::setMtfClippingLabelText(double shadowPercent, double highlightPercent)
	{
		controls->mtfShadowClipValueLabel->setText(
			QString("<b>%L1%</b>").arg(QString::number(std::clamp(shadowPercent, 0.0, 100.0), 'f', 3)));
		controls->mtfHighlightClipValueLabel->setText(
			QString("<b>%L1%</b>").arg(QString::number(std::clamp(highlightPercent, 0.0, 100.0), 'f', 3)));
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::drawHistoBar(QPainter& painter, double redCount, double greenCount, double blueCount, size_t X, int lHeight)
	{
		std::array<ColorOrder, 3> vColors = {{ 
			ColorOrder{qRgb(255, 0, 0), static_cast<size_t>(redCount)},
			ColorOrder{qRgb(0, 255, 0), static_cast<size_t>(greenCount)},
			ColorOrder{qRgb(0, 0, 255), static_cast<size_t>(blueCount)} }};
		int lLastHeight = 0;

		std::sort(vColors.begin(), vColors.end());

		for (int i = 0; i < vColors.size(); i++)
		{
			if (vColors[i].m_lSize > lLastHeight)
			{
				// Create a color from the remaining values
				int lNrColors = 1;

				double fRed = qRed(vColors[i].m_crColor);		// Get the red component of the colour
				double fGreen = qGreen(vColors[i].m_crColor);	// Get the green component of the colour
				double fBlue = qBlue(vColors[i].m_crColor);	// Get the blue component of the colour

				for (int j = i + 1; j < vColors.size(); j++)
				{
					fRed += qRed(vColors[j].m_crColor);		// Get the red component of the colour
					fGreen += qGreen(vColors[j].m_crColor);	// Get the green component of the colour
					fBlue += qBlue(vColors[j].m_crColor);	// Get the blue component of the colour
					lNrColors++;
				}

				QPen colorPen(QColor(static_cast<int>(fRed / lNrColors), static_cast<int>(fGreen / lNrColors), static_cast<int>(fBlue / lNrColors)));
				painter.setPen(colorPen);
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)
#endif
				painter.drawLine(static_cast<int>(X), lHeight - lLastHeight, static_cast<int>(X), lHeight - static_cast<int>(vColors[i].m_lSize));

				lLastHeight = static_cast<int>(vColors[i].m_lSize);
#ifdef _MSC_VER
#pragma warning(pop)
#endif

			}
		}
	}

	/* ------------------------------------------------------------------- */
	void ProcessingDlg::drawGaussianCurves(QPainter& painter, RGBHistogram& histo, int lWidth, int lHeight)
	{
		size_t				binCount;
		double				fAverage[3] = { 0, 0, 0 };
		double				fStdDev[3] = { 0, 0, 0 };
		double				fSum[3] = { 0, 0, 0 };
		double				fSquareSum[3] = { 0, 0, 0 };
		double				fTotalPixels[3] = { 0, 0, 0 };
		int				i;

		binCount = histo.GetRedHistogram().GetNrValues();

		if (binCount)
		{
			for (i = 0; i < binCount; i++)
			{

				double	redCount;
				double	greenCount;
				double	blueCount;

				histo.GetValues(i, redCount, greenCount, blueCount);

				fSum[0] += redCount * i;	
				fSum[1] += greenCount * i;
				fSum[2] += blueCount * i;
				fTotalPixels[0] += redCount;
				fTotalPixels[1] += greenCount;
				fTotalPixels[2] += blueCount;
			};

			fAverage[0] = fSum[0] / fTotalPixels[0];
			fAverage[1] = fSum[1] / fTotalPixels[1];
			fAverage[2] = fSum[2] / fTotalPixels[2];

			for (i = 0; i < binCount; i++)
			{
				double redCount;
				double greenCount;
				double blueCount;

				histo.GetValues(i, redCount, greenCount, blueCount);

				fSquareSum[0] += pow(i - fAverage[0], 2) * redCount;
				fSquareSum[1] += pow(i - fAverage[1], 2) * greenCount;
				fSquareSum[2] += pow(i - fAverage[2], 2) * blueCount;
			};

			fStdDev[0] = sqrt(fSquareSum[0] / fTotalPixels[0]);
			fStdDev[1] = sqrt(fSquareSum[1] / fTotalPixels[1]);
			fStdDev[2] = sqrt(fSquareSum[2] / fTotalPixels[2]);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4267)
#endif		
			//
			// Create the list of points with the initial size set correctly.
			//
			QList<QPointF>	redPoints{ static_cast<qsizetype>(binCount) };
			QList<QPointF>	greenPoints{ static_cast<qsizetype>(binCount) };
			QList<QPointF>	bluePoints{ static_cast<qsizetype>(binCount) };
#ifdef _MSC_VER
#pragma warning(pop)
#endif

			bool				bShow = true;

			for (i = 0; i < binCount; i++)
			{
				double		fX,
					fY;
				fX = i;

				fY = exp(-(fX - fAverage[0]) * (fX - fAverage[0]) / (fStdDev[0] * fStdDev[0] * 2)) * lWidth / static_cast<double>(binCount);
				fY = lHeight - fY * lHeight;
				redPoints.emplace_back(fX, fY);

				bShow = bShow && (fX < 1000 && fY < 1000);

				fY = exp(-(fX - fAverage[1]) * (fX - fAverage[1]) / (fStdDev[1] * fStdDev[1] * 2)) * lWidth / static_cast<double>(binCount);
				fY = lHeight - fY * lHeight;
				greenPoints.emplace_back(fX, fY);

				bShow = bShow && (fX < 1000 && fY < 1000);

				fY = exp(-(fX - fAverage[2]) * (fX - fAverage[2]) / (fStdDev[2] * fStdDev[2] * 2)) * lWidth / static_cast<double>(binCount);
				fY = lHeight - fY * lHeight;
				bluePoints.emplace_back(fX, fY);

				bShow = bShow && (fX < 1000 && fY < 1000);
			};

			QPen redPen{ qRgba(255, 0, 0, 128) };
			QPen greenPen{ qRgba(0, 255, 0, 128) };
			QPen bluePen{ qRgba(0, 0, 255, 128) };

			redPen.setStyle(Qt::DashLine);
			greenPen.setStyle(Qt::DashLine);
			bluePen.setStyle(Qt::DashLine);

			if (bShow)
			{
				QPen oldPen = painter.pen();
				painter.setPen(redPen); painter.drawLines(redPoints);
				painter.setPen(greenPen); painter.drawLines(greenPoints);
				painter.setPen(bluePen); painter.drawLines(bluePoints);
				painter.setPen(oldPen);
			};
		};
	};

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::drawHistogram(RGBHistogram& Histogram)
	{
		QPixmap pix(controls->histogram->size());
		QPainter painter;
		QBrush brush(palette().button());  // QPalette::Window was too dark - use QPalette::Button instead
		const QRect histogramRect{ controls->histogram->rect() };
		const int width{ histogramRect.width() };
		const int height{ histogramRect.height() };

		painter.begin(&pix);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);

		painter.fillRect(histogramRect, brush);

		size_t	maxValue = 0;

		maxValue = std::max(maxValue, Histogram.GetRedHistogram().GetMaximumNrValues());
		maxValue = std::max(maxValue, Histogram.GetGreenHistogram().GetMaximumNrValues());
		maxValue = std::max(maxValue, Histogram.GetBlueHistogram().GetMaximumNrValues());

		double	maxLogarithm = 0.0;
		bool useLogarithm = useLogarithmicHistogram && maxValue > 0;
		size_t binCount = Histogram.GetRedHistogram().GetNrValues();

		if (binCount)
		{
			if (useLogarithm)
			{
				maxLogarithm = exp(log(static_cast<double>(maxValue)) / height);
			}

			for (size_t i = 0; i < binCount; i++)
			{
				double	redCount;
				double	greenCount;
				double	blueCount;

				Histogram.GetValues(i, redCount, greenCount, blueCount);
				if (useLogarithm)
				{
					if (0.0 != redCount)
						redCount = log(static_cast<double>(redCount)) / log(maxLogarithm);
					if (0.0 != greenCount)
						greenCount = log(static_cast<double>(greenCount)) / log(maxLogarithm);
					if (0.0 != blueCount)
						blueCount = log(static_cast<double>(blueCount)) / log(maxLogarithm);
				}
				else
				{
					redCount = static_cast<double>(redCount) / static_cast<double>(maxValue) * height;
					greenCount = static_cast<double>(greenCount) / static_cast<double>(maxValue) * height;
					blueCount = static_cast<double>(blueCount) / static_cast<double>(maxValue) * height;
				}

				drawHistoBar(painter, redCount, greenCount, blueCount, i, height);
			};
		}
		drawGaussianCurves(painter, Histogram, width, height);

		painter.end();
		controls->histogram->setPixmap(pix);
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::processAndShow()
	{
		DSSRect			cell;
		//
		// selectionRect is set whenever signal SelectRect::selectRectChanged is emitted
		// It will be the null rectangle when no selection has been made by the user
		//
		if (!selectionRect.isEmpty())
			cell = selectionRect;

		if (usePreviewDeepStack)
		{
			//
			// If using the preview DeepStack, need to display the image and histogram
			// for the image preview DeepStack
			//
			previewDeepStack.PartialProcess(cell);
			picture->setImage(previewDeepStack.getImage());
		}
		else
		{
			DeepStack& deepStack = undoRedoStack.current();
			deepStack.PartialProcess(cell);
			picture->setImage(deepStack.getImage());
		}
	}

	bool ProcessingDlg::askToSave()
	{
		ZFUNCTRACE_RUNTIME();
		//
		// The existing image is being closed and has been changed, ask the user if they want to save it
		//
		if (dirty_)
		{
			QString message{ tr("Do you want to save the modifications?", "IDS_MSG_SAVEMODIFICATIONS") };
			auto result = QMessageBox::question(this, "DeepSkyStacker",
				message, (QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel), QMessageBox::No);
			switch (result)
			{
			case QMessageBox::Cancel:
				return false;
				break;
			case QMessageBox::No:
				return true;
				break;
			default:
				return saveImage();
			}
		}
		else return true;
	}

	void ProcessingDlg::doPreview(ProcessingFunction function)
	{
		previewMutex.lock();
		// Copy the current DeepStack object from the undo-redo stack and process it with the current settings	
		previewDeepStack = undoRedoStack.current();

		StackedBitmap& bitmap{ previewDeepStack.GetStackedBitmap() };

		//
		// Normalise the image to a range of [0.0, 1.0], which is required for
		// the processing
		//
		bitmap.normalise();

		switch (function)
		{
		case ProcessingFunction::MtfStretch:
			//
			// Apply the MTF stretch using specific shadow, midtone, and highlight parameters
			//
			bitmap.mtfStretch(mtfParameters);
			break;

		case ProcessingFunction::AsinhStretch:
			//
			// Apply the ASinH stretch to the image and set the stretch values to zero
			//
			bitmap.asinhStretch(asinhBeta, asinhBP, asinhHWLuminance);
			break;

		case ProcessingFunction::ColourBalance:
			//
			// Adjust the colour balance of the image and
			// reset the colour balance shifts to zero
			//
			bitmap.adjustColourBalance(redShift, greenShift, blueShift);
			break;

		default:
			ZASSERT(false);	// Invalid processing function
			break;
		}

		//
		// Now de-normalise the image back to the original range
		//
		bitmap.deNormalise();

		//
		// Calculate the histogram for the changed image so that it can be
		// displayed when the preview is shown
		//
		previewDeepStack.computeHistogram();

		//
		// The preview has been processed, use the preview DeepStack object
		// to update the preview image and histogram
		//
		usePreviewDeepStack = true;	// Use the preview DeepStack object

		processAndShow();
		showHistogram();

		usePreviewDeepStack = false;
		previewMutex.unlock();

		//
		// enable the appropriate Apply button to allow the user to apply the adjustment
		// to the main image if they are happy with the preview.
		// 
		// Note that we need to use QMetaObject::invokeMethod() rather than emit as we are
		// not running in the same thread as the main GUI thread.
		//
		switch (function)
		{
		case ProcessingFunction::MtfStretch:
			//
			// Enable the Apply button for the MTF stretch controls
			//
			QMetaObject::invokeMethod(controls->mtfApply, "setEnabled", Qt::ConnectionType::AutoConnection, Q_ARG(bool, true));
			break;

		case ProcessingFunction::AsinhStretch:
			//
			// Enable the Apply button for the ASinH stretch controls
			//
			QMetaObject::invokeMethod(controls->asinhApply, "setEnabled", Qt::ConnectionType::AutoConnection, Q_ARG(bool, true));
			break;

		case ProcessingFunction::ColourBalance:
			//
			// Enable the Apply button for the Colour Balance controls
			//
			QMetaObject::invokeMethod(controls->cbApply, "setEnabled", Qt::ConnectionType::AutoConnection, Q_ARG(bool, true));
			break;

		default:
			ZASSERT(false);	// Invalid processing function
			break;
		}		
	}

	//
	// Slots
	//
	void ProcessingDlg::onPreview(ProcessingFunction function)
	{
		if (imageLoaded)
		{
			std::ignore = QtConcurrent::run(&ProcessingDlg::doPreview, this, function);
		}
	}

	void ProcessingDlg::onApply(ProcessingFunction function)
	{
		//
		// Get the current DeepStack object from the undo-redo stack and duplicate it at the top
		// of the undo-redo stack.
		undoRedoStack.add(undoRedoStack.current());

		//
		// Now process the image with the current settings and show the result
		//
		DeepStack& deepStack = undoRedoStack.current();

		StackedBitmap& bitmap{ deepStack.GetStackedBitmap() };

		//
		// Normalise the image to a range of [0.0, 1.0], which is required for
		// the ASinH stretch and colour balance processing
		//
		bitmap.normalise();

		switch (function)
		{
		case ProcessingFunction::MtfStretch:
			//
			// Apply the MTF stretch and set the stretch values to zero
			//
			bitmap.mtfStretch(mtfParameters);
			deepStack.setDescription(tr("MTF stretch: R %L1 %L2 %L3, G %L4 %L5 %L6, B %L7 %L8 %L9")
				.arg(mtfParameters.clipPoint[0], 0, 'f', 4).arg(mtfParameters.midtoneBalance[0], 0, 'f', 4).arg(mtfParameters.whitePoint[0], 0, 'f', 4)
				.arg(mtfParameters.clipPoint[1], 0, 'f', 4).arg(mtfParameters.midtoneBalance[1], 0, 'f', 4).arg(mtfParameters.whitePoint[1], 0, 'f', 4)
				.arg(mtfParameters.clipPoint[2], 0, 'f', 4).arg(mtfParameters.midtoneBalance[2], 0, 'f', 4).arg(mtfParameters.whitePoint[2], 0, 'f', 4));

			zeroMtfControls();
			zeroAsinHControls();
			break;

		case ProcessingFunction::AsinhStretch:
			//
			// Apply the ASinH stretch to the image and set the stretch values to zero
			//
			bitmap.asinhStretch(asinhBeta, asinhBP, asinhHWLuminance);
			deepStack.setDescription(tr("ASinH stretch: beta %L1, bp %L2, hw %3")
				.arg(asinhBeta, 0, 'f', 1).arg(asinhBP, 0, 'f', 4).arg(QVariant(asinhHWLuminance).toString()));

			zeroMtfControls();
			zeroAsinHControls();
			break;

		case ProcessingFunction::ColourBalance:
			//
			// Adjust the colour balance of the image and
			// reset the colour balance shifts to zero
			//
			bitmap.adjustColourBalance(redShift, greenShift, blueShift);
			deepStack.setDescription(tr("Colour Balance: R %L1, G %L2, B %L3")
				.arg(redShift, 0, 'f', 2).arg(greenShift, 0, 'f', 2).arg(blueShift, 0, 'f', 2));

			zeroColourBalanceControls();
			break;

		default:
			ZASSERT(false);	// Invalid processing function
			break;
		}

		controls->undoButton->setToolTip(tr("Undo %1").arg(deepStack.description()));
		
		//
		// Now de-normalise the image back to the original range
		//
		bitmap.deNormalise();

		updateControls();

		processAndShow();
		showHistogram();

		setDirty(true);
	}

	void ProcessingDlg::onUndo()
	{
		controls->redoButton->setToolTip(tr("Redo %1").arg(undoRedoStack.current().description()));

		undoRedoStack.moveBackward();

		if (undoRedoStack.backwardAvailable())
		{
			//
			// Set all the stretch and colour balance adjustments to zero and set the controls to match
			// 
			zeroAdjustmentControls();

			controls->undoButton->setToolTip(tr("Undo %1").arg(undoRedoStack.current().description()));
		}
		else
		{
			//
			// Restore the processing settings for the image to the original values and update the controls to match.
			// 
			setAdjustmentControlDefaults();

			controls->undoButton->setToolTip("");
		}

		updateControls();

		if (undoRedoStack.index() == 0 && preview)
		{
			emit onPreview(ProcessingFunction::AsinhStretch);
			return;
		}

		processAndShow();
		showHistogram();
	}

	void ProcessingDlg::onRedo()
	{
		undoRedoStack.moveForward();

		controls->undoButton->setToolTip(tr("Undo %1").arg(undoRedoStack.current().description()));
		if (undoRedoStack.forwardAvailable())
		{
			auto index = undoRedoStack.index(); index++;
			controls->redoButton->setToolTip(tr("Redo %1").arg(undoRedoStack.at(index).description()));
		}
		else
		{
			controls->redoButton->setToolTip("");
		}

		//
		// Set all the stretch and colour balance adjustments to zero and set the controls to match
		// 
		zeroAdjustmentControls();

		updateControls();

		processAndShow();
		showHistogram();
	}

	void ProcessingDlg::onReset()
	{
		//
		// Reset the undo-redo stack to the original image, which is the first entry in the stack
		//
		undoRedoStack.reset();

		//
		// Restore the processing settings for the image to the original values and update the controls to match.
		// 
		setAdjustmentControlDefaults();

		updateControls();
		
		//
		// Do this before preview stuff otherwise the dirty flag doesn't get reset if preview is true
		// 
		setDirty(false);

		if (preview)
		{
			emit onPreview(ProcessingFunction::AsinhStretch);
			return;
		}

		processAndShow();
		showHistogram();
	}

	//
	// Triggered when the user changes the state of the "Show Clipping" checkbox, which
	// controls whether to show clipping of shadow and highlight pixels in the image
	//
	void ProcessingDlg::clippingStateChanged(int state)
	{
		Qt::CheckState checked{ static_cast<Qt::CheckState>(state) };
		QSettings{}.setValue("ShowBlackWhiteClipping", (Qt::Checked == checked));
		if (preview)
		{
			if (controls->tabWidget->currentWidget() == controls->mtfStretchTab)
			{
				// Apply the stretch asynchronously to the preview image.
				emit onPreview(ProcessingFunction::MtfStretch);
			}
			if (controls->tabWidget->currentWidget() == controls->asinhStretchTab)
			{
				// Apply the stretch asynchronously to the preview image.
				emit onPreview(ProcessingFunction::AsinhStretch);
			}
			if (controls->tabWidget->currentWidget() == controls->colourBalanceTab)
			{
				// Apply the adjustment asynchronously to the preview image.
				emit onPreview(ProcessingFunction::ColourBalance);
			}
		}
	}

	void ProcessingDlg::onLinkToggled()
	{
		syncMtfSpinBoxesFromModel();

		if (!imageLoaded || undoRedoStack.empty())
			return;

		if (preview) emit onPreview(ProcessingFunction::MtfStretch);
	}

	void ProcessingDlg::onAutostretch()
	{
		if (!imageLoaded)
			return;

		//
		// Create a temporary copy and normalise it to properly calculate the MTF parameters
		//
		DeepStack tempStack = undoRedoStack.current();
		StackedBitmap& tempBitmap = tempStack.GetStackedBitmap();
		tempBitmap.normalise();
		tempBitmap.mtfComputeAutoStretchParameters(controls->mtfLinkButton->isChecked(), mtfParameters, mtfTargetBkg, mtfShadowClip);

		controls->mtfRedGradient->setValues(mtfParameters.clipPoint[0], mtfParameters.midtoneBalance[0], mtfParameters.whitePoint[0]);
		controls->mtfGreenGradient->setValues(mtfParameters.clipPoint[1], mtfParameters.midtoneBalance[1], mtfParameters.whitePoint[1]);
		controls->mtfBlueGradient->setValues(mtfParameters.clipPoint[2], mtfParameters.midtoneBalance[2], mtfParameters.whitePoint[2]);

		syncMtfSpinBoxesFromModel();

		if (preview) emit onPreview(ProcessingFunction::MtfStretch);
		else controls->mtfApply->setEnabled(true);
	}

	void ProcessingDlg::setSelectionRect(const QRectF& rect)
	{
		selectionRect = DSSRect(static_cast<int>(rect.x()), static_cast<int>(rect.y()),
			static_cast<int>(rect.right()), static_cast<int>(rect.bottom()));
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::updatePixelInfo(QPoint pos, QRgb colour)
	{
		if (pos.x() >= 0 && pos.y() >= 0)
		{
			//
			// Use "deepskyblue" (rbg(0, 191, 255)) for the blue text as pure blue
			// is hard to read on a black background	
			// 
			pixelInfo->setText(QString("X: %1 Y: %2<br>"
				"<font color=#ff0000>R: %3 </font>"
				"<font color=#00ff00>G: %4 </font>"
				"<font color=#00bfff>B: %5 </font>")
				.arg(pos.x())
				.arg(pos.y())
				.arg(qRed(colour))
				.arg(qGreen(colour))
				.arg(qBlue(colour)));
		}
		else
		{
			pixelInfo->setText("");
		}
	}

	void ProcessingDlg::mtfRedGradientSliderMoved()
	{
		activeMtfChannel = 0;
		mtfParameters.clipPoint[0] = controls->mtfRedGradient->shadows();
		mtfParameters.midtoneBalance[0] = controls->mtfRedGradient->midtones();
		mtfParameters.whitePoint[0] = controls->mtfRedGradient->highlights();

		if (controls->mtfLinkButton->isChecked())
		{
			controls->mtfGreenGradient->setValues(mtfParameters.clipPoint[0], mtfParameters.midtoneBalance[0], mtfParameters.whitePoint[0]);
			controls->mtfBlueGradient->setValues(mtfParameters.clipPoint[0], mtfParameters.midtoneBalance[0], mtfParameters.whitePoint[0]);

			mtfParameters.clipPoint[1] = mtfParameters.clipPoint[0]; mtfParameters.midtoneBalance[1] = mtfParameters.midtoneBalance[0]; mtfParameters.whitePoint[1] = mtfParameters.whitePoint[0];
			mtfParameters.clipPoint[2] = mtfParameters.clipPoint[0]; mtfParameters.midtoneBalance[2] = mtfParameters.midtoneBalance[0]; mtfParameters.whitePoint[2] = mtfParameters.whitePoint[0]	;
		}
		syncMtfSpinBoxesFromModel();
		mtfSliderTimer.start(mtfPreviewDelay);
	}
	
	void ProcessingDlg::mtfGreenGradientSliderMoved()
	{
		activeMtfChannel = 1;
		mtfParameters.clipPoint[1] = controls->mtfGreenGradient->shadows();
		mtfParameters.midtoneBalance[1] = controls->mtfGreenGradient->midtones();
		mtfParameters.whitePoint[1] = controls->mtfGreenGradient->highlights();

		if (controls->mtfLinkButton->isChecked())
		{
			controls->mtfRedGradient->setValues(mtfParameters.clipPoint[1], mtfParameters.midtoneBalance[1], mtfParameters.whitePoint[1]);
			controls->mtfBlueGradient->setValues(mtfParameters.clipPoint[1], mtfParameters.midtoneBalance[1], mtfParameters.whitePoint[1]);

			mtfParameters.clipPoint[0] = mtfParameters.clipPoint[1]; mtfParameters.midtoneBalance[0] = mtfParameters.midtoneBalance[1]; mtfParameters.whitePoint[0] = mtfParameters.whitePoint[1];
			mtfParameters.clipPoint[2] = mtfParameters.clipPoint[1]; mtfParameters.midtoneBalance[2] = mtfParameters.midtoneBalance[1]; mtfParameters.whitePoint[2] = mtfParameters.whitePoint[1];
		}
		syncMtfSpinBoxesFromModel();
		mtfSliderTimer.start(mtfPreviewDelay);
	}

	void ProcessingDlg::mtfBlueGradientSliderMoved()
	{
		activeMtfChannel = 2;
		mtfParameters.clipPoint[2] = controls->mtfBlueGradient->shadows();
		mtfParameters.midtoneBalance[2] = controls->mtfBlueGradient->midtones();
		mtfParameters.whitePoint[2] = controls->mtfBlueGradient->highlights();

		if (controls->mtfLinkButton->isChecked())
		{
			controls->mtfRedGradient->setValues(mtfParameters.clipPoint[2], mtfParameters.midtoneBalance[2], mtfParameters.whitePoint[2]);
			controls->mtfGreenGradient->setValues(mtfParameters.clipPoint[2], mtfParameters.midtoneBalance[2], mtfParameters.whitePoint[2]);

			mtfParameters.clipPoint[0] = mtfParameters.clipPoint[2]; mtfParameters.midtoneBalance[0] = mtfParameters.midtoneBalance[2]; mtfParameters.whitePoint[0] = mtfParameters.whitePoint[2];
			mtfParameters.clipPoint[1] = mtfParameters.clipPoint[2]; mtfParameters.midtoneBalance[1] = mtfParameters.midtoneBalance[2]; mtfParameters.whitePoint[1] = mtfParameters.whitePoint[2];
		}
		syncMtfSpinBoxesFromModel();
		mtfSliderTimer.start(mtfPreviewDelay);
	}

	void ProcessingDlg::syncMtfSpinBoxesFromModel()
	{
		const QSignalBlocker shadowsBlocker(controls->mtfShadowsSpinBox);
		const QSignalBlocker midtonesBlocker(controls->mtfMidtonesSpinBox);
		const QSignalBlocker highlightsBlocker(controls->mtfHighlightsSpinBox);

		float s, m, h;
		getCurrentChannelValues(s, m, h);

		QColor labelColour{ Qt::black };
		if (!controls->mtfLinkButton->isChecked() && !mtfMonochrome)
		{
			switch (activeMtfChannel)
			{
			case 0: labelColour = Qt::red; break;
			case 1: labelColour = Qt::darkGreen; break;
			case 2: labelColour = Qt::blue; break;
			default: break;
			}
		}
		const QString style{ QString("color: %1;").arg(labelColour.name()) };
		controls->mtfShadowsSpinLabel->setStyleSheet(style);
		controls->mtfMidtonesSpinLabel->setStyleSheet(style);
		controls->mtfHighlightsSpinLabel->setStyleSheet(style);

		controls->mtfShadowsSpinBox->setValue(s);
		controls->mtfMidtonesSpinBox->setValue(m);
		controls->mtfHighlightsSpinBox->setValue(h);
	}

	void ProcessingDlg::applyMtfSpinValues(float shadows, float midtones, float highlights, bool forceAllChannels)
	{
		if (midtones > highlights)
			midtones = highlights;

		auto setChannel = [&](int ch, float s, float m, float h)
			{
			switch (ch)
			{
			case 0:
				mtfParameters.clipPoint[0] = s;
				mtfParameters.midtoneBalance[0] = m;
				mtfParameters.whitePoint[0] = h;
				controls->mtfRedGradient->setValues(s, m, h);
				break;
			case 1:
				mtfParameters.clipPoint[1] = s;
				mtfParameters.midtoneBalance[1] = m;
				mtfParameters.whitePoint[1] = h;
				controls->mtfGreenGradient->setValues(s, m, h);
				break;
			case 2:
				mtfParameters.clipPoint[2] = s;
				mtfParameters.midtoneBalance[2] = m;
				mtfParameters.whitePoint[2] = h;
				controls->mtfBlueGradient->setValues(s, m, h);
				break;
			default: break;
			}
		};

		if (forceAllChannels || controls->mtfLinkButton->isChecked())
		{
			setChannel(0, shadows, midtones, highlights);
			setChannel(1, shadows, midtones, highlights);
			setChannel(2, shadows, midtones, highlights);
		}
		else
		{
			setChannel(activeMtfChannel, shadows, midtones, highlights);
		}

		syncMtfSpinBoxesFromModel();
		mtfSliderTimer.start(mtfPreviewDelay);
	}

	/* ------------------------------------------------------------------- */

	//
	// Helper method to get current channel values based on activeMtfChannel.
	// Reduces code duplication across spinbox change handlers.
	//
	void ProcessingDlg::getCurrentChannelValues(float& shadows, float& midtones, float& highlights) const
	{
		shadows = mtfParameters.clipPoint[activeMtfChannel];
		midtones = mtfParameters.midtoneBalance[activeMtfChannel];
		highlights = mtfParameters.whitePoint[activeMtfChannel];
	}

	/* ------------------------------------------------------------------- */

	void ProcessingDlg::mtfShadowsSpinBoxChanged(double value)
	{
		float s, m, h;
		getCurrentChannelValues(s, m, h);
		applyMtfSpinValues(static_cast<float>(value), m, h, false);
	}

	void ProcessingDlg::mtfMidtonesSpinBoxChanged(double value)
	{
		float s, m, h;
		getCurrentChannelValues(s, m, h);
		applyMtfSpinValues(s, static_cast<float>(value), h, false);
	}

	void ProcessingDlg::mtfHighlightsSpinBoxChanged(double value)
	{
		float s, m, h;
		getCurrentChannelValues(s, m, h);
		applyMtfSpinValues(s, m, static_cast<float>(value), false);
	}

	void ProcessingDlg::onAutostretchContextMenu(const QPoint& pos)
	{
		QMenu menu(this);
		QAction* settingsAction = menu.addAction(tr("Edit autostretch parameters ..."));
		if (menu.exec(controls->mtfAutostretchButton->mapToGlobal(pos)) == settingsAction)
		{
			onMtfAutostretchSettings();
		}
	}


	void ProcessingDlg::onMtfAutostretchSettings()
	{
		MtfAutostretchSettings dlg(mtfTargetBkg, mtfShadowClip, this);
		if (dlg.exec() == QDialog::Accepted)
		{
			mtfTargetBkg = dlg.targetBkg();
			mtfShadowClip = dlg.shadowClip();
		}
	}

	void ProcessingDlg::mtfApplyPressed()
	{
		mtfParameters.clipPoint[0] = controls->mtfRedGradient->shadows();
		mtfParameters.midtoneBalance[0] = controls->mtfRedGradient->midtones();
		mtfParameters.whitePoint[0] = controls->mtfRedGradient->highlights();
		mtfParameters.clipPoint[1] = controls->mtfGreenGradient->shadows();
		mtfParameters.midtoneBalance[1] = controls->mtfGreenGradient->midtones();
		mtfParameters.whitePoint[1] = controls->mtfGreenGradient->highlights();
		mtfParameters.clipPoint[2] = controls->mtfBlueGradient->shadows();
		mtfParameters.midtoneBalance[2] = controls->mtfBlueGradient->midtones();
		mtfParameters.whitePoint[2] = controls->mtfBlueGradient->highlights();

		onApply(ProcessingFunction::MtfStretch);
		zeroMtfControls();
	}
} // namespace DSS
