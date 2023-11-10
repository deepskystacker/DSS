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
#include "DSSCommon.h"
#include "BitmapExt.h"
#include "FrameList.h"
#include "dssrect.h"
#include "mrupath.h"
#include "imageloader.h"

class QNetworkAccessManager;
class QNetworkReply;
class CAllStackingTasks;
class CStackingEngine;

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

class QErrorMessage;
class QTextOption;
class QSortFilterProxyModel;
class QLabel;
class Workspace;

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
			Qt::TextElideMode textElideMode, [[maybe_unused]] int flags,
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

	protected:
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	};

	class PictureList;

	class StackingDlg : public QWidget
	{
		typedef QWidget
			Inherited;

		Q_OBJECT

		friend class BatchStacking;

	signals:
		void statusMessage(const QString& text);

	public slots:
		void setSelectionRect(const QRectF& rect);
		void imageLoad();
		void imageLoadFailed();
		
		void toolBar_rectButtonPressed(bool checked);
		void toolBar_starsButtonPressed(bool checked);
		void toolBar_cometButtonPressed(bool checked);
		void toolBar_saveButtonPressed(bool checked);
		void tableView_customContextMenuRequested(const QPoint& pos);
		void tabBar_customContextMenuRequested(const QPoint& pos);
		void tableView_selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
		void tableViewModel_dataChanged(const QModelIndex& first, const QModelIndex& last, const QList<int>& roles);
		void gammaChanging(int peg);
		void gammaChanged(int peg);
		void tabBar_currentChanged(int index);

		void onAddImages(PICTURETYPE type);

		//
		// dssfilelist operations
		//
		void		clearList();
		void		loadList(const QPoint&);
		void		saveList();

		//
		// Check marks
		//
		void		checkAbove();
		void		checkAll();
		void		unCheckAll();

		//
		// Registration
		//
		void registerCheckedImages();
		void computeOffsets();
		void stackCheckedImages();
		void batchStack();

	public:
		explicit StackingDlg(QWidget* parent = nullptr, PictureList* list = nullptr);
		~StackingDlg();

		bool eventFilter(QObject* watched, QEvent* event) override;

		void dropFiles(QDropEvent* e);


		void setFileList(const fs::path& file)
		{
			fileList = file;
		};

		void showImageList(bool visible = true);

		void copyToClipboard();

		inline void fillTasks(CAllStackingTasks& tasks)
		{
			return frameList.fillTasks(tasks);
		}

		inline size_t checkedImageCount(PICTURETYPE t)
		{
			return frameList.checkedImageCount(t);
		}

		inline fs::path getFirstCheckedLightFrame()
		{
			return frameList.getFirstCheckedLightFrame();
		}

		inline size_t countUnregisteredCheckedLightFrames(const int id = -1) const
		{
			return frameList.countUnregisteredCheckedLightFrames(id);
		}

		void reloadCurrentImage();

		void pictureChanged();

		bool saveOnClose();

		inline bool customRectangleIsValid() const
		{
			return !selectRect.isEmpty();
		}

	protected:
		void changeEvent(QEvent* e) override;
		bool event(QEvent* event) override;
		void showEvent(QShowEvent* event) override;

	private:
		PictureList* pictureList;
		Ui::StackingDlg* ui;
		bool initialised;
		fs::path fileToShow;
		GammaTransformation	m_GammaTransformation;
		fs::path		fileList;
		FrameList		frameList;
		IconSizeDelegate* iconSizeDelegate;
		ItemEditDelegate* itemEditDelegate;
		QSortFilterProxyModel* proxyModel;
		uint m_tipShowCount;

		bool fileAlreadyLoaded(const fs::path& file);

		EditStars* editStarsPtr;
		SelectRect* selectRectPtr;
		ToolBar* pToolBar;

		DSSRect	selectRect;

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

		QLabel* dockTitle;

		void checkAskRegister();

		void onInitDialog();

		void versionInfoReceived(QNetworkReply* reply);

		void retrieveLatestVersionInfo();

		bool checkEditChanges();

		bool checkReadOnlyFolders(CAllStackingTasks& tasks);

		bool checkStacking([[maybe_unused]] CAllStackingTasks& tasks);

		bool showRecap(CAllStackingTasks& tasks);

		void doStacking(CAllStackingTasks& tasks, const double fPercent = 100.0);

		void updateCheckedAndOffsets(CStackingEngine& StackingEngine);
		
		bool checkWorkspaceChanges();
		
		void openFileList(const fs::path& file);

		void updateGroupTabs();

		void updateListInfo();

		void switchGroup(int);

		void loadList(MRUPath& MRUList, [[maybe_unused]] QString& strFileList);

		void saveList(MRUPath& MRUList, [[maybe_unused]] QString& strFileList);

		bool isValidImage(const fs::path& path);

		void retranslateUi();
	};
}
