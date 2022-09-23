#ifndef __BATCHSTACKING_H__
#define __BATCHSTACKING_H__

#include "EasySize.h"

class CBatchStacking : public CDialog
{
	DECLARE_DYNAMIC(CBatchStacking)
	DECLARE_EASYSIZE

private :
	CCheckListBox			m_Lists;
	CMRUList				m_MRUList;
	CScrollBar				m_Gripper;

// Construction
public:
	CBatchStacking(CWnd* pParent = nullptr);   // standard constructor
	void setMRUList(const CMRUList& MRUList)
	{
		m_MRUList = MRUList;
	};

// Dialog Data
	//{{AFX_DATA(CBatchStacking)
	enum { IDD = IDD_BATCHSTACKING };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchStacking)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL
	afx_msg void OnBnClickedAddLists();
	afx_msg void OnBnClickedClearList();

// Implementation
private :
	bool ProcessList(LPCTSTR szList, CString& strOutputFile);
	void	UpdateListBoxWidth();

protected:

	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnSizing(UINT nSide, LPRECT lpRect);

	// Generated message map functions
	//{{AFX_MSG(CBatchStacking)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif