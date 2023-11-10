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
#include "dssbase.h"
#include "dss_settings.h"
namespace DSS
{
	class PictureList;
	class StackingDlg;
}
class ExplorerBar;
class QStackedWidget;
class QWinHost;
class CProcessingDlg;
class CDSSSettings;
class CDeepStack;

class DeepSkyStacker :
	public QMainWindow,
	public DSSBase
{
	typedef QMainWindow
		Inherited;

	Q_OBJECT

signals:
	void tabChanged();

public slots:
	void help();

protected slots:
	void updateStatus(const QString& text);
	void qMessageBox(const QString& message, QMessageBox::Icon icon, bool terminate);
	void qErrorMessage(const QString& message, const QString& type, QMessageBox::Icon icon, bool terminate);

private:
	bool initialised;
	ExplorerBar* explorerBar;
	DSS::PictureList* pictureList;
	QStackedWidget* stackedWidget;
	DSS::StackingDlg* stackingDlg;
	QWinHost* winHost;
	CWnd hostWnd;
	std::unique_ptr<CProcessingDlg>	processingDlg;
	std::unique_ptr<CDeepStack> m_DeepStack;
	std::unique_ptr<CDSSSettings> m_ImageProcessingSettings;
	std::uint32_t currTab;
	QStringList args;
	QString baseTitle;
	QString currentPathName;
	bool m_progress;
	QLabel* sponsorText;
	QLabel* statusBarText;
	QErrorMessage* errorMessageDialog;
	QLabel* eMDI;		// errorMessageDialogIcon pointer
	QShortcut* helpShortCut;

	void createStatusBar();
	void updateTab();
	static inline DeepSkyStacker* theMainWindow{ nullptr };
	void connectSignalsToSlots();

protected:
	void closeEvent(QCloseEvent* e) override;
	void dragEnterEvent(QDragEnterEvent* e) override;
	void dropEvent(QDropEvent* e) override;
	void showEvent(QShowEvent* event) override;

	void onInitialise();

public:
	inline static DeepSkyStacker* instance()
	{
		return dynamic_cast<DeepSkyStacker*>(DSSBase::instance());
	}

	DeepSkyStacker();
	~DeepSkyStacker();

	inline qreal pixelRatio() { return devicePixelRatioF(); }
	inline std::uint32_t tab() { return currTab; }


	QString statusMessage();
	CDeepStack& deepStack();
	void setTab(std::uint32_t dwTabID);
	void disableSubDialogs();
	void enableSubDialogs();
	CDSSSettings& imageProcessingSettings();
	DSS::StackingDlg& getStackingDlg();
	CProcessingDlg& getProcessingDlg();
	ExplorerBar& GetExplorerBar();
	void setWindowFilePath(const QString& name);
	void reportError(const QString& message, const QString& type, Severity severity, Method method, bool terminate) override;
};


class DeepSkyStackerApp : public CWinApp
{
public :
	CWnd *				m_pMainDlg;

public :
	DeepSkyStackerApp() : m_pMainDlg{ nullptr } {}
	virtual ~DeepSkyStackerApp() = default;

	virtual BOOL InitInstance( ) override;
	virtual int ExitInstance() override;
	virtual int Run() override;

};

//
// Temporarily left here while still have to position MFC windows
//
void	SaveWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath);
void	RestoreWindowPosition(CWnd* pWnd, LPCSTR szRegistryPath, bool bCenter = false);

