#include "stdafx.h"
#include "picturelist.h"
namespace DSS
{
	PictureList::PictureList(QWidget* parent)
		: QDockWidget(parent),
		dssClosing { false }
	{
		setupUi(this);
		tableView->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
		tableView->horizontalHeader()->setSortIndicatorShown(true);
	}

	PictureList::~PictureList()
	{}

	//
	// The user may not close the undocked window, but once DSS has set the 
	// closing flag a closeEvent must be accepted (default) otherwise DSS 
	// shutdown never completes.
	//
	void PictureList::closeEvent(QCloseEvent* event)
	{
		if (!dssClosing) event->ignore();
	}
}
