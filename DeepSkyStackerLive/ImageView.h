#ifndef __IMAGEVIEWTAB_H__
#define __IMAGEVIEWTAB_H__

#pragma once

#include "afxwin.h"
#include "label.h"
#include <BtnST.h>
#include <ControlPos.h>
#include <WndImage.h>
#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "GradientCtrl.h"

// CImageViewTab dialog

#include "StackedSink.h"

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
	CSmartPtr<C32BitsBitmap>	m_pWndImage;
	CSmartPtr<CMemoryBitmap>	m_pBitmap;
	CGammaTransformation		m_GammaTransformation;
	bool						m_bDarkMode;

public:
	CImageViewTab(CWnd* pParent = NULL, bool bDarkMode = false);   // standard constructor
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
	void	SetImage(CMemoryBitmap * pBitmap, C32BitsBitmap * pWndBitmap, LPCTSTR szFileName);
	void	OnStackedImageSaved();
	void	OnSetFootprint(CPointExt const& pt1, CPointExt const& pt2, CPointExt const& pt3, CPointExt const& pt4);

};


#endif