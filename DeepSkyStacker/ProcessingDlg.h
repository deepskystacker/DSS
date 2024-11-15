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
#include "ProcessingSettings.h"
#include "processingcontrols.h"
#include "ui_ProcessingDlg.h"

namespace DSS
{
	class SelectRect;
	class ProcessingControls;
	class ProcessingSettingsDlg;

	class ValuedRect final
	{
	public:
		DSSRect					m_rc;
		double					m_fScore;

	public:
		ValuedRect()
		{
			m_fScore = 0.0;
		};

		~ValuedRect() {};

		ValuedRect(const ValuedRect& vr) = default;

		ValuedRect& operator = (const ValuedRect& vr) = default;

		bool operator < (const ValuedRect& rhs) const
		{
			return m_fScore < rhs.m_fScore;
		};
	};

	class ProcessRect final
	{
	private:
		int						m_lWidth;
		int						m_lHeight;
		int						m_lSize;
		std::vector<ValuedRect>	m_vRects;
		std::vector<bool>			m_vProcessed;
		bool						m_bToProcess;
		DSSRect						m_rcToProcess;

	private:
		bool IsProcessRectOk()
		{
			bool			bResult = false;

			bResult = (m_rcToProcess.left >= 0) && (m_rcToProcess.left < m_lWidth) &&
				(m_rcToProcess.right >= 0) && (m_rcToProcess.right < m_lWidth) &&
				(m_rcToProcess.top >= 0) && (m_rcToProcess.top < m_lHeight) &&
				(m_rcToProcess.bottom >= 0) && (m_rcToProcess.bottom < m_lHeight) &&
				(m_rcToProcess.left < m_rcToProcess.right) &&
				(m_rcToProcess.top < m_rcToProcess.bottom);

			return bResult;
		};

	public:
		ProcessRect()
		{
			m_bToProcess = false;
			m_rcToProcess.left = 0;
			m_rcToProcess.top = 0;
			m_rcToProcess.right = 0;
			m_rcToProcess.bottom = 0;
			m_rcToProcess.setEmpty();
			m_lWidth = 0;
			m_lHeight = 0;
			m_lSize = 0;
		};
		virtual ~ProcessRect() {};

		ProcessRect(const ValuedRect& vr) = delete;

		ProcessRect& operator = (const ProcessRect& vr) = delete;


		void	Init(int lWidth, int lHeight, int lRectSize)
		{
			int			i, j;

			m_lWidth = lWidth;
			m_lHeight = lHeight;
			m_lSize = lRectSize;

			m_vRects.clear();
			m_vProcessed.clear();
			for (i = 0; i < m_lWidth; i += lRectSize)
			{
				for (j = 0; j < m_lHeight; j += lRectSize)
				{
					ValuedRect	rcCell;

					rcCell.m_rc.left = i;
					rcCell.m_rc.right = min(i + m_lSize, m_lWidth);
					rcCell.m_rc.top = j;
					rcCell.m_rc.bottom = min(j + m_lSize, m_lHeight);

					rcCell.m_fScore = fabs(((i + m_lSize / 2.0) - m_lWidth / 2.0) / (double)m_lWidth) + fabs(((j + m_lSize / 2) - m_lHeight / 2.0) / (double)m_lHeight);

					m_vRects.push_back(rcCell);
					m_vProcessed.push_back(false);
				};
			};
			std::sort(m_vRects.begin(), m_vRects.end());
		};

		void	SetProcessRect(const DSSRect& rc)
		{
			m_rcToProcess = rc;
		};

		void	Reset()
		{
			for (int i = 0; i < m_vProcessed.size(); i++)
				m_vProcessed[i] = false;
			m_bToProcess = true;
		};

		bool	GetNextUnProcessedRect(DSSRect& rcCell)
		{
			bool		bResult = false;

			if (m_bToProcess)
			{
				if (!m_rcToProcess.isEmpty() && IsProcessRectOk())
				{
					if (!m_vProcessed[0])
					{
						rcCell = m_rcToProcess;
						m_vProcessed[0] = true;
						bResult = true;
					};
				}
				else
				{
					for (int i = 0; i < m_vProcessed.size() && !bResult; i++)
					{
						if (!m_vProcessed[i])
						{
							bResult = true;
							rcCell = m_vRects[i].m_rc;
							m_vProcessed[i] = true;
						};
					};
				};

				m_bToProcess = bResult;
			};

			return bResult;
		};

		float GetPercentageComplete() const
		{
			if (m_vProcessed.size() == 0)
				return 100.0f;

			float fPercentage = 0.0f;
			const float fDelta = 100.0f / static_cast<float>(m_vProcessed.size());

			// The iteration loop here could be corrupted by a call to Init() on a different thread.
			// To make totally thread safe this should really have a mutex lock associated with it.
			for (bool bState : m_vProcessed)
				fPercentage += bState ? fDelta : 0.0f;

			return fPercentage;
		}
	};

	typedef std::list<ProcessingSettings>		PROCESSINGSETTINGSLIST;
	typedef PROCESSINGSETTINGSLIST::iterator	PROCESSINGSETTINGSITERATOR;

	class ProcessingSettingsList
	{
	public:
		PROCESSINGSETTINGSLIST		m_lParams;
		int					m_lCurrent;

	public:
		ProcessingSettingsList()
		{
			m_lCurrent = -1;
		};
		virtual ~ProcessingSettingsList()
		{
		};

		int size()
		{
			return (int)m_lParams.size();
		};

		int current()
		{
			return m_lCurrent;
		};

		void clear()
		{
			m_lParams.clear();
			m_lCurrent = -1;
		};

		bool	MoveForward()
		{
			bool			bResult = false;

			if (m_lCurrent + 1 < size())
			{
				m_lCurrent++;
				bResult = true;
			};
			return bResult;
		};
		bool	MoveBackward()
		{
			bool			bResult = false;

			if ((m_lCurrent - 1 >= 0) && (size() > 0))
			{
				m_lCurrent--;
				bResult = true;
			};
			return bResult;
		};

		bool IsBackwardAvailable()
		{
			return (m_lCurrent - 1 >= 0);
		};

		bool IsForwardAvailable()
		{
			return (m_lCurrent + 1 < size());
		};

		bool	GetCurrentSettings(ProcessingSettings& pp)
		{
			return GetSettings(m_lCurrent, pp);
		};

		bool	GetSettings(int lIndice, ProcessingSettings& pp)
		{
			bool					bResult = false;
			PROCESSINGSETTINGSITERATOR    it;
			//bool					bFound = false;

			if (!(lIndice >= 0) && (lIndice < size()))
				return false;

			for (it = m_lParams.begin(); it != m_lParams.end() && lIndice > 0; it++, lIndice--);
			if (it != m_lParams.end())
			{
				pp = (*it);
				bResult = true;
			};

			return bResult;
		};

		bool	AddParams(const ProcessingSettings& pp)
		{
			bool						bResult = false;

			if ((m_lCurrent >= 0) && (m_lCurrent < size() - 1))
			{
				PROCESSINGSETTINGSITERATOR	it;
				int					lIndice = m_lCurrent + 1;

				for (it = m_lParams.begin(); it != m_lParams.end() && lIndice > 0; it++, lIndice--);

				m_lParams.erase(it, m_lParams.end());
			}
			else if (m_lCurrent == -1)
				m_lParams.clear();

			m_lParams.push_back(pp);

			m_lCurrent = size() - 1;

			bResult = true;

			return bResult;
		};

	};



	class ProcessingDlg : public QWidget, public Ui::ProcessingDlg
	{
		Q_OBJECT

	public:
		ProcessingDlg(QWidget *parent, ProcessingControls* processingControls);
		~ProcessingDlg();

		inline bool dirty() const { return dirty_; };
		inline void setDirty(bool v = true) { dirty_ = v; };

		void copyToClipboard();
		void createStarMask();
		void loadStackedImage(const fs::path& file);
		void loadImage();
		bool saveImage();

		bool saveOnClose();

		HistogramAdjustmentCurve redAdjustmentCurve() const { return redAdjustmentCurve_; }
		HistogramAdjustmentCurve greenAdjustmentCurve() const { return greenAdjustmentCurve_; }
		HistogramAdjustmentCurve blueAdjustmentCurve() const { return blueAdjustmentCurve_; }
		
	private:
		ProcessingControls* controls;
		ProcessingSettings	processingSettings;
		ProcessingSettingsList processingSettingsList;
		ProcessRect		rectToProcess;
		bool dirty_;
		fs::path currentFile;
		QString iconModifier;
		QMenu hacMenu;		// Menu to display when the adjustment curve button is pressed
		QAction* linearAction;
		QAction* cubeRootAction;
		QAction* squareRootAction;
		QAction* logAction;
		QAction* logLogAction;
		QAction* logSquareRootAction;
		QAction* asinHAction;
		QTimer	timer;
		inline static const QStringList iconNames{ "linear", "cuberoot", "sqrt", "log", "loglog", "logsqrt", "asinh" };
		double gradientOffset_;
		double gradientRange_;

		SelectRect* selectRect;
		DSSRect	selectionRect;


		HistogramAdjustmentCurve redAdjustmentCurve_;
		HistogramAdjustmentCurve greenAdjustmentCurve_;
		HistogramAdjustmentCurve blueAdjustmentCurve_;

		void initialiseSliders();
		void connectSignalsToSlots();
		void setButtonIcons();
		void setRedButtonIcon();
		void setGreenButtonIcon();
		void setBlueButtonIcon();

		void modifyRGBKGradientControls();

		void	updateControlsFromSettings();

		void updateControls();
		void updateInformation();

		void processAndShow(bool bSaveUndo = true);		// Driven by Apply button

		inline void updateDarkText()
		{
			//
			// Set the descriptive text for the two sliders (\xc2\xb0 is UTF-8 degree sign)
			//
			controls->darkLabel->setText(QString(" %1 \xc2\xb0\n %2")
				.arg(controls->darkAngle->sliderPosition()).arg(controls->darkPower->value() / 10.0, 0, 'f', 1));

		}

		inline void updateMidText()
		{
			//
			// Set the descriptive text for the two sliders (\xc2\xb0 is UTF-8 degree sign)
			//
			controls->midLabel->setText(QString(" %1 \xc2\xb0\n %2")
				.arg(controls->midAngle->sliderPosition()).arg(controls->midTone->value() / 10.0, 0, 'f', 1));

		}

		inline void updateHighText()
		{
			//
			// Set the descriptive text for the two sliders (\xc2\xb0 is UTF-8 degree sign)
			//
			controls->highLabel->setText(QString(" %1 \xc2\xb0\n %2")
				.arg(controls->highAngle->sliderPosition()).arg(controls->highPower->value() / 10.0, 0, 'f', 1));

		}

		inline void updateSaturationText()
		{
			controls->saturationLabel->setText(QString("%1 %").arg(controls->saturation->value()));
		}

		//
		// Initial settings for the Luminance tab sliders
		//
		static const inline unsigned int maxAngle{ 45 };
		static const inline unsigned int maxLuminance { 1000 };

		static const inline unsigned int darkAngleInitialValue{ 0 };
		static const inline unsigned int darkPowerInitialValue{ 500 };
		static const inline unsigned int midAngleInitialValue{ 20 };
		static const inline unsigned int midToneInitialValue{ 200 };
		static const inline unsigned int highAngleInitialPostion{ 0 };
		static const inline unsigned int highPowerInitialValue{ 500 };

		//
		// Initial values for the Saturation slider
		//
		static const inline int minSaturation { -50 };
		static const inline int maxSaturation { 50 };
		static const inline int initialSaturation { 20 };

		void	drawHistogram(RGBHistogram& Histogram, bool useLogarithm);
		void	drawHistoBar(QPainter& painter, int lNrReds, int lNrGreens, int lNrBlues, int X, int lHeight);
		void	drawGaussianCurves(QPainter& painter, RGBHistogram& Histogram, int lWidth, int lHeight);
		void	drawBezierCurve(QPainter& painter, int lWidth, int lHeight);

		void showHistogram(bool useLogarithm = false);	// Calls drawHistogram 

		void resetSliders();

		void	UpdateHistogramAdjust();

		bool askToSave();

		inline void setRedAdjustmentCurve(HistogramAdjustmentCurve hac)
		{
			redAdjustmentCurve_ = hac;
			setRedButtonIcon();
		};
		inline void setGreenAdjustmentCurve(HistogramAdjustmentCurve hac)
		{
			greenAdjustmentCurve_ = hac;
			setGreenButtonIcon();
		};
		inline void setBlueAdjustmentCurve(HistogramAdjustmentCurve hac)
		{
			blueAdjustmentCurve_ = hac;
			setBlueButtonIcon();
		};

	public slots:
		void setSelectionRect(const QRectF& rect);

	private slots:

		void onApply();
		void onUndo();
		void onRedo();
		void onReset();
		void onSettings();

		void redChanging(int peg);
		void redChanged(int peg);

		void greenChanging(int peg);
		void greenChanged(int peg);

		void blueChanging(int peg);
		void blueChanged(int peg);

		void onColorSchemeChanged(Qt::ColorScheme colorScheme);
		void onTimer();

		void redButtonPressed();
		void greenButtonPressed();
		void blueButtonPressed();

		void darkAngleChanged();
		void darkPowerChanged();

		void midAngleChanged();
		void midToneChanged();

		void highAngleChanged();
		void highPowerChanged();

		void updateBezierCurve();

		void saturationChanged();


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
