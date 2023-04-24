#pragma once
#include "StackedSink.h"
#include "resource.h"
#include "gradientctrl.h"
#include "BtnST.h"
#include "ControlPos.h"
#include "Label.h"
#include "BitmapExt.h"


// CImageViewTab dialog
class C32BitsBitmap;
class CMemoryBitmap;
class CImageViewTab : public CDialog
{
	DECLARE_DYNAMIC(CImageViewTab)
private :
	CStackedSink				m_StackedSink;
	CControlPos					m_ControlPos;
	CLabel						m_Background;
	CLabel						m_FileName;
	CStatic						m_PictureStatic;
	CLabel						m_CopyToClipboard;
	CGradientCtrl				m_Gamma;
	CWndImage					m_Picture;
	CButtonST					m_4Corners;
	BOOL						m_bStackedImage;
	std::shared_ptr<C32BitsBitmap>	m_pWndImage;
	std::shared_ptr<CMemoryBitmap>	m_pBitmap;
	DSS::GammaTransformation m_GammaTransformation;
	bool						m_bDarkMode;

public:
	CImageViewTab(CWnd* pParent = nullptr, bool bDarkMode = false);   // standard constructor
	virtual ~CImageViewTab();

	void	SetStackedImage(BOOL bStackedImage)
	{
		m_bStackedImage = bStackedImage;
	};

	//void	CallHelp();

// Dialog Data
	enum { IDD = IDD_IMAGEVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeGamma(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopyToClipboard(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClicked4corners();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

//	afx_msg void OnAbout( NMHDR * pNotifyStruct, LRESULT * result );
//	afx_msg void OnHelp( NMHDR * pNotifyStruct, LRESULT * result );

public :
	void	SetImage(const std::shared_ptr<CMemoryBitmap>& pBitmap, const std::shared_ptr<C32BitsBitmap>& pWndBitmap, LPCTSTR szFileName);
	void	OnStackedImageSaved();
	void	OnSetFootprint(QPointF const& pt1, QPointF const& pt2, QPointF const& pt3, QPointF const& pt4);

};
