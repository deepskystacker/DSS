#pragma once
/****************************************************************************
**
** Copyright (C) 2023 David C. Partridge
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
// DeepSkyStackerLive.h : main header file for DeepSkyStackerLive
//
#include "dssbase.h"
#include "ui/ui_DeepSkyStackerLive.h"

class QWinHost;

namespace DSS
{
	class ImageViewer;
	class GraphViewer;
	class LiveSettings;
}

class DeepSkyStackerLive :
	public QWidget,
	public Ui_DeepSkyStackerLive,
	public DSSBase
{
	using Inherited = QWidget;
	
	Q_OBJECT

public:
	DeepSkyStackerLive();
	~DeepSkyStackerLive();

	//
	// Don't intend this to be copied or assigned.
	//
	DeepSkyStackerLive(const DeepSkyStackerLive&) = delete;
	DeepSkyStackerLive& operator=(const DeepSkyStackerLive&) = delete;
	DeepSkyStackerLive(DeepSkyStackerLive&& rhs) = delete;
	DeepSkyStackerLive& operator=(DeepSkyStackerLive&& rhs) = delete;

	void reportError(const QString& message, const QString& type, Severity severity, Method method, bool terminate) override;
	void writeToLog(const QString& message, bool addTimeStamp = false, bool bold = false, bool italic = false, QColor colour = QColor(QPalette().color(QPalette::WindowText)));

	inline qreal pixelRatio() { return this->devicePixelRatioF(); }

	inline static DeepSkyStackerLive* instance()
	{
		return dssInstance;
	}

	std::unique_ptr<DSS::LiveSettings> liveSettings;


protected:
	void closeEvent(QCloseEvent* e) override;
	void showEvent(QShowEvent* event) override;

	void onInitialise();

protected slots:
	// void updateStatus(const QString& text);
	void qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate);
	void qErrorMessage(const QString& message, const QString& type, QMessageBox::Icon icon, bool terminate);
	void writeLogMessage(const QString& message, bool addTimeStamp, bool bold, bool italic, QColor colour);

private:
	bool initialised;
	bool monitoring;
	static inline DeepSkyStackerLive* dssInstance{ nullptr };
	QWinHost* winHost;
	QStringList args;
	QString baseTitle;
	QErrorMessage* errorMessageDialog;
	QLabel* eMDI;		// errorMessageDialogIcon pointer
	QString linkColour;
	DSS::ImageViewer* stackedImageViewer;
	DSS::ImageViewer* lastImageViewer;
	QString monitoredFolder;

	void connectSignalsToSlots();
	void makeLinks();

private slots:
	void setMonitoredFolder(const QString& link);
	void monitorTriggered(bool checked);
	void stackTriggered(bool checked);
	void stopTriggered();

};
using DeepSkyStacker = DeepSkyStackerLive;
using DSSLive = DeepSkyStackerLive;
