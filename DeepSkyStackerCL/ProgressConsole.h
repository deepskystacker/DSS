#ifndef _PROGRESSCONSOLE_H__
#define _PROGRESSCONSOLE_H__

#include "DSSProgress.h"

class CProgressConsole : public CDSSProgress
{
private :
	LONG				m_lTotal1,
						m_lTotal2;
	DWORD				m_dwStartTime,
						m_dwLastTime;
	LONG				m_lLastTotal1,
						m_lLastTotal2;
	BOOL				m_bFirstProgress;
	CString				m_strTitle;
	CString				m_strStartText;
	CString				m_strStart2Text;
	BOOL				m_bTitleDone;
	BOOL				m_bStartTextDone;
	BOOL				m_bStart2TextDone;
	CString				m_strLastOut;

private :
	void	AnsiToOEM(CString & str)
	{
		CHAR			szOut[2000];

		::CharToOem((LPCTSTR)str, szOut);
		str = szOut;
	};

	void	PrintText(LPCTSTR szText)
	{
		CString			strOut = szText;

		// AnsiToOEM(strOut);

		if (strOut.GetLength() && m_strLastOut.CompareNoCase(strOut))
		{
			_tprintf(_T("%s\n"), (LPCTSTR)strOut);
			m_strLastOut = strOut;
		};
	};

public :
	CProgressConsole() 
	{
		m_bTitleDone	  = FALSE;
		m_bStartTextDone  = FALSE;
		m_bStart2TextDone = FALSE;
	};
	virtual ~CProgressConsole() 
	{
		Close();
	};

	virtual void	GetStartText(CString & strText)
	{
		strText = m_strStartText;
	};

	virtual void	GetStart2Text(CString & strText)
	{
		strText = m_strStart2Text;
	};

	virtual	void	Start(LPCTSTR szTitle, LONG lTotal1, BOOL bEnableCancel = TRUE)
	{
		CString			strTitle = szTitle;

		m_lLastTotal1 = 0;
		m_lTotal1 = lTotal1;
		m_dwStartTime = GetTickCount();
		m_dwLastTime  = m_dwStartTime;
		m_bFirstProgress = TRUE;
		if (strTitle.GetLength())
		{
			if (m_strTitle.CompareNoCase(szTitle))
				m_bTitleDone = FALSE;
			m_strTitle = strTitle;
			if (!m_bTitleDone)
			{
				PrintText(m_strTitle);
				m_bTitleDone = TRUE;
			};
		};
	};
	virtual void	Progress1(LPCTSTR szText, LONG lAchieved1)
	{
		CString			strText = szText;
		DWORD			dwCurrentTime = GetTickCount();

		if (strText.GetLength())
		{
			if (m_strStartText.CompareNoCase(szText))
				m_bStartTextDone = FALSE;
			m_strStartText = szText;
		};

		if (m_bFirstProgress || ((double)(lAchieved1-m_lLastTotal1) > (m_lTotal1 / 100.0)) || ((dwCurrentTime - m_dwLastTime) > 1000))
		{
			m_bFirstProgress = FALSE;
			m_lLastTotal1 = lAchieved1;
			m_dwLastTime  = dwCurrentTime;
			
			double			fAchieved  =0.0;
			if (m_lTotal1)
				fAchieved = (double)lAchieved1/(double)m_lTotal1 * 100.0;

			// Update progress
			if (!m_bStartTextDone && m_strStartText.GetLength())
			{
				PrintText(m_strStartText);
				m_bStartTextDone = TRUE;
			};

			printf("%.1f %%   \r", fAchieved);
		};
	};
	
	virtual void	Start2(LPCTSTR szText, LONG lTotal2)
	{
		CString			strText = szText;

		m_lLastTotal2 = 0;
		if (strText.GetLength())
		{
			if (m_strStart2Text.CompareNoCase(szText))
				m_bStart2TextDone = FALSE;

			m_strStart2Text = strText;
			if (!m_bStart2TextDone && m_strStart2Text.GetLength())
			{
				PrintText(m_strStart2Text);
				m_bStart2TextDone = TRUE;
			};
		};

		m_lTotal2 = lTotal2;

//		if (m_bJointProgress)
//			Start(szText, lTotal2, FALSE);
	};

	virtual void	Progress2(LPCTSTR szText, LONG lAchieved2)
	{
		if ((double)(lAchieved2 - m_lLastTotal2) > (m_lTotal2 / 100.0))
		{
			CString			strText = szText;

			m_lLastTotal2 = lAchieved2;

			if (strText.GetLength())
			{
				if (m_strStart2Text.CompareNoCase(szText))
					m_bStart2TextDone = FALSE;
				m_strStart2Text = strText;
			};

			double			fAchieved  =0.0;
			if (m_lTotal2)
				fAchieved = (double)lAchieved2/(double)m_lTotal2 * 100.0;

			if (!m_bStart2TextDone && m_strStart2Text.GetLength())
			{
				PrintText(m_strStart2Text);
				m_bStart2TextDone = TRUE;
			};
			printf("%.1f %%   \r", fAchieved);
		};

//		if (m_bJointProgress)
//			Progress1(szText, lAchieved2);
	};

	virtual void	End2()
	{
		// printf("\n");
	};

	virtual BOOL	IsCanceled()
	{
		return FALSE;
	};
	virtual BOOL	Close()
	{
		return TRUE;
	};
};

#endif // _PROGRESSCONSOLE_H__