#pragma once
#include "afxwin.h"
#include "label.h"
#include "ScrollDialog.h"


// CExplorerBar dialog

class CExplorerBar : public CScrollDialog
{
	DECLARE_DYNAMIC(CExplorerBar)

public:
	CExplorerBar(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CExplorerBar();

	void	CallHelp();

// Dialog Data
	enum { IDD = IDD_EXPLORERBAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected :
	void	DrawSubFrameRect(CDC * pDC, const CRect & rc);
	void	DrawGradientRect(CDC * pDC, const CRect & rc, COLORREF crColor1, COLORREF crColor2, double fAlpha = 0.0);
	void	DrawGradientBackgroundRect(CDC * pDC, const CRect & rc);
	void	DrawGradientFrameRect(CDC * pDC, const CRect & rc, BOOL bActive, BOOL bShadow);

	void	LoadSettingFile();
	void	SaveSettingFile();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown( UINT, CPoint );
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	virtual BOOL OnInitDialog();
	afx_msg void OnStackingTitle( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnProcessingTitle( NMHDR * pNotifyStruct, LRESULT * result );

	afx_msg void OnRegisteringRegisterChecked( NMHDR * pNotifyStruct, LRESULT * result );

	afx_msg void OnStackingOpenFiles( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingOpenDarks( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingOpenDarkFlats( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingOpenFlats( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingOpenOffsets( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingClearList( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingCheckAll( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingCheckAbove( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingUncheckAll( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingComputeOffsets( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingStackChecked( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingBatchStack( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingLoadList( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnStackingSaveList( NMHDR * pNotifyStruct, LRESULT * result );

	afx_msg void OnProcessingOpenFile( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnProcessingCopyToClipboard( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnProcessingCreateStarMask( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnProcessingSaveFile( NMHDR * pNotifyStruct, LRESULT * result );

	afx_msg void OnOptionsRawDDPSettings( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnOptionsSettings( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnOptionsLoadSettings( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnOptionsSaveSettings( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnOptionsRecommandedSettings( NMHDR * pNotifyStruct, LRESULT * result );

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnAbout( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnHelp( NMHDR * pNotifyStruct, LRESULT * result );

private :
	CBitmap		m_Background;
	CRect		m_rcBackground;
	DWORD		m_dwCurrentTabID;
	int			m_nScrollPos;
	std::vector<CLabel *>	m_vLabels;

	void InitLabel(CLabel & label, BOOL bMain = FALSE);

public :
	CRect		m_rcRegisterStack;
	CRect		m_rcProcessing;

	CStatic		m_ProcessingRect;
	CStatic		m_StackingRect;
	CStatic		m_OptionsRect;

	CStatic		m_SubRect1;
	CStatic		m_SubRect2;
	CStatic		m_SubRect3;

	CLabel		m_Stacking_Title;
	CLabel		m_Processing_Title;
	CLabel		m_Options_Title;

	CLabel		m_Register_OpenFiles;
	CLabel		m_Register_ClearList;
	CLabel		m_Register_CheckAll;
	CLabel		m_Register_UncheckAll;
	CLabel		m_Register_RegisterChecked;

	CLabel		m_Stacking_OpenFiles;
	CLabel		m_Stacking_OpenDarks;
	CLabel		m_Stacking_OpenDarkFlats;
	CLabel		m_Stacking_OpenFlats;
	CLabel		m_Stacking_OpenOffsets;
	CLabel		m_Stacking_LoadList;
	CLabel		m_Stacking_SaveList;
	CLabel		m_Stacking_ClearList;
	CLabel		m_Stacking_CheckAll;
	CLabel		m_Stacking_CheckAbove;
	CLabel		m_Stacking_UncheckAll;
	CLabel		m_Stacking_ComputeOffsets;
	CLabel		m_Stacking_StackChecked;
	CLabel		m_Stacking_BatchStack;

	CLabel		m_Processing_OpenFile;
	CLabel		m_Processing_CopyToClipboard;
	CLabel		m_Processing_CreateStarMask;
	CLabel		m_Processing_SaveFile;

	CLabel		m_Options_RawDDPSettings;
	CLabel		m_Options_Settings;
	CLabel		m_Options_LoadSettings;
	CLabel		m_Options_SaveSettings;
	CLabel		m_Options_RecommandedSettings;

	CLabel		m_About;
	CLabel		m_Help;

	CMRUList	m_MRUSettings;
};
