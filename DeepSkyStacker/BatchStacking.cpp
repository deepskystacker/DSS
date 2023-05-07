// BatchStacking.cpp : implementation file
//
#include "stdafx.h"
#include "BatchStacking.h"
#include "ui/ui_BatchStacking.h"
#include "Ztrace.h"
#include "Workspace.h"
#include "StackingTasks.h"
#include "FrameList.h"
#include "QtProgressDlg.h"
#include "StackingEngine.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"

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
			DSS::ProgressDlg dlg;
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

						const QString strText(QCoreApplication::translate("BatchStacking", "Saving Final image in %1", "IDS_SAVINGFINAL").arg(QString::fromWCharArray(strFileName.GetString())));
						dlg.Start2(strText, 0);

						if (iff == IFF_TIFF)
						{
							if (pBitmap->IsMonochrome())
								WriteTIFF(strFileName.GetString(), pBitmap.get(), &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE);
							else
								WriteTIFF(strFileName.GetString(), pBitmap.get(), &dlg, TF_32BITRGBFLOAT, TC_DEFLATE);
						}
						else
						{
							if (pBitmap->IsMonochrome())
								WriteFITS(strFileName.GetString(), pBitmap.get(), &dlg, FF_32BITGRAYFLOAT);
							else
								WriteFITS(strFileName.GetString(), pBitmap.get(), &dlg, FF_32BITRGBFLOAT);
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
	extern QStringList OUTPUTLIST_FILTERS;

	BatchStacking::BatchStacking(QWidget* parent /*=nullptr*/) :
		Inherited(Behaviour::PersistGeometry, parent),
		ui(new Ui::BatchStacking),
		m_fileListModel(new QStandardItemModel(this))
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
		auto files = QFileDialog::getOpenFileNames(this, QString(), baseDir, OUTPUTLIST_FILTERS.join(QStringLiteral(";;")));

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
			filePaths.append(QString::fromWCharArray(path.native().c_str()));
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
