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
// StackingDlg.cpp : implementation file
//

#include "stdafx.h"

#include <chrono>
#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QTreeWidget>
#include <QPainter>
#include <QTextLayout>
#include <QShowEvent>
#include <QSettings>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QStyleOptionButton>
#include <QUrl>

#include <filesystem>

#include "ui/ui_StackingDlg.h"
#include "mrupath.h"

#include "DeepSkyStacker.h"
#include "StackingDlg.h"
#include "ProcessingDlg.h"
#include "DeepStack.h"
#include "FrameInfoSupport.h"
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
#include "dsseditstars.h"
#include "dssselectrect.h"
#include "dsstoolbar.h"
#include "ui/ui_StackingDlg.h"
#include "avx_support.h"


#include <ZExcept.h>

#define _USE_MATH_DEFINES
#include <cmath>

#define dssApp DeepSkyStacker::instance()

namespace
{
	static QSizeF viewItemTextLayout(QTextLayout& textLayout, int lineWidth, int maxHeight = -1, int* lastVisibleLine = nullptr)
	{
		if (lastVisibleLine)
			*lastVisibleLine = -1;
		qreal height = 0;
		qreal widthUsed = 0;
		textLayout.beginLayout();
		int i = 0;
		while (true) {
			QTextLine line = textLayout.createLine();
			if (!line.isValid())
				break;
			line.setLineWidth(lineWidth);
			line.setPosition(QPointF(0, height));
			height += line.height();
			widthUsed = qMax(widthUsed, line.naturalTextWidth());
			// we assume that the height of the next line is the same as the current one
			if (maxHeight > 0 && lastVisibleLine && height + line.height() > maxHeight) {
				const QTextLine nextLine = textLayout.createLine();
				*lastVisibleLine = nextLine.isValid() ? i : -1;
				break;
			}
			++i;
		}
		textLayout.endLayout();
		return QSizeF(widthUsed, height);
	}
}

namespace DSS
{
	constexpr	DWORD					IDC_EDIT_SELECT = 1;
	constexpr	DWORD					IDC_EDIT_STAR   = 2;
	constexpr	DWORD					IDC_EDIT_COMET  = 3;
	constexpr	DWORD					IDC_EDIT_SAVE   = 4;

	const QStringList OUTPUTLIST_FILTERS({
		QCoreApplication::translate("StackingDlg", "File List (*.dssfilelist)", "IDS_LISTFILTER_OUTPUT"),
		QCoreApplication::translate("StackingDlg", "File List (*.txt)", "IDS_LISTFILTER_OUTPUT"),
		QCoreApplication::translate("StackingDlg", "All Files (*)", "IDS_LISTFILTER_OUTPUT")
		});


	const QStringList INPUTFILE_FILTERS({
		QCoreApplication::translate("StackingDlg", "Picture Files (*.bmp *.jpg *.jpeg *.tif *.tiff *.png *.fit *.fits *.fts *.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)"),
		QCoreApplication::translate("StackingDlg", "Windows Bitmaps (*.bmp)"),
		QCoreApplication::translate("StackingDlg", "JPEG or PNG Files (*.jpg *.jpeg *.png)"),
		QCoreApplication::translate("StackingDlg", "TIFF Files (*.tif *.tiff)"),
		QCoreApplication::translate("StackingDlg", "RAW Files (*.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)"),
		QCoreApplication::translate("StackingDlg", "FITS Files (*.fits *.fit *.fts)"),
		QCoreApplication::translate("StackingDlg", "All Files (*)")
		});


	QString IconSizeDelegate::calculateElidedText(const ::QString& text, const QTextOption& textOption,
		const QFont& font, const QRect& textRect, const Qt::Alignment valign,
		Qt::TextElideMode textElideMode, int flags,
		bool lastVisibleLineShouldBeElided, QPointF* paintStartPosition) const
	{
		QTextLayout textLayout(text, font);
		textLayout.setTextOption(textOption);

		// In AlignVCenter mode when more than one line is displayed and the height only allows
		// some of the lines it makes no sense to display those. From a users perspective it makes
		// more sense to see the start of the text instead something inbetween.
		const bool vAlignmentOptimization = paintStartPosition && valign.testFlag(Qt::AlignVCenter);

		int lastVisibleLine = -1;
		viewItemTextLayout(textLayout, textRect.width(), vAlignmentOptimization ? textRect.height() : -1, &lastVisibleLine);

		const QRectF boundingRect = textLayout.boundingRect();
		// don't care about LTR/RTL here, only need the height
		const QRect layoutRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, valign,
			boundingRect.size().toSize(), textRect);

		if (paintStartPosition)
			*paintStartPosition = QPointF(textRect.x(), layoutRect.top());

		QString ret;
		qreal height = 0;
		const int lineCount = textLayout.lineCount();
		for (int i = 0; i < lineCount; ++i) {
			const QTextLine line = textLayout.lineAt(i);
			height += line.height();

			// above visible rect
			if (height + layoutRect.top() <= textRect.top()) {
				if (paintStartPosition)
					paintStartPosition->ry() += line.height();
				continue;
			}

			const int start = line.textStart();
			const int length = line.textLength();
			const bool drawElided = line.naturalTextWidth() > textRect.width();
			bool elideLastVisibleLine = lastVisibleLine == i;
			if (!drawElided && i + 1 < lineCount && lastVisibleLineShouldBeElided) {
				const QTextLine nextLine = textLayout.lineAt(i + 1);
				const int nextHeight = height + nextLine.height() / 2;
				// elide when less than the next half line is visible
				if (nextHeight + layoutRect.top() > textRect.height() + textRect.top())
					elideLastVisibleLine = true;
			}

			QString text = textLayout.text().mid(start, length);
			if (drawElided || elideLastVisibleLine) {
				if (elideLastVisibleLine) {
					if (text.endsWith(QChar::LineSeparator))
						text.chop(1);
					text += QChar(0x2026);
				}
				QFontMetrics fontMetrics(font);
				ret += fontMetrics.elidedText(text, textElideMode, textRect.width());

				// no newline for the last line (last visible or real)
				// sometimes drawElided is true but no eliding is done so the text ends
				// with QChar::LineSeparator - don't add another one. This happened with
				// arabic text in the testcase for QTBUG-72805
				if (i < lineCount - 1 &&
					!ret.endsWith(QChar::LineSeparator))
					ret += QChar::LineSeparator;
			}
			else {
				ret += text;
			}

			// below visible text, can stop
			if ((height + layoutRect.top() >= textRect.bottom()) ||
				(lastVisibleLine >= 0 && lastVisibleLine == i))
				break;
		}
		return ret;
	}


	void IconSizeDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		Q_ASSERT(index.isValid());
		QStyleOptionViewItem opt{ option };
		initStyleOption(&opt, index);

		QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

		constexpr uint neededFeatures
		{ static_cast<uint>(QStyleOptionViewItem::HasCheckIndicator |
			QStyleOptionViewItem::HasDisplay |
			QStyleOptionViewItem::HasDecoration) };

		Q_ASSERT(neededFeatures == (opt.features & neededFeatures));
		
		QIcon icon = qvariant_cast<QIcon> (index.model()->data(index, Qt::DecorationRole)); 

		painter->save();

		QRect checkRect = style->subElementRect(QStyle::SE_ItemViewItemCheckIndicator, &opt);
		QRect iconRect = style->subElementRect(QStyle::SE_ItemViewItemDecoration, &opt);
		QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt);
		
		//
		// Draw the check box normally
		//
		QStyleOptionButton checkBoxStyle;
		//constexpr int size = 17;
		constexpr int space = 6;
		checkBoxStyle.rect = checkRect;
		//checkBoxStyle.rect = QRect(opt.rect.x(),
		//	opt.rect.center().y() - (size / 2),
		//	size,
		//	size);
		
		checkBoxStyle.state = (Qt::Checked == opt.checkState) ? QStyle::State_On : QStyle::State_Off;
		style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkBoxStyle, painter);

		//
		// Draw the icon twice as large as the default
		//
		QSize iconSize = opt.decorationSize;
		iconSize.scale(2 * iconSize.width(), 2 * iconSize.height(), Qt::KeepAspectRatio);
		iconRect.setWidth(iconSize.width()); iconRect.setHeight(iconSize.height());
		iconRect.setTop(opt.rect.center().y() - (iconSize.height() / 2));
		// draw the icon
		QIcon::Mode mode = QIcon::Normal;
		if (!(opt.state & QStyle::State_Enabled))
			mode = QIcon::Disabled;
		else if (opt.state & QStyle::State_Selected)
			mode = QIcon::Selected;
		QIcon::State state = opt.state & QStyle::State_Open ? QIcon::On : QIcon::Off;
		opt.icon.paint(painter, iconRect, opt.decorationAlignment, mode, state);
		//QPixmap iconPixmap{ icon.pixmap(iconSize) };
		//painter->drawPixmap(nextRect.x(), nextRect.center().y() - (iconSize.height() / 2), iconPixmap);

		//
		// Draw the text as normal
		//
		textRect.setLeft(iconRect.right() + 2);
		QFontMetrics fontMetrics(opt.font);
		const int textMargin = style->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr) + 1;
		textRect = textRect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding
		const bool wrapText = opt.features & QStyleOptionViewItem::WrapText;
		QTextOption textOption;
		textOption.setWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::ManualWrap);
		textOption.setTextDirection(opt.direction);
		textOption.setAlignment(QStyle::visualAlignment(opt.direction, opt.displayAlignment));

		QPointF paintPosition;
		const QString newText = calculateElidedText(opt.text, textOption,
			opt.font, textRect, opt.displayAlignment,
			opt.textElideMode, 0,
			true, &paintPosition);

		QTextLayout textLayout(newText, opt.font);
		textLayout.setTextOption(textOption);
		viewItemTextLayout(textLayout, textRect.width());
		textLayout.draw(painter, paintPosition);

		painter->restore();
	}

	/////////////////////////////////////////////////////////////////////////////
	// StackingDlg dialog

	StackingDlg::StackingDlg(QWidget* parent) :
		QWidget(parent),
		ui(new Ui::StackingDlg),
		initialised(false)
	{
		ui->setupUi(this);

		mruPath.readSettings();
		connect(ui->tableView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableViewItemClickedEvent(const QModelIndex&)));
		connect(ui->fourCorners, SIGNAL(clicked(bool)), ui->picture, SLOT(on_fourCorners_clicked(bool)));
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
		ZFUNCTRACE_RUNTIME();
		ui->picture->setVisible(true);
		editStarsPtr = std::make_unique<EditStars>(ui->picture);
		selectRectPtr = std::make_unique<SelectRect>(ui->picture);
		pToolBar = std::make_unique<ToolBar>(this);
		pToolBar->setObjectName(QString::fromUtf8("toolBar"));
		pToolBar->setVisible(false);

		ui->picture->setToolBar(pToolBar.get());
		pToolBar->setVisible(false); pToolBar->setEnabled(false);

		if (!fileList.empty())
			openFileList(fileList);

		ui->tableView->setModel(frameList.currentTableModel());
		//
		// The default icon display is really rather small, so use a subclass
		// of QStyledItemDelegate to handle the rendering for column zero of 
		// the table with the icon doubled in size.
		//
		iconSizeDelegate = std::make_unique<IconSizeDelegate>();

		ui->tableView->setItemDelegateForColumn(0, iconSizeDelegate.get());

		//
		// Reduce font size and increase weight
		//
		QFont font { ui->tableView->font() };
		font.setPointSize(font.pointSize() - 1); font.setWeight(QFont::Medium);
		ui->tableView->setFont(font);
		font = ui->tableView->horizontalHeader()->font();
		font.setPointSize(font.pointSize() - 1);  font.setWeight(QFont::Medium);
		ui->tableView->horizontalHeader()->setFont(font);
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
				QString  fileName;
				const ImageListModel* model = dynamic_cast<const ImageListModel*>(index.model());
				int row = index.row();
				fileName = model->selectedFileName(row);
				//
				// If the filename hasn't changed but we have changes to the stars that need to be saved
				//
				if (fileName == m_strShowFile && checkEditChanges())
				{
					ui->information->setText(m_strShowFile);
					ui->information->setTextFormat(Qt::PlainText);
					ui->information->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
					ui->information->setOpenExternalLinks(false);
					m_strShowFile = fileName;
					imageLoad();
				}
				else if (fileName != m_strShowFile)
				{
					ui->information->setText(m_strShowFile);
					ui->information->setTextFormat(Qt::PlainText);
					ui->information->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
					ui->information->setOpenExternalLinks(false);
					m_strShowFile = fileName;
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
	//    both pBitMap and pImage will be set.
	//
	//    If the image is not in the cache, then the result will be false, and imageLoader.load will load the image
	//    into the cache in a background thread running in the default Qt threadpool.
	//
	// 2. On completion of image loading by the background thread.  In this case the image will now be available in 
	//    the cache, so invoking imageLoader.load() will now return true.
	//
	void StackingDlg::imageLoad()
	{
		std::shared_ptr<CMemoryBitmap>	pBitmap;
		std::shared_ptr<QImage>	pImage;

		if (!m_strShowFile.isEmpty() && imageLoader.load(m_strShowFile, pBitmap, pImage))
		{
			//
			// The image we want is available in the cache
			//
			m_LoadedImage.m_Image = pImage;
			m_LoadedImage.m_pBitmap = pBitmap;
			if (m_GammaTransformation.IsInitialized())
				ApplyGammaTransformation(m_LoadedImage.m_Image.get(), m_LoadedImage.m_pBitmap.get(), m_GammaTransformation);
			ui->picture->setPixmap(QPixmap::fromImage(*(m_LoadedImage.m_Image)));

			if (frameList.isLightFrame(m_strShowFile))
			{
				editStarsPtr->setLightFrame(m_strShowFile);
				editStarsPtr->setBitmap(pBitmap);
				pToolBar->setVisible(true); pToolBar->setEnabled(true);
			}
			else
			{
				pToolBar->setVisible(false); pToolBar->setEnabled(false);
				editStarsPtr->setBitmap(nullptr);
			};

			CBilinearParameters		Transformation;
			VOTINGPAIRVECTOR		vVotedPairs;

			if (frameList.getTransformation(m_strShowFile, Transformation, vVotedPairs))
				editStarsPtr->setTransformation(Transformation, vVotedPairs);
			ui->information->setStyleSheet(
					"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
					"stop:0 rgb(224, 244, 252), stop:1 rgb(138, 185, 242)) }");
			ui->information->setText(m_strShowFile);
		}
		else if (!m_strShowFile.isEmpty())
		{
			//
			// Display the "Loading filename" with red background gradient while loading in background
			//
			ui->information->setStyleSheet(
				"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
				"stop:0 rgb(252, 251, 222), stop:1 rgb(255, 151, 154)) }");
			ui->information->setText(tr("Loading %1", "IDS_LOADPICTURE")
				.arg(m_strShowFile));
			//
			// No longer interested in signals from the imageView object
			//
			ui->picture->disconnect(editStarsPtr.get(), nullptr);
			ui->picture->disconnect(selectRectPtr.get(), nullptr);

			pToolBar->setVisible(false); pToolBar->setEnabled(false);
			editStarsPtr->setBitmap(nullptr);
		}
		else
		{
			//
			// Display the blue gradient with no text
			//
			ui->information->setStyleSheet(
				"QLabel { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
				"stop:0 rgb(224, 244, 252), stop:1 rgb(138, 185, 242)) }");
			ui->information->setText("");
			//
			// No longer interested in signals from the imageView object
			//
			ui->picture->disconnect(editStarsPtr.get(), nullptr);
			ui->picture->disconnect(selectRectPtr.get(), nullptr);
		
			pToolBar->setVisible(false); pToolBar->setEnabled(false);
			editStarsPtr->setBitmap(nullptr);
		};
	};

	void StackingDlg::toolBar_rectButtonPressed([[maybe_unused]] bool checked)
	{
		qDebug() << "StackingDlg: rectButtonPressed";
		editStarsPtr->rectButtonPressed();
		selectRectPtr->rectButtonPressed();
	}

	void StackingDlg::toolBar_starsButtonPressed([[maybe_unused]] bool checked)
	{
		qDebug() << "StackingDlg: starsButtonPressed";
		//CheckAskRegister(); TODO
		editStarsPtr->starsButtonPressed();
		selectRectPtr->starsButtonPressed();
	}

	void StackingDlg::toolBar_cometButtonPressed([[maybe_unused]] bool checked)
	{
		qDebug() << "StackingDlg: cometButtonPressed";
		// TODO CheckAskRegister();
		editStarsPtr->cometButtonPressed();
		selectRectPtr->cometButtonPressed();
	}

	void StackingDlg::toolBar_saveButtonPressed([[maybe_unused]] bool checked)
	{
		editStarsPtr->saveRegisterSettings();
		pToolBar->setSaveEnabled(false);
		// Update the list with the new info
		//frameList.updateItemScores(m_strShowFile); //TODO
	}

	void StackingDlg::pictureChanged()
	{
		// Here check if the new image is dirty
		//if (editStarsPtr->isDirty())
		pToolBar->setSaveEnabled(true);
	}

	bool StackingDlg::fileAlreadyLoaded(const fs::path& file)
	{
		//
		// Check all groups to see if this file has already been loaded
		//
		if (auto groupId = Group::whichGroupContains(file); groupId != -1)
		{
				//
				// If the file has already been loaded complain
				//
				QString errorMessage(tr("File %1 has already been loaded in group %2 (%3)")
					.arg(file.generic_string().c_str())
					.arg(groupId)
					.arg(frameList.groupName(groupId)));

	#if defined(_CONSOLE)
				std::cerr << errorMessage;
	#else
				int ret = QMessageBox::warning(this, "DeepSkyStacker",
					errorMessage,
					QMessageBox::Ok);
	#endif
				return true;
		}
		return false;
	}

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

			//
			// Before attempting to add the files prune out those that have already been loaded
			// and issue an error message
			//
			auto it = std::remove_if(files.begin(), files.end(), 
				[&](const QString& s) { return fileAlreadyLoaded(s.toStdU16String()); });
			files.erase(it, files.end());

			//
			// Now add the images to the end of the current group in the frame list remembering
			// that we need to bracket the code  that adds the with beginInsertRows(rowcount) and
			// endInsertRows(), so the table model is informed of the new rows
			//
			frameList.beginInsertRows(files.size());
			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16

				frameList.addFile(file);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}
			frameList.endInsertRows();
			
			QGuiApplication::restoreOverrideCursor();
			//frameList.RefreshList();

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
		updateListInfo();
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

			//
			// Before attempting to add the files prune out those that have already been loaded
			// and issue an error message
			//
			auto it = std::remove_if(files.begin(), files.end(),
				[&](const QString& s) { return fileAlreadyLoaded(s.toStdU16String()); });
			files.erase(it, files.end());

			//
			// Now add the images to the end of the current group in the frame list remembering
			// that we need to bracket the code  that adds the with beginInsertRows(rowcount) and
			// endInsertRows(), so the table model is informed of the new rows
			//
			frameList.beginInsertRows(files.size());
			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16

				frameList.addFile(file,
					PICTURETYPE_DARKFRAME, true);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}
			frameList.endInsertRows();
			QGuiApplication::restoreOverrideCursor();
			// frameList.RefreshList(); TODO

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
		updateListInfo();
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

			//
			// Before attempting to add the files prune out those that have already been loaded
			// and issue an error message
			//
			auto it = std::remove_if(files.begin(), files.end(),
				[&](const QString& s) { return fileAlreadyLoaded(s.toStdU16String()); });
			files.erase(it, files.end());

			//
			// Now add the images to the end of the current group in the frame list remembering
			// that we need to bracket the code  that adds the with beginInsertRows(rowcount) and
			// endInsertRows(), so the table model is informed of the new rows
			//
			frameList.beginInsertRows(files.size());
			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16

				frameList.addFile(file,
					PICTURETYPE_DARKFLATFRAME, true);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}
			frameList.endInsertRows();
			QGuiApplication::restoreOverrideCursor();
			//frameList.RefreshList();

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
		updateListInfo();
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

			//
			// Before attempting to add the files prune out those that have already been loaded
			// and issue an error message
			//
			auto it = std::remove_if(files.begin(), files.end(),
				[&](const QString& s) { return fileAlreadyLoaded(s.toStdU16String()); });
			files.erase(it, files.end());

			//
			// Now add the images to the end of the current group in the frame list remembering
			// that we need to bracket the code  that adds the with beginInsertRows(rowcount) and
			// endInsertRows(), so the table model is informed of the new rows
			//
			frameList.beginInsertRows(files.size());
			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16

				frameList.addFile(file,
					PICTURETYPE_FLATFRAME, true);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}
			frameList.endInsertRows();
			QGuiApplication::restoreOverrideCursor();
			//frameList.RefreshList();

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
		updateListInfo();
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

			//
			// Before attempting to add the files prune out those that have already been loaded
			// and issue an error message
			//
			auto it = std::remove_if(files.begin(), files.end(),
				[&](const QString& s) { return fileAlreadyLoaded(s.toStdU16String()); });
			files.erase(it, files.end());

			//
			// Now add the images to the end of the current group in the frame list remembering
			// that we need to bracket the code  that adds the with beginInsertRows(rowcount) and
			// endInsertRows(), so the table model is informed of the new rows
			//
			frameList.beginInsertRows(files.size());
			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16

				frameList.addFile(file,
					PICTURETYPE_OFFSETFRAME, true);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}
			frameList.endInsertRows();
			QGuiApplication::restoreOverrideCursor();
			//frameList.RefreshList();

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
		updateListInfo();
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
				frameList.UpdateItemScores(m_strShowFile);
			}
			else if (nResult == IDNO)
				bResult = TRUE;
		}
		else
			bResult = TRUE;

		return bResult;
	#endif
	}

	/* ------------------------------------------------------------------- */

	void StackingDlg::UpdateCheckedAndOffsets(CStackingEngine& StackingEngine)
	{
		LIGHTFRAMEINFOVECTOR& vBitmaps = StackingEngine.LightFrames();

		frameList.clearOffsets();
		for (size_t i = 0; i < vBitmaps.size(); i++)
		{
			if (vBitmaps[i].m_bDisabled)
				frameList.clearOffset(vBitmaps[i].filePath);
			else
			{
				frameList.updateOffset(vBitmaps[i].filePath, vBitmaps[i].m_fXOffset, vBitmaps[i].m_fYOffset, vBitmaps[i].m_fAngle, vBitmaps[i].m_BilinearParameters, vBitmaps[i].m_vVotedPairs);
			}
		};

		if (vBitmaps.size())
			editStarsPtr->setRefStars(vBitmaps[0].m_vStars);
		else
			editStarsPtr->clearRefStars();
	};

	/* ------------------------------------------------------------------- */

	bool StackingDlg::saveOnClose()
	{
		//frameList.SaveState();
		mruPath.saveSettings();

		return checkEditChanges() && checkWorkspaceChanges();
	};

	/* ------------------------------------------------------------------- */

	void	StackingDlg::updateListInfo()
	{
		QString	text{ tr("Light Frames: %1      -      Dark Frames: %2      -      Flat Frames: %3      -   Dark Flat Frames: %4   -      Offset/Bias Frames: %5",
			"IDS_LISTINFO")
			.arg(frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_DARKFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_FLATFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_DARKFLATFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_OFFSETFRAME))
			};

		ui->listInfo->setText(text);

		for (int i = 0; i < ui->tabWidget->count(); i++)
		{
			text = tr("Light Frames: %1\nDark Frames: %2\nFlat Frames: %3\nDark Flat Frames: %4\nOffset/Bias Frames: %5",
				"IDS_LISTINFO2")
				.arg(frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_DARKFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_FLATFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_DARKFLATFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_OFFSETFRAME, i));
			
			ui->tabWidget->setTabToolTip(i, text);
		};
	};

	/* ------------------------------------------------------------------- */


	void StackingDlg::clearList()
	{
		if (checkEditChanges() && checkWorkspaceChanges())
		{
			frameList.clear();
			editStarsPtr->setBitmap(nullptr);
			m_strShowFile.clear();
			ui->information->setText(m_strShowFile);
			imageLoader.clearCache();
			m_LoadedImage.reset();
			//TODO: UpdateGroupTabs();
			updateListInfo();
			fileList.clear();
			ui->picture->clear();
			dssApp->setTitleFilename(fileList);
			update();
		}
	}

	void StackingDlg::checkAbove()
	{
		if (checkEditChanges())
		{
			CCheckAbove		dlg;
			double			fThreshold;

			if (dlg.DoModal() == IDOK)
			{
				fThreshold = dlg.GetThreshold();
				if (dlg.IsPercent())
					frameList.checkBest(fThreshold);
				else
					frameList.checkAbove(fThreshold);
			};
			updateListInfo();
		};
	};

	/* ------------------------------------------------------------------- */	void StackingDlg::loadList()
	{
		ZFUNCTRACE_RUNTIME();
		if (checkWorkspaceChanges())
		{
			bool openAnother{ false };

			if (m_MRUList.m_vLists.size())
			{
				QMenu menu(this);

				for (uint32_t i = 0; i < m_MRUList.m_vLists.size(); i++)
				{ 
					fs::path path{ m_MRUList.m_vLists[i].toStdU16String() };
					// Get the fileid without the extension
					QString name{ QString::fromStdU16String(path.stem().generic_u16string()) };

					QAction* action = menu.addAction(name);
					action->setData(i);		// Index into the list of files

				};
				menu.addSeparator();
				QAction* loadAnother = menu.addAction(tr("Open another File List...", "ID_FILELIST_OPENANOTHERFILELIST"));

				QAction* a = menu.exec(QCursor::pos());

				if (loadAnother == a)
					openAnother = true;
				else
				{
					QString			strList;

					//
					// Use the index we stored with the Menu Action to access the fill fileid
					//
					strList = m_MRUList.m_vLists[a->data().toUInt()];

					frameList.loadFilesFromList(strList.toStdU16String().c_str());
					// frameList.RefreshList(); TODO
					m_MRUList.Add(strList);
					fileList = strList.toStdU16String();
					dssApp->setTitleFilename(fileList);
				}
			}

			if (openAnother)
			{
				QString name;
				loadList(m_MRUList, name);
				dssApp->setTitleFilename(name.toStdWString().c_str());
			};
			// TODO UpdateGroupTabs();
			updateListInfo();
		}
	}

	void StackingDlg::loadList(CMRUList& MRUList, QString& strFileList)
	{
		ZFUNCTRACE_RUNTIME();
		QSettings settings;
		QString directory;
		QString	extension;

		QFileDialog			fileDialog;
		Workspace			workspace;

		directory = settings.value("Folders/ListFolder").toString();
		const auto filterIndex = settings.value("Folders/ListIndex", uint(0)).toUInt();
		extension = settings.value("Folders/ListExtension").toString();

		if (extension.isEmpty())
			extension = ".dssfilelist";

		fileDialog.setDefaultSuffix(extension);
		fileDialog.setFileMode(QFileDialog::ExistingFiles);
		fileDialog.setNameFilters(OUTPUTLIST_FILTERS);
		fileDialog.selectNameFilter(OUTPUTLIST_FILTERS[0]);
		fileDialog.setDirectory(directory);

		ZTRACE_RUNTIME("About to show file open dlg");
		if (QDialog::Accepted == fileDialog.exec())
		{
			QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			QStringList files = fileDialog.selectedFiles();

			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16

				frameList.loadFilesFromList(file);
				strFileList = QString::fromStdU16String(file.generic_u16string());
				MRUList.Add(strFileList);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}

			QGuiApplication::restoreOverrideCursor();
			// frameList.RefreshList(); TODO

			const auto selectedIndex = OUTPUTLIST_FILTERS.indexOf(fileDialog.selectedNameFilter());

			settings.setValue("Folders/ListFolder", directory);
			settings.setValue("Folders/ListIndex", static_cast<uint>(selectedIndex));
			settings.setValue("Folders/ListExtension", extension);
		}
	}

	void StackingDlg::saveList()
	{
		QString name;

		saveList(m_MRUList, name);
		dssApp->setTitleFilename(name.toStdWString().c_str());
	};

	/* ------------------------------------------------------------------- */

	void StackingDlg::saveList(CMRUList& MRUList, QString& strFileList)
	{
		ZFUNCTRACE_RUNTIME();
		QSettings					settings;
		QString directory;
		QString	extension;

		QFileDialog			fileDialog;
		Workspace			workspace;

		directory = settings.value("Folders/ListFolder").toString();
		const auto filterIndex = settings.value("Folders/ListIndex", uint(0)).toUInt();
		extension = settings.value("Folders/ListExtension").toString();

		if (extension.isEmpty())
			extension = ".dssfilelist";

		fileDialog.setDefaultSuffix(extension);
		fileDialog.setFileMode(QFileDialog::AnyFile);
		fileDialog.setAcceptMode(QFileDialog::AcceptSave);
		fileDialog.setNameFilters(OUTPUTLIST_FILTERS);
		fileDialog.selectNameFilter(OUTPUTLIST_FILTERS[0]);
		fileDialog.setDirectory(directory);

		ZTRACE_RUNTIME("About to show file save dlg");
		if (QDialog::Accepted == fileDialog.exec())
		{
			QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			QStringList files = fileDialog.selectedFiles();
			const auto selectedIndex = OUTPUTLIST_FILTERS.indexOf(fileDialog.selectedNameFilter());

			ZASSERTSTATE(1 == files.size());

			for (int i = 0; i < files.size(); i++)
			{
				fs::path file(files.at(i).toStdU16String());		// as UTF-16
				fileList = file;		// save this filelist

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());

				frameList.saveListToFile(file);
				strFileList = QString::fromStdU16String(file.generic_u16string());
				MRUList.Add(strFileList);
			}

			QGuiApplication::restoreOverrideCursor();

			settings.setValue("Folders/ListFolder", directory);
			settings.setValue("Folders/ListIndex", static_cast<uint>(selectedIndex));
			settings.setValue("Folders/ListExtension", extension);
		}

	}

	/* ------------------------------------------------------------------- */

	/* ------------------------------------------------------------------- */

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
			QMessageBox::warning(nullptr, dssApp->windowTitle(),
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
			req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
			networkManager->get(req);
		}
		//#endif
	}

	void StackingDlg::registerCheckedImages()
	{
		CDSSProgressDlg			dlg;
		::RegisterSettings		dlgSettings(this);
		bool					bContinue = true;
		const auto start{ std::chrono::steady_clock::now() };


		bool					bFound = false;

		if (frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME))
		{
			//CString				strFirstLightFrame;

			//frameList.GetFirstCheckedLightFrame(strFirstLightFrame);

			//dlgSettings.SetForceRegister(!frameList.GetNrUnregisteredCheckedLightFrames());
			//dlgSettings.SetNoDark(!frameList.GetNrCheckedDarks());
			//dlgSettings.SetNoFlat(!frameList.GetNrCheckedFlats());
			//dlgSettings.SetNoOffset(!frameList.GetNrCheckedOffsets());
			//dlgSettings.SetFirstLightFrame(strFirstLightFrame);

			CAllStackingTasks	tasks;
			CRect				rcSelect;

			frameList.fillTasks(tasks);

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

				if (checkReadOnlyFolders(tasks))
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
						//GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT); TODO

						CRegisterEngine	RegisterEngine;

						imageLoader.clearCache();
						frameList.blankCheckedItemScores();

						bContinue = RegisterEngine.RegisterLightFrames(tasks, bForceRegister, &dlg);

						frameList.updateCheckedItemScores();
						// Update the current image score if necessary
						if (!m_strShowFile.isEmpty()
							&& frameList.isLightFrame(m_strShowFile)
							&& frameList.isChecked(m_strShowFile))
						{
							// Update the registering info
							editStarsPtr->setLightFrame(m_strShowFile);
							ui->picture->update();
						};

						dlg.Close();
					};
					const auto now{ std::chrono::steady_clock::now() };
					std::chrono::duration<double> elapsed{ now - start };

					QString avxActive;
					if (AvxSupport::checkSimdAvailability())
						avxActive += "(SIMD)";
					QString message{ tr("Total registering time: %1 %2")
						.arg(exposureToString(elapsed.count()))
						.arg(avxActive) };
					QMessageBox::information(this, "DeepSkyStacker", message, QMessageBox::Ok, QMessageBox::Ok);

					if (bContinue && bStackAfter)
					{
						DoStacking(tasks, fPercent);
						dwEndTime = GetTickCount();
					};

					// GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_STOP); TODO
				};
			};
		}
		else
		{
			QMessageBox::critical(this, "DeepSkyStacker", 
				tr("You must check light frames to register them.", "IDS_ERROR_NOTLIGHTCHECKED2"));
		};
	};

	void StackingDlg::stackCheckedImages()
	{
		if (checkEditChanges())
		{
			BOOL				bContinue;
			CAllStackingTasks	tasks;
			CRect				rcSelect;

			frameList.fillTasks(tasks);

			// Set the selection rectangle if needed.   It is set by Qt signal from DSSSelectRect.cpp
			if (!selectRect.isEmpty())
			{
				tasks.SetCustomRectangle(CRect(selectRect.left(), selectRect.top(), selectRect.right(), selectRect.bottom()));
			}

			if (checkReadOnlyFolders(tasks))
			{
				bContinue = CheckStacking(tasks);
				if (bContinue)
					bContinue = showRecap(tasks);
				if (bContinue)
				{
					// GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT); TODO

					imageLoader.clearCache();
					if (frameList.unregisteredCheckedLightFrameCount())
					{
						CRegisterEngine	RegisterEngine;
						CDSSProgressDlg	dlg;

						frameList.blankCheckedItemScores();
						bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
						frameList.updateCheckedItemScores();
						dlg.Close();
					};

					if (bContinue)
						DoStacking(tasks);

					//GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_STOP); TODO
				};
			};
		};
	};

	/* ------------------------------------------------------------------- */

	bool StackingDlg::checkReadOnlyFolders(CAllStackingTasks& tasks)
	{
		bool result = true;
		QStringList folders;

		if (!tasks.checkReadOnlyStatus(folders))
		{
			QString message = tr("The following folder(s) are read-only:\n%1\nDeepSkyStacker needs to create files in these folders during its processing.",
				"IDS_WARNINGREADONLY")
				.arg(folders.join('\n'));

			QMessageBox::critical(this, "DeepSkyStacker",
				message, QMessageBox::Ok);
			result = FALSE;
		};

		return result;
	};

	/* ------------------------------------------------------------------- */

	bool StackingDlg::CheckStacking(CAllStackingTasks& tasks)
	{
		bool result = false;

		if (!frameList.areCheckedImagesCompatible())
			QMessageBox::critical(this, "DeepSkyStacker",
				tr("The checked pictures are not compatible (width, height, number of colors,"
					" number of channels, only one master dark, offset and flat).",
					"IDS_ERROR_NOTCOMPATIBLE"));
		else if (!frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME))
			QMessageBox::critical(this, "DeepSkyStacker",
				tr("You must check light frames to stack them.",
					"IDS_ERROR_NOTLIGHTCHECKED"));
		else
			result = true;

		return result;
	};

	bool StackingDlg::checkWorkspaceChanges()
	{
		bool						bResult = false;

		if (!fileList.empty() || Group::fileCount())
		{
			Workspace				workspace;

			if (frameList.dirty() || workspace.isDirty())
			{
				QString msg = tr("You have made some changes to the %1 file list and/or to the settings.\n\nDo you want to save the changes?",
					"IDS_WARNING_SAVECHANGES")
					.arg(QString::fromStdU16String(fileList.generic_u16string()));

				QMessageBox msgBox(QMessageBox::Question, QString(""), msg, (QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel));

				msgBox.setDefaultButton(QMessageBox::Yes);

				int ret = msgBox.exec();

				QString name;

				switch (ret)
				{
					case QMessageBox::Yes:
						if (!fileList.empty())
						{
							QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
							frameList.saveListToFile(fileList);
							name = QString::fromStdU16String(fileList.generic_u16string());
							QGuiApplication::restoreOverrideCursor();
						}
						else
						{
							saveList(m_MRUList, name);
						}

						dssApp->setTitleFilename(name.toStdWString().c_str());
						[[fallthrough]];
					case QMessageBox::No:
						bResult = true;
						break;
					case QMessageBox::Cancel:
						bResult = false;
				}

			}
			else
				bResult = true;
		}
		else
			bResult = true;

		return bResult;
	};

	/* ------------------------------------------------------------------- */

	void StackingDlg::reloadCurrentImage()
	{
		if (!m_strShowFile.isEmpty())
		{
			QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			imageLoader.clearCache();
			imageLoad();
			QGuiApplication::restoreOverrideCursor();

		};

		//frameList.Invalidate(FALSE); TODO
	};

	/* ------------------------------------------------------------------- */


	/* ------------------------------------------------------------------- */


	bool StackingDlg::showRecap(CAllStackingTasks& tasks)
	{
		StackRecap	dlg(this);

		dlg.setStackingTasks(&tasks);
		return dlg.exec();
	};

	/* ------------------------------------------------------------------- */

	void StackingDlg::DoStacking(CAllStackingTasks& tasks, const double fPercent)
	{
		ZFUNCTRACE_RUNTIME();

		bool bContinue = true;
		CDSSProgressDlg dlg;
		const auto start{ std::chrono::steady_clock::now() };

		if (tasks.m_vStacks.empty())
		{
			frameList.fillTasks(tasks);
			tasks.ResolveTasks();
		}

		if (!tasks.m_vStacks.empty() && tasks.m_vStacks.cbegin()->m_pLightTask != nullptr && !tasks.m_vStacks.cbegin()->m_pLightTask->m_vBitmaps.empty()
			&& tasks.m_vStacks.cbegin()->m_pLightTask->m_vBitmaps.cbegin()->filePath.empty() != true)
		{
			ZTRACE_RUNTIME("Start stacking process");
		}

		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		{
			// Stack registered light frames
			CStackingEngine StackingEngine;

			QString referenceFrame{ frameList.getReferenceFrame() };
			if (!referenceFrame.isEmpty())
				StackingEngine.SetReferenceFrame(referenceFrame.toStdWString().c_str());

			StackingEngine.SetKeptPercentage(fPercent);

			std::shared_ptr<CMemoryBitmap> pBitmap;
			bContinue = StackingEngine.StackLightFrames(tasks, &dlg, pBitmap);
			const auto now{ std::chrono::steady_clock::now() };
			std::chrono::duration<double> elapsed{ now - start };

			QString avxActive;
			if (AvxSupport::checkSimdAvailability())
				avxActive += "(SIMD)";
			QString message{ tr("Total stacking time: %1 %2")
				.arg(exposureToString(elapsed.count()))
				.arg(avxActive) };
			QMessageBox::information(this, "DeepSkyStacker", message, QMessageBox::Ok, QMessageBox::Ok);

			UpdateCheckedAndOffsets(StackingEngine);

			if (bContinue)
			{
				CString strFileName;
				CString strText;

				Workspace workspace;
				const auto iff = (INTERMEDIATEFILEFORMAT)workspace.value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();

				if (StackingEngine.GetDefaultOutputFileName(strFileName, fileList.generic_wstring().c_str(), iff == IFF_TIFF))
				{
					StackingEngine.WriteDescription(tasks, strFileName);

					strText.Format(IDS_SAVINGFINAL, strFileName);
					dlg.Start2(strText, 0);

					if (iff == IFF_TIFF)
					{
						if (pBitmap->IsMonochrome())
							WriteTIFF(strFileName, pBitmap.get(), &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE, nullptr);
						else
							WriteTIFF(strFileName, pBitmap.get(), &dlg, TF_32BITRGBFLOAT, TC_DEFLATE, nullptr);
					}
					else
					{
						if (pBitmap->IsMonochrome())
							WriteFITS(strFileName, pBitmap.get(), &dlg, FF_32BITGRAYFLOAT, nullptr);
						else
							WriteFITS(strFileName, pBitmap.get(), &dlg, FF_32BITRGBFLOAT, nullptr);
					}

					dlg.End2();
					dlg.Close();

					dssApp->getProcessingDlg().LoadFile(strFileName);

					// Change tab to processing
					dssApp->setTab(IDD_PROCESSING);
				}
				// Total elapsed time

			}
		}

		ZTRACE_RUNTIME("------------------------------\nEnd of stacking process");
		QGuiApplication::restoreOverrideCursor();
	}

	/* ------------------------------------------------------------------- */

	void StackingDlg::openFileList(const fs::path& file)
	{
		QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

		try
		{
			// Check that the file can be opened
			if (std::FILE* hFile =
#if defined(_WINDOWS)
				_wfopen(file.c_str(), L"rt")
#else
				std::fopen(file.c_ctr(), "rt")
#endif
				)
			{
				fclose(hFile);
				frameList.loadFilesFromList(file);
				// frameList.RefreshList(); TODO
				m_MRUList.Add(QString::fromStdU16String(file.generic_u16string()));
				// TODO UpdateGroupTabs();
				updateListInfo();
				dssApp->setTitleFilename(file);
			};
		}
		catch (const fs::filesystem_error& e)
		{
			QString errorMessage(tr("Exception caught in function: %1\n"
									"%2 (line %3):\n\n%4")
				.arg(__FUNCTION__)
				.arg(__FILE__)
				.arg(__LINE__)
				.arg(e.what())
			);
#if defined(_CONSOLE)
			std::cerr << errorMessage;
#else
			int ret = QMessageBox::warning(this, "DeepSkyStacker",
				errorMessage,
				QMessageBox::Ok);
#endif
		}
		QGuiApplication::restoreOverrideCursor();
	};

	/* ------------------------------------------------------------------- */

	void StackingDlg::computeOffsets()
	{

		if (checkEditChanges() && (frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME) > 0))
		{
			BOOL					bContinue = TRUE;
			CAllStackingTasks		tasks;

			frameList.fillTasks(tasks);

			if (frameList.unregisteredCheckedLightFrameCount())
			{
				CRegisterEngine	RegisterEngine;
				CDSSProgressDlg	dlg;

				frameList.blankCheckedItemScores();
				bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
				frameList.updateCheckedItemScores();
				dlg.Close();
			};

			if (bContinue)
			{
				CDSSProgressDlg			dlg;
				CStackingEngine			StackingEngine;

				QString referenceFrame{ frameList.getReferenceFrame() };
				if (!referenceFrame.isEmpty())
					StackingEngine.SetReferenceFrame(referenceFrame.toStdWString().c_str());

				QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				StackingEngine.ComputeOffsets(tasks, &dlg);

				// For each light frame - update the offset in the list
				UpdateCheckedAndOffsets(StackingEngine);

				QGuiApplication::restoreOverrideCursor();

			};
		};
	};

	void StackingDlg::checkAll()
	{
		frameList.checkAll(true);
		updateListInfo();
	};

	void StackingDlg::unCheckAll()
	{
		frameList.checkAll(false);
		updateListInfo();
	};

	/* ------------------------------------------------------------------- */



	void StackingDlg::batchStack()
	{
#if (0)
		CBatchStacking			dlg; // TODO

		dlg.SetMRUList(m_MRUList);
		dlg.DoModal();
#endif
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
		DDX_Control(pDX, IDC_PICTURES, frameList);
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

		frameList.Initialize();
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
		int dwLastGroupID = static_cast<int>(frameList.GetLastGroupID());
		if (frameList.GetNrFrames(dwLastGroupID) != 0)
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
			frameList.SetCurrentGroupID(0);
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
				ApplyGammaTransformation(m_LoadedImage.m_hBitmap.get(), m_LoadedImage.m_pBitmap.get(), m_GammaTransformation);
				// Refresh
				m_Picture.Invalidate(TRUE);
			}
		}
	}

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
				frameList.addFile(vFiles[i], frameList.groupID(), dlg.GetDropType(), true);

			frameList.RefreshList();
			UpdateGroupTabs();
			updateListInfo();
			EndWaitCursor();
		};
	};

	/* ------------------------------------------------------------------- */


	/* ------------------------------------------------------------------- */


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
					frameList.CheckAllLights(FALSE);
					frameList.CheckImage(m_strShowFile, TRUE);
					RegisterCheckedImage();
				}
				else if (dlg.GetAction()==ARA_ALL)
				{
					// Register all the checked light frames
					frameList.CheckImage(m_strShowFile, TRUE);
					RegisterCheckedImage();
				};
			};
		};
	};

	/* ------------------------------------------------------------------- */

	/* ------------------------------------------------------------------- */

	void CStackingDlg::ButtonToolbar_OnClick(DWORD dwID, CButtonToolbar * pButtonToolbar)
	{
		if (dwID == IDC_EDIT_SAVE)
		{
			m_EditStarSink.SaveRegisterSettings();
			m_ButtonToolbar.Enable(IDC_EDIT_SAVE, FALSE);
			// Update the list with the new info
			frameList.UpdateItemScores(m_strShowFile);
		};
	};

	/* ------------------------------------------------------------------- */

	/* ------------------------------------------------------------------- */



	/* ------------------------------------------------------------------- */

	LRESULT CStackingDlg::OnBackgroundImageLoaded(WPARAM wParam, LPARAM lParam)
	{
		std::shared_ptr<CMemoryBitmap> pBitmap;
		std::shared_ptr<C32BitsBitmap> phBitmap;

		if (m_strShowFile.GetLength() && m_BackgroundLoading.LoadImage(m_strShowFile, pBitmap, phBitmap))
		{
			m_LoadedImage.m_hBitmap = phBitmap;
			m_LoadedImage.m_pBitmap = pBitmap;
			if (m_GammaTransformation.IsInitialized())
				ApplyGammaTransformation(m_LoadedImage.m_hBitmap.get(), m_LoadedImage.m_pBitmap.get(), m_GammaTransformation);
			m_Picture.SetImg(phBitmap->GetHBITMAP(), true);

			if (frameList.IsLightFrame(m_strShowFile))
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
				m_EditStarSink.SetBitmap(std::shared_ptr<CMemoryBitmap>{});
			};
			m_Picture.SetBltMode(CWndImage::bltFitXY);
			m_Picture.SetAlign(CWndImage::bltCenter, CWndImage::bltCenter);

			CBilinearParameters		Transformation;
			VOTINGPAIRVECTOR		vVotedPairs;

			if (frameList.GetTransformation(m_strShowFile, Transformation, vVotedPairs))
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
			m_EditStarSink.SetBitmap(std::shared_ptr<CMemoryBitmap>{});
		}
		else
		{
			m_Infos.SetBkColor(RGB(224, 244, 252), RGB(138, 185, 242), CLabel::Gradient);
			m_Infos.SetText("");
			m_Picture.SetImageSink(nullptr);
			m_Picture.SetButtonToolbar(nullptr);
			m_EditStarSink.SetBitmap(std::shared_ptr<CMemoryBitmap>{});
		}
		return 1;
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::OnClickPictures(NMHDR* pNMHDR, LRESULT* pResult)
	{
		CString				strFileName;

		updateListInfo();
		if (frameList.GetSelectedFileName(strFileName))
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

		frameList.Invalidate(FALSE);
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::OnSelChangeGroup(NMHDR* pNMHDR, LRESULT* pResult)
	{
		frameList.SetCurrentGroupID(m_GroupTab.GetCurSel());
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::OnSelChangeJob(NMHDR* pNMHDR, LRESULT* pResult)
	{
		//frameList.SetCurrentGroupID(m_GroupTab.GetCurSel());
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::OnShowHideJobs( NMHDR * pNotifyStruct, LRESULT * result )
	{
		//
	};

	/* ------------------------------------------------------------------- */

	LRESULT CStackingDlg::OnCheckItem(WPARAM, LPARAM)
	{
		updateListInfo();

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
		CSplitterControl::ChangeHeight(&frameList, -pHdr->delta, CW_BOTTOMALIGN);
		CSplitterControl::ChangePos(&m_ListInfo, 0, pHdr->delta);
		Invalidate();
		UpdateWindow();
		m_Picture.Invalidate();
		frameList.Invalidate();
		m_ListInfo.Invalidate();
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::UncheckNonStackablePictures()
	{
		frameList.UnCheckNonStackable();
	};


	/* ------------------------------------------------------------------- */


	/* ------------------------------------------------------------------- */

	/* ------------------------------------------------------------------- */

	/* ------------------------------------------------------------------- */

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
					frameList.CheckBest(fThreshold);
				else
					frameList.CheckAbove(fThreshold);
			};
			updateListInfo();
		};
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::CheckAll()
	{
		frameList.CheckAll(TRUE);
		frameList.CheckAllDarks(TRUE);
		updateListInfo();
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::UncheckAll()
	{
		frameList.CheckAll(FALSE);
		frameList.CheckAllDarks(FALSE);
		updateListInfo();
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::ClearList()
	{
		if (CheckEditChanges() && checkWorkspaceChanges())
		{
			frameList.Clear();
			m_Picture.SetImg((CBitmap*)nullptr);
			m_Picture.SetImageSink(nullptr);
			m_Picture.SetButtonToolbar(nullptr);
			m_EditStarSink.SetBitmap(std::shared_ptr<CMemoryBitmap>{});
			m_strShowFile.Empty();
			m_Infos.SetText(m_strShowFile);
			m_BackgroundLoading.ClearList();
			m_LoadedImage.Clear();
			UpdateGroupTabs();
			updateListInfo();
			m_strCurrentFileList.Empty();
			dssApp->setTitleFilename(m_strCurrentFileList);
		};
	};

	/* ------------------------------------------------------------------- */

	void CStackingDlg::CheckBests(double fPercent)
	{
		if (checkEditChanges())
			frameList.CheckBest(fPercent);
	};

	/* ------------------------------------------------------------------- */



	/* ------------------------------------------------------------------- */

	void CStackingDlg::OnBnClicked4corners()
	{
		m_Picture.Set4CornersMode(!m_Picture.Get4CornersMode());
	}

	/* ------------------------------------------------------------------- */

	#endif
}