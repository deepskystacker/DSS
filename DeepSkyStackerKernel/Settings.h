#pragma once

#include "Workspace.h"


class CSetting
{
public :
	QString			m_strVariable;
	QString			m_strValue;

private :
	void	CopyFrom(const CSetting & s)
	{
		m_strVariable = s.m_strVariable;
		m_strValue	  = s.m_strValue;
	};

public :
	CSetting()
	{
	};

	CSetting(const CSetting & s)
	{
		CopyFrom(s);
	};

	CSetting & operator = (const CSetting & s)
	{
		CopyFrom(s);
		return (*this);
	};

	bool operator == (const CSetting & s) const
	{
		if (m_strVariable == s.m_strVariable)
			return (m_strValue == s.m_strValue);
		else
			return false;
	};

	bool operator < (const CSetting & s) const
	{
		if (m_strVariable < s.m_strVariable)
			return true;
		else
			return false;
	};

	bool	Read(LPCTSTR szLine)
	{
		bool			bResult = false;
		QString			strLine = QString::fromWCharArray(szLine);
		int				nPos;

		nPos = strLine.indexOf("=");
		if (nPos >= 0)
		{
			m_strVariable = strLine.left(nPos).trimmed();
			m_strValue = strLine.right(strLine.length() - nPos - 1).trimmed();
			bResult = true;
		}
		return bResult;
	};

	bool	Read(FILE * hFile)
	{
		bool			bResult = false;
		CHAR			szBuffer[2000];

		if (fgets(szBuffer, sizeof(szBuffer), hFile))
			bResult = Read((LPCTSTR)CA2CTEX<sizeof(szBuffer)>(szBuffer));

		return bResult;
	};

	bool	Write(FILE * hFile)
	{
		bool		bResult = true;

		fprintf(hFile, "%s=%s\n", m_strVariable.toStdString().c_str(), m_strValue.toStdString().c_str());

		return bResult;
	};
};

typedef std::set<CSetting>			SETTINGSET;
typedef SETTINGSET::iterator		SETTINGITERATOR;
typedef SETTINGSET::const_iterator	SETTINGCONSTITERATOR;

/* ------------------------------------------------------------------- */

class CGlobalSettings
{
protected :
	SETTINGSET m_sSettings;
	std::vector<QString> m_vFiles; // Not really files - they are files with date/times on the end, so stored as a string and not a fs::path.

protected :
	bool	ReadVariableFromWorkspace(LPCTSTR szKey, LPCTSTR szDefault, LPCTSTR szPrefix = nullptr)
	{
		Workspace workspace;
		const QString keyName(QString::fromWCharArray(szKey));
		const QString prefix(szPrefix ? QString::fromWCharArray(szPrefix) : "");
		const QString strDefault(QString::fromWCharArray(szDefault));
		QString strValue(workspace.value(keyName).toString());

		if (strValue.isEmpty())
			strValue = strDefault;

		CSetting s;

		if (szPrefix)
			s.m_strVariable = QString("%1.%2").arg(prefix).arg(keyName);
		else
			s.m_strVariable = keyName;
		s.m_strValue = strValue;

		m_sSettings.insert(s);

		return true;
	};

	void	AddVariable(LPCTSTR szVariable, int lValue)
	{
		CSetting		s;

		s.m_strVariable = QString::fromWCharArray(szVariable);
		s.m_strValue = QString("%1").arg(lValue);

		if (m_sSettings.find(s) == m_sSettings.end())
			m_sSettings.insert(s);
	};

	void	AddVariable(LPCTSTR szVariable, LPCTSTR szValue)
	{
		CSetting		s;

		s.m_strVariable = QString::fromWCharArray(szVariable);
		s.m_strValue    = QString::fromWCharArray(szValue);

		if (m_sSettings.find(s) == m_sSettings.end())
			m_sSettings.insert(s);
	};

	void	AddFileVariable(LPCTSTR szVariable, LPCTSTR szFileName)
	{
		CBitmapInfo		bmpInfo;

		// Retrieve the date and time of creation and append it to the file name
		if (GetPictureInfo(szFileName, bmpInfo))
		{
			QString strValue = QString("%1[%2]").arg(szFileName, bmpInfo.m_strDateTime);
			AddVariable(szVariable, strValue.toStdWString().c_str());
		};
	};
	void	AddFileVariable(LPCTSTR szVariable, const fs::path& szFileName)
	{
		CBitmapInfo		bmpInfo;

		// Retrieve the date and time of creation and append it to the file name
		if (GetPictureInfo(szFileName, bmpInfo))
		{
			QString strValue = QString("%1[%2]").arg(szFileName.c_str(), bmpInfo.m_strDateTime);
			AddVariable(szVariable, strValue.toStdWString().c_str());
		};
	};

	void	AddRAWSettings()
	{
		ReadVariableFromWorkspace(_T("RawDDP/Brightness"), _T("1.0"), _T("Raw"));
		ReadVariableFromWorkspace(_T("RawDDP/RedScale"), _T("1.0"), _T("Raw"));
		ReadVariableFromWorkspace(_T("RawDDP/BlueScale"), _T("1.0"), _T("Raw"));
		ReadVariableFromWorkspace(_T("RawDDP/NoWB"), _T("0"), _T("Raw"));
		ReadVariableFromWorkspace(_T("RawDDP/CameraWB"), _T("0"), _T("Raw"));
		ReadVariableFromWorkspace(_T("RawDDP/BlackPointTo0"), _T("0"), _T("Raw"));
	};

	void	AddFITSSettings()
	{
		ReadVariableFromWorkspace(_T("FitsDDP/FITSisRAW"), _T("0"), _T("Fits"));
		ReadVariableFromWorkspace(_T("FitsDDP/Brightness"), _T("1.0"), _T("Fits"));
		ReadVariableFromWorkspace(_T("FitsDDP/RedScale"), _T("1.0"), _T("Fits"));
		ReadVariableFromWorkspace(_T("FitsDDP/BlueScale"), _T("1.0"), _T("Fits"));
		ReadVariableFromWorkspace(_T("FitsDDP/DSLR"), _T(""), _T("Fits"));
		ReadVariableFromWorkspace(_T("FitsDDP/BayerPattern"), _T("4"), _T("Fits"));
		ReadVariableFromWorkspace(_T("FitsDDP/ForceUnsigned"), _T("0"), _T("Fits"));
	};

public :
	CGlobalSettings()
	{
	};

	virtual ~CGlobalSettings()
	{
	};

	bool	ReadFromFile(LPCTSTR szFile);
	bool	InitFromCurrent(CTaskInfo * pTask, LPCTSTR szFile);
	void	WriteToFile(LPCTSTR szFile);

	virtual void	ReadFromRegistry() {};

	bool operator == (const CGlobalSettings & gs) const;
};

/* ------------------------------------------------------------------- */

class CDarkSettings : public CGlobalSettings
{
public :
	CDarkSettings() {}
	virtual ~CDarkSettings() {};

	virtual void	ReadFromRegistry() override
	{
		ReadVariableFromWorkspace(_T("Stacking/Dark_Method"), _T("0"));
		ReadVariableFromWorkspace(_T("Stacking/Dark_Iteration"), _T("5"));
		ReadVariableFromWorkspace(_T("Stacking/Dark_Kappa"), _T("2.0"));
	};

	void	SetMasterOffset(CTaskInfo * pTask)
	{
		if (pTask && !pTask->m_strOutputFile.empty())
			AddFileVariable(_T("MasterOffset"), pTask->m_strOutputFile);
	};
};

/* ------------------------------------------------------------------- */

class CFlatSettings : public CGlobalSettings
{
public :
	CFlatSettings() {}
	virtual ~CFlatSettings() {};

	virtual void	ReadFromRegistry() override
	{
		ReadVariableFromWorkspace(_T("Stacking/Flat_Method"), _T("0"));
		ReadVariableFromWorkspace(_T("Stacking/Flat_Iteration"), _T("5"));
		ReadVariableFromWorkspace(_T("Stacking/Flat_Kappa"), _T("2.0"));
	};

	void	SetMasterOffset(CTaskInfo * pTask)
	{
		if (pTask && !pTask->m_strOutputFile.empty())
			AddFileVariable(_T("MasterOffset"), pTask->m_strOutputFile);
	};
	void	SetMasterDarkFlat(CTaskInfo * pTask)
	{
		if (pTask && !pTask->m_strOutputFile.empty())
			AddFileVariable(_T("MasterDarkFlat"), pTask->m_strOutputFile);
	};
};

/* ------------------------------------------------------------------- */

class COffsetSettings : public CGlobalSettings
{
public :
	COffsetSettings() {}
	virtual ~COffsetSettings() {};

	virtual void	ReadFromRegistry() override
	{
		ReadVariableFromWorkspace(_T("Stacking/Offset_Method"), _T("0"));
		ReadVariableFromWorkspace(_T("Stacking/Offset_Iteration"), _T("5"));
		ReadVariableFromWorkspace(_T("Stacking/Offset_Kappa"), _T("2.0"));
	};
};

/* ------------------------------------------------------------------- */


