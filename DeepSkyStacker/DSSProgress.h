#ifndef _DSSPROGRESS_H__
#define _DSSPROGRESS_H__

/* ------------------------------------------------------------------- */

class CDSSProgress
{
protected :
	BOOL			m_bJointProgress;

public :
	CDSSProgress() 
	{
		m_bJointProgress = FALSE;
	};
	virtual ~CDSSProgress() {};

	virtual void	SetNrUsedProcessors(LONG lNrProcessors=1) {};
	void	SetJointProgress(BOOL bJointProgress)
	{
		m_bJointProgress = bJointProgress;
	};
	virtual void	GetStartText(CString & strText) = 0;
	virtual void	GetStart2Text(CString & strText) = 0;
	virtual	void	Start(LPCTSTR szTitle, LONG lTotal1, BOOL bEnableCancel = TRUE) = 0;
	virtual void	Progress1(LPCTSTR szText, LONG lAchieved1) = 0;
	virtual void	Start2(LPCTSTR szText, LONG lTotal2) = 0;
	virtual void	Progress2(LPCTSTR szText, LONG lAchieved2) = 0;
	virtual void	End2() = 0;
	virtual BOOL	IsCanceled() = 0;
	virtual BOOL	Close() = 0;
	virtual BOOL	Warning(LPCTSTR szText) { return TRUE; }
};

/* ------------------------------------------------------------------- */

#endif // _DSSPROGRESS_H__