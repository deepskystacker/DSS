#pragma once


// CCometStacking dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>
#include "StackingTasks.h"


class CCometStacking : public CChildPropertyPage
{
	DECLARE_DYNAMIC(CCometStacking)

public:
	CCometStacking();
	virtual ~CCometStacking();

	void	SetCometStackingMode(COMETSTACKINGMODE CometStackingMode)
	{
		m_CometStackingMode = CometStackingMode;
		UpdateControls();
	};

	COMETSTACKINGMODE	GetCometStackingMode()
	{
		return m_CometStackingMode;
	};

// Dialog Data
	enum { IDD = IDD_STACKINGCOMET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnSetActive();

	DECLARE_MESSAGE_MAP()

private :
	bool				m_bFirstActivation;

	void				UpdateControls();

	afx_msg void OnBnClickedStandardStacking();
	afx_msg void OnBnClickedCometStacking();
	afx_msg void OnBnClickedAdvancedStacking();

public :
	CLabel				m_Title;
	CButton				m_StandardStacking;
	CButton				m_CometStacking;
	CButton				m_AdvancedStacking;
	CStatic				m_Preview;

	COMETSTACKINGMODE	m_CometStackingMode;
};
