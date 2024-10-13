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

	class RawDDPSettings : public QDialog
	{
		Q_OBJECT

			typedef QDialog
			Inherited;
	public:
		explicit RawDDPSettings(QWidget* parent = nullptr);
		~RawDDPSettings();

	private slots:
		void accept() override;
		void apply();
		void reject() override;
		void on_buttonBox_clicked(QAbstractButton* button);

		// Slots for RAW Files tab
		void brightness_editingFinished();
		void redScale_editingFinished();
		void blueScale_editingFinished();
		void on_noWB_stateChanged();
		void on_cameraWB_stateChanged();
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

	private:
		Ui::RawDDPSettings* ui;
		std::unique_ptr<Workspace> workspace;
		std::vector<CDSLR> vector_DSLRs;
		bool		initialised;
		QValidator* scaleValidator;
		QPixmap bggrPix;
		QPixmap gbrgPix;
		QPixmap grbgPix;
		QPixmap rggbPix;
		QPixmap cygmcymgPix;


		static void fillDSLRList(std::vector<CDSLR>& vDSLRs);
		RawDDPSettings& updateBayerPattern();
		RawDDPSettings& updateControls();



		void showEvent(QShowEvent* event) override;
		void onInitDialog();
	};
}