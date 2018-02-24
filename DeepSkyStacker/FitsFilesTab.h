#ifndef __FITSFILESTAB_H__
#define __FITSFILESTAB_H__

#include "afxwin.h"
#include "BitmapExt.h"

class CDSLR
{
public :
	CString				m_strName;
	CFATYPE				m_CFAType;

private :
	void	CopyFrom(const CDSLR & cd)
	{
		m_strName = cd.m_strName;
		m_CFAType = cd.m_CFAType;
	};

public :
	CDSLR(LPCTSTR szName, CFATYPE CFAType)
	{
		m_strName = szName;
		m_CFAType = CFAType;
	};

	CDSLR(const CDSLR & cd)
	{
		CopyFrom(cd);
	};

	virtual ~CDSLR()
	{
	};

	const CDSLR & operator = (const CDSLR & cd)
	{
		CopyFrom(cd);
		return *this;
	};
};

/////////////////////////////////////////////////////////////////////////////
// CFitsFilesTab dialog

class CFitsFilesTab : public CPropertyPage //CChildPropertyPage
{
	DECLARE_DYNCREATE(CFitsFilesTab)
private :
	BOOL				m_bFirstActivation;
	std::vector<CDSLR>	m_vDSLRs;

// Construction
public:
	CFitsFilesTab();
	~CFitsFilesTab();

// Dialog Data
	//{{AFX_DATA(CFitsFilesTab)
	enum { IDD = IDD_FITSSETTINGS_TAB };
	CEdit		m_RedScale;
	CEdit		m_Brightness;
	CEdit		m_BlueScale;
	CButton		m_Bilinear;
	CButton		m_AHD;
	CButton		m_SuperPixels;
	CButton		m_RawBayer;
	CComboBox	m_DSLR;
	CButton		m_FITSisRAW;
	CStatic		m_BayerPattern;
	CButton		m_ForceUnsigned;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFitsFilesTab)
	public:
	virtual BOOL OnSetActive();
	void	SaveValues();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private :
	void	UpdateControls();
	void	UpdateBayerPattern();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFitsFilesTab)
	afx_msg void OnChangeBluescale();
	afx_msg void OnChangeBrightness();
	afx_msg void OnChangeRedscale();
	afx_msg void OnBilinear();
	afx_msg void OnAHD();
	afx_msg void OnBnClickedSuperpixels();
	afx_msg void OnBnClickedRawbayer();
	afx_msg void OnDSLRChange( );
	afx_msg void OnFITSIsRAW( );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __FITSFILESTAB_H__
