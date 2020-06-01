#ifndef INTERMEDIATEFILES_H
#define INTERMEDIATEFILES_H

#include <QWidget>

class CWorkspace;

namespace Ui {
class IntermediateFiles;
}

class IntermediateFiles : public QWidget
{
    Q_OBJECT

typedef QWidget
		Inherited;

public:
    explicit IntermediateFiles(QWidget *parent = nullptr);
    ~IntermediateFiles();

public slots:
	void onSetActive();

private:
    Ui::IntermediateFiles *ui;
	std::unique_ptr<CWorkspace> workspace;

private slots:
	void on_formatFITS_clicked();
	void on_formatTIFF_clicked();

	void on_saveCalibrated_stateChanged(int state);
	void on_saveBayered_stateChanged(int state);
	void on_saveIntermediate_stateChanged(int state);
};

#endif // INTERMEDIATEFILES_H
