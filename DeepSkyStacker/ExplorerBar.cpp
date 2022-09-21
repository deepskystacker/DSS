#include <algorithm>
using std::min;
using std::max;

#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <afx.h>
#include <afxcmn.h>
#include <afxcview.h>
#include <afxwin.h>

#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QStandardPaths>

#include <Ztrace.h>
#include <zexcept.h>

extern bool		g_bShowRefStars;

#include "DSSCommon.h"
#include "commonresource.h"
#include "About.h"
#include "DSSVersion.h"
#include "DeepSkyStacker.h"

//#include "FrameList.h"
#include "RawDDPSettings.h"
#include "RecommendedSettings.h"
#include "RegisterSettings.h"
#include "StackSettings.h"
#include "Workspace.h"

#include "ExplorerBar.h"
#include "ui/ui_ExplorerBar.h"
#define dssApp DeepSkyStacker::instance()

static void makeLink(QLabel *label, QString color, QString text)
{
	label->setText(QString("<a href='.' style='text-decoration: none; color: %1'>%2</a>").arg(color, text));
}

static void makeLink(QLabel *label, QString color)
{
	makeLink(label, color, label->text());
}

ExplorerBar::ExplorerBar(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ExplorerBar)
{
	ZTRACE_RUNTIME("Creating Left Panel");
	ui->setupUi(this);

	makeLinks();

	raise();
	show();
	activateWindow();
	ZTRACE_RUNTIME("Creating Left Panel - ok");
}

ExplorerBar::~ExplorerBar()
{
	delete ui;
}

void ExplorerBar::makeLinks()
{
	QString defColour = palette().color(QPalette::ColorRole::WindowText).name();
	QString redColour = QColor(Qt::red).name();

	makeLink(ui->openLights, redColour);
	makeLink(ui->openDarks, defColour);
	makeLink(ui->openFlats, defColour);
	makeLink(ui->openDarkFlats, defColour);
	makeLink(ui->openBias, defColour);

	makeLink(ui->openFilelist, defColour);
	makeLink(ui->saveFilelist, defColour);

	makeLink(ui->clearList, defColour);

	makeLink(ui->checkAll, defColour);
	makeLink(ui->checkAbove, defColour);
	makeLink(ui->unCheckAll, defColour);

	makeLink(ui->registerChecked, redColour);
	makeLink(ui->computeOffsets, defColour);
	makeLink(ui->stackChecked, redColour);
	makeLink(ui->batchStacking, defColour);

	makeLink(ui->openPicture, defColour);
	makeLink(ui->copyPicture, defColour);
	makeLink(ui->doStarMask, defColour);
	makeLink(ui->savePicture, defColour);

	makeLink(ui->settings, defColour);
	makeLink(ui->ddpSettings, defColour);
	makeLink(ui->loadSettings, defColour);
	makeLink(ui->saveSettings, defColour);
	makeLink(ui->recommendedSettings, redColour);
	makeLink(ui->about, defColour);
	makeLink(ui->help, defColour);
}

//void ExplorerBar::linkActivated()
//{
//}

void ExplorerBar::onOpenLights()
{
	dssApp->getStackingDlg().onAddPictures();
}
void ExplorerBar::onOpenDarks()
{
	dssApp->getStackingDlg().onAddDarks();
}
void ExplorerBar::onOpenFlats()
{
	dssApp->getStackingDlg().onAddFlats();
}
void ExplorerBar::onOpenDarkFlats()
{
	dssApp->getStackingDlg().onAddDarkFlats();
}
void ExplorerBar::onOpenBias()
{
	dssApp->getStackingDlg().onAddOffsets();
}

/************************************************************************************/

void ExplorerBar::onOpenFilelist()
{
	dssApp->getStackingDlg().loadList();
}
void ExplorerBar::onSaveFilelist()
{
	dssApp->getStackingDlg().saveList();
}
void ExplorerBar::onClearList()
{
	dssApp->getStackingDlg().clearList();
}

/************************************************************************************/

void ExplorerBar::onCheckAbove()
{
	dssApp->getStackingDlg().checkAbove();
}
void ExplorerBar::onCheckAll()
{
	dssApp->getStackingDlg().checkAll();
}
void ExplorerBar::onUncheckAll()
{
	dssApp->getStackingDlg().unCheckAll();
}

/************************************************************************************/

void ExplorerBar::onRegisterChecked()
{
	dssApp->getStackingDlg().registerCheckedImages();
}
void ExplorerBar::onComputeOffsets()
{
	dssApp->getStackingDlg().computeOffsets();
}
void ExplorerBar::onStackChecked()
{
	dssApp->getStackingDlg().stackCheckedImages();
}
void ExplorerBar::onBatchStacking()
{
	dssApp->getStackingDlg().batchStack();
}

/************************************************************************************/

void ExplorerBar::onOpenPicture()
{
	dssApp->getProcessingDlg().OnLoaddsi();
}
void ExplorerBar::onCopyPicture()
{
	dssApp->getProcessingDlg().CopyPictureToClipboard();
}
void ExplorerBar::onDoStarMask()
{
	dssApp->getProcessingDlg().CreateStarMask();
}
void ExplorerBar::onSavePicture()
{
	dssApp->getProcessingDlg().SavePictureToFile();
}

/************************************************************************************/

void ExplorerBar::onSettings()
{
	QMenu menu(this);


	QAction *aRegisterSettings;
	QAction *aStackingSettings;

	aRegisterSettings = menu.addAction(QCoreApplication::translate("ExplorerBar", "Register Settings...", "ID_EDITSETTINGS_REGISTERSETTINGS"));
	aStackingSettings = menu.addAction(QCoreApplication::translate("ExplorerBar", "Stacking Settings...", "ID_EDITSETTINGS_STACKINGSETTINGS"));

	QPoint point(ui->settings->mapToGlobal(QPoint(0, 2 + ui->settings->height())));
	QAction *a = menu.exec(point);

	if (a == aRegisterSettings)
	{
		RegisterSettings dlg(this);
		dlg.setSettingsOnly(true);
		dlg.exec();
	}
	else if (a == aStackingSettings)
	{
		StackSettings dlg(this);
		dlg.setEnableAll(true);
		dlg.exec();
	}
}

void ExplorerBar::onDDPSettings()
{
	RawDDPSettings dlg(this);

	dlg.exec();
}

void ExplorerBar::onLoadSettings()
{
	ZFUNCTRACE_RUNTIME();
	Workspace	workspace;
	QMenu menu(this);

	if (mruPath.paths.size() != 0)
	{
		for (size_t i = 0; i != mruPath.paths.size(); i++)
		{
			menu.addAction(QString::fromStdU16String(mruPath.paths[i].generic_u16string()));
		}
		menu.addSeparator();
	}

	QAction* restoreSettings =
		menu.addAction(tr("Restore Default settings", "ID_LOADSETTINGS_RESTOREDEFAULTSETTINGS"));
	QAction* loadLiveSettings =
		menu.addAction(tr("Load DeepSkyStacker Live settings", "ID_LOADSETTINGS_LOADDEEPSKYSTACKERLIVESETTINGS"));

	menu.addSeparator();
	QAction* loadAnother =
		menu.addAction(tr("Load...", "ID_LOADSETTINGS_LOAD"));

	QPoint point(ui->settings->mapToGlobal(QPoint(0, 2 + ui->loadSettings->height())));
	QAction* a = menu.exec(point);
	if (a != nullptr)
	{
		if (a == restoreSettings)
		{
			ZTRACE_RUNTIME("Restoring default settings.");
			workspace.ResetToDefault();
			workspace.saveSettings();
		}
		else if (a == loadLiveSettings)
		{
			// Read the DSSLive setting file from the folder %AppData%/DeepSkyStacker
			fs::path fileName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdU16String());
			fileName /= "DSSLive.settings";		// Append the filename with a path separator
			ZTRACE_RUNTIME("Loading DSSLive settings from: %s", fileName.generic_string());
			workspace.ReadFromFile(fileName.generic_wstring().c_str());
			workspace.saveSettings();
		}
		else if (a == loadAnother)
		{
			ZTRACE_RUNTIME("Invoking dialog to load another settings file.");
			LoadSettingFile();
		}
		else
		{
			ZTRACE_RUNTIME("Loading settings file: %s", a->text().toLocal8Bit());
			//
			// One of the paths in the mruPath must have been selected
			// In which case the action's text string is the fully qualified name of the file to load
			//
			fs::path fileName(a->text().toStdU16String());
			workspace.ReadFromFile(fileName.generic_wstring().c_str());
			workspace.saveSettings();
			mruPath.Add(fileName);
			mruPath.saveSettings();
		}
	}
}
void ExplorerBar::onSaveSettings()
{
	ZFUNCTRACE_RUNTIME();
	Workspace	workspace;
	QMenu menu(this);

	if (mruPath.paths.size() != 0)
	{
		for (size_t i = 0; i != mruPath.paths.size(); i++)
		{
			menu.addAction(QString::fromStdU16String(mruPath.paths[i].generic_u16string()));
		}
		menu.addSeparator();
	}

	QAction* saveLiveSettings =
		menu.addAction(tr("Save as DeepSkyStacker Live settings", "ID_SAVESETTINGS_SAVEASDEEPSKYSTACKERLIVESETTINGS"));

	menu.addSeparator();
	QAction* saveAnother =
		menu.addAction(tr("Save as...", "ID_SAVESETTINGS_SAVEAS"));

	QPoint point(ui->settings->mapToGlobal(QPoint(0, 2 + ui->loadSettings->height())));
	QAction* a = menu.exec(point);
	if (a != nullptr)
	{
		if (a == saveLiveSettings)
		{
			// Read the DSSLive setting file from the folder %AppData%/DeepSkyStacker
			fs::path fileName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdU16String());
			fileName /= "DSSLive.settings";		// Append the filename with a path separator
			ZTRACE_RUNTIME("Saving DSSLive settings to: %s", fileName.generic_string());
			workspace.SaveToFile(fileName.generic_wstring().c_str());
		}
		else if (a == saveAnother)
		{
			ZTRACE_RUNTIME("Invoking dialog to load another settings file.");
			SaveSettingFile();
		}
		else
		{
			ZTRACE_RUNTIME("Saving settings to file: %s", a->text().toLocal8Bit());
			//
			// One of the paths in the mruPath must have been selected
			// In which case the action's text string is the fully qualified name of the file to load
			//
			fs::path fileName(a->text().toStdU16String());
			workspace.SaveToFile(fileName.generic_wstring().c_str());
			mruPath.Add(fileName);
			mruPath.saveSettings();
		}
	}
}

void ExplorerBar::onRecommendedSettings()
{
	RecommendedSettings		dlg(this);

	dlg.exec();
}

/************************************************************************************/

void ExplorerBar::onAbout()
{
	About dlg(this);

	dlg.exec();
}

void ExplorerBar::onHelp()
{
	QString helpFile = QCoreApplication::applicationDirPath() + 
		"/" + tr("DeepSkyStacker Help.chm", "IDS_HELPFILE");

	//
	// Invoking HtmlHelp works fine on Windows but ...
	//
	::HtmlHelp(::GetDesktopWindow(), CString((wchar_t*)helpFile.utf16()), HH_DISPLAY_TOPIC, 0);
}

void ExplorerBar::LoadSettingFile()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	Workspace			workspace;
	QString				directory;
	QString				extension("settings");
	bool				fileLoaded(false);

	directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	fileDialog.setWindowTitle(tr("Load DeepSkyStacker Settings", "IDS_TITLE_LOADSETTINGS"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFile);

	fileDialog.setNameFilter(tr("DSS Settings Files (*.settings *.dsssettings *.txt)"));
	fileDialog.selectFile(QString());		// No file(s) selected
	fileDialog.setDirectory(directory);

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QStringList files = fileDialog.selectedFiles();

		if (files.size() != 0)
		{
			fs::path fileName(files.at(0).toStdU16String());		// as UTF-16
			if (status(fileName).type() == fs::file_type::regular)
			{
				ZTRACE_RUNTIME("Loading settings file: %s", fileName.generic_string());
				QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

				workspace.ReadFromFile(fileName.generic_wstring().c_str());
				workspace.saveSettings();
				mruPath.Add(fileName);
				mruPath.saveSettings();
				fileLoaded = true;
				QGuiApplication::restoreOverrideCursor();
			}
		}
	}
	if (!fileLoaded) ZTRACE_RUNTIME("No settings file selected to load");
}

void ExplorerBar::SaveSettingFile()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	Workspace			workspace;
	QString				directory;
	QString				extension("settings");
	bool				fileSaved(false);

	directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	fileDialog.setWindowTitle(tr("Save DeepSkyStacker Settings", "IDS_TITLE_SAVESETTINGS"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::AnyFile);

	fileDialog.setNameFilter(tr("DSS Settings Files (*.settings *.dsssettings *.txt)"));
	fileDialog.setDirectory(directory);

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QString file = fileDialog.selectedFiles()[0];

		if (!file.isEmpty())
		{
			fs::path fileName(file.toStdU16String());		// as UTF-16
			if (status(fileName).type() == fs::file_type::regular)
			{
				ZTRACE_RUNTIME("Saving settings file: %s", fileName.generic_string());
				QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

				workspace.SaveToFile(fileName.generic_wstring().c_str());
				mruPath.Add(fileName);
				mruPath.saveSettings();
				fileSaved = true;
				QGuiApplication::restoreOverrideCursor();
			}
		}
	}
	if (!fileSaved) ZTRACE_RUNTIME("No file specified to save settings");
}

void ExplorerBar::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);

		//
		// The Labels are now plain text labels, so make them into links
		// just as done by the ctor.
		//
		makeLinks();
	}

	Inherited::changeEvent(event);
}

void ExplorerBar::mousePressEvent(QMouseEvent *event)
{
	if (Qt::LeftButton == event->buttons())
	{
		const auto dwTabID = dssApp->tab();
		if ((ui->registerAndStack->underMouse()) && (dwTabID != IDD_REGISTERING) && (dwTabID != IDD_STACKING))
		{
			// Change tab to stacking
			dssApp->setTab(IDD_STACKING);
		}
		else if (ui->processing->underMouse() && (dwTabID != IDD_PROCESSING))
		{
			// Change tab to processing
			dssApp->setTab(IDD_PROCESSING);
		};
	}
	Inherited::mousePressEvent(event);
}