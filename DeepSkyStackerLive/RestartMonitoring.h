#ifndef __RESTARTMONITORING_H__
#define __RESTARTMONITORING_H__

class CRestartMonitoring : public CDialog
{
// Construction
public:
	CRestartMonitoring(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RESTARTMONITORING };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private :
	CButton					m_CreateNewEmpty;
	CButton					m_StartFromCurrent;
	CButton					m_DropPending;
	CButton					m_UsePending;

	BOOL					m_bClearStacked;
	BOOL					m_bDropPending;

public :
	BOOL	IsClearStackedImage()
	{
		return m_bClearStacked;
	};

	BOOL	IsDropPending()
	{
		return m_bDropPending;
	};

// Implementation
protected:
	// Generated message map functions
	virtual BOOL	OnInitDialog();
	virtual void    OnOK();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedTest();
	afx_msg void OnBnClickedCreatenewstackedimage();
	afx_msg void OnBnClickedStartfromcurrentstackedimage();
	afx_msg void OnBnClickedDroppendingimages();
	afx_msg void OnBnClickedUsependingimages();
};


#endif // __RESTARTMONITORING_H__