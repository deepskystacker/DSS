#ifndef RAWDDPSETTINGS_H
#define RAWDDPSETTINGS_H
#include <memory>

class CWorkspace;
class QAbstractButton;
class QValidator;
class CWorkspace;
class CDSLR;
enum CFATYPE : unsigned long;

#include "DSSCommon.h"
#include <QDialog>

namespace Ui {
	class RawDDPSettings;
}

class RawDDPSettings : public QDialog
{
	Q_OBJECT

typedef QDialog
		Inherited;
public:
	explicit RawDDPSettings(QWidget *parent = nullptr);
	~RawDDPSettings();

private slots:
	void accept() override;
	void apply();
	void reject() override;
	void on_buttonBox_clicked(QAbstractButton *button);

	// Slots for RAW Files tab
	void on_brightness_textEdited(const QString&);
	void on_redScale_textEdited(const QString&);
	void on_blueScale_textEdited(const QString&);
	void on_noWB_clicked();
	void on_cameraWB_clicked();
	void on_bilinear_clicked();
	void on_AHD_clicked();
	void on_rawBayer_clicked();
	void on_superPixels_clicked();
	void on_blackPointToZero_clicked(bool);

	// Slots for FITS Files tab
	void on_isFITSRaw_clicked(bool);
	void on_DSLRs_currentIndexChanged(int);
	void on_brightness_2_textEdited(const QString&);
	void on_redScale_2_textEdited(const QString&);
	void on_blueScale_2_textEdited(const QString&);
	void on_bilinear_2_toggled(bool);
	void on_AHD_2_toggled(bool);
	void on_rawBayer_2_toggled(bool);
	void on_superPixels_2_toggled(bool);
private:
	Ui::RawDDPSettings *ui;
	std::unique_ptr<CWorkspace> workspace;
	std::vector<CDSLR> vector_DSLRs;
	bool		initialised;
	QValidator *	scaleValidator;
	QPixmap bggrPix;
	QPixmap gbrgPix;
	QPixmap grbgPix;
	QPixmap rggbPix;
	QPixmap cygmcymgPix;


	static void fillDSLRList(std::vector<CDSLR> & vDSLRs);
	RawDDPSettings & updateBayerPattern();
	RawDDPSettings & updateControls();



	void showEvent(QShowEvent *event) override;
	void onInitDialog();
};

#endif // RAWDDPSETTINGS_H
