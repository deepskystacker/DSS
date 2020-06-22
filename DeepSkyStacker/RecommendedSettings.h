#pragma once

#include "afxcmn.h"
#include "StackingTasks.h"
#include "Workspace.h"
#include <ControlPos.h>
#include "EasySize.h"
#include <QString>
#include <QVariant>

// CRecommendedSettings dialog

/* ------------------------------------------------------------------- */

class CRecommendationItem
{
public :
	WORKSPACESETTINGVECTOR				m_vSettings;
	LONG								m_lLinkID;
	CString								m_strRecommendation;


private:
	void	CopyFrom(const CRecommendationItem & right)
	{
		m_vSettings			= right.m_vSettings;
		m_strRecommendation = right.m_strRecommendation;
		m_lLinkID			= right.m_lLinkID;
	};

public:
	CRecommendationItem()
	{
		m_lLinkID = 0;
	};
	~CRecommendationItem()
	{
	};

	CRecommendationItem(const CRecommendationItem & right)
	{
		CopyFrom(right);
	};

	CRecommendationItem & operator = (const CRecommendationItem & right)
	{
		CopyFrom(right);
		return (*this);
	};

	void	Clear()
	{
		m_vSettings.clear();
		m_lLinkID = 0;
		m_strRecommendation.Empty();
	};

	bool	IsDifferent()
	{
		bool					bResult = false;
		CWorkspace				workspace;

		// Check that the current values are (or not)
		for (LONG i = 0;i<m_vSettings.size() && !bResult;i++)
		{
			CString				strValue;
			QString				keyName;
			CString				strCurrentValue;
			QString	temp;

			keyName = m_vSettings[i].key();

			temp = workspace.value(keyName).toString();
			strCurrentValue = CString((LPCTSTR)temp.utf16());
			temp = m_vSettings[i].value().toString();
			strValue = CString((LPCTSTR)temp.utf16());

			bResult = (strCurrentValue != strValue);
		};

		return bResult;
	};

	void	ApplySettings()
	{
		CWorkspace				workspace;

		for (LONG i = 0;i<m_vSettings.size();i++)
		{
			QString				keyName;
			QVariant			value;

			keyName = m_vSettings[i].key();
			value = m_vSettings[i].value();

			workspace.setValue(keyName, value);
		};
	};

	void	SetRecommendation(DWORD dwID)
	{
		CString				strText;

		strText.LoadString(dwID);
		SetRecommendation(strText);
	};

	void	SetRecommendation(LPCTSTR szText)
	{
		m_strRecommendation = szText;
	};
	void	AddSetting(QString keyName, QVariant value)
	{
		m_vSettings.emplace_back(keyName, value);
	};
};

typedef std::vector<CRecommendationItem>			RECOMMENDATIONITEMVECTOR;

/* ------------------------------------------------------------------- */

class CRecommendation
{
public:
	RECOMMENDATIONITEMVECTOR			m_vRecommendations;
	bool								m_bApplicable;
	bool								m_bImportant;
	bool								m_bBreakBefore;
	CString								m_strText;

	void	CopyFrom(const CRecommendation & right)
	{
		m_bApplicable		= right.m_bApplicable;
		m_bBreakBefore		= right.m_bBreakBefore;
		m_bImportant		= right.m_bImportant;
		m_strText			= right.m_strText;
		m_vRecommendations	= right.m_vRecommendations;
	};

public:
	CRecommendation()
	{
		m_bApplicable = true;
		m_bImportant  = true;
		m_bBreakBefore= false;
	};
	~CRecommendation()
	{
	};

	CRecommendation(const CRecommendation & right)
	{
		CopyFrom(right);
	};

	CRecommendation & operator = (const CRecommendation & right)
	{
		CopyFrom(right);
		return (*this);
	};

	bool	IsApplicable()
	{
		return m_bApplicable;
	};

	bool	IsBreakBefore()
	{
		return m_bBreakBefore;
	};

	void	SetText(DWORD dwID)
	{
		CString				strText;

		strText.LoadString(dwID);
		SetText(strText);
	};

	void	SetText(LPCTSTR szText)
	{
		m_strText = szText;
	};

	void	AddItem(const CRecommendationItem & ri)
	{
		m_vRecommendations.push_back(ri);
	};
};


typedef std::vector<CRecommendation>	RECOMMANDATIONVECTOR;
typedef RECOMMANDATIONVECTOR::iterator	RECOMMANDATIONITERATOR;

/* ------------------------------------------------------------------- */

class CRecommendedSettings : public CDialog
{
	DECLARE_DYNAMIC(CRecommendedSettings)
	DECLARE_EASYSIZE

public:
	CRecommendedSettings(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CRecommendedSettings();

	void		setStackingTasks(CAllStackingTasks * pStackingTasks)
	{
		m_pStackingTasks = pStackingTasks;
	};

// Dialog Data
	enum { IDD = IDD_RECOMMENDEDSETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnSizing(UINT nSide, LPRECT lpRect);

	DECLARE_MESSAGE_MAP()
private :
	void	ClearText();
	void	InsertHeader();
	void	InsertText(LPCTSTR szText, COLORREF crColor = RGB(0, 0, 0), bool bBold = false, bool bItalic = false, LONG lLinkID = 0);
	void	FillWithRecommendedSettings();
	void	SetSetting(LONG lID = 0);

private:
	CQhtmWnd								m_RecommendedSettingsHTML;
	CButton									m_ShowAll;
	CAllStackingTasks *						m_pStackingTasks;
	RECOMMANDATIONVECTOR					m_vRecommendations;
	CAllStackingTasks						m_StackingTasks;
	CScrollBar								m_Gripper;

	afx_msg void OnQHTMHyperlink(NMHDR*nmh, LRESULT*);

	afx_msg void OnBnClickedShowAll();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};

/* ------------------------------------------------------------------- */
