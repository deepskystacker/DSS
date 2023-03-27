#pragma once
#include "dssrect.h"
class Workspace;

namespace Ui {
class ResultParameters;
}

class CAllStackingTasks;
class ResultParameters : public QWidget
{
	Q_OBJECT

typedef QWidget
		Inherited;

public:
	explicit ResultParameters(QWidget *parent = nullptr);
	~ResultParameters();

	void setStackingTasks(CAllStackingTasks* pTasks);

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
