// BatchStacking.cpp : implementation file
//
#include "stdafx.h"
#include "BatchStacking.h"
#include "ui/ui_BatchStacking.h"
#include "Ztrace.h"
#include "Workspace.h"
#include "StackingTasks.h"
#include "FrameList.h"
#include "progressdlg.h"
#include "StackingEngine.h"
#include "TIFFUtil.h"
#include "FITSUtil.h"



namespace DSS

{
	class BatchMode
	{
	public: 
		BatchMode(FrameList* fl) : frameList{fl}
		{
			frameList->setBatchStacking(true);
		};
		~BatchMode()
		{
			frameList->setBatchStacking(false);
		}
		BatchMode(const BatchMode&) = delete;
		BatchMode(BatchMode&&) = delete;
		BatchMode& operator=(const BatchMode&) = delete;
		BatchMode& operator=(BatchMode&&) = delete;
	private:
		FrameList* frameList;
	};

	extern QStringList OUTPUTLIST_FILTERS;

	BatchStacking::BatchStacking(QWidget* parent) :
		stackingDlg { dynamic_cast<StackingDlg*>(parent) },
		Inherited(Behaviour::PersistGeometry, parent),
		ui(new Ui::BatchStacking),
		m_fileListModel(new QStandardItemModel(this))
	{
		ZASSERT(nullptr != stackingDlg);
		ui->setupUi(this);
		connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
		connect(ui->addLists, &QAbstractButton::clicked, this, &BatchStacking::addLists);
		connect(ui->clearLists, &QAbstractButton::clicked, this, &BatchStacking::clearLists);

		ui->fileLists->setModel(m_fileListModel);
	}

	BatchStacking::~BatchStacking()
	{
		delete ui;
	}

	bool BatchStacking::processList(const fs::path& fileList, QString& outputFile)
	{
		ZFUNCTRACE_RUNTIME();
		bool bResult = true;
		Workspace workspace;
		CAllStackingTasks tasks;
		stackingDlg->clearList();		// Clear groups etc. for next filelist.

		workspace.Push();
		stackingDlg->frameList.loadFilesFromList(fileList);
		stackingDlg->frameList.fillTasks(tasks);
		tasks.ResolveTasks();

		if (!tasks.m_vStacks.empty())
		{
			bool bContinue = true;
			DSS::ProgressDlg dlg{ this };
			CStackingEngine StackingEngine;

			// First check that the images are registered
			if (stackingDlg->frameList.countUnregisteredCheckedLightFrames() != 0)
			{
				CRegisterEngine	RegisterEngine;
				bContinue = RegisterEngine.RegisterLightFrames(tasks, false, &dlg);
			}

			if (bContinue)
			{
				const QString referenceFrame(stackingDlg->frameList.getReferenceFrame());
				if (!referenceFrame.isEmpty())
					StackingEngine.SetReferenceFrame(referenceFrame.toStdU16String());

				std::shared_ptr<CMemoryBitmap> pBitmap;
				bContinue = StackingEngine.StackLightFrames(tasks, &dlg, pBitmap);
				if (bContinue)
				{
					fs::path file;

					const auto iff = workspace.value("Stacking/IntermediateFileFormat").toUInt();

					if (StackingEngine.GetDefaultOutputFileName(file, fileList, iff == IFF_TIFF))
					{
						StackingEngine.WriteDescription(tasks, file);

						const QString strText(QCoreApplication::translate("BatchStacking", "Saving Final image in %1", "IDS_SAVINGFINAL").arg(QString::fromStdU16String(file.generic_u16string())));
						dlg.Start2(strText, 0);

						if (iff == IFF_TIFF)
						{
							if (pBitmap->IsMonochrome())
								WriteTIFF(file, pBitmap.get(), &dlg, TF_32BITGRAYFLOAT, TC_DEFLATE);
							else
								WriteTIFF(file, pBitmap.get(), &dlg, TF_32BITRGBFLOAT, TC_DEFLATE);
						}
						else
						{
							if (pBitmap->IsMonochrome())
								WriteFITS(file, pBitmap.get(), &dlg, FF_32BITGRAYFLOAT);
							else
								WriteFITS(file, pBitmap.get(), &dlg, FF_32BITRGBFLOAT);
						}
						dlg.End2();
					}
					outputFile = QString::fromStdU16String(file.generic_u16string());
				}
			}
			dlg.Close();
			bResult = bContinue;
		}
		workspace.Pop();

		return bResult;
	}

	void BatchStacking::accept()
	{
		ZFUNCTRACE_RUNTIME();
		//
		// Tell the frameList it is doing batch processing.
		//
		BatchMode batchMode(&stackingDlg->frameList);

		long processedListCount = 0;
		bool successfulProcessing = true;

		ZASSERT(nullptr != m_fileListModel);
		const auto rows = m_fileListModel->rowCount();
		for (auto i = 0; i < rows && successfulProcessing; ++i) {
			auto item = m_fileListModel->item(i);
			ZASSERT(nullptr != item);
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
		ZASSERT(nullptr != m_fileListModel);
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
		ZASSERT(nullptr != m_fileListModel);
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
		ZASSERT(nullptr != m_fileListModel);
		QStringList filePaths;
		const auto rows = m_fileListModel->rowCount();
		for (auto i = 0; i < rows; ++i) {
			auto item = m_fileListModel->item(i);
			ZASSERT(nullptr != item);
			filePaths.append(item->text());
		}
		return filePaths;
	}

}
