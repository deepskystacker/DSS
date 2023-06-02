#pragma once
#include "DSSProgress.h"

namespace Ui {
	class ProgressDlg;
}

namespace DSS
{
	class ProgressDlg : public QDialog, public ProgressBase				
	{
		Q_OBJECT

	private:
		Ui::ProgressDlg* ui;
		bool m_cancelInProgress;
		static inline const QString m_emptyString{};

	public:
		ProgressDlg(QWidget* parent = nullptr);
		~ProgressDlg();

		//
		// These eight mfs implement the public interface defined in DSS::ProgressBase
		// They invoke the corresponding slots using QMetaObject::invokeMethod
		// so that they can be invoked from ANY thread in the application will run on
		// the GUI thread.
		//
		virtual void Start1(const QString& szTitle, int lTotal1, bool bEnableCancel = true) override;
		virtual void Progress1(const QString& szText, int lAchieved1) override;
		virtual void Start2(const QString& szText, int lTotal2) override;
		virtual void Progress2(const QString& szText, int lAchieved2) override;
		virtual void End2() override;
		virtual void Close() override;
		virtual bool IsCanceled() const override { return m_cancelInProgress; }
		virtual bool Warning(const QString& szText) override;


		void setTimeRemaining(const QString& strText);
		void setProgress1Range(int nMin, int nMax);
		void setProgress2Range(int nMin, int nMax);
		void setItemVisibility(bool bSet1, bool bSet2);

		void EnableCancelButton(bool bState);


	protected:
		void applyStart1Text(const QString& strText);
		void applyStart2Text(const QString& strText);
		void applyProgress1(int lAchieved);
		void applyProgress2(int lAchieved);
		void applyTitleText(const QString& strText);
		void initialise();
		void endProgress2();
		void closeProgress();
		bool doWarning(const QString& szText);

		// ProgressBase
		virtual void applyProcessorsUsed(int nCount) override;

	protected slots:
		virtual void slotStart1(const QString& szTitle, int lTotal1, bool bEnableCancel = true);
		virtual void slotProgress1(const QString& szText, int lAchieved1);
		virtual void slotStart2(const QString& szText, int lTotal2);
		virtual void slotProgress2(const QString& szText, int lAchieved2);
		virtual void slotEnd2();
		virtual void slotClose();

	private slots:
		void cancelPressed();

	private:
		void closeEvent(QCloseEvent* bar);
		void retainHiddenWidgetSize(QWidget& rWidget);
	};
}