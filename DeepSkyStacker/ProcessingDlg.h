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

#include <QDialog>
#include "dssrect.h"
#include "histogram.h"
#include "processingcontrols.h"
#include "undoredostack.h"
#include "ui_ProcessingDlg.h"

namespace DSS
{
	class SelectRect;
	class ProcessingControls;

	class ProcessingDlg final: public QWidget, public Ui::ProcessingDlg
	{
		Q_OBJECT

	public:
		ProcessingDlg(QWidget *parent, ProcessingControls* processingControls);
		~ProcessingDlg() override;
		ProcessingDlg(const ProcessingDlg&) = delete;
		ProcessingDlg& operator=(const ProcessingDlg&) = delete;
		ProcessingDlg(ProcessingDlg&&) = delete;


		inline bool dirty() const { return dirty_; }
		inline void setDirty(bool v = true) { dirty_ = v; }

		void copyToClipboard();
		void createStarMask();
		void loadStackedImage(const fs::path& file);
		void loadImage();
		bool saveImage();

		bool saveOnClose();

	private:
		ProcessingControls* controls;
		bool dirty_;
		fs::path currentFile;
		DSS::UndoRedoStack& undoRedoStack;

		SelectRect* selectRect;
		DSSRect	selectionRect;

		enum class ProcessingFunction
		{
			AsinhStretch,
			ColourBalance,
			AutoStretch
		};

		//
		// Set the default image adjustment parameters and the default state of the preview checkbox
		//
		void setAdjustmentControlDefaults();

		//
		// Set the image adjustment parameters to zero values
		//
		void zeroAsinHControls();
		void zeroColourBalanceControls();
		inline void zeroAdjustmentControls()
		{
			zeroAsinHControls();
			zeroColourBalanceControls();
		}

		void connectSignalsToSlots();

		void updateInformation();

		void processAndShow();		// Driven by Apply button

		//
		// Initial values for the Image adjustment parameters
		//
		static constexpr float DefaultAsinhBeta{ 100.0f };
		static constexpr float DefaultAsinhBP{ 0.001f };

		bool useLogarithmicHistogram{ false };	// Whether to use a logarithmic scale for the histogram display

		void	drawHistogram(RGBHistogram& Histogram);
		void	drawHistoBar(QPainter& painter, double lNrReds, double lNrGreens, double lNrBlues, size_t X, int lHeight);
		void	drawGaussianCurves(QPainter& painter, RGBHistogram& Histogram, int lWidth, int lHeight);

		void showHistogram();	// Calls drawHistogram 

		bool askToSave();

		bool imageLoaded{ false };	// Whether an image is loaded and can be processed	

		//
		// Image adjustment parameters, which are used for the preview image and histogram when the preview
		// checkbox is checked, and are applied to the current DeepStack object when the user clicks the
		// Apply button.
		//
		float asinhBeta{ DefaultAsinhBeta };	// Asinh stretch value
		float asinhBP{ DefaultAsinhBP };		// Asinh black point value
		bool asinhHWLuminance{ true };	// Whether to use human weighted luminance for asinh stretch
		float redShift{ 0.0f };		// Red channel shift value
		float greenShift{ 0.0f };	// Green channel shift value
		float blueShift{ 0.0f };	// Blue channel shift value

		bool preview{ true };		// Whether to show a preview of the processed image

		//
		// Flag to control which DeepStack object to use for processAndShow() and showHistogram().
		// If true, use the preview DeepStack object, which is created by the doPreview() method
		// and is processed with the current settings for the asinh stretch and black point.
		//
		// If false, use the current DeepStack object from the undo-redo stack, which is not modified
		// until the user clicks the Apply button.
		//
		bool usePreviewDeepStack{ false };
		QMutex previewMutex;	// Mutex to protect access to the preview code
		
		//
		// Timer to control handling of valueChanged signals from the asinh stretch and black point
		// sliders and spin boxes, to avoid excessive processing of the preview image when the user
		// is adjusting the sliders or spin boxes.
		// 
		// The timer is single shot and is restarted each time the valueChanged signal is emitted,
		// The timer is started or restarted in the valueChanged handlers for the sliders and spin boxes, and
		// the onPreview() slot is called when the timer times out, which applies the stretch to the preview image.
		//
		QTimer previewTimer;

		//
		// Timers to control the handling to valueChanged signals from the red, green and blue shift sliders
		// to avoid excessive processing of the preview image when the user is adjusting the sliders.
		//
		QTimer redSliderTimer;
		QTimer greenSliderTimer;
		QTimer blueSliderTimer;

		//
		// The DeepStack object used for the preview image and histogram, created by the doPreview() method
		//
		DeepStack previewDeepStack;	

		void doPreview(ProcessingFunction function);

	signals:
		void asinhBPChanged(double value);
		void asinhStretchChanged(double value);

	public slots:
		void setSelectionRect(const QRectF& rect);

	private slots:

		void updateControls();
		void onPreview(ProcessingFunction function);
		void onApply(ProcessingFunction function);
		void onUndo();
		void onRedo();
		void onReset();
		void onLinkToggled();
		void onAutostretch();

		//
		// Asinh stretch and black point sliders and spin boxes
		//
		// The multipliers and divisors in the slider handlers are to convert between the double values used in the spin boxes
		// and the integer values used in the sliders.
		// 
		// The asinhBP slider and spin box are designed to allow values between 0.00000 and 0.20000 with a resolution of 0.00001
		// hence the multiplier and divisor of 1000.0f.
		//	
		// The asinhStretch slider and spin box are designed to allow values between 0.0 and 1000.0f with a resolution of 0.1,
		// hence the multiplier and divisor of 10.0f.
		//
		void asinhBPChangedHandler(float value)
		{
			if (value != asinhBP)
			{
				asinhBP = value;
				const QSignalBlocker spinBoxBlocker(controls->asinhBPSpinBox);
				const QSignalBlocker sliderBlocker(controls->asinhBPSlider);
				controls->asinhBPSpinBox->setValue(value);
				controls->asinhBPSlider->setValue(static_cast<int>(value * 1000.0f));
				//
				// if preview is enabled, apply the stretch asynchronously to the preview image.
				// when the preview processing is complete, the apply button will be enabled
				//
				if (preview)
				{
					// Apply the stretch asynchronously to the preview image.
					emit onPreview(ProcessingFunction::AsinhStretch);
				}
				else controls->asinhApply->setEnabled(true);
			}
		}

		void setAsinhBP(double value)
		{
			if (value != asinhBP)
			{
				previewTimer.stop();	// Stop the timer
				//
				// Disconnect the timer so that any previous connections to the asinhBPChanged signal are removed
				//
				previewTimer.disconnect();

				//
				// Now set up so that when the timer expires it will emit the asinhBPChanged signal with the new
				// value which will call the asinhBPChangedHandler slot to update the preview image with the new
				// black point value.
				//
				previewTimer.callOnTimeout(this, [=, this]() {
					asinhBPChanged(value);
						});
				//
				// Stop and restart the timer to delay the preview processing until the user has finished
				// adjusting the slider or spin box.
				//
				previewTimer.start();
			}
		}

		void asinhBPSpinBoxChanged(double value)
		{
			setAsinhBP(value);
		}

		void asinhBPSliderChanged(int value)
		{
			setAsinhBP(static_cast<float>(value) / 1000.0f);
		}

		void asinhStretchChangedHandler(float value)
		{
			if (value != asinhBeta)
			{
				asinhBeta = value;
				const QSignalBlocker spinBoxBlocker(controls->asinhStretchSpinBox);
				const QSignalBlocker sliderBlocker(controls->asinhStretchSlider);
				controls->asinhStretchSpinBox->setValue(value);
				controls->asinhStretchSlider->setValue(static_cast<int>(value * 10.0f));
				//
				// if preview is enabled, apply the stretch asynchronously to the preview image.
				// when the preview processing is complete, the apply button will be enabled
				//
				if (preview)
				{
					// Apply the stretch asynchronously to the preview image.
					emit onPreview(ProcessingFunction::AsinhStretch);
				}
				else controls->asinhApply->setEnabled(true);
			}
		}

		void setAsinhStretch(double value)
		{
			if (value != asinhBeta)
			{
				previewTimer.stop();	// Stop the timer 
				//
				// Disconnect the timer so that any previous connections to the asinhStretchChanged signal are removed
				//
				previewTimer.disconnect();

				//
				// Now set up so that when the timer expires it will emit the asinhStretchChanged signal with the new
				// value which will call the asinhStretchChangedHandler slot to update the preview image with the new
				// stretch value.
				//
				previewTimer.callOnTimeout(this, [=, this]() {
					asinhStretchChanged(value);
					});
				//
				// Stop and restart the timer to delay the preview processing until the user has finished
				// adjusting the slider or spin box.
				//
				previewTimer.start();
			}
		}

		void asinhStretchSpinBoxChanged(double value)
		{
			setAsinhStretch(value);
		}

		void asinhStretchSliderChanged(int value)
		{
			setAsinhStretch(static_cast<float>(value) / 10.0f);
		}

		void asinhHumanWeightedChanged(Qt::CheckState state)
		{
			switch (state)
			{
			case Qt::Unchecked:
				asinhHWLuminance = false;
				break;
			default:
				asinhHWLuminance = true;
				break;
			}
			if (preview)
			{
				// Apply the stretch asynchronously to the preview image.
				emit onPreview(ProcessingFunction::AsinhStretch);
			}
			else controls->asinhApply->setEnabled(true);
		}

		void previewChanged(Qt::CheckState state)
		{
			switch (state)
			{
			case Qt::Unchecked:
				preview = false;
				break;
			default:
				preview = true;
				break;
			}
			if (preview)
			{
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

		void redSliderChanged(int value)
		{
			//
			// Convert the slider value, which is between 0 and 200, to a shift value between -0.4 and +0.4,
			// with a default of 0.0 at the middle of the slider (i.e. when the slider value is 100).
			//
			redShift = (static_cast<float>(value) / 200.0f - 0.5f) * 0.8f;
			if (preview)
			{
				// Apply the adjustment asynchronously to the preview image.
				emit onPreview(ProcessingFunction::ColourBalance);
			}
			else controls->cbApply->setEnabled(true);
		}

		void greenSliderChanged(int value)
		{
			//
			// Convert the slider value, which is between 0 and 200, to a shift value between -0.4 and +0.4,
			// with a default of 0.0 at the middle of the slider (i.e. when the slider value is 100).
			//
			greenShift = (static_cast<float>(value) / 200.0f - 0.5f) * 0.8f;
			if (preview)
			{
				// Apply the adjustment asynchronously to the preview image.
				emit onPreview(ProcessingFunction::ColourBalance);
			}
			else controls->cbApply->setEnabled(true);
		}

		void blueSliderChanged(int value)
		{
			//
			// Convert the slider value, which is between 0 and 200, to a shift value between -0.4 and +0.4,
			// with a default of 0.0 at the middle of the slider (i.e. when the slider value is 100).
			//
			blueShift = (static_cast<float>(value) / 200.0f - 0.5f) * 0.8f;
			if (preview)
			{
				// Apply the adjustment asynchronously to the preview image.
				emit onPreview(ProcessingFunction::ColourBalance);
			}
			else controls->cbApply->setEnabled(true);
		}

		void asinhApplyPressed()
		{
			controls->asinhApply->setEnabled(false);
			emit onApply(ProcessingFunction::AsinhStretch);
		}

		void cbApplyPressed()
		{
			controls->cbApply->setEnabled(false);
			emit onApply(ProcessingFunction::ColourBalance);
		}

		void updatePixelInfo(QPoint pos, QRgb colour);

#if (0)

		void	UpdateMonochromeControls();


		// Implementation
	public:

		void	CopyPictureToClipboard();
		bool	SavePictureToFile();
		void	CreateStarMask();

#endif
	 
	};
} // namespace DSS
