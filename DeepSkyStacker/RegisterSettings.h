#ifndef REGISTERSETTINGS_H
#define REGISTERSETTINGS_H
#include <memory>

class CWorkspace;
class CAllStackingTasks;
class QValidator;

#include "DSSCommon.h"
#include <QDialog>

namespace Ui {
	class RegisterSettings;
}

class CWorkspace;

class RegisterSettings : public QDialog
{
	Q_OBJECT

typedef QDialog
		Inherited;
public:
	explicit RegisterSettings(QWidget *parent = nullptr);
	~RegisterSettings();

	inline void	setSettingsOnly(bool bSettingsOnly) noexcept
	{
		settingsOnly = bSettingsOnly;
	};

	inline bool	isForceRegister() noexcept
	{
		return forceRegister;
	};

	inline bool	isStackAfter(double & fPercent) noexcept
	{
		fPercent = (double)percentStack;

		return stackAfter;
	};

	inline RegisterSettings& setStackingTasks(CAllStackingTasks * ptr) noexcept
	{
		pStackingTasks = ptr;
		return *this;
	};
	   
private slots:

	void on_recommendedSettings_clicked();
	void on_stackingSettings_clicked();

	void accept() override;
	void reject() override;

	void on_forceRegister_stateChanged(int);
	void on_hotPixels_stateChanged(int);
	void on_stackAfter_clicked();
	void on_percentStack_textEdited(const QString &text);

	void on_luminanceThreshold_valueChanged(int);
	void on_computeDetectedStars_clicked();
	void on_medianFilter_stateChanged(int);



private:
	Ui::RegisterSettings *ui;
	std::unique_ptr<CWorkspace> workspace;

	bool					initialised;
	bool					forceRegister;
	bool					stackAfter;
	uint 					percentStack;
	bool					noDarks;
	bool					noFlats;
	bool					noOffsets;
	uint					detectionThreshold;
	bool					medianFilter;
	QString					firstLightFrame;
	CAllStackingTasks *		pStackingTasks;
	bool					settingsOnly;
	QValidator *			perCentValidator;

	void showEvent(QShowEvent *event) override;

	void onInitDialog();
};

#endif // REGISTERSETTINGS_H
