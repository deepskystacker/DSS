#pragma once


// CDropFilesDlg dialog

class CDropFilesDlg : public CDialog
{
	DECLARE_DYNAMIC(CDropFilesDlg)

public:
	CDropFilesDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CDropFilesDlg();

	void	SetDropInfo(HDROP hDropInfo)
	{
		m_hDropInfo = hDropInfo;
	};

	PICTURETYPE	GetDropType()
	{
		return m_DropType;
	};

	void	GetDroppedFiles(std::vector<CString> & vFiles)
	{
		vFiles = m_vFiles;
	};

// Dialog Data
	enum { IDD = IDD_DROPFILES };

private :
	HDROP					m_hDropInfo;
	PICTURETYPE				m_DropType;
	CStatic					m_Text;
	CButton					m_LightFrames;
	CButton					m_DarkFrames;
	CButton					m_FlatFrames;
	CButton					m_BiasFrames;
	CButton					m_DarkFlatFrames;
	std::vector<CString>	m_vFiles;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	bool	IsMasterFile(LPCTSTR szFile);
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg	void OnLightFrames();
	afx_msg	void OnDarkFrames();
	afx_msg	void OnDarkFlatFrames();
	afx_msg	void OnFlatFrames();
	afx_msg	void OnBiasFrames();

	DECLARE_MESSAGE_MAP()
};
