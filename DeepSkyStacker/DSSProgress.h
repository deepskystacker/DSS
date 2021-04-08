#ifndef _DSSPROGRESS_H__
#define _DSSPROGRESS_H__

/* ------------------------------------------------------------------- */

class CDSSProgress
{
protected:
	bool m_bJointProgress;

public:
	CDSSProgress() : m_bJointProgress{ false }
	{}

	virtual ~CDSSProgress() = default;

	virtual void SetNrUsedProcessors(int lNrProcessors = 1) {}
	void SetJointProgress(bool bJointProgress)
	{
		m_bJointProgress = bJointProgress;
	};
	virtual void GetStartText(CString& strText) = 0;
	virtual void GetStart2Text(CString& strText) = 0;
	virtual void Start(LPCTSTR szTitle, int lTotal1, bool bEnableCancel = true) = 0;
	virtual void Progress1(LPCTSTR szText, int lAchieved1) = 0;
	virtual void Start2(LPCTSTR szText, int lTotal2) = 0;
	virtual void Progress2(LPCTSTR szText, int lAchieved2) = 0;
	virtual void End2() = 0;
	virtual bool IsCanceled() = 0;
	virtual bool Close() = 0;
	virtual bool Warning(LPCTSTR szText) { return true; }
};

/* ------------------------------------------------------------------- */

#endif // _DSSPROGRESS_H__