/****************************************************************************
**
** Copyright (C) 2020 David C. Partridge
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
// StackingDlg.cpp : implementation file
//

#include "stdafx.h"

#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QTreeWidget>
#include <QShowEvent>
#include <QSettings>
#include <QUrl>

#include "ui/ui_StackingDlg.h"
#include "mrupath.h"

#include "DeepSkyStacker.h"
#include "StackingDlg.h"
#include "DeepStackerDlg.h"
#include "DeepStack.h"
#include "ProgressDlg.h"
#include "CheckAbove.h"
#include "Registry.h"
#include "RegisterSettings.h"
#include "StackRecap.h"
#include "TIFFUtil.h"
#include "RegisterEngine.h"
#include "StackingEngine.h"
#include "DropFilesDlg.h"
#include "SaveEditChanges.h"
#include "AskRegistering.h"
#include "BatchStacking.h"
#include "DSSVersion.h"
#include "dssgroup.h"
#include "ui/ui_StackingDlg.h"

#define _USE_MATH_DEFINES
#include <cmath>

constexpr	DWORD					IDC_EDIT_SELECT = 1;
constexpr	DWORD					IDC_EDIT_STAR   = 2;
constexpr	DWORD					IDC_EDIT_COMET  = 3;
constexpr	DWORD					IDC_EDIT_SAVE   = 4;

const QStringList INPUTFILE_FILTERS({
	QCoreApplication::translate("StackingDlg", "Picture Files (*.bmp *.jpg *.jpeg *.tif *.tiff *.png *.fit *.fits *.fts *.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)"),
	QCoreApplication::translate("StackingDlg", "Windows Bitmaps (*.bmp)"),
	QCoreApplication::translate("StackingDlg", "JPEG or PNG Files (*.jpg *.jpeg *.png)"),
	QCoreApplication::translate("StackingDlg", "TIFF Files (*.tif *.tiff)"),
	QCoreApplication::translate("StackingDlg", "RAW Files (*.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)"),
	QCoreApplication::translate("StackingDlg", "FITS Files (*.fits *.fit *.fts)"),
	QCoreApplication::translate("StackingDlg", "All Files (*)")
	});

/////////////////////////////////////////////////////////////////////////////
// StackingDlg dialog

StackingDlg::StackingDlg(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::StackingDlg),
	initialised(false),
	groupId(0)
{
	mruPath.readSettings();
	connect(ui->tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableViewItemClickedEvent(const QModelIndex&)));
	connect(&imageLoader, SIGNAL(imageLoaded()), this, SLOT(imageLoad()));
}

StackingDlg::~StackingDlg()
{
	delete ui;
}

void StackingDlg::setSelectionRect(QRectF rect)
{
	selectRect = rect;
}

void StackingDlg::showEvent(QShowEvent* event)
{
	if (!event->spontaneous())
	{
		if (!initialised)
		{
			initialised = true;
			onInitDialog();
		}
	}
	// Invoke base class showEvent()
	return Inherited::showEvent(event);
}

void StackingDlg::onInitDialog()
{
}

void StackingDlg::tableViewItemClickedEvent(const QModelIndex& index)
{
	qDebug() << "Table View item clicked, row " << index.row();
	QItemSelectionModel * qsm = ui->tableView->selectionModel();
	QModelIndexList selectedRows = qsm->selectedRows();
	//
	// If only one row is selected, we want to know the filename
	//
	if (1 == selectedRows.count())
	{
		QModelIndex& index = selectedRows[0];
		if (index.isValid())
		{
			CString  fileName;
			const DSS::ImageListModel* model = dynamic_cast<const DSS::ImageListModel*>(index.model());
			int row = index.row();
			fileName = model->selectedFileName(row);
			//
			// If the filename hasn't changed but we have changes saved
			//
			if (fileName == m_strFileName  && checkEditChanges())
			{
				ui->information->setText(QString::fromWCharArray(m_strShowFile.GetString()));
				ui->information->setTextFormat(Qt::PlainText);
				ui->information->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
				ui->information->setOpenExternalLinks(false);
				imageLoad();
			}
		}
		else
		{
			ui->information->setText("");
			ui->information->setTextFormat(Qt::PlainText);
			ui->information->setTextInteractionFlags(Qt::NoTextInteraction);
			ui->information->setOpenExternalLinks(false);
		}
	}
}

//
// This member function/Slot is invoked under two conditions:
//
// 1. To request the loading of an image file which may or may not already be loaded,
//    by invoking imageLoader.load().
// 
//    If the image was previously loaded is still available in the image cache then the result will be true and 
//    both pBitMap and phBitmap will be set.
//
//    If the image is not in the cache, then the result will be false, and imageLoader.load will load the image
//    into the cache in a background thread running in the default Qt threadpool.
//
// 2. On completion of image loading by the background thread.  In this case the image will now be available in 
//    the cache, so invoking imageLoader.load() will now return true. 
void StackingDlg::imageLoad()
{
	std::shared_ptr<CMemoryBitmap>	pBitmap;
	std::shared_ptr<C32BitsBitmap>	phBitmap;

	if (m_strShowFile.GetLength() && imageLoader.load(QString::fromWCharArray(m_strShowFile.GetString()), &pBitmap, &phBitmap))
	{
		//
		// The image we want is available in the cache
		//
		m_LoadedImage.m_hBitmap = phBitmap;
		m_LoadedImage.m_pBitmap = pBitmap;
		if (m_GammaTransformation.IsInitialized())
			ApplyGammaTransformation(m_LoadedImage.m_hBitmap, m_LoadedImage.m_pBitmap, m_GammaTransformation);
		m_Picture.SetImg(phBitmap->GetHBITMAP(), true);

		if (m_Pictures.IsLightFrame(m_strShowFile))
		{
			m_Picture.SetButtonToolbar(&m_ButtonToolbar);
			m_EditStarSink.SetLightFrame(m_strShowFile);
			m_EditStarSink.SetBitmap(pBitmap);
			m_Picture.SetImageSink(GetCurrentSink());
		}
		else
		{
			m_Picture.SetImageSink(nullptr);
			m_Picture.SetButtonToolbar(nullptr);
			m_EditStarSink.SetBitmap(nullptr);
		};
		m_Picture.SetBltMode(CWndImage::bltFitXY);
		m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);

		CBilinearParameters		Transformation;
		VOTINGPAIRVECTOR		vVotedPairs;

		if (m_Pictures.GetTransformation(m_strShowFile, Transformation, vVotedPairs))
			m_EditStarSink.SetTransformation(Transformation, vVotedPairs);
		m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
		m_Infos.SetText(m_strShowFile);
	}
	else if (m_strShowFile.GetLength())
	{
		CString				strText;

		strText.Format(IDS_LOADPICTURE, (LPCTSTR)m_strShowFile);
		m_Infos.SetBkColor(RGB(252, 251, 222), RGB(255, 151, 154), CLabel::Gradient);
		m_Infos.SetText(strText);
		m_Picture.SetImageSink(nullptr);
		m_Picture.SetButtonToolbar(nullptr);
		m_EditStarSink.SetBitmap(nullptr);
	}
	else
	{
		m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
		m_Infos.SetText("");
		m_Picture.SetImageSink(nullptr);
		m_Picture.SetButtonToolbar(nullptr);
		m_EditStarSink.SetBitmap(nullptr);
	};
	return 1;
};


void StackingDlg::onAddPictures()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	QSettings			settings;
	QString				directory;
	QString				extension;
	uint				filterIndex = 0;
	QString				strTitle;

	directory = settings.value("Folders/AddPictureFolder").toString();

	extension = settings.value("Folders/AddPictureExtension").toString();

	filterIndex = settings.value("Folders/AddPictureIndex", 0U).toUInt();

	if (extension.isEmpty())
		extension = "bmp";			// Note that Qt doesn't want/ignores leading . in file extensions

	fileDialog.setWindowTitle(QCoreApplication::translate("StackingDlg", "Open Light Frames...", "IDS_TITLE_OPENLIGHTFRAMES"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);

	fileDialog.setNameFilters(INPUTFILE_FILTERS);
	fileDialog.selectFile(QString());		// No file(s) selected
	if (!directory.isEmpty())
		fileDialog.setDirectory(directory);

	//
	// A value of zero for filterIndex means that the user hasn't previously chosen a name filter.
	// In that case we'll choose to use the first one to start things off.
	// 
	// If the user *has* previously chosen a name filter, then that index is 1 based, not zero based.
	// This means that the index must be decremented to index into the list of name filters.
	//
	if (0 == filterIndex) filterIndex = 1;
	filterIndex = std::clamp(filterIndex, 1U, static_cast<uint>(INPUTFILE_FILTERS.size()));
	fileDialog.selectNameFilter(INPUTFILE_FILTERS.at(filterIndex - 1));
	
	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList files = fileDialog.selectedFiles();
		for (int i = 0; i < files.size(); i++)
		{
			fs::path file(files.at(i).toStdU16String());		// as UTF-16

			imageGroups[groupId].AddFile((LPCTSTR)file.generic_wstring().c_str(),
				currentGroupId());
			directory = QString::fromStdU16String(file.remove_filename().generic_u16string());
			extension = QString::fromStdU16String(file.extension().generic_u16string());
		};
		QGuiApplication::restoreOverrideCursor();
		//pictures.RefreshList();

		//
		// What filter has the user actually selected, or has been auto-selected?
		// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
		// 
		filterIndex = INPUTFILE_FILTERS.indexOf(fileDialog.selectedNameFilter());
		filterIndex++;
		settings.setValue("Folders/AddPictureFolder", directory);
		settings.setValue("Folders/AddPictureExtension", extension);
		settings.setValue("Folders/AddPictureIndex", filterIndex);

		//UpdateGroupTabs();
	};
	//UpdateListInfo();
}

void StackingDlg::onAddDarks()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	QSettings			settings;
	QString				directory;
	QString				extension;
	uint				filterIndex = 0;
	QString				strTitle;

	directory = settings.value("Folders/AddDarkFolder").toString();
	if (directory.isEmpty())
		directory = settings.value("Folders/AddPictureFolder").toString();

	extension = settings.value("Folders/AddDarkExtension").toString();
	if (extension.isEmpty())
		extension = settings.value("Folders/AddPictureExtension").toString();

	filterIndex = settings.value("Folders/AddDarkIndex", 0U).toUInt();
	if (!filterIndex)
		filterIndex = settings.value("Folders/AddPictureIndex", 0U).toUInt();

	if (extension.isEmpty())
		extension = "bmp";			// Note that Qt doesn't want/ignores leading . in file extensions

	fileDialog.setWindowTitle(QCoreApplication::translate("StackingDlg", "Open Dark Frames...", "IDS_TITLE_OPENDARKFRAMES"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);

	fileDialog.setNameFilters(INPUTFILE_FILTERS);
	fileDialog.selectFile(QString());		// No file(s) selected
	if (!directory.isEmpty())
		fileDialog.setDirectory(directory);

	//
	// A value of zero for filterIndex means that the user hasn't previously chosen a name filter.
	// In that case we'll choose to use the first one to start things off.
	// 
	// If the user *has* previously chosen a name filter, then that index is 1 based, not zero based.
	// This means that the index must be decremented to index into the list of name filters.
	//
	if (0 == filterIndex) filterIndex = 1;
	filterIndex = std::clamp(filterIndex, 1U, static_cast<uint>(INPUTFILE_FILTERS.size()));
	fileDialog.selectNameFilter(INPUTFILE_FILTERS.at(filterIndex - 1));

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList files = fileDialog.selectedFiles();
		for (int i = 0; i < files.size(); i++)
		{
			fs::path file(files.at(i).toStdU16String());		// as UTF-16

			imageGroups[groupId].AddFile((LPCTSTR)file.generic_wstring().c_str(),
				currentGroupId(),
				PICTURETYPE_DARKFRAME, true);
			directory = QString::fromStdU16String(file.remove_filename().generic_u16string());
			extension = QString::fromStdU16String(file.extension().generic_u16string());
		}
		QGuiApplication::restoreOverrideCursor();
		////pictures.RefreshList();

		//
		// What filter has the user actually selected, or has been auto-selected?
		// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
		// 
		filterIndex = INPUTFILE_FILTERS.indexOf(fileDialog.selectedNameFilter());
		filterIndex++;
		settings.setValue("Folders/AddDarkFolder", directory);
		settings.setValue("Folders/AddDarkExtension", extension);
		settings.setValue("Folders/AddDarkIndex", filterIndex);

		//UpdateGroupTabs();
	}
	//UpdateListInfo();
}


/* ------------------------------------------------------------------- */

void StackingDlg::onAddDarkFlats()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	QSettings			settings;
	QString				directory;
	QString				extension;
	uint				filterIndex = 0;
	QString				strTitle;

	directory = settings.value("Folders/AddDarkFlatFolder").toString();
	if (directory.isEmpty())
		directory = settings.value("Folders/AddPictureFolder").toString();

	extension = settings.value("Folders/AddDarkFlatExtension").toString();
	if (extension.isEmpty())
		extension = settings.value("Folders/AddPictureExtension").toString();

	filterIndex = settings.value("Folders/AddDarkFlatIndex", 0U).toUInt();
	if (!filterIndex)
		filterIndex = settings.value("Folders/AddPictureIndex", 0U).toUInt();

	if (extension.isEmpty())
		extension = "bmp";			// Note that Qt doesn't want/ignores leading . in file extensions

	fileDialog.setWindowTitle(QCoreApplication::translate("StackingDlg", "Open Dark Flat Frames...", "IDS_TITLE_OPENDARKFLATFRAMES"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);

	fileDialog.setNameFilters(INPUTFILE_FILTERS);
	fileDialog.selectFile(QString());		// No file(s) selected
	if (!directory.isEmpty())
		fileDialog.setDirectory(directory);

	//
	// A value of zero for filterIndex means that the user hasn't previously chosen a name filter.
	// In that case we'll choose to use the first one to start things off.
	// 
	// If the user *has* previously chosen a name filter, then that index is 1 based, not zero based.
	// This means that the index must be decremented to index into the list of name filters.
	//
	if (0 == filterIndex) filterIndex = 1;
	filterIndex = std::clamp(filterIndex, 1U, static_cast<uint>(INPUTFILE_FILTERS.size()));
	fileDialog.selectNameFilter(INPUTFILE_FILTERS.at(filterIndex - 1));

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList files = fileDialog.selectedFiles();
		for (int i = 0; i < files.size(); i++)
		{
			fs::path file(files.at(i).toStdU16String());		// as UTF-16

			imageGroups[groupId].AddFile((LPCTSTR)file.generic_wstring().c_str(),
				currentGroupId(),
				PICTURETYPE_DARKFLATFRAME, true);
			directory = QString::fromStdU16String(file.remove_filename().generic_u16string());
			extension = QString::fromStdU16String(file.extension().generic_u16string());
		}
		QGuiApplication::restoreOverrideCursor();
		//pictures.RefreshList();

		//
		// What filter has the user actually selected, or has been auto-selected?
		// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
		// 
		filterIndex = INPUTFILE_FILTERS.indexOf(fileDialog.selectedNameFilter());
		filterIndex++;
		settings.setValue("Folders/AddDarkFlatFolder", directory);
		settings.setValue("Folders/AddDarkFlatExtension", extension);
		settings.setValue("Folders/AddDarkFlatIndex", filterIndex);

		//UpdateGroupTabs();
	}
	//UpdateListInfo();
}

/* ------------------------------------------------------------------- */

void StackingDlg::onAddFlats()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	QSettings			settings;
	QString				directory;
	QString				extension;
	uint				filterIndex = 0;
	QString				strTitle;

	directory = settings.value("Folders/AddFlatFolder").toString();
	if (directory.isEmpty())
		directory = settings.value("Folders/AddPictureFolder").toString();

	extension = settings.value("Folders/AddFlatExtension").toString();
	if (extension.isEmpty())
		extension = settings.value("Folders/AddPictureExtension").toString();

	filterIndex = settings.value("Folders/AddFlatIndex", 0U).toUInt();
	if (!filterIndex)
		filterIndex = settings.value("Folders/AddPictureIndex", 0U).toUInt();

	if (extension.isEmpty())
		extension = "bmp";			// Note that Qt doesn't want/ignores leading . in file extensions

	fileDialog.setWindowTitle(QCoreApplication::translate("StackingDlg", "Open Flat Frames...", "IDS_TITLE_OPENFLATFRAMES"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);

	fileDialog.setNameFilters(INPUTFILE_FILTERS);
	fileDialog.selectFile(QString());		// No file(s) selected
	if (!directory.isEmpty())
		fileDialog.setDirectory(directory);

	//
	// A value of zero for filterIndex means that the user hasn't previously chosen a name filter.
	// In that case we'll choose to use the first one to start things off.
	// 
	// If the user *has* previously chosen a name filter, then that index is 1 based, not zero based.
	// This means that the index must be decremented to index into the list of name filters.
	//
	if (0 == filterIndex) filterIndex = 1;
	filterIndex = std::clamp(filterIndex, 1U, static_cast<uint>(INPUTFILE_FILTERS.size()));
	fileDialog.selectNameFilter(INPUTFILE_FILTERS.at(filterIndex - 1));

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList files = fileDialog.selectedFiles();
		for (int i = 0; i < files.size(); i++)
		{
			fs::path file(files.at(i).toStdU16String());		// as UTF-16

			imageGroups[groupId].AddFile((LPCTSTR)file.generic_wstring().c_str(),
				currentGroupId(),
				PICTURETYPE_FLATFRAME, true);
			directory = QString::fromStdU16String(file.remove_filename().generic_u16string());
			extension = QString::fromStdU16String(file.extension().generic_u16string());
		}
		QGuiApplication::restoreOverrideCursor();
		//pictures.RefreshList();

		//
		// What filter has the user actually selected, or has been auto-selected?
		// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
		// 
		filterIndex = INPUTFILE_FILTERS.indexOf(fileDialog.selectedNameFilter());
		filterIndex++;
		settings.setValue("Folders/AddFlatFolder", directory);
		settings.setValue("Folders/AddFlatExtension", extension);
		settings.setValue("Folders/AddFlatIndex", filterIndex);

		//UpdateGroupTabs();
	}
	//UpdateListInfo();
}


/* ------------------------------------------------------------------- */

void StackingDlg::onAddOffsets()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	QSettings			settings;
	QString				directory;
	QString				extension;
	uint				filterIndex = 0;
	QString				strTitle;

	directory = settings.value("Folders/AddOffsetFolder").toString();
	if (directory.isEmpty())
		directory = settings.value("Folders/AddPictureFolder").toString();

	extension = settings.value("Folders/AddOffsetExtension").toString();
	if (extension.isEmpty())
		extension = settings.value("Folders/AddPictureExtension").toString();

	filterIndex = settings.value("Folders/AddOffsetIndex", 0U).toUInt();
	if (!filterIndex)
		filterIndex = settings.value("Folders/AddPictureIndex", 0U).toUInt();

	if (extension.isEmpty())
		extension = "bmp";			// Note that Qt doesn't want/ignores leading . in file extensions

	fileDialog.setWindowTitle(QCoreApplication::translate("StackingDlg", "Open Bias Frames...", "IDS_TITLE_OPENBIASFRAMES"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);

	fileDialog.setNameFilters(INPUTFILE_FILTERS);
	fileDialog.selectFile(QString());		// No file(s) selected
	if (!directory.isEmpty())
		fileDialog.setDirectory(directory);

	//
	// A value of zero for filterIndex means that the user hasn't previously chosen a name filter.
	// In that case we'll choose to use the first one to start things off.
	// 
	// If the user *has* previously chosen a name filter, then that index is 1 based, not zero based.
	// This means that the index must be decremented to index into the list of name filters.
	//
	if (0 == filterIndex) filterIndex = 1;
	filterIndex = std::clamp(filterIndex, 1U, static_cast<uint>(INPUTFILE_FILTERS.size()));
	fileDialog.selectNameFilter(INPUTFILE_FILTERS.at(filterIndex - 1));

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList files = fileDialog.selectedFiles();
		for (int i = 0; i < files.size(); i++)
		{
			fs::path file(files.at(i).toStdU16String());		// as UTF-16

			imageGroups[groupId].AddFile((LPCTSTR)file.generic_wstring().c_str(),
				currentGroupId(),
				PICTURETYPE_OFFSETFRAME, true);
			directory = QString::fromStdU16String(file.remove_filename().generic_u16string());
			extension = QString::fromStdU16String(file.extension().generic_u16string());
		}
		QGuiApplication::restoreOverrideCursor();
		//pictures.RefreshList();

		//
		// What filter has the user actually selected, or has been auto-selected?
		// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
		// 
		filterIndex = INPUTFILE_FILTERS.indexOf(fileDialog.selectedNameFilter());
		filterIndex++;
		settings.setValue("Folders/AddOffsetFolder", directory);
		settings.setValue("Folders/AddOffsetExtension", extension);
		settings.setValue("Folders/AddOffsetIndex", filterIndex);

		//UpdateGroupTabs();
	}
	//UpdateListInfo();
}

bool StackingDlg::checkEditChanges()
{
	return true;
	// TODO
	#if (0)
	BOOL						bResult = FALSE;

	if (m_EditStarSink.IsDirty())
	{
		int			nResult;

		nResult = AskSaveEditChangesMode();

		if (nResult == IDYES)
		{
			// Save the changes
			bResult = TRUE;
			m_EditStarSink.SaveRegisterSettings();
			m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);
			// Update the list with the new info
			m_Pictures.UpdateItemScores(m_strShowFile);
		}
		else if (nResult == IDNO)
			bResult = TRUE;
	}
	else
		bResult = TRUE;

	return bResult;
#endif
}

size_t StackingDlg::checkedImageCount(PICTURETYPE type, int16_t group)
{
	size_t result = 0;

	// Iterate over all groups.
	for (size_t i = 0; i != imageGroups.size(); ++i)
	{
		// If the group number passed in was -1 then want to count the number of
		// checked images of the relevant type in ALL groups.  Otherwise only
		// count checked images for the passed group number.
		if (-1 == group || group == i)
		{
			for (auto it = imageGroups[i].model.cbegin();
				it != imageGroups[i].model.cend(); ++it)
			{
				if (it->m_PictureType == type && it->m_bChecked == Qt::Checked) ++result;
			}
		}
	}
	
	return result;
}

void StackingDlg::fillTasks(CAllStackingTasks& tasks)
{
	size_t				comets = 0;
	bool				bReferenceFrameHasComet = false;
	bool				bReferenceFrameSet = false;
	double				fMaxScore = -1.0;

	// Iterate over all groups.
	for (uint16_t group = 0; group != imageGroups.size(); ++group)
	{
		// and then over each image in the group
		for (auto it = imageGroups[group].model.cbegin();
			it != imageGroups[group].model.cend(); ++it)
		{
			if (it->m_bChecked == Qt::Checked)
			{
				if (it->m_bUseAsStarting)
				{
					bReferenceFrameSet = true;
					bReferenceFrameHasComet = it->m_bComet;
				}
				if (!bReferenceFrameSet && (it->m_fOverallQuality > fMaxScore))
				{
					fMaxScore = it->m_fOverallQuality;
					bReferenceFrameHasComet = it->m_bComet;
				}
				tasks.AddFileToTask(*it, group);
				if (it->m_bComet)
					comets++;
			}
		}
	}

	if (comets > 1 && bReferenceFrameHasComet)
		tasks.SetCometAvailable(true);
	tasks.ResolveTasks();
};

/* ------------------------------------------------------------------- */


void StackingDlg::clearList()
{
#if(0)
		if (checkEditChanges() && CheckWorkspaceChanges())
		{
			m_Pictures.Clear();
			m_Picture.SetImg((CBitmap*)nullptr);
			m_Picture.SetImageSink(nullptr);
			m_Picture.SetButtonToolbar(nullptr);
			m_EditStarSink.SetBitmap(nullptr);
			m_strShowFile.Empty();
			m_Infos.SetText(m_strShowFile);
			imageLoader.clearCache();
			m_LoadedImage.Clear();
			UpdateGroupTabs();
			UpdateListInfo();
			m_strCurrentFileList.Empty();
			SetCurrentFileInTitle(m_strCurrentFileList);
		}
#endif
}

void StackingDlg::loadList()
{
#if (0)
	if (CheckWorkspaceChanges())
	{
		BOOL			bOpenAnother = TRUE;

		if (m_MRUList.m_vLists.size())
		{
			CPoint				pt;
			CMenu				menu;
			CMenu* popup;
			int					nResult;
			UINT				lStartID;

			bOpenAnother = FALSE;

			menu.LoadMenu(IDR_FILELISTS);
			popup = menu.GetSubMenu(0);

			CRect				rc;

			QPoint point = QCursor::pos();
			pt.x = point.x();
			pt.y = point.y();

			lStartID = ID_FILELIST_FIRSTMRU + 1;
			for (LONG i = 0; i < m_MRUList.m_vLists.size(); i++)
			{
				TCHAR				szDrive[1 + _MAX_DRIVE];
				TCHAR				szDir[1 + _MAX_DIR];
				TCHAR				szName[1 + _MAX_FNAME];
				CString				strItem;

				_tsplitpath((LPCTSTR)m_MRUList.m_vLists[i], szDrive, szDir, szName, nullptr);
				strItem.Format(_T("%s%s%s"), szDrive, szDir, szName);

				popup->InsertMenu(ID_FILELIST_FIRSTMRU, MF_BYCOMMAND, lStartID, (LPCTSTR)strItem);
				lStartID++;
			};

			popup->DeleteMenu(ID_FILELIST_FIRSTMRU, MF_BYCOMMAND);

			nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this, nullptr);;

			if (nResult == ID_FILELIST_OPENANOTHERFILELIST)
				bOpenAnother = TRUE;
			else if (nResult > ID_FILELIST_FIRSTMRU)
			{
				CString			strList;

				strList = m_MRUList.m_vLists[nResult - ID_FILELIST_FIRSTMRU - 1];

				m_Pictures.LoadFilesFromList(strList);
				m_Pictures.RefreshList();
				m_MRUList.Add(strList);
				m_strCurrentFileList = strList;
				SetCurrentFileInTitle(m_strCurrentFileList);
			};
		};

		if (bOpenAnother)
		{
			m_Pictures.LoadList(m_MRUList, m_strCurrentFileList);
			SetCurrentFileInTitle(m_strCurrentFileList);
		};
		UpdateGroupTabs();
		UpdateListInfo();
	}
#endif
}

/* ------------------------------------------------------------------- */

void StackingDlg::saveList()
{
#if (0)
	m_Pictures.SaveList(m_MRUList, m_strCurrentFileList);
	SetCurrentFileInTitle(m_strCurrentFileList);
#endif
}



void StackingDlg::versionInfoReceived(QNetworkReply* reply)
{
	QNetworkReply::NetworkError error = reply->error();
	if (QNetworkReply::NoError == error)
	{
		QString string(reply->read(reply->bytesAvailable()));

		if (string.startsWith("DeepSkyStackerVersion="))
		{
			QString verStr = string.section('=', 1, 1);
			int version = verStr.section('.', 0, 0).toInt();
			int release = verStr.section('.', 1, 1).toInt();
			int mod = verStr.section('.', 2, 2).toInt();

			if ((version > DSSVER_MAJOR) ||
				(version == DSSVER_MAJOR && release > DSSVER_MINOR) ||
				(version == DSSVER_MAJOR && release == DSSVER_MINOR && mod > DSSVER_SUB)
				)
			{
				QString	strNewVersion(tr("DeepSkyStacker version %1 is available for download.",
										 "IDS_VERSIONAVAILABLE").arg(verStr));

				ui->information->setText(
					QString("<a href=\"https://github.com/deepskystacker/DSS/releases/latest/\" \
								style=\"color: red;\">%1</a>")
						.arg(strNewVersion));
				ui->information->setTextFormat(Qt::RichText);
				ui->information->setTextInteractionFlags(Qt::TextBrowserInteraction);
				ui->information->setOpenExternalLinks(true);
			};
		}
	}
	else
	{
		CDeepStackerDlg* pDlg = GetDeepStackerDlg(nullptr);
		CString title;
		pDlg->GetWindowText(title);
		QMessageBox::warning(nullptr, QString::fromWCharArray(title.GetString()),
			tr("Internet version check error code %1:\n%2")
			.arg(error)
			.arg(reply->errorString()), QMessageBox::Ok);

	}
	reply->deleteLater();
	networkManager->deleteLater();
};

void StackingDlg::retrieveLatestVersionInfo()
{
	//#ifndef DSSBETA
	ZFUNCTRACE_RUNTIME();

	QSettings			settings;

	bool checkVersion = settings.value("InternetCheck", false).toBool();
	if (checkVersion)
	{
		networkManager = new QNetworkAccessManager();

		QObject::connect(networkManager, &QNetworkAccessManager::finished,
			[this](QNetworkReply* reply) { this->versionInfoReceived(reply); });

		QNetworkRequest req(QUrl("https://github.com/deepskystacker/DSS/raw/release/CurrentVersion.txt"));
		req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
		networkManager->get(req);
	}
	//#endif
}

void StackingDlg::registerCheckedImages()
{
	CDSSProgressDlg			dlg;
	RegisterSettings		dlgSettings(this);
	bool					bContinue = true;

	bool					bFound = false;

	if (checkedImageCount(PICTURETYPE_LIGHTFRAME))
	{
		//CString				strFirstLightFrame;

		//m_Pictures.GetFirstCheckedLightFrame(strFirstLightFrame);

		//dlgSettings.SetForceRegister(!m_Pictures.GetNrUnregisteredCheckedLightFrames());
		//dlgSettings.SetNoDark(!m_Pictures.GetNrCheckedDarks());
		//dlgSettings.SetNoFlat(!m_Pictures.GetNrCheckedFlats());
		//dlgSettings.SetNoOffset(!m_Pictures.GetNrCheckedOffsets());
		//dlgSettings.SetFirstLightFrame(strFirstLightFrame);

		CAllStackingTasks	tasks;
		CRect				rcSelect;

		fillTasks(tasks);

		// Set the selection rectangle if needed.   It is set by Qt signal from DSSSelectRect.cpp
		if (!selectRect.isEmpty())
		{
			tasks.SetCustomRectangle(CRect(selectRect.left(), selectRect.top(), selectRect.right(), selectRect.bottom()));
		}

		dlgSettings.setStackingTasks(&tasks);

		if (dlgSettings.exec())
		{
			double				fMinLuminancy = 0.10;
			bool				bForceRegister = false;
			LONG				lCount = 0;
			LONG				lMaxCount = 0;
			double				fPercent = 20.0;
			bool				bStackAfter = false;

			bForceRegister = dlgSettings.isForceRegister();

			bStackAfter = dlgSettings.isStackAfter(fPercent);

			if (CheckReadOnlyFolders(tasks))
			{
				if (bStackAfter)
				{
					bContinue = CheckStacking(tasks);
					if (bContinue)
						bContinue = showRecap(tasks);
				}
				else
				{
					bContinue = CheckStacking(tasks);
				};

				DWORD				dwStartTime = GetTickCount();
				DWORD				dwEndTime;

				if (bContinue)
				{
					GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT);

					CRegisterEngine	RegisterEngine;

					imageLoader.clearCache();
					blankCheckedItemScores();

					bContinue = RegisterEngine.RegisterLightFrames(tasks, bForceRegister, &dlg);

					updateCheckedItemScores();
					// Update the current image score if necessary
					if (m_strShowFile.GetLength()
						&& m_Pictures.IsLightFrame(m_strShowFile)
						&& m_Pictures.IsChecked(m_strShowFile))
					{
						// Update the registering info
						m_EditStarSink.SetLightFrame(m_strShowFile);
						m_Picture.Invalidate(true);
					};

					dlg.Close();
				};

				if (bContinue && bStackAfter)
				{
					DoStacking(tasks, fPercent);
					dwEndTime = GetTickCount();
				};

				GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_STOP);
			};
		};
	}
	else
	{
		AfxMessageBox(IDS_ERROR_NOTLIGHTCHECKED2, MB_OK | MB_ICONSTOP);
	};
};

void StackingDlg::stackCheckedImages()
{
	if (checkEditChanges())
	{
		BOOL				bContinue;
		CAllStackingTasks	tasks;
		CRect				rcSelect;

		fillTasks(tasks);

		// Set the selection rectangle if needed.   It is set by Qt signal from DSSSelectRect.cpp
		if (!selectRect.isEmpty())
		{
			tasks.SetCustomRectangle(CRect(selectRect.left(), selectRect.top(), selectRect.right(), selectRect.bottom()));
		}

		if (CheckReadOnlyFolders(tasks))
		{
			bContinue = CheckStacking(tasks);
			if (bContinue)
				bContinue = showRecap(tasks);
			if (bContinue)
			{
				GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT);

				imageLoader.clearCache();
				if (m_Pictures.GetNrUnregisteredCheckedLightFrames())
				{
					CRegisterEngine	RegisterEngine;
					CDSSProgressDlg	dlg;

					m_Pictures.BlankCheckedItemScores();
					bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
					m_Pictures.updateCheckedItemScores();
					dlg.Close();
				};

				if (bContinue)
					DoStacking(tasks);

				GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_STOP);
			};
		};
	};
};

/* ------------------------------------------------------------------- */

bool StackingDlg::CheckReadOnlyFolders(CAllStackingTasks& tasks)
{
	bool					bResult = TRUE;
	std::vector<CString>	vFolders;

	if (!tasks.CheckReadOnlyStatus(vFolders))
	{
		CString				strText;
		CString				strFolders;

		for (LONG i = 0; i < vFolders.size(); i++)
		{
			strFolders += vFolders[i];
			strFolders += "\n";
		};

		strText.Format(IDS_WARNINGREADONLY, strFolders);

		AfxMessageBox(strText, MB_OK | MB_ICONSTOP);
		bResult = FALSE;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool StackingDlg::CheckStacking(CAllStackingTasks& tasks)
{
	bool				bResult = FALSE;

	if (!areCheckedImagesCompatible())
		AfxMessageBox(IDS_ERROR_NOTCOMPATIBLE, MB_OK | MB_ICONSTOP);
	else if (!checkedImageCount(PICTURETYPE_LIGHTFRAME))
		AfxMessageBox(IDS_ERROR_NOTLIGHTCHECKED, MB_OK | MB_ICONSTOP);
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */

bool StackingDlg::areCheckedImagesCompatible()
{
	bool				bResult = true;
	bool				bFirst = true;
	const ListBitMap*	lb;
	LONG				lNrDarks = 0;
	LONG				lNrDarkFlats = 0;
	LONG				lNrFlats = 0;
	LONG				lNrOffsets = 0;
	bool				bMasterDark = false;
	bool				bMasterFlat = false;
	bool				bMasterDarkFlat = false;
	bool				bMasterOffset = false;

	// Iterate over all groups.
	for (uint16_t group = 0; group != imageGroups.size(); ++group)
	{
		// and then over each image in the group
		for (auto it = imageGroups[group].model.cbegin();
			it != imageGroups[group].model.cend(); ++it)
		{
			if (it->m_bChecked == Qt::Checked)
			{
				if (bFirst)
				{
					lb = &(*it);
					bFirst = false;
				}
				else
					bResult = lb->IsCompatible(*it);
			}
		}
	}

	if (bResult)
	{
		if (bMasterDark && lNrDarks > 1)
			bResult = false;
		if (bMasterDarkFlat && lNrDarkFlats > 1)
			bResult = false;
		if (bMasterFlat && lNrFlats > 1)
			bResult = false;
		if (bMasterOffset && lNrOffsets > 1)
			bResult = false;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

bool StackingDlg::showRecap(CAllStackingTasks& tasks)
{
	StackRecap	dlg(this);

	dlg.setStackingTasks(&tasks);
	return dlg.exec();
};


void StackingDlg::blankCheckedItemScores()
{
	// Iterate over all groups.
	for (uint16_t group = 0; group != imageGroups.size(); ++group)
	{
		// and then over each image in the group
		for (auto it = imageGroups[group].model.begin();
			it != imageGroups[group].model.end(); ++it)
		{
			if (it->m_bChecked == Qt::Checked && it->IsLightFrame())
			{
				it->m_bRegistered = false;
			}
		}
	}

};

/* ------------------------------------------------------------------- */

void StackingDlg::updateCheckedItemScores()
{
	int row = 0;

	// Iterate over all groups.
	for (uint16_t group = 0; group != imageGroups.size(); ++group)
	{
		// and then over each image in the group
		for (auto it = imageGroups[group].model.begin();
			it != imageGroups[group].model.end(); ++it)
		{
			if (it->m_bChecked == Qt::Checked &&
				it->IsLightFrame())
			{
				CLightFrameInfo		bmpInfo;

				bmpInfo.SetBitmap(it->m_strFileName, false, false);

				//
				// Update list information, but beware that you must use setData() for any of the columns
				// that are defined in the DSS::ImageListModel::Column enumeration as they used for the 
				// QTableView.   If this isn't done, the image list view won't get updated.
				//
				// The "Sky Background" (Column::BackgroundCol) is a special case it's a class, not a primitive, so the model 
				// class has a specific member function to set that.
				//
				// Other member of ListBitMap (e.g.) m_bRegistered and m_bComet can be updated directly.
				//
				if (bmpInfo.m_bInfoOk)
				{
					it->m_bRegistered = true;
					imageGroups[group].model.setData(row, DSS::ImageListModel::Column::ScoreCol, bmpInfo.m_fOverallQuality);
					imageGroups[group].model.setData(row, DSS::ImageListModel::Column::FWHMCol, bmpInfo.m_fFWHM);
					imageGroups[group].model.setData(row, DSS::ImageListModel::Column::StarsCol, (int)bmpInfo.m_vStars.size());
					it->m_bComet = bmpInfo.m_bComet;
					imageGroups[group].model.setData(row, DSS::ImageListModel::Column::BackgroundCol, (int)bmpInfo.m_vStars.size());
					imageGroups[group].model.setSkyBackground(row, bmpInfo.m_SkyBackground);

				}
				else
				{
					it->m_bRegistered = false;
				};
			};
			++row;
		};
	}

};


/* ------------------------------------------------------------------- */

#if (0)
CStackingDlg::CStackingDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CStackingDlg::IDD, pParent),
	m_cCtrlCache(this)
{
	//{{AFX_DATA_INIT(CStackingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_MRUList.readSettings();
}


/* ------------------------------------------------------------------- */

void CStackingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStackingDlg)
	DDX_Control(pDX, IDC_PICTURES, m_Pictures);
	DDX_Control(pDX, IDC_PICTURE, m_PictureStatic);
	DDX_Control(pDX, IDC_INFOS, m_Infos);
	DDX_Control(pDX, IDC_LISTINFO, m_ListInfo);
	DDX_Control(pDX, IDC_GAMMA, m_Gamma);
	DDX_Control(pDX, IDC_GROUPTAB, m_GroupTab);
	DDX_Control(pDX, IDC_SHOWHIDEJOBS, m_ShowHideJobs);
	DDX_Control(pDX, IDC_4CORNERS, m_4Corners);
	//}}AFX_DATA_MAP
}

/* ------------------------------------------------------------------- */

BEGIN_MESSAGE_MAP(CStackingDlg, CDialog)
	//{{AFX_MSG_MAP(CStackingDlg)
	ON_NOTIFY(NM_CLICK, IDC_PICTURES, OnClickPictures)
	ON_NOTIFY(NM_NOTIFYMODECHANGE, IDC_PICTURE, OnPictureChange)
	ON_MESSAGE(WM_CHECKITEM, OnCheckItem)
	ON_MESSAGE(WM_SELECTITEM, OnSelectItem)
	ON_WM_SIZE()
	ON_NOTIFY(GC_PEGMOVE, IDC_GAMMA, OnChangeGamma)
	ON_NOTIFY(GC_PEGMOVED, IDC_GAMMA, OnChangeGamma)
	ON_NOTIFY(CTCN_SELCHANGE, IDC_GROUPTAB, OnSelChangeGroup)
	ON_NOTIFY(CTCN_SELCHANGE, IDC_JOBTAB, OnSelChangeJob)
	ON_NOTIFY(NM_LINKCLICK, IDC_SHOWHIDEJOBS, OnShowHideJobs)
	ON_NOTIFY(SPN_SIZED, IDC_SPLITTER, OnSplitter)
//}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_4CORNERS, &CStackingDlg::OnBnClicked4corners)
END_MESSAGE_MAP()

/* ------------------------------------------------------------------- */
/////////////////////////////////////////////////////////////////////////////
// CStackingDlg message handlers

BOOL CStackingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	m_ListInfo.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
	m_Picture.CreateFromStatic(&m_PictureStatic);

	{
		CRect				rc;

		GetDlgItem(IDC_SPLITTER)->GetWindowRect(rc);
		ScreenToClient(rc);
		m_Splitter.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER);
	};

	// Add controls to the control cache - this is just a container for helping calcualte sizes and
	// positions when resizing the dialog.
	m_cCtrlCache.AddToCtrlCache(IDC_INFOS);
	m_cCtrlCache.AddToCtrlCache(IDC_4CORNERS);
	m_cCtrlCache.AddToCtrlCache(IDC_GAMMA);
	m_cCtrlCache.AddToCtrlCache(IDC_PICTURE);
	m_cCtrlCache.AddToCtrlCache(IDC_SPLITTER);
	m_cCtrlCache.AddToCtrlCache(IDC_LISTINFO);
	m_cCtrlCache.AddToCtrlCache(IDC_PICTURES);
	m_cCtrlCache.AddToCtrlCache(IDC_GROUPTAB);

	m_Pictures.Initialize();
	m_Picture.SetBltMode(CWndImage::bltFitXY);
	m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);
	CString				strTooltip;

	strTooltip.LoadString(IDS_TOOLTIP_SELECTRECT);
	m_ButtonToolbar.AddCheck(IDC_EDIT_SELECT,	MBI(SELECT), IDB_BUTTONBASE_MASK, strTooltip);
	strTooltip.LoadString(IDS_TOOLTIP_STAR);
	m_ButtonToolbar.AddCheck(IDC_EDIT_STAR,	MBI(STAR), IDB_BUTTONBASE_MASK, strTooltip);
	strTooltip.LoadString(IDS_TOOLTIP_COMET);
	m_ButtonToolbar.AddCheck(IDC_EDIT_COMET,	MBI(COMET), IDB_BUTTONBASE_MASK, strTooltip);
	strTooltip.LoadString(IDS_TOOLTIP_SAVE);
	m_ButtonToolbar.AddButton(IDC_EDIT_SAVE,	MBI(SAVE), IDB_BUTTONBASE_MASK, strTooltip);

	m_ButtonToolbar.Check(IDC_EDIT_SELECT);
	m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);

	m_Picture.EnableZoom(TRUE);
	m_Picture.SetButtonToolbar(&m_ButtonToolbar);
	m_SelectRectSink.ShowDrizzleRectangles();
	m_Picture.SetImageSink(&m_SelectRectSink);
	m_ButtonToolbar.SetSink(this);

	m_Gamma.SetBackgroundColor(GetSysColor(COLOR_3DFACE));
	m_Gamma.ShowTooltips(FALSE);
	m_Gamma.SetOrientation(CGradientCtrl::ForceHorizontal);
	m_Gamma.SetPegSide(TRUE, FALSE);
	m_Gamma.SetPegSide(FALSE, TRUE);
	m_Gamma.GetGradient().SetStartPegColour(RGB(0, 0, 0));
	m_Gamma.GetGradient().AddPeg(RGB(0, 0, 0), 0.0, 0);
	m_Gamma.GetGradient().AddPeg(RGB(128, 128, 128), sqrt(0.5), 1);
	m_Gamma.GetGradient().AddPeg(RGB(255, 255, 255), 1.0, 2);
	m_Gamma.GetGradient().SetEndPegColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetBackgroundColour(RGB(255, 255, 255));
	m_Gamma.GetGradient().SetInterpolationMethod(CGradient::Linear);


	m_4Corners.SetBitmaps(IDB_4CORNERS, RGB(255,0, 255));
	m_4Corners.SetFlat(TRUE);
	//m_4Corners.DrawTransparent(TRUE);

	QSettings			settings;
	bool checkVersion = settings.value("InternetCheck", false).toBool();
	if (checkVersion)
		retrieveLatestVersionInfo();   // will update ui asynchronously


	{
		m_GroupTab.ModifyStyle(0, CTCS_AUTOHIDEBUTTONS | CTCS_TOOLTIPS, 0);

		UpdateGroupTabs();
	};

/*
	m_ShowHideJobs.SetLink(TRUE, TRUE);
	m_ShowHideJobs.SetTransparent(TRUE);
	m_ShowHideJobs.SetLinkCursor(LoadCursor(nullptr,MAKEINTRESOURCE(IDC_HAND)));
	m_ShowHideJobs.SetFont3D(FALSE);
	m_ShowHideJobs.SetTextColor(RGB(0, 0, 192));
	m_ShowHideJobs.SetWindowText("Show/Hide Jobs");
	{
		m_JobTab.ModifyStyle(0, CTCS_RIGHT | CTCS_AUTOHIDEBUTTONS | CTCS_TOOLTIPS, 0);
		m_JobTab.InsertItem(0, "Main Job");
		m_JobTab.InsertItem(1, "Red");
		m_JobTab.InsertItem(2, "Green");
		m_JobTab.InsertItem(3, "Blue");
		m_JobTab.InsertItem(4, "Lum");
	};*/

	if (m_strStartingFileList.GetLength())
		OpenFileList(m_strStartingFileList);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateLayout()
{
	// No controls present, nothing to do!
	if (GetDlgItem(IDC_PICTURE) == nullptr)
		return;

	// Update the cache so all the sizes and positions are correct.
	m_cCtrlCache.UpdateCtrlCache();

	CRect rcCurrentDlgSize;
	GetClientRect(rcCurrentDlgSize);

	// Cache the controls that we can scale to make things fit.
	// Work out vertical space change.
	int nCtrlHeightSum = 0;
	const int nTopSpacing = min(m_cCtrlCache.GetCtrlOffset(IDC_LISTINFO).y, (min(m_cCtrlCache.GetCtrlOffset(IDC_GAMMA).y, m_cCtrlCache.GetCtrlOffset(IDC_4CORNERS).y)));
	nCtrlHeightSum += nTopSpacing;
	nCtrlHeightSum += max(m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Height(), (max(m_cCtrlCache.GetCtrlSize(IDC_GAMMA).Height(), m_cCtrlCache.GetCtrlSize(IDC_4CORNERS).Height())));

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_SPLITTER).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_SPLITTER).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_LISTINFO).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_PICTURES).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height();

	nCtrlHeightSum += m_cCtrlCache.GetCtrlOffset(IDC_GROUPTAB).y - nCtrlHeightSum;
	nCtrlHeightSum += m_cCtrlCache.GetCtrlSize(IDC_GROUPTAB).Height();

	nCtrlHeightSum += nTopSpacing;

	// Preferentially scale the picture first, then the list afterwards (if possible)
	int nDiffPictureY = rcCurrentDlgSize.Height() - nCtrlHeightSum;
	int nDiffListY = 0;

	// Handle if there isn't enough space to handle the picture resizing alone.
	if (m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height() + nDiffPictureY <= sm_nMinImageHeight)
	{
		int nMaxMovement = m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height() - sm_nMinImageHeight;
		if (nMaxMovement <= 0)
		{
			nDiffListY = nDiffPictureY;
			nDiffPictureY = 0;
		}
		else
		{
			nDiffListY = nDiffPictureY + nMaxMovement;
			nDiffPictureY = -nMaxMovement;
		}
		// Handle if there isn't enough space to handle the list resizing as well.
		if (m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height() + nDiffListY <= sm_nMinListHeight)
		{
			int nMaxMovement = m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height() - sm_nMinListHeight;
			if (nMaxMovement <= 0)
				nDiffListY = 0;
			else
				nDiffListY = -nMaxMovement;
		}
	}

	// Perform the resizing and moving of the controls.
	if (nDiffPictureY != 0)
	{
		m_cCtrlCache.SizeCtrlVert(IDC_PICTURE, m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Height() + nDiffPictureY);
		m_Splitter.ChangePos(&m_Splitter, 0, nDiffPictureY);
		m_cCtrlCache.MoveCtrlVert(IDC_LISTINFO, nDiffPictureY);
		m_cCtrlCache.MoveCtrlVert(IDC_PICTURES, nDiffPictureY);
		m_cCtrlCache.MoveCtrlVert(IDC_GROUPTAB, nDiffPictureY);
	}
	if (nDiffListY != 0)
	{
		m_cCtrlCache.SizeCtrlVert(IDC_PICTURES, m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Height() + nDiffListY);
		m_cCtrlCache.MoveCtrlVert(IDC_GROUPTAB, nDiffListY);
	}

	// Now look at the widths
	int nCtrlWidthSum = m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).x + m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() + m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).x; // Assume same padding at either end.
	int nDiffX = rcCurrentDlgSize.Width() - nCtrlWidthSum;
	if (m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() + nDiffX <= sm_nMinListWidth)
	{
		int nMaxMovement = m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() - sm_nMinListWidth;
		if (nMaxMovement <= 0)
			nDiffX = 0;
		else
			nDiffX = -nMaxMovement;
	}
	if (nDiffX)
	{
		m_cCtrlCache.SizeCtrlHoriz(IDC_INFOS, m_cCtrlCache.GetCtrlSize(IDC_INFOS).Width() + nDiffX);
		m_cCtrlCache.MoveCtrlHoriz(IDC_4CORNERS, nDiffX);
		m_cCtrlCache.MoveCtrlHoriz(IDC_GAMMA, nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_PICTURE, m_cCtrlCache.GetCtrlSize(IDC_PICTURE).Width() + nDiffX);
		m_Splitter.ChangeWidth(&m_Splitter, nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_LISTINFO, m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Width() + nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_PICTURES, m_cCtrlCache.GetCtrlSize(IDC_PICTURES).Width() + nDiffX);
		m_cCtrlCache.SizeCtrlHoriz(IDC_GROUPTAB, m_cCtrlCache.GetCtrlSize(IDC_GROUPTAB).Width() + nDiffX);
	}

	// Because we've resized things, we need to update the max splitter range accordingly.
	// This is not quite right - couple of pixels out - but not sure why.
	int nMinY = m_cCtrlCache.GetCtrlOffset(IDC_PICTURE).y + sm_nMinImageHeight;
	int nMaxY = std::max(nMinY + m_cCtrlCache.GetCtrlSize(IDC_SPLITTER).Height(), rcCurrentDlgSize.Height() - (sm_nMinListHeight + m_cCtrlCache.GetCtrlSize(IDC_GROUPTAB).Height() + m_cCtrlCache.GetCtrlSize(IDC_LISTINFO).Height() + m_cCtrlCache.GetCtrlSize(IDC_SPLITTER).Height()));
	m_Splitter.SetRange(nMinY, nMaxY);

	// Update everything.
	Invalidate();
	UpdateWindow();
	m_cCtrlCache.InvalidateCtrls();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateGroupTabs()
{
	int dwLastGroupID = static_cast<int>(m_Pictures.GetLastGroupID());
	if (m_Pictures.GetNrFrames(dwLastGroupID) != 0)
		dwLastGroupID++;

	const auto lCurrentGroup = std::max(0, m_GroupTab.GetCurSel());

	m_GroupTab.DeleteAllItems();

	CString				strGroup;

	strGroup.LoadString(IDS_MAINGROUP);

	m_GroupTab.InsertItem(0, strGroup);

	strGroup.LoadString(IDS_GROUPIDMASK);

	for (int i = 1; i <= dwLastGroupID; i++)
	{
		CString			strName;

		strName.Format(strGroup, i);
		m_GroupTab.InsertItem(i, strName);
	};

	if (lCurrentGroup > dwLastGroupID)
	{
		m_GroupTab.SetCurSel(0);
		m_Pictures.SetCurrentGroupID(0);
	}
	else
		m_GroupTab.SetCurSel(lCurrentGroup);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnChangeGamma(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pResult)
		*pResult = 1;

	PegNMHDR *			pPegNMHDR = (PegNMHDR*)pNMHDR;
	double				fBlackPoint,
						fGrayPoint,
						fWhitePoint;

	if ((pPegNMHDR->nmhdr.code == GC_PEGMOVE) ||
		(pPegNMHDR->nmhdr.code == GC_PEGMOVED))
	{
		// Adjust
		CGradient &			Gradient = m_Gamma.GetGradient();
		fBlackPoint = Gradient.GetPeg(Gradient.IndexFromId(0)).position;
		fGrayPoint  = Gradient.GetPeg(Gradient.IndexFromId(1)).position;
		fWhitePoint = Gradient.GetPeg(Gradient.IndexFromId(2)).position;
		BOOL				bAdjust = FALSE;

		switch (pPegNMHDR->peg.id)
		{
		case 0 :
			// Black point moving
			if (fBlackPoint>fWhitePoint-0.02)
			{
				fBlackPoint = fWhitePoint-0.02;
				bAdjust = TRUE;
			};
			if (fBlackPoint>fGrayPoint-0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = TRUE;
			};
			break;
		case 1 :
			// Gray point moving
			if (fGrayPoint<fBlackPoint+0.01)
			{
				fGrayPoint = fBlackPoint+0.01;
				bAdjust = TRUE;
			};
			if (fGrayPoint>fWhitePoint-0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = TRUE;
			};
			break;
		case 2 :
			// White point moving
			if (fWhitePoint<fBlackPoint+0.02)
			{
				fWhitePoint = fBlackPoint+0.02;
				bAdjust = TRUE;
			};
			if (fWhitePoint < fGrayPoint+0.01)
			{
				fGrayPoint = fWhitePoint-0.01;
				bAdjust = TRUE;
			};
			break;
		};
		if (bAdjust)
		{
			Gradient.SetPeg(Gradient.IndexFromId(0), (float)fBlackPoint);
			Gradient.SetPeg(Gradient.IndexFromId(1), (float)fGrayPoint);
			Gradient.SetPeg(Gradient.IndexFromId(2), (float)fWhitePoint);
			m_Gamma.InvalidateRect(nullptr);
		};
	};

	if (pPegNMHDR->nmhdr.code == GC_PEGMOVED)
	{
		// Adjust Gamma
		m_GammaTransformation.InitTransformation(fBlackPoint*fBlackPoint, fGrayPoint*fGrayPoint, fWhitePoint*fWhitePoint);

		if (m_LoadedImage.m_hBitmap)
		{
			ApplyGammaTransformation(m_LoadedImage.m_hBitmap, m_LoadedImage.m_pBitmap, m_GammaTransformation);
			// Refresh
			m_Picture.Invalidate(TRUE);
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckDiskSpace(CAllStackingTasks & tasks)
{
	BOOL				bResult = FALSE;
	__int64				ulFlatSpace = 0,
						ulDarkSpace = 0,
						ulOffsetSpace = 0;
	__int64				ulNeededSpace = 0;

	for (size_t i = 0; i < tasks.m_vStacks.size(); i++)
	{
		int			lWidth,
						lHeight,
						lNrChannels,
						lNrBytesPerChannel;
		__int64			ulSpace;

		lWidth		= tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lWidth;
		lHeight		= tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lHeight;
		lNrChannels = tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lNrChannels;
		lNrBytesPerChannel = tasks.m_vStacks[i].m_pLightTask->m_vBitmaps[0].m_lBitPerChannels/8;

		ulSpace		= lWidth * lHeight * lNrBytesPerChannel * lNrChannels;

		if (tasks.m_vStacks[i].m_pOffsetTask)
			ulOffsetSpace = max(ulOffsetSpace, static_cast<__int64>(ulSpace * tasks.m_vStacks[i].m_pOffsetTask->m_vBitmaps.size()));

		if (tasks.m_vStacks[i].m_pDarkTask)
			ulDarkSpace = max(ulDarkSpace, static_cast<__int64>(ulSpace * tasks.m_vStacks[i].m_pDarkTask->m_vBitmaps.size()));

		if (tasks.m_vStacks[i].m_pFlatTask)
			ulFlatSpace = max(ulFlatSpace, static_cast<__int64>(ulSpace * tasks.m_vStacks[i].m_pFlatTask->m_vBitmaps.size()));
	};

	ulNeededSpace = max(ulFlatSpace, max(ulOffsetSpace, ulDarkSpace));
	ulNeededSpace += ulNeededSpace / 10;

	// Get available space from drive
	TCHAR			szTempPath[1+_MAX_PATH];

	szTempPath[0] = 0;
	GetTempPath(sizeof(szTempPath)/sizeof(szTempPath[0]), szTempPath);

	ULARGE_INTEGER			ulFreeSpace;
	ULARGE_INTEGER			ulTotal;
	ULARGE_INTEGER			ulTotalFree;

	GetDiskFreeSpaceEx(szTempPath, &ulFreeSpace, &ulTotal, &ulTotalFree);

	if (ulFreeSpace.QuadPart < ulNeededSpace)
	{
		ulFreeSpace.QuadPart /= 1024;
		ulNeededSpace /= 1024;

		int			lNeededSpace = static_cast<int>(ulNeededSpace);
		auto			lFreeSpace = ulFreeSpace.LowPart;
		CString			strDrive;

		strDrive = szTempPath;
		strDrive = strDrive.Left(2);

		CString			strMessage;

		strMessage.Format(IDS_ERROR_NOTENOUGHFREESPACE, lNeededSpace, lFreeSpace, strDrive);
		if (AfxMessageBox(strMessage, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)==IDYES)
			bResult = TRUE;
		else
			bResult = FALSE;
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

void CStackingDlg::OnAddpictures()
{
	ZFUNCTRACE_RUNTIME();
	QFileDialog			fileDialog;
	QSettings			settings;
	QString				directory;
	QString				extension;
	uint				filterIndex = 0;
	QString				strTitle;

	directory = settings.value("Folders/AddPictureFolder").toString();

	extension = settings.value("Folders/AddPictureExtension").toString();

	filterIndex = settings.value("Folders/AddPictureIndex", 0U).toUInt();
	
	if (extension.isEmpty())
		extension = "bmp";			// Note that Qt doesn't want/ignores leading . in file extensions

	fileDialog.setWindowTitle(QCoreApplication::translate("StackingDlg", "Open Light Frames...", "IDS_TITLE_OPENLIGHTFRAMES"));
	fileDialog.setDefaultSuffix(extension);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);

	const QStringList filters({
		QCoreApplication::translate("StackingDlg", "Picture Files (*.bmp *.jpg *.jpeg *.tif *.tiff *.png *.fit *.fits *.fts *.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)"),
		QCoreApplication::translate("StackingDlg", "Windows Bitmaps (*.bmp)"),
		QCoreApplication::translate("StackingDlg", "JPEG or PNG Files (*.jpg *.jpeg *.png)"),
		QCoreApplication::translate("StackingDlg", "TIFF Files (*.tif *.tiff)"),
		QCoreApplication::translate("StackingDlg", "RAW Files (*.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)"),
		QCoreApplication::translate("StackingDlg", "FITS Files (*.fits *.fit *.fts)"),
		QCoreApplication::translate("StackingDlg", "All Files (*)")
		});

	fileDialog.setNameFilters(filters);
	fileDialog.selectFile(QString());		// No file(s) selected
	if (!directory.isEmpty())
		fileDialog.setDirectory(directory);

	//
	// A value of zero for filterIndex means that the user hasn't previously chosen a name filter.
	// In that case we'll choose to use the first one to start things off.
	// 
	// If the user *has* previously chosen a name filter, then that index is 1 based, not zero based.
	// This means that the index must be decremented to index into the list of name filters.
	//
	if (0 == filterIndex) filterIndex = 1;
	filterIndex = std::clamp(filterIndex, 1U, static_cast<uint>(filters.size()));
	fileDialog.selectNameFilter(filters.at(filterIndex - 1));

	ZTRACE_RUNTIME("About to show file open dlg");
	if (QDialog::Accepted == fileDialog.exec())
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QStringList files = fileDialog.selectedFiles();
		for (int i = 0; i < files.size(); i++)
		{
			fs::path file(files.at(i).toStdU16String());		// as UTF-16

			m_Pictures.AddFile((LPCTSTR)file.generic_wstring().c_str(),
				m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID());
			directory = QString::fromStdU16String(file.remove_filename().generic_u16string());
			extension = QString::fromStdU16String(file.extension().generic_u16string());
		};
		QGuiApplication::restoreOverrideCursor();
		m_Pictures.RefreshList();

		//
		// What filter has the user actually selected, or has been auto-selected?
		// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
		// 
		filterIndex = filters.indexOf(fileDialog.selectedNameFilter());
		filterIndex++;		
		settings.setValue("Folders/AddPictureFolder", directory);
		settings.setValue("Folders/AddPictureExtension", extension);
		settings.setValue("Folders/AddPictureIndex", filterIndex);

		UpdateGroupTabs();
	};
	UpdateListInfo();
}


/* ------------------------------------------------------------------- */

void CStackingDlg::DropFiles(HDROP hDropInfo)
{
	CDropFilesDlg			dlg;

	dlg.SetDropInfo(hDropInfo);
	if (dlg.DoModal() == IDOK)
	{
		std::vector<CString>	vFiles;

		BeginWaitCursor();
		dlg.GetDroppedFiles(vFiles);

		for (size_t i = 0; i < vFiles.size(); i++)
			m_Pictures.AddFile(vFiles[i], m_Pictures.GetCurrentGroupID(), m_Pictures.GetCurrentJobID(), dlg.GetDropType(), TRUE);

		m_Pictures.RefreshList();
		UpdateGroupTabs();
		UpdateListInfo();
		EndWaitCursor();
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OpenFileList(LPCTSTR szFileList)
{
	CString					strList = szFileList;

	// Check that the file can be opened
	FILE *					hFile;

	hFile = _tfopen(strList, _T("rt"));
	if  (hFile)
	{
		fclose(hFile);
		SetCursor(::LoadCursor(nullptr, IDC_WAIT));
		m_Pictures.LoadFilesFromList(strList);
		m_Pictures.RefreshList();
		m_MRUList.Add(strList);
		UpdateGroupTabs();
		UpdateListInfo();
		m_strCurrentFileList = szFileList;
		SetCurrentFileInTitle(m_strCurrentFileList);
		SetCursor(::LoadCursor(nullptr, IDC_ARROW));
	};
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

void CStackingDlg::OnPictureChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	// Here check if the new image is dirty
	if (m_EditStarSink.IsDirty())
		m_ButtonToolbar.Enable(IDC_EDIT_SAVE, TRUE);
};

/* ------------------------------------------------------------------- */

CWndImageSink *	CStackingDlg::GetCurrentSink()
{
	if (m_ButtonToolbar.IsChecked(IDC_EDIT_STAR))
		return &m_EditStarSink;
	else if (m_ButtonToolbar.IsChecked(IDC_EDIT_COMET))
		return &m_EditStarSink;
	else if (m_ButtonToolbar.IsChecked(IDC_EDIT_SELECT))
		return &m_SelectRectSink;

	return nullptr;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAskRegister()
{
	// Check that the current light frame is registered (or not)
	// and ask accordingly
	CLightFrameInfo			lfi;

	lfi.SetBitmap(m_strShowFile, FALSE, FALSE);
	if (!lfi.IsRegistered())
	{
		CAskRegistering		dlg;

		if (dlg.DoModal()==IDOK)
		{
			if (dlg.GetAction()==ARA_ONE)
			{
				// Register only this light frame
				m_Pictures.CheckAllLights(FALSE);
				m_Pictures.CheckImage(m_strShowFile, TRUE);
				RegisterCheckedImage();
			}
			else if (dlg.GetAction()==ARA_ALL)
			{
				// Register all the checked light frames
				m_Pictures.CheckImage(m_strShowFile, TRUE);
				RegisterCheckedImage();
			};
		};
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ButtonToolbar_OnCheck(DWORD dwID, CButtonToolbar * pButtonToolbar)
{
	switch (dwID)
	{
	case IDC_EDIT_COMET :
		if (pButtonToolbar->IsChecked(dwID))
		{
			CheckAskRegister();
			pButtonToolbar->Check(IDC_EDIT_STAR, FALSE);
			pButtonToolbar->Check(IDC_EDIT_SELECT, FALSE);
			m_EditStarSink.SetCometMode(TRUE);
			m_Picture.SetImageSink(&m_EditStarSink);
		};
		break;
	case IDC_EDIT_STAR :
		if (pButtonToolbar->IsChecked(dwID))
		{
			CheckAskRegister();
			pButtonToolbar->Check(IDC_EDIT_COMET, FALSE);
			pButtonToolbar->Check(IDC_EDIT_SELECT, FALSE);
			m_EditStarSink.SetCometMode(FALSE);
			m_Picture.SetImageSink(&m_EditStarSink);
		};
		break;
	case IDC_EDIT_SELECT :
		if (pButtonToolbar->IsChecked(dwID))
		{
			pButtonToolbar->Check(IDC_EDIT_COMET, FALSE);
			pButtonToolbar->Check(IDC_EDIT_STAR, FALSE);
			m_Picture.SetImageSink(&m_SelectRectSink);
		};
		break;
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ButtonToolbar_OnClick(DWORD dwID, CButtonToolbar * pButtonToolbar)
{
	if (dwID == IDC_EDIT_SAVE)
	{
		m_EditStarSink.SaveRegisterSettings();
		m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);
		// Update the list with the new info
		m_Pictures.UpdateItemScores(m_strShowFile);
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ButtonToolbar_OnRClick(DWORD dwID, CButtonToolbar * pButtonToolbar)
{
	if (dwID == IDC_EDIT_SAVE)
	{
		// Open the popup menu to maybe change the mode
		CPoint				pt;
		CMenu				menu;
		CMenu *				popup;
		int					nResult;

		GetCursorPos(&pt);
		menu.LoadMenu(IDR_SAVECONTEXT);
		popup = menu.GetSubMenu(0);

		switch (GetSaveEditMode())
		{
		case SECM_SAVEDONTASK :
			popup->CheckMenuItem(ID_SAVECONTEXT_SAVEWITHOUTASKING, MF_BYCOMMAND | MF_CHECKED);
			break;
		case SECM_DONTSAVEDONTASK :
			popup->CheckMenuItem(ID_SAVECONTEXT_DONTSAVEWITHOUTASKING, MF_BYCOMMAND | MF_CHECKED);
			break;
		case SECM_ASKALWAYS :
			popup->CheckMenuItem(ID_SAVECONTEXT_ASKALWAYS, MF_BYCOMMAND | MF_CHECKED);
			break;
		};
		nResult = popup->TrackPopupMenuEx(TPM_NONOTIFY | TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, this, nullptr);

		if (nResult == ID_SAVECONTEXT_SAVEWITHOUTASKING)
			SetSaveEditMode(SECM_SAVEDONTASK);
		else if (nResult == ID_SAVECONTEXT_DONTSAVEWITHOUTASKING)
			SetSaveEditMode(SECM_DONTSAVEDONTASK);
		else if (nResult == ID_SAVECONTEXT_ASKALWAYS)
			SetSaveEditMode(SECM_ASKALWAYS);
	};
};

/* ------------------------------------------------------------------- */

void	CStackingDlg::UpdateListInfo()
{
	CString					strText;

	strText.Format(IDS_LISTINFO,
		checkedImageCount(PICTURETYPE_LIGHTFRAME),
		checkedImageCount(PICTURETYPE_DARKFRAME),
		checkedImageCount(PICTURETYPE_FLATFRAME),
		checkedImageCount(PICTURETYPE_DARKFLATFRAME),
		checkedImageCount(PICTURETYPE_OFFSETFRAME));

	m_ListInfo.SetText(strText);

	for (int i = 0; i < m_GroupTab.GetItemCount(); i++)
	{
		strText.Format(IDS_LISTINFO2,
			checkedImageCount(PICTURETYPE_LIGHTFRAME, i),
			checkedImageCount(PICTURETYPE_DARKFRAME, i),
			checkedImageCount(PICTURETYPE_FLATFRAME, i),
			checkedImageCount(PICTURETYPE_DARKFLATFRAME, i),
			checkedImageCount(PICTURETYPE_OFFSETFRAME, i));
		m_GroupTab.SetItemTooltipText(i, strText);
	};

};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::CheckWorkspaceChanges()
{
	BOOL						bResult = FALSE;

	if (m_strCurrentFileList.GetLength() || m_Pictures.GetNrFrames())
	{
		CWorkspace				workspace;

		if (m_Pictures.IsDirty() || workspace.isDirty())
		{
			// Ask for saving the current list.
			CString				strText;
			int					nResult;

			strText.Format(IDS_WARNING_SAVECHANGES, (LPCTSTR)m_strCurrentFileList);
			nResult = AfxMessageBox(strText, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);

			if (nResult == IDYES)
			{
				if (m_strCurrentFileList.GetLength())
					m_Pictures.SaveListToFile(m_strCurrentFileList);
				else
					m_Pictures.SaveList(m_MRUList, m_strCurrentFileList);
				SetCurrentFileInTitle(m_strCurrentFileList);
			};

			bResult = (nResult != IDCANCEL);
		}
		else
			bResult = TRUE;
	}
	else
		bResult = TRUE;

	return bResult;
};

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

void CStackingDlg::OnClickPictures(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString				strFileName;

	UpdateListInfo();
	if (m_Pictures.GetSelectedFileName(strFileName))
	{
		if (strFileName.CompareNoCase(m_strShowFile))
		{
			if (checkEditChanges())
			{
				BeginWaitCursor();
				m_Infos.SetTextColor(RGB(0, 0, 0));
				m_Infos.SetText(strFileName);
				m_Infos.SetLink(FALSE, FALSE);
				m_strShowFile = strFileName;
				OnBackgroundImageLoaded(0, 0);
			};
		};
	}
	else
	{
		m_Infos.SetTextColor(RGB(0, 0, 0));
		m_Infos.SetLink(FALSE, FALSE);
		m_Infos.SetText("");
	};

	*pResult = 0;
}

/* ------------------------------------------------------------------- */

void CStackingDlg::ReloadCurrentImage()
{
	if (m_strShowFile.GetLength())
	{
		BeginWaitCursor();
		imageLoader.clearCache();
		OnBackgroundImageLoaded(0, 0);
		EndWaitCursor();
	};

	m_Pictures.Invalidate(FALSE);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSelChangeGroup(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_Pictures.SetCurrentGroupID(m_GroupTab.GetCurSel());
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSelChangeJob(NMHDR* pNMHDR, LRESULT* pResult)
{
	//m_Pictures.SetCurrentGroupID(m_GroupTab.GetCurSel());
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnShowHideJobs( NMHDR * pNotifyStruct, LRESULT * result )
{
	//
};

/* ------------------------------------------------------------------- */

LRESULT CStackingDlg::OnCheckItem(WPARAM, LPARAM)
{
	UpdateListInfo();

	return 0;
};

/* ------------------------------------------------------------------- */

LRESULT CStackingDlg::OnSelectItem(WPARAM, LPARAM)
{
	LRESULT				lResult;

	OnClickPictures(nullptr, &lResult);

	return 0;
};

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (!(cx == 0 && cy == 0))
		UpdateLayout();
}

/* ------------------------------------------------------------------- */

void CStackingDlg::OnSplitter(NMHDR* pNMHDR, LRESULT* pResult)
{
	SPC_NMHDR* pHdr = reinterpret_cast<SPC_NMHDR*>(pNMHDR);

	CSplitterControl::ChangeHeight(&m_Picture, pHdr->delta);
	CSplitterControl::ChangeHeight(&m_Pictures, -pHdr->delta, CW_BOTTOMALIGN);
	CSplitterControl::ChangePos(&m_ListInfo, 0, pHdr->delta);
	Invalidate();
	UpdateWindow();
	m_Picture.Invalidate();
	m_Pictures.Invalidate();
	m_ListInfo.Invalidate();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UncheckNonStackablePictures()
{
	m_Pictures.UnCheckNonStackable();
};


/* ------------------------------------------------------------------- */

void CStackingDlg::UpdateCheckedAndOffsets(CStackingEngine & StackingEngine)
{
	LIGHTFRAMEINFOVECTOR &	vBitmaps = StackingEngine.LightFrames();

	m_Pictures.ClearOffsets();
	for (size_t i = 0; i < vBitmaps.size(); i++)
	{
		if (vBitmaps[i].m_bDisabled)
			m_Pictures.ClearOffset(vBitmaps[i].m_strFileName);
		else
			m_Pictures.UpdateOffset(vBitmaps[i].m_strFileName, vBitmaps[i].m_fXOffset, vBitmaps[i].m_fYOffset, vBitmaps[i].m_fAngle, vBitmaps[i].m_BilinearParameters, vBitmaps[i].m_vVotedPairs);
	};

	if (vBitmaps.size())
		m_EditStarSink.SetRefStars(vBitmaps[0].m_vStars);
	else
		m_EditStarSink.ClearRefStars();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::DoStacking(CAllStackingTasks & tasks, double fPercent)
{
	ZFUNCTRACE_RUNTIME();
	BOOL				bContinue = TRUE;
	CDSSProgressDlg		dlg;
	const auto			dwStartTime = GetTickCount64();

	if (!tasks.m_vStacks.size())
	{
		fillTasks(tasks);
	};
	if (tasks.m_vStacks.size() &&
		tasks.m_vStacks[0].m_pLightTask &&
		tasks.m_vStacks[0].m_pLightTask->m_vBitmaps.size() &&
		tasks.m_vStacks[0].m_pLightTask->m_vBitmaps[0].m_strFileName.GetLength())
	{
		ZTRACE_RUNTIME("Start stacking process");
	};

	{
		// Stack registered light frames
		CStackingEngine				StackingEngine;
		CSmartPtr<CMemoryBitmap>	pBitmap;
		CString						strReferenceFrame;

		if (m_Pictures.GetReferenceFrame(strReferenceFrame))
			StackingEngine.SetReferenceFrame(strReferenceFrame);

		StackingEngine.SetKeptPercentage(fPercent);
		bContinue = StackingEngine.StackLightFrames(tasks, &dlg, &pBitmap);
		const auto dwElapsedTime = GetTickCount64() - dwStartTime;
		UpdateCheckedAndOffsets(StackingEngine);
		if (bContinue)
		{
			CString					strFileName;
			CString					strText;
			DWORD					iff;
			CWorkspace				workspace;

			const auto iff = (INTERMEDIATEFILEFORMAT)workspace.value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();

			if (StackingEngine.GetDefaultOutputFileName(strFileName, m_strCurrentFileList, (iff == IFF_TIFF)))
			{
				StackingEngine.WriteDescription(tasks, strFileName);

				strText.Format(IDS_SAVINGFINAL, strFileName);
				dlg.Start2(strText, 0);

				if (iff == IFF_TIFF)
				{
					if (pBitmap->IsMonochrome())
						WriteTIFF(strFileName, pBitmap, &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE, nullptr);
					else
						WriteTIFF(strFileName, pBitmap, &dlg, TF_32BITRGBFLOAT, TC_DEFLATE, nullptr);
				}
				else
				{
					if (pBitmap->IsMonochrome())
						WriteFITS(strFileName, pBitmap, &dlg, FF_32BITGRAYFLOAT, nullptr);
					else
						WriteFITS(strFileName, pBitmap, &dlg, FF_32BITRGBFLOAT, nullptr);
				};

				dlg.End2();
                dlg.Close();

				GetProcessingDlg(this).LoadFile(strFileName);

				// Change tab to processing
				CDeepStackerDlg *	pDlg = GetDeepStackerDlg(this);

				if (pDlg)
					pDlg->ChangeTab(IDD_PROCESSING);
			};

			// Total elapsed time
			/*#ifdef DSSBETA
			{
				CString			strElapsed;
				CString			strText;

				ExposureToString((double)dwElapsedTime/1000.0, strElapsed);
				strText.Format(_T("Total process time: %s"), (LPCTSTR)strElapsed);
				AfxMessageBox(strText, MB_OK | MB_ICONINFORMATION);
			};
			#endif*/
		};
	};

	ZTRACE_RUNTIME("------------------------------\nEnd of stacking process");

	EndWaitCursor();
};

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */

void CStackingDlg::BatchStack()
{
	CBatchStacking			dlg;

	dlg.SetMRUList(m_MRUList);
	dlg.DoModal();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAbove()
{
	if (checkEditChanges())
	{
		CCheckAbove		dlg;
		double			fThreshold;

		if (dlg.DoModal() == IDOK)
		{
			fThreshold = dlg.GetThreshold();
			if (dlg.IsPercent())
				m_Pictures.CheckBest(fThreshold);
			else
				m_Pictures.CheckAbove(fThreshold);
		};
		UpdateListInfo();
	};
};

/* ------------------------------------------------------------------- */

void CStackingDlg::CheckAll()
{
	m_Pictures.CheckAll(TRUE);
	m_Pictures.CheckAllDarks(TRUE);
	UpdateListInfo();
};

/* ------------------------------------------------------------------- */

void CStackingDlg::UncheckAll()
{
	m_Pictures.CheckAll(FALSE);
	m_Pictures.CheckAllDarks(FALSE);
	UpdateListInfo();
};

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

void CStackingDlg::CheckBests(double fPercent)
{
	if (checkEditChanges())
		m_Pictures.CheckBest(fPercent);
};

/* ------------------------------------------------------------------- */

void CStackingDlg::ComputeOffsets()
{

	if (checkEditChanges() && (checkedImageCount(PICTURETYPE_LIGHTFRAME) > 0))
	{
		BOOL					bContinue = TRUE;
		CAllStackingTasks		tasks;

		fillTasks(tasks);

		if (m_Pictures.GetNrUnregisteredCheckedLightFrames())
		{
			CRegisterEngine	RegisterEngine;
			CDSSProgressDlg	dlg;

			m_Pictures.BlankCheckedItemScores();
			bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
			m_Pictures.updateCheckedItemScores();
			dlg.Close();
		};

		if (bContinue)
		{
			CDSSProgressDlg			dlg;
			CStackingEngine			StackingEngine;
			CString					strReferenceFrame;

			if (m_Pictures.GetReferenceFrame(strReferenceFrame))
				StackingEngine.SetReferenceFrame(strReferenceFrame);

			BeginWaitCursor();
			StackingEngine.ComputeOffsets(tasks, &dlg);

			// For each light frame - update the offset in the list
			UpdateCheckedAndOffsets(StackingEngine);

			EndWaitCursor();
		};
	};
};

/* ------------------------------------------------------------------- */

BOOL CStackingDlg::SaveOnClose()
{
	m_Pictures.SaveState();
	m_MRUList.saveSettings();

	return checkEditChanges() && CheckWorkspaceChanges();
};

/* ------------------------------------------------------------------- */


/* ------------------------------------------------------------------- */

void CStackingDlg::OnBnClicked4corners()
{
	m_Picture.Set4CornersMode(!m_Picture.Get4CornersMode());
}

/* ------------------------------------------------------------------- */

void CStackingDlg::versionInfoReceived(QNetworkReply* reply)
{
	QNetworkReply::NetworkError error = reply->error();
	if (QNetworkReply::NoError == error)
	{
		QString string(reply->read(reply->bytesAvailable()));

		if (string.startsWith("DeepSkyStackerVersion="))
		{
			QString verStr = string.section('=', 1, 1);
			int version = verStr.section('.', 0, 0).toInt();
			int release = verStr.section('.', 1, 1).toInt();
			int mod = verStr.section('.', 2, 2).toInt();

			if ((version > DSSVER_MAJOR) ||
				(version == DSSVER_MAJOR && release > DSSVER_MINOR) ||
				(version == DSSVER_MAJOR && release == DSSVER_MINOR && mod > DSSVER_SUB)
				)
			{
				CString	strNewVersion;

				strNewVersion.Format(IDS_VERSIONAVAILABLE, CString((wchar_t*)verStr.utf16()));
				m_Infos.SetTextColor(RGB(255, 0, 0));
				m_Infos.SetText(strNewVersion);
				m_Infos.SetLink(true, false);
				m_Infos.SetHyperLink("https://github.com/deepskystacker/DSS/releases/latest");
			};
		}
	}
	else
	{
		CDeepStackerDlg* pDlg = GetDeepStackerDlg(nullptr);
		CString title;
		pDlg->GetWindowText(title);
		QMessageBox::warning(nullptr, QString::fromWCharArray(title.GetString()),
			QCoreApplication::translate("StackingDlg", "Internet version check error code %1:\n%2")
			.arg(error)
			.arg(reply->errorString()), QMessageBox::Ok);

	}
	reply->deleteLater();
	networkManager->deleteLater();
};

void CStackingDlg::retrieveLatestVersionInfo()
{
	//#ifndef DSSBETA
	ZFUNCTRACE_RUNTIME();

	QSettings			settings;

	bool checkVersion = settings.value("InternetCheck", false).toBool();
	if (checkVersion)
	{
		networkManager = new QNetworkAccessManager();

		QObject::connect(networkManager, &QNetworkAccessManager::finished,
			[this](QNetworkReply* reply) { this->versionInfoReceived(reply); });

		QNetworkRequest req(QUrl("https://github.com/deepskystacker/DSS/raw/release/CurrentVersion.txt"));
		req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
		networkManager->get(req);
	}
	//#endif
}
#endif