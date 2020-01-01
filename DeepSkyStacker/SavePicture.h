#pragma once

// CSavePicture
#include <ToolTipButton.h>

class CSavePicture : public CFileDialog
{
private :
	BOOL			m_bApplied;
	BOOL			m_bEnableUseRect;
	BOOL			m_bUseRect;
	TIFFCOMPRESSION m_Compression;

	DECLARE_DYNAMIC(CSavePicture)

private :
	CToolTipButton			m_Applied;
	CToolTipButton			m_Embedded;
	CToolTipButton			m_UseRect;
	CButton					m_CompressionNone;
	CButton					m_CompressionZIP;
	CButton					m_CompressionLZW;
	CString					m_strSaveEmbed;

public:
	CSavePicture(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = nullptr,
		LPCTSTR lpszFileName = nullptr,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = nullptr,
		CWnd* pParentWnd = nullptr);
	virtual ~CSavePicture();

	void	SetApplied(BOOL bApplied)
	{
		m_bApplied = bApplied;
	};

	void	SetUseRect(BOOL bEnabled, BOOL bUseRect)
	{
		m_bEnableUseRect = bEnabled;
		m_bUseRect = bUseRect;
	};

	BOOL	GetApplied()
	{
		return m_bApplied;
	};

	BOOL	GetUseRect()
	{
		return m_bUseRect;
	};

	void	SetCompression(TIFFCOMPRESSION Compression)
	{
		m_Compression = Compression;
	};

	TIFFCOMPRESSION GetCompression()
	{
		return m_Compression;
	};

protected:
	DECLARE_MESSAGE_MAP()

	void	UpdateControls();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnApplied();
	afx_msg void OnEmbedded();
	afx_msg void OnUseRect();
	afx_msg void OnCompressionNone();
	afx_msg void OnCompressionZIP();
	afx_msg void OnCompressionLZW();

	virtual void OnTypeChange();

public:
	virtual BOOL OnInitDialog();
};


