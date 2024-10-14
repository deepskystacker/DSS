#include "stdafx.h"
#include "ImageProperties.h"

namespace DSS
{
	ImageProperties::ImageProperties(QWidget* parent)
		: QDialog(parent)
	{
		setupUi(this);
		connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	}

ImageProperties::~ImageProperties()
{}
} // namespace DSS
