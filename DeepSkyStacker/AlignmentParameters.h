#pragma once


// CAlignmentParameters dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>


class CAlignmentParameters : public CChildPropertyPage
{
	DECLARE_DYNAMIC(CAlignmentParameters)

public:
	CAlignmentParameters();
	virtual ~CAlignmentParameters();

// Dialog Data
	enum { IDD = IDD_ALIGNMENTSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnSetActive();

	void	SetAlignment(WORD wAlignment)
	{
		m_Alignment = wAlignment;
		UpdateControls();
	};

	WORD	GetAlignment()
	{
		return m_Alignment;
	};


	DECLARE_MESSAGE_MAP()

private :
	BOOL				m_bFirstActivation;

	void				UpdateControls();

	afx_msg void OnBnClickedAutomatic();
	afx_msg void OnBnClickedBilinear();
	afx_msg void OnBnClickedBisquared();
	afx_msg void OnBnClickedBicubic();
	afx_msg void OnBnClickedNoAlignment();

public :
	CButton				m_Automatic;
	CButton				m_Bilinear;
	CButton				m_Bicubic;
	CButton				m_Bisquared;
	CButton				m_NoAlignment;
	CStatic				m_Explanation;
	CLabel				m_Title;

	WORD				m_Alignment;
};
