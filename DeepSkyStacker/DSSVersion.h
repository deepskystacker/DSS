#ifndef __DSSVERSION_H_
#define __DSSVERSION_H_

//#define DSSBETA

#ifdef DSSBETA
#define DSSBETARELEASE 3
#define DSSBETAEXPIREYEAR 2019
#define DSSBETAEXPIREMONTH 9
#endif

#define DSSVER_MAJOR			4
#define DSSVER_MINOR			2
#define DSSVER_SUB				2

#ifdef DSSBETA
#define DSSVER_BUILD			DSSBETARELEASE
#else
#define DSSVER_BUILD			1
#endif

#define FB(arg)					#arg

#ifdef DSSBETA
#define VERSTRING(M, m, s, b)	FB(M)"."FB(m)"."FB(s)" Beta "FB(b)
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

constexpr auto DSSVER_COPYRIGHT = "Copyright © 2006-2019, Luc Coiffier; \
	Copyright © 2018-2019, David C. Partridge, Tony Cook, Mat Draper, Simon C.Smith, Vitali Pelenjow";
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

