#pragma once
// DeepSkyStackerLive.h : main header file for the PROJECT_NAME application
//
class CDeepSkyStackerLiveApp : public CWinApp
{
public:
	CDeepSkyStackerLiveApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDeepSkyStackerLiveApp theApp;

CDeepSkyStackerLiveApp *		GetDSSLiveApp();
