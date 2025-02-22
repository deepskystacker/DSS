#pragma once
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

#include <QDockWidget>
#include "DSSCommon.h"

namespace DSS
{
	class LowerDockWidget : public QDockWidget
	{
		friend class StackingDlg;
		friend class ProcessingDlg;
		typedef QDockWidget
			Inherited;

		Q_OBJECT

	public:
		LowerDockWidget(QWidget* parent = nullptr);
		~LowerDockWidget();

		inline int addWidget(QWidget* widget)
		{
			return stackedWidget->addWidget(widget);
		}

	public slots:
		void setDockTitle(const QString& text);

		void panelChanged(ActivePanel panel);


#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
		inline void setDSSClosing() { dssClosing = true; }
#endif

	private:
		QLabel* dockTitle;
		QStackedWidget* stackedWidget;

#if QT_VERSION < 0x060601		// Shouldn't need this in QT 6.6.1
	protected:
		void closeEvent(QCloseEvent* event) override;

	private:
		bool dssClosing;
#endif

	};
}