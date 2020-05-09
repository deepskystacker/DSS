#pragma once


// CResultParameters dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>
#include "StackingTasks.h"


class CResultParameters : public CChildPropertyPage
{
	DECLARE_DYNAMIC(CResultParameters)

public:
	CResultParameters();
	virtual ~CResultParameters();

// Dialog Data
	enum { IDD = IDD_RESULTSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnSetActive();

	void	SetResultMode(STACKINGMODE Mode)
	{
		m_Mosaic.SetCheck(Mode == SM_MOSAIC);
		m_Normal.SetCheck(Mode == SM_NORMAL);
		m_Intersection.SetCheck(Mode==SM_INTERSECTION);
		m_Custom.SetCheck(false);
		m_ResultMode = Mode;
		UpdateControls();
	};

	STACKINGMODE	GetResultMode()
	{
		return m_ResultMode;
	};

	void	SetCustom(bool bEnable, bool bUse)
	{
		if (bUse)
			bEnable = true;

		m_bEnableCustom = bEnable;
		m_bUseCustom    = bUse;

		m_Custom.EnableWindow(bEnable);

		if (bUse)
		{
			m_Mosaic.SetCheck(false);
			m_Normal.SetCheck(false);
			m_Intersection.SetCheck(false);
			m_Custom.SetCheck(true);
		};
		UpdateControls();
	};

	bool	GetCustom()
	{
		return m_bUseCustom;
	};

	void	SetDrizzle(LONG lDrizzle)
	{
		m_lDrizzle = lDrizzle;
		UpdateControls();
	};

	LONG	GetDrizzle()
	{
		return m_lDrizzle;
	};

	void	SetAlignChannels(bool bAlignChannels)
	{
		m_bAlignChannels = bAlignChannels;
		UpdateControls();
	};

	bool	GetAlignChannels()
	{
		return m_bAlignChannels;
	};

	DECLARE_MESSAGE_MAP()

private :
	bool				m_bFirstActivation;

	void				UpdateControls();

	afx_msg void OnBnClickedNormal();
	afx_msg void OnBnClickedMosaic();
	afx_msg void OnBnClickedCustom();
	afx_msg void OnBnClickedIntersection();
	afx_msg void OnBnClicked2xDrizzle();
	afx_msg void OnBnClicked3xDrizzle();

public :
	CButton				m_Normal;
	CButton				m_Mosaic;
	CButton				m_Custom;
	CStatic				m_ModeText;
	CButton				m_Intersection;
	CStatic				m_Preview;
	CButton				m_Drizzlex2;
	CButton				m_Drizzlex3;
	CButton				m_AlignChannels;

	STACKINGMODE		m_ResultMode;
	bool				m_bEnableCustom;
	bool				m_bUseCustom;
	LONG				m_lDrizzle;
	bool				m_bAlignChannels;
	afx_msg void OnBnClickedAlignchannels();
};
