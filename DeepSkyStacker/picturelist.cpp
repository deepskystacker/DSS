#include "stdafx.h"
#include "picturelist.h"
namespace DSS
{
	PictureList::PictureList(QWidget* parent)
		: QDockWidget(parent)
	{
		setupUi(this);
		tableView->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
		tableView->horizontalHeader()->setSortIndicatorShown(true);
	}

	PictureList::~PictureList()
	{}

#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
	//
	// The user may not close the undocked window, but once DSS has set the 
	// closing flag a closeEvent must be accepted (default) otherwise DSS 
	// shutdown never completes.
	//
	void PictureList::closeEvent(QCloseEvent* event)
	{
		if (!dssClosing) event->ignore();
	}
#endif
}
