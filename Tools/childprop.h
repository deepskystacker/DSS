#pragma once

/////////////////////////////////////////////////////////////////////////////
// CChildPropertyPage
//

class CChildPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CChildPropertyPage)

public:
	CChildPropertyPage() : CPropertyPage()
    {
        m_nSelectedPage = 0;
        m_nIDGroup = 0;
    }
	CChildPropertyPage(UINT nIDPage, UINT nIDCaption = 0, UINT nSelectedPage = 0, UINT nIDGroup = 0);
	~CChildPropertyPage();

	void AddPage(CPropertyPage* pPage) { m_arrPages.Add(pPage); }
	CPropertyPage* GetPage(const int i) const { return (i<GetPageCount()) ? m_arrPages[i] : 0; }
	CPropertyPage* GetActivePage() const { return (GetPageCount()>0) ? m_Sheet.GetActivePage() : 0; }
	BOOL GetPageCount() const { return (m_arrPages.GetSize() > 0); }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg LRESULT OnQuerySiblings(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	CPropertySheet m_Sheet;
	UINT m_nSelectedPage;
	UINT m_nIDGroup;
	CArray<CPropertyPage*, CPropertyPage*> m_arrPages;
};
