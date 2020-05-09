#pragma once


// CStackSettings dialog
#include "StackingParameters.h"
#include "ResultParameters.h"
#include "AlignmentParameters.h"
#include "IntermediateFiles.h"
#include "CometStacking.h"
#include "PostCalibration.h"
#include "OutputTab.h"
#include "afxwin.h"
#include <RichToolTipCtrl.h>

const LONG					SSTAB_RESULT			= 1;
const LONG					SSTAB_LIGHT				= 2;
const LONG					SSTAB_DARK				= 3;
const LONG					SSTAB_FLAT				= 4;
const LONG					SSTAB_OFFSET			= 5;
const LONG					SSTAB_ALIGNMENT			= 6;
const LONG					SSTAB_INTERMEDIATE		= 7;
const LONG					SSTAB_COMET				= 8;
const LONG					SSTAB_POSTCALIBRATION	= 9;
const LONG					SSTAB_OUTPUT			= 10;

class CStackSettings : public CDialog
{
	DECLARE_DYNAMIC(CStackSettings)

private :
	CPropertySheet			m_Sheet;
	CResultParameters		m_tabResult;
	CStackingParameters		m_tabLightFrames;
	CStackingParameters		m_tabDarkFrames;
	CStackingParameters		m_tabFlatFrames;
	CStackingParameters		m_tabOffsetFrames;
	CAlignmentParameters	m_tabAlignment;
	CIntermediateFiles		m_tabIntermediate;
	CCometStacking			m_tabComet;
	CPostCalibration		m_tabPostCalibration;
	COutputTab				m_tabOutput;
	CStatic					m_TabRect;
	CStatic					m_TempFolder;
	CButton					m_UseAllProcessors;
	CButton					m_ReducePriority;
	CButton					m_OK;
	LONG					m_lStartingTab;
	bool					m_bRegisteringOnly;
	bool					m_bEnableCustomRectangle;
	bool					m_bUseCustomRectangle;
	bool					m_bEnableCometStacking;
	bool					m_bEnableDark;
	bool					m_bEnableFlat;
	bool					m_bEnableBias;
	bool					m_bEnableAll;
	CAllStackingTasks *		m_pStackingTasks;

public:
	CStackSettings(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStackSettings();

	void		UpdateControls();

	void		SetCustomRectangleAvailability(bool bEnabled, bool bUsed = false)
	{
		m_bEnableCustomRectangle	= bEnabled;
		m_bUseCustomRectangle		= bUsed;
	};

	bool		IsCustomRectangleUsed()
	{
		return m_bEnableCustomRectangle && m_bUseCustomRectangle;
	};

	void		SetStartingTab(LONG lStartingTab)
	{
		m_lStartingTab = lStartingTab;
	};

	void		SetRegisteringOnly(bool bRegisteringOnly)
	{
		m_bRegisteringOnly = bRegisteringOnly;
	};

	void		EnableCometStacking(bool bEnable)
	{
		m_bEnableCometStacking = bEnable;
	};

	void		SetStackingTasks(CAllStackingTasks * pStackingTasks)
	{
		m_pStackingTasks = pStackingTasks;
	};

	void		SetDarkFlatBiasTabsVisibility(bool bDark, bool bFlat, bool bBias)
	{
		m_bEnableDark = bDark;
		m_bEnableFlat = bFlat;
		m_bEnableBias = bBias;
	};

	void		SetEnableAll(bool bEnableAll)
	{
		m_bEnableAll = bEnableAll;
	};

// Dialog Data
	enum { IDD = IDD_STACKSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	bool	CheckTabControls(CStackingParameters * pTab);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedChangetempfolder();
};
