#include "stdafx.h"
#include "picturelist.h"
namespace DSS
{
	PictureList::PictureList(QWidget* parent)
		: QDockWidget(parent)
	{
		setupUi(this);
	}

	PictureList::~PictureList()
	{}

	//
	// The user may not close the undocked window
	//
	void PictureList::closeEvent(QCloseEvent* event)
	{
		event->ignore();
	}
}
