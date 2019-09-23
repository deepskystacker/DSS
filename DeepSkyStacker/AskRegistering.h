#ifndef __ASKREGISTERING_H__
#define __ASKREGISTERING_H__

typedef enum tagASKREGISTERINGANSWER
{
	ARA_ONE			= 1,
	ARA_ALL			= 2,
	ARA_CONTINUE	= 3
}ASKREGISTERINGANSWER;


class CAskRegistering : public CDialog
{
// Construction
public:
	CAskRegistering(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAskRegistering)
	enum { IDD = IDD_ASKREGISTERING };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAskRegistering)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected :
	ASKREGISTERINGANSWER			m_Result;
	CButton							m_RegisterOne;
	CButton							m_RegisterAll;
	CButton							m_RegisterNone;

	void	UpdateControls();

public :
	ASKREGISTERINGANSWER	GetAction()
	{
		return m_Result;
	};

protected:

	// Generated message map functions
	//{{AFX_MSG(CAskRegistering)
	virtual BOOL	OnInitDialog();
	virtual void	OnOK();
	afx_msg void	OnRegisterOne();
	afx_msg void	OnRegisterAll();
	afx_msg void	OnRegisterNone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



#endif // __ASKREGISTERING_H__