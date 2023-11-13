#include <stdafx.h>
#include "Workspace.h"
#include "DSSCommon.h"
#include "ZExcBase.h"
class WorkspaceSettings
{
public:
	WORKSPACESETTINGVECTOR				m_vSettings;

protected:
	void	CopyFrom(const WorkspaceSettings & ws)
	{
		m_vSettings = ws.m_vSettings;
	};

	void	InitToDefault(WORKSPACESETTINGVECTOR & vSettings);
	void	Init();

public:
	WorkspaceSettings()
	{
		Init();
	};
	virtual ~WorkspaceSettings() {};

	WorkspaceSettings & operator = (const WorkspaceSettings & ws)
	{
		CopyFrom(ws);
		return (*this);
	};

	WorkspaceSettings(WorkspaceSettings const& other)
	{
		CopyFrom(other);
	}

	void	InitFrom(const WorkspaceSettings & ws)
	{
		CopyFrom(ws);
	};

	WORKSPACESETTINGITERATOR	findSetting(const QString& key);
	bool	isDirty();
	void	resetDirty();
	WORKSPACESETTINGITERATOR	end()
	{
		return m_vSettings.end();
	};

	void	readSettings();
	void	saveSettings();
	void	ReadFromFile(FILE * hFile);
	void	ReadFromFile(const fs::path& fileName);
	void	SaveToFile(FILE * hFile);
	void	SaveToFile(const fs::path& fileName);
	bool	ReadFromString(const QString& theString);
	void	ResetToDefault();
};

/* ------------------------------------------------------------------- */

WorkspaceSetting & WorkspaceSetting::readSetting()
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

WorkspaceSetting & WorkspaceSetting::saveSetting()
{
	if (dirty)
	{
		QSettings settings;

		settings.setValue(keyName, Value);
		//dirty = false;		DO NOT RESET DIRTY BIT - that is done by resetDirty()
	}
	return *this;
};


/* ------------------------------------------------------------------- */

WorkspaceSetting & WorkspaceSetting::setValue(const WorkspaceSetting & ws)
{

	if (Value != ws.Value)
	{
		dirty = true;
		Value = ws.Value;
	};
	return *this;
};

/* ------------------------------------------------------------------- */

WorkspaceSetting & WorkspaceSetting::setValue(const QVariant& value)
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

void	WorkspaceSettings::InitToDefault(WORKSPACESETTINGVECTOR & vSettings)
{
	vSettings.clear();
	vSettings.push_back(WorkspaceSetting("Stacking/Light_Method", (uint)MBP_AVERAGE));
	vSettings.push_back(WorkspaceSetting("Stacking/Light_Iteration", (uint)5));
	vSettings.push_back(WorkspaceSetting("Stacking/Light_Kappa", 2.0));

	vSettings.push_back(WorkspaceSetting("Stacking/Debloom", false));

	vSettings.push_back(WorkspaceSetting("Stacking/Dark_Method", (uint)MBP_MEDIAN));
	vSettings.push_back(WorkspaceSetting("Stacking/Dark_Iteration", (uint)5));
	vSettings.push_back(WorkspaceSetting("Stacking/Dark_Kappa", 2.0));

	vSettings.push_back(WorkspaceSetting("Stacking/Flat_Method", (uint)MBP_MEDIAN));
	vSettings.push_back(WorkspaceSetting("Stacking/Flat_Iteration", (uint)5));
	vSettings.push_back(WorkspaceSetting("Stacking/Flat_Kappa", 2.0));

	vSettings.push_back(WorkspaceSetting("Stacking/Offset_Method", (uint)MBP_MEDIAN));
	vSettings.push_back(WorkspaceSetting("Stacking/Offset_Iteration", (uint)5));
	vSettings.push_back(WorkspaceSetting("Stacking/Offset_Kappa", 2.0));

	vSettings.push_back(WorkspaceSetting("Stacking/BackgroundCalibration", false));
	vSettings.push_back(WorkspaceSetting("Stacking/PerChannelBackgroundCalibration", true));

	vSettings.push_back(WorkspaceSetting("Stacking/BackgroundCalibrationInterpolation", (uint)BCI_RATIONAL));
	vSettings.push_back(WorkspaceSetting("Stacking/RGBBackgroundCalibrationMethod", (uint)RBCM_MAXIMUM));

	vSettings.push_back(WorkspaceSetting("Stacking/DarkOptimization", false));

	vSettings.push_back(WorkspaceSetting("Stacking/UseDarkFactor", false));
	vSettings.push_back(WorkspaceSetting("Stacking/DarkFactor", 1.0));

	vSettings.push_back(WorkspaceSetting("Stacking/HotPixelsDetection", true));

	vSettings.push_back(WorkspaceSetting("Stacking/BadLinesDetection", false));

	vSettings.push_back(WorkspaceSetting("Stacking/Mosaic", (uint)0));

	vSettings.push_back(WorkspaceSetting("Stacking/CreateIntermediates", false));
	vSettings.push_back(WorkspaceSetting("Stacking/SaveCalibrated", false));
	vSettings.push_back(WorkspaceSetting("Stacking/SaveCalibratedDebayered", false));

	vSettings.push_back(WorkspaceSetting("Stacking/AlignmentTransformation", (uint)0));
	vSettings.push_back(WorkspaceSetting("Stacking/LockCorners", true));

	vSettings.push_back(WorkspaceSetting("Stacking/PixelSizeMultiplier", (uint)1));

	vSettings.push_back(WorkspaceSetting("Stacking/AlignChannels", false));

	vSettings.push_back(WorkspaceSetting("Stacking/CometStackingMode", (uint)0));

	vSettings.push_back(WorkspaceSetting("Stacking/SaveCometImages", false));
	vSettings.push_back(WorkspaceSetting("Stacking/ApplyFilterToCometImages", true));

	vSettings.push_back(WorkspaceSetting("Stacking/IntermediateFileFormat", (uint)1));

	vSettings.push_back(WorkspaceSetting("Stacking/PCS_DetectCleanHot", false));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_HotFilter", (uint)1));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_HotDetection", (uint)500));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_DetectCleanCold", false));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_ColdFilter", (uint)1));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_ColdDetection", (uint)500));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_SaveDeltaImage", false));
	vSettings.push_back(WorkspaceSetting("Stacking/PCS_ReplaceMethod", (uint)1));

	vSettings.push_back(WorkspaceSetting("Register/PercentStack", (uint)80));
  	vSettings.push_back(WorkspaceSetting("Register/StackAfter", true));
  	vSettings.push_back(WorkspaceSetting("Register/DetectHotPixels", true));
  	vSettings.push_back(WorkspaceSetting("Register/DetectionThreshold", (uint)10));
	vSettings.push_back(WorkspaceSetting("Register/ApplyMedianFilter", false));

	vSettings.push_back(WorkspaceSetting("RawDDP/Brightness", 1.0));
	vSettings.push_back(WorkspaceSetting("RawDDP/RedScale", 1.0));
	vSettings.push_back(WorkspaceSetting("RawDDP/BlueScale", 1.0));
	vSettings.push_back(WorkspaceSetting("RawDDP/NoWB", false));
	vSettings.push_back(WorkspaceSetting("RawDDP/CameraWB", false));
	vSettings.push_back(WorkspaceSetting("RawDDP/BlackPointTo0", false));
	vSettings.push_back(WorkspaceSetting("RawDDP/Interpolation", "Bilinear"));
	vSettings.push_back(WorkspaceSetting("RawDDP/SuperPixels", false));
	vSettings.push_back(WorkspaceSetting("RawDDP/RawBayer", false));
	vSettings.push_back(WorkspaceSetting("RawDDP/AHD", false));

	vSettings.push_back(WorkspaceSetting("FitsDDP/FITSisRAW", false));
	vSettings.push_back(WorkspaceSetting("FitsDDP/Brightness", 1.0));
	vSettings.push_back(WorkspaceSetting("FitsDDP/RedScale", 1.0));
	vSettings.push_back(WorkspaceSetting("FitsDDP/BlueScale", 1.0));
	vSettings.push_back(WorkspaceSetting("FitsDDP/DSLR", ""));
	vSettings.push_back(WorkspaceSetting("FitsDDP/BayerPattern", (uint)4));
	vSettings.push_back(WorkspaceSetting("FitsDDP/ForceUnsigned", false));

	std::sort(vSettings.begin(), vSettings.end());
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::Init()
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

WORKSPACESETTINGITERATOR	WorkspaceSettings::findSetting(const QString& key)
{
	WORKSPACESETTINGITERATOR	it;
	WorkspaceSetting					s(key);

	it = lower_bound(m_vSettings.begin(), m_vSettings.end(), s);
	if (it != m_vSettings.end())
	{
		if ((*it)!=s)
			it = m_vSettings.end();
	};

	return it;
};

/* ------------------------------------------------------------------- */

bool WorkspaceSettings::isDirty()
{
	bool						bResult = false;

	for (size_t i = 0; i < m_vSettings.size() && !bResult; i++)
		bResult = m_vSettings[i].isDirty(false);

	return bResult;
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::resetDirty()
{
	for (size_t i = 0; i < m_vSettings.size(); i++)
		m_vSettings[i].isDirty(true);
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::readSettings()
{
	for (size_t i = 0; i < m_vSettings.size(); i++)
		m_vSettings[i].readSetting();
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::saveSettings()
{
	for (size_t i = 0; i < m_vSettings.size(); i++)
		m_vSettings[i].saveSetting();
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::ReadFromFile(std::FILE * hFile)
{
	char szString[1000];

	while (fgets(szString, sizeof(szString), hFile))
		ReadFromString(QString::fromUtf8(szString));
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::ReadFromFile(const fs::path& file)
{
	if (std::FILE* hFile =
#if defined _WINDOWS
		_wfopen(file.c_str(), L"rt")
#else
		std::fopen(file.c_str(), "rt")
#endif
		)
	{
		ReadFromFile(hFile);
		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::SaveToFile(FILE * hFile)
{
	for (size_t i = 0; i < m_vSettings.size(); i++)
	{
		QVariant value = m_vSettings[i].value();

		fprintf(hFile, "#V5WS#%s#%s\n", m_vSettings[i].key().toUtf8().constData(), value.toString().toUtf8().constData());
	};
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::SaveToFile(const fs::path& file)
{
	if (std::FILE* hFile =
#if defined _WINDOWS
		_wfopen(file.c_str(), L"wt")
#else
		std::fopen(file.c_str(), "wt")
#endif
		)
	{
		SaveToFile(hFile);
		fclose(hFile);
	};
};

/* ------------------------------------------------------------------- */

bool	WorkspaceSettings::ReadFromString(const QString& theString)
{
	bool				bResult = false;

	QString keyName, value;

	static std::map<QString, QString> keyMap {
		{ "Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\Batch\\Position", "Batch/Position/" },
		{ "Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\Recommended\\Position", "Recommended/Position/" },
		{ "Software\\DeepSkyStacker\\DeepSkyStacker\\Dialogs\\StackingSteps\\Position", "StackingSteps/Position/" },
		{ "Software\\DeepSkyStacker\\EditStars", "EditStars/" },
		{ "Software\\DeepSkyStacker\\Folders", "Folders/" },
		{ "Software\\DeepSkyStacker\\FileLists", "FileLists/" },
		{ "Software\\DeepSkyStacker\\FitsDDP", "FitsDDP/" },
		{ "Software\\DeepSkyStacker\\Live", "Live/" },
		{ "Software\\DeepSkyStacker\\Output", "Output/" },
		{ "Software\\DeepSkyStacker\\DeepSkyStacker\\Position", "Position/" },
		{ "Software\\DeepSkyStacker\\RawDDP", "RawDDP/" },
		{ "Software\\DeepSkyStacker\\Register", "Register/" },
		{ "Software\\DeepSkyStacker\\SettingsFiles", "SettingsFiles/" },
		{ "Software\\DeepSkyStacker\\StarMask", "StarMask/" },
		{ "Software\\DeepSkyStacker\\Stacking", "Stacking/" }
	};

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
		// Special Case for Stacking/Mosaic - don't allow Custom mode
		// 
		if ("Stacking/Mosaic" == keyName && "2" == value)
		{
			value.setNum(static_cast<int>(SM_INTERSECTION)); // Force Intersection mode
		}
		//
		// In all cases when we enter here the variable "value" will be
		// a QString.
		// We need to convert it to the same type as is currently stored
		//
		QVariant variant(value);
		QMetaType type = it->value().metaType();
		ZASSERT(variant.canConvert(type));
		variant.convert(type);
		it->setValue(variant);
		bResult = true;
	}

	return bResult;
};

/* ------------------------------------------------------------------- */

void	WorkspaceSettings::ResetToDefault()
{
	WORKSPACESETTINGVECTOR		vDefaults;

	InitToDefault(vDefaults);

	if (vDefaults.size() == m_vSettings.size())
	{
		for (size_t i = 0; i < m_vSettings.size(); i++)
			m_vSettings[i].setValue(vDefaults[i]);
	};
};


namespace {
	std::shared_ptr<WorkspaceSettings> g_pSettings;
	std::mutex mutex;
	std::deque<WorkspaceSettings> g_WSStack;
}


Workspace::Workspace() : pSettings{ g_pSettings }
{
	if (this->pSettings != nullptr) // If g_pSettings was already initialised -> all good and return.
		return;
	std::unique_lock lock{ mutex };
	if (nullptr == g_pSettings) // After acquiring the lock, we need to check again, if another thread has already initialised g_pSettings.
		g_pSettings = std::make_shared<WorkspaceSettings>();
	this->pSettings = g_pSettings;
}


void Workspace::setValue(const QString& key, const QVariant& value)
{
	WORKSPACESETTINGITERATOR				it;

	it = pSettings->findSetting(key);

	if (it != pSettings->end())
		it->setValue(value);
}


QVariant Workspace::value(const QString& key, const QVariant& value) const
{
	 WORKSPACESETTINGITERATOR it = pSettings->findSetting(key);

	if (it != pSettings->end())
		return it->value();
	else
		return value;
}

/* ------------------------------------------------------------------- */

bool Workspace::isDirty()
{
	return pSettings->isDirty();
}


void Workspace::resetDirty()
{
	pSettings->resetDirty();
}


void Workspace::readSettings()
{
	pSettings->readSettings();
}


void Workspace::saveSettings()
{
	pSettings->saveSettings();
}


void Workspace::ReadFromFile(FILE * hFile)
{
	pSettings->ReadFromFile(hFile);
}

void Workspace::ReadFromFile(LPCTSTR name)
{
	fs::path file(name);
	pSettings->ReadFromFile(file);
}

void Workspace::ReadFromFile(const fs::path& file)
{
	pSettings->ReadFromFile(file);
}


void Workspace::SaveToFile(FILE * hFile)
{
	pSettings->SaveToFile(hFile);
}

void Workspace::SaveToFile(const fs::path& file)
{
	pSettings->SaveToFile(file);
}

bool Workspace::ReadFromString(const QString& string)
{
	return pSettings->ReadFromString(string);
}

void Workspace::Push()
{
	g_WSStack.push_back(*(pSettings));
}


void Workspace::ResetToDefault()
{
	pSettings->ResetToDefault();
}


void Workspace::Pop(bool bRestore)
{
	WorkspaceSettings		ws;

	if (g_WSStack.size())
	{
		ws= *(g_WSStack.rbegin());
		g_WSStack.pop_back();
		if (bRestore)
			pSettings->InitFrom(ws);
	}
}
