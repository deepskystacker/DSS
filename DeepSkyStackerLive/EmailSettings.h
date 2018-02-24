#ifndef __EMAILSETTINGS_H__
#define __EMAILSETTINGS_H__

#include "LiveSettings.h"

class CEmailSettings : public CDialog
{
// Construction
public:
	CEmailSettings(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EMAILSETTINGS };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private :
	CLiveSettings *			m_pLiveSettings;

	CEdit					m_Object;
	CEdit					m_SendTo;
	CEdit					m_SMTP;
	CEdit					m_Account;
	CButton					m_SendOnce;

public :
	void	SetLiveSettings(CLiveSettings * pLiveSettings)
	{
		m_pLiveSettings = pLiveSettings;
	};

// Implementation
protected:
	// Generated message map functions
	virtual BOOL	OnInitDialog();
	virtual void    OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTest();
};


#endif // __EMAILSETTINGS_H__