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

	bool	Read(const QString&  strLine)
	{
		bool			bResult = false;
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
		char			szBuffer[2000];

		if (fgets(szBuffer, sizeof(szBuffer), hFile))
			bResult = Read(QString::fromLatin1(szBuffer));

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
	bool	ReadVariableFromWorkspace(const QString& keyName, const QString& defaultValue, const QString prefix = QString())
	{
		Workspace workspace;
		QString value(workspace.value(keyName).toString());

		if (value.isEmpty())
			value = defaultValue;

		CSetting s;

		if (!prefix.isEmpty())
			s.m_strVariable = QString("%1.%2").arg(prefix).arg(keyName);
		else
			s.m_strVariable = keyName;
		s.m_strValue = value;

		m_sSettings.insert(s);

		return true;
	};

	void	AddVariable(const QString& variable, int lValue)
	{
		CSetting		s;

		s.m_strVariable = variable;
		s.m_strValue = QString("%1").arg(lValue);

		if (m_sSettings.find(s) == m_sSettings.end())
			m_sSettings.insert(s);
	};

	void	AddVariable(const QString& variable, const QString& value)
	{
		CSetting		s;

		s.m_strVariable = variable;
		s.m_strValue    = value;

		if (m_sSettings.find(s) == m_sSettings.end())
			m_sSettings.insert(s);
	};

	void	AddFileVariable(const QString& variable, const fs::path& file)
	{
		CBitmapInfo		bmpInfo;

		// Retrieve the date and time of creation and append it to the file name
		if (GetPictureInfo(file, bmpInfo))
		{
			QString value = QString("%1[%2]").arg(file.generic_u16string(), bmpInfo.m_strDateTime);
			AddVariable(variable, value);
		};
	};

	void	AddRAWSettings()
	{
		ReadVariableFromWorkspace("RawDDP/Brightness", "1.0", "Raw");
		ReadVariableFromWorkspace("RawDDP/RedScale", "1.0", "Raw");
		ReadVariableFromWorkspace("RawDDP/BlueScale", "1.0", "Raw");
		ReadVariableFromWorkspace("RawDDP/NoWB", "0", "Raw");
		ReadVariableFromWorkspace("RawDDP/CameraWB", "0", "Raw");
		ReadVariableFromWorkspace("RawDDP/BlackPointTo0", "0", "Raw");
	};

	void	AddFITSSettings()
	{
		ReadVariableFromWorkspace("FitsDDP/FITSisRAW", "0", "Fits");
		ReadVariableFromWorkspace("FitsDDP/Brightness", "1.0", "Fits");
		ReadVariableFromWorkspace("FitsDDP/RedScale", "1.0", "Fits");
		ReadVariableFromWorkspace("FitsDDP/BlueScale", "1.0", "Fits");
		ReadVariableFromWorkspace("FitsDDP/DSLR", "", "Fits");
		ReadVariableFromWorkspace("FitsDDP/BayerPattern", "4", "Fits");
		ReadVariableFromWorkspace("FitsDDP/ForceUnsigned", "0", "Fits");
	};

public :
	CGlobalSettings()
	{
	};

	virtual ~CGlobalSettings()
	{
	};

	bool	ReadFromFile(const fs::path&);
	bool	InitFromCurrent(CTaskInfo * pTask, const fs::path&);
	void	WriteToFile(const fs::path&);

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
		ReadVariableFromWorkspace("Stacking/Dark_Method", "0");
		ReadVariableFromWorkspace("Stacking/Dark_Iteration", "5");
		ReadVariableFromWorkspace("Stacking/Dark_Kappa", "2.0");
	};

	void	SetMasterOffset(CTaskInfo * pTask)
	{
		if (pTask && !pTask->m_strOutputFile.empty())
			AddFileVariable("MasterOffset", pTask->m_strOutputFile);
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
		ReadVariableFromWorkspace("Stacking/Flat_Method", "0");
		ReadVariableFromWorkspace("Stacking/Flat_Iteration", "5");
		ReadVariableFromWorkspace("Stacking/Flat_Kappa", "2.0");
	};

	void	SetMasterOffset(CTaskInfo * pTask)
	{
		if (pTask && !pTask->m_strOutputFile.empty())
			AddFileVariable("MasterOffset", pTask->m_strOutputFile);
	};
	void	SetMasterDarkFlat(CTaskInfo * pTask)
	{
		if (pTask && !pTask->m_strOutputFile.empty())
			AddFileVariable("MasterDarkFlat", pTask->m_strOutputFile);
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
		ReadVariableFromWorkspace("Stacking/Offset_Method", "0");
		ReadVariableFromWorkspace("Stacking/Offset_Iteration", "5");
		ReadVariableFromWorkspace("Stacking/Offset_Kappa", "2.0");
	};
};

/* ------------------------------------------------------------------- */


