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
	virtual const QString GetStartText() const = 0;
	virtual const QString GetStart2Text() const = 0;
	virtual void Start(const QString& szTitle, int lTotal1, bool bEnableCancel = true) = 0;
	virtual void Progress1(const QString& szText, int lAchieved1) = 0;
	virtual void Start2(const QString& szText, int lTotal2) = 0;
	virtual void Progress2(const QString& szText, int lAchieved2) = 0;
	virtual void End2() = 0;
	virtual bool IsCanceled() = 0;
	virtual bool Close() = 0;
	virtual bool Warning(const QString& szText) { return true; }
};

/* ------------------------------------------------------------------- */

#endif // _DSSPROGRESS_H__