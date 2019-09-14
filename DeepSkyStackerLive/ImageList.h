#ifndef __IMAGELISTTAB_H__
#define __IMAGELISTTAB_H__

#pragma once

#include "afxwin.h"
#include "label.h"
#include <ControlPos.h>
#include <WndImage.h>
#include "DSSProgress.h"
#include "DSSTools.h"
#include "BitmapExt.h"
#include "ListViewCtrlEx.h"

// CImageListTab dialog

class CImageListTab : public CDialog
{
	DECLARE_DYNAMIC(CImageListTab)
private :
	CControlPos			m_ControlPos;
	CListCtrlEx			m_ImageList;

public:
	CImageListTab(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageListTab();


// Dialog Data
	enum { IDD = IDD_IMAGELIST };

	void SetToDarkMode();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	virtual BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);

private :
	void	InitList();

public :
	void	AddImage(LPCTSTR szImage);
	void	ChangeImageStatus(LPCTSTR szImage, IMAGESTATUS status);
	void	UpdateImageOffsets(LPCTSTR szImage, double fdX, double fdY, double fAngle);
	BOOL	Close();
};


#endif