#pragma once
#include <QDialog>

namespace Ui {
	class ProgressDlg;
}

namespace DSS
{
	class ProgressDlg :	public QDialog,
						public ProgressBase
	{
		Q_OBJECT

	private:
		Ui::ProgressDlg* m_ui;
		bool m_cancelInProgress;
		static const QString m_emptyString;

	public:
		ProgressDlg();
		~ProgressDlg();

		void setTimeRemaining(const QString& strText);
		void setProgress1Range(int nMin, int nMax);
		void setProgress2Range(int nMin, int nMax);
		void setItemVisibility(bool bSet1, bool bSet2);

		void EnableCancelButton(bool bState);

		bool IsCancelled() const { return m_cancelInProgress; }

		// ProgressBase
		virtual void applyStart1Text(const QString& strText) override;
		virtual void applyStart2Text(const QString& strText) override;
		virtual void applyProgress1(int lAchieved) override;
		virtual void applyProgress2(int lAchieved) override;
		virtual void applyTitleText(const QString& strText) override;
		virtual void initialise() override;
		virtual void endProgress2() override;
		virtual bool hasBeenCanceled() override;
		virtual void closeProgress() override;
		virtual bool doWarning(const QString& szText) override;
		virtual void applyProcessorsUsed(int nCount) override;

	private slots:
		void cancelPressed();

	private:
		void closeEvent(QCloseEvent* bar);
		void retainHiddenWidgetSize(QWidget& rWidget);
	};
}