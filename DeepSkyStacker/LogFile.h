#ifndef __LOGFILE_H__
#define __LOGFILE_H__

void	CreateLogFile(LPCTSTR szReferenceFile);
void	AddTimeToLogFile();
void	AddToLogFile(LPCTSTR szFormat, ...);
void	CloseLogFile();


/* ------------------------------------------------------------------- */

#ifdef DSSBETA
void		StartLog();
void		AddToLog(LPCTSTR szFormat, ...);
#else
#define StartLog(...)
#define AddToLog(...)
#endif

/* ------------------------------------------------------------------- */

#endif // __LOGFILE_H__