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
#include <QClipboard>
#include <QComboBox>
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
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>
#include <QStyleOptionButton>
#include <QTableWidgetItem>
#include <QTimeEdit>
#include <QTimer>
#include <QToolTip>
#include <QUrl>

#include <filesystem>

#include "mrupath.h"

#include "DeepSkyStacker.h"
#include "dssrect.h"
#include "StackingDlg.h"
#include "ProcessingDlg.h"
#include "DeepStack.h"
#include "FileProperty.h"
#include "FrameInfoSupport.h"
#include "QtProgressDlg.h"
#include "CheckAbove.h"
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
#include "group.h"
#include "editstars.h"
#include "selectrect.h"
#include "toolbar.h"
#include "avx_support.h"
#include "ui/ui_StackingDlg.h"
#include "picturelist.h"
#include "RenameGroup.h"

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

enum class Menuitem
{
	markAsReference,
	check,
	uncheck,
	remove,
	copy,
	erase
};
namespace DSS
{
	constexpr	DWORD					IDC_EDIT_SELECT = 1;
	constexpr	DWORD					IDC_EDIT_STAR   = 2;
	constexpr	DWORD					IDC_EDIT_COMET  = 3;
	constexpr	DWORD					IDC_EDIT_SAVE   = 4;

	static struct { const char* const source; const char* comment; } OUTPUTLIST_FILTER_SOURCES[]{
		QT_TRANSLATE_NOOP3("DSS", "File List (*.dssfilelist)", "IDS_LISTFILTER_OUTPUT"),
		QT_TRANSLATE_NOOP3("DSS", "File List (*.txt)", "IDS_LISTFILTER_OUTPUT"),
		QT_TRANSLATE_NOOP3("DSS", "All Files (*)", "IDS_LISTFILTER_OUTPUT")
		};

	QStringList OUTPUTLIST_FILTERS{};

	static struct { const char* const source; const char* comment; } INPUTFILE_FILTER_SOURCES [] {
		QT_TRANSLATE_NOOP3("DSS",
			"Picture Files (*.bmp *.jpg *.jpeg *.tif *.tiff *.png *.fit *.fits *.fts "
			"*.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr *.kdc *.srf "
			"*.arw *.raw *.dng *.ia *.rw2)", "IDS_FILTER_INPUT"),
		QT_TRANSLATE_NOOP3("DSS", "Windows Bitmaps (*.bmp)", "IDS_FILTER_INPUT"),
		QT_TRANSLATE_NOOP3("DSS", "JPEG or PNG Files (*.jpg *.jpeg *.png)", "IDS_FILTER_INPUT"),
		QT_TRANSLATE_NOOP3("DSS", "TIFF Files (*.tif *.tiff)", "IDS_FILTER_INPUT"),
		QT_TRANSLATE_NOOP3("DSS",
			"RAW Files (*.cr2 *.cr3 *.crw *.nef *.mrw *.orf *.raf *.pef *.x3f *.dcr "
			"*.kdc *.srf *.arw *.raw *.dng *.ia *.rw2)", "IDS_FILTER_INPUT"),
		QT_TRANSLATE_NOOP3("DSS", "FITS Files (*.fits *.fit *.fts)", "IDS_FILTER_INPUT"),
		QT_TRANSLATE_NOOP3("DSS", "All Files (*)", "IDS_FILTER_INPUT")
		};

	QStringList INPUTFILE_FILTERS{};

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
		
		checkBoxStyle.state = (Qt::Checked == opt.checkState) ? QStyle::State_On : QStyle::State_Off;
		style->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &checkBoxStyle, painter);

		//
		// Draw the icon 2.5 times as large as the default
		//
		QSize iconSize = opt.decorationSize;
		iconSize.scale(2.5 * iconSize.width(), 2.5 * iconSize.height(), Qt::KeepAspectRatio);
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

	bool ItemEditDelegate::eventFilter(QObject* watched, QEvent* event)
	{
		if (QEvent::Show == event->type())
		{
			QTimeEdit* timeEdit{ dynamic_cast<QTimeEdit*>(watched) };
			QShowEvent* showEvent{ static_cast<QShowEvent*>(event) };
			if (timeEdit)
				if (!showEvent->spontaneous())
			{
				QTimer::singleShot(100,
					[timeEdit]()
				{
					timeEdit->setSelectedSection(QDateTimeEdit::MinuteSection);
				});
			}
		}
		//
		// Always call the base class event filter (even though it currently 
		// only handles stuff for TextEdit controls)
		//
		return Inherited::eventFilter(watched, event);
	}

	QWidget* ItemEditDelegate::createEditor(QWidget* parent,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const
	{
		static QStringList types;
		if (types.isEmpty())
			types << 
				QCoreApplication::translate("DSS::Group", "Light", "IDS_TYPE_LIGHT") <<
				QCoreApplication::translate("DSS::Group", "Dark", "IDS_TYPE_DARK") <<
				QCoreApplication::translate("DSS::Group", "Flat", "IDS_TYPE_FLAT") <<
				QCoreApplication::translate("DSS::Group", "Dark Flat", "IDS_TYPE_DARKFLAT") <<
				QCoreApplication::translate("DSS::Group", "Bias/Offset", "IDS_TYPE_OFFSET");
		static QStringList isos;
		if (isos.isEmpty())
			isos << "100" << "125" << "160" << "200" << "250" << "320" << "400" <<
				"500" << "640" << "800" << "1000" << "1250" << "1600" << "3200" <<
				"6400" << "12800";

		switch (static_cast<Column>(index.column()))
		{
		case Column::Type:
			{
				QComboBox* editor = new QComboBox(parent);
				editor->addItems(types);
				return editor;
			}
			break;
		case Column::ISO:
			{
				QComboBox* editor = new QComboBox(parent);
				editor->addItems(isos);
				editor->setEditable(true);
				editor->setValidator(new QIntValidator(1, 256000, parent));
				return editor;
			}
			break;
		case Column::Exposure:
			{
				QTimeEdit* editor = new QTimeEdit(parent);
				editor->setDisplayFormat("hh:mm:ss.zzz");
				//
				// Commented out as parent class already sets an eventFilter up!
				//
				// editor->installEventFilter(const_cast<ItemEditDelegate*>(this));
				
				return editor;
			}
		break;
		}
		//
		// Not one we want to handle so return the default
		//
		return Inherited::createEditor(parent, option, index);
	}

	void ItemEditDelegate::setEditorData(QWidget* editor,
		const QModelIndex& index) const
	{
		QComboBox* combo{ nullptr };
		QTimeEdit* timeEdit{ nullptr };

		switch (static_cast<Column>(index.column()))
		{
		case Column::Type:
			{
				QString type{ index.model()->data(index).toString() };
				combo = qobject_cast<QComboBox*>(editor);
				Q_ASSERT(combo);
				combo->setCurrentIndex(combo->findText(type));
			}
			break;
		case Column::ISO:
			{
				QString value{ index.model()->data(index, Qt::EditRole).toString() };
				combo = qobject_cast<QComboBox*>(editor);
				Q_ASSERT(combo);
				if (int index = combo->findText(value))
					combo->setCurrentIndex(index);
				else
					combo->setCurrentText(value);
			}
			break;

		case Column::Exposure:
			{
				double secs{ index.model()->data(index, Qt::EditRole).toDouble() };
				if (secs > 66399.999) secs = 86399.999;		// 24 hours less 1 ms
				double msecs = secs * 1000.0;
				timeEdit = qobject_cast<QTimeEdit*>(editor);
				Q_ASSERT(timeEdit);
				QTime time{ QTime(0, 0) };
				time = time.addMSecs(msecs);
				timeEdit->setTime(time);
				//
				// timeEdit->setSelectedSection(QDateTimeEdit::MinuteSection);
				// didn't work when invoked here, so set a singleshot timer in
				// the eventFilter (above) to do it after 100 ms
				//
			}
			break;
		default:
			Inherited::setEditorData(editor, index);
		}
	}

	void ItemEditDelegate::setModelData(QWidget* editor,
		QAbstractItemModel* model,
		const QModelIndex& index) const
	{
		QComboBox* combo { nullptr };
		QTimeEdit* timeEdit { nullptr };
		switch (static_cast<Column>(index.column()))
		{
		case Column::Type:
			{
				combo = qobject_cast<QComboBox*>(editor);
				Q_ASSERT(combo);
				model->setData(index, combo->currentIndex());
			}
			break;
		case Column::ISO:
			{
				combo = qobject_cast<QComboBox*>(editor);
				Q_ASSERT(combo);
				model->setData(index, combo->currentText());
			}
			break;
		case Column::Exposure:
			{
				timeEdit = qobject_cast<QTimeEdit*>(editor);
				Q_ASSERT(timeEdit);
				QTime time{ timeEdit->time() };
				double secs = (static_cast<double>(time.hour()) * 3600) +
					(static_cast<double>(time.minute()) * 60) +
					(time.second() +
					(static_cast<double>(time.msec()) / 1000.0));
				model->setData(index, secs);
			}
			break;
		default:
			Inherited::setModelData(editor, model, index);
		}
	}

	void ItemEditDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		Inherited::paint(painter, option, index);
		QStyleOptionViewItem opt{ option };
		initStyleOption(&opt, index);

		QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

		//
		// Draw a single pixel wide rounded rectangle around the text to show this
		// cell is editable.
		//
		painter->save();
		QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt);
		textRect.adjust(1,1, -1, -1);
		painter->setPen(QPen(QColorConstants::Svg::lightblue));
		painter->drawRect(textRect);
		painter->restore();
	}

	/////////////////////////////////////////////////////////////////////////////
	// StackingDlg dialog

	StackingDlg::StackingDlg(QWidget* parent, PictureList* pictures) :
		QWidget(parent),
		pictureList{ pictures },
		ui(new Ui::StackingDlg),
		initialised(false),
		markAsReference{ nullptr },
		check{ nullptr },
		uncheck{ nullptr },
		toLight{ nullptr },
		toDark{ nullptr },
		toDarkFlat{ nullptr },
		toFlat{ nullptr },
		toOffset{ nullptr },
		remove{ nullptr },
		properties{ nullptr },
		copy{ nullptr },
		erase{ nullptr },
		networkManager{ nullptr },
		m_tipShowCount{ 0 },
		dockTitle{ new QLabel(this) }
	{
		ui->setupUi(this);
		retranslateUi();		// translate some of our stuff.

		mruPath.readSettings();

		connect(ui->fourCorners, SIGNAL(clicked(bool)), ui->picture, SLOT(on_fourCorners_clicked(bool)));
		connect(&imageLoader, SIGNAL(imageLoaded()), this, SLOT(imageLoad()));
		connect(ui->gamma, SIGNAL(pegMove(int)), this, SLOT(gammaChanging(int)));
		connect(ui->gamma, SIGNAL(pegMoved(int)), this, SLOT(gammaChanged(int)));
		//
		// If user changes tab need to switch groups and table model
		//
		connect(pictureList->tabBar, SIGNAL(currentChanged(int)), this, SLOT(tabBar_currentChanged(int)));

		//
		// Handle Menu requests for PictureList tableView and tabBar controls
		//
		connect(pictureList->tableView, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(tableView_customContextMenuRequested(const QPoint&)));
		connect(pictureList->tabBar, SIGNAL(customContextMenuRequested(const QPoint&)),
			this, SLOT(tabBar_customContextMenuRequested(const QPoint&)));

	}

	StackingDlg::~StackingDlg()
	{
		delete ui;
	}

	void StackingDlg::setSelectionRect(const QRectF& rect)
	{
		selectRect = DSSRect(rect.x(), rect.y(), rect.right(), rect.bottom());
	}

	bool StackingDlg::eventFilter(QObject* watched, QEvent* event)
	{
		if (pictureList->tableView == watched)
		{
			if (QEvent::KeyPress == event->type())
			{
				int i{ 0 };
				QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
				const QKeySequence received(keyEvent->key() | keyEvent->modifiers());
				QItemSelectionModel* qsm = pictureList->tableView->selectionModel();
				ImageListModel* imageModel{ frameList.currentTableModel() }; 

				//
				// Was it the Space Bar?  If so toggle the checked state of all selected items
				//
				if (Qt::Key_Space == keyEvent->key() && Qt::NoModifier == keyEvent->modifiers())
				{
					QModelIndexList selectedRows = qsm->selectedRows();

					int rowCount = selectedRows.size();

					//
					// If the QSortFilterProxyModel is being used, need to map 
					// to the imageModel index in the base imageModel (our ImageListModel)
					//
					if (pictureList->tableView->model() == proxyModel.get())
					{
						for (i = 0; i < rowCount; i++)
						{
							selectedRows[i] = proxyModel->mapToSource(selectedRows[i]);
						}
					}

					for (i = 0; i < rowCount; i++)
					{
						int row = selectedRows[i].row();

						if (Qt::Checked == imageModel->mydata[row].m_bChecked)
							imageModel->mydata[row].m_bChecked = Qt::Unchecked;
						else
							imageModel->mydata[row].m_bChecked = Qt::Checked;

						imageModel->emitChanged(row, row, static_cast<int>(Column::Path), static_cast<int>(Column::Path));
					}
					return true;
				}

				//
				// Was it the Delete key? If so, remove all selected rows
				//
				if (Qt::Key_Delete == keyEvent->key() && Qt::NoModifier == keyEvent->modifiers())
				{
					QModelIndexList selectedRows = qsm->selectedRows();

					int rowCount = selectedRows.size();

					//
					// If the QSortFilterProxyModel is being used, need to map 
					// to the imageModel index in the base imageModel (our ImageListModel)
					//
					if (pictureList->tableView->model() == proxyModel.get())
					{
						for (i = 0; i < rowCount; i++)
						{
							selectedRows[i] = proxyModel->mapToSource(selectedRows[i]);
						}
					}

					//
					// Sort the list of QModelIndex in descending order so that 
					// when we iterate over the list we will delete the rows 
					// with higher row numbers first.
					//
					std::sort(selectedRows.rbegin(), selectedRows.rend());

					for (i = 0; i < rowCount; i++)
					{
						int row = selectedRows[i].row();
						frameList.removeFromMap(imageModel->mydata[row].filePath);
						imageModel->beginRemoveRows(QModelIndex(), row, row);
						imageModel->removeRows(row, 1);
						imageModel->endRemoveRows();
					}
					return true;
				}
			}
		}
		return Inherited::eventFilter(watched, event);
	}
	
	void StackingDlg::changeEvent(QEvent* event)
	{
		if (event->type() == QEvent::LanguageChange)
		{
			ui->retranslateUi(this);
			retranslateUi();		// translate some of our stuff.
		}

		Inherited::changeEvent(event);
	}


	void StackingDlg::retranslateUi()
	{
		OUTPUTLIST_FILTERS.clear();
		int i = 0;
		int count = sizeof(OUTPUTLIST_FILTER_SOURCES) / sizeof(OUTPUTLIST_FILTER_SOURCES[0]);
		for (i = 0; i < count; ++i)
		{
			OUTPUTLIST_FILTERS.append(
				qApp->translate("DSS", OUTPUTLIST_FILTER_SOURCES[i].source, OUTPUTLIST_FILTER_SOURCES[i].comment));
		}
		Q_ASSERT(OUTPUTLIST_FILTERS.size() == count);

		count = sizeof(INPUTFILE_FILTER_SOURCES) / sizeof(INPUTFILE_FILTER_SOURCES[0]);
		INPUTFILE_FILTERS.clear();
		for (i = 0; i < count; ++i)
		{
			INPUTFILE_FILTERS.append(
				qApp->translate("DSS", INPUTFILE_FILTER_SOURCES[i].source, INPUTFILE_FILTER_SOURCES[i].comment));
		}
		Q_ASSERT(INPUTFILE_FILTERS.size() == count);

		pictureList->tableView->viewport()->setToolTip(tr("Space Bar to check/uncheck selected rows\n"
			"Ctrl-A or equivalent to select all rows\n"
			"Delete key to remove (not erase) selected rows\n"
			"Right mouse button to display the menu"));

		QString	text{ tr("Light Frames: %1      -      Dark Frames: %2      -      Flat Frames: %3      -   Dark Flat Frames: %4   -      Offset/Bias Frames: %5",
			"IDS_LISTINFO")
			.arg(frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_DARKFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_FLATFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_DARKFLATFRAME))
			.arg(frameList.checkedImageCount(PICTURETYPE_OFFSETFRAME))
		};
		dockTitle->setText(text);
		dockTitle->setToolTip(tr("Double click here to dock/undock the image list"));

		pictureList->tabBar->setTabText(0, tr("Main Group", "IDS_MAINGROUP"));
	}



	bool StackingDlg::event(QEvent* event)
	{
		if (QEvent::ToolTip == event->type())
		{
			//
			// If the mouse is over the image, but not over the toolbar,
			// get the tooltip text and if there is any, display it
			//
			const QPoint globalMouseLocation(QCursor::pos());
			const QPointF mouseLocation(mapFromGlobal(globalMouseLocation));
			if (pictureList->tableView->viewport()->underMouse())
			{
				const QString tip = toolTip();
				if (!tip.isEmpty() && m_tipShowCount % 25 == 0)
				{
					QToolTip::showText(globalMouseLocation, tip, this);
				}
				m_tipShowCount++;
			}
			return true;
		}
		// Make sure the rest of events are handled
		return Inherited::event(event);
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

	//
	// Copy the Table View header and rows to the clipboard as tab delimited data
	//
	void StackingDlg::copyToClipboard()
	{
		int i{ 0 }, j{ 0 };
		ImageListModel* model{ frameList.currentTableModel() };
		QString str;

		for (i = 0; i < model->columnCount(); i++)
		{
			if (i)
				str += "\t";

			str += model->headerData(i, Qt::Horizontal).toString();
		}
		for (i = 0; i < model->rowCount(); i++)
		{
			str += "\n";
			for (j = 0; j < model->columnCount(); j++)
			{
				if (j)
					str += "\t";
				QModelIndex ndx{ model->createIndex(i, j) };

				if (pictureList->tableView->model() == proxyModel.get())
					ndx = proxyModel->mapFromSource(ndx);
				str += model->data(ndx, Qt::DisplayRole).toString();
			}

		}

		QApplication::clipboard()->setText(str);

	}

	void StackingDlg::tabBar_customContextMenuRequested(const QPoint& pos)
	{
		ZFUNCTRACE_RUNTIME();
		auto tab = pictureList->tabBar->tabAt(pos);
		if (tab > 0)
		{
			QMenu tabMenu;
			QAction* rename = tabMenu.addAction(tr("Rename group"));
			const QAction* a = tabMenu.exec(QCursor::pos());
			if (a == rename)
			{
				qDebug() << "Rename requested";
				RenameGroup dlg{ this, frameList.groupName(tab) };
				if (dlg.exec())
				{
					QString newName{ dlg.lineEdit->text() };
					frameList.setGroupName(tab, newName);
					pictureList->tabBar->setTabText(tab, newName);
				}
			}
		}
	}

	void StackingDlg::tableView_customContextMenuRequested(const QPoint& pos)
	{
		ZFUNCTRACE_RUNTIME();

		QModelIndex ndx = pictureList->tableView->indexAt(pos);
		int i{ 0 };

		//
		// If the QSortFilterProxyModel is being used, need to map 
		// to the model index in the base model (our ImageListModel)
		//
		if (pictureList->tableView->model() == proxyModel.get())
			ndx = proxyModel->mapToSource(ndx);

		ImageListModel* imageModel = frameList.currentTableModel();
		int row = ndx.row();
		bool indexValid = ndx.isValid();

		if (indexValid)
		{
			if (imageModel->mydata[row].m_bUseAsStarting)
				markAsReference->setChecked(true);
			else
				markAsReference->setChecked(false);

			markAsReference->setEnabled(true);
			check->setEnabled(true);
			uncheck->setEnabled(true);
			remove->setEnabled(true);
			erase->setEnabled(true);
		}
		else
		{
			markAsReference->setEnabled(false);
			check->setEnabled(false);
			uncheck->setEnabled(false);
			remove->setEnabled(false);
			erase->setEnabled(false);
		}


		QAction* action = menu.exec(pictureList->tableView->mapToGlobal(pos));
		if (!action)
			return;
		Menuitem item = static_cast<Menuitem>(action->data().toInt());

		QItemSelectionModel* qsm = pictureList->tableView->selectionModel();
		QModelIndexList selectedRows = qsm->selectedRows();

		int rowCount = selectedRows.size();

		//
		// If the QSortFilterProxyModel is being used, need to map 
		// to the model index in the base model (our ImageListModel)
		//
		if (pictureList->tableView->model() == proxyModel.get())
		{
			for (i = 0; i < rowCount; i++)
			{
				selectedRows[i] = proxyModel->mapToSource(selectedRows[i]);
			}
		}

		//
		// Sort the list of QModelIndex in descending order so that 
		// when we iterate over the list we will delete the rows 
		// with higher row numbers first.
		//
		std::sort(selectedRows.rbegin(), selectedRows.rend());

		bool eraseOK = false;
		if (Menuitem::erase == item)
		{
			QString message{ tr("Do you really want to permanently erase %n file(s)?\n"
				"This operation cannot be reversed or cancelled.",
				"IDS_WARNING_ERASEFILES", rowCount) };
			auto result = QMessageBox::question(this, "DeepSkyStacker",
				message, (QMessageBox::Yes | QMessageBox::No), QMessageBox::No );
			if (QMessageBox::Yes == result)
				eraseOK = true;
		}

		if (Menuitem::copy == item)
		{
			copyToClipboard();
		}
		else
		{
			//
			// Iterate over the selected items doing whatever needs to be done
			//
			for (i = 0; i < rowCount; i++)
			{
				int row = selectedRows[i].row();

				switch (item)
				{
				case Menuitem::markAsReference:
					//
					// Toggle the value
					//
					imageModel->mydata[row].m_bUseAsStarting ^= true;
					imageModel->emitChanged(row, row, static_cast<int>(Column::Score), static_cast<int>(Column::Score));
					break;
				case Menuitem::check:
					imageModel->mydata[row].m_bChecked = Qt::Checked;
					imageModel->emitChanged(row, row, static_cast<int>(Column::Path), static_cast<int>(Column::Path));
					break;
				case Menuitem::uncheck:
					imageModel->mydata[row].m_bChecked = Qt::Unchecked;
					imageModel->emitChanged(row, row, static_cast<int>(Column::Path), static_cast<int>(Column::Path));
					break;
				case Menuitem::erase:
					if (eraseOK)
					{
						fs::remove(imageModel->mydata[row].filePath); // erase the file
					}
					break;
				}
			}

			if (Menuitem::remove == item || Menuitem::erase == item)
			{
				for (i = 0; i < rowCount; i++)
				{
					int row = selectedRows[i].row();
					frameList.removeFromMap(imageModel->mydata[row].filePath);
					imageModel->beginRemoveRows(QModelIndex(), row, row);
					imageModel->removeRows(row, 1);
					imageModel->endRemoveRows();
				}
			}
		}
	}

	void StackingDlg::onInitDialog()
	{
		ZFUNCTRACE_RUNTIME();
		// 
		//
		// Build the context menu for the tableview (list of images).
		//
		markAsReference = menu.addAction(tr("Use as reference frame", "IDM_USEASSTARTING"));
		markAsReference->setCheckable(true);
		markAsReference->setData(int(Menuitem::markAsReference));
		menu.addSeparator();
		check = menu.addAction(tr("Check", "IDM_CHECK"));
		check->setData(int(Menuitem::check));
		uncheck = menu.addAction(tr("Uncheck", "IDM_UNCHECK"));
		uncheck->setData(int(Menuitem::uncheck));
		menu.addSeparator();
		remove = menu.addAction(tr("Remove from list", "IDM_REMOVEFROMLIST"));
		remove->setData(int(Menuitem::remove));
		menu.addSeparator();
		copy = menu.addAction(tr("Copy to clipboard", "IDM_COPYTOCLIPBOARD"));
		copy->setData(int(Menuitem::copy));
		menu.addSeparator();
		erase = menu.addAction(tr("Erase from disk...", "IDM_ERASEFROMDISK"));
		erase->setData(int(Menuitem::erase));

		ui->picture->setVisible(true);
		editStarsPtr = std::make_unique<EditStars>(ui->picture);
		selectRectPtr = std::make_unique<SelectRect>(ui->picture);
		pToolBar = std::make_unique<ToolBar>(this);
		pToolBar->setObjectName(QString::fromUtf8("toolBar"));
		pToolBar->setVisible(false);

		ui->picture->setToolBar(pToolBar.get());
		pToolBar->setVisible(false); pToolBar->setEnabled(false);

		//
		// Restore windowState of this and the table view's horizontal header
		//
		QSettings settings;
		pictureList->tableView->horizontalHeader()->restoreState(
			settings.value("Dialogs/PictureList/TableView/HorizontalHeader/windowState").toByteArray());
		//
		// If the model data changes let me know
		//
		connect(frameList.currentTableModel(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QList<int>&)),
			this, SLOT(tableViewModel_dataChanged(const QModelIndex&, const QModelIndex&, const QList<int>&)));
		
		//
		// Set up a QSortFilterProxyModel to allow sorting of the table view
		// (it sits between the actual model and the view to provide sorting
		// capability)
		//
		proxyModel = std::make_unique<QSortFilterProxyModel>(this);
		proxyModel->setSourceModel(frameList.currentTableModel());
		proxyModel->setSortRole(Qt::EditRole);

		pictureList->tableView->setModel(proxyModel.get());
		pictureList->tableView->setSortingEnabled(true);

		//
		// The default icon display is really rather small, so use a subclass
		// of QStyledItemDelegate to handle the rendering for column zero of 
		// the table with the icon size increased by 2.5 times.
		//
		iconSizeDelegate = std::make_unique<IconSizeDelegate>();

		pictureList->tableView->setItemDelegateForColumn(0, iconSizeDelegate.get());

		//
		// Create an edit
		//
		itemEditDelegate = std::make_unique<ItemEditDelegate>();
		pictureList->tableView->setItemDelegateForColumn(static_cast<int>(Column::Type), itemEditDelegate.get());
		pictureList->tableView->setItemDelegateForColumn(static_cast<int>(Column::ISO), itemEditDelegate.get());
		pictureList->tableView->setItemDelegateForColumn(static_cast<int>(Column::Exposure), itemEditDelegate.get());

		//
		// Reduce font size and increase weight
		//
		QFont font { pictureList->tableView->font() };
		font.setPointSize(font.pointSize() - 1); font.setWeight(QFont::Medium);
		pictureList->tableView->setFont(font);
		font = pictureList->tableView->horizontalHeader()->font();
		//font.setPointSize(font.pointSize() - 1);
		font.setWeight(QFont::Medium);
		pictureList->tableView->horizontalHeader()->setFont(font);
		pictureList->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		pictureList->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		pictureList->tableView->setAlternatingRowColors(true);
		pictureList->tableView->setTabKeyNavigation(true);
		pictureList->tableView->horizontalHeader()->setSectionsMovable(true);
		pictureList->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
		pictureList->tableView->installEventFilter(this);

		QItemSelectionModel* qsm = pictureList->tableView->selectionModel();
		connect(qsm, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
			this, SLOT(tableView_selectionChanged(const QItemSelection&, const QItemSelection&)));

		//
		// Add the Grey Point stop to the gradient 
		//
		ui->gamma->setColorAt(sqrt(0.5), QColor(qRgb(128, 128, 128)));
		ui->gamma->setPegsOnLeftOrBottom(true).
			setOrientation(QLinearGradientCtrl::Orientation::ForceHorizontal);

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
		pictureList->setTitleBarWidget(dockTitle);

		//
		// Set up the tab bar (used to be a tab widget)
		//
		pictureList->tabBar->setShape(QTabBar::TriangularSouth);
		pictureList->tabBar->setExpanding(false);
		pictureList->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
		pictureList->tabBar->setCurrentIndex(pictureList->tabBar->addTab(tr("Main Group", "IDS_MAINGROUP")));

		if (!fileList.empty())
			openFileList(fileList);			// Will call updateListInfo()
		else
			updateListInfo();
	}

	void StackingDlg::dropFiles(QDropEvent* e)
	{
		ZFUNCTRACE_RUNTIME();
		DropFilesDlg dlg(this);
		dlg.setDropInfo(e);
		if (dlg.exec())
		{
			QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

			auto files{ dlg.getFiles() };
			auto type{ dlg.dropType() };
			//
			// Before attempting to add the files prune out those that have already been loaded
			// and issue an error message
			//
			auto it = std::remove_if(files.begin(), files.end(),
				[&](const fs::path& p) { return fileAlreadyLoaded(p.generic_u16string()); });
			files.erase(it, files.end());

			if (!files.empty())		// Never, ever attempt to add zero rows!!!
			{
				frameList.beginInsertRows(static_cast<int>(files.size()));
				for (size_t i = 0; i != files.size(); ++i)
				{
					frameList.addFile(files[i], type, true);
				}
				frameList.endInsertRows();
			}
			updateGroupTabs();
			updateListInfo();
			QGuiApplication::restoreOverrideCursor();
		};

	}

	void StackingDlg::tableViewModel_dataChanged(const QModelIndex& first,
		[[maybe_unused]] const QModelIndex& last, [[maybe_unused]] const QList<int>&)
	{
		//
		// Only interested if the user has ticked the check box in column 0
		//
		if (first.isValid() && 0 == first.column())
		{
			updateListInfo();
		}
	}

	void StackingDlg::tableView_selectionChanged([[maybe_unused]] const QItemSelection& selected, [[maybe_unused]] const QItemSelection& deselected)
	{

		QItemSelectionModel * qsm = pictureList->tableView->selectionModel();
		QModelIndexList selectedRows = qsm->selectedRows();
		//
		// If only one row is selected, we want to know the filename
		//
		if (1 == selectedRows.count())
		{
			QModelIndex& ndx = selectedRows[0];

			//
			// If the QSortFilterProxyModel is being used, need to map 
			// to the model index in the base model (our ImageListModel)
			//
			if (pictureList->tableView->model() == proxyModel.get())
				ndx = proxyModel->mapToSource(ndx);

			if (ndx.isValid())
			{
				QString  fileName;
				const ImageListModel* model = dynamic_cast<const ImageListModel*>(ndx.model());
				int row = ndx.row();
				fileName = model->selectedFileName(row);
				//
				// If the filename has changed but we have changes to the stars that need to be saved
				//
				if (fileName != m_strShowFile && checkEditChanges())
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
			pictureList->tableView->setEnabled(true);
			//
			// Disabling the tableview resulted in it loosing focus
			// so put the focus back
			//
			pictureList->tableView->setFocus(Qt::OtherFocusReason);
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
			pictureList->tableView->setEnabled(false);
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
		checkAskRegister();
		editStarsPtr->starsButtonPressed();
		selectRectPtr->starsButtonPressed();
	}

	void StackingDlg::toolBar_cometButtonPressed([[maybe_unused]] bool checked)
	{
		checkAskRegister();
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
				std::wcerr << errorMessage;
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

		fileDialog.setWindowTitle(tr("Open Light Frames...", "IDS_TITLE_OPENLIGHTFRAMES"));
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
				[&](const QString& s) {
					std::u16string u16s{ s.toStdU16String() };
					fs::path path(u16s);
					return fileAlreadyLoaded(path);
				});
			files.erase(it, files.end());

			//
			// Now add the images to the end of the current group in the frame list remembering
			// that we need to bracket the code  that adds the with beginInsertRows(rowcount) and
			// endInsertRows(), so the table model is informed of the new rows
			//
			if (!files.empty())		// Never, ever attempt to add zero rows!!!
			{
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
			}
			
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

			updateGroupTabs();
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

		fileDialog.setWindowTitle(tr("Open Dark Frames...", "IDS_TITLE_OPENDARKFRAMES"));
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
			if (!files.empty())		// Never, ever attempt to add zero rows!!!
			{
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
			}

			QGuiApplication::restoreOverrideCursor();

			//
			// What filter has the user actually selected, or has been auto-selected?
			// Note that the index value we store is 1 based, not zero based, so add one to the selected index.
			// 
			filterIndex = INPUTFILE_FILTERS.indexOf(fileDialog.selectedNameFilter());
			filterIndex++;
			settings.setValue("Folders/AddDarkFolder", directory);
			settings.setValue("Folders/AddDarkExtension", extension);
			settings.setValue("Folders/AddDarkIndex", filterIndex);

			updateGroupTabs();
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

		fileDialog.setWindowTitle(tr("Open Dark Flat Frames...", "IDS_TITLE_OPENDARKFLATFRAMES"));
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
			if (!files.empty())		// Never, ever attempt to add zero rows!!!
			{
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
			}

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

			updateGroupTabs();
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

		fileDialog.setWindowTitle(tr("Open Flat Frames...", "IDS_TITLE_OPENFLATFRAMES"));
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
			if (!files.empty())		// Never, ever attempt to add zero rows!!!
			{
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
			}

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

			updateGroupTabs();
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

		fileDialog.setWindowTitle(tr("Open Bias Frames...", "IDS_TITLE_OPENBIASFRAMES"));
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
			if (!files.empty())		// Never, ever attempt to add zero rows!!!
			{
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
			}

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

			updateGroupTabs();
		}
		updateListInfo();
	}

	bool StackingDlg::checkEditChanges()
	{
		bool result = false;

		if (editStarsPtr->isDirty())
		{
			auto dlgResult { askToSaveEditChangeMode() };

			if (dlgResult == QDialogButtonBox::AcceptRole)
			{
				//
				// Save the changes as Save was pressed or defaulted
				//
				result = true;
				editStarsPtr->saveRegisterSettings();
				pToolBar->setSaveEnabled(false);
				// Update the list with the new info
				frameList.updateItemScores(m_strShowFile);
			}
			else if (dlgResult == QDialogButtonBox::DestructiveRole)
			{
				//
				// Discard changes as Discard was pressed or defaulted.
				//
				result = true;
			}
			// Must have pressed Cancel, so result remains as false
		}
		else
			result = true;

		return result;
	}

	/* ------------------------------------------------------------------- */

	void StackingDlg::updateCheckedAndOffsets(CStackingEngine& StackingEngine)
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

		dockTitle->setText(text);

		for (int i = 0; i < pictureList->tabBar->count(); i++)
		{
			text = tr("Light Frames: %1\nDark Frames: %2\nFlat Frames: %3\nDark Flat Frames: %4\nOffset/Bias Frames: %5",
				"IDS_LISTINFO2")
				.arg(frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_DARKFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_FLATFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_DARKFLATFRAME, i))
				.arg(frameList.checkedImageCount(PICTURETYPE_OFFSETFRAME, i));
			pictureList->tabBar->setTabToolTip(i, text);
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
			updateGroupTabs();
			updateListInfo();
			fileList.clear();
			ui->picture->clear();
			dssApp->setWindowFilePath(QString());
			update();
		}
	}

	void StackingDlg::checkAbove()
	{
		if (checkEditChanges())
		{
			CheckAbove		dlg;
			double			fThreshold;

			if (dlg.exec() == QDialog::Accepted)
			{
				fThreshold = dlg.threshold();
				if (dlg.isPercent())
					frameList.checkBest(fThreshold);
				else
					frameList.checkAbove(fThreshold);
			};
			updateListInfo();
		};
	};

	/* ------------------------------------------------------------------- */
	
	void StackingDlg::loadList(const QPoint& pt)
	{
		bool openAnother{ true };

		ZFUNCTRACE_RUNTIME();
		if (checkWorkspaceChanges())
		{
			if (mruPath.paths.size())
			{
				openAnother = false;
				QMenu menu(this);

				for (uint32_t i = 0; i < mruPath.paths.size(); i++)
				{
					fs::path path{ mruPath.paths[i] };
					fs::path display{ path };
					display.replace_extension();
					// Get the fileid without the extension

					QString name{ QString::fromStdU16String(display.generic_u16string()) };

					QAction* action = menu.addAction(name);
					action->setData(i);		// Index into the list of files

				};
				menu.addSeparator();
				QAction* loadAnother = menu.addAction(tr("Open another File List...", "ID_FILELIST_OPENANOTHERFILELIST"));

				QAction* a = menu.exec(pt);

				if (loadAnother == a)
					openAnother = true;
				else
				{
					//
					// Use the index we stored with the Menu Action to access the full fileid
					//
					fs::path path{ mruPath.paths[a->data().toUInt()] };

					frameList.loadFilesFromList(path);
					mruPath.Add(path);
					fileList = path;
					dssApp->setWindowFilePath(QString::fromStdU16String(path.generic_u16string()));
				}
			}
		}

		if (openAnother)
		{
			QString name;
			loadList(mruPath, name);
			dssApp->setWindowFilePath(name);
		};
		updateGroupTabs();
		updateListInfo();
		raise(); show();
	}

	void StackingDlg::loadList(MRUPath& MRUList, QString& strFileList)
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
				MRUList.Add(file);

				if (file.has_parent_path())
					directory = QString::fromStdU16String(file.parent_path().generic_u16string());
				else
					directory = QString::fromStdU16String(file.root_path().generic_u16string());

				extension = QString::fromStdU16String(file.extension().generic_u16string());
			}

			QGuiApplication::restoreOverrideCursor();

			const auto selectedIndex = OUTPUTLIST_FILTERS.indexOf(fileDialog.selectedNameFilter());

			settings.setValue("Folders/ListFolder", directory);
			settings.setValue("Folders/ListIndex", static_cast<uint>(selectedIndex));
			settings.setValue("Folders/ListExtension", extension);
		}
	}

	void StackingDlg::saveList()
	{
		QString name;

		saveList(mruPath, name);
		dssApp->setWindowFilePath(name);
	};

	/* ------------------------------------------------------------------- */

	void StackingDlg::saveList(MRUPath& MRUList, QString& strFileList)
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
				MRUList.Add(file);
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
		DSS::DSSProgressDlg			dlg;
		::RegisterSettings		dlgSettings(this);
		bool					bContinue = true;
		const auto start{ std::chrono::steady_clock::now() };


		bool					bFound = false;

		if (frameList.checkedImageCount(PICTURETYPE_LIGHTFRAME))
		{
			emit statusMessage("");
			//CString				strFirstLightFrame;

			//frameList.GetFirstCheckedLightFrame(strFirstLightFrame);

			//dlgSettings.SetForceRegister(!frameList.GetNrUnregisteredCheckedLightFrames());
			//dlgSettings.SetNoDark(!frameList.GetNrCheckedDarks());
			//dlgSettings.SetNoFlat(!frameList.GetNrCheckedFlats());
			//dlgSettings.SetNoOffset(!frameList.GetNrCheckedOffsets());
			//dlgSettings.SetFirstLightFrame(strFirstLightFrame);

			CAllStackingTasks	tasks;

			frameList.fillTasks(tasks);

			// Set the selection rectangle if needed.   It is set by Qt signal from DSSSelectRect.cpp
			if (!selectRect.isEmpty())
			{
				tasks.SetCustomRectangle(selectRect);
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
						bContinue = checkStacking(tasks);
						if (bContinue)
							bContinue = showRecap(tasks);
					}
					else
					{
						bContinue = checkStacking(tasks);
					};

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
					emit statusMessage(message);
					QApplication::beep();
					ZTRACE_RUNTIME(message);

					if (bContinue && bStackAfter)
					{
						doStacking(tasks, fPercent);
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

			emit statusMessage("");

			frameList.fillTasks(tasks);

			// Set the selection rectangle if needed.   It is set by Qt signal from DSSSelectRect.cpp
			if (!selectRect.isEmpty())
			{
				tasks.SetCustomRectangle(selectRect);
			}

			if (checkReadOnlyFolders(tasks))
			{
				bContinue = checkStacking(tasks);
				if (bContinue)
					bContinue = showRecap(tasks);
				if (bContinue)
				{
					// GetDeepStackerDlg(nullptr)->PostMessage(WM_PROGRESS_INIT); TODO

					imageLoader.clearCache();
					if (frameList.countUnregisteredCheckedLightFrames())
					{
						CRegisterEngine	RegisterEngine;
						DSS::DSSProgressDlg	dlg;

						frameList.blankCheckedItemScores();
						bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
						frameList.updateCheckedItemScores();
						dlg.Close();
					};

					if (bContinue)
						doStacking(tasks);

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

	bool StackingDlg::checkStacking(CAllStackingTasks& tasks)
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

	void StackingDlg::checkAskRegister()
	{
		// Check that the current light frame is registered (or not)
		// and ask accordingly
		CLightFrameInfo			lfi;

		lfi.SetBitmap(m_strShowFile.toStdU16String(), FALSE, FALSE);
		if (!lfi.IsRegistered())
		{
			CAskRegistering		dlg;

			if (dlg.DoModal() == IDOK)
			{
				if (dlg.GetAction() == ARA_ONE)
				{
					// Register only this light frame
					frameList.checkAllLights(FALSE);
					frameList.checkImage(m_strShowFile, TRUE);
					registerCheckedImages();
				}
				else if (dlg.GetAction() == ARA_ALL)
				{
					// Register all the checked light frames (including this one).
					frameList.checkImage(m_strShowFile, TRUE);
					registerCheckedImages();
				};
			};
		};
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
							saveList(mruPath, name);
						}

						dssApp->setWindowFilePath(name);
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

	void StackingDlg::doStacking(CAllStackingTasks& tasks, const double fPercent)
	{
		ZFUNCTRACE_RUNTIME();

		bool bContinue = true;
		DSS::DSSProgressDlg dlg;
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
			ZTRACE_RUNTIME(message);

			//
			// Get current status message and append the stacking time 
			//
			QString statusMsg = dssApp->statusMessage();
			if (statusMsg.isEmpty())
			{
				emit statusMessage(message);
			}
			else
			{
				statusMsg.append(" : ").append(message);
				emit statusMessage(statusMsg);
			}
			QApplication::beep();

			updateCheckedAndOffsets(StackingEngine);

			if (bContinue)
			{
				CString strFileName;
				Workspace workspace;
				const auto iff = (INTERMEDIATEFILEFORMAT)workspace.value("Stacking/IntermediateFileFormat", (uint)IFF_TIFF).toUInt();

				if (StackingEngine.GetDefaultOutputFileName(strFileName, fileList.generic_wstring().c_str(), iff == IFF_TIFF))
				{
					StackingEngine.WriteDescription(tasks, strFileName);

					const QString strText(tr("Saving Final image in %1", "IDS_SAVINGFINAL").arg(QString::fromWCharArray(strFileName.GetString())));
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
				mruPath.Add(file);
				updateGroupTabs();
				updateListInfo();
				dssApp->setWindowFilePath(QString::fromStdU16String(file.generic_u16string().c_str()));
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
			std::wcerr << errorMessage.toStdWString().c_str();
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

			if (frameList.countUnregisteredCheckedLightFrames())
			{
				CRegisterEngine	RegisterEngine;
				DSS::DSSProgressDlg	dlg;

				frameList.blankCheckedItemScores();
				bContinue = RegisterEngine.RegisterLightFrames(tasks, FALSE, &dlg);
				frameList.updateCheckedItemScores();
				dlg.Close();
			};

			if (bContinue)
			{
				DSS::DSSProgressDlg			dlg;
				CStackingEngine			StackingEngine;

				QString referenceFrame{ frameList.getReferenceFrame() };
				if (!referenceFrame.isEmpty())
					StackingEngine.SetReferenceFrame(referenceFrame.toStdWString().c_str());

				QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
				StackingEngine.ComputeOffsets(tasks, &dlg);

				// For each light frame - update the offset in the list
				updateCheckedAndOffsets(StackingEngine);

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
		CBatchStacking			dlg; // TODO

		//dlg.setMRUList(mruPath); TODO use MRUPath
		dlg.DoModal();
	};

	void StackingDlg::gammaChanging(int peg)
	{
		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ ui->gamma->gradient() };
		QGradientStops stops{ gradient.stops() };

		//
		// Adjust stop values if necessary
		//
		Q_ASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;
		bool adjust{ false };

		switch (peg)
		{
		case 1:
			// Black point moving
			if (blackPoint > whitePoint - 0.02)
			{
				blackPoint = whitePoint - 0.02;
				adjust = true;
			};
			if (blackPoint > greyPoint - 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			break;
		case 2:
			// Gray point moving
			if (greyPoint < blackPoint + 0.01)
			{
				greyPoint = blackPoint + 0.01;
				adjust = true;
			};
			if (greyPoint > whitePoint - 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		case 3:
			// White point moving
			if (whitePoint < blackPoint + 0.02)
			{
				whitePoint = blackPoint + 0.02;
				adjust = true;
			};
			if (whitePoint < greyPoint + 0.01)
			{
				greyPoint = whitePoint - 0.01;
				adjust = true;
			};
			break;
		};
		if (adjust)
		{
			stops[1].first = blackPoint;
			stops[2].first = greyPoint;
			stops[3].first = whitePoint;
			gradient.setStops(stops);
			ui->gamma->update();
		};
	}

	void StackingDlg::gammaChanged(int peg)
	{
		//
		// Before applying the changes, make any corrections necessary by invoking gammaChanging 
		// on final time
		//
		gammaChanging(peg);

		double blackPoint{ 0.0 },
			greyPoint{ 0.0 },
			whitePoint{ 0.0 };

		QLinearGradient& gradient{ ui->gamma->gradient() };
		QGradientStops stops{ gradient.stops() };
		//
		// Adjust stop values if necessary
		//
		Q_ASSERT(5 == stops.size());

		blackPoint = stops[1].first;
		greyPoint = stops[2].first;
		whitePoint = stops[3].first;

		// Adjust Gamma
		m_GammaTransformation.InitTransformation(blackPoint * blackPoint, greyPoint * greyPoint, whitePoint * whitePoint);

		if (m_LoadedImage.m_pBitmap)
		{
			ApplyGammaTransformation(m_LoadedImage.m_Image.get(), m_LoadedImage.m_pBitmap.get(), m_GammaTransformation);
			// Refresh
			ui->picture->setPixmap(QPixmap::fromImage(*(m_LoadedImage.m_Image)));
		}
	}

	void StackingDlg::tabBar_currentChanged(int index)
	{
		//
		// User has changed tabs, so switch to the corresponding group
		// and set the table model for the table view accordingly
		//
		if (-1 != index)
		{
			switchGroup(index);
		}
	}

	void StackingDlg::switchGroup(int index)
	{
		frameList.setGroup(index);
		auto model{ frameList.currentTableModel() };
		connect(frameList.currentTableModel(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QList<int>&)),
			this, SLOT(tableViewModel_dataChanged(const QModelIndex&, const QModelIndex&, const QList<int>&)));
		proxyModel->setSourceModel(model);
	}

	void StackingDlg::updateGroupTabs()
	{
		auto lastGroup{ frameList.lastGroupId() };
		if (frameList.groupSize(lastGroup) != 0)
		{
			//
			// Need to add a group
			//
			lastGroup = frameList.addGroup();
			pictureList->tabBar->addTab(frameList.groupName(lastGroup));
		}

	};

	/* ------------------------------------------------------------------- */

	void StackingDlg::showImageList(bool visible)
	{
		if (pictureList->isFloating())
			pictureList->setVisible(visible);
	}
}