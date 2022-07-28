#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include <filesystem>
#include <memory>
#include <QString>
#include <QVariant>
namespace fs = std::filesystem;

class WorkSpaceSettings;

class WorkSpaceSetting
{
private :
	QString					keyName;
	QVariant				Value;
	bool					dirty;

public :
	WorkSpaceSetting(const QString& name, const QVariant& value = QVariant())
		: keyName(name), Value(value), dirty(true)
	{
	};

	WorkSpaceSetting & operator = (const WorkSpaceSetting & rhs)
	{
		keyName = rhs.keyName;
		Value = rhs.Value;
		dirty = rhs.dirty;
		return (*this);
	};

	bool operator < (const WorkSpaceSetting & s) const
	{
		if (keyName < s.keyName)
			return true;
		else if (keyName > s.keyName)
			return false;
		else
			return keyName < s.keyName;
	};

	bool operator != (const WorkSpaceSetting & s) const
	{
		return (keyName != s.keyName);
	};

	WorkSpaceSetting &	readSetting();
	WorkSpaceSetting &	saveSetting();

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

	WorkSpaceSetting &	setValue(const WorkSpaceSetting & ws);
	WorkSpaceSetting &	setValue(const QVariant& value);

	inline QVariant value() const
	{
		return Value;
	};
};

/* ------------------------------------------------------------------- */

typedef std::vector<WorkSpaceSetting>				WORKSPACESETTINGVECTOR;
typedef WORKSPACESETTINGVECTOR::iterator			WORKSPACESETTINGITERATOR;

class CWorkspace
{
private:
	std::shared_ptr <WorkSpaceSettings > pSettings;
public:
	CWorkspace();

	~CWorkspace()
	{
	};

	void	setValue(const QString& key, const QVariant& value);

	QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

	bool	isDirty();
	void	setDirty();

	void	readSettings();
	void	saveSettings();
	void	ReadFromFile(FILE * hFile);
	void	ReadFromFile(const fs::path& fileName);
	void	SaveToFile(FILE * hFile);
	void	SaveToFile(const fs::path& fileName);
	bool	ReadFromString(LPCTSTR szString);
	void	ResetToDefault();

	void	Push();
	void	Pop(bool bRestore = true);
};

#endif // __WORKSPACE_H__
