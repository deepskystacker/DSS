#pragma once

// CFileProperty dialog
#include "PictureListCtrl.h"
#include <unordered_map>

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

    CPictureListCtrl*   m_imageList;
	std::unordered_map<int, ListBitMap*> m_bitmaps;
	bool				m_bChangeType;
	bool				m_bChangeISOSpeed;
	bool				m_bChangeExposure;


public:
	CFileProperty(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFileProperty();

	void AddBitmap(int itemId, ListBitMap* bitmap)
	{
        m_bitmaps[itemId] = bitmap;
	};

    void SetImageList(CPictureListCtrl* imagelist)
    {
        m_imageList = imagelist;
    }

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
