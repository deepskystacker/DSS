#pragma once

// CFileProperty dialog
#include "PictureListCtrl.h"

class CFileProperty : public CDialog
{
	DECLARE_DYNAMIC(CFileProperty)
private :
	CStatic				m_FileName;
	CStatic				m_DateTime;
	CStatic				m_Sizes;
	CStatic				m_Depth;
	CStatic				m_Info;
	CStatic				m_CFA;
	CComboBox			m_Type;
	CComboBox			m_ISOSpeed;
	CEdit				m_Exposure;
	CButton				m_OK;

	LISTPBITMAPVECTOR	m_vpBitmaps;
	BOOL				m_bChangeType;
	BOOL				m_bChangeISOSpeed;
	BOOL				m_bChangeExposure;


public:
	CFileProperty(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFileProperty();

	void	AddBitmap(CListBitmap * pBitmap)
	{
		m_vpBitmaps.push_back(pBitmap);
	};

// Dialog Data
	enum { IDD = IDD_PROPERTIES };

protected:
	void	InitControls();
	void	UpdateControls();
	void	ApplyChanges();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected :
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeType();
	afx_msg void OnCbnEditupdateIsospeed();
	afx_msg void OnEnChangeExposure();
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnCbnSelchangeIsospeed();
};
