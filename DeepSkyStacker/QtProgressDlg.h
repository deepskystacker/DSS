#pragma once
#include <QDialog>

namespace Ui {
	class ProgressDlg;
}

namespace DSS
{
	class ProgressDlg : public QDialog
	{
		Q_OBJECT

	public:
		ProgressDlg(QWidget* parent = nullptr);
		~ProgressDlg();

		const QString getStart1Text() const;
		const QString getStart2Text() const;

		void setStart1Text(const QString& strText);
		void setStart2Text(const QString& strText);
		void setProgress1(int lAchieved);
		void setProgress2(int lAchieved);
		void setTimeRemaining(const QString& strText);
		void setProcessorsUsed(int lNrProcessors);
		void setProgress1Range(int nMin, int nMax);
		void setProgress2Range(int nMin, int nMax);
		void setItemVisibility(bool bSet1, bool bSet2);

		void EnableCancelButton(bool bState);
		void SetTitleText(const QString& strText);

		void RunDialog();
		bool IsCancelled() const { return m_bCancelInProgress; }

	private slots:
		void cancelPressed();

	private:
		void closeEvent(QCloseEvent* bar);
		void retainHiddenWidgetSize(QWidget& rWidget);

	private:
		Ui::ProgressDlg* ui;
		bool m_bCancelInProgress;
	};

	class DSSProgressDlg : public CDSSProgress
	{
	private:
		std::unique_ptr<ProgressDlg>		m_pDlg;
		QWidget* m_pParent;

		int					m_lTotal1;
		int					m_lTotal2;
		unsigned long long	m_dwStartTime;
		unsigned long long	m_dwLastTime;
		int					m_lLastTotal1;
		int					m_lLastTotal2;
		bool				m_bFirstProgress;
		bool				m_bEnableCancel;
		
		static const QString sm_EmptyString;
		static const float sm_fMinProgressStep;

	private:
		bool CreateProgressDialog();

	public:
		DSSProgressDlg();
		virtual ~DSSProgressDlg() override;

		virtual void SetNrUsedProcessors(int lNrProcessors = 1) override;
		virtual const QString GetStartText() const override;
		virtual const QString GetStart2Text() const override;

		virtual void Start(const QString& szTitle, int lTotal1, bool bEnableCancel = true) override;
		virtual void Progress1(const QString& szText, int lAchieved1) override;
		virtual void Start2(const QString& szText, int lTotal2) override;
		virtual void Progress2(const QString& szText, int lAchieved2) override;

		virtual void End2() override;
		virtual bool IsCanceled() override;
		virtual bool Close() override;
		virtual bool Warning(const QString& szText) override;
	};
}