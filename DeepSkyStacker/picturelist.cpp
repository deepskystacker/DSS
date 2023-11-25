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
}
