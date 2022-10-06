#pragma once
#include <QDialog>

namespace Ui {
	class ProgressDlg;
}
class CQtProgressDlg : public QDialog
{
	Q_OBJECT;

public:
	CQtProgressDlg(QWidget* parent = nullptr);
	~CQtProgressDlg();

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

private:
	Ui::ProgressDlg* ui;
	bool m_bCancelInProgress;
};

class CQtDSSProgressDlg : public CDSSProgress
{
private:
	std::unique_ptr<CQtProgressDlg>		m_pDlg;
	QWidget* m_pParent;

	int					m_lTotal1;
	int					m_lTotal2;
	unsigned long long	m_dwStartTime;
	unsigned long long	m_dwLastTime;
	int					m_lLastTotal1;
	int					m_lLastTotal2;
	bool				m_bFirstProgress;
	bool				m_bEnableCancel;

private:
	bool CreateProgressDialog();

public:
	CQtDSSProgressDlg(QWidget* pParent = nullptr);
	virtual ~CQtDSSProgressDlg() override;

	virtual void SetNrUsedProcessors(int lNrProcessors = 1) override;
	virtual void GetStartText(CString& strText) override;
	virtual void GetStart2Text(CString& strText) override;
	virtual	void Start(LPCTSTR szTitle, int lTotal1, bool bEnableCancel = true) override;
	virtual void Progress1(LPCTSTR szText, int lAchieved1) override;
	virtual void Start2(LPCTSTR szText, int lTotal2) override;
	virtual void Progress2(LPCTSTR szText, int lAchieved2) override;
	virtual void End2() override;
	virtual bool IsCanceled() override;
	virtual bool Close() override;
	virtual bool Warning(LPCTSTR szText) override;
};