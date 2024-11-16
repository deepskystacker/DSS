/****************************************************************************
**
** Copyright (C) 2024 David C. Partridge
**
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
#include "stdafx.h"
#include "lowerdockwidget.h"

namespace DSS
{
	LowerDockWidget::LowerDockWidget(QWidget* parent) :
		QDockWidget(parent),
		dockTitle{ new QLabel(this) },
		stackedWidget{ new QStackedWidget(this) }
	{
		dockTitle->setToolTip(tr("Double click here to dock/undock"));
		setObjectName("lowerDockWidget");

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

		stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		setWidget(stackedWidget);
	}

	LowerDockWidget::~LowerDockWidget()
	{}

#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
	//
	// The user may not close the undocked window, but once DSS has set the 
	// closing flag a closeEvent must be accepted (default) otherwise DSS 
	// shutdown never completes.
	//
	void LowerDockWidget::closeEvent(QCloseEvent* event)
	{
		if (!dssClosing) event->ignore();
	}
#endif

	//
	// Slots
	//

	void LowerDockWidget::setDockTitle(const QString& text)
	{
		dockTitle->setText(text);
	}

	void LowerDockWidget::panelChanged(ActivePanel panel)
	{
		if (ActivePanel::StackingPanel != panel)
			setDockTitle("");

		auto index{ static_cast<int>(panel) };
		ZASSERT(index < stackedWidget->count());

		stackedWidget->setCurrentIndex(index);
	}

}
