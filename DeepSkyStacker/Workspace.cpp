#include <stdafx.h>
#include "Workspace.h"
#include "Registry.h"
#include "BitmapExt.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "StackingTasks.h"
#include <algorithm>
#include <deque>


/* ------------------------------------------------------------------- */

void CWorkspaceSetting::ReadFromRegistry()
{
	CRegistry				reg;
	DWORD					dwValue;
	CString					strValue;
	CRegVal					regval;

	if (reg.LoadKey(m_strPath, m_strName, regval))
	{
		switch (regval.GetType())
		{
		case REG_SZ :
			regval.GetValue(strValue);
			SetValue(strValue);
			break;
		case REG_DWORD :
			regval.GetValue(dwValue);
			SetValue(dwValue);
			break;
		};

		/*
		switch (m_Type)
		{
		case DST_STRING :
			reg.LoadKey(m_strPath, m_strName, m_strValue);
			break;
		case DST_BOOL : 
			dwValue = (DWORD)m_bValue;
			reg.LoadKey(m_strPath, m_strName, dwValue);
			m_bValue = dwValue ? true : false;
			break;
		case DST_DWORD :
			reg.LoadKey(m_strPath, m_strName, m_dwValue);
			break;
		case DST_DOUBLE :
			strValue.Format("%.2f", m_fValue);
			reg.LoadKey(m_strPath, m_strName, strValue);
			m_fValue = _ttof(strValue);
			break;
		};
		*/
		m_bDirty = FALSE;
	};
};

/* ------------------------------------------------------------------- */

void CWorkspaceSetting::SaveToRegistry() const
{
	CRegistry				reg;
	DWORD					dwValue;
	CString					strValue;

	switch (m_Type)
	{
	case DST_STRING :
		reg.SaveKey(m_strPath, m_strName, m_strValue);
		break;
	case DST_BOOL : 
		dwValue = (DWORD)m_bValue;
		reg.SaveKey(m_strPath, m_strName, dwValue);
		break;
	case DST_DWORD :
		reg.SaveKey(m_strPath, m_strName, m_dwValue);
		break;
	case DST_DOUBLE :
		strValue.Format(_T("%.4f"), m_fValue);
		reg.SaveKey(m_strPath, m_strName, strValue);
		break;
	};

	//m_bDirty = FALSE;
};

/* ------------------------------------------------------------------- */

bool	CWorkspaceSetting::SetValue(const CWorkspaceSetting & ws)
{
	bool				bResult = false;
	
	// Assume that this is the same type
	if (m_Type == ws.m_Type)
	{
		switch (m_Type)
		{
		case DST_STRING :
			if (m_strValue != ws.m_strValue)
			{
				m_bDirty = TRUE;
				m_strValue = ws.m_strValue;
			};
			break;
		case DST_BOOL : 
			if (m_bValue != ws.m_bValue)
			{
				m_bDirty = TRUE;
				m_bValue = ws.m_bValue;
			};
			break;
		case DST_DWORD :
			if (m_dwValue != ws.m_dwValue)
			{
				m_bDirty = TRUE;
				m_dwValue = ws.m_dwValue;
			};
			break;
		case DST_DOUBLE :
			if (m_fValue != ws.m_fValue)
			{
				m_bDirty = TRUE;
				m_fValue = ws.m_fValue;
			};
			break;
		};
		bResult = true;
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::SetValue(LPCTSTR szValue)
{
	CString				strValue;
	bool				bValue;
	DWORD				dwValue;
	double				fValue;

	switch (m_Type)
	{
	case DST_STRING :
		strValue = szValue;
		if (strValue != m_strValue)
			m_bDirty = TRUE;
		m_strValue = strValue;
		break;
	case DST_BOOL : 
		bValue = _ttol(szValue) ? true : false;
		if (bValue != m_bValue)
			m_bDirty = TRUE;
		m_bValue = bValue;
		break;
	case DST_DWORD :
		dwValue = _ttol(szValue);
		if (dwValue != m_dwValue)
			m_bDirty = TRUE;
		m_dwValue = dwValue;
		break;
	case DST_DOUBLE :
		fValue = _ttof(szValue);
		if (fValue != m_fValue)
			m_bDirty = TRUE;
		m_fValue = fValue;
		break;
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::SetValue(bool bNewValue)
{
	CString				strValue;
	bool				bValue;
	DWORD				dwValue;
	double				fValue;

	switch (m_Type)
	{
	case DST_STRING :
		strValue = bNewValue ? "1" : "0";
		if (strValue != m_strValue)
			m_bDirty = TRUE;
		m_strValue = strValue;
		break;
	case DST_BOOL : 
		bValue = bNewValue;
		if (bValue != m_bValue)
			m_bDirty = TRUE;
		m_bValue = bValue;
		break;
	case DST_DWORD :
		dwValue = bNewValue ? 1 : 0;
		if (dwValue != m_dwValue)
			m_bDirty = TRUE;
		m_dwValue = dwValue;
		break;
	case DST_DOUBLE :
		fValue = bNewValue ? 1 : 0;
		if (fValue != m_fValue)
			m_bDirty = TRUE;
		m_fValue = fValue;
		break;
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::SetValue(DWORD dwNewValue)
{
	CString				strValue;
	bool				bValue;
	DWORD				dwValue;
	double				fValue;

	switch (m_Type)
	{
	case DST_STRING :
		strValue.Format(_T("%ld"), dwNewValue);
		if (strValue != m_strValue)
			m_bDirty = TRUE;
		m_strValue = strValue;
		break;
	case DST_BOOL : 
		bValue = dwNewValue ? true : false;
		if (bValue != m_bValue)
			m_bDirty = TRUE;
		m_bValue = bValue;
		break;
	case DST_DWORD :
		dwValue = dwNewValue;
		if (dwValue != m_dwValue)
			m_bDirty = TRUE;
		m_dwValue = dwValue;
		break;
	case DST_DOUBLE :
		fValue = dwNewValue;
		if (fValue != m_fValue)
			m_bDirty = TRUE;
		m_fValue = fValue;
		break;
	};
};


/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::SetValue(double fNewValue)
{
	CString				strValue;
	bool				bValue;
	DWORD				dwValue;
	double				fValue;

	switch (m_Type)
	{
	case DST_STRING :
		strValue.Format(_T("%.4f"), fNewValue);
		if (strValue != m_strValue)
			m_bDirty = TRUE;
		m_strValue = strValue;
		break;
	case DST_BOOL : 
		bValue = fNewValue ? true : false;
		if (bValue != m_bValue)
			m_bDirty = TRUE;
		m_bValue = bValue;
		break;
	case DST_DWORD :
		dwValue = fNewValue;
		if (dwValue != m_dwValue)
			m_bDirty = TRUE;
		m_dwValue = dwValue;
		break;
	case DST_DOUBLE :
		fValue = fNewValue;
		if (fValue != m_fValue)
			m_bDirty = TRUE;
		m_fValue = fValue;
		break;
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::GetValue(CString & strValue) const
{
	switch (m_Type)
	{
	case DST_STRING :
		strValue = m_strValue;
		break;
	case DST_BOOL : 
		strValue = m_bValue ? "1" : "0";
		break;
	case DST_DWORD :
		strValue.Format(_T("%ld"), m_dwValue);
		break;
	case DST_DOUBLE :
		strValue.Format(_T("%.4f"), m_fValue);
		break;
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::GetValue(bool & bValue) const
{
	switch (m_Type)
	{
	case DST_STRING :
		bValue = _ttol(m_strValue) ? true : false;
		break;
	case DST_BOOL : 
		bValue = m_bValue;
		break;
	case DST_DWORD :
		bValue = m_dwValue ? true : false;
		break;
	case DST_DOUBLE :
		bValue = m_fValue ? true : false;
		break;
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::GetValue(DWORD & dwValue) const
{
	switch (m_Type)
	{
	case DST_STRING :
		dwValue = _ttol(m_strValue);
		break;
	case DST_BOOL : 
		dwValue = m_bValue ? 1 : 0;
		break;
	case DST_DWORD :
		dwValue = m_dwValue;
		break;
	case DST_DOUBLE :
		dwValue = m_fValue;
		break;
	};
};


/* ------------------------------------------------------------------- */

void	CWorkspaceSetting::GetValue(double & fValue) const
{
	switch (m_Type)
	{
	case DST_STRING :
		fValue = _ttof(m_strValue);
		break;
	case DST_BOOL : 
		fValue = m_bValue ? 1 : 0;
		break;
	case DST_DWORD :
		fValue = m_dwValue;
		break;
	case DST_DOUBLE :
		fValue = m_fValue;
		break;
	};
};


/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::InitToDefault(WORKSPACESETTINGVECTOR & vSettings)
{
	vSettings.clear();
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Method"), (DWORD)MBP_AVERAGE));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Iteration"), (DWORD)5));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Light_Kappa"), 2.0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Debloom"), false));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Method"), (DWORD)MBP_MEDIAN));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Iteration"), (DWORD)5));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Dark_Kappa"), 2.0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Method"), (DWORD)MBP_MEDIAN));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Iteration"), (DWORD)5));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Flat_Kappa"), 2.0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Method"), (DWORD)MBP_MEDIAN));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Iteration"), (DWORD)5));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Offset_Kappa"), 2.0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibration"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PerChannelBackgroundCalibration"), true));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BackgroundCalibrationInterpolation"), (DWORD)BCI_RATIONAL));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("RGBBackgroundCalibrationMethod"), (DWORD)RBCM_MAXIMUM));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkOptimization"), false));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("UseDarkFactor"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("DarkFactor"), 1.0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("HotPixelsDetection"), true));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("BadLinesDetection"), false));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("Mosaic"), (DWORD)0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CreateIntermediates"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibrated"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCalibratedDebayered"), false));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignmentTransformation"), (DWORD)0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("LockCorners"), true));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PixelSizeMultiplier"), (DWORD)1));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("AlignChannels"), false));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("CometStackingMode"), (DWORD)0));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("SaveCometImages"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("ApplyFilterToCometImages"), true));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("IntermediateFileFormat"), (DWORD)1));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_DetectCleanHot"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_HotFilter"), (DWORD)1));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_HotDetection"), (DWORD)500));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_DetectCleanCold"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ColdFilter"), (DWORD)1));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ColdDetection"), (DWORD)500));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_SaveDeltaImage"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("PCS_ReplaceMethod"), (DWORD)1));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("PercentStack"), (DWORD)80));
  	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("StackAfter"), true));
  	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectHotPixels"), true));
  	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("DetectionThreshold"), (DWORD)10));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_REGISTERSETTINGS, _T("ApplyMedianFilter"), false));
  
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("Brighness"), 1.0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("RedScale"), 1.0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlueScale"), 1.0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("AutoWB"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("CameraWB"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("BlackPointTo0"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("Interpolation"), _T("Bilinear")));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("SuperPixels"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("RawBayer"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_RAWSETTINGS, _T("AHD"), false));

	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("FITSisRAW"), false));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("Brighness"), 1.0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("RedScale"), 1.0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("BlueScale"), 1.0));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("DSLR"), _T("")));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("BayerPattern"), (DWORD)4));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("Interpolation"), _T("Bilinear")));
	vSettings.push_back(CWorkspaceSetting(REGENTRY_BASEKEY_FITSSETTINGS, _T("ForceUnsigned"), false));

	std::sort(vSettings.begin(), vSettings.end());
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::Init()
{
	InitToDefault(m_vSettings);
	ReadFromRegistry();
};

/* ------------------------------------------------------------------- */

WORKSPACESETTINGITERATOR	CWorkspaceSettingsInternal::FindSetting(LPCTSTR szPath, LPCTSTR szName)
{
	WORKSPACESETTINGITERATOR	it;
	CWorkspaceSetting					s(szPath, szName);

	it = lower_bound(m_vSettings.begin(), m_vSettings.end(), s);
	if (it != m_vSettings.end())
	{
		if ((*it)!=s)
			it = m_vSettings.end();
	};

	return it;
};

/* ------------------------------------------------------------------- */

BOOL CWorkspaceSettingsInternal::IsDirty()
{
	BOOL						bResult = FALSE;

	for (LONG i = 0;i<m_vSettings.size() && !bResult;i++)
		bResult = m_vSettings[i].IsDirty(FALSE);

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::ResetDirty()
{
	for (LONG i = 0;i<m_vSettings.size();i++)
		m_vSettings[i].IsDirty(TRUE);
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::ReadFromRegistry()
{
	for (LONG i = 0;i<m_vSettings.size();i++)
		m_vSettings[i].ReadFromRegistry();
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::SaveToRegistry()
{
	for (LONG i = 0;i<m_vSettings.size();i++)
		m_vSettings[i].SaveToRegistry();
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::ReadFromFile(FILE * hFile)
{
	CHAR				szString[1000];

	while (fgets(szString, sizeof(szString), hFile))
		ReadFromString((LPCTSTR)CA2CTEX<sizeof(szString)>(szString));
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::ReadFromFile(LPCTSTR szFile)
{
	FILE *				hFile;

	hFile = _tfopen(szFile, _T("rt"));
	if (hFile)
	{
		ReadFromFile(hFile);
		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::SaveToFile(FILE * hFile)
{
	for (LONG i = 0;i<m_vSettings.size();i++)
	{
		CString				strPath;
		CString				strName;
		CString				strValue;

		m_vSettings[i].GetPath(strPath);
		m_vSettings[i].GetName(strName);
		m_vSettings[i].GetValue(strValue);

		fprintf(hFile, "#WS#%s|%s=%s\n", (LPCSTR)CT2CA(strPath), (LPCSTR)CT2CA(strName), (LPCSTR)CT2CA(strValue));
	};
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::SaveToFile(LPCTSTR szFile)
{
	FILE *				hFile;

	hFile = _tfopen(szFile, _T("wt"));
	if (hFile)
	{
		SaveToFile(hFile);
		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

BOOL	CWorkspaceSettingsInternal::ReadFromString(LPCTSTR szString)
{
	BOOL				bResult = FALSE;
	CString				strString = szString;
	CString				strPrefix;

	strString.TrimRight(_T("\n"));
	strPrefix = strString.Left(4);
	if (strPrefix == _T("#WS#"))
	{
		// It seems that it is a workspace setting
		// Decod the path, name and value
		int				nSepPos;
		int				nEqualPos;

		nSepPos = strString.Find('|');
		nEqualPos = strString.Find('=');

		if (nSepPos > 0 && nEqualPos > nSepPos)
		{
			CString		strPath;
			CString		strName;
			CString		strValue;

			// 012345678901234567890123456789 
			// #WS#HKCU\MYPATH|MYNAME=MYVALUE
			// Length    = 30
			// nSepPos   = 15
			// nEqualPos = 22
			// PathStartPos = 4

			// PathLength   = 11 = nSepPos-4
			// NameStartPos = 16 = nSepPos+1
			// NameLength   = 6  = nEqualPos-nSepPos-1
			// ValueStartPos= 23 = nEqualPos+1
			// ValueLength  = 7  = Length - nEqualPos -1

			strPath  = strString.Mid(4, nSepPos-4);
			strName  = strString.Mid(nSepPos+1, nEqualPos-nSepPos-1);
			strValue = strString.Mid(nEqualPos+1, strString.GetLength()-nEqualPos-1);

			WORKSPACESETTINGITERATOR			it;

			it = FindSetting(strPath, strName);
			if (it != m_vSettings.end())
				it->SetValue(strValue);

			bResult = TRUE;
		};
	};

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::ResetToDefault()
{
	WORKSPACESETTINGVECTOR		vDefaults;

	InitToDefault(vDefaults);

	if (vDefaults.size() == m_vSettings.size())
	{
		for (LONG i = 0;i<m_vSettings.size();i++)
			m_vSettings[i].SetValue(vDefaults[i]);
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

static CSmartPtr<CWorkspaceSettings>			g_pSettings;
static CComAutoCriticalSection					g_WSCriticalSection;
static std::deque<CWorkspaceSettingsInternal>	g_WSStack;

/* ------------------------------------------------------------------- */

CWorkspace::CWorkspace()
{
	g_WSCriticalSection.Lock();
	if (!g_pSettings)
		g_pSettings.Create();
	g_WSCriticalSection.Unlock();
	m_pSettings = g_pSettings;
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SetValue(LPCTSTR szPath, LPCTSTR szName, LPCTSTR szValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);

	if (it != m_pSettings->end())
		it->SetValue(szValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SetValue(LPCTSTR szPath, LPCTSTR szName, bool bValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->SetValue(bValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SetValue(LPCTSTR szPath, LPCTSTR szName, DWORD dwValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->SetValue(dwValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SetValue(LPCTSTR szPath, LPCTSTR szName, double fValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->SetValue(fValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::GetValue(LPCTSTR szPath, LPCTSTR szName, CString & strValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->GetValue(strValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::GetValue(LPCTSTR szPath, LPCTSTR szName, bool & bValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->GetValue(bValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::GetValue(LPCTSTR szPath, LPCTSTR szName, DWORD & dwValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->GetValue(dwValue);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::GetValue(LPCTSTR szPath, LPCTSTR szName, double & fValue)
{
	WORKSPACESETTINGITERATOR				it;

	it = m_pSettings->FindSetting(szPath, szName);
	
	if (it != m_pSettings->end())
		it->GetValue(fValue);
};

/* ------------------------------------------------------------------- */

BOOL	CWorkspace::IsDirty()
{
	return m_pSettings->IsDirty();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::ResetDirty()
{
	m_pSettings->ResetDirty();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::ReadFromRegistry()
{
	m_pSettings->ReadFromRegistry();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SaveToRegistry()
{
	m_pSettings->SaveToRegistry();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::ReadFromFile(FILE * hFile)
{
	m_pSettings->ReadFromFile(hFile);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::ReadFromFile(LPCTSTR szFile)
{
	m_pSettings->ReadFromFile(szFile);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SaveToFile(FILE * hFile)
{
	m_pSettings->SaveToFile(hFile);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SaveToFile(LPCTSTR szFile)
{
	m_pSettings->SaveToFile(szFile);
};

/* ------------------------------------------------------------------- */

BOOL CWorkspace::ReadFromString(LPCTSTR szString)
{
	return m_pSettings->ReadFromString(szString);
};

/* ------------------------------------------------------------------- */

void CWorkspace::Push()
{
	g_WSStack.push_back(*(m_pSettings));
};

/* ------------------------------------------------------------------- */

void CWorkspace::ResetToDefault()
{
	m_pSettings->ResetToDefault();
};

/* ------------------------------------------------------------------- */

void CWorkspace::Pop(bool bRestore)
{
	CWorkspaceSettingsInternal		wsi;

	if (g_WSStack.size())
	{
		wsi = *(g_WSStack.rbegin());
		g_WSStack.pop_back();
		if (bRestore)
			m_pSettings->InitFrom(wsi);
	};
};

/* ------------------------------------------------------------------- */
