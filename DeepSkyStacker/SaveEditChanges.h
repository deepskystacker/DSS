#ifndef __SAVEEDITCHANGES_H__
#define __SAVEEDITCHANGES_H__

class CSaveEditChanges : public CDialog
{
public:
	CSaveEditChanges(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveEditChanges)
	enum { IDD = IDD_SAVEEDITCHANGES };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveEditChanges)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



// Implementation
protected :
	CButton							m_AskAlways;
	CButton							m_SaveDontAsk;
	CButton							m_DontSaveDontAsk;

	void	UpdateControls();

private :
	void	SaveSettings();

protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveEditChanges)
	virtual BOOL	OnInitDialog();
	virtual	void	OnCancel();

	afx_msg	void	OnYes();
	afx_msg	void	OnNo();

	afx_msg void	OnAskAlways();
	afx_msg void	OnSaveDontAsk();
	afx_msg void	OnDontSaveDontAsk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

enum class EditSaveMode
{
	SECM_ASKALWAYS = 0,
	SECM_SAVEDONTASK = 1,
	SECM_DONTSAVEDONTASK = 2
};


int AskSaveEditChangesMode();
void	SetSaveEditMode(EditSaveMode Mode);
EditSaveMode GetSaveEditMode();

#endif // __SAVEEDITCHANGES_H__