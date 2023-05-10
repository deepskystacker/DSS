#pragma once

#include <QObject>
namespace DSS
{
	class FolderMonitor : public QObject, public QRunnable
	{
		Q_OBJECT

	void run() override;

	public:
		FolderMonitor(QObject* parent);
		~FolderMonitor();

		//
		// Don't intend this to be copied or assigned.
		//
		FolderMonitor(const FolderMonitor&) = delete;
		FolderMonitor& operator=(const FolderMonitor&) = delete;
		FolderMonitor(FolderMonitor&& rhs) = delete;
		FolderMonitor& operator=(FolderMonitor&& rhs) = delete;


	public slots:
		void stop();

	private:
		bool stopped;
		unsigned long delay;
		std::u16string folderToWatch;
		std::unordered_map<fs::path, fs::file_time_type> paths_;
		std::vector<fs::path> existing_;

	signals:
		void existingFiles(const std::vector<fs::path>&);
		void fileChanged(const fs::path& file);
		void fileCreated(const fs::path& file);
		void fileErased(const fs::path& file);

	};
}
