#pragma once

class QPixmap;

#include "DSSCommon.h"

class Workspace;
namespace DSS
{
	namespace Ui {
		class CometStacking;
	}

	class CometStacking : public QWidget
	{
		Q_OBJECT

			typedef QWidget
			Inherited;
	public:
		explicit CometStacking(QWidget* parent = nullptr);
		~CometStacking();

		void setCometStackingMode(COMETSTACKINGMODE);

	public slots:
		void onSetActive();

	private slots:
		void on_modeStandard_clicked();
		void on_modeComet_clicked();
		void on_modeAdvanced_clicked();

	private:
		Ui::CometStacking* ui;
		std::unique_ptr<Workspace> workspace;
		COMETSTACKINGMODE m_CometStackingMode;
		QPixmap standardPix;
		QPixmap cometPix;
		QPixmap advancedPix;

		void updateImage();
	};
}