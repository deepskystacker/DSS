#pragma once
#include "DSSCommon.h"
#include "ToolTipButton.h"

// CSavePicture
class CSavePicture : public CFileDialog
{
private :
	bool			m_bApplied;
	bool			m_bEnableUseRect;
	bool			m_bUseRect;
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
	CSavePicture(bool bOpenFileDialog, // true for FileOpen, false for FileSaveAs
		LPCTSTR lpszDefExt = nullptr,
		LPCTSTR lpszFileName = nullptr,
		std::uint32_t dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = nullptr,
		CWnd* pParentWnd = nullptr);
	virtual ~CSavePicture();

	void	SetApplied(bool bApplied)
	{
		m_bApplied = bApplied;
	};

	void	SetUseRect(bool bEnabled, bool bUseRect)
	{
		m_bEnableUseRect = bEnabled;
		m_bUseRect = bUseRect;
	};

	bool	GetApplied()
	{
		return m_bApplied;
	};

	bool	GetUseRect()
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


