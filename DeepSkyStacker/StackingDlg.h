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
// StackingDlg.h : header file
//

#include "mrupath.h"
#include <CtrlCache.h>
#include <WndImage.h>
#include <BtnST.h>
#include "Label.h"
#include "StackingTasks.h"
#include "DeepStack.h"
#include <BitmapSlider.h>
#include <CustomTabCtrl.h>
#include "StackingEngine.h"
#include "imageloader.h"
#include "FrameList.h"
#include "mrulist.h"

#include "GradientCtrl.h"
#include "SplitterControl.h"

#include "ImageSinks.h"

class QNetworkAccessManager;
class QNetworkReply;

#include <QDialog>
#include <QFileDialog>
#include <QMenu>
#include <QStyledItemDelegate>

namespace DSS
{
	class Group;
	class EditStars;
	class SelectRect;
	class ToolBar;
}

namespace Ui
{
	class StackingDlg;
}

namespace std::filesystem
{
	class path;
}

class QTextOption;
class QSortFilterProxyModel;

namespace fs = std::filesystem;

namespace DSS
{

	class IconSizeDelegate : public QStyledItemDelegate
	{
		Q_OBJECT

	public:
		using QStyledItemDelegate::QStyledItemDelegate;


	protected:
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		//inline QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
		//{
		//	QSize result = QStyledItemDelegate::sizeHint(option, index);
		//	result.setHeight(result.height() * 1.2);
		//	return result;
		//}

		QString calculateElidedText(const QString& text, const QTextOption& textOption,
			const QFont& font, const QRect& textRect, const Qt::Alignment valign,
			Qt::TextElideMode textElideMode, int flags,
			bool lastVisibleLineShouldBeElided, QPointF* paintStartPosition) const;

	};

	class ItemEditDelegate :
		public QStyledItemDelegate
	{
		typedef QStyledItemDelegate
			Inherited;

		Q_OBJECT

	public:
		using QStyledItemDelegate::QStyledItemDelegate;

		bool eventFilter(QObject* watched, QEvent* event) override;

		QWidget* createEditor(QWidget* parent,
			const QStyleOptionViewItem& option,
			const QModelIndex& index) const override;

		void setEditorData(QWidget* editor,
			const QModelIndex& index) const override;

		void setModelData(QWidget* editor,
			QAbstractItemModel* model,
			const QModelIndex& index) const override;

	};


	class StackingDlg : public QWidget
	{
		typedef QWidget
			Inherited;

		Q_OBJECT

	public slots:
		void setSelectionRect(QRectF rect);
		void imageLoad();
		
		void toolBar_rectButtonPressed(bool checked);
		void toolBar_starsButtonPressed(bool checked);
		void toolBar_cometButtonPressed(bool checked);
		void toolBar_saveButtonPressed(bool checked);
		void on_tableView_customContextMenuRequested(const QPoint& pos);
		void tableView_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	public:
		explicit StackingDlg(QWidget* parent = nullptr);
		~StackingDlg();

		bool eventFilter(QObject* watched, QEvent* event) override;

		void dropFiles(QDropEvent* e);

		void		onAddPictures();
		void		onAddDarks();
		void		onAddDarkFlats();
		void		onAddFlats();
		void		onAddOffsets();

		//
		// dssfilelist operations
		//
		void		clearList();
		void		loadList();
		void		saveList();

		//
		// Check marks
		//
		void		checkAbove();
		void		checkAll();
		void		unCheckAll();

		void setFileList(const fs::path& file)
		{
			fileList = file;
		};

		void computeOffsets();

		void copyToClipboard();

		void registerCheckedImages();

		void stackCheckedImages();

		void batchStack();

		inline void fillTasks(CAllStackingTasks& tasks)
		{
			return frameList.fillTasks(tasks);
		}

		inline size_t checkedImageCount(PICTURETYPE t)
		{
			return frameList.checkedImageCount(t);
		}

		inline QString getFirstCheckedLightFrame()
		{
			return frameList.getFirstCheckedLightFrame();
		}

		inline size_t countUnregisteredCheckedLightFrames(int id = -1) const
		{
			return frameList.countUnregisteredCheckedLightFrames(id);
		}

		void reloadCurrentImage();

		void pictureChanged();

	protected:
		bool event(QEvent* event) override;
		void showEvent(QShowEvent* event) override;

	private:
		Ui::StackingDlg* ui;
		std::unique_ptr<Workspace> workspace;
		bool initialised;
		QString			m_strShowFile;
		CGammaTransformation	m_GammaTransformation;
		fs::path		fileList;
		FrameList		frameList;
		MRUList		    m_MRUList;
		std::unique_ptr<IconSizeDelegate> iconSizeDelegate;
		std::unique_ptr<ItemEditDelegate> itemEditDelegate;
		std::unique_ptr<QSortFilterProxyModel> proxyModel;
		uint m_tipShowCount;



		bool fileAlreadyLoaded(const fs::path& file);

		std::unique_ptr<EditStars> editStarsPtr;
		std::unique_ptr<SelectRect> selectRectPtr;
		std::unique_ptr<ToolBar> pToolBar;

		QRectF	selectRect;

		//QFileDialog			fileDialog;

		MRUPath			mruPath;
		QNetworkAccessManager* networkManager;			// deleted using QObject::deleteLater();

		ImageLoader		imageLoader;
		LoadedImage		m_LoadedImage;

		//
		// Popup menu for tableview
		//
		QMenu menu;
		QAction* markAsReference;
		QAction* check;
		QAction* uncheck;
		QAction* toLight;
		QAction* toDark;
		QAction* toDarkFlat;
		QAction* toFlat;
		QAction* toOffset;
		QAction* remove;
		QAction* properties;
		QAction* copy;
		QAction* erase;

		void checkAskRegister();

		void onInitDialog();

		void versionInfoReceived(QNetworkReply* reply);

		void retrieveLatestVersionInfo();

		// void updateGroupTabs(); TODO
		bool checkEditChanges();

		bool checkReadOnlyFolders(CAllStackingTasks& tasks);

		bool checkStacking(CAllStackingTasks& tasks);

		void unCheckNonStackablePictures();

		bool showRecap(CAllStackingTasks& tasks);

		bool saveOnClose();

		void doStacking(CAllStackingTasks& tasks, const double fPercent = 100.0);

		void updateCheckedAndOffsets(CStackingEngine& StackingEngine);
		
		bool checkWorkspaceChanges();
		
		void openFileList(const fs::path& file);

		void updateListInfo();

		void loadList(MRUList& MRUList, QString& strFileList);

		void saveList(MRUList& MRUList, QString& strFileList);

	};
}
