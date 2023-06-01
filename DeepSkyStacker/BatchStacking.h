#pragma once

#include "BaseDialog.h"

class QStandardItemModel;

namespace Ui {
	class BatchStacking;
}

namespace DSS
{
	class BatchStacking : public BaseDialog
	{
		Q_OBJECT

			typedef BaseDialog
			Inherited;

	public:
		BatchStacking(QWidget* parent = nullptr);
		virtual ~BatchStacking();

		void setMRUPaths(const std::vector<fs::path>& mruPaths);

	public slots:
		void accept() override;
		void clearLists();
		void addLists();

	private:
		void addItemsFor(const QStringList& paths, bool checked);
		QStringList getFilePaths() const;

	private:
		Ui::BatchStacking*	ui{ nullptr };
		QStandardItemModel*	m_fileListModel{ nullptr };

	private:
		bool processList(const fs::path& fileList, QString& outputFile);
		Q_DISABLE_COPY(BatchStacking)
	};
}

