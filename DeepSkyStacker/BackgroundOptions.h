#pragma once

enum BACKGROUNDCALIBRATIONMODE : short;
enum BACKGROUNDCALIBRATIONINTERPOLATION : short;
enum RGBBACKGROUNDCALIBRATIONMETHOD : short;

namespace DSS
{
	namespace Ui {
		class BackgroundOptions;
	}

	class BackgroundOptions : public QDialog
	{
		Q_OBJECT

			typedef QDialog
			Inherited;

	public:
		explicit BackgroundOptions(QWidget* parent = 0);
		~BackgroundOptions();

		void	SetBackgroundCalibrationMode(BACKGROUNDCALIBRATIONMODE Mode)
		{
			m_CalibrationMode = Mode;
		};

		BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode()
		{
			return m_CalibrationMode;
		};

	private slots:
		void on_rbLinear_clicked();
		void on_rbRational_clicked();
		void on_rbNone_clicked();
		void on_rbMinimum_clicked();
		void on_rbMiddle_clicked();
		void on_rbMaximum_clicked();

		void accept() override;
		void reject() override;

	private:
		Ui::BackgroundOptions* ui;
		BACKGROUNDCALIBRATIONMODE m_CalibrationMode;
		BACKGROUNDCALIBRATIONINTERPOLATION m_CalibrationInterpolation;
		RGBBACKGROUNDCALIBRATIONMETHOD m_RGBCalibrationMethod;
		QPixmap pxNone;
		QPixmap pxMin;
		QPixmap pxMid;
		QPixmap pxMax;
		QPixmap pxLinear;
		QPixmap pxRational;

		void updateInterpolation(BACKGROUNDCALIBRATIONINTERPOLATION interpolation);
	};
}