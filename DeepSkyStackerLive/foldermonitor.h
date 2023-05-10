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

	public slots:
		void stop();

	private:
		std::atomic_bool stopped;
	};
}
