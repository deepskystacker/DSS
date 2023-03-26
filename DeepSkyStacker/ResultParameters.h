#ifndef RESULTPARAMETERS_H
#define RESULTPARAMETERS_H
#include <memory>

#include <QWidget>
#include <QPicture>

#include "StackingTasks.h"

class Workspace;

namespace Ui {
class ResultParameters;
}

class ResultParameters : public QWidget
{
	Q_OBJECT

typedef QWidget
		Inherited;

public:
	explicit ResultParameters(QWidget *parent = nullptr);
	~ResultParameters();

	inline void setStackingTasks(CAllStackingTasks * pTasks)
	{
		pStackingTasks = pTasks;
		customRectEnabled = pTasks->getCustomRectangle(customRect);
	}

public slots:
	void onSetActive();

private:
	Ui::ResultParameters *ui;
	std::unique_ptr<Workspace> workspace;
	QPixmap normalPix;
	QPixmap mosaicPix;
	QPixmap intersectionPix;
	QPixmap customPix;
	CAllStackingTasks* pStackingTasks;
	bool customRectEnabled;
	DSSRect customRect;

private slots:
	void	on_normalMode_clicked();
	void	on_mosaicMode_clicked();
	void	on_intersectionMode_clicked();
	void	on_customMode_clicked();
	void	on_drizzle2x_clicked();
	void	on_drizzle3x_clicked();
	void	on_alignRGB_clicked();

};
#endif // RESULTPARAMETERS_H
