#include "stdafx.h"
#include "picturelist.h"
namespace DSS
{
	PictureList::PictureList(QWidget* parent) :
		QDockWidget(parent),
		dockTitle{ new QLabel(this) }
	{
		setupUi(this);
		tableView->horizontalHeader()->setSortIndicator(1, Qt::AscendingOrder);
		tableView->horizontalHeader()->setSortIndicatorShown(true);

		dockTitle->setToolTip(tr("Double click here to dock/undock the image list"));

		//
		// Set an informative title bar on the dockable image list with a nice gradient
		// as the background (like the old "listInfo" static control).
		// 
		QSize size{ 625, 25 };
		dockTitle->setObjectName("dockTitle");
		dockTitle->setMinimumSize(size);
		dockTitle->resize(size);
		dockTitle->setStyleSheet(QString::fromUtf8("QLabel {"
			"background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
			"stop:0 rgba(138, 185, 242, 0), stop:1 rgba(138, 185, 242, 255))}"));
		setTitleBarWidget(dockTitle);
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
