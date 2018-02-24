#pragma once


// CIntermediateFiles dialog
#include <ChildProp.h>
#include "BitmapExt.h"
#include <Label.h>
#include "StackingTasks.h"


class CIntermediateFiles : public CChildPropertyPage
{
	DECLARE_DYNAMIC(CIntermediateFiles)

public:
	CIntermediateFiles();
	virtual ~CIntermediateFiles();

// Dialog Data
	enum { IDD = IDD_STACKINGINTERMEDIATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	virtual BOOL OnSetActive();

	void	SetCreateIntermediates(BOOL bCreateIntermediates)
	{
		m_bCreateIntermediates = bCreateIntermediates;
		m_CreateIntermediates.SetCheck(m_bCreateIntermediates);
		UpdateControls();
	};

	void	SetSaveCalibrated(BOOL bSaveCalibrated)
	{
		m_bSaveCalibrated = bSaveCalibrated;
		m_SaveCalibrated.SetCheck(m_bSaveCalibrated);
		UpdateControls();
	};

	void	SetSaveDebayered(BOOL bSaveDebayered)
	{
		m_bSaveDebayered = bSaveDebayered;
		m_SaveDebayered.SetCheck(m_bSaveDebayered);
		UpdateControls();
	};

	void	SetFileFormat(INTERMEDIATEFILEFORMAT lFileFormat)
	{
		m_lSaveFormat = lFileFormat;
		m_SaveAsTIFF.SetCheck(lFileFormat == IFF_TIFF);
		m_SaveAsFITS.SetCheck(lFileFormat == IFF_FITS);
		UpdateControls();
	};

	void	SetRegisteringOnly(BOOL bRegisteringOnly)
	{
		m_bRegisteringOnly = bRegisteringOnly;
	};

	BOOL	GetCreateIntermediates()
	{
		return m_bCreateIntermediates;
	};

	BOOL	GetSaveCalibrated()
	{
		return m_bSaveCalibrated;
	};

	BOOL	GetSaveDebayered()
	{
		return m_bSaveDebayered;
	};

	INTERMEDIATEFILEFORMAT GetFileFormat()
	{
		return m_lSaveFormat;
	};

	DECLARE_MESSAGE_MAP()

private :
	BOOL				m_bFirstActivation;
	BOOL				m_bRegisteringOnly;

	void				UpdateControls();

	afx_msg void OnBnClickedCreateIntermediates();
	afx_msg void OnBnClickedSaveCalibrated();
	afx_msg void OnBnClickedSaveDebayered();
	afx_msg void OnBnClickedSaveAsTIFF();
	afx_msg void OnBnClickedSaveAsFITS();

public :
	CLabel				m_Title;
	CButton				m_CreateIntermediates;
	CButton				m_SaveCalibrated;
	CButton				m_SaveDebayered;
	CButton				m_SaveAsTIFF;
	CButton				m_SaveAsFITS;

	BOOL					m_bCreateIntermediates;
	BOOL					m_bSaveDebayered;
	BOOL					m_bSaveCalibrated;
	INTERMEDIATEFILEFORMAT	m_lSaveFormat;
};
