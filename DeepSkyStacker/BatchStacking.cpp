// BatchStacking.cpp : implementation file
//

#include "stdafx.h"
#include "BatchStacking.h"


#include "ui/ui_BatchStacking.h"

#include "StackingTasks.h"
#include "Workspace.h"
#include "FrameList.h"
#include "QtProgressDlg.h"
#include "StackingEngine.h"
#include "DeepSkyStacker.h"

#include <QStandardItemModel>
#include <QFileDialog>
#include <QDir>
#include <QSettings>
#include <QShowEvent>

namespace
{
	static bool processList(const fs::path& fileList, QString& outputFile)
	{
		ZFUNCTRACE_RUNTIME();
		bool bResult = true;
		Workspace workspace;
		CAllStackingTasks tasks;
		DSS::FrameList list;

		workspace.Push();
		list.loadFilesFromList(fileList);
		list.fillTasks(tasks);
		tasks.ResolveTasks();

		if (!tasks.m_vStacks.empty())
		{
			bool bContinue = true;
			DSS::DSSProgressDlg dlg;
			CStackingEngine StackingEngine;
			CString strReferenceFrame;

			// First check that the images are registered
			if (list.countUnregisteredCheckedLightFrames() != 0)
			{
				CRegisterEngine	RegisterEngine;
				bContinue = RegisterEngine.RegisterLightFrames(tasks, false, &dlg);
			}

			if (bContinue)
			{
				if (list.getReferenceFrame(strReferenceFrame))
					StackingEngine.SetReferenceFrame(strReferenceFrame);

				std::shared_ptr<CMemoryBitmap> pBitmap;
				bContinue = StackingEngine.StackLightFrames(tasks, &dlg, pBitmap);
				if (bContinue)
				{
					CString strFileName{ fileList.stem().c_str() };

					const auto iff = workspace.value("Stacking/IntermediateFileFormat").toUInt();

					if (StackingEngine.GetDefaultOutputFileName(strFileName, fileList.c_str(), iff == IFF_TIFF))
					{
						StackingEngine.WriteDescription(tasks, strFileName);

						const QString strText(QObject::tr("Saving Final image in %1", "IDS_SAVINGFINAL").arg(QString::fromWCharArray(strFileName.GetString())));
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
					}
					outputFile = QString::fromWCharArray(strFileName.GetString());
				}
			}
			dlg.Close();
			bResult = bContinue;
		}
		workspace.Pop();

		return bResult;
	};
}

namespace DSS
{
	static const QString DIALOG_GEOMETRY_SETTING	= QStringLiteral("Dialogs/%1/geometry");
	static const QString DEFAULT_LIST_FILE_FILTER	= QStringLiteral("*.txt");

	BaseDialog::BaseDialog(const QString& name, const Behaviours& behaviours /*= Behaviour::None*/, QWidget* parent /*= nullptr*/) :
		Inherited(parent),
		m_name{name},
		m_behaviours{behaviours}
	{
		Q_ASSERT(!m_behaviours.testFlag(Behaviour::PersistGeometry) || !m_name.isEmpty());
		connect(this, &QDialog::finished, this, &BaseDialog::saveState);
	}

	void BaseDialog::showEvent(QShowEvent* event)
	{
		if (!event->spontaneous()) {
			if (!m_initialised) {
				onInitDialog();
			}
		}
		Inherited::showEvent(event);
	}

	void BaseDialog::onInitDialog()
	{
		//
		// Restore Window position etc..
		//
		bool geometryRestored = false;
		if (hasPersistentGeometry()) {
			QByteArray ba = QSettings{}.value(DIALOG_GEOMETRY_SETTING.arg(m_name)).toByteArray();
			if (!ba.isEmpty()) {
				restoreGeometry(ba);
				geometryRestored = true;
			}			
		}
		if (!geometryRestored)
		{
			//
			// Center it in the main Window rectangle
			//
			const QRect r{ DeepSkyStacker::instance()->rect() };
			const QSize size = this->size();

			int top = ((r.top() + (r.height() / 2) - (size.height() / 2)));
			int left = ((r.left() + (r.width() / 2) - (size.width() / 2)));
			move(left, top);
		}

		m_initialised = true;
	}

	void BaseDialog::saveState() const
	{
		if (hasPersistentGeometry()) {
			QSettings{}.setValue(DIALOG_GEOMETRY_SETTING.arg(m_name), saveGeometry());
		}
	}


	BatchStacking::BatchStacking(QWidget* parent /*=nullptr*/) :
		Inherited(QStringLiteral("Batch"), Behaviour::PersistGeometry, parent),
		ui(new Ui::BatchStacking),
		m_fileListModel(new QStandardItemModel(this)),
		m_listFileFilters{ DEFAULT_LIST_FILE_FILTER }
	{
		ui->setupUi(this);

		ui->fileLists->setModel(m_fileListModel);
	}

	BatchStacking::~BatchStacking()
	{
		delete ui;
	}

	void BatchStacking::accept()
	{
		ZFUNCTRACE_RUNTIME();
		long processedListCount = 0;
		bool successfulProcessing = true;

		Q_ASSERT(m_fileListModel);
		const auto rows = m_fileListModel->rowCount();
		for (auto i = 0; i < rows && successfulProcessing; ++i) {
			auto item = m_fileListModel->item(i);
			Q_ASSERT(item);
			if (item->checkState() == Qt::Checked) {
				const auto& file = item->text();
				QString outputFile;
				successfulProcessing = processList(file.toStdString(), outputFile);
				item->setCheckState(Qt::Unchecked);
				++processedListCount;

				if (successfulProcessing) {
					auto index = m_fileListModel->index(i, 0);
					item->setText(QStringLiteral("->") + outputFile);
					item->setEnabled(false);
					item->setCheckState(Qt::Unchecked);
				}
			}
		}

		if (!processedListCount)
		{
			Inherited::accept();
		};
	}

	void BatchStacking::clearLists()
	{
		Q_ASSERT(m_fileListModel);
		m_fileListModel->clear();
	}

	void BatchStacking::addLists()
	{
		QSettings settings;
		static const QString settingKey = QStringLiteral("Folders/ListFolder");
		const QString& baseDir = settings.value(settingKey, QString()).toString();
		auto files = QFileDialog::getOpenFileNames(this, QString(), baseDir, m_listFileFilters.join(QStringLiteral(";;")));

		const auto& filePaths = getFilePaths();
		QStringList pathsToAdd;
		for (const auto& file : files) {
			//TODO: this is doesn't take into account fs case-sensitivity
			//On the other hand filesystem::path::equivalent is probably way too heavy-duty for this purpose 
			const auto& nativeFilePath = QDir::toNativeSeparators(file);
			if (!filePaths.contains(file, Qt::CaseInsensitive)) {
				pathsToAdd.append(nativeFilePath);
			}
		}
		addItemsFor(pathsToAdd, true);

		if (!files.isEmpty()) {
			settings.setValue(settingKey, QDir{ files.last() }.absolutePath());
		}
	}

	void BatchStacking::setMRUPaths(const std::vector<fs::path>& mruPaths)
	{
		QStringList filePaths;
		for (const auto& path : mruPaths) {
			filePaths.append(QString::fromStdWString(path.native()));
		}
		clearLists();
		addItemsFor(filePaths, false);
	}

	void BatchStacking::addItemsFor(const QStringList& paths, bool checked)
	{
		Q_ASSERT(m_fileListModel);
		for (const auto& path : paths) {
			QStandardItem* item = new QStandardItem(path);
			item->setCheckable(true);
			item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			m_fileListModel->appendRow(item);
		}
	}

	QStringList BatchStacking::getFilePaths() const
	{
		Q_ASSERT(m_fileListModel);
		QStringList filePaths;
		const auto rows = m_fileListModel->rowCount();
		for (auto i = 0; i < rows; ++i) {
			auto item = m_fileListModel->item(i);
			Q_ASSERT(item);
			filePaths.append(item->text());
		}
		return filePaths;
	}

}
