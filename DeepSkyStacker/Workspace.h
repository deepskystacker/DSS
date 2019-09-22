#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

typedef enum tagDSSSETTINGTYPE
{
	DST_STRING		= 1,
	DST_DWORD		= 2,
	DST_BOOL		= 3,
	DST_DOUBLE		= 4
}DSSSETTINGTYPE;

class CWorkspaceSetting
{
private :
	DSSSETTINGTYPE			m_Type;
	CString					m_strPath;
	CString					m_strName;
	CString					m_strValue;
	DWORD					m_dwValue;
	double					m_fValue;
	bool					m_bValue;
	BOOL					m_bDirty;

private :
	void	CopyFrom(const CWorkspaceSetting & s)
	{
		m_Type		= s.m_Type;
		m_strPath	= s.m_strPath;
		m_strName	= s.m_strName;
		m_strValue	= s.m_strValue;
		m_dwValue	= s.m_dwValue;
		m_fValue	= s.m_fValue;
		m_bValue	= s.m_bValue;
		m_bDirty	= s.m_bDirty;
	};

    void Initialize()
    {
        m_Type		= DST_STRING;
		m_bDirty	= FALSE;
        m_dwValue   = 0;
        m_fValue    = 0;
        m_bValue    = FALSE;
    }

public :
	CWorkspaceSetting(LPCTSTR szPath, LPCTSTR szName)
	{
        Initialize();

		m_strPath	= szPath;
		m_strName	= szName;
	};

	CWorkspaceSetting(LPCTSTR szPath, LPCTSTR szName, LPCTSTR szDefValue)
	{
        Initialize();

		m_strPath	= szPath;
		m_strName	= szName;
		m_strValue	= szDefValue;
	};

	CWorkspaceSetting(LPCTSTR szPath, LPCTSTR szName, DWORD dwValue)
	{
        Initialize();

		m_strPath	= szPath;
		m_strName	= szName;
		m_dwValue   = dwValue;
	};

	CWorkspaceSetting(LPCTSTR szPath, LPCTSTR szName, bool bValue)
	{
        Initialize();

		m_strPath	= szPath;
		m_strName	= szName;
		m_bValue	= bValue;
	};

	CWorkspaceSetting(LPCTSTR szPath, LPCTSTR szName, double fValue)
	{
        Initialize();

		m_strPath	= szPath;
		m_strName	= szName;
		m_fValue	= fValue;
	};

	CWorkspaceSetting(const CWorkspaceSetting & s)
	{
		CopyFrom(s);
	};

	CWorkspaceSetting & operator = (const CWorkspaceSetting & s)
	{
		CopyFrom(s);
		return (*this);
	};

	bool operator < (const CWorkspaceSetting & s) const
	{
		if (m_strPath < s.m_strPath)
			return true;
		else if (m_strPath > s.m_strPath)
			return false;
		else
			return m_strName < s.m_strName;
	};

	bool operator != (const CWorkspaceSetting & s) const
	{
		return (m_strPath!=s.m_strPath) || (m_strName!=s.m_strName);
	};

	void	ReadFromRegistry();
	void	SaveToRegistry() const;

	BOOL	IsDirty(BOOL bClear)
	{
		BOOL			bResult = m_bDirty;

		if (bClear)
			m_bDirty = FALSE;

		return bResult;
	};

	void	GetPath(CString & strPath) const
	{
		strPath = m_strPath;
	};

	void	GetName(CString & strName) const
	{
		strName = m_strName;
	};

	bool	SetValue(const CWorkspaceSetting & ws);
	void	SetValue(LPCTSTR szValue);
	void	SetValue(bool bValue);
	void	SetValue(DWORD dwValue);
	void	SetValue(double fValue);

	void	GetValue(CString & strValue) const;
	void	GetValue(bool & bValue) const;
	void	GetValue(DWORD & dwValue) const;
	void	GetValue(double & fValue) const;

};

typedef std::vector<CWorkspaceSetting>				WORKSPACESETTINGVECTOR;
typedef WORKSPACESETTINGVECTOR::iterator			WORKSPACESETTINGITERATOR;

/* ------------------------------------------------------------------- */

class CWorkspaceSettingsInternal
{
public :
	WORKSPACESETTINGVECTOR				m_vSettings;

protected :
	void	CopyFrom(const CWorkspaceSettingsInternal & ws)
	{
		m_vSettings = ws.m_vSettings;
	};

	void	InitToDefault(WORKSPACESETTINGVECTOR & vSettings);
	void	Init();

public :
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

	WORKSPACESETTINGITERATOR	FindSetting(LPCTSTR szPath, LPCTSTR szName);
	BOOL	IsDirty();
	void	ResetDirty();
	WORKSPACESETTINGITERATOR	end()
	{
		return m_vSettings.end();
	};

	void	ReadFromRegistry();
	void	SaveToRegistry();
	void	ReadFromFile(FILE * hFile);
	void	ReadFromFile(LPCTSTR szFile);
	void	SaveToFile(FILE * hFile);
	void	SaveToFile(LPCTSTR szFile);
	BOOL	ReadFromString(LPCTSTR szString);
	void	ResetToDefault();
};

/* ------------------------------------------------------------------- */

class CWorkspaceSettings : public CWorkspaceSettingsInternal,
						   public CRefCount
{
public :
	CWorkspaceSettings() {};
	virtual ~CWorkspaceSettings() {};
};

/* ------------------------------------------------------------------- */

class CWorkspace
{
private :
	CSmartPtr<CWorkspaceSettings>		m_pSettings;

public:
	CWorkspace();

	~CWorkspace()
	{
	};

	void	SetValue(LPCTSTR szPath, LPCTSTR szName, LPCTSTR szValue);
	void	SetValue(LPCTSTR szPath, LPCTSTR szName, bool bValue);
	void	SetValue(LPCTSTR szPath, LPCTSTR szName, DWORD dwValue);
	void	SetValue(LPCTSTR szPath, LPCTSTR szName, double fValue);

	void	GetValue(LPCTSTR szPath, LPCTSTR szName, CString & strValue);
	void	GetValue(LPCTSTR szPath, LPCTSTR szName, bool & bValue);
	void	GetValue(LPCTSTR szPath, LPCTSTR szName, DWORD & dwValue);
	void	GetValue(LPCTSTR szPath, LPCTSTR szName, double & fValue);

	BOOL	IsDirty();
	void	ResetDirty();

	void	ReadFromRegistry();
	void	SaveToRegistry();
	void	ReadFromFile(FILE * hFile);
	void	ReadFromFile(LPCTSTR szFile);
	void	SaveToFile(FILE * hFile);
	void	SaveToFile(LPCTSTR szFile);
	BOOL	ReadFromString(LPCTSTR szString);
	void	ResetToDefault();

	void	Push();
	void	Pop(bool bRestore = true);
};

#endif // __WORKSPACE_H__