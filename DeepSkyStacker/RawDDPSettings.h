#pragma once
#include "dslr.h"

class Workspace;
class QValidator;
class QShowEvent;

namespace DSS
{
	namespace Ui {
		class RawDDPSettings;
	}

	class RawDDPSettings final : public QDialog
	{
		Q_OBJECT

			typedef QDialog
			Inherited;
	public:
		explicit RawDDPSettings(QWidget* parent = nullptr);
		~RawDDPSettings() override;

	private slots:
		void accept() override;
		void apply();
		void reject() override;
		void on_buttonBox_clicked(QAbstractButton* button);

		// Slots for RAW Files tab
		void brightness_editingFinished();
		void redScale_editingFinished();
		void blueScale_editingFinished();
		void on_noWB_clicked();
		void on_daylightWB_clicked();
		void on_cameraWB_clicked();
		void on_bilinear_clicked();
		void on_AHD_clicked();
		void on_rawBayer_clicked();
		void on_superPixels_clicked();

		// Slots for FITS Files tab
		void on_isFITSRaw_clicked(bool);
		void DSLRs_currentIndexChanged(int);
		void brightness_2_editingFinished();
		void redScale_2_editingFinished();
		void blueScale_2_editingFinished();
		void on_bilinear_2_clicked();
		void on_AHD_2_clicked();
		void on_rawBayer_2_clicked();
		void on_superPixels_2_clicked();

		void dataMin_editingFinished();
		void dataMax_editingFinished();

		// Slots for TIFF Files tab
		void brightness_3_editingFinished();
		void redScale_3_editingFinished();
		void blueScale_3_editingFinished();

	private:
		Ui::RawDDPSettings* ui;
		std::unique_ptr<Workspace> workspace;
		std::vector<CDSLR> vector_DSLRs;
		bool		initialised;
		QValidator* scaleValidator;
		QDoubleValidator* rangeValidator;
		QPixmap bggrPix;
		QPixmap gbrgPix;
		QPixmap grbgPix;
		QPixmap rggbPix;
		QPixmap cygmcymgPix;

		inline static const QString scaleDefault{ "1.0" };
		inline static const QString dataMinDefault { "0.0" };
		inline static const QString dataMaxDefault { "1.0" };

		static void fillDSLRList(std::vector<CDSLR>& vDSLRs);
		RawDDPSettings& updateBayerPattern();
		RawDDPSettings& updateControls();



		void showEvent(QShowEvent* event) override;
		void onInitDialog();
	};
}
