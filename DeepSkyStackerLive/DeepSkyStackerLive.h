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
#include <QStyledItemDelegate>

class QWinHost;
class CLightFrameInfo;

namespace DSS
{
	class ImageViewer;
	class GraphViewer;
	class LiveSettings;
	class FolderMonitor;
	class FileRegistrar;
	class FileStacker;
	class ProgressLive;
}

class DeepSkyStackerLive :
	public QWidget,
	public Ui_DeepSkyStackerLive,
	public DSSBase
{
	using Inherited = QWidget;
	
	Q_OBJECT

signals:
	void stopMonitor();
	void clearStackedImage();
	void dropPendingImages();
	void showResetEmailCount();
	void clearCharts();

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

	inline std::uint32_t stackedImageCount() { return stackedImageCnt; }

	std::unique_ptr<DSS::LiveSettings> liveSettings;

protected:
	void closeEvent(QCloseEvent* e) override;
	void showEvent(QShowEvent* event) override;

	void onInitialise();

	void moveToNonStackable(fs::path& file);

public slots:
	void help();
	void resetEmailCount();

protected slots:
	// void updateStatus(const QString& text);
	void qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate);
	void qErrorMessage(const QString& message, const QString& type, QMessageBox::Icon icon, bool terminate);
	void writeLogMessage(const QString& message, bool addTimeStamp, bool bold, bool italic, QColor colour);

	void progress(const QString& str, int achieved, int total);
	void endProgress();
	void addImageToList(fs::path path);
	void fileLoaded(std::shared_ptr<LoadedImage> image, const fs::path fileName);
	void fileRegistered(std::shared_ptr<CLightFrameInfo> lfi);
	void fileNotStackable(fs::path file);
	void fileStacked(std::shared_ptr<CLightFrameInfo> p);
	void changeImageStatus(const QString& name, ImageStatus status);
	void updateStatusMessage();
	void handleWarning(QString text);
	void setImageOffsets(QString name, double dx, double dy, double angle);
	void setImageFootprint(QPointF p1, QPointF p2, QPointF p3, QPointF p4);
	void showStackedImage(std::shared_ptr<LoadedImage> li, int count, double exposure);
	void addToStackingQueue(std::shared_ptr<CLightFrameInfo> p);

private:
	bool initialised;
	static inline DeepSkyStackerLive* dssInstance{ nullptr };
	QWinHost* winHost;
	QStringList args;
	QString baseTitle;
	QErrorMessage* errorMessageDialog;
	QLabel* eMDI;		// errorMessageDialogIcon pointer
	QShortcut* helpShortCut;
	QString linkColour;
	QString monitoredFolder;
	DSS::FolderMonitor* folderMonitor;
	QStringList validExtensions;
	DSS::FileRegistrar* fileRegistrar;
	DSS::FileStacker* fileStacker;
	QLabel* progressLabel;
	DSS::ProgressLive* pProgress;
	std::uint32_t stackedImageCnt;		// was m_lNrStacked
	double totalExposure;
	std::uint32_t emailsSent;

	void connectSignalsToSlots();
	void connectMonitorSignals();
	void createFileRegistrar();
	void createFileStacker();
	void makeLinks();
	void startMonitoring();
	void stopMonitoring();
	void stopStacking();
	bool checkRestartMonitor();
	void removeFromListIfStatusIs(const QString& status);
	bool canWriteToMonitoredFolder();

	inline QString isoToString(int lISOSpeed) const
	{
		if (lISOSpeed)
			return QString::number(lISOSpeed);
		else
			return QString("-");
	}

	inline QString gainToString(int lGain) const
	{
		if (lGain >= 0)
			return QString::number(lGain);
		else
			return QString("-");
	}

private slots:
	bool setMonitoredFolder(const QString& link);
	void monitorPressed(bool checked);
	void stackPressed(bool checked);
	void stopPressed();

	void settingsChanged();

	void onExistingFiles(const std::vector<fs::path>&);
	void onNewFile(const fs::path& file);
};
using DeepSkyStacker = DeepSkyStackerLive;
using DSSLive = DeepSkyStackerLive;
