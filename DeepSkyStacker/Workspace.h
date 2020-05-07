#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__
#include <memory>
#include <QString>
#include <QVariant>

class CWorkspaceSettings;

class CWorkspaceSetting
{
private :
	QString					keyName;
	QVariant				Value;
	QVariant::Type			type;
	bool					dirty;

private :
	void	CopyFrom(const CWorkspaceSetting & s)
	{
		keyName		= s.keyName;
		Value	= s.Value;
		dirty = s.dirty;
	};

    inline void Initialize() 
    {
		dirty	= false;
    }

public :
	CWorkspaceSetting(const QString& name, const QVariant& value = QVariant())
	{
        Initialize();

		keyName = name;
		Value	= value;
		type = value.type();
	};

	CWorkspaceSetting(const char * const aName, const DWORD& aValue)
	{
		QString name(aName); QVariant value((uint)aValue);
		CWorkspaceSetting(name, value);
	};

	CWorkspaceSetting & operator = (const CWorkspaceSetting & s)
	{
		CopyFrom(s);
		return (*this);
	};

	bool operator < (const CWorkspaceSetting & s) const
	{
		if (keyName < s.keyName)
			return true;
		else if (keyName > s.keyName)
			return false;
		else
			return keyName < s.keyName;
	};

	bool operator != (const CWorkspaceSetting & s) const
	{
		return (keyName != s.keyName);
	};

	void	readSetting();
	void	saveSetting() const;

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

	bool	setValue(const CWorkspaceSetting & ws);
	void	setValue(const QVariant& value);

	inline QVariant value() const
	{
		return Value;
	};
};

/* ------------------------------------------------------------------- */

typedef std::vector<CWorkspaceSetting>				WORKSPACESETTINGVECTOR;
typedef WORKSPACESETTINGVECTOR::iterator			WORKSPACESETTINGITERATOR;

class CWorkspace
{
private:
	std::shared_ptr <CWorkspaceSettings > pSettings;
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
	void	ReadFromFile(LPCTSTR szFile);
	void	SaveToFile(FILE * hFile);
	void	SaveToFile(LPCTSTR szFile);
	bool	ReadFromString(LPCTSTR szString);
	void	ResetToDefault();

	void	Push();
	void	Pop(bool bRestore = true);
};

#endif // __WORKSPACE_H__
