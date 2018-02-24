#include <stdafx.h>
#include <stdlib.h>
#include "Registry.h"
#include "LogFile.h"

static FILE *				g_hLogFile = NULL;

/* ------------------------------------------------------------------- */

static BOOL	IsLogFileEnabled()
{
	CRegistry			reg;
	DWORD				dwValue = 0;

	reg.LoadKey(REGENTRY_BASEKEY_STACKINGSETTINGS, _T("LogEnabled"), dwValue);

	return dwValue;
};

/* ------------------------------------------------------------------- */

void	CreateLogFile(LPCTSTR szReferenceFile)
{
	if (IsLogFileEnabled())
	{
		TCHAR				szDrive[_MAX_DRIVE];
		TCHAR				szDir[_MAX_DIR];
		CString				strLogFile;

		_splitpath(szReferenceFile, szDrive, szDir, NULL, NULL);

		strLogFile = szDrive;
		strLogFile += szDir;
		strLogFile += "StackingLogFile.txt";

		CloseLogFile();
		g_hLogFile = fopen(strLogFile, "wt");
	};
};

/* ------------------------------------------------------------------- */

void	AddToLogFile(LPCTSTR szFormat, ...)
{
	va_list			argptr;

	va_start(argptr, szFormat);

	if (g_hLogFile)
	{
		CString		str;

		str.FormatV(szFormat, argptr);
		fprintf(g_hLogFile, (LPCTSTR)str); 
		fflush(g_hLogFile);
	};

	va_end(argptr);
};

/* ------------------------------------------------------------------- */

void	AddTimeToLogFile()
{
	SYSTEMTIME			Time;
	GetSystemTime(&Time);

	AddToLogFile("> %d/%d/%d - %d:%d:%d\n",
		Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
};

/* ------------------------------------------------------------------- */

void	CloseLogFile()
{
	if (g_hLogFile)
	{
		fclose(g_hLogFile);
		g_hLogFile = NULL;
	};
};

#ifdef DSSBETA

/* ------------------------------------------------------------------- */

static	CString		g_strLog;

static	void	GetLogFileName(CString & strLog)
{
	if (!g_strLog.GetLength())
	{
		CHAR			szModuleFileName[_MAX_PATH];
		CHAR			szDrive[_MAX_DRIVE];
		CHAR			szDir[_MAX_DIR];

		GetModuleFileName(NULL, szModuleFileName, sizeof(szModuleFileName));
		_splitpath(szModuleFileName, szDrive, szDir, NULL, NULL);
		g_strLog.Format("%s%s/DSSLog.txt", szDrive, szDir);
	};
	strLog = g_strLog;
};

void		StartLog()
{
	FILE *			hLog;
	CString			strLog;

	GetLogFileName(strLog);

	hLog = fopen(strLog, "wt");
	if (hLog)
	{
		SYSTEMTIME			Time;
		GetSystemTime(&Time);

		fprintf(hLog, "Starting DSS - > %d/%d/%d - %d:%d:%d\n",
			Time.wYear, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute, Time.wSecond);
		fclose(hLog);
	};

};

/* ------------------------------------------------------------------- */

void		AddToLog(LPCTSTR szFormat, ...)
{
	FILE *			hLog;
	CString			strLog;

	va_list			argptr;

	va_start(argptr, szFormat);

	GetLogFileName(strLog);

	hLog = fopen(strLog, "at");
	if (hLog)
	{
		CString		str;

		str.FormatV(szFormat, argptr);
		fprintf(hLog, (LPCTSTR)str); 
		fclose(hLog);
	};

	va_end(argptr);
};

#endif

/* ------------------------------------------------------------------- */
