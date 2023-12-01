#pragma once

#include <QDialog>
#include "dssrect.h"
#include "Histogram.h"
#include "ProcessingSettings.h"
#include "ui_ProcessingDlg.h"

namespace DSS
{
	class SelectRect;

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

		bool operator < (const ValuedRect& rhs)
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


	class ProcessingDlg : public QDialog, public Ui::ProcessingDlg
	{
		Q_OBJECT

	public:
		ProcessingDlg(QWidget *parent = nullptr);
		~ProcessingDlg();

		inline bool dirty() const { return dirty_; };
		inline void setDirty(bool v = true) { dirty_ = v; };

		void copyToClipboard();
		void createStarMask();
		void loadFile(const fs::path& file);
		void loadImage();
		void saveImage();

		bool saveOnClose();

		HistogramAdjustmentCurve redAdjustmentCurve() const { return redAdjustmentCurve_; }
		HistogramAdjustmentCurve greenAdjustmentCurve() const { return greenAdjustmentCurve_; }
		HistogramAdjustmentCurve blueAdjustmentCurve() const { return blueAdjustmentCurve_; }

	private:
		ProcessingSettings	processParms;
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
		inline static const QStringList iconNames{ "linear", "cuberoot", "sqrt", "log", "loglog", "logsqrt", "asinh" };
		double gradientOffset;
		double gradientRange;

		SelectRect* selectRect;
		DSSRect	selectionRect;


		HistogramAdjustmentCurve redAdjustmentCurve_;
		HistogramAdjustmentCurve greenAdjustmentCurve_;
		HistogramAdjustmentCurve blueAdjustmentCurve_;

		void connectSignalsToSlots();
		void setButtonIcons();
		void setRedButtonIcon();
		void setGreenButtonIcon();
		void setBlueButtonIcon();

		void modifyRGBKGradientControls();

		void	updateControlsFromParams();

		void updateInformation();

	signals:
		void showOriginalHistogram();

	public slots:
		void setSelectionRect(const QRectF& rect);

	private slots:
		void redChanging(int peg);
		void redChanged(int peg);

		void greenChanging(int peg);
		void greenChanged(int peg);

		void blueChanging(int peg);
		void blueChanged(int peg);

		inline void setRedAdjustmentCurve(HistogramAdjustmentCurve hac) { redAdjustmentCurve_ = hac; };
		inline void setGreenAdjustmentCurve(HistogramAdjustmentCurve hac) { greenAdjustmentCurve_ = hac; };
		inline void setBlueAdjustmentCurve(HistogramAdjustmentCurve hac) { blueAdjustmentCurve_ = hac; };

		void onColorSchemeChanged(Qt::ColorScheme colorScheme);

		void redButtonPressed();
		void greenButtonPressed();
		void blueButtonPressed();

#if (0)
		void	ProcessAndShow(bool bSaveUndo = true);

		void	ResetSliders();
		void	DrawHistoBar(Graphics* pGraphics, int lNrReds, int lNrGreens, int lNrBlues, int X, int lHeight);
		void	ShowHistogram(CWndImage& wndImage, RGBHistogram& Histogram, bool bLog = false);
		void	ShowOriginalHistogram(bool bLog = false);

		void	DrawGaussCurves(Graphics* pGraphics, RGBHistogram& Histogram, int lWidth, int lHeight);

		void	UpdateHistogramAdjust();
		void	DrawBezierCurve(Graphics* pGraphics, int lWidth, int lHeight);

		void	UpdateControls();
		void	UpdateMonochromeControls();


		// Implementation
	public:
		void	UpdateBezierCurve();
		void	OnUndo();
		void	OnRedo();
		void	OnSettings();

		void	CopyPictureToClipboard();
		bool	SavePictureToFile();
		void	CreateStarMask();

		void	LoadFile(LPCTSTR szFileName);


#endif
	 
	};
} // namespace DSS
