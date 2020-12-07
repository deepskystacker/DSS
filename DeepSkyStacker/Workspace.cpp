#include <stdafx.h>
#include "Workspace.h"

#include "BitmapExt.h"
#include "DSSProgress.h"
#include "DSSTools.h"
#include "StackingTasks.h"
#include <algorithm>
#include <deque>
#include <memory>
#include <QDebug>
#include <QGlobalStatic>
#include <QMutex>
#include <QSettings>

class CWorkspaceSettingsInternal
{
public:
	WORKSPACESETTINGVECTOR				m_vSettings;

protected:
	void	CopyFrom(const CWorkspaceSettingsInternal & ws)
	{
		m_vSettings = ws.m_vSettings;
	};

	void	InitToDefault(WORKSPACESETTINGVECTOR & vSettings);
	void	Init();

public:
	CWorkspaceSettingsInternal()
	{
		Init();
	};
	virtual ~CWorkspaceSettingsInternal() {};

	CWorkspaceSettingsInternal & operator = (const CWorkspaceSettingsInternal & ws)
	{
		CopyFrom(ws);
		return (*this);
	};

	CWorkspaceSettingsInternal(CWorkspaceSettingsInternal const& other)
	{
		CopyFrom(other);
	}

	void	InitFrom(const CWorkspaceSettingsInternal & ws)
	{
		CopyFrom(ws);
	};

	WORKSPACESETTINGITERATOR	findSetting(const QString& key);
	bool	isDirty();
	void	setDirty();
	WORKSPACESETTINGITERATOR	end()
	{
		return m_vSettings.end();
	};

	void	readSettings();
	void	saveSettings();
	void	ReadFromFile(FILE * hFile);
	void	ReadFromFile(LPCTSTR szFile);
	void	SaveToFile(FILE * hFile);
	void	SaveToFile(LPCTSTR szFile);
	bool	ReadFromString(LPCTSTR szString);
	void	ResetToDefault();
};

/* ------------------------------------------------------------------- */

class CWorkspaceSettings : public CWorkspaceSettingsInternal,
	public CRefCount
{
public:
	CWorkspaceSettings() {};
	virtual ~CWorkspaceSettings() {};

};

/* ------------------------------------------------------------------- */

CWorkspaceSetting & CWorkspaceSetting::readSetting()
{
	QSettings settings;

	// If there wasn't a valid stored value in the settings, we will be returned 
	// a null QVariant. We for sure don't want to use that!
	QVariant temp = settings.value(keyName);
	if (!temp.isNull())
	{
		Value = temp;
		dirty = false;
	}
	return *this;
};

/* ------------------------------------------------------------------- */

CWorkspaceSetting & CWorkspaceSetting::saveSetting()
{
	if (dirty)
	{
		QSettings settings;

		settings.setValue(keyName, Value);
		dirty = false;
	}
	return *this;
};


/* ------------------------------------------------------------------- */

CWorkspaceSetting & CWorkspaceSetting::setValue(const CWorkspaceSetting & ws)
{

	if (Value != ws.Value)
	{
		dirty = true;
		Value = ws.Value;
	};
	return *this;
};

/* ------------------------------------------------------------------- */

CWorkspaceSetting & CWorkspaceSetting::setValue(const QVariant& value)
{
	if (Value != value)
	{
		dirty = true;
		Value = value;
	}
	return *this;
};


/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::InitToDefault(WORKSPACESETTINGVECTOR & vSettings)
{
	vSettings.clear();
	vSettings.push_back(CWorkspaceSetting("Stacking/Light_Method", (uint)MBP_AVERAGE));
	vSettings.push_back(CWorkspaceSetting("Stacking/Light_Iteration", (uint)5));
	vSettings.push_back(CWorkspaceSetting("Stacking/Light_Kappa", double(2.0));

	vSettings.push_back(CWorkspaceSetting("Stacking/Debloom", false));

	vSettings.push_back(CWorkspaceSetting("Stacking/Dark_Method", (uint)MBP_MEDIAN));
	vSettings.push_back(CWorkspaceSetting("Stacking/Dark_Iteration", (uint)5));
	vSettings.push_back(CWorkspaceSetting("Stacking/Dark_Kappa", 2.0));

	vSettings.push_back(CWorkspaceSetting("Stacking/Flat_Method", (uint)MBP_MEDIAN));
	vSettings.push_back(CWorkspaceSetting("Stacking/Flat_Iteration", (uint)5));
	vSettings.push_back(CWorkspaceSetting("Stacking/Flat_Kappa", 2.0));

	vSettings.push_back(CWorkspaceSetting("Stacking/Offset_Method", (uint)MBP_MEDIAN));
	vSettings.push_back(CWorkspaceSetting("Stacking/Offset_Iteration", (uint)5));
	vSettings.push_back(CWorkspaceSetting("Stacking/Offset_Kappa", 2.0));

	vSettings.push_back(CWorkspaceSetting("Stacking/BackgroundCalibration", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/PerChannelBackgroundCalibration", true));

	vSettings.push_back(CWorkspaceSetting("Stacking/BackgroundCalibrationInterpolation", (uint)BCI_RATIONAL));
	vSettings.push_back(CWorkspaceSetting("Stacking/RGBBackgroundCalibrationMethod", (uint)RBCM_MAXIMUM));

	vSettings.push_back(CWorkspaceSetting("Stacking/DarkOptimization", false));

	vSettings.push_back(CWorkspaceSetting("Stacking/UseDarkFactor", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/DarkFactor", 1.0));

	vSettings.push_back(CWorkspaceSetting("Stacking/HotPixelsDetection", true));

	vSettings.push_back(CWorkspaceSetting("Stacking/BadLinesDetection", false));

	vSettings.push_back(CWorkspaceSetting("Stacking/Mosaic", (uint)0));

	vSettings.push_back(CWorkspaceSetting("Stacking/CreateIntermediates", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/SaveCalibrated", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/SaveCalibratedDebayered", false));

	vSettings.push_back(CWorkspaceSetting("Stacking/AlignmentTransformation", (uint)0));
	vSettings.push_back(CWorkspaceSetting("Stacking/LockCorners", true));

	vSettings.push_back(CWorkspaceSetting("Stacking/PixelSizeMultiplier", (uint)1));

	vSettings.push_back(CWorkspaceSetting("Stacking/AlignChannels", false));

	vSettings.push_back(CWorkspaceSetting("Stacking/CometStackingMode", (uint)0));

	vSettings.push_back(CWorkspaceSetting("Stacking/SaveCometImages", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/ApplyFilterToCometImages", true));

	vSettings.push_back(CWorkspaceSetting("Stacking/IntermediateFileFormat", (uint)1));

	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_DetectCleanHot", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_HotFilter", (uint)1));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_HotDetection", (uint)500));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_DetectCleanCold", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_ColdFilter", (uint)1));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_ColdDetection", (uint)500));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_SaveDeltaImage", false));
	vSettings.push_back(CWorkspaceSetting("Stacking/PCS_ReplaceMethod", (uint)1));

	vSettings.push_back(CWorkspaceSetting("Register/PercentStack", (uint)80));
  	vSettings.push_back(CWorkspaceSetting("Register/StackAfter", true));
  	vSettings.push_back(CWorkspaceSetting("Register/DetectHotPixels", true));
  	vSettings.push_back(CWorkspaceSetting("Register/DetectionThreshold", (uint)10));
	vSettings.push_back(CWorkspaceSetting("Register/ApplyMedianFilter", false));

	vSettings.push_back(CWorkspaceSetting("RawDDP/Brightness", 1.0));
	vSettings.push_back(CWorkspaceSetting("RawDDP/RedScale", 1.0));
	vSettings.push_back(CWorkspaceSetting("RawDDP/BlueScale", 1.0));
	vSettings.push_back(CWorkspaceSetting("RawDDP/NoWB", false));
	vSettings.push_back(CWorkspaceSetting("RawDDP/CameraWB", false));
	vSettings.push_back(CWorkspaceSetting("RawDDP/BlackPointTo0", false));
	vSettings.push_back(CWorkspaceSetting("RawDDP/Interpolation", "Bilinear"));
	vSettings.push_back(CWorkspaceSetting("RawDDP/SuperPixels", false));
	vSettings.push_back(CWorkspaceSetting("RawDDP/RawBayer", false));
	vSettings.push_back(CWorkspaceSetting("RawDDP/AHD", false));

	vSettings.push_back(CWorkspaceSetting("FitsDDP/FITSisRAW", false));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/Brightness", 1.0));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/RedScale", 1.0));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/BlueScale", 1.0));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/DSLR", ""));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/BayerPattern", (uint)4));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/Interpolation", "Bilinear"));
	vSettings.push_back(CWorkspaceSetting("FitsDDP/ForceUnsigned", false));

	std::sort(vSettings.begin(), vSettings.end());
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::Init()
{
	InitToDefault(m_vSettings);		// Set up default values for all the main settings, and mark all as dirty
	//
	// Now read all the settings that have previously been saved.
	//
	// NOTE WELL:
	// When a non-null setting is read from wherever QSettings has saved it
	// that setting value in the settings cache (this workspace) will have its
	// dirty flag reset so saveSettings() won't store it again.
	//
	readSettings();		// Read all settings that were previously hardened to wherever by QSettings
	//
	// Now write out the settings whose dirty flag is still set
	// IOW those that were NOT read by readSettings().
	//
	saveSettings();						// Save all dirty settings.
};

/* ------------------------------------------------------------------- */

WORKSPACESETTINGITERATOR	CWorkspaceSettingsInternal::findSetting(const QString& key)
{
	WORKSPACESETTINGITERATOR	it;
	CWorkspaceSetting					s(key);

	it = lower_bound(m_vSettings.begin(), m_vSettings.end(), s);
	if (it != m_vSettings.end())
	{
		if ((*it)!=s)
			it = m_vSettings.end();
	};

	return it;
};

/* ------------------------------------------------------------------- */

bool CWorkspaceSettingsInternal::isDirty()
{
	bool						bResult = false;

	for (LONG i = 0;i<m_vSettings.size() && !bResult;i++)
		bResult = m_vSettings[i].isDirty(false);

	return bResult;
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::setDirty()
{
	for (LONG i = 0;i<m_vSettings.size();i++)
		m_vSettings[i].isDirty(true);
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::readSettings()
{
	for (LONG i = 0;i<m_vSettings.size();i++)
		m_vSettings[i].readSetting();
};

/* ------------------------------------------------------------------- */

void	CWorkspaceSettingsInternal::saveSettings()
{
	for (LONG i = 0;i<m_vSettings.size();i++)
		m_vSettings[i].saveSetting();
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
		QVariant value = m_vSettings[i].value();

		fprintf(hFile, "#V5WS#%s#%s\n", m_vSettings[i].key().toUtf8().constData(), value.toString().toUtf8().constData());
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

bool	CWorkspaceSettingsInternal::ReadFromString(LPCTSTR szString)
{
	bool				bResult = false;

	//
	// Convert the line to a QString and strip leading and trailing white space
	//
	QString	theString = QString::fromWCharArray(szString).trimmed();
	QString keyName, value;

	static std::map<QString, QString> keyMap;
	if (keyMap.empty())
	{
		keyMap.emplace("Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\Batch\\Position", "Batch/Position/");
		keyMap.emplace("Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\Recommended\\Position", "Recommended/Position/");
		keyMap.emplace("Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\StackingSteps\\Position", "StackingSteps/Position/");
		keyMap.emplace("Software\\DeepSkyStacker\\EditStars", "EditStars/");
		keyMap.emplace("Software\\DeepSkyStacker\\Folders", "Folders/");
		keyMap.emplace("Software\\DeepSkyStacker\\FileLists", "FileLists/");
		keyMap.emplace("Software\\DeepSkyStacker\\FitsDDP", "FitsDDP/");
		keyMap.emplace("Software\\DeepSkyStacker\\Live", "Live/");
		keyMap.emplace("Software\\DeepSkyStacker\\Output", "Output/");
		keyMap.emplace("Software\\DeepSkyStacker\\DeepSkyStacker\\Position", "Position/");
		keyMap.emplace("Software\\DeepSkyStacker\\RawDDP", "RawDDP/");
		keyMap.emplace("Software\\DeepSkyStacker\\Register", "Register/");
		keyMap.emplace("Software\\DeepSkyStacker\\SettingsFiles", "SettingsFiles/");
		keyMap.emplace("Software\\DeepSkyStacker\\StarMask", "StarMask/");
		keyMap.emplace("Software\\DeepSkyStacker\\Stacking", "Stacking/");
	}

	if (theString.startsWith("#WS#"))
	{
		QString temp = theString.section("#", 2);
		QString regKey = temp.section("|", 0, 0);
		QString nameAndValue = temp.section("|", 1);
		QString name = nameAndValue.section("=", 0, 0);

		//
		// Fix issue with changed spelling of "Brightness" - DSS4 and below spelt it "Brighness"
		//
		if ("Brighness" == name)
			name = "Brightness";
		
		value = nameAndValue.section("=", 1);

		auto keyIter = keyMap.find(regKey);
		ZASSERT(keyMap.end() != keyIter);

		//
		// Get the root of our QSettings key name
		//
		keyName = (keyIter->second) + name;
	}
	else if (theString.startsWith("#V5WS#"))
	{
		keyName = theString.section("#", 2, 2);
		value = theString.section("#", 3);
	}
	else
	{
		return false;
	}

	WORKSPACESETTINGITERATOR it = findSetting(keyName);
	if (it != m_vSettings.end())
	{
		//
		// In all cases when we enter here the variable "value" will be
		// a QString.
		// We need to convert it to the same type as is currently stored
		//
		QVariant variant(value);
		QVariant::Type type = it->value().type();
		ZASSERT(variant.canConvert(type));
		variant.convert(type);
		it->setValue(variant);
		bResult = true;
	}

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
			m_vSettings[i].setValue(vDefaults[i]);
	};
};

/* ------------------------------------------------------------------- */
/* ------------------------------------------------------------------- */
Q_GLOBAL_STATIC(QMutex, theLock);

std::shared_ptr <CWorkspaceSettings> g_pSettings;

static std::deque<CWorkspaceSettingsInternal>	g_WSStack;

/* ------------------------------------------------------------------- */

CWorkspace::CWorkspace()
{ 
	theLock->lock();
	if (nullptr == g_pSettings)
	{
		g_pSettings = std::make_shared <CWorkspaceSettings>();
	}
	theLock->unlock();

	pSettings = g_pSettings;
};

/* ------------------------------------------------------------------- */

void	CWorkspace::setValue(const QString& key, const QVariant& value)
{
	WORKSPACESETTINGITERATOR				it;

	it = pSettings->findSetting(key);

	if (it != pSettings->end())
		it->setValue(value);
};


/* ------------------------------------------------------------------- */

QVariant	CWorkspace::value(const QString& key, const QVariant& value) const
{
	WORKSPACESETTINGITERATOR				it;

	it = pSettings->findSetting(key);

	if (it != pSettings->end())
		return it->value();
	else
		return value;
};

/* ------------------------------------------------------------------- */

bool	CWorkspace::isDirty()
{
	return pSettings->isDirty();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::setDirty()
{
	pSettings->setDirty();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::readSettings()
{
	pSettings->readSettings();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::saveSettings()
{
	pSettings->saveSettings();
};

/* ------------------------------------------------------------------- */

void	CWorkspace::ReadFromFile(FILE * hFile)
{
	pSettings->ReadFromFile(hFile);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::ReadFromFile(LPCTSTR szFile)
{
	pSettings->ReadFromFile(szFile);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SaveToFile(FILE * hFile)
{
	pSettings->SaveToFile(hFile);
};

/* ------------------------------------------------------------------- */

void	CWorkspace::SaveToFile(LPCTSTR szFile)
{
	pSettings->SaveToFile(szFile);
};

/* ------------------------------------------------------------------- */

bool CWorkspace::ReadFromString(LPCTSTR szString)
{
	return pSettings->ReadFromString(szString);
};

/* ------------------------------------------------------------------- */

void CWorkspace::Push()
{
	g_WSStack.push_back(*(pSettings));
};

/* ------------------------------------------------------------------- */

void CWorkspace::ResetToDefault()
{
	pSettings->ResetToDefault();
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
			pSettings->InitFrom(wsi);
	};
};

/* ------------------------------------------------------------------- */
