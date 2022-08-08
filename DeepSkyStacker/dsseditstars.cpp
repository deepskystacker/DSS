/****************************************************************************
**
** Copyright (C) 2020 David C. Partridge
* **
** BSD License Usage
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of DeepSkyStacker nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
**
****************************************************************************/
#include <QDebug>
#include <QMouseEvent>

#include "dsseditstars.h"
#include "dssimageview.h"

DSSEditStars::DSSEditStars(QWidget* parent) :
	QWidget(parent)
{
	imageView = dynamic_cast<DSSImageView*>(parent);
	Q_ASSERT(nullptr != imageView);
	setAttribute(Qt::WA_TransparentForMouseEvents);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_WState_ExplicitShowHide);
}


void DSSEditStars::mousePressEvent([[maybe_unused]] QMouseEvent* e)
{
	qDebug() << "mouse pressed";
}

void DSSEditStars::mouseMoveEvent([[maybe_unused]] QMouseEvent* e)
{

}

void DSSEditStars::mouseReleaseEvent([[maybe_unused]] QMouseEvent* e)
{

}

void DSSEditStars::rectButtonChecked()
{
	//
	// No longer interested in signals from the imageView object
	//
	imageView->disconnect(this, nullptr);
}

void DSSEditStars::starsButtonChecked()
{
	qDebug() << "stars checked";
	connect(imageView, SIGNAL(Image_mousePressEvent(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
	connect(imageView, SIGNAL(Image_mouseMoveEvent(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
	connect(imageView, SIGNAL(Image_mouseReleaseEvent(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
	connect(imageView, SIGNAL(Image_resizeEvent(QResizeEvent*)), this, SLOT(resizeEvent(QResizeEvent*)));
}

void DSSEditStars::cometButtonChecked()
{
	qDebug() << "comet checked";
	connect(imageView, SIGNAL(Image_mousePressEvent(QMouseEvent*)), this, SLOT(mousePressEvent(QMouseEvent*)));
	connect(imageView, SIGNAL(Image_mouseMoveEvent(QMouseEvent*)), this, SLOT(mouseMoveEvent(QMouseEvent*)));
	connect(imageView, SIGNAL(Image_mouseReleaseEvent(QMouseEvent*)), this, SLOT(mouseReleaseEvent(QMouseEvent*)));
	connect(imageView, SIGNAL(Image_resizeEvent(QResizeEvent*)), this, SLOT(resizeEvent(QResizeEvent*)));
}

void DSSEditStars::saveButtonPressed()
{
	qDebug() << "save pressed";
}
