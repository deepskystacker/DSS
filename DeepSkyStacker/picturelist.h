#pragma once

#include "ui/ui_PictureList.h"

namespace DSS
{

	class PictureList : public QDockWidget, public Ui::PictureList
	{
		friend class StackingDlg;
		typedef QDockWidget
			Inherited;
		
		Q_OBJECT

	public:
		PictureList(QWidget* parent = nullptr);
		~PictureList();

		inline void setDSSClosing() { dssClosing = true; }

	protected:
		void closeEvent(QCloseEvent* event);

	private:
		bool dssClosing;

	};
}
