#ifndef __DSSVERSION_H_
#define __DSSVERSION_H_

//#define DSSBETA

#ifdef DSSBETA
#define DSSBETARELEASE 51
#define DSSBETAEXPIREYEAR 2013
#define DSSBETAEXPIREMONTH 12
#endif

#define DSSVER_MAJOR			3
#define DSSVER_MINOR			3
#define DSSVER_SUB				6

#ifdef DSSBETA
#define DSSVER_BUILD			DSSBETARELEASE
#else
#define DSSVER_BUILD			0
#endif

#define FB(arg)					#arg

#ifdef DSSBETA
#define VERSTRING(M, m, s, b)	FB(M)"."FB(m)"."FB(s)" beta "FB(b)
#define VERSION_DEEPSKYSTACKER	VERSTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB, DSSVER_BUILD)
#else
#define VERSTRING(M, m, s)		FB(M)"."FB(m)"."FB(s)
#define VERSION_DEEPSKYSTACKER	VERSTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB)
#endif

#define VERFILESTRING1(VER)		FB(VER)
#define VERFILESTRING(M, m, s, b) FB(M)\
								"."\
								FB(m)\
								"."\
								FB(s)\
								"."\
								FB(b)
#define DSSVER_FILEVERSION		VERFILESTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB, DSSVER_BUILD)
#define DSSVER_PRODUCTVERSION	VERFILESTRING(DSSVER_MAJOR, DSSVER_MINOR, DSSVER_SUB, DSSVER_BUILD)

#define DSSVER_COPYRIGHT		"Copyright © Luc Coiffier 2006-2014"
#define DSSVER_PRODUCTNAME		"DeepSkyStacker"

#define DSSLIVEVER_PRODUCTNAME		"DeepSkyStacker Live"
#define DSSLIVEVER_FILEDESCRIPTION	"DeepSkyStacker Live"
#define DSSLIVEVER_INTERNALNAME		"DeepSkyStacker Live"
#define DSSLIVEVER_ORIGINALFILENAME	"DeepSkyStackerLive.exe"

#if defined(DSS_COMMANDLINE)
#define DSSVER_FILEDESCRIPTION	"DeepSkyStacker Command line"
#define DSSVER_INTERNALNAME		"DeepSkyStackerCL"
#define DSSVER_ORIGINALFILENAME	"DeepSkyStackerCL.exe"
#elif defined(DSS_PM32)
#define DSSVER_FILEDESCRIPTION	"DeepSkyStacker PixInsight Plug-In"
#define DSSVER_INTERNALNAME		"DeepSkyStacker32"
#define DSSVER_ORIGINALFILENAME	"DeepSkyStacker-pm32.dll"
#else
#define DSSVER_FILEDESCRIPTION	"DeepSkyStacker"
#define DSSVER_INTERNALNAME		"DeepSkyStacker"
#define DSSVER_ORIGINALFILENAME	"DeepSkyStacker.exe"
#endif

#endif // __DSSVERSION_H_

