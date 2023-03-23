
#pragma once
/****************************************************************************
**
** Copyright (C) 2020, 2022 David C. Partridge
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
#include <list>
#include <QMainWindow>
#include <QMessageBox>
#include <QLabel>

#include "dssbase.h"

#include "ExplorerBar.h"
class QSplitter;
class QStackedWidget;
#include "StackingDlg.h"
class QWinHost;

namespace DSS
{
	class PictureList;
}

#include "ProcessingDlg.h"
#include "dss_settings.h"

class DeepSkyStacker :
	public QMainWindow,
	public DSSBase
{
	typedef QMainWindow
		Inherited;

	Q_OBJECT

protected slots:
	void updateStatus(const QString& text);
	void displayMessage(const QString& message, QMessageBox::Icon icon);

private:
	bool initialised;
	ExplorerBar* explorerBar;
	DSS::PictureList* pictureList;
	QStackedWidget* stackedWidget;
	DSS::StackingDlg* stackingDlg;
	QWinHost* winHost;

	CProcessingDlg	processingDlg;

	CDeepStack				m_DeepStack;
	CDSSSettings			m_Settings;
	std::uint32_t			currTab;
	QStringList				args;
	QString					baseTitle;
	QString currentPathName;
	//ITaskbarList3* m_taskbarList;
	bool                    m_progress;
	QLabel*	statusBarText;

	void createStatusBar();
	void updateTab();
	static inline DeepSkyStacker* theMainWindow{ nullptr };
	void connectSignalsToSlots();

protected:
	void closeEvent(QCloseEvent* e) override;
	void dragEnterEvent(QDragEnterEvent* e);
	void dropEvent(QDropEvent* e);
	void showEvent(QShowEvent* event) override;

	void onInitialise();

public:
	inline static DeepSkyStacker* instance()
	{
		return dynamic_cast<DeepSkyStacker*>(DSSBase::instance());
	}

	DeepSkyStacker();

	~DeepSkyStacker()
	{
	}

	inline QString statusMessage()
	{
		return statusBarText->text();
	}

	CDeepStack& deepStack()
	{
		return m_DeepStack;
	};

	qreal pixelRatio()
	{
		return devicePixelRatioF();
	}

	void	setTab(std::uint32_t dwTabID)
	{
		if (dwTabID == IDD_REGISTERING)
			dwTabID = IDD_STACKING;
		//#ifdef DSSBETA
		//	if (dwTabID == IDD_STACKING && 	(GetAsyncKeyState(VK_CONTROL) & 0x8000))
		//		dwTabID = IDD_LIBRARY;
		//#endif
		currTab = dwTabID;
		updateTab();
	};

	std::uint32_t tab()
	{
		return currTab;
	};

	inline void disableSubDialogs()
	{
		stackingDlg->setEnabled(false);
		processingDlg.EnableWindow(false);
		//m_dlgLibrary.EnableWindow(false);
		explorerBar->setEnabled(false);
	};

	inline void enableSubDialogs()
	{
		stackingDlg->setEnabled(true);
		processingDlg.EnableWindow(true);
		//m_dlgLibrary.EnableWindow(true);
		explorerBar->setEnabled(true);
	};

	CDSSSettings& settings()
	{
		if (!m_Settings.IsLoaded())
			m_Settings.Load();

		return m_Settings;
	};

	DSS::StackingDlg& getStackingDlg()
	{
		return *stackingDlg;
	};

	CProcessingDlg& getProcessingDlg()
	{
		return processingDlg;
	};


	ExplorerBar& GetExplorerBar()
	{
		return *explorerBar;
	};

	inline void setWindowFilePath(const QString& name)
	{
		if (currentPathName == name) return;
		currentPathName = name;
		if (!name.isEmpty())
			setWindowTitle(QString("%1 - %2").arg(baseTitle).arg(name));
		else
			setWindowTitle(baseTitle);
	}

	virtual inline void reportError(const QString& message, DSSBase::Severity severity) override
	{
		QMetaObject::invokeMethod(this, "displayMessage", Qt::QueuedConnection,
			Q_ARG(const QString&, message),
			Q_ARG(QMessageBox::Icon, static_cast<QMessageBox::Icon>(severity) ));
	}

};


class DeepSkyStackerApp : public CWinApp
{
public :
	CWnd *				m_pMainDlg;

public :
	DeepSkyStackerApp() :
		m_pMainDlg{ nullptr }
	{
        
	};

	virtual ~DeepSkyStackerApp()
	{

	};

	virtual BOOL InitInstance( ) override;
	virtual int ExitInstance() override;
	virtual int Run() override;

};

//
// Temporarily left here while still have to position MFC windows
//
void	SaveWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath);
void	RestoreWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath, bool bCenter = false);

