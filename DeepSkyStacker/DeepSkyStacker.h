
#if !defined(AFX_DEEPSKYSTACKER_H__DE048E44_D5E4_40E9_9454_5EFC9E73A8C3__INCLUDED_)
#define AFX_DEEPSKYSTACKER_H__DE048E44_D5E4_40E9_9454_5EFC9E73A8C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QTranslator>

class CDeepSkyStackerApp : public CWinApp
{
public :
	CWnd *				m_pMainDlg;
	QTranslator *		appTranslator;
	QTranslator *		qtTranslator;

public :
	CDeepSkyStackerApp()
	{
        m_pMainDlg = nullptr;
		appTranslator = nullptr;
		qtTranslator = nullptr;

		EnableHtmlHelp();
	};

	virtual ~CDeepSkyStackerApp()
	{
		if (appTranslator)
		{
			delete appTranslator;
			appTranslator = nullptr;
		}
		if (qtTranslator)
		{
			delete qtTranslator;
			qtTranslator = nullptr;
		}

	};

	virtual BOOL InitInstance( );
	virtual BOOL Run();

};

CDeepSkyStackerApp *		GetDSSApp();

#endif // !defined(AFX_DEEPSKYSTACKER_H__DE048E44_D5E4_40E9_9454_5EFC9E73A8C3__INCLUDED_)
