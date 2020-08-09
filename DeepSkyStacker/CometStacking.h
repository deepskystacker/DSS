#ifndef COMETSTACKING_H
#define COMETSTACKING_H
#include <memory>

class QPixmap;

#include "DSSCommon.h"
#include <QWidget>

namespace Ui {
class CometStacking;
}

class CWorkspace;

class CometStacking : public QWidget
{
    Q_OBJECT

typedef QWidget
		Inherited;
public:
    explicit CometStacking(QWidget *parent = nullptr);
    ~CometStacking();

	void setCometStackingMode(COMETSTACKINGMODE);

public slots:
	void onSetActive();

private slots:
    void on_modeStandard_clicked();
	void on_modeComet_clicked();
	void on_modeAdvanced_clicked();

private:
    Ui::CometStacking *ui;
	std::unique_ptr<CWorkspace> workspace;
    COMETSTACKINGMODE m_CometStackingMode;
	QPixmap standardPix;
	QPixmap cometPix;
	QPixmap advancedPix;

    void updateImage();
};

#endif // COMETSTACKING_H
