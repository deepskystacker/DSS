#ifndef __RAWFILESTAB_H__
#define __RAWFILESTAB_H__

#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// CRawFilesTab dialog

class CRawFilesTab : public CPropertyPage //CChildPropertyPage
{
	DECLARE_DYNCREATE(CRawFilesTab)
private :
	BOOL				m_bFirstActivation;

// Construction
public:
	CRawFilesTab();
	~CRawFilesTab();

// Dialog Data
	//{{AFX_DATA(CRawFilesTab)
	enum { IDD = IDD_RAWSETTINGS_TAB };
	CEdit	m_RedScale;
	CButton	m_CameraWB;
	CEdit	m_Brightness;
	CEdit	m_BlueScale;
	CButton	m_Bilinear;
	CButton	m_NoWB;
	CButton	m_AHD;
	CButton m_SuperPixels;
	CButton m_RawBayer;
	CButton	m_BlackPoint;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRawFilesTab)
	public:
	virtual BOOL OnSetActive();
	void	SaveValues();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private :
	void UpdateControls();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRawFilesTab)
	afx_msg void OnChangeBluescale();
	afx_msg void OnChangeBrightness();
	afx_msg void OnChangeRedscale();
	afx_msg void OnNoWB();
	afx_msg void OnCameraWB();
	afx_msg void OnBilinear();
	afx_msg void OnAhd();
	afx_msg void OnBnClickedSuperpixels();
	afx_msg void OnBnClickedRawbayer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __RAWFILESTAB_H__
