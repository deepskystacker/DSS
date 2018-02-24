#if !defined(AFX_SHELLVIEW_H__10EC4EE4_E283_11D2_9B23_004005649FB5__INCLUDED_)
#define AFX_SHELLVIEW_H__10EC4EE4_E283_11D2_9B23_004005649FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShellView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShellView view

class CShellView : public CEditView
{
protected:
	CShellView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CShellView)

private:
	HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, 
		hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, 
		hSaveStdin, hSaveStdout;  
	CWinThread*	m_pReadThread;
	DWORD dwProcessId;

	LOGFONT m_lf;
	CFont m_defFont;
// Attributes
public:

// Operations
public:
	void GetUserInput( CString& input );
	void AddTexts( LPCTSTR string );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CShellView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	BOOL CreateShellRedirect();
	//{{AFX_MSG(CShellView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void AddTexts( TCHAR ch );
	void WriteToPipe(LPCTSTR line);
	BOOL CreateChildProcess(DWORD& dwProcessId);

	static UINT ReadPipeThreadProc( LPVOID pParam );
private:
	int GetSelLength();
	int GetCurrentPosition();
	void MoveToEnd();
	int m_nLength;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLVIEW_H__10EC4EE4_E283_11D2_9B23_004005649FB5__INCLUDED_)
