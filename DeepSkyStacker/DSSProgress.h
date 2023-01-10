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

	// Helper functions - when you just want to update the progress and not the text.
	void Start(int lTotal1, bool bEnableCancel = true) { Start(m_strEmptyString, lTotal1, bEnableCancel); }
	void Start2(int lTotal2) { Start2(m_strEmptyString, lTotal2); }
	void Progress1(int lAchieved1) { Progress1(m_strEmptyString, lAchieved1); }
	void Progress2(int lAchieved2) { Progress2(m_strEmptyString, lAchieved2); }

protected:
	static const QString m_strEmptyString;
};

/* ------------------------------------------------------------------- */

#endif // _DSSPROGRESS_H__