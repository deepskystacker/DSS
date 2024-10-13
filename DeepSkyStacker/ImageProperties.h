#pragma once

#include "ui/ui_ImageProperties.h"

namespace DSS 
{
	class ImageProperties  : public QDialog, public Ui_ImageProperties
	{
		Q_OBJECT

	public:
		ImageProperties(QWidget *parent);
		~ImageProperties();
	};
} // namespace DSS
