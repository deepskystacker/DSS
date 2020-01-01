#ifndef __BACKGROUNDOPTIONS_H__
#define __BACKGROUNDOPTIONS_H__

class CBackgroundOptions : public CDialog
{
private :
	CButton				m_Linear;
	CButton				m_Rational;
	CStatic				m_CalibrationPreview;

	CButton				m_None;
	CButton				m_Minimum;
	CButton				m_Middle;
	CButton				m_Maximum;
	CStatic				m_RGBCalibrationPreview;

	BACKGROUNDCALIBRATIONMODE			m_CalibrationMode;
	BACKGROUNDCALIBRATIONINTERPOLATION	m_CalibrationInterpolation;
	RGBBACKGROUNDCALIBRATIONMETHOD		m_RGBCalibrationMethod;

// Construction
public:
	CBackgroundOptions(CWnd* pParent = nullptr);   // standard constructor

	void	SetBackgroundCalibrationMode(BACKGROUNDCALIBRATIONMODE Mode)
	{
		m_CalibrationMode = Mode;
	};

	BACKGROUNDCALIBRATIONMODE GetBackgroundCalibrationMode()
	{
		return m_CalibrationMode;
	};

// Dialog Data
	//{{AFX_DATA(CBackgroundOptions)
	enum { IDD = IDD_BACKGROUNDOPTIONS };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackgroundOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
private :
	void		UpdateCalibrationPreview();
	void		UpdateRGBCalibrationPreview();

protected:

	// Generated message map functions
	//{{AFX_MSG(CBackgroundOptions)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLinear();
	afx_msg void OnBnClickedRational();
	afx_msg void OnBnClickedNone();
	afx_msg void OnBnClickedMinimum();
	afx_msg void OnBnClickedMiddle();
	afx_msg void OnBnClickedMaximum();
};


#endif