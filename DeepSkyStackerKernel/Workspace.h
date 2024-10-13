#pragma once

class WorkspaceSettings;

class WorkspaceSetting
{
private :
	QString					keyName;
	QVariant				Value;
	bool					dirty;

public :
	WorkspaceSetting(const QString& name, const QVariant& value = QVariant())
		: keyName(name), Value(value), dirty(true)
	{
	};

	WorkspaceSetting & operator = (const WorkspaceSetting & rhs)
	{
		keyName = rhs.keyName;
		Value = rhs.Value;
		dirty = rhs.dirty;
		return (*this);
	};

	bool operator < (const WorkspaceSetting & s) const
	{
		if (keyName < s.keyName)
			return true;
		else if (keyName > s.keyName)
			return false;
		else
			return keyName < s.keyName;
	};

	bool operator != (const WorkspaceSetting & s) const
	{
		return (keyName != s.keyName);
	};

	WorkspaceSetting &	readSetting();
	WorkspaceSetting &	saveSetting();

	bool	isDirty(bool bClear)
	{
		bool result = dirty;

		if (bClear)
			dirty = false;

		return result;
	};

	inline QString key() const
	{
		return keyName;
	};

	WorkspaceSetting &	setValue(const WorkspaceSetting & ws);
	WorkspaceSetting &	setValue(const QVariant& value);

	inline QVariant value() const
	{
		return Value;
	};
};

/* ------------------------------------------------------------------- */

typedef std::vector<WorkspaceSetting> WORKSPACESETTINGVECTOR;
typedef WORKSPACESETTINGVECTOR::iterator WORKSPACESETTINGITERATOR;

class Workspace
{
private:
	std::shared_ptr<WorkspaceSettings> pSettings;
public:
	Workspace();
	~Workspace() = default;

	void setValue(const QString& key, const QVariant& value);

	QVariant value(const QString& key, const QVariant& defaultValue = QVariant{}) const;

	bool isDirty();
	void resetDirty();

	void readSettings();
	void saveSettings();
	void ReadFromFile(FILE* hFile);
	//void ReadFromFile(LPCTSTR name);
	void ReadFromFile(const fs::path& file);
	void SaveToFile(FILE* hFile);
	void SaveToFile(const fs::path& file);
	bool ReadFromString(const QString& string);
	void ResetToDefault();

	void Push();
	void Pop(bool bRestore = true);
};

